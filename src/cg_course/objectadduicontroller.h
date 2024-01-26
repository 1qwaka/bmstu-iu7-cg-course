#ifndef OBJECTADDUICONTROLLER_H
#define OBJECTADDUICONTROLLER_H


#include "object.h"
#include "polyobject.h"
#include "qcombobox.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "scene.h"
#include "sphere.h"
#include "ui_mainwindow.h"
#include "awesomemacros.h"
#include "objectnaming.h"
#include "uiutils.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class ObjectAddUiController : public QObject
{
    Q_OBJECT

    #define CREATION_ARGS const QVector3D& pos, \
                        const QVector3D& rot, \
                        const QVector3D& arg, \
                        const Material& mat
    using CreationFunction = Object*(CREATION_ARGS);

    struct ObjectTypeDescriptor {
        QString name;
        QString arg_labels[4];
        std::function<CreationFunction> create;
    };

public:
    QList<ObjectTypeDescriptor> possible_types;

    QLabel *lab_cent{};
    QLabel *lab1{}, *lab2{}, *lab3{};
    QDoubleSpinBox *arg1{}, *arg2{}, *arg3{};

    QDoubleSpinBox *x{}, *y{}, *z{};
    QDoubleSpinBox *rx{}, *ry{}, *rz{};
    QDoubleSpinBox *dif{}, *refr{}, *spec{};  // material
    QPushButton *col{};
    QColor material_color{128, 128, 255};

    QLabel *last{};
    QPushButton *add{};
    QComboBox *type{};

    Scene *scene;

    ObjectAddUiController(QWidget *parent = nullptr) : QObject(parent)
    {
        possible_types.append(ObjectTypeDescriptor{
                                  "Сфера",
                                  {"Радиус", "", "", "Координаты центра:"},
                                  [](CREATION_ARGS) -> Object* {
                                      (void)rot;
                                      return new Sphere(pos, arg.x(), mat);
                                  }
                              });
        possible_types.append(ObjectTypeDescriptor{
                                  "Параллелепипед",
                                  {"Длина по Х", "Длина по Y", "Длина по Z", "Координаты центра:"},
                                  [](CREATION_ARGS) -> Object* {
                                      return PolyObject::make_parallel(pos, rot, arg, mat);
                                  }
                              });
        possible_types.append(ObjectTypeDescriptor{
                                  "Призма трехгранная",
                                  {"Высота", "Длина стороны основания", "", "Координаты центра основания:"},
                                  [](CREATION_ARGS) -> Object* {
                                      return PolyObject::make_prism(pos, rot, arg.x(), arg.y(), mat);
                                  }
                              });
        possible_types.append(ObjectTypeDescriptor{
                                  "Пирамида четырехгранная",
                                  {"Высота", "Длина стороны основания", "", "Координаты центра основания:"},
                                  [](CREATION_ARGS) -> Object* {
                                      return PolyObject::make_pyramid(pos, rot, arg.x(), arg.y(), mat);
                                  }
                              });
    }

    void init(Scene *scene, Ui::MainWindow* ui)
    {
        #define ASSIGN(name) this->name = ui->obj_##name;

        FOR_EACH(ASSIGN, arg1, arg2, arg3, lab1, lab2, lab3, lab_cent, x, y, z, rx, ry, rz, col, dif, refr, spec, last, add, type);
        this->scene = scene;

        QMap<QDoubleSpinBox*, std::pair<double, double>> ranges = {
            {dif, {0, 1}},
            {spec, {-1, 1e10}},
            {refr, {-1, 1e10}},
            {x, {-1e10, 1e10}},
            {y, {-1e10, 1e10}},
            {z, {-1e10, 1e10}},
            {rx, {-1e10, 1e10}},
            {ry, {-1e10, 1e10}},
            {rz, {-1e10, 1e10}},
            {arg1, {0, 1e10}},
            {arg2, {0, 1e10}},
            {arg3, {0, 1e10}},
        };
        for (auto& [box, range] : ranges.toStdMap()) {
            box->setRange(range.first, range.second);
        }

        QMap<QDoubleSpinBox*, double> steps= {
            {dif, 0.05},
            {spec, 10},
            {refr, 0.05},
            {x, 1},
            {y, 1},
            {z, 1},
            {rx, 5},
            {ry, 5},
            {rz, 5},
            {arg1, 1},
            {arg2, 1},
            {arg3, 1},
        };
        for (auto& [box, step] : steps.toStdMap()) {
            box->setSingleStep(step);
        }

        QMap<QDoubleSpinBox*, double> values = {
            {dif, 1},
            {spec, -1},
            {refr, -1},
            {x, 0},
            {y, 0},
            {z, 1},
            {rx, 0},
            {ry, 0},
            {rz, 0},
            {arg1, 1},
            {arg2, 1},
            {arg3, 1},
        };
        for (auto& [box, val] : values.toStdMap()) {
            box->setValue(val);
        }

        spec->setToolTip("Значения менее 0 отключают зеркальное отражение");
        refr->setToolTip("Значения менее 0 отключают преломление");
        last->setText("");

        connect(add, &QPushButton::clicked, [this](){
            Object *obj = create(type->currentText());
            this->scene->add(obj);
            last->setText("Последний добавленный объект: " + ObjectNaming::obj_type_name_ru(obj) + " " + QString::number(obj->id));
        });

        UiUtils::color_button_setup_click(this, col, &material_color);

        #undef ASSIGN
    }


    void setup_combobox(QComboBox* box)
    {
        box->clear();
        for (const auto& t : possible_types) {
            box->addItem(t.name);
        }
        box->setCurrentIndex(0);
        this->handle_combobox_change(box, box->currentText());
    }


    ObjectTypeDescriptor* get_desc_by_name(const QString& name)
    {
        for (auto& t : possible_types) {
            if (t.name == name) {
                return &t;
            }
        }
        return nullptr;
    }


    void handle_combobox_change(QComboBox* box, const QString& new_text)
    {
        (void)box;

        auto* tp = get_desc_by_name(new_text);
        if (tp == nullptr) {
            qDebug() << "cannot found type descriptor for" << new_text;
            lab1->setText("ERROR!");
            return;
        }

        auto& t = *tp;

        QList<QLabel*> labels = { lab1, lab2, lab3, lab_cent };
        QList<QDoubleSpinBox*> args = { arg1, arg2, arg3, nullptr };

        for (int i = 0; i < labels.size(); i++) {
            if (!t.arg_labels[i].isEmpty()) {
                labels[i]->setText(t.arg_labels[i]);
                if (args[i]) args[i]->setDisabled(false);
            } else {
                labels[i]->setText("");
                if (args[i]) args[i]->setDisabled(true);
            }
        }
    }

    Object* create(const QString& name)
    {
        #define F(edit) (float)(edit->value())
        #define D(edit) (qreal)(edit->value())

        auto* tp = get_desc_by_name(name);
        if (tp == nullptr) {
            qDebug() << "cannot found type descriptor for" << name;
            lab1->setText("ERROR!");
            return nullptr;
        }

        auto& t = *tp;

        return t.create(QVector3D{ F(x), F(y), F(z) },
                        QVector3D{ F(rx), F(ry), F(rz) },
                        QVector3D{ F(arg1), F(arg2), F(arg3) },
                        Material{material_color, D(spec), D(dif), D(refr)});
        #undef F
        #undef D
    }
#undef CREATION_ARGS
};

#endif // OBJECTADDUICONTROLLER_H
