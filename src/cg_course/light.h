#ifndef LIGHT_H
#define LIGHT_H

#include "object.h"
#include "qglobal.h"
#include <QVector3D>
#include <QColor>


enum LightType
{
    AMBIENT,
    POINT,
    DIRECTIONAL,
};


class Light : public Object
{
public:
    qreal intensity;
    QVector3D position;
    QVector3D direction;
    LightType type = LightType::POINT;
    QColor color = Qt::white;

    Light(qreal intensity = 0.1, QVector3D pos={}, QVector3D dir={}, LightType typ=LightType::AMBIENT, QColor col = qRgb(255, 255, 255))
    {
        this->intensity = intensity;
        this->position = pos;
        this->direction = dir;
        this->type = typ;
        this->color = col;
    }

    ObjectType get_type() const override;

    virtual void accept(Visitor *v) override;
};


#endif // LIGHT_H
