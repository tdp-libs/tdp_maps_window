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
  ~MapWindow() override;

  //################################################################################################
  tp_maps::Map* map();

protected:
  //################################################################################################
  void initializeGL() override;

  //################################################################################################
  void resizeGL(int width, int height) override;

  //################################################################################################
  void paintGL() override;

  //################################################################################################
  void mousePressEvent(QMouseEvent* event) override;

  //################################################################################################
  void mouseMoveEvent(QMouseEvent* event) override;

  //################################################################################################
  void mouseReleaseEvent(QMouseEvent* event) override;

  //################################################################################################
  void wheelEvent(QWheelEvent* event) override;

  //################################################################################################
  void mouseDoubleClickEvent(QMouseEvent* event) override;

  //################################################################################################
  void touchEvent(QTouchEvent* event) override;

private:
  struct Private;
  Private* d;
  friend struct Private;
};
}
#endif

