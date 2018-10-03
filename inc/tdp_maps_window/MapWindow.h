#ifndef tdp_maps_window_MapWindow_h
#define tdp_maps_window_MapWindow_h

#include <QOpenGLWindow>

#include "tdp_maps_window/Globals.h"

#include "tp_maps/Map.h"


namespace tdp_maps_window
{
class TDP_MAPS_WINDOW_SHARED_EXPORT MapWindow : public QOpenGLWindow
{
  Q_OBJECT

public:
  //################################################################################################
  MapWindow(QWindow* parent = nullptr);

  //################################################################################################
  ~MapWindow();

  //################################################################################################
  tp_maps::Map* map();

protected:
  //################################################################################################
  virtual void initializeGL();

  //################################################################################################
  virtual void resizeGL(int width, int height);

  //################################################################################################
  virtual void paintGL();

  //################################################################################################
  virtual void mousePressEvent(QMouseEvent* event);

  //################################################################################################
  virtual void mouseMoveEvent(QMouseEvent* event);

  //################################################################################################
  virtual void mouseReleaseEvent(QMouseEvent* event);

  //################################################################################################
  virtual void wheelEvent(QWheelEvent* event);

  //################################################################################################
  virtual void mouseDoubleClickEvent(QMouseEvent* event);

private:
  struct Private;
  Private* d;
  friend struct Private;
};
}
#endif

