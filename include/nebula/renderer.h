#ifndef NEBULA_RENDERER_INCLUDED
#define NEBULA_RENDERER_INCLUDED


typedef int nb_result;


typedef enum nb_render_cmd_type {
        NB_RENDER_CMD_TYPE_TRIANGLES = 0,
        NB_RENDER_CMD_TYPE_LINES = 1,
        NB_RENDER_CMD_TYPE_SCISSOR = 2,
} nb_render_cmd_type;

struct nb_render_elem {
        unsigned int offset;
        unsigned int count;
};

union nb_render_cmd_data {
        struct nb_render_elem elem;
        short clip_rect[4];
};

struct nb_render_cmd {
        unsigned int type;
        union nb_render_cmd_data data;
};

struct nb_render_cmd_list {
        struct nb_render_cmd * cmds;
        unsigned int count;
};

struct nb_render_data {
        float * vtx;
        unsigned int vtx_count;

        unsigned short * idx;
        unsigned int idx_count;

        struct nb_render_cmd_list cmd_lists[64];
        unsigned int cmd_list_count;
};

/* This isn't going to stay around */
#define NB_FONT_COUNT_MAX 16

struct nb_font_tex {
        unsigned char * mem;
        unsigned int width;
};


/*
 * Get the render cmds
 * returns NB_OK on success
 * returns NB_FAIL if an internal error occured
 * returns NB_INVALID_PARAMS if ctx or data is null
 */
nb_result
nb_get_render_data(struct nbr_ctx *ctx, struct nb_render_data * data);

unsigned int
nb_get_font_count(struct nbr_ctx *ctx);

nb_result
nb_get_font_tex_list(struct nbr_ctx *ctx, struct nb_font_tex * tex_list);

/* DEBUG!! */
nb_result
nb_debug_set_font(struct nbr_ctx *ctx, unsigned int idx);

/* DEBUG!! */
unsigned int
nb_debug_get_font(struct nbr_ctx *ctx);



struct nbr_ctx {
        int i;
};


struct nbr_vtx_buf {
        float * v;
        unsigned int v_count;

        unsigned short * i;
        unsigned int i_count;
};


struct nbr_cmd_buf {
        struct nb_ctx * ctx;

        struct nb_render_cmd cmds[4096];
        unsigned int cmd_count;
};

void
nbr_box(struct nbi_cmd_buf * buf, float * rect, float * color, float radius);


void
nbr_line(struct nbi_cmd_buf * buf, float * p, float * q, float * color);


void
nbr_bez(struct nbi_cmd_buf * buf,
        float * p0,
        float * p1,
        float * p2,
        float * p3,
        float * color);


enum nbr_text_flags {
        NBI_TEXT_FLAGS_CURSOR = 1 << 15,
        NBI_TEXT_FLAGS_WRAP = 1 << 14,
        NBI_TEXT_FLAGS_TERM = 1 << 13,
};

void
nbr_get_text_size(struct nb_ctx * ctx, float width, unsigned int flags, const char * str, float * out_size);


void
nbr_text(struct nbi_cmd_buf * buf, float * pos, unsigned int flags, float * color, const char * str);


void nbr_scissor_set(struct nbi_cmd_buf * buf, float * rect);


void nbr_scissor_clear(struct nbi_cmd_buf * buf);



#endif


/* IMPL */


#ifdef NEBULA_RENDERER_IMPL
#ifndef NEBULA_RENDERER_IMPL_INCLUDED
#define NEBULA_RENDERER_IMPL_INCLUDED


