#include "tdp_maps_window/MapWindow.h"

#include "tp_maps/MouseEvent.h"

#include "tp_utils/DebugUtils.h"

#include <QGLFramebufferObject>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTimer>
#include <QCoreApplication>

namespace tdp_maps_window
{
namespace
{
class Map_lt : public tp_maps::Map
{
public:
  //################################################################################################
  Map_lt(MapWindow* mapWidget_):
    tp_maps::Map(true)
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
  void makeCurrent() override
  {
    mapWidget->makeCurrent();
  }

  //################################################################################################
  void update() override
  {
    mapWidget->update();
  }

  //################################################################################################
  float pixelScale() override
  {
    return float(mapWidget->devicePixelRatioF());
  }

private:
  MapWindow* mapWidget;
};

//##################################################################################################
float pinchDelta(const QTouchEvent::TouchPoint& p0, const QTouchEvent::TouchPoint& p1)
{
  return glm::distance(glm::vec2{p0.pos().x(), p0.pos().y()}, glm::vec2{p1.pos().x(), p1.pos().y()});
}

}

//##################################################################################################
struct MapWindow::Private
{
  MapWindow* q;
  Map_lt* map;

  float pinchDelta{0.0f};

  bool pinchActive{false};
  bool mouseActive{false};

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
  QSurfaceFormat format;

#ifdef TDP_IOS
  format.setMajorVersion(3);
  format.setMinorVersion(2);
#elif defined TDP_OSX
  format.setProfile(QSurfaceFormat::CompatibilityProfile);
  format.setMajorVersion(2);
  format.setMinorVersion(1);
#else

#endif

  format.setSamples(4);

  QSurfaceFormat::setDefaultFormat(format);
  setFormat(format);

  setSurfaceType(QWindow::OpenGLSurface);

  QCoreApplication::setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents, false);
  QCoreApplication::setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, false);
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

//##################################################################################################
void MapWindow::mousePressEvent(QMouseEvent* event)
{
  tp_maps::MouseEvent e(tp_maps::MouseEventType::Press);

  e.button = Private::convertMouseButton(event->button());
  e.pos.x = event->pos().x();
  e.pos.y = event->pos().y();

  if(d->map->mouseEvent(e))
    event->accept();
}

//##################################################################################################
void MapWindow::mouseMoveEvent(QMouseEvent* event)
{
  tp_maps::MouseEvent e(tp_maps::MouseEventType::Move);

  e.button = Private::convertMouseButton(event->button());
  e.pos.x = event->pos().x();
  e.pos.y = event->pos().y();

  if(d->map->mouseEvent(e))
    event->accept();
}

//##################################################################################################
void MapWindow::mouseReleaseEvent(QMouseEvent* event)
{
  tp_maps::MouseEvent e(tp_maps::MouseEventType::Release);

  e.button = Private::convertMouseButton(event->button());
  e.pos.x = event->pos().x();
  e.pos.y = event->pos().y();

  if(d->map->mouseEvent(e))
    event->accept();
}

//##################################################################################################
void MapWindow::wheelEvent(QWheelEvent* event)
{
  tp_maps::MouseEvent e(tp_maps::MouseEventType::Wheel);

  e.pos.x = event->pos().x();
  e.pos.y = event->pos().y();

  e.delta = event->delta();

  if(d->map->mouseEvent(e))
    event->accept();
}

//##################################################################################################
void MapWindow::mouseDoubleClickEvent(QMouseEvent* event)
{
  tp_maps::MouseEvent e(tp_maps::MouseEventType::DoubleClick);

  e.button = Private::convertMouseButton(event->button());
  e.pos.x = event->pos().x();
  e.pos.y = event->pos().y();

  if(d->map->mouseEvent(e))
    event->accept();
}

//##################################################################################################
void MapWindow::touchEvent(QTouchEvent* event)
{
  QList<QTouchEvent::TouchPoint> touchPoints = event->touchPoints();

  switch (event->type())
  {
  case QEvent::TouchBegin: //-----------------------------------------------------------------------
  {
    if(touchPoints.size() == 1)
    {
      d->mouseActive = true;

      const QTouchEvent::TouchPoint& p0 = touchPoints.at(0);
      tp_maps::MouseEvent e(tp_maps::MouseEventType::Press);

      e.button = tp_maps::Button::LeftButton;
      e.pos.x = int(p0.pos().x());
      e.pos.y = int(p0.pos().y());

      d->map->mouseEvent(e);
    }
    else
    {

    }

    break;
  }

  case QEvent::TouchUpdate: //----------------------------------------------------------------------
  {
    if(touchPoints.size() == 2)
    {
      const QTouchEvent::TouchPoint& p0 = touchPoints.at(0);
      const QTouchEvent::TouchPoint& p1 = touchPoints.at(1);

      if(d->pinchActive)
      {
        float delta = pinchDelta(p0, p1);
        auto dd = delta - d->pinchDelta;
        if(std::fabs(dd)>16.0f)
        {
          d->pinchDelta = delta;

          tp_maps::MouseEvent e(tp_maps::MouseEventType::Wheel);
          e.pos.x = int((p0.pos().x()+p1.pos().x())/2.0);
          e.pos.y = int((p0.pos().y()+p1.pos().y())/2.0);
          e.delta = int(dd/8.0f);
          d->map->mouseEvent(e);
        }
      }
      else
      {
        d->pinchDelta = pinchDelta(p0, p1);
        d->pinchActive = true;
      }

      if(d->mouseActive)
      {
        d->mouseActive = false;

        tp_maps::MouseEvent e(tp_maps::MouseEventType::Release);

        e.button = tp_maps::Button::LeftButton;
        e.pos.x = int(p0.pos().x());
        e.pos.y = int(p0.pos().y());

        d->map->mouseEvent(e);
      }
    }

    if(d->mouseActive && !touchPoints.isEmpty())
    {
      const QTouchEvent::TouchPoint& p0 = touchPoints.at(0);
      tp_maps::MouseEvent e(tp_maps::MouseEventType::Move);

      e.button = tp_maps::Button::LeftButton;
      e.pos.x = int(p0.pos().x());
      e.pos.y = int(p0.pos().y());

      d->map->mouseEvent(e);
    }

    break;
  }

  case QEvent::TouchEnd: //-------------------------------------------------------------------------
  {
    if(d->mouseActive && !touchPoints.isEmpty())
    {
      const QTouchEvent::TouchPoint& p0 = touchPoints.at(0);

      d->mouseActive = false;

      tp_maps::MouseEvent e(tp_maps::MouseEventType::Release);

      e.button = tp_maps::Button::LeftButton;
      e.pos.x = int(p0.pos().x());
      e.pos.y = int(p0.pos().y());

      d->map->mouseEvent(e);
    }

    d->pinchActive = false;
    d->mouseActive = false;
    break;
  }

  default:
    break;
  }

  event->accept();
}
}
