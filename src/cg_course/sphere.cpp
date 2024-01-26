#include "sphere.h"

Sphere::Sphere()
{

}

ObjectType Sphere::get_type() const
{
    return ObjectType::SPHERE;
}

Material Sphere::get_material() const
{
    return mat;
}

std::tuple<int, double, double> Sphere::intersect_line_with_norm(const QVector3D& beg, const QVector3D& end, QVector3D& possible_norm) const
{
    (void)beg;(void)end;(void)possible_norm;
    return {0, 0, 0};
}

void Sphere::accept(Visitor *v)
{
    v->accept(this);
}

