
#define NEB_CORE_IMPL
#define NEB_RENDERER_IMPL
#define NEB_SUGAR_IMPL

#include <nebula/core.h>
#include <nebula/renderer.h>
#include <nebula/sugar.h>

#ifdef NEB_BACKEND_IS_GL
#define NEB_OGL3_IMPL
#include <nebula/backend/ogl3.h>
#endif