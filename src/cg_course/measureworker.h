#ifndef MEASUREWORKER_H
#define MEASUREWORKER_H

#include "scene.h"
#include "tracer.h"
#include <time.h>
#include <iostream>
#include <sstream>

#include <QObject>

struct MeasureParams
{
    enum Type {
        SIZED,
        OBJECTS,
    } type;

    Scene *scene;

    union
    {
        struct {
            int start;
            int end;
            int step;
        } sized;

        struct {
            int size;
        } objects;
    };

};

class MeasureWorker : public QObject
{
    Q_OBJECT
public:
    Tracer tracer;
    QStringList results;

    explicit MeasureWorker(QObject *parent = nullptr);

    void stop()
    {
        tracer.should_stop = true;
    }

    long long get_time(long long start_point = 0)
    {
        struct timespec time;
        clock_gettime(CLOCK_MONOTONIC, &time);
        return time.tv_nsec + time.tv_sec * 1'000'000'000 - start_point;
    }


public slots:

    void run_measure(const MeasureParams& params)
    {
        tracer.should_stop = false;
        results.clear();
        QString type{};
        switch (params.type) {
        case MeasureParams::SIZED:
            type = "sized";
            sized_measure(params.scene, params.sized.start, params.sized.end, params.sized.step);
            break;
        case MeasureParams::OBJECTS:
            type = "objects";
            objects_measure(params.scene, params.objects.size);
            break;
        default:
            break;
        }

        if (tracer.should_stop)
            type = "unfinished";

        results.insert(0, type);
        emit measure_finished(results);
    };

    QString sized_measure(Scene *scene, int start, int stop, int step)
    {
        tracer.should_stop = false;
        std::stringstream data;

        int total_pixels = 0;
        int tested_pixels = 0;
        for (int size = start; size <= stop; size += step) total_pixels += size * size;


        for (int size = start; size <= stop; size += step)
        {
            QPixmap pix{size, size};
            auto begin = get_time();
            tracer.render_pixmap(*scene, pix);
            auto elapsed = get_time(begin);
            auto time = (elapsed / 1.0e6);
            data << size << " " << time << "\n";
            tested_pixels += size * size;
            int progress = (float(size - start) / float(stop - start) * 100);
//            int progress = 1.0 * tested_pixels / total_pixels * 100;
//            qDebug() << "tested_pixels =" << tested_pixels << "total_pixels =" << total_pixels << "emit" << progress;
            emit rendering_progress(progress);
        }

        results.append(QString{data.str().c_str()});
        return results.last();
    };

    QString objects_measure(Scene *scene, int size)
    {
        Scene scene_copy{*scene};
        QPixmap pix{size, size};
        std::stringstream data;

        int polys_and_spheres = std::count_if(scene_copy.objects.begin(),
                                              scene_copy.objects.end(),
                                              [](const Object* obj) {
                                                return obj->get_type() == ObjectType::POLY ||
                                                        obj->get_type() == ObjectType::SPHERE;
                                              });
        int processed_objects = 0;

        tracer.render_pixmap(scene_copy, pix);
        tracer.render_pixmap(scene_copy, pix);
        tracer.render_pixmap(scene_copy, pix);


        for (int i = 0; i < scene_copy.objects.size(); i++) {
            auto& obj = scene_copy.objects[i];

            if (obj->get_type() != ObjectType::POLY && obj->get_type() != ObjectType::SPHERE)
                continue;
//            qDebug() << "cycle " << obj->id;

            auto begin = get_time();
            tracer.render_pixmap(scene_copy, pix);
            auto elapsed = get_time(begin) / 1.0e6;
            data << (polys_and_spheres - processed_objects) << " " << elapsed << "\n";

            emit rendering_progress(1.0 * processed_objects / polys_and_spheres * 100);
            scene_copy.remove(obj->id, false);
            i--;
            processed_objects++;
//            qDebug() << "processed_objects" << processed_objects << "polys_and_spheres" << polys_and_spheres;
        }

        // for 0 removable objects on scene
        auto begin = get_time();
        tracer.render_pixmap(scene_copy, pix);
        auto elapsed = get_time(begin) / 1.0e6;
        data << (polys_and_spheres - processed_objects) << " " << elapsed << "\n";
//        qDebug() << "processed_objects" << processed_objects << "polys_and_spheres" << polys_and_spheres;
        emit rendering_progress(1.0 * processed_objects / polys_and_spheres * 100);

        results.append(QString{data.str().c_str()});
        results.append(QString::number(size));
        return results[results.size() - 2];
    };


signals:

    void rendering_progress(int);

    void measure_finished(const QStringList& measure_data);
};

#endif // MEASUREWORKER_H