void
nbr_box(struct nbi_cmd_buf * buf, float * rect, float * color, float radius) {
        struct nbi_vtx_buf * data = &buf->ctx->vtx_buf;

        int prim = NB_RENDER_CMD_TYPE_TRIANGLES;
        uint16_t vtx;

        struct nb_render_cmd * cmd = nbi_cmd_begin(buf, data, prim, &vtx);

        /* sharp corners - shortcut */
        if(radius <= 0.0f) {
                nbi_push_quad(data, vtx, rect, color);
                nbi_cmd_end(data, cmd);
                return;
        }

        float extent_min = (rect[2] < rect[3] ? rect[2] : rect[3]) * 0.5f;
        if(radius > extent_min) {
                radius = extent_min;
        }

        uint16_t seg_count = 7;

        uint16_t cv0 = vtx;
        uint16_t cv1 = vtx + (seg_count + 2);
        uint16_t cv2 = vtx + (seg_count + 2) * 2;
        uint16_t cv3 = vtx + (seg_count + 2) * 3;

        nbi_push_quad_idxs(data, cv1 + 1, cv1, cv0, cv0 + (seg_count + 1));
        nbi_push_quad_idxs(data, cv2 + 1, cv2, cv1, cv1 + (seg_count + 1));
        nbi_push_quad_idxs(data, cv3 + 1, cv3, cv2, cv2 + (seg_count + 1));
        nbi_push_quad_idxs(data, cv0 + 1, cv0, cv3, cv3 + (seg_count + 1));
        nbi_push_quad_idxs(data, cv1, cv2, cv3, cv0);

        float c0[2] = { rect[0] + rect[2] - radius, rect[1] + radius, };
        float ang = NB_TAU * 0.f;
        nbi_push_round_corner(data, c0, color, seg_count, radius, ang);

        float c1[2] = { rect[0] + radius, rect[1] + radius, };
        ang = NB_TAU * 0.25f;
        nbi_push_round_corner(data, c1, color, seg_count, radius, ang);

        float c2[2] = { rect[0] + radius, rect[1] + rect[3] - radius, };
        ang = NB_TAU * 0.50f;
        nbi_push_round_corner(data, c2, color, seg_count, radius, ang);

        float c3[2] = { rect[0] + rect[2] - radius, rect[1] + rect[3] - radius, };
        ang = NB_TAU * 0.75f;
        nbi_push_round_corner(data, c3, color, seg_count, radius, ang);

        nbi_cmd_end(data, cmd);
}

void
nbr_line(struct nbi_cmd_buf * buf, float * p, float * q, float * color) {
        struct nbi_vtx_buf * data = &buf->ctx->vtx_buf;

        uint16_t vtx;
        struct nb_render_cmd * cmd = nbi_cmd_begin(buf, data, NB_RENDER_CMD_TYPE_LINES, &vtx);

        data->i[data->i_count++] = vtx;
        data->i[data->i_count++] = vtx + 1;

        nbi_push_vtx(data, p[0], p[1], color);
        nbi_push_vtx(data, q[0], q[1], color);

        nbi_cmd_end(data, cmd);
}

void
nbr_bez(
        struct nbi_cmd_buf * buf,
        float * p0,
        float * p1,
        float * p2,
        float * p3,
        float * color)
{
        struct nbi_vtx_buf * data = &buf->ctx->vtx_buf;

        uint16_t vtx;
        struct nb_render_cmd * cmd = nbi_cmd_begin(buf, data, NB_RENDER_CMD_TYPE_LINES, &vtx);

        uint16_t seg_count = 16;
        uint16_t si, i;

        for(si = 0; si < seg_count; si++) {
                data->i[data->i_count++] = vtx + si;
        }

        for(i = 0; i < seg_count; i++) {
                float t = (float)i / (float)(seg_count - 1);
                float t2 = t * t;
                float t3 = t2 * t;

                float u = (1.0f - t);
                float u2 = u * u;
                float u3 = u2 * u;

                float px = u3 * p0[0] + (3 * u2 * t) * p1[0] + (3 * u * t2) * p2[0] + t3 * p3[0];
                float py = u3 * p0[1] + (3 * u2 * t) * p1[1] + (3 * u * t2) * p2[1] + t3 * p3[1];

                nbi_push_vtx(data, px, py, color);
        }

        nbi_cmd_end(data, cmd);
}

static unsigned int
nbi_decode_utf8_cp(char * utf8, unsigned int * cp) {
        *cp = 0;

        uint8_t * it = (uint8_t *)utf8;
        uint8_t byte = *it++;
        if((byte & 0x80) == 0) {
                *cp = (unsigned int)byte;
        }
        else if((byte & 0xE0) == 0xC0) {
                *cp =  (byte  & 0x1F) << 6;
                *cp |= (*it++ & 0x3F);
        }
        else if((byte & 0xF0) == 0xE0) {
                *cp =  (byte  & 0x0F) << 12;
                *cp |= (*it++ & 0x3F) << 6;
                *cp |= (*it++ & 0x3F);
        }
        else if((byte & 0xF8) == 0xF0) {
                *cp =  (byte  & 0x07) << 18;
                *cp |= (*it++ & 0x3F) << 12;
                *cp |= (*it++ & 0x3F) << 6;
                *cp |= (*it++ & 0x3F);
        }

        unsigned int result = (unsigned int)((char *)it - utf8);
        return result;
}

