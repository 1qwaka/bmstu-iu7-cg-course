#include "polyobject.h"

PolyObject::PolyObject()
{

}


ObjectType PolyObject::get_type() const
{
    return ObjectType::POLY;
}

void PolyObject::accept(Visitor *v)
{
    v->accept(this);
}

Material PolyObject::get_material() const
{
    return mat;
}

