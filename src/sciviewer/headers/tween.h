#ifndef __TWEEN_H_
#define __TWEEN_H_
#include <stdlib.h>
extern float zoomLevel;
extern float dZoomLevel;
extern float verticalOffset;
extern float dVerticalOffset;
extern float horizontalOffset;
extern float dHorizontalOffset;
void tweenIn();
void tweenOut();
void tweenPanUp();
void tweenPanDown();
void tweenPanLeft();
void tweenPanRight();
#endif