struct nbi_text_out {
        struct nbi_font * font;

        float start_x;
        float end_x;
        float max_x;
        float x;
        float y;
        float space;

        unsigned int vtx_start;
        unsigned int align_type;
};

static void
nbi_line_adv(struct nbi_vtx_buf * buf, struct nbi_text_out * out) {
        if(out->align_type != NB_TEXT_ALIGN_LEFT) {
                float offset = out->end_x - out->x;
                if(offset < 0.0f) {
                        offset = 0.0f;
                }
                if(out->align_type == NB_TEXT_ALIGN_CENTER) {
                        offset *= 0.5f;
                }
                offset = (float)((int32_t)offset);

                if(buf) {
                        unsigned int i;
                        for(i = out->vtx_start; i < buf->v_count; i += 8) {
                                float * v = buf->v + i;
                                *v += offset;
                        }

                        out->vtx_start = buf->v_count;
                }
        }

        if(out->x > out->max_x) {
                out->max_x = out->x;
        }

        out->x = out->start_x;
        /* Use actual font metrics to do vertical advance!! */
        out->y += out->font->height;
        out->space = 0.0f;
}

void
nbr_text_(struct nbi_cmd_buf * buf, struct nbi_font * font, float * rect, unsigned int flags, float * color, const char * text, float * out_size) {
        if(!text) {
                if(out_size) {
                        out_size[0] = 0.0f;
                        out_size[1] = font->height;
                }
                return;
        }

        unsigned int wrap = flags & NBI_TEXT_FLAGS_WRAP;
        unsigned int term_tag = flags & NBI_TEXT_FLAGS_TERM;

        struct nbi_text_out out = {
                .font = font,

                .start_x = rect[0],
                .end_x = rect[0] + rect[2],
                .max_x = rect[0],
                .x = rect[0],
                .y = rect[1],

                .align_type = flags & _NB_TEXT_ALIGN_BIT_MASK,
        };

        struct nbi_vtx_buf * data = 0;
        uint16_t vtx = 0;
        struct nb_render_cmd * cmd = 0;
        if(buf) {
                data = &buf->ctx->vtx_buf;
                cmd = nbi_cmd_begin(buf, data, NB_RENDER_CMD_TYPE_TRIANGLES, &vtx);

                out.vtx_start = data->v_count;
        }

        char * it = (char *)text;
        while(*it) {
                unsigned int cp;
                unsigned int cp_size = nbi_decode_utf8_cp(it, &cp);

                char c = *it;

                if(c == '\n') {
                        nbi_line_adv(data, &out);
                        it += cp_size;
                }
                else if(c == ' ') {
                        out.space += out.font->space_width;
                        it += cp_size;
                }
                else if(nbi_char_valid(out.font, cp)) {
                        unsigned int i;

                        char * word = it;
                        unsigned int word_size = 0;
                        while(*it && *it != '\n' && *it != ' ') {
                                if(term_tag && strncmp(it, "##", 2) == 0) {
                                        word_size = (unsigned int)(it - word);
                                        it += strlen(it);
                                        break;
                                }

                                unsigned int word_cp;
                                unsigned int word_cp_size = nbi_decode_utf8_cp(it, &word_cp);
                                if(nbi_char_valid(out.font, word_cp)) {
                                        it += word_cp_size;
                                        word_size = (unsigned int)(it - word);
                                }
                                else {
                                        word_size = (unsigned int)(it - word);
                                        break;
                                }
                        }


                        if(wrap && out.x > out.start_x) {
                                float word_end = out.x + out.space;
                                for(i = 0; i < word_size;) {
                                        unsigned int word_cp;
                                        i += nbi_decode_utf8_cp(word + i, &word_cp);
                                        word_end += nbi_get_glyph_width(out.font, word_cp);
                                }
                                if(word_end > out.end_x) {
                                        nbi_line_adv(data, &out);
                                }
                        }

                        out.x += out.space;
                        out.space = 0.0f;

                        for(i = 0; i < word_size;) {
                                unsigned int word_cp;
                                i += nbi_decode_utf8_cp(word + i, &word_cp);

                                float prev_x = out.x;

                                stbtt_aligned_quad q;
                                nbi_get_glyph_quad(out.font, word_cp, &out.x, &out.y, &q);

                                if(wrap && out.x > out.end_x) {
                                        out.x = prev_x;
                                        nbi_line_adv(data, &out);
                                        nbi_get_glyph_quad(out.font, word_cp, &out.x, &out.y, &q);
                                }

                                if(data) {
                                        nbi_push_quad_idxs(data, vtx, vtx + 1, vtx + 2, vtx + 3);
                                        nbi_push_vtx_uv(data, q.x0, q.y0, q.s0, q.t0, color); vtx++;
                                        nbi_push_vtx_uv(data, q.x0, q.y1, q.s0, q.t1, color); vtx++;
                                        nbi_push_vtx_uv(data, q.x1, q.y1, q.s1, q.t1, color); vtx++;
                                        nbi_push_vtx_uv(data, q.x1, q.y0, q.s1, q.t0, color); vtx++;
                                }
                        }
                }
                else {
                        it += cp_size;
                }
        }

        if(flags & NBI_TEXT_FLAGS_CURSOR) {
                float cursor_width = 1.0f;

                float prev_x = out.x;

                out.x += out.space;
                if(wrap && out.x > out.start_x) {
                        float cursor_end = out.x + cursor_width;
                        if(cursor_end > out.end_x) {
                                out.x = prev_x;
                                nbi_line_adv(data, &out);
                        }
                }

                if(buf) {
                        unsigned int cursor_on = ((unsigned int)(buf->ctx->state.text_cursor_time * 2.0f) & 1) == 0;
                        if(cursor_on) {
                                /* Font size is baked into these numbers */
                                float cursor_rect[4] = {
                                        out.x, out.y - 12.0f,
                                        cursor_width, 15.0f,
                                };
                                vtx += nbi_push_quad(data, vtx, cursor_rect, color);
                        }
                }

                out.x += cursor_width;
        }

        nbi_line_adv(data, &out);

        if(out_size) {
                out_size[0] = out.max_x - rect[0];
                out_size[1] = out.y - rect[1];
        }

        if(data) {
                nbi_cmd_end(data, cmd);
        }
}

