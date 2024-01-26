#ifndef CAMERA_H
#define CAMERA_H

#include "object.h"

class Camera : public Object
{
public:
    qreal d = 1, vw = 1, vh = 1;
    QVector3D pos{};
    QVector3D rot{};  // угол поворота вокруг осей соответственно

    Camera();

    QVector3D get_viewport_point(int x, int y, int img_width, int img_height) const
    {
        return {
            (float)(this->pos.x() + 1.0 * x * this->vw / img_width),
            (float)(this->pos.y() + 1.0 * y * this->vh / img_height),
            (float)(this->pos.z() + this->d)
        };
    }

    ObjectType get_type() const override;

    virtual void accept(Visitor *v) override;
};

#endif // CAMERA_H
