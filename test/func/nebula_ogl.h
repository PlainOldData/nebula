#ifndef NEB_OGL3_INCLUDED
#define NEB_OGL3_INCLUDED


#include <nebula/nebula.h>


struct nbgl_ctx {
        GLuint ftex[NB_FONT_COUNT_MAX];
        GLuint vao;
        GLuint pro;
        GLuint vbo, ibo;
        GLint unitex, uniproj;
        GLint inpos, intex, incol;
};


void
nbgl_setup(
        struct nbgl_ctx *ctx,
        struct nb_ctx *nb_ctx);


void
nbgl_render(
        int display_width,
        int display_height,
        struct nbgl_ctx *ctx,
        struct nb_ctx *nb_ctx);


void
nbgl_shutdown(
        struct nbgl_ctx *ctx);


/* inc guard */
#endif

/* IMPL */

#ifdef NEB_OGL3_IMPL
#ifndef NEB_OGL3_IMPL_INCLUDED
#define NEB_OGL3_IMPL_INCLUDED


#include <assert.h>
#include <string.h>


void
nbgl_setup(struct nbgl_ctx *ctx, struct nb_ctx *nb_ctx)
{
        unsigned int font_count = nb_get_font_count(nb_ctx);
        assert(font_count);

        unsigned int i;

        struct nb_font_tex font_tex_list[NB_FONT_COUNT_MAX];
        nb_get_font_tex_list(nb_ctx, font_tex_list);
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
}


void
nbgl_render(int display_width, int display_height, struct nbgl_ctx *ctx, struct nb_ctx *nb_ctx)
{
        /* get nebula render data */
        struct nb_render_data rd;
        memset(&rd, 0, sizeof(rd));
        nb_get_render_data(nb_ctx, &rd);

        /* setup gl */
        glDisable(GL_DEPTH_TEST);

        /* prepare pass */
        GLfloat proj[4][4] = {
                { 2.f, 0.f, 0.f, 0.f },
                { 0.f, -2.f, 0.f, 0.f },
                { 0.f, 0.f, -1.f, 0.f },
                { -1.f, 1.f, 0.f, 1.f },
        };

        proj[0][0] /= (GLfloat)display_width;
        proj[1][1] /= (GLfloat)display_height;

        glUseProgram(ctx->pro);
        glUniformMatrix4fv(ctx->uniproj, 1, GL_FALSE, &proj[0][0]);
        glViewport(0, 0, display_width, display_height);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        unsigned int f_idx = nb_debug_get_font(nb_ctx);
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

        /* render */
        for (unsigned int list_idx = 0; list_idx < rd.cmd_list_count; list_idx++) {
                struct nb_render_cmd_list * cmd_list = rd.cmd_lists + list_idx;

                for (unsigned int i = 0; i < cmd_list->count; ++i) {
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
                                        (void *)offset);
                        }
                }
        }

        glDisable(GL_SCISSOR_TEST);
}


void
nbgl_shutdown(struct nbgl_ctx *ctx)
{
        glDeleteProgram(ctx->pro);
        glDeleteVertexArrays(1, &ctx->vao);
}

#endif
#endif
