#pragma once

#define GLES
//#define OPENGL

#ifdef GLES
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <egl/egl.h>
#include <egl/eglext.h>
#endif // GLES


#ifdef OPENGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")
#endif // opengl