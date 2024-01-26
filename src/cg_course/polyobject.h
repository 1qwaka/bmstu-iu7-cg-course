#ifndef POLYOBJECT_H
#define POLYOBJECT_H

#include "object.h"
#include "tri.h"
#include <QList>
#include <QMatrix4x4>


static const QVector3D to_x{1, 0, 0}, to_y{0, 1, 0}, to_z{0, 0, 1};


class PolyObject : public Object
{
public:
    QList<Tri> edges;
    QList<QVector3D> norms;
    Material mat;
    QString shape_name = "Полигон";
    QVector3D conventional_position;

    PolyObject();
    PolyObject(const QList<Tri>& edges, const QList<QVector3D>& norms, Material mat): mat(mat)
    {
        this->edges = edges;
        this->norms = norms;
    }


    ObjectType get_type() const override;

    Material get_material() const override;

    virtual void accept(Visitor *v) override;

    template <typename TFun>
//    void each_vertex(std::function<void(QVector3D& vertex)> transformer)
    void each_vertex(TFun transformer)
    {
        for (auto& e : edges) {
            transformer(e.p0);
            transformer(e.p1);
            transformer(e.p2);
        }
    }

    static PolyObject* make_parallel(const QVector3D& pos, const QVector3D& rot, const QVector3D& len, const Material& mat)
    {
        auto height = len.y();
        auto width = len.x();
        auto depth = len.z();

        auto poly = new PolyObject(
                    {
                        // перед
                        {
                            pos + QVector3D{-width / 2, -height/2, -depth / 2},
                            pos + QVector3D{-width / 2, +height/2, -depth / 2},
                            pos + QVector3D{+width / 2, +height/2, -depth / 2},
                        },
                        {
                            pos + QVector3D{+width / 2, +height/2, -depth / 2},
                            pos + QVector3D{+width / 2, -height/2, -depth / 2},
                            pos + QVector3D{-width / 2, -height/2, -depth / 2},
                        },
                        // зад
                        {
                            pos + QVector3D{-width / 2, -height/2, +depth / 2},
                            pos + QVector3D{-width / 2, +height/2, +depth / 2},
                            pos + QVector3D{+width / 2, +height/2, +depth / 2},
                        },
                        {
                            pos + QVector3D{+width / 2, +height/2, +depth / 2},
                            pos + QVector3D{+width / 2, -height/2, +depth / 2},
                            pos + QVector3D{-width / 2, -height/2, +depth / 2},
                        },
                        // дно
                        {
                            pos + QVector3D{-width / 2, -height/2, -depth / 2},
                            pos + QVector3D{-width / 2, -height/2, +depth / 2},
                            pos + QVector3D{+width / 2, -height/2, +depth / 2},
                        },
                        {
                            pos + QVector3D{+width / 2, -height/2, +depth / 2},
                            pos + QVector3D{+width / 2, -height/2, -depth / 2},
                            pos + QVector3D{-width / 2, -height/2, -depth / 2},
                        },

                        // верх
                        {
                            pos + QVector3D{-width / 2, +height/2, -depth / 2},
                            pos + QVector3D{-width / 2, +height/2, +depth / 2},
                            pos + QVector3D{+width / 2, +height/2, +depth / 2},
                        },
                        {
                            pos + QVector3D{+width / 2, +height/2, +depth / 2},
                            pos + QVector3D{+width / 2, +height/2, -depth / 2},
                            pos + QVector3D{-width / 2, +height/2, -depth / 2},
                        },
                        // право
                        {
                            pos + QVector3D{+width / 2, +height/2, -depth / 2},
                            pos + QVector3D{+width / 2, -height/2, -depth / 2},
                            pos + QVector3D{+width / 2, -height/2, +depth / 2},
                        },
                        {
                            pos + QVector3D{+width / 2, -height/2, +depth / 2},
                            pos + QVector3D{+width / 2, +height/2, +depth / 2},
                            pos + QVector3D{+width / 2, +height/2, -depth / 2},
                        },
                        // лево
                        {
                            pos + QVector3D{-width / 2, +height/2, -depth / 2},
                            pos + QVector3D{-width / 2, -height/2, -depth / 2},
                            pos + QVector3D{-width / 2, -height/2, +depth / 2},
                        },
                        {
                            pos + QVector3D{-width / 2, -height/2, +depth / 2},
                            pos + QVector3D{-width / 2, +height/2, +depth / 2},
                            pos + QVector3D{-width / 2, +height/2, -depth / 2},
                        },
                    },
                    {
                        -to_z, -to_z,
                        to_z, to_z,
                        -to_y, -to_y,
                        to_y, to_y,
                        to_x, to_x,
                        -to_x, -to_x
                    },
                    mat);

        poly->shape_name = "Параллелепипед";
        poly->conventional_position = pos;
        poly->rotate_over(pos, rot);
        return poly;
    }



