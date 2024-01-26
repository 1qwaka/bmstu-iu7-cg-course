#include "visitor.h"

#include "object.h"
#include "camera.h"
#include "light.h"
#include "lens.h"
#include "polyobject.h"
#include "qdebug.h"
#include "sphere.h"

#define DEFINE_ACCEPT(TYPE) void Visitor::accept(TYPE *obj) { \
                    qDebug() << "CALL NOT IMPELEMENTED ACCEPT: obj=" \
                    << obj << " obj.id=" << obj->id << " obj.type=" \
                    << obj->get_type() << " visitor=" << this; };

DEFINE_ACCEPT(Object);
DEFINE_ACCEPT(Camera);
DEFINE_ACCEPT(Light);
DEFINE_ACCEPT(PolyObject);
DEFINE_ACCEPT(Sphere);
DEFINE_ACCEPT(Lens);
