#include "object.h"
#include "qdebug.h"

Object::Object()
{
    this->id = Object::next_id++;
//     qDebug() << "create object" << id << this->get_type();
}


std::tuple<int, double, double> Object::intersect_line(const QVector3D& beg, const QVector3D& end)
{
    (void)beg;(void)end;
    return {0, 0, 0};
}


std::tuple<int, double, double> Object::intersect_line_with_norm(const QVector3D& beg, const QVector3D& end, QVector3D& possible_norm) const
{
    (void)beg;(void)end;(void)possible_norm;
    return {0, 0, 0};
}


ObjectType Object::get_type() const
{
    return ObjectType::OBJ;
}


Material Object::get_material() const
{
    return {};
}


void Object::accept(Visitor *v)
{
    v->accept(this);
}

    

int Object::next_id = 0;
