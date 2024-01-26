    #ifndef TRACER_H
#define TRACER_H

#include "light.h"
#include "material.h"
#include "object.h"
#include "qdebug.h"
#include "qglobal.h"
#include "intersectvisitor.h"
#include<iostream>
#include <QVector3D>
#include "qmatrix4x4.h"
#include "qpainter.h"
#include "qpen.h"
#include "qpixmap.h"
#include "scene.h"
#include "vars.h"
#include <QList>
#include <QColor>
#include "colorop.h"


class Tracer : public QObject
{
    Q_OBJECT
public:
    std::atomic_bool should_stop = false;

    explicit Tracer(QObject *parent = nullptr);
    virtual ~Tracer() {};

    void render_pixmap(const Scene& scene, QPixmap& pix)
    {
        const auto& cam = scene.cam;

        QMatrix4x4 cam_rot, cam_rot_x, cam_rot_y, cam_rot_z;
        cam_rot_x.setToIdentity();
        cam_rot_x.rotate(cam.rot.x(), 1, 0, 0);
        cam_rot_y.setToIdentity();
        cam_rot_y.rotate(cam.rot.y(), 0, 1, 0);
        cam_rot_z.setToIdentity();
        cam_rot_z.rotate(cam.rot.z(), 0, 0, 1);
        cam_rot = cam_rot_z * cam_rot_y * cam_rot_x;



        QPainter paint{&pix};
        int height = pix.height();
        int width = pix.width();
        int rendered_pixels = 0;

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                if (should_stop)
                {
                    return;
                }

                QVector3D viewport_point = cam.get_viewport_point(x - width / 2, -y + height / 2, width, height);
                QVector3D direction = viewport_point - cam.pos;

                direction = cam_rot.mapVector(direction);



                QColor col = Tracer::trace_ray(scene.objects, scene.lights, cam.pos, cam.pos + direction, 1, Vars::far_distance);
                paint.setPen(QPen{QColor{col}});
                paint.drawPoint(x, y);

                ++rendered_pixels;
                if (rendered_pixels % 100 == 0)
                {
                    emit rendering_progress(rendered_pixels);
                }
            }
        }
    }

    QColor trace_ray(const QList<Object*>& objects, const QList<Light*>& lights, const QVector3D& beg, const QVector3D& end, double t_min, double t_max) {
        // auto [closest_sphere, closest_t, norm] = Tracer::closest_intersection_with_norm(objects, beg, end, t_min, t_max);
        auto [closest_object, intersection_point, norm] = Tracer::closest_intersection_with_norm(objects, beg, end, t_min, t_max);
//        qDebug() << closest_object << intersection_point << norm;

        if (closest_object == nullptr) {
            return Vars::bg_color;
        }

        QColor result_color{};
        const Material& m = closest_object->get_material();

//         qDebug() << "material" << &m << m.color << closest_object->get_type() << closest_object->id;

//        QVector3D norm = intersection_point - closest_sphere->pos;
//        QVector3D norm = closest_sphere->get_norm(intersection_point, closest_t);

//        qDebug() << "trace ray norm " << norm << intersection_point;
        norm.normalize();
//        qDebug() << "trace ray norm " << norm << intersection_point;

        // double light_intensity = this->compute_light_intensity(intersection_point, norm, line.direction() * -1, closest_sphere->specular, closest_sphere->diffuse);
        // result_color = closest_sphere->col * light_intensity;

        result_color = Tracer::compute_color_with_lights(objects, lights, intersection_point, norm, (end - beg) * -1, m.spec_factor, m.dif_factor, m.color);

        if (m.refact_factor > 0) {
            // std::cout << ">>\ncalc refact for " << *closest_sphere << " closest_t=" << closest_t << " intersection_point=" << intersection_point << std::endl;
            QVector3D refact_dir = Tracer::compute_refact_direction(end - beg, norm, m.refact_factor);
            refact_dir.normalize();
            auto refact_color = Tracer::trace_ray(objects, lights, intersection_point, intersection_point + refact_dir * Vars::small_distance, t_min, t_max);
//            if (refact_color != Vars::bg_color)
//                qDebug() << "refact dir =" << refact_dir << "intersection_point =" << intersection_point << "norm =" << norm << "dir =" << (end - beg) << "col =" << refact_color;
//            if (refact_dir.z() < 0)
//                qDebug() << "^^^^^^";

            // std::cout << "refact col " << refact_color << " for " << *closest_sphere << std::endl;
            result_color = colorSum(result_color, refact_color);
            // std::cout << "result_color=" << result_color << "\n<<" << std::endl;
        }

        // std::cout << "computed light_intensity=" << light_intensity << " computed_color=" << result_color << std::endl;

        // qDebug() << "color" << result_color.name();

        return result_color;
    }

    std::tuple<Object*, QVector3D, QVector3D>
    closest_intersection_with_norm(const QList<Object*>& objects, const QVector3D& beg, const QVector3D& end, qreal t_min, qreal t_max)
    {
        qreal closest_t = Vars::far_distance;
        Object* closest_obj = nullptr;
        QVector3D intersection_point, norm;

        IntersectVisitor v{};
        v.beg = beg;
        v.end = end;
        v.t_min = t_min;
        v.t_max = t_max;

//        qDebug() << "win\n\n\n";
        for (const auto& obj : objects) {
            if (should_stop)
            {
                break;
            }

//            qDebug() << "intersect" << obj;

            obj->accept(&v);

            if (!v.intersects) {
                continue;
            }

            if (v.t_intersect < closest_t) {
                closest_t = v.t_intersect;
                closest_obj = obj;
                intersection_point = v.intersection_point;
                norm = v.norm;
            }
        }

        return {closest_obj, intersection_point, norm};
    }

    QColor compute_color_with_lights(const QList<Object*>& objects, const QList<Light*>& lights, const QVector3D& point, const QVector3D& norm, const QVector3D& v, qreal s, qreal dif, QColor obj_color) {
        QColor result_color{};

        for (const auto& li : lights) {
            if (should_stop)
            {
                break;
            }

            switch (li->type)
            {
            case LightType::AMBIENT: {
                result_color = colorSum(result_color, colorMul(colorMul(obj_color, li->color), li->intensity));
                // qDebug() << "ambient" << result_color;
                // qDebug() << obj_color << li->color << colorMul(obj_color, li->color);
                break;
            }

            case LightType::DIRECTIONAL:
            case LightType::POINT: {
                QVector3D l;
                double t_max;
                if (li->type == LightType::POINT) {
                    l = li->position - point;
                    t_max = 1;
                } else {
                    l = li->direction;
                    t_max = Vars::far_distance;
                }

                if (Vars::enable_shadows) {
                    auto [shadow_object, shadow_intersection_point, shadow_norm] = Tracer::closest_intersection_with_norm(objects, point, point + l, 1e-3, t_max);
                    if (shadow_object != nullptr) {
                        continue;
                    }
                }

                double n_dot_l = QVector3D::dotProduct(norm, l);
//                qDebug() << "point n" << point << norm << n_dot_l;

                // std::cout << "point lignt n_dot_l=" << n_dot_l << " norm=" << norm << " point=" << point << " l=" << l << std::endl;
                if (n_dot_l > 0) {
                    // qDebug() << "was" << result_color;
                    result_color = colorSum(result_color, cmul(cmul(obj_color, li->color), ((li->intensity * n_dot_l / (norm.length() * l.length())) * dif)));
                    // qDebug() << "stal" << result_color;
                }

                // double prev_intensity = intensity;
                // std::cout << "POINT specualr from " << intensity << " ";
                if (s >= 0) {
                    QVector3D r = (norm * 2 * QVector3D::dotProduct(norm, l)) - l;
                    double r_dot_v = QVector3D::dotProduct(r, v);
                    if (r_dot_v > 0) {
                        result_color = csum(result_color, cmul(cmul(obj_color, li->color), (li->intensity * pow(r_dot_v / (r.length() * v.length()), s))));
                    }
                }
                // std::cout << " to  " << intensity << " (by " << (intensity - prev_intensity) << ")" << std::endl;

                break;
            }

            default:
                break;
            }
        }

        return result_color;
    }

    QVector3D compute_refact_direction(QVector3D direction, QVector3D norm, double refact_index) {
        direction.normalize();
        double eta = 1.0 / refact_index;
        double cos_theta = -1 * QVector3D::dotProduct(norm, direction);
        if (cos_theta < 0) {
            cos_theta *= -1;
            norm = norm * -1;
            eta = 1.0 / eta;
        }
        double k = 1.0 - eta * eta * (1.0 - cos_theta * cos_theta);
        if (k >= 0) {
            return direction * eta + norm * (eta * cos_theta - qSqrt(k));
        }
        return QVector3D{0, 0, 0};
    }

signals:
    void rendering_progress(int rendered_pixels);

};

#endif // TRACER_H
