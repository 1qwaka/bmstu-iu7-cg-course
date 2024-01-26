#include "light.h"



ObjectType Light::get_type() const
{
    return ObjectType::LIGHT;
}

void Light::accept(Visitor *v)
{
    v->accept(this);
}
