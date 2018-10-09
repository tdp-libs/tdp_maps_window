#ifndef tdp_maps_window_global_h
#define tdp_maps_window_global_h

#include "tp_utils/Globals.h"

#if defined(TDP_MAPS_WINDOW_LIBRARY)
#  define TDP_MAPS_WINDOW_SHARED_EXPORT
#else
#  define TDP_MAPS_WINDOW_SHARED_EXPORT
#endif

#include "tp_maps/textures/BasicTexture.h"

class QImage;

//##################################################################################################
//! A simple 3D engine for window based applications
namespace tdp_maps_window
{

//##################################################################################################
tp_maps::TextureData convertTexture(const QImage& image);

}

#endif
