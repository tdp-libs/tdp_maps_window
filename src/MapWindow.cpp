#include "tdp_maps_window/MapWindow.h"

#include "tp_maps/MouseEvent.h"

#include "tp_utils/DebugUtils.h"

#include <QGLFramebufferObject>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTimer>

namespace tdp_maps_window
{
namespace
{
class Map_lt : public tp_maps::Map
{
public:
  //################################################################################################
  Map_lt(MapWindow* mapWidget_):
    tp_maps::Map(false)
  {
    mapWidget = mapWidget_;
  }

  //################################################################################################
  ~Map_lt()
  {
    preDelete();
  }

  // GL functions
  //################################################################################################
  virtual void makeCurrent()
  {
    mapWidget->makeCurrent();
  }

  //################################################################################################
  virtual void update()
  {
    mapWidget->update();
  }

private:
  MapWindow* mapWidget;
};
}

struct MapWindow::Private
{
  MapWindow* q;
  Map_lt* map;

  //################################################################################################
  Private(MapWindow* q_):
    q(q_)
  {
    map = new Map_lt(q);
  }

  //################################################################################################
  ~Private()
  {
    map->clearLayers();
    delete map;
  }

  //################################################################################################
  static QGLFormat glFormat()
  {
    QGLFormat glf = QGLFormat::defaultFormat();

#ifdef TDP_OSX
    glf.setProfile(QGLFormat::CompatibilityProfile);
    glf.setVersion(2, 1);
    glf.setSampleBuffers(true);
    glf.setSamples(4);
#else
    glf.setSampleBuffers(true);
    glf.setSamples(4);
#endif
    return glf;
  }

  //################################################################################################
  static tp_maps::Button convertMouseButton(Qt::MouseButton button)
  {
    switch(button)
    {
    case Qt::RightButton: return tp_maps::Button::RightButton;
    case Qt::LeftButton:  return tp_maps::Button::LeftButton;
    default:              return tp_maps::Button::NoButton;
    }
  }
};


//##################################################################################################
MapWindow::MapWindow(QWindow* parent):
  QOpenGLWindow(QOpenGLWindow::NoPartialUpdate, parent),
  d(new Private(this))
{

//  QSurfaceFormat format;
//  format.setMajorVersion(3);
//  format.setMinorVersion(2);
//  QSurfaceFormat::setDefaultFormat(format);
//  setFormat(format);

  setSurfaceType(QWindow::OpenGLSurface);
}

//##################################################################################################
MapWindow::~MapWindow()
{
  delete d;
}

//##################################################################################################
tp_maps::Map* MapWindow::map()
{
  return d->map;
}

//##################################################################################################
void MapWindow::initializeGL()
{
  d->map->initializeGL();

  //After a reparent paintGL does not get called and calling update in here does not seem to have
  //any effect, so this little kludge calls update once control returns to the event loop.
  {
    auto t = new QTimer(this);
    t->setSingleShot(true);
    connect(t, &QTimer::timeout, [&, t](){update();delete t;});
    t->start(0);
  }
}

//##################################################################################################
void MapWindow::resizeGL(int width, int height)
{
  d->map->resizeGL(width, height);
}

//##################################################################################################
void MapWindow::paintGL()
{
  d->map->paintGL();
}

//################################################################################################
void MapWindow::mousePressEvent(QMouseEvent* event)
{
  tp_maps::MouseEvent e(tp_maps::MouseEventType::Press);

  e.button = Private::convertMouseButton(event->button());
  e.pos.x = event->pos().x();
  e.pos.y = event->pos().y();

  if(d->map->mouseEvent(e))
    event->accept();
}

//################################################################################################
void MapWindow::mouseMoveEvent(QMouseEvent* event)
{
  tp_maps::MouseEvent e(tp_maps::MouseEventType::Move);

  e.button = Private::convertMouseButton(event->button());
  e.pos.x = event->pos().x();
  e.pos.y = event->pos().y();

  if(d->map->mouseEvent(e))
    event->accept();
}

//################################################################################################
void MapWindow::mouseReleaseEvent(QMouseEvent* event)
{
  tp_maps::MouseEvent e(tp_maps::MouseEventType::Release);

  e.button = Private::convertMouseButton(event->button());
  e.pos.x = event->pos().x();
  e.pos.y = event->pos().y();

  if(d->map->mouseEvent(e))
    event->accept();
}

//################################################################################################
void MapWindow::wheelEvent(QWheelEvent* event)
{
  tp_maps::MouseEvent e(tp_maps::MouseEventType::Wheel);

  e.pos.x = event->pos().x();
  e.pos.y = event->pos().y();

  e.delta = event->delta();

  if(d->map->mouseEvent(e))
    event->accept();
}

//################################################################################################
void MapWindow::mouseDoubleClickEvent(QMouseEvent* event)
{
  tp_maps::MouseEvent e(tp_maps::MouseEventType::DoubleClick);

  e.button = Private::convertMouseButton(event->button());
  e.pos.x = event->pos().x();
  e.pos.y = event->pos().y();

  if(d->map->mouseEvent(e))
    event->accept();
}
}
