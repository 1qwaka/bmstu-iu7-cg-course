#include "mainwindow.h"
#include "objectadduicontroller.h"
#include "predefinedscenes.h"
#include "renderworker.h"
#include "tracer.h"
#include "ui_mainwindow.h"
#include <QGraphicsPixmapItem>
#include <QClipboard>
#include <QFile>
#include "uiutils.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QLocale::setDefault(QLocale::C);
    ui->setupUi(this);

    // setup scene

    this->scene = new QGraphicsScene(this);
    ui->view->setScene(this->scene);
    ui->view->setStyleSheet("border: 0px");
    ui->view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->scene->setBackgroundBrush(QBrush{QColor{0xf0, 0xf0, 0xf0}});

    // setup pixmap
    this->pix = this->pix.scaled(100, 100);
    this->pix.fill(Qt::white);
    this->pix_item = this->scene->addPixmap(this->pix);
    ui->view->fitInView(pix_item, Qt::KeepAspectRatio);

    PredefinedScenes::simple(this->state.scene);

    setup_ui_callbacks();

    system("cd");
//    system("gnuplot ..\\plot.sh");
}

MainWindow::~MainWindow()
{
    stop_draw_thread();
    if (measure_thread != nullptr)
    {
        measure_worker->stop();
        measure_thread->wait();
        measure_thread->deleteLater();
        measure_thread = nullptr;
    }

    delete ui;
}

void MainWindow::stop_draw_thread()
{
    if (draw_thread != nullptr)
    {
        draw_worker->stop_rendering();
        draw_thread->wait();
        draw_thread->deleteLater();
        draw_thread = nullptr;
    }
}

void MainWindow::try_autodraw(bool enable)
{
    if (autodraw && enable) update_pixmap();
}

void MainWindow::update_pixmap()
{
    stop_draw_thread();

    if (draw_worker == nullptr)
    {
        draw_worker = new RenderWorker();
        connect(draw_worker, &RenderWorker::rendering_progress, this, &MainWindow::set_rendering_prgressbar, Qt::QueuedConnection);
        connect(draw_worker, &RenderWorker::scene_rendered, this, &MainWindow::draw_rendered_image, Qt::QueuedConnection);
    }

    ui->progress->setMaximum(pix.width() * pix.height());
    ui->progress->setMinimum(0);

    draw_thread = QThread::create([this](){
        try{
            draw_worker->render_scene(state.scene, pix.width(), pix.height());
        } catch (const std::exception& e) {
            qDebug() << "exception " << e.what();
        }
    });
    draw_thread->start();
}


