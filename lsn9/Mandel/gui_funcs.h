//
// Created by andrey on 12.11.2020.
//

#ifndef INC_3_RD_SEM_GUI_FUNCS_H
#define INC_3_RD_SEM_GUI_FUNCS_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "mand.h"

extern double ZoomX;
extern double ZoomY;

void onDisplay( void );

void onKeyboard( BYTE key, int x, int y );

void onResize( int newW, int newH );

void GlutWork( int argc, char *argv[] );

#endif //INC_3_RD_SEM_GUI_FUNCS_H
