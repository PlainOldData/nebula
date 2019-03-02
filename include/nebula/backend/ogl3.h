#ifndef OGL_BACKEND_INCLUDED_6824A550_3032_4C95_85C8_D3CD8E19820B
#define OGL_BACKEND_INCLUDED_6824A550_3032_4C95_85C8_D3CD8E19820B

#include <nebula/core.h>

typedef struct nbogl3_ctx * nbogl3_ctx_t;
typedef struct nb_renderer_ctx * nbr_ctx_t;


/* -------------------------------------------------------------- Lifetime -- */


nb_result
nbogl3_ctx_create(
        nbogl3_ctx_t *ctx,
        nbr_ctx_t nbr_ctx);


nb_result
nbogl3_ctx_destroy(
        nbogl3_ctx_t *ctx);


/* ---------------------------------------------------------------- Render -- */


nb_result
nbogl3_render(
        nbogl3_ctx_t ctx,
        nbr_ctx_t nbr_ctx);


/* inc guard */
#endif


/* ================================== IMPL ================================== */


#ifdef NEB_OGL3_IMPL
#ifndef NEB_OGL_IMPL_INCLUDED_ADD1F38B_2C6E_4D81_8AA7_48539E0E0B76
#define NEB_OGL_IMPL_INCLUDED_ADD1F38B_2C6E_4D81_8AA7_48539E0E0B76


#include <nebula/renderer.h>


#define NEB_OGL3_DEBUG_SUPPORT 1


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

#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>


#define GL_DEBUG_SOURCE_APPLICATION       0x824A
#define GL_DEBUG_TYPE_PUSH_GROUP          0x8269


#define glPushDebugGroup glPushDebugGroup_neb
#define glPopDebugGroup glPopDebugGroup_neb


static void
glPushDebugGroup_neb(
        GLenum source,
        GLuint ids,
        GLsizei length,
        const GLchar *msg)
{
        (void)source;
        (void)ids;
        glPushGroupMarkerEXT(length, msg);
}


static void
glPopDebugGroup_neb() {
        glPopGroupMarkerEXT();
}


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
        nbogl3_ctx_t *nctx,
        nbr_ctx_t nbr_ctx)
{
        if (!nctx || !nbr_ctx) {
                return NB_INVALID_PARAMS;
        }

        struct nbogl3_ctx *ctx = 0;
        ctx = (struct nbogl3_ctx*)NB_ALLOC(sizeof(*ctx));

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
        
        if(NEB_OGL3_DEBUG_SUPPORT) {
                glPushDebugGroup(
                        GL_DEBUG_SOURCE_APPLICATION,
                        GL_DEBUG_TYPE_PUSH_GROUP,
                        -1,
                        "Nebula OGL Create");
        }
        
        unsigned int font_count = nb_get_font_count(nbr_ctx);
        assert(font_count);

        unsigned int i;

        struct nb_font_tex font_tex_list[NB_FONT_COUNT_MAX];
        nb_get_font_tex_list(nbr_ctx, font_tex_list);
        for (i = 0; i < font_count; i++) {
                struct nb_font_tex * f = font_tex_list + i;

                GLuint * ftex = ctx->ftex + i;
                glGenTextures(1, ftex);
                assert(*ftex);
                glBindTexture(GL_TEXTURE_2D, *ftex);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, f->width, f->width, 0, GL_RED, GL_UNSIGNED_BYTE, f->mem);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }

        const char * vs_src = "#version 150\n"
                "in vec2 position;\n"
                "in vec2 texcoord;\n"
                "in vec4 color;\n"
                "uniform mat4 projection;\n"
                "out vec2 frag_uv;\n"
                "out vec4 frag_color;\n"
                "void main() {"
                "       frag_uv = texcoord;\n"
                "       frag_color = color;\n"
                "       gl_Position = projection * vec4(position.xy, 0, 1);\n"
                "}";

        const char * fs_src = "#version 150\n"
                "in vec2 frag_uv;\n"
                "in vec4 frag_color;\n"
                "uniform sampler2D texture_map;\n"
                "out vec4 out_color;\n"
                "void main() {"
                "       float a = frag_color.a;"
                "       if(frag_uv.x != 0.0 || frag_uv.y != 0.0) {"
                "               a *= texture(texture_map, frag_uv.xy).r;"
                "       }"
                "       out_color = vec4(frag_color.rgb * a, a);\n"
                "}";

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(vs, 1, (const GLchar * const *)&vs_src, 0);
        glShaderSource(fs, 1, (const GLchar * const *)&fs_src, 0);

        glCompileShader(vs);
        glCompileShader(fs);

        GLint status;
        glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
        assert(status == GL_TRUE);
        glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
        assert(status == GL_TRUE);

        ctx->pro = glCreateProgram();
        glAttachShader(ctx->pro, vs);
        glAttachShader(ctx->pro, fs);
        glLinkProgram(ctx->pro);
        glGetProgramiv(ctx->pro, GL_LINK_STATUS, &status);
        assert(status == GL_TRUE);

        glDeleteShader(vs);
        glDeleteShader(fs);

        ctx->unitex = glGetUniformLocation(ctx->pro, "texture_map");
        ctx->uniproj = glGetUniformLocation(ctx->pro, "projection");
        ctx->inpos = glGetAttribLocation(ctx->pro, "position");
        ctx->incol = glGetAttribLocation(ctx->pro, "color");
        ctx->intex = glGetAttribLocation(ctx->pro, "texcoord");

        glGenBuffers(1, &ctx->ibo);
        glGenBuffers(1, &ctx->vbo);
        glGenVertexArrays(1, &ctx->vao);

        glBindVertexArray(ctx->vao);
        glBindBuffer(GL_ARRAY_BUFFER, ctx->vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ctx->ibo);

        glEnableVertexAttribArray(ctx->inpos);
        glEnableVertexAttribArray(ctx->incol);
        glEnableVertexAttribArray(ctx->intex);

        struct vertex {
                float pos[2];
                float uv[2];
                float color[4];
        };

        GLsizei stride = sizeof(struct vertex);
        void * ptr = (void*)offsetof(struct vertex, pos);
        glVertexAttribPointer(ctx->inpos, 2, GL_FLOAT, GL_FALSE, stride, ptr);

        ptr = (void*)offsetof(struct vertex, uv);
        glVertexAttribPointer(ctx->intex, 2, GL_FLOAT, GL_FALSE, stride, ptr);

        ptr = (void*)offsetof(struct vertex, color);
        glVertexAttribPointer(ctx->incol, 4, GL_FLOAT, GL_FALSE, stride, ptr);
        
        if(NEB_OGL3_DEBUG_SUPPORT) {
                glPopDebugGroup();
        }

        *nctx = ctx;

        return NB_OK;
}


