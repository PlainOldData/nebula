#ifndef NEB_GLFW_INCLUDED
#define NEB_GLFW_INCLUDED


#ifdef __cplusplus
extern "C" {
#endif


#include <GL/glcorearb.h>
#include <GL/gl3w.h>
#define NEB_OGL3_IMPL
#include "nebula_ogl.h"

#include <GLFW/glfw3.h>


#define NB_GLFW_CHAR_COUNT_MAX 4096

struct nb_glfw_ctx {
        GLFWwindow *window;

        int width, height;
        int display_width, display_height;
        int ms_pos_x, ms_pos_y;
        int ms_left_button_down;

        char chars[NB_GLFW_CHAR_COUNT_MAX];
        int char_count;

        struct nbgl_ctx gl_ctx;

        double frame_start;
        float dt;
};


int
nb_glfw_setup(struct nb_glfw_ctx *ctx, struct nb_ctx *nb_ctx);


int
nb_glfw_shutdown(struct nb_glfw_ctx *ctx);


int
nb_glfw_tick(struct nb_glfw_ctx *ctx, struct nb_ctx *nb_ctx);


#ifdef __cplusplus
}
#endif


/* inc guard */
#endif

/* -- IMPL -- */

#ifdef NEB_GLFW_IMPL
#ifndef NEB_GLFW_IMPL_INCLUDED
#define NEB_GLFW_IMPL_INCLUDED


#include <GL/gl3w.h>
#include <nebula/nebula.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


/* -------------------------------------------------------------- Internal -- */


void
nb_glfw_resize_cb(
        GLFWwindow* window,
        int width,
        int height)
{
        struct nb_glfw_ctx * user_data = glfwGetWindowUserPointer(window);

        user_data->width = width;
        user_data->height = height;

        glfwGetFramebufferSize(user_data->window, &user_data->display_width, &user_data->display_height);
}


void
nb_glfw_mouse_pos_cb(
        GLFWwindow *window,
        double x_pos,
        double y_pos)
{
        struct nb_glfw_ctx * user_data = glfwGetWindowUserPointer(window);

        user_data->ms_pos_x = (int)x_pos;
        user_data->ms_pos_y = (int)y_pos;
}


void
nb_glfw_mouse_button_cb(
        GLFWwindow *window,
        int mouse_button,
        int mouse_action,
        int mods)
{
        (void)mods;

        if (mouse_button != GLFW_MOUSE_BUTTON_1) {
                return;
        }

        struct nb_glfw_ctx * user_data = glfwGetWindowUserPointer(window);
        user_data->ms_left_button_down = GLFW_RELEASE == mouse_action ? 0 : 1;
}

void nb_glfw_push_char(struct nb_glfw_ctx * ctx, char c) {
        if(ctx->char_count < (NB_GLFW_CHAR_COUNT_MAX - 1)) {
                ctx->chars[ctx->char_count++] = c;
                ctx->chars[ctx->char_count] = 0;
        }
        else {
                assert(!"nb_glfw_ctx.chars array is full!");
        }
}

void
nb_glfw_key_cb(GLFWwindow * window, int key, int scan_code, int action, int mods) {
        (void)scan_code;
        (void)mods;

        struct nb_glfw_ctx * ctx = glfwGetWindowUserPointer(window);
        assert(ctx);

        if(key == GLFW_KEY_BACKSPACE) {
                if(action == GLFW_PRESS || action == GLFW_REPEAT) {
                        if(ctx->char_count) {
                                ctx->chars[--ctx->char_count] = 0;
                        }
                        else {
                                nb_glfw_push_char(ctx, '\b');
                        }
                }
        }
        else if(key == GLFW_KEY_ENTER) {
                if(action == GLFW_PRESS || action == GLFW_REPEAT) {
                        nb_glfw_push_char(ctx, '\n');
                }
        }
}