    static PolyObject* make_pyramid(const QVector3D& pos, const QVector3D& rot, qreal height, qreal aa, const Material& mat)
    {
        float a = aa;
        if (a < 1e-6) a = 1e-6;
        auto top = pos + QVector3D{0, float(height), 0};
//        auto right_norm = QVector3D::crossProduct(QVector3D{0, 0, a}, top - (pos + QVector3D{+a/2, 0, -a/2}));
//        auto left_norm = QVector3D::crossProduct(QVector3D{0, 0, -a}, top - (pos + QVector3D{-a/2, 0, +a/2}));
//        auto back_norm = QVector3D::crossProduct(QVector3D{-a, 0, 0}, top - (pos + QVector3D{+a/2, 0, +a/2}));
//        auto front_norm = QVector3D::crossProduct(QVector3D{a, 0, 0}, top - (pos + QVector3D{-a/2, 0, -a/2}));

        auto right_norm = QVector3D{1, float(a / 2 / height), 0};
        auto left_norm = QVector3D{-1, float(a / 2 / height), 0};
        auto back_norm = QVector3D{0, float(a / 2 / height), 1};
        auto front_norm = QVector3D{0, float(a / 2 / height), -1};


        auto poly = new PolyObject({
                                       { // bottom
                                           pos + QVector3D{-a/2, 0, -a/2},
                                           pos + QVector3D{+a/2, 0, -a/2},
                                           pos + QVector3D{+a/2, 0, +a/2},
                                       },
                                       {
                                           pos + QVector3D{-a/2, 0, -a/2},
                                           pos + QVector3D{-a/2, 0, +a/2},
                                           pos + QVector3D{+a/2, 0, +a/2},
                                       },

                                       { // front
                                           pos + QVector3D{-a/2, 0, -a/2},
                                           top,
                                           pos + QVector3D{+a/2, 0, -a/2},
                                       },

                                       { // left
                                           pos + QVector3D{-a/2, 0, +a/2},
                                           top,
                                           pos + QVector3D{-a/2, 0, -a/2},
                                       },

                                       { // back
                                           pos + QVector3D{-a/2, 0, +a/2},
                                           top,
                                           pos + QVector3D{+a/2, 0, +a/2},
                                       },

                                       { // right
                                           pos + QVector3D{+a/2, 0, +a/2},
                                           top,
                                           pos + QVector3D{+a/2, 0, -a/2},
                                       },

                                   },
                                   {
                                        -to_y, -to_y,
                                        front_norm, left_norm,
                                        back_norm, right_norm,
                                   },
                                   mat);

        poly->shape_name = "Пирамида";
        poly->conventional_position = pos;
        poly->rotate_over(pos, rot);
        return poly;
    }


    static PolyObject* make_prism(const QVector3D& pos, const QVector3D& rot, qreal height, qreal aa, const Material& mat)
    {
        float a = aa;
        float k = a * qSqrt(3) / 8;  // half height
        float h = height;

        auto poly = new PolyObject({
                                       { // top
                                           pos + QVector3D{+a/2, h, +k},
                                           pos + QVector3D{0, h, -k},
                                           pos + QVector3D{-a/2, h, +k},
                                       },
                                       { // bottom
                                           pos + QVector3D{+a/2, 0, +k},
                                           pos + QVector3D{0, 0, -k},
                                           pos + QVector3D{-a/2, 0, +k},
                                       },

                                       { // back
                                           pos + QVector3D{-a/2, 0, +k},
                                           pos + QVector3D{+a/2, 0, +k},
                                           pos + QVector3D{+a/2, h, +k},
                                       },
                                       {
                                           pos + QVector3D{+a/2, h, +k},
                                           pos + QVector3D{-a/2, h, +k},
                                           pos + QVector3D{-a/2, 0, +k},
                                       },

                                       { // right
                                           pos + QVector3D{0, 0, -k},
                                           pos + QVector3D{+a/2, 0, +k},
                                           pos + QVector3D{+a/2, h, +k},
                                       },
                                       {
                                           pos + QVector3D{+a/2, h, +k},
                                           pos + QVector3D{0, h, -k},
                                           pos + QVector3D{0, 0, -k},
                                       },

                                       { // left
                                           pos + QVector3D{0, 0, -k},
                                           pos + QVector3D{-a/2, 0, +k},
                                           pos + QVector3D{-a/2, h, +k},
                                       },
                                       {
                                           pos + QVector3D{-a/2, h, +k},
                                           pos + QVector3D{0, h, -k},
                                           pos + QVector3D{0, 0, -k},
                                       },

                                   },
                                   {
                                        to_y, -to_y,
                                       -to_z, -to_z,
                                       QVector3D{1, 0, -a / 2 / k}.normalized(), QVector3D{1, 0, -a / 2 / k}.normalized(),
                                       QVector3D{-1, 0, -a / 2 / k}.normalized(), QVector3D{-1, 0, -a / 2 / k}.normalized(),
                                   },
                                   mat);

        poly->shape_name = "Призма";
        poly->conventional_position = pos;
        poly->rotate_over(pos, rot);
        return poly;
    }

    void rotate_over(const QVector3D& pos, const QVector3D& rot)
    {
        QMatrix4x4 cam_rot, cam_rot_x, cam_rot_y, cam_rot_z;
        cam_rot_x.setToIdentity();
        cam_rot_x.rotate(rot.x(), 1, 0, 0);
        cam_rot_y.setToIdentity();
        cam_rot_y.rotate(rot.y(), 0, 1, 0);
        cam_rot_z.setToIdentity();
        cam_rot_z.rotate(rot.z(), 0, 0, 1);
        cam_rot = cam_rot_z * cam_rot_y * cam_rot_x;

        each_vertex([&](QVector3D& vert){
            vert = cam_rot.mapVector(vert - pos) + pos;
        });
        for (auto& norm : norms) {
            norm = cam_rot.mapVector(norm);
        }
    }

};

#endif // POLYOBJECT_H
