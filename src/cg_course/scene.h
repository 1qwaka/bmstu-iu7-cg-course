#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "lens.h"
#include "light.h"
#include "object.h"
#include "polyobject.h"
#include "qlist.h"
#include "sphere.h"

class Scene : public Object
{
public:
    QList<Object*> objects;
    QList<Lens*> lenses;
    QList<Light*> lights;
    QList<Sphere*> spheres;
    QList<PolyObject*> polys;
    Camera cam;

    void add(Lens* o)
    {
        objects.append(o);
        lenses.append(o);
    }

    void add(Light* o)
    {
        objects.append(o);
        lights.append(o);
    }

    void add(Sphere* o)
    {
        objects.append(o);
        spheres.append(o);
    }

    void add(PolyObject* o)
    {
        objects.append(o);
        polys.append(o);
    }

    void add(Object *o)
    {
        if (o == nullptr) {
            qDebug() << "trying to add null object";
            return;
        }

        qDebug() << "add object type is" << obj_type_name(o->get_type()) << o->id;
        objects.append(o);

        switch (o->get_type()) {
        case ObjectType::LIGHT:
            lights.append(dynamic_cast<Light*>(o));
            break;
        case ObjectType::LENS:
            lenses.append(dynamic_cast<Lens*>(o));
            break;
        case ObjectType::POLY:
            polys.append(dynamic_cast<PolyObject*>(o));
            break;
        case ObjectType::SPHERE:
            spheres.append(dynamic_cast<Sphere*>(o));
            break;
        default:
            qDebug() << "added object strange type" << obj_type_name(o->get_type()) << o->id;
            break;
        }
    }

    void clear_basic_objects()
    {
        for (int i = 0; i < objects.size(); i++) {
            auto *obj = objects[i];
            if (obj->get_type() == ObjectType::POLY || obj->get_type() == ObjectType::SPHERE)
            {
                delete obj;
                objects.removeAt(i);
                i--;
            }
        }
    }

    void clear_all()
    {
        lenses.clear();
        lights.clear();
        spheres.clear();
        polys.clear();
        for (auto& obj : objects) {
            delete obj;
        }
        objects.clear();
    }


    void remove(int id, bool should_delete = true)
    {
        auto pred = [id](Object *o) {
            return o->id == id;
        };
        auto it = std::find_if(objects.begin(), objects.end(), pred);
        if (it == objects.end())
            return;

        Object *obj = *it;

        objects.removeIf(pred);
        lenses.removeIf(pred);
        lights.removeIf(pred);
        spheres.removeIf(pred);
        polys.removeIf(pred);

        if (should_delete) {
            delete obj;
        }
    }

    Scene();
};

#endif // SCENE_H