void
nb_glfw_char_cb(GLFWwindow * window, unsigned int code_point) {
        /* Only support ASCII right now */
        if(code_point > 31 && code_point < 127) {
                struct nb_glfw_ctx * ctx = glfwGetWindowUserPointer(window);
                assert(ctx);

                char c = (char)code_point;
                nb_glfw_push_char(ctx, c);
        }
}


/* ----------------------------------------------------- NB GLFW Interface -- */


int
nb_glfw_setup(struct nb_glfw_ctx *ctx, struct nb_ctx *nb_ctx)
{
        memset(ctx, 0, sizeof(*ctx));

        /* setp GLFW and GL3w */
        glfwInit();

        glfwWindowHint(GLFW_SAMPLES, 8);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

        #ifdef __APPLE__
        glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint (GLFW_COCOA_RETINA_FRAMEBUFFER, 0);
        #endif

        ctx->width = 640;
        ctx->height = 480;

        ctx->window = glfwCreateWindow(
                ctx->width,
                ctx->height,
                "Nebula GLFW Demo",
                0,
                0);

        glfwMakeContextCurrent(ctx->window);
        glfwSetWindowUserPointer(ctx->window, ctx);
        glfwSetWindowSizeCallback(ctx->window, nb_glfw_resize_cb);
        glfwSetCursorPosCallback(ctx->window, nb_glfw_mouse_pos_cb);
        glfwSetMouseButtonCallback(ctx->window, nb_glfw_mouse_button_cb);
        glfwSetKeyCallback(ctx->window, nb_glfw_key_cb);
        glfwSetCharCallback(ctx->window, nb_glfw_char_cb);

        glfwSwapInterval(1);
        glfwGetWindowSize(ctx->window, &ctx->width, &ctx->height);
        glfwGetFramebufferSize(ctx->window, &ctx->display_width, &ctx->display_height);

        gl3wInit();
        nbgl_setup(&ctx->gl_ctx, nb_ctx);

        ctx->frame_start = glfwGetTime();
        ctx->dt = 0.0f;

        return 1;
}


int
nb_glfw_shutdown(struct nb_glfw_ctx *ctx)
{
        /* clean up gl */
        nbgl_shutdown(&ctx->gl_ctx);

        /* clean up glfw */
        glfwDestroyWindow(ctx->window);
        glfwTerminate();

        return 1;
}


int
nb_glfw_tick(struct nb_glfw_ctx *ctx, struct nb_ctx *nb_ctx)
{
        /* update glfw */
        glfwPollEvents();
        int should_close = glfwWindowShouldClose(ctx->window);

        /* update neb states */
        nb_result res = NB_OK;

        struct nb_pointer_desc ptr_desc;
        ptr_desc.type_id = NB_STRUCT_POINTER;
        ptr_desc.ext = 0;
        ptr_desc.scroll_y = 0;
        ptr_desc.x = ctx->ms_pos_x;
        ptr_desc.y = ctx->ms_pos_y;
        ptr_desc.interact = ctx->ms_left_button_down;

        res = nb_state_set_pointer(nb_ctx, &ptr_desc);
        assert(res == NB_OK);

        struct nb_viewport_desc view_desc;
        view_desc.type_id = NB_STRUCT_VIEWPORT;
        view_desc.ext = 0;
        view_desc.width = ctx->display_width;
        view_desc.height = ctx->display_height;

        res = nb_state_set_viewport(nb_ctx, &view_desc);
        assert(res == NB_OK);

        if(!ctx->char_count) {
                ctx->chars[0] = 0;
        }
        nb_state_set_text_input(nb_ctx, ctx->chars);
        ctx->char_count = 0;

        nb_state_set_dt(nb_ctx, ctx->dt);

        double time = glfwGetTime();
        ctx->dt = (float)(time - ctx->frame_start);
        ctx->frame_start = time;

        glfwSwapBuffers(ctx->window);

        return !should_close;
}


#endif
#endif
