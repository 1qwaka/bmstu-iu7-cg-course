#ifndef MATERIAL_H
#define MATERIAL_H

#include <QColor>
#include "QDebug"

struct Material
{
public:
    QColor color;
    qreal spec_factor = -1;
    qreal dif_factor = 1;
    qreal refact_factor = -1;

    Material(QColor color=qRgb(128, 128, 255), qreal spec=-1, qreal dif=1, qreal refract=-1)
    {
        // qDebug() << "initialize material color " << color << this;
        this->color = color;
        this->spec_factor = spec;
        this->dif_factor = dif;
        this->refact_factor = refract;
    }
};

#endif // MATERIAL_H
