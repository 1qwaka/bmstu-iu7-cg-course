#ifndef INTERSECTVISITOR_H
#define INTERSECTVISITOR_H

#include "qmatrix4x4.h"
#include "visitor.h"
#include "sphere.h"
#include "polyobject.h"
#include "lens.h"
#include <QDebug>
#include <QVector3D>
#include "tri.h"
#include "vars.h"

thread_local static Sphere temp{};

//#define TEST_CULL 1

static inline bool intersect_tri(const QVector3D& o, const QVector3D& d, const Tri& tri, qreal& t)
{
    QVector3D edge1, edge2, tvec, pvec, qvec;
    qreal det, inv_det;

    edge1 = tri.p1 - tri.p0;
    edge2 = tri.p2 - tri.p0;

    pvec = QVector3D::crossProduct(d, edge2);

    det = QVector3D::dotProduct(edge1, pvec);

#ifdef TEST_CULL
    if (det < 1e-6) {
        return 0;
    }

    tvec = o - tri.p0;

    qreal u = QVector3D::dotProduct(tvec, pvec);

    if (u < 0 || u > det) {
        return 0;
    }

    qvec = QVector3D::crossProduct(tvec, edge1);

    qreal v = QVector3D::dotProduct(d, qvec);

    if (v < 0 || u + v > det) {
        return 0;
    }

    t = QVector3D::dotProduct(edge2, qvec);
    inv_det = 1 / det;
    t *= inv_det;
//    u *= inv_det;
//    v *= inv_det;
#else

    if (det > -1e-6 && det < 1e-6)
    {
//        qDebug() << "det";
        return 0;
    }
    inv_det = 1 / det;

    tvec = o - tri.p0;

    qreal u = QVector3D::dotProduct(tvec, pvec) * inv_det;
    if (u < 0 || u > 1.0)
    {
//        qDebug() << "u" << u;
        return 0;
    }

    qvec = QVector3D::crossProduct(tvec, edge1);

    qreal v = QVector3D::dotProduct(d, qvec) * inv_det;
    if (v < 0 || u + v > 1.0)
    {
//        qDebug() << "v" << v << "    " << u;
        return 0;
    }

    t = QVector3D::dotProduct(edge2, qvec) * inv_det;
#endif
    return 1;
}

class IntersectVisitor : public Visitor
{
public:
    // input
    qreal t_min = 0, t_max = 0;
    QVector3D beg, end;
    
    // output
    qreal t_intersect = 0;
    QVector3D norm, intersection_point;
    bool intersects = false;

    const char *msg = "ok";

    IntersectVisitor();

    void accept(Light *o) override
    {
        (void)o;
        intersects = false;
    }

    bool half_lens(const QVector3D& pos, const QVector3D& norm, qreal r, qreal d)
    {
        qreal val = r*r - d * d /2/2;
        if (val < 0) val = 0;

        QVector3D sphere_center = pos - norm * qSqrt(val);
        IntersectVisitor vi;
        temp.pos = sphere_center;
        temp.rad = r;
        vi.t_min = t_min;
        vi.t_max = t_max;
        vi.beg = beg;
        vi.end = end;
        vi.accept(&temp);

        if (!vi.intersects)
        {
            intersects = false;
            return false;
        }

        QVector3D h = vi.intersection_point - pos;
        qreal dot = QVector3D::dotProduct(h, norm);

//        if (dot > 0)
//            qDebug() << "pos =" << pos << "ip =" << vi.intersection_point << "norm =" << norm << "dot =" << dot;

        if (dot < 0)
        {
            intersects = false;
            return false;
        }

        t_intersect = vi.t_intersect;
        this->norm = vi.norm;
        intersection_point = vi.intersection_point;
        intersects = true;

        return true;
    }

    void accept(Lens *o) override
    {
        QMatrix4x4 lens_rot, lens_rot_x, lens_rot_y, lens_rot_z;
        lens_rot.setToIdentity();
        lens_rot_x.setToIdentity();
        lens_rot_y.setToIdentity();
        lens_rot_z.setToIdentity();
        lens_rot_z.rotate(o->rot.z(), 0, 0, 1);
        lens_rot_y.rotate(o->rot.y(), 0, 1, 0);
        lens_rot_x.rotate(o->rot.x(), 1, 0, 0);
        lens_rot = lens_rot_z * lens_rot_y * lens_rot_x;

        o->norm1 = lens_rot.mapVector(default_norm1);
        o->norm2 = lens_rot.mapVector(default_norm2);


        intersects = false;
        half_lens(o->pos, o->norm1, o->r1, o->diam);

        qreal first_half_t = t_intersect;
        QVector3D first_half_norm = norm;
        QVector3D first_half_ip = intersection_point;
        bool first_half_int = intersects;

        half_lens(o->pos, o->norm2, o->r2, o->diam);

        if (first_half_int && intersects)
        {
            if (first_half_t < t_intersect)
            {
                t_intersect = first_half_t;
                norm = first_half_norm;
                intersection_point = first_half_ip;
                intersects = true;
            }

        }
        else if (first_half_int && !intersects)
        {
            t_intersect = first_half_t;
            norm = first_half_norm;
            intersection_point = first_half_ip;
            intersects = true;
        }

    }

    void accept(PolyObject *o) override
    {
        intersects = false;
        QVector3D d = end - beg;
        qreal closest_t = Vars::far_distance;

        for (int i = 0; i < o->edges.size(); i++)
        {
            auto& tri = o->edges.at(i);
//            auto& norm = o->norms.at(i);
            qreal t;
            bool ints = intersect_tri(beg, d, tri, t);

//            qDebug() << "test tri" << i << ints;

            if (ints)
            {
                if (t < closest_t && t > 0)
                {
                    closest_t = t;
                    norm = o->norms.at(i);
                    intersects = true;
                }
            }
        }

        if (intersects)
        {
            intersection_point = beg + d * closest_t;
            t_intersect = closest_t;
        }
    }
    
    void accept(Sphere *o) override
    {
        const auto& d = this->end - this->beg;
        const auto& oc = this->beg - o->pos;
        const auto k1 = QVector3D::dotProduct(d, d);
        const auto k2 = 2 * QVector3D::dotProduct(oc, d);
        const auto k3 = QVector3D::dotProduct(oc, oc) - o->rad * o->rad;

        const auto disc = k2 * k2 - 4 * k1 * k3;
        if (std::abs(disc) < 1e-10) {
            this->intersects = false;
            msg = "no intersection";
            return;
        }

        const auto t1 = (-k2 + sqrt(disc)) / (2 * k1);
        const auto t2 = (-k2 - sqrt(disc)) / (2 * k1);

        this->t_intersect = std::min(t1, t2);
        if (this->t_min < this->t_intersect && this->t_intersect < this->t_max)
        {
            this->intersects = true;
        }
        else
        {
            this->t_intersect = std::max(t1, t2);
            if (this->t_min < this->t_intersect && this->t_intersect < this->t_max)
            {
                this->intersects = true;
            }
            else
            {
                this->intersects = false;
            }
        }

        if (this->intersects)
        {
            this->intersection_point = this->beg + (d * this->t_intersect);
            this->norm = intersection_point - o->pos;
//            qDebug() << "norm " << norm << " for " << intersection_point << " pos is" << o->pos;
        }
    }

};

#endif // INTERSECTVISITOR_H
