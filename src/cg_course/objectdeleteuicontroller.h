#ifndef OBJECTDELETEUICONTROLLER_H
#define OBJECTDELETEUICONTROLLER_H


#include "qapplication.h"
#include "qcheckbox.h"
#include "qheaderview.h"
#include "qpushbutton.h"
#include "qtablewidget.h"
#include "scene.h"
#include "objectnaming.h"
#include "colorop.h"

#include <QVBoxLayout>
class ObjectDeleteUiController : public QObject
{
    Q_OBJECT

public:
    Scene *scene{};
    QTableWidget* table;

    explicit ObjectDeleteUiController(QWidget *parent = nullptr);
    virtual ~ObjectDeleteUiController() {};  //

    void init(Scene* scene, QTableWidget* table)
    {
        this->scene = scene;
        this->table = table;

        QStringList headers = {"id", "тип", "позиция", "цвет", "удалить"};
        int cols = 5;
        for (int i = 0; i < cols; i++) table->insertColumn(i);
        table->setColumnWidth(0, 3);
        table->setColumnWidth(2, 120);
        //        table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

        table->setHorizontalHeaderLabels(headers);
        table->verticalHeader()->setVisible(false);

        // https://stackoverflow.com/questions/38554640/add-border-under-column-headers-in-qtablewidget
        table->horizontalHeader()->setStyleSheet("QHeaderView::section{"
                                                           "border-top:0px solid #D8D8D8;"
                                                           "border-left:0px solid #D8D8D8;"
                                                           "border-right:1px solid #D8D8D8;"
                                                           "border-bottom: 1px solid #D8D8D8;"
                                                           "background-color:white;"
                                                           "padding:4px;"
                                                       "}"
                                                       "QTableCornerButton::section{"
                                                           "border-top:0px solid #D8D8D8;"
                                                           "border-left:0px solid #D8D8D8;"
                                                           "border-right:1px solid #D8D8D8;"
                                                           "border-bottom: 1px solid #D8D8D8;"
                                                           "background-color:white;"
                                                       "}");

        table->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    }

    void fill_table()
    {
        table->clearContents();
        table->setRowCount(0);

        const auto& red_x_icon = QApplication::style()->standardIcon(QStyle::SP_MessageBoxCritical);
        int rows = 0;
        for (auto& obj : scene->objects) {
            if (!can_delete(obj)) {
                continue;
            }
            //        | id | type | color | hide | delete
            rows++;
            table->setRowCount(rows);

            auto color_item = new QTableWidgetItem(obj->get_material().color.name());
            color_item->setBackground(obj->get_material().color);
            color_item->setForeground(colorInv(obj->get_material().color));
            color_item->setToolTip(obj->get_material().color.name());

            auto pos = get_position(obj);

            int id = obj->id;
            auto del_btn = new QPushButton(red_x_icon, "");
            connect(del_btn, &QPushButton::clicked, [id, this]() {
                this->remove_by_id(id);
            });


            table->setItem(rows - 1, 0, new QTableWidgetItem(QString::number(obj->id)));
            table->setItem(rows - 1, 1, new QTableWidgetItem(ObjectNaming::obj_type_name_ru(obj)));
            table->setItem(rows - 1, 2, new QTableWidgetItem("x: " + QString::number(pos.x()) +
                                                             ",  y: " + QString::number(pos.y()) +
                                                             ",  z: " + QString::number(pos.z())));
            table->setItem(rows - 1, 3, color_item);
            table->setCellWidget(rows - 1, 4, del_btn);

            //            table->setCellWidget(rows - 1, 3, new QCheckBox());
        }

    }

    void remove_by_id(int id)
    {
        scene->remove(id);
        for (int i = 0; i < table->rowCount(); i++)
        {
            bool ok;
            if (table->item(i, 0) != nullptr && table->item(i, 0)->text().toInt(&ok) == id && ok)
            {
                table->removeRow(i);
                break;
            }
        }

        emit object_removed();
    }

    QVector3D get_position(Object* o)
    {
        switch (o->get_type()) {
        case ObjectType::POLY:
            return dynamic_cast<PolyObject*>(o)->conventional_position;
        case ObjectType::SPHERE:
            return dynamic_cast<Sphere*>(o)->pos;
        default:
            break;
        }
        return QVector3D{-99, -99, -99};
    }

    bool can_delete(Object *o) {
        return o->get_type() == ObjectType::POLY || o->get_type() == ObjectType::SPHERE;
    }

signals:
    void object_removed();
};

#endif // OBJECTDELETEUICONTROLLER_H
