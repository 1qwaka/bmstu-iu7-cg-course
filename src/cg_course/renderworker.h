#ifndef RENDERWORKER_H
#define RENDERWORKER_H

#include "scene.h"
#include "tracer.h"
#include <QObject>
#include <QPixmap>

class RenderWorker : public QObject
{
    Q_OBJECT
public:
    Tracer tracer;
    std::atomic_bool finished = true;

    explicit RenderWorker(QObject *parent = nullptr);
    void stop_rendering();

public slots:
    void render_scene(std::reference_wrapper<const Scene> scene, int width, int height);

    void accept_rendering_progress_from_tracer(int pixels_rendered);

signals:
    void scene_rendered(QPixmap pix);

    void rendering_progress(int pixels_rendered);
};

#endif // RENDERWORKER_H
