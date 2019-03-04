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


#ifndef NEB_OGL3_DEBUG_SUPPORT
        #ifndef NDEBUG
                #define NEB_OGL3_DEBUG_SUPPORT 1
        #else
                #define NEB_OGL3_DEBUG_SUPPORT 0
        #endif
#endif


#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#undef near
#undef far
#include <windows.h>
#include <gl/GL.h>
#include <stdint.h>
typedef char GLchar;
typedef void GLvoid;
typedef uintptr_t GLsizeiptr;
#define APIENTRYP *
#define glGetProcAddr wglGetProcAddress 
#endif

#if defined(__linux__)
#include <GL/gl.h>
#define glGetProcAddr glXGetProcAddress
#endif

#if defined(__linux__) || (_WIN32)


#define GL_R8                             0x8229
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_DEBUG_SOURCE_APPLICATION       0x824A
#define GL_DEBUG_TYPE_PUSH_GROUP          0x8269
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_TEXTURE0                       0x84C0
#define GL_STREAM_DRAW                    0x88E0
#define GL_WRITE_ONLY                     0x88B9


typedef void (APIENTRYP PFNGLPUSHDEBUGGROUPPROC)(GLenum source, GLuint id, GLsizei length, const GLchar *message);
typedef void (APIENTRYP PFNGLPOPDEBUGGROUPPROC)(void);
typedef void (APIENTRYP PFNGLACTIVETEXTUREPROC)(GLenum texture);
typedef void (APIENTRYP PFNGLATTACHSHADERPROC)(GLuint program, GLuint shd);
typedef void (APIENTRYP PFNGLBINDBUFFERPROC)(GLenum type, GLuint buffer_id);
typedef void (APIENTRYP PFNGLBINDVERTEXARRAYPROC)(GLuint vao);
typedef void (APIENTRYP PFNGLBUFFERDATAPROC)(GLenum, GLsizeiptr, const GLvoid*, GLenum use);
typedef void (APIENTRYP PFNGLCOMPILESHADERPROC)(GLuint shd);
typedef GLuint (APIENTRYP PFNGLCREATEPROGRAMPROC)(void);
typedef void (APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint idx);
typedef void (APIENTRYP PFNGLGENBUFFERSPROC)(GLsizei n, GLuint *buffs);
typedef void (APIENTRYP PFNGLGETPROGRAMIVPROC)(GLuint prog, GLenum pname, GLint*params);
typedef GLuint (APIENTRYP PFNGLCREATESHADERPROC)(GLenum shaderType);
typedef void (APIENTRYP PFNGLDELETESHADERPROC)(GLuint shd);
typedef void (APIENTRYP PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint *arr);
typedef GLint (APIENTRYP PFNGLGETATTRIBLOCATIONPROC)(GLuint prog, const GLchar *name);
typedef void (APIENTRYP PFNGLGETSHADERIVPROC)(GLuint shd, GLenum pname, GLint *params);
typedef GLint (APIENTRYP PFNGLGETUNIFORMLOCATIONPROC)(GLuint pro, const GLchar *name);
typedef void (APIENTRYP PFNGLLINKPROGRAMPROC)(GLuint pro);
typedef void * (APIENTRYP PFNGLMAPBUFFERPROC)(GLenum target, GLenum access);
typedef void (APIENTRYP PFNGLSHADERSOURCEPROC)(GLuint shd, GLsizei count, const GLchar *const *str, const GLint *len);
typedef void (APIENTRYP PFNGLUNIFORM1IPROC)(GLint loc, GLint v0);

#ifndef __linux__
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4FVPROC)(GLint loc, GLsizei count, const GLfloat *val);
#endif

typedef GLboolean (APIENTRYP PFNGLUNMAPBUFFERPROC)(GLenum target);
typedef void (APIENTRYP PFNGLUSEPROGRAMPROC)(GLuint pro);
typedef void (APIENTRYP PFNGVERTEXATTRIBPOINTERPROC)(GLuint idx, GLint size, GLenum type, GLboolean norm, GLsizei stride, const GLvoid *ptr);


