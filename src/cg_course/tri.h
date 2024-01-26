#ifndef TRI_H
#define TRI_H
#include <QVector3D>
struct Tri{
    QVector3D p0, p1, p2;

    QVector3D& operator[](int i)
    {
        if (i == 0)
            return p0;
        if (i == 1)
            return p1;
        return p2;
    }

    const QVector3D& operator[](int i) const
    {
        if (i == 0)
            return p0;
        if (i == 1)
            return p1;
        return p2;
    }
};

#endif // TRI_H
