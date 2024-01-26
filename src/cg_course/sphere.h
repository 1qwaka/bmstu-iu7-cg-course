#ifndef SPHERE_H
#define SPHERE_H

#include "object.h"

class Sphere : public Object
{
public:
    QVector3D pos;
    qreal rad = 10;
    Material mat;

    Sphere();

    Sphere(QVector3D pos, qreal rad, Material mat): pos(pos), rad(rad), mat(mat)
    {
//        this->pos = pos;
//        this->rad = rad;
//        this->mat = mat;
        // qDebug() << "in sphere construtor" << this->mat.color;
    }

    ObjectType get_type() const override;
    Material get_material() const override;


    virtual std::tuple<int, double, double> intersect_line_with_norm(const QVector3D& beg, const QVector3D& end, QVector3D& possible_norm) const override;
    
    virtual void accept(Visitor *v) override;
};

#endif // SPHERE_H
