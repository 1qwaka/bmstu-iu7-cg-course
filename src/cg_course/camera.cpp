#include "camera.h"

Camera::Camera()
{

}


ObjectType Camera::get_type() const
{
    return ObjectType::CAMERA;
}

void Camera::accept(Visitor *v)
{
    v->accept(this);
}

