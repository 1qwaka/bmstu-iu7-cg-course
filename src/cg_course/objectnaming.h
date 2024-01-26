#ifndef OBJECTNAMING_H
#define OBJECTNAMING_H

#include "object.h"
#include "polyobject.h"
#include <QString>


struct ObjectNaming
{
public:

    static QString obj_type_name_ru(Object* o) {
        switch (o->get_type()) {
        case ObjectType::OBJ:
            return "Объект";
        case ObjectType::LIGHT:
            return "Свет";
        case ObjectType::LENS:
            return "Линза";
        case ObjectType::POLY:
            return dynamic_cast<PolyObject*>(o)->shape_name;
        case ObjectType::SPHERE:
            return "Сфера";
        case ObjectType::CAMERA:
            return "Камера";
        }
        return "UNKNOWN";
    }

};

#endif // OBJECTNAMING_H