void nbr_get_text_size(struct nb_ctx * ctx, float width, unsigned int flags, const char * text, float * out_size) {
        if(out_size) {
                struct nbi_font * font = ctx->font;
                float rect[4] = { 0.0f, 0.0f, width, 0.0f, };
                nbi_text_(0, font, rect, flags, 0, text, out_size);
        }
}

void
nbr_text(struct nbi_cmd_buf * buf, float * rect, unsigned int flags, float * color, const char * text) {
        struct nbi_font * font = buf->ctx->font;
        nbi_text_(buf, font, rect, flags, color, text, 0);
}

int16_t
nbr_clamp_f32_to_i16(float x) {
        return x < 32767.0f ? (x > -32768.0f ? (int16_t)x : -32768) : 32767;
}

void nbr_scissor_set(struct nbi_cmd_buf * buf, float * rect) {
        buf->cmds[buf->cmd_count++] = (struct nb_render_cmd) {
                .type = NB_RENDER_CMD_TYPE_SCISSOR,
                .data.clip_rect[0] = nbi_clamp_f32_to_i16(rect[0]),
                .data.clip_rect[1] = nbi_clamp_f32_to_i16(rect[1]),
                .data.clip_rect[2] = nbi_clamp_f32_to_i16(rect[2]),
                .data.clip_rect[3] = nbi_clamp_f32_to_i16(rect[3]),
        };
}

void nbr_scissor_clear(struct nbi_cmd_buf * buf) {
        buf->cmds[buf->cmd_count++] = (struct nb_render_cmd) {
                .type = NB_RENDER_CMD_TYPE_SCISSOR,
                .data.clip_rect[0] = 0,
                .data.clip_rect[1] = 0,
                .data.clip_rect[2] = 0x7FFF,
                .data.clip_rect[3] = 0x7FFF,
        };
}

#endif
#endif
