
#define NEBULA_IMPL
#include <nebula/nebula.h>
#include <nebula/nebula_sugar.h>

#if 0

#ifdef NEBULA_OGL3
#define NEB_OGL3_IMPL
#include <nebula_ogl.h>
#endif

#endif

#ifdef NEBULA_GLFW
#define NEB_GLFW_IMPL
#include <nebula_glfw.h>
#endif