void MainWindow::setup_ui_callbacks()
{
    // ---------------------- SOME MACROS ----------------------

    #define SPINBOX_TYPE_LAMBDA(TYPE, NAME, LAMBDA) connect(ui->NAME, &TYPE::valueChanged, LAMBDA);

    #define DSPINBOX_LAMBDA(NAME, LAMBDA) ui->NAME->setRange(-1e10, 1e10); \
                                            SPINBOX_TYPE_LAMBDA(QDoubleSpinBox, NAME, LAMBDA);
    #define SPINBOX_LAMBDA(NAME, LAMBDA) ui->NAME->setRange(int(-1e6), int(1e6)); \
                                            SPINBOX_TYPE_LAMBDA(QSpinBox, NAME, LAMBDA);

//    METHOD(ui->NAME->value());
    #define DSPINBOX_CALL(NAME, METHOD)  \
                                        DSPINBOX_LAMBDA(NAME, [this](double value) { \
                                            METHOD(value); \
                                            try_autodraw(); \
                                        });
//    VALUE = ui->NAME->value();
    #define DSPINBOX_VALUE(NAME, VALUE)  \
                                        DSPINBOX_LAMBDA(NAME, [this](double value) { \
                                            VALUE = value; \
                                            try_autodraw(); \
                                        });

    // ---------------------- SOME MACROS END ------------------


    // ---------------------- MAIN TAB ----------------------
    // IMAGE
//    ui->img_width->setMinimum(1);
    SPINBOX_LAMBDA(img_width, [this](int val){
        pix = pix.scaled(val, pix.height());
        try_autodraw();
    });
    ui->img_width->setRange(1, 20'000);
    ui->img_width->setSingleStep(100);
    SPINBOX_LAMBDA(img_height, [this](int val){
        pix = pix.scaled(pix.width(), val);
        try_autodraw();
    });
    ui->img_height->setRange(1, 20'000);
    ui->img_height->setSingleStep(100);

    connect(ui->copy_to_clipboard, &QCheckBox::stateChanged, [this](int a){
        copy_pix_to_clipboard = a != Qt::Unchecked;
    });

    connect(ui->shadows, &QCheckBox::stateChanged, [this](int a){
        Vars::enable_shadows = a != Qt::Unchecked;
        try_autodraw();
    });

    connect(ui->autodraw, &QCheckBox::stateChanged, [this](int a){
        autodraw = a != Qt::Unchecked;
        try_autodraw();
    });

    UiUtils::color_button_setup_click(this, ui->bg_col, &Vars::bg_color, [this]() { try_autodraw(); });


    // CAMERA
    DSPINBOX_CALL(cam_x, state.scene.cam.pos.setX);
    DSPINBOX_CALL(cam_y, state.scene.cam.pos.setY);
    DSPINBOX_CALL(cam_z, state.scene.cam.pos.setZ);
    DSPINBOX_CALL(cam_rx, state.scene.cam.rot.setX);
    DSPINBOX_CALL(cam_ry, state.scene.cam.rot.setY);
    DSPINBOX_CALL(cam_rz, state.scene.cam.rot.setZ);


    // LIGHT
    DSPINBOX_CALL(light_x,  state.scene.lights[0]->position.setX);
    DSPINBOX_CALL(light_y,  state.scene.lights[0]->position.setY);
    DSPINBOX_CALL(light_z,  state.scene.lights[0]->position.setZ);
    DSPINBOX_VALUE(light_intensity,  state.scene.lights[0]->intensity);
    ui->light_intensity->setRange(0, 1);
    UiUtils::color_button_setup_click(this, ui->light_col, &state.scene.lights[0]->color, [this]() { try_autodraw(); });


    // LENS
    DSPINBOX_CALL(lens_x,  state.scene.lenses[0]->pos.setX);
    DSPINBOX_CALL(lens_y,  state.scene.lenses[0]->pos.setY);
    DSPINBOX_CALL(lens_z,  state.scene.lenses[0]->pos.setZ);
    DSPINBOX_CALL(lens_rx,  state.scene.lenses[0]->rot.setX);
    DSPINBOX_CALL(lens_ry,  state.scene.lenses[0]->rot.setY);
    DSPINBOX_CALL(lens_rz,  state.scene.lenses[0]->rot.setZ);
    DSPINBOX_VALUE(lens_diam,  state.scene.lenses[0]->diam);
    DSPINBOX_VALUE(lens_r1,  state.scene.lenses[0]->r1);
    DSPINBOX_VALUE(lens_r2,  state.scene.lenses[0]->r2);
    DSPINBOX_VALUE(lens_refract,  state.scene.lenses[0]->mat.refact_factor);
    ui->lens_refract->setMinimum(0);
    ui->lens_diam->setMinimum(0);
    connect(ui->lens_r1, &QDoubleSpinBox::valueChanged, [this](double v) {
        double diam = ui->lens_diam->value();
        if (v < diam / 2) {
            ui->lens_r1->setValue(diam / 2);
        }
    });
    connect(ui->lens_r2, &QDoubleSpinBox::valueChanged, [this](double v) {
        double diam = ui->lens_diam->value();
        if (v < diam / 2) {
            ui->lens_r2->setValue(diam / 2);
        }
    });
    connect(ui->lens_diam, &QDoubleSpinBox::valueChanged, [this](double v) {
        if (ui->lens_r1->value() < v / 2) {
            ui->lens_r1->setValue(v / 2);
        }
        if (ui->lens_r2->value() < v / 2) {
            ui->lens_r2->setValue(v / 2);
        }
    });


    // 3 invisible line edits make right panel bigger and expanding
    ui->lineEdit->setVisible(false);
    ui->lineEdit_2->setVisible(false);
    ui->lineEdit_3->setVisible(false);

    // ---------------------- MAIN TAB END ------------------



    // ---------------------- OBJECT ADD TAB ----------------------
    obj_add.init(&state.scene, ui);
    obj_add.setup_combobox(ui->obj_type);
    // Важно: подключается после подключения слота в obj_add.init,
    // потому что именно тот слот добавляет объект, соответственно если
    // построить табицу раньше, новосозданного объекта не будет, потому что он
    // еще не *новосозданный*
    connect(ui->obj_add, &QPushButton::clicked, [this]() {
        obj_del.fill_table();
        try_autodraw();
    });
    // ---------------------- OBJECT ADD TAB END ------------------


    // ---------------------- OBJECT DELETE TAB ----------------------
    obj_del.init(&state.scene, ui->obj_table);
    obj_del.fill_table();
    connect(&obj_del, &ObjectDeleteUiController::object_removed, [this]() {
        try_autodraw();
    });
    // ---------------------- OBJECT DELETE TAB END ------------------


    // ---------------------- SCENE TAB ----------------------
    connect(ui->scene_simple, &QPushButton::clicked, [this]() {
        state.scene.clear_all();
        PredefinedScenes::simple(state.scene);
        try_autodraw();
        obj_del.fill_table();
    });
    connect(ui->scene_cubes, &QPushButton::clicked, [this]() {
        state.scene.clear_all();
        PredefinedScenes::cubes(state.scene);
        try_autodraw();
        obj_del.fill_table();
    });
    connect(ui->scene_sphere, &QPushButton::clicked, [this]() {
        state.scene.clear_all();
        PredefinedScenes::spheres(state.scene);
        try_autodraw();
        obj_del.fill_table();
    });
    connect(ui->scene_various, &QPushButton::clicked, [this]() {
        state.scene.clear_all();
        PredefinedScenes::various(state.scene);
        try_autodraw();
        obj_del.fill_table();
    });

    // ---------------------- SCENE TAB END ------------------



    // ---------------------- HIDING SOME ELEMENTS ----------------------
    if (!extended_mode) {
        ui->shadows->setVisible(false);

        ui->lab_refr->setVisible(false);
        ui->obj_refr->setVisible(false);
    }
    // ---------------------- HIDING SOME ELEMENTS END ------------------


    #undef LINE_EDIT_DOUBLE_LAMBDA
    #undef LINE_EDIT_DOUBLE_CALL
    #undef LINE_EDIT_DOUBLE_VALUE
}


void MainWindow::on_drawb_clicked()
{
    render_stopped = false;
    this->update_pixmap();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);
   ui->view->fitInView(pix_item, Qt::KeepAspectRatio);
}

void MainWindow::showEvent(QShowEvent* event)
{
    QWidget::showEvent( event );
    ui->view->fitInView(pix_item, Qt::KeepAspectRatio);
}

void MainWindow::draw_rendered_image(QPixmap pix)
{
    qDebug() << "draw rendered image" << pix;
    pix_item->setPixmap(pix);
    ui->view->fitInView(pix_item, Qt::KeepAspectRatio);

    if (copy_pix_to_clipboard) {
        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setPixmap(pix);
    }
}

void MainWindow::on_stop_drawb_clicked()
{
    stop_draw_thread();
    render_stopped = true;
    ui->progress->reset();
}


void MainWindow::set_rendering_prgressbar(int pixels_rendered)
{
    if (render_stopped) {
        ui->progress->reset();
    } else {
        ui->progress->setValue(pixels_rendered);
    }
}

void MainWindow::set_measuring_prgressbar(int percents)
{
//    qDebug() << "set_measuring_prgressbar" << percents << measure_stopped;
    if (measure_stopped) {
        ui->measure_progress->reset();
    } else {
        ui->measure_progress->setValue(percents);
//        qDebug() << ui->measure_progress->value();
    }
}


void MainWindow::on_clear_scene_clicked()
{
    ui->obj_last->setText("");
    state.scene.clear_basic_objects();
    obj_del.fill_table();
}


void MainWindow::on_obj_type_textActivated(const QString &arg1)
{
    obj_add.handle_combobox_change(ui->obj_type, arg1);
}

// measure_data
// [0] = type
// [1] = data to write in data file
void MainWindow::accept_measurements(const QStringList& measure_data)
{
    qDebug() << "accept_measurements" << measure_data;
    if (measure_data[0] == "unfinished") {
        return;
    }

    QFile datafile{"..\\data.txt"};
    datafile.open(QIODeviceBase::WriteOnly | QIODeviceBase::Text);
    datafile.write(measure_data[1].toUtf8());
    datafile.close();

    if (measure_data[0] == "sized") {
        system("gnuplot ..\\sized_plot.sh");
    } else if (measure_data[0] == "objects") {
        system("gnuplot ..\\objects_plot.sh");
    }
//    QThread::msleep(100);

    measure_pix = QPixmap{QString{"plot.svg"}};
    pix_item->setPixmap(measure_pix);
    ui->view->fitInView(pix_item, Qt::KeepAspectRatio);
}


void MainWindow::run_measurements(const MeasureParams& params)
{
    if (measure_thread != nullptr)
    {
        measure_worker->stop();
        measure_thread->wait();
        measure_thread->deleteLater();
        measure_thread = nullptr;
    }

    if (measure_worker == nullptr)
    {
        measure_worker = new MeasureWorker();
        connect(measure_worker, &MeasureWorker::rendering_progress, this, &MainWindow::set_measuring_prgressbar, Qt::QueuedConnection);
        connect(measure_worker, &MeasureWorker::measure_finished, this, &MainWindow::accept_measurements, Qt::QueuedConnection);
    }

    ui->measure_progress->setMaximum(100);
    ui->measure_progress->setMinimum(0);

    measure_thread = QThread::create([this, params](){
        try{
            measure_worker->run_measure(params);
        } catch (const std::exception& e) {
            qDebug() << "exception " << e.what();
        }
    });
    measure_thread->start();
}



void MainWindow::on_measure_run_sized_clicked()
{
    measure_stopped = false;
    MeasureParams params;
    params.type = MeasureParams::SIZED;
    params.scene = &state.scene;
    params.sized = {
        .start = (int)ui->measure_start->value(),
        .end = (int)ui->measure_end->value(),
        .step = (int)ui->measure_step->value(),
    };

    run_measurements(params);
}


void MainWindow::on_measure_stop_clicked()
{
    if (measure_thread != nullptr)
    {
        measure_worker->stop();
        measure_thread->wait();
        measure_thread->deleteLater();
        measure_thread = nullptr;
    }
    measure_stopped = true;
    ui->measure_progress->reset();
}


void MainWindow::on_measure_run_objects_clicked()
{
    measure_stopped = false;
    MeasureParams params;
    params.type = MeasureParams::OBJECTS;
    params.scene = &state.scene;
    params.objects = {
        .size = (int)ui->measure_size->value(),
    };

    run_measurements(params);
}