PFNGLPUSHDEBUGGROUPPROC glPushDebugGroup_neb;
PFNGLPOPDEBUGGROUPPROC glPopDebugGroup_neb;
PFNGLACTIVETEXTUREPROC glActiveTexture_neb;
PFNGLATTACHSHADERPROC glAttachShader_neb;
PFNGLBINDBUFFERPROC glBindBuffer_neb;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray_neb;
PFNGLBUFFERDATAPROC glBufferData_neb;
PFNGLCOMPILESHADERPROC glCompileShader_neb;
PFNGLCREATEPROGRAMPROC glCreateProgram_neb;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray_neb;
PFNGLGENBUFFERSPROC glGenBuffers_neb;
PFNGLGETPROGRAMIVPROC glGetProgramiv_neb;
PFNGLCREATESHADERPROC glCreateShader_neb;
PFNGLDELETESHADERPROC glDeleteShader_neb;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays_neb;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation_neb;
PFNGLGETSHADERIVPROC glGetShaderiv_neb;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation_neb;
PFNGLLINKPROGRAMPROC glLinkProgram_neb;
PFNGLMAPBUFFERPROC glMapBuffer_neb;
PFNGLSHADERSOURCEPROC glShaderSource_neb;
PFNGLUNIFORM1IPROC glUniform1i_neb;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv_neb;
PFNGLUNMAPBUFFERPROC glUnmapBuffer_neb;
PFNGLUSEPROGRAMPROC glUseProgram_neb;
PFNGVERTEXATTRIBPOINTERPROC glVertexAttribPointer_neb;

#define glPushDebugGroup glPushDebugGroup_neb
#define glPopDebugGroup glPopDebugGroup_neb
#define glActiveTexture glActiveTexture_neb
#define glAttachShader glAttachShader_neb
#define glBindBuffer glBindBuffer_neb
#define glBindVertexArray glBindVertexArray_neb
#define glBufferData glBufferData_neb
#define glCompileShader glCompileShader_neb
#define glCreateProgram glCreateProgram_neb
#define glEnableVertexAttribArray glEnableVertexAttribArray_neb
#define glGenBuffers glGenBuffers_neb
#define glGetProgramiv glGetProgramiv_neb
#define glCreateShader glCreateShader_neb
#define glDeleteShader glDeleteShader_neb
#define glGenVertexArrays glGenVertexArrays_neb
#define glGetAttribLocation glGetAttribLocation_neb
#define glGetShaderiv glGetShaderiv_neb
#define glGetUniformLocation glGetUniformLocation_neb
#define glLinkProgram glLinkProgram_neb
#define glMapBuffer glMapBuffer_neb
#define glShaderSource glShaderSource_neb
#define glUniform1i glUniform1i_neb
#define glUniformMatrix4fv glUniformMatrix4fv_neb
#define glUnmapBuffer glUnmapBuffer_neb
#define glUseProgram glUseProgram_neb
#define glVertexAttribPointer glVertexAttribPointer_neb

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
        
        int vp_width, vp_height;
        nbr_viewport_get(nbr_ctx, &vp_width, &vp_height);
        
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
        
        proj[0][0] /= (GLfloat)vp_width;
        proj[1][1] /= (GLfloat)vp_height;

        glUseProgram(ctx->pro);
        glUniformMatrix4fv(ctx->uniproj, 1, GL_FALSE, &proj[0][0]);
        glViewport(0, 0, vp_width, vp_height);

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
                                int y = vp_height - (cmd->data.clip_rect[1] + h);

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

        tmp = glGetProcAddr("glPushDebugGroup");
        NB_ASSERT(tmp);
        glPushDebugGroup = (PFNGLPUSHDEBUGGROUPPROC)tmp;

        tmp = glGetProcAddr("glPopDebugGroup");
        NB_ASSERT(tmp);
        glPopDebugGroup = (PFNGLPOPDEBUGGROUPPROC)tmp;

        tmp = glGetProcAddr("glActiveTexture");
        NB_ASSERT(tmp);
        glActiveTexture = (PFNGLACTIVETEXTUREPROC)tmp;

        tmp = glGetProcAddr("glAttachShader");
        NB_ASSERT(tmp);
        glAttachShader = (PFNGLATTACHSHADERPROC)tmp;

        tmp = glGetProcAddr("glBindBuffer");
        NB_ASSERT(tmp);
        glBindBuffer = (PFNGLBINDBUFFERPROC)tmp;

        tmp = glGetProcAddr("glBindVertexArray");
        NB_ASSERT(tmp);
        glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)tmp;

        tmp = glGetProcAddr("glBufferData");
        NB_ASSERT(tmp);
        glBufferData = (PFNGLBUFFERDATAPROC)tmp;

        tmp = glGetProcAddr("glCompileShader");
        NB_ASSERT(tmp);
        glCompileShader = (PFNGLCOMPILESHADERPROC)tmp;

        tmp = glGetProcAddr("glCreateProgram");
        NB_ASSERT(tmp);
        glCreateProgram = (PFNGLCREATEPROGRAMPROC)tmp;

        tmp = glGetProcAddr("glEnableVertexAttribArray");
        NB_ASSERT(tmp);
        glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)tmp;

        tmp = glGetProcAddr("glGenBuffers");
        NB_ASSERT(tmp);
        glGenBuffers = (PFNGLGENBUFFERSPROC)tmp;

        tmp = glGetProcAddr("glGetProgramiv");
        NB_ASSERT(tmp);
        glGetProgramiv = (PFNGLGETPROGRAMIVPROC)tmp;

        tmp = glGetProcAddr("glCreateShader");
        NB_ASSERT(tmp);
        glCreateShader = (PFNGLCREATESHADERPROC)tmp;

        tmp = glGetProcAddr("glDeleteShader");
        NB_ASSERT(tmp);
        glDeleteShader = (PFNGLDELETESHADERPROC)tmp;

        tmp = glGetProcAddr("glGenVertexArrays");
        NB_ASSERT(tmp);
        glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)tmp;

        tmp = glGetProcAddr("glGetAttribLocation");
        NB_ASSERT(tmp);
        glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)tmp;

        tmp = glGetProcAddr("glGetShaderiv");
        NB_ASSERT(tmp);
        glGetShaderiv = (PFNGLGETSHADERIVPROC)tmp;

        tmp = glGetProcAddr("glGetUniformLocation");
        NB_ASSERT(tmp);
        glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)tmp;

        tmp = glGetProcAddr("glLinkProgram");
        NB_ASSERT(tmp);
        glLinkProgram = (PFNGLLINKPROGRAMPROC)tmp;

        tmp = glGetProcAddr("glMapBuffer");
        NB_ASSERT(tmp);
        glMapBuffer = (PFNGLMAPBUFFERPROC)tmp;

        tmp = glGetProcAddr("glShaderSource");
        NB_ASSERT(tmp);
        glShaderSource = (PFNGLSHADERSOURCEPROC)tmp;

        tmp = glGetProcAddr("glUniform1i");
        NB_ASSERT(tmp);
        glUniform1i = (PFNGLUNIFORM1IPROC)tmp;

        tmp = glGetProcAddr("glUniformMatrix4fv");
        NB_ASSERT(tmp);
        glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)tmp;

        tmp = glGetProcAddr("glUnmapBuffer");
        NB_ASSERT(tmp);
        glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)tmp;

        tmp = glGetProcAddr("glUseProgram");
        NB_ASSERT(tmp);
        glUseProgram = (PFNGLUSEPROGRAMPROC)tmp;

        tmp = glGetProcAddr("glVertexAttribPointer");
        NB_ASSERT(tmp);
        glVertexAttribPointer = (PFNGVERTEXATTRIBPOINTERPROC)tmp;

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


