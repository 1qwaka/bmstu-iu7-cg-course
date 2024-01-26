#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "measureworker.h"
#include "objectadduicontroller.h"
#include "objectdeleteuicontroller.h"
#include "qgraphicsscene.h"
#include "renderworker.h"
#include "state.h"
#include <QMainWindow>
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void update_pixmap();

    void setup_ui_callbacks();

    void stop_draw_thread();

    void showEvent(QShowEvent* event);
    void resizeEvent(QResizeEvent* event);

private slots:
    void on_drawb_clicked();

    void draw_rendered_image(QPixmap pix);

    void on_stop_drawb_clicked();

    void set_rendering_prgressbar(int pixels_rendered);

    void set_measuring_prgressbar(int percents);

    void on_clear_scene_clicked();

    void on_obj_type_textActivated(const QString &arg1);

    void try_autodraw(bool enable = true);

    void on_measure_run_sized_clicked();

    void accept_measurements(const QStringList& measure_data);

    void run_measurements(const MeasureParams& params);

    void on_measure_stop_clicked();

    void on_measure_run_objects_clicked();

signals:
    void start_render_image(std::reference_wrapper<const Scene>, int width, int height);


private:
    Ui::MainWindow *ui;

    QGraphicsScene *scene = nullptr;
    QGraphicsPixmapItem *pix_item = nullptr;
    QPixmap pix{1, 1};

    bool copy_pix_to_clipboard = false;
    bool extended_mode = false;
    bool autodraw = false;

    State state;
    QList<State> back_states;

    RenderWorker *draw_worker = nullptr;
    QThread *draw_thread = nullptr;  // Q треды говно
    std::atomic_bool render_stopped = false;

    MeasureWorker *measure_worker = nullptr;
    QThread *measure_thread = nullptr;
    std::atomic_bool measure_stopped = false;
    QPixmap measure_pix{1, 1};

    ObjectAddUiController obj_add;
    ObjectDeleteUiController obj_del;
};
#endif // MAINWINDOW_H
