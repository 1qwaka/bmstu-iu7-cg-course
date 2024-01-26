#include "renderworker.h"
#include "tracer.h"

#include <QPixmap>

RenderWorker::RenderWorker(QObject *parent)
    : QObject{parent}
{
    connect(&tracer, &Tracer::rendering_progress, this, &RenderWorker::accept_rendering_progress_from_tracer);
}


void RenderWorker::stop_rendering()
{
    tracer.should_stop = true;
}


void RenderWorker::accept_rendering_progress_from_tracer(int pixels_rendered)
{
    emit rendering_progress(pixels_rendered);
}


void RenderWorker::render_scene(std::reference_wrapper<const Scene> scene, int width, int height)
{
    finished = false;
    tracer.should_stop = false;
    QPixmap pix{width, height};
    tracer.render_pixmap(scene, pix);
    emit scene_rendered(pix);
    finished = true;
}
