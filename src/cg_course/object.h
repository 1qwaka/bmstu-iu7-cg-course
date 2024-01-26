#ifndef OBJECT_H
#define OBJECT_H

#include "material.h"
#include "visitor.h"
#include <QVector3D>
#include <tuple>
#include <iostream>


enum ObjectType {
    OBJ,
    LIGHT,
    LENS,
    POLY,
    SPHERE,
    CAMERA,
};

static inline const char * obj_type_name(ObjectType t) {
    switch (t) {
    case ObjectType::OBJ:
        return "OBJ";
    case ObjectType::LIGHT:
        return "LIGHT";
    case ObjectType::LENS:
        return "LENS";
    case ObjectType::POLY:
        return "POLY";
    case ObjectType::SPHERE:
        return "SPHERE";
    case ObjectType::CAMERA:
        return "CAMERA";
    }
    return "UNKNOWN";
}

class Object
{
public:
    int id;

    Object();

    virtual ~Object() = default;

    virtual std::tuple<int, double, double> intersect_line(const QVector3D& beg, const QVector3D& end);

    virtual std::tuple<int, double, double> intersect_line_with_norm(const QVector3D& beg, const QVector3D& end, QVector3D& possible_norm) const;

    virtual ObjectType get_type() const;

    virtual Material get_material() const;

    virtual void accept(Visitor *v);
    
    static int next_id;
};

#endif // OBJECT_H
