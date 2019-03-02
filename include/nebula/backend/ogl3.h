#ifndef OGL_BACKEND_INCLUDED_6824A550_3032_4C95_85C8_D3CD8E19820B
#define OGL_BACKEND_INCLUDED_6824A550_3032_4C95_85C8_D3CD8E19820B


#include <nebula/core.h>


struct nb_renderer_data;
typedef struct nbogl3_ctx * nbogl3_ctx_t;


/* -------------------------------------------------------------- Lifetime -- */


nb_result
nbogl3_ctx_create(
        nbogl3_ctx_t *ctx);


nb_result
nbogl3_ctx_destroy(
        nbogl3_ctx_t *ctx);


/* ---------------------------------------------------------------- Render -- */


nb_result
nbogl3_render(
        nbogl3_ctx_t ctx,
        struct nb_render_data *d);


/* inc guard */
#endif


/* ================================== IMPL ================================== */


#ifdef NEB_OGL3_IMPL
#ifndef NEB_OGL_IMPL_INCLUDED_ADD1F38B_2C6E_4D81_8AA7_48539E0E0B76
#define NEB_OGL_IMPL_INCLUDED_ADD1F38B_2C6E_4D81_8AA7_48539E0E0B76


#include <nebula/renderer.h>

#ifdef _WIN32
#include <windows.h>
#include <gl/GL.h>

typedef char GLchar;

#define GL_DEBUG_SOURCE_APPLICATION       0x824A
#define GL_DEBUG_TYPE_PUSH_GROUP          0x8269

#define APIENTRYP *

typedef void (APIENTRYP PFNGLPUSHDEBUGGROUPPROC)(GLenum source, GLuint id, GLsizei length, const GLchar *message);
typedef void (APIENTRYP PFNGLPOPDEBUGGROUPPROC)(void);
typedef void (APIENTRYP PFNGLCLEARCOLORPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (APIENTRYP PFNGLCLEARPROC)(GLbitfield bits);

PFNGLPUSHDEBUGGROUPPROC glPushDebugGroup_neb;
PFNGLPOPDEBUGGROUPPROC glPopDebugGroup_neb;
PFNGLCLEARCOLORPROC glClearColor_neb;
PFNGLCLEARPROC glClear_neb;

#define glPushDebugGroup glPushDebugGroup_neb
#define glPopDebugGroup glPopDebugGroup_neb
#define glClearColor glClearColor_neb
#define glClear glClear_neb

#endif

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#endif


struct nbogl3_ctx {
        GLuint ftex[NB_FONT_COUNT_MAX];
        GLuint vao;
        GLuint pro;
        GLuint vbo, ibo;
        GLint unitex, uniproj;
        GLint inpos, intex, incol;
};


/* ---------------------------------------------- Stdlib / Config / Macros -- */
/*
 * Nebula uses stdlib for some things, you can override them here.
 * Most of these macros are duplicated in core.h, renderer.h, sugar.h
 */


#ifndef NB_ASSERT
#include <assert.h>
#define NB_ASSERT(expr) assert(expr)
#endif

#ifndef NB_ALLOC
#include <stdlib.h>
#define NB_ALLOC(bytes) malloc(bytes)
#endif

#ifndef NB_FREE
#include <stdlib.h>
#define NB_FREE(addr) free(addr)
#endif

#ifndef NB_ZERO_MEM
#include <string.h>
#define NB_ZERO_MEM(ptr) do{memset((ptr), 0, sizeof((ptr)[0]));}while(0)
#endif


#define NB_ARR_COUNT(ARR) (sizeof((ARR)) / sizeof((ARR)[0]))
#define NB_ARRAY_DATA(ARR) &ARR[0]


/* -------------------------------------------------------------- Lifetime -- */


nb_result
nbogl3_ctx_create(
        nbogl3_ctx_t *ctx)
{
        if (!ctx) {
                return NB_INVALID_PARAMS;
        }

        struct nbogl3_ctx *nctx = 0;
        nctx = (struct nbogl3_ctx*)NB_ALLOC(sizeof(*nctx));

        #ifdef _WIN32
        void * tmp = 0;

        tmp = wglGetProcAddress("glClearColor");
        glClearColor = (PFNGLCLEARCOLORPROC)tmp;

        tmp = wglGetProcAddress("glClear");
        glClear = (PFNGLCLEARPROC)tmp;

        tmp = wglGetProcAddress("glPushDebugGroup");
        glPushDebugGroup = (PFNGLPUSHDEBUGGROUPPROC)tmp;

        tmp = wglGetProcAddress("glPopDebugGroup");
        glPopDebugGroup = (PFNGLPOPDEBUGGROUPPROC)tmp;

        #endif

        glPushDebugGroup(
                GL_DEBUG_SOURCE_APPLICATION,
                GL_DEBUG_TYPE_PUSH_GROUP,
                -1,
                "Nebula OGL Create");
        
        glPopDebugGroup();

        return NB_FAIL;
}


nb_result
nbogl3_ctx_destroy(
        nbogl3_ctx_t *ctx)
{
        if (!ctx) {
                return NB_INVALID_PARAMS;
        }

        glPushDebugGroup(
                GL_DEBUG_SOURCE_APPLICATION,
                GL_DEBUG_TYPE_PUSH_GROUP,
                -1,
                "Nebula OGL Destroy");



        glPopDebugGroup();

        return NB_FAIL;
}


/* ---------------------------------------------------------------- Render -- */


nb_result
nbogl3_render(
        nbogl3_ctx_t ctx,
        struct nb_render_data *d)
{
        if (!ctx || !d) {
                //return NB_INVALID_PARAMS;
        }

        glPushDebugGroup(
                GL_DEBUG_SOURCE_APPLICATION,
                GL_DEBUG_TYPE_PUSH_GROUP,
                -1,
                "Nebula OGL Render");

        glClearColor(1, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPopDebugGroup();

        return NB_FAIL;
}


/* ---------------------------------------------- Stdlib / Config / Macros -- */


#undef NB_ASSERT
#undef NB_ALLOC
#undef NB_FREE
#undef NB_ZERO_MEM
#undef NB_ARR_COUNT
#undef NB_ARRAY_DATA


#ifndef _WIN32
#undef glPopDebugGroup
#undef glPushDebugGroup
#endif


/* impl guard */
#endif
/* impl request */
#endif