/* ---------------------------------------------- Stdlib / Config / Macros -- */


#undef NB_ASSERT
#undef NB_ALLOC
#undef NB_FREE
#undef NB_ZERO_MEM
#undef NB_ARR_COUNT
#undef NB_ARRAY_DATA


#ifndef _WIN32
#undef glPushDebugGroup
#undef glPopDebugGroup
#undef glActiveTexture
#undef glAttachShader
#undef glBindBuffer
#undef glBindVertexArray
#undef glBufferData
#undef glCompileShader
#undef glCreateProgram
#undef glEnableVertexAttribArray
#undef glGenBuffers
#undef glGetProgramiv
#undef glCreateShader
#undef glDeleteShader
#undef glGenVertexArrays
#undef glGetAttribLocation
#undef glGetShaderiv
#undef glGetUniformLocation
#undef glLinkProgram
#undef glMapBuffer
#undef glShaderSource
#undef glUniform1i
#undef glUniformMatrix4fv
#undef glUnmapBuffer
#undef glUseProgram
#undef glVertexAttribPointer

#undef GL_R8
#undef GL_FRAGMENT_SHADER
#undef GL_VERTEX_SHADER
#undef GL_DEBUG_SOURCE_APPLICATION
#undef GL_DEBUG_TYPE_PUSH_GROUP
#undef GL_COMPILE_STATUS
#undef GL_LINK_STATUS
#undef GL_ARRAY_BUFFER
#undef GL_ELEMENT_ARRAY_BUFFER
#undef GL_TEXTURE0
#undef GL_STREAM_DRAW
#undef GL_WRITE_ONLY
#endif


#ifdef __APPLE__
#undef GL_SILENCE_DEPRECATION
#endif


/* impl guard */
#endif
/* impl request */
#endif
