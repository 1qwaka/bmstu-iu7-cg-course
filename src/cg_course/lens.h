#ifndef LENS_H
#define LENS_H

#include "object.h"

static constexpr QVector3D default_norm1{0, 0, 1};
static constexpr QVector3D default_norm2{0, 0, -1};

class Lens : public Object
{
public:
    QVector3D pos;
    QVector3D rot;
    qreal diam;
    qreal r1, r2;
    QVector3D norm1{default_norm1}, norm2{default_norm2};
    Material mat = Material{QColor{0, 30, 50}, 125, 0.1, 1.5};


    Lens();
    Lens(QVector3D pos, QVector3D rot, qreal diam, qreal R1, qreal R2, QVector3D norm1=default_norm1, QVector3D norm2=default_norm2)
        : pos(pos), rot(rot), diam(diam), r1(R1), r2(R2), norm1(norm1), norm2(norm2)
    {

    }


    ObjectType get_type() const override;
    Material get_material() const override;



    virtual void accept(Visitor *v) override;
};

#endif // LENS_H
