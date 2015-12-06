#include "headers/tween.h"

void tweenIn(){
    while (zoomLevel < dZoomLevel){
        zoomLevel += (.001) * zoomLevel/10;
        sleep(1);
    }	
}

void tweenOut(){
    while (zoomLevel > dZoomLevel){
        zoomLevel -= (.001) * zoomLevel/10;
        sleep(1);
    }	
}

void tweenPanUp(){
    while(verticalOffset > dVerticalOffset){
        verticalOffset -= (.001 / zoomLevel);
        sleep(3);
    }
}

void tweenPanDown(){
    while(verticalOffset < dVerticalOffset){
        verticalOffset += (.001 / zoomLevel);
        sleep(3);
    }
}

void tweenPanLeft(){
    while(horizontalOffset < dHorizontalOffset){
        horizontalOffset += (.001 / zoomLevel);
        sleep(3);
    }
}

void tweenPanRight(){
    while(horizontalOffset > dHorizontalOffset){
        horizontalOffset -= (.001 / zoomLevel);
        sleep(3);
    }
}


