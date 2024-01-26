#ifndef VISITOR_H
#define VISITOR_H

class Object;
class Camera;
class Light;
class PolyObject;
class Sphere;
class Lens;

#define DEFINE_ACCEPT(TYPE) virtual void accept(TYPE *obj)

class Visitor
{
public:
    DEFINE_ACCEPT(Object);
    DEFINE_ACCEPT(Camera);
    DEFINE_ACCEPT(Light);
    DEFINE_ACCEPT(PolyObject);
    DEFINE_ACCEPT(Sphere);
    DEFINE_ACCEPT(Lens);
};

#undef DEFINE_ACCEPT

#endif // VISITOR_H
