#include "lens.h"

Lens::Lens()
{

}


ObjectType Lens::get_type() const
{
    return ObjectType::LENS;
}

void Lens::accept(Visitor *v)
{
    v->accept(this);
}


Material Lens::get_material() const
{
    return this->mat;
}