nb_result
nbogl3_ctx_destroy(
        nbogl3_ctx_t *ctx)
{
        if (!ctx) {
                return NB_INVALID_PARAMS;
        }

        if(NEB_OGL3_DEBUG_SUPPORT) {
                glPushDebugGroup(
                        GL_DEBUG_SOURCE_APPLICATION,
                        GL_DEBUG_TYPE_PUSH_GROUP,
                        -1,
                        "Nebula OGL Destroy");
        }

        

        if(NEB_OGL3_DEBUG_SUPPORT) {
                glPopDebugGroup();
        }

        return NB_FAIL;
}


/* ---------------------------------------------------------------- Render -- */


nb_result
nbogl3_render(
        nbogl3_ctx_t ctx,
        nbr_ctx_t nbr_ctx)
{
        if (!ctx || !nbr_ctx) {
                return NB_INVALID_PARAMS;
        }

        if(NEB_OGL3_DEBUG_SUPPORT) {
                glPushDebugGroup(
                        GL_DEBUG_SOURCE_APPLICATION,
                        GL_DEBUG_TYPE_PUSH_GROUP,
                        -1,
                        "Nebula OGL Render");
        }

        /* get nebula render data */
        struct nb_render_data rd;
        memset(&rd, 0, sizeof(rd));
        nb_get_render_data(nbr_ctx, &rd);

        /* setup gl */
        glDisable(GL_DEPTH_TEST);

        /* prepare pass */
        GLfloat proj[4][4] = {
                { 2.f, 0.f, 0.f, 0.f },
                { 0.f, -2.f, 0.f, 0.f },
                { 0.f, 0.f, -1.f, 0.f },
                { -1.f, 1.f, 0.f, 1.f },
        };
        
        float display_width = 500;
        float display_height = 500;

        proj[0][0] /= (GLfloat)display_width;
        proj[1][1] /= (GLfloat)display_height;

        glUseProgram(ctx->pro);
        glUniformMatrix4fv(ctx->uniproj, 1, GL_FALSE, &proj[0][0]);
        glViewport(0, 0, display_width, display_height);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        unsigned int f_idx = nb_debug_get_font(nbr_ctx);
        GLuint ftex = ctx->ftex[f_idx];

        glUniform1i(ctx->unitex, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ftex);

        glBindVertexArray(ctx->vao);
        glBindBuffer(GL_ARRAY_BUFFER, ctx->vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ctx->ibo);

        int vtx_size = sizeof(rd.vtx[0]) * rd.vtx_count;
        int idx_size = sizeof(rd.idx[0]) * rd.idx_count;

        glBufferData(GL_ARRAY_BUFFER, vtx_size, NULL, GL_STREAM_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx_size, NULL, GL_STREAM_DRAW);

        void *vbo_data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        void *ibo_data = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);

        memcpy(vbo_data, (void*)rd.vtx, vtx_size);
        memcpy(ibo_data, (void*)rd.idx, idx_size);

        glUnmapBuffer(GL_ARRAY_BUFFER);
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

        glEnable(GL_SCISSOR_TEST);
        // glEnable(GL_DEPTH_TEST);

        /* render */
        unsigned int list_idx, i;
        for (list_idx = 0; list_idx < rd.cmd_list_count; list_idx++) {
                struct nb_render_cmd_list * cmd_list = rd.cmd_lists + list_idx;

                for (i = 0; i < cmd_list->count; ++i) {
                        struct nb_render_cmd * cmd = cmd_list->cmds + i;

                        if (cmd->type == NB_RENDER_CMD_TYPE_SCISSOR) {
                                int w = cmd->data.clip_rect[2];
                                int h = cmd->data.clip_rect[3];

                                int x = cmd->data.clip_rect[0];
                                int y = display_height - (cmd->data.clip_rect[1] + h);

                                glScissor(x, y, w, h);
                        }
                        else {
                                GLenum mode = GL_TRIANGLES;

                                if (cmd->type == NB_RENDER_CMD_TYPE_LINES) {
                                        mode = GL_LINE_STRIP;
                                }

                                unsigned long offset = cmd->data.elem.offset * sizeof(unsigned short);
                                glDrawElements(
                                        mode,
                                        cmd->data.elem.count,
                                        GL_UNSIGNED_SHORT,
                                        (void *)((uint64_t)offset));
                        }
                }
        }

        glDisable(GL_SCISSOR_TEST);

        if(NEB_OGL3_DEBUG_SUPPORT) {
                glPopDebugGroup();
        }

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


#ifdef __APPLE__
#undef GL_SILENCE_DEPRECATION
#endif


/* impl guard */
#endif
/* impl request */
#endif
