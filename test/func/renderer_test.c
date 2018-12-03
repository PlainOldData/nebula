#define NEB_RENDERER_IMPL
#include <nebula/renderer.h>

#define NEB_GLFW_IMPL
#include "nebula_glfw.h"

#define NEB_OGL3_IMPL
#include "nebula_ogl.h"

struct nbgl_ctx rdr_ctx;
struct nb_glfw_ctx glfw_ctx;
struct nbr_ctx nbr_ctx;

int
main() {

        nb_glfw_setup(&glfw_ctx, &nbr_ctx);

        while(nb_glfw_tick(&glfw_ctx, &nbr_ctx)) {
                glClearColor(0.157, 0.153, 0.161, 1);
                glClear(GL_COLOR_BUFFER_BIT);
        };

        return 0;
}
