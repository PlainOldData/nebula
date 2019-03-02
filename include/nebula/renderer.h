#ifndef NEBULA_RENDERER_INCLUDED
#define NEBULA_RENDERER_INCLUDED


#include <nebula/core.h>

#include "stb_truetype.h"

#include "nebula_font_open_sans.h"
#include "nebula_font_proggy.h"
#include "nebula_font_awesome.h"


#define NB_FONT_COUNT_MAX 16
#define NB_TAU 6.2831853071


typedef enum nb_text_align {
        NB_TEXT_ALIGN_LEFT = 0,
        NB_TEXT_ALIGN_RIGHT = 1,
        NB_TEXT_ALIGN_CENTER = 2,

        _NB_TEXT_ALIGN_BIT_MASK = 3,
} nb_text_align;


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
#define NBI_FONT_RANGE_COUNT_MAX 4


struct nb_font_tex {
        unsigned char * mem;
        unsigned int width;
};


struct nbi_font_range {
        unsigned int start;
        unsigned int end;
        float height;
};


struct nbi_font {
        struct nb_font_tex tex;

        stbtt_packedchar * range_data[NBI_FONT_RANGE_COUNT_MAX];
        struct nbi_font_range ranges[NBI_FONT_RANGE_COUNT_MAX];
        unsigned int range_count;

        float height;
        float space_width;
};


struct nbi_vtx_buf {
        float * v;
        unsigned int v_count;

        unsigned short * i;
        unsigned int i_count;
};


struct nbi_cmd_buf {
        struct nb_render_cmd cmds[4096];
        unsigned int cmd_count;
};

typedef struct nb_renderer_ctx * nbr_ctx_t;

struct nb_renderer_ctx {
        struct nbi_font fonts[NB_FONT_COUNT_MAX];
        unsigned int font_count;
        struct nbi_font * font;
        struct nbi_font * debug_font_next;

        struct nbi_vtx_buf vtx_buf;

        struct nbi_cmd_buf cmds[256];
        int cmds_count;

        struct nbi_cmd_buf *submited_cmds[256];
        int cmds_submit_count;
};


/* -------------------------------------------------------------- Lifetime -- */


nb_result
nbr_ctx_create(
        nbr_ctx_t *c);


nb_result
nbr_ctx_destroy(
        nbr_ctx_t *c);

/*
 * return NB_OK if the new frame has started
 * return NB_INVALID_PARAMS if ctx if not valid
 * return NB_FAIL if an internal error occured
 */
nb_result
nbr_frame_begin(
        struct nb_renderer_ctx * ctx);      /* required */


/*
 * return NB_OK if the new frame was submitted
 * return NB_INVALID_PARAMS if ctx is not valid
 * return NB_FAIL if an internal error occured
 */
nb_result
nbr_frame_submit(
        struct nb_renderer_ctx * ctx,       /* required */
        struct nbi_cmd_buf ** cmd_bufs,     /* optional - null renders nothing */
        int cmd_buf_count);                 /* 0 renders nothing */


/* ----------------------------------------------------------------- Fonts -- */


unsigned int
nb_get_font_count(
        struct nb_renderer_ctx *ctx);


nb_result
nb_get_font_tex_list(
        struct nb_renderer_ctx *ctx,
        struct nb_font_tex * tex_list);


/* DEBUG!! */
nb_result
nb_debug_set_font(
        struct nb_renderer_ctx *ctx,
        unsigned int idx);


/* DEBUG!! */
unsigned int
nb_debug_get_font(
        struct nb_renderer_ctx *ctx);


void
nbi_push_font_range(
        struct nbi_font * font,
        stbtt_pack_context * stbtt,
        unsigned char * ttf,
        unsigned int start,
        unsigned int end,
        float height);


unsigned int
nbi_get_font_range_idx(
        struct nbi_font * font,
        unsigned int cp);


void
nbi_font_init(
        struct nbi_font * font,
        unsigned char * ttf,
        float height);


/* ------------------------------------------------------- Render Commands -- */


nb_result
nb_render_cmd_create(
        struct nb_renderer_ctx * ctx,       /* required */
        struct nbi_cmd_buf ** out_cmd);     /* required */


/*
 * Get the render cmds
 * returns NB_OK on success
 * returns NB_FAIL if an internal error occured
 * returns NB_INVALID_PARAMS if ctx or data is null
 */
nb_result
nb_get_render_data(
        struct nb_renderer_ctx *ctx,        /* required */
        struct nb_render_data * data);      /* required */


void
nbr_box(
        struct nb_renderer_ctx *ctx,        /* required */
        struct nbi_cmd_buf * buf,           /* required */
        struct nb_rect rect,
        struct nb_color color,
        float radius);


void
nbr_line(
        struct nb_renderer_ctx *ctx,
        struct nbi_cmd_buf * buf,
        float * p,
        float * q,
        float * color);


void
nbr_bez(
        struct nb_renderer_ctx *ctx,
        struct nbi_cmd_buf * buf,
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
nbr_get_text_size(
        struct nb_renderer_ctx * ctx,
        float width,
        unsigned int flags,
        const char * str,
        float * out_size);


void
nbr_text(
        struct nb_renderer_ctx * ctx,
        struct nbi_cmd_buf * buf,
        float * pos,
        unsigned int flags,
        float * color,
        const char * str);


void
nbr_scissor_set(
        struct nbi_cmd_buf * buf,
        float * rect);


void
nbr_scissor_clear(
        struct nbi_cmd_buf * buf);


unsigned int
nbi_char_valid(
        struct nbi_font * font,
        unsigned int cp);


void
nbi_get_glyph_quad(
        struct nbi_font * font,
        unsigned int cp,
        float * x,
        float * y,
        stbtt_aligned_quad * q);


float
nbi_get_glyph_width(
        struct nbi_font * font,
        unsigned int cp);


#endif


/* ================================== IMPL ================================== */


#ifdef NEB_RENDERER_IMPL
#ifndef NEBULA_RENDERER_IMPL_INCLUDED
#define NEBULA_RENDERER_IMPL_INCLUDED


#include "nebula_font_awesome.h"
#include "nebula_font_open_sans.h"
#include "nebula_font_proggy.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"


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


/* ----------------------------------------------------------- Text / Font -- */


unsigned int
nbi_char_valid(struct nbi_font * font, unsigned int cp) {
        unsigned int range_idx = nbi_get_font_range_idx(font, cp);
        unsigned int result = range_idx < font->range_count ? 1 : 0;
        return result;
}



void
nbi_get_glyph_quad(
        struct nbi_font * font,
        unsigned int cp,
        float * x,
        float * y,
        stbtt_aligned_quad * q)
{
        /*cp = 0xF588;*/

        unsigned int range_idx = nbi_get_font_range_idx(font, cp);
        if(range_idx < font->range_count) {
                stbtt_packedchar * data = font->range_data[range_idx];
                unsigned int size = font->tex.width;
                int glyph = (int)(cp - font->ranges[range_idx].start);
                stbtt_GetPackedQuad(data, size, size, glyph, x, y, q, 1);
        }
}

float
nbi_get_glyph_width(struct nbi_font * font, unsigned int cp) {
        float result = 0.0f;
        if(nbi_char_valid(font, cp)) {
                float y = 0.0f;
                stbtt_aligned_quad q;
                nbi_get_glyph_quad(font, cp, &result, &y, &q);
        }
        return result;
}


unsigned int
nb_debug_get_font(
        struct nb_renderer_ctx * ctx)
{
        NB_ASSERT(ctx);

        struct nbi_font * font = ctx->debug_font_next;
        if(!font) {
                font = ctx->font;
        }
        NB_ASSERT(font);

        unsigned int result = (unsigned int)(font - ctx->fonts);
        NB_ASSERT(result < ctx->font_count);
        return result;
}


void
nbi_push_font_range(
        struct nbi_font * font,
        stbtt_pack_context * stbtt,
        unsigned char * ttf,
        unsigned int start,
        unsigned int end,
        float height)
{
        unsigned int idx = font->range_count++;
        struct nbi_font_range * range = font->ranges + idx;
        *range = (struct nbi_font_range) {
                .start = start,
                .end = end,
                .height = height,
        };

        unsigned int char_count = range->end - range->start;
        font->range_data[idx] = NB_ALLOC(sizeof(stbtt_packedchar) * char_count);
        stbtt_PackFontRange(stbtt, ttf, 0, height, range->start, char_count, font->range_data[idx]);
}


unsigned int
nbi_get_font_range_idx(
        struct nbi_font * font,
        unsigned int cp)
{
        unsigned int result = font->range_count;
        unsigned int i;

        for(i = 0; i < font->range_count; i++) {
                struct nbi_font_range * range = font->ranges + i;
                if(cp >= range->start && cp < range->end) {
                        result = i;
                        break;
                }
        }
        return result;
}


void
nbi_font_init(
        struct nbi_font * font,
        unsigned char * ttf,
        float height)
{
        font->tex.width = 512;
        font->tex.mem = NB_ALLOC(font->tex.width * font->tex.width);
        font->height = height;

        stbtt_pack_context stbtt;
        stbtt_PackBegin(&stbtt, font->tex.mem, font->tex.width, font->tex.width, font->tex.width, 1, 0);
        /*stbtt_PackSetOversampling(&stbtt, 2, 2);*/

        font->range_count = 0;
        nbi_push_font_range(font, &stbtt, ttf, 32, 127, height);
        nbi_push_font_range(font, &stbtt, NB_FONT_AWESOME_TTF, NB_FA_CODE_MIN, NB_FA_CODE_MAX, 12.0f);

        stbtt_PackEnd(&stbtt);

        font->space_width = nbi_get_glyph_width(font, ' ');
}


/* -------------------------------------------------------- Mesh Resources -- */


static void
nbi_push_vtx(struct nbi_vtx_buf * buf, float x, float y, float * color) {
        buf->v[buf->v_count++] = x;
        buf->v[buf->v_count++] = y;

        buf->v[buf->v_count++] = 0.0f;
        buf->v[buf->v_count++] = 0.0f;

        buf->v[buf->v_count++] = color[0];
        buf->v[buf->v_count++] = color[1];
        buf->v[buf->v_count++] = color[2];
        buf->v[buf->v_count++] = color[3];
}

static void
nbi_push_vtx_uv(
        struct nbi_vtx_buf * buf,
        float x,
        float y,
        float u,
        float v,
        float * color)
{
        buf->v[buf->v_count++] = x;
        buf->v[buf->v_count++] = y;

        buf->v[buf->v_count++] = u;
        buf->v[buf->v_count++] = v;

        buf->v[buf->v_count++] = color[0];
        buf->v[buf->v_count++] = color[1];
        buf->v[buf->v_count++] = color[2];
        buf->v[buf->v_count++] = color[3];
}

static void
nbi_push_quad_idxs(
        struct nbi_vtx_buf * buf,
        short top_lft,
        short bot_lft,
        short bot_rgt,
        short top_rgt)
{
        buf->i[buf->i_count++] = top_lft;
        buf->i[buf->i_count++] = bot_lft;
        buf->i[buf->i_count++] = bot_rgt;

        buf->i[buf->i_count++] = top_lft;
        buf->i[buf->i_count++] = bot_rgt;
        buf->i[buf->i_count++] = top_rgt;
}

static short
nbi_push_quad(
        struct nbi_vtx_buf * buf,
        short vtx,
        float * rect,
        float * color)
{
        nbi_push_quad_idxs(buf, vtx, vtx + 1, vtx + 2, vtx + 3);
        nbi_push_vtx(buf, rect[0], rect[1], color);
        nbi_push_vtx(buf, rect[0], rect[1] + rect[3], color);
        nbi_push_vtx(buf, rect[0] + rect[2], rect[1] + rect[3], color);
        nbi_push_vtx(buf, rect[0] + rect[2], rect[1], color);
        return 4;
}

static short
nbi_push_round_corner(
        struct nbi_vtx_buf * buf,
        float * pos,
        float * color,
        short seg_count,
        float radius, float angle)
{
        NB_ASSERT(seg_count);

        short i;

        short vtx = (short)(buf->v_count / 8);

        for(i = 0; i < seg_count; i++) {
                buf->i[buf->i_count++] = vtx;
                buf->i[buf->i_count++] = vtx + (i + 1);
                buf->i[buf->i_count++] = vtx + (i + 2);
        }

        nbi_push_vtx(buf, pos[0], pos[1], color);

        float d_angle = ((float)NB_TAU * 0.25f) / (float)seg_count;

        for(i = 0; i < (seg_count + 1); i++) {
                float t = angle + d_angle * (float)i;
                float x = pos[0] + cosf(t) * radius;
                float y = pos[1] - sinf(t) * radius;
                nbi_push_vtx(buf, x, y, color);
        }

        return seg_count + 2;
}


unsigned int
nb_get_font_count(
        struct nb_renderer_ctx * ctx)
{
        NB_ASSERT(ctx);
        return ctx->font_count;
}

nb_result
nb_get_font_tex_list(
        struct nb_renderer_ctx * ctx,
        struct nb_font_tex * tex_list)
{
        NB_ASSERT(ctx);
        NB_ASSERT(tex_list);

        unsigned int i;
        for(i = 0; i < ctx->font_count; i++) {
                tex_list[i] = ctx->fonts[i].tex;
        }

        return NB_OK;
}


static unsigned int
nbi_decode_utf8_cp(
        char * utf8,
        unsigned int * cp)
{
        *cp = 0;

        unsigned char * it = (unsigned char *)utf8;
        unsigned char byte = *it++;
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


/* ------------------------------------------------------- Render Commands -- */


nb_result
nb_render_cmd_create(
        struct nb_renderer_ctx * ctx,
        struct nbi_cmd_buf ** out_cmd)
{
        if(!ctx || !out_cmd) {
                NB_ASSERT(!"NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        size_t i = ctx->cmds_count;
        *out_cmd = &ctx->cmds[i];

        ctx->cmds_count += 1;

        return NB_OK;
}


static struct nb_render_cmd *
nbi_cmd_begin(
        struct nbi_cmd_buf * buf,
        struct nbi_vtx_buf * data,
        unsigned int type,
        short * vtx)
{
        struct nb_render_cmd * cmd = buf->cmds + buf->cmd_count++;
        cmd->type = type;
        cmd->data.elem.offset = data->i_count;

        if(vtx) {
                *vtx = (short)(data->v_count / 8);
        }

        return cmd;
}

static void
nbi_cmd_end(struct nbi_vtx_buf * data, struct nb_render_cmd * cmd) {
        if(cmd) {
                cmd->data.elem.count = data->i_count - cmd->data.elem.offset;
        }
}


void
nbr_box(
        struct nb_renderer_ctx * ctx,
        struct nbi_cmd_buf * buf,
        struct nb_rect rec,
        struct nb_color col,
        float radius)
{
        float rect[4];
        rect[0] = (float)rec.x; rect[1] = (float)rec.y;
        rect[2] = (float)rec.w; rect[3] = (float)rec.h;

        float color[4];
        color[0] = col.r; color[1] = col.g;
        color[2] = col.b; color[3] = col.a;

        struct nbi_vtx_buf * data = &ctx->vtx_buf;

        int prim = NB_RENDER_CMD_TYPE_TRIANGLES;
        short vtx;

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

        short seg_count = 7;

        short cv0 = vtx;
        short cv1 = vtx + (seg_count + 2);
        short cv2 = vtx + (seg_count + 2) * 2;
        short cv3 = vtx + (seg_count + 2) * 3;

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

        /* err */
        ctx->cmds_count += 1;
}


void
nbr_line(
        struct nb_renderer_ctx *ctx,
        struct nbi_cmd_buf * buf,
        float * p,
        float * q,
        float * color)
{
        struct nbi_vtx_buf * data = &ctx->vtx_buf;

        short vtx;
        struct nb_render_cmd * cmd = nbi_cmd_begin(buf, data, NB_RENDER_CMD_TYPE_LINES, &vtx);

        data->i[data->i_count++] = vtx;
        data->i[data->i_count++] = vtx + 1;

        nbi_push_vtx(data, p[0], p[1], color);
        nbi_push_vtx(data, q[0], q[1], color);

        nbi_cmd_end(data, cmd);
}


void
nbr_bez(
        struct nb_renderer_ctx *ctx,
        struct nbi_cmd_buf * buf,
        float * p0,
        float * p1,
        float * p2,
        float * p3,
        float * color)
{
        struct nbi_vtx_buf * data = &ctx->vtx_buf;

        short vtx;
        struct nb_render_cmd * cmd = nbi_cmd_begin(buf, data, NB_RENDER_CMD_TYPE_LINES, &vtx);

        short seg_count = 16;
        short si, i;

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
                offset = (float)((int)offset);

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
nbr_text_(
        struct nb_renderer_ctx * ctx,
        struct nbi_cmd_buf * buf,
        struct nbi_font * font,
        float * rect,
        unsigned int flags,
        float * color,
        const char * text,
        float * out_size)
{
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
        short vtx = 0;
        struct nb_render_cmd * cmd = 0;
        if(buf) {
                data = &ctx->vtx_buf;
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
                        unsigned int cursor_on = 1;//((unsigned int)(buf->ctx->state.text_cursor_time * 2.0f) & 1) == 0;
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

void
nbr_get_text_size(
        struct nb_renderer_ctx * ctx,
        float width, unsigned int flags,
        const char * text,
        float * out_size)
{
        if(out_size) {
                return;
        }

        struct nbi_font * font = ctx->font;
        float rect[4] = { 0.0f, 0.0f, width, 0.0f, };
        nbr_text_(ctx, 0, font, rect, flags, 0, text, out_size);
}


void
nbr_text(
        struct nb_renderer_ctx * ctx,
        struct nbi_cmd_buf * buf,
        float * rect,
        unsigned int flags,
        float * color,
        const char * text)
{
        struct nbi_font * font = ctx->font;
        nbr_text_(ctx, buf, font, rect, flags, color, text, 0);
}


short
nbr_clamp_f32_to_i16(float x) {
        return x < 32767.0f ? (x > -32768.0f ? (short)x : -32768) : 32767;
}


void nbr_scissor_set(struct nbi_cmd_buf * buf, float * rect) {
        buf->cmds[buf->cmd_count++] = (struct nb_render_cmd) {
                .type = NB_RENDER_CMD_TYPE_SCISSOR,
                .data.clip_rect[0] = nbr_clamp_f32_to_i16(rect[0]),
                .data.clip_rect[1] = nbr_clamp_f32_to_i16(rect[1]),
                .data.clip_rect[2] = nbr_clamp_f32_to_i16(rect[2]),
                .data.clip_rect[3] = nbr_clamp_f32_to_i16(rect[3]),
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


nb_result
nb_get_render_data(
        struct nb_renderer_ctx * ctx,
        struct nb_render_data * data)
{
        if(!ctx || !data) {
                NB_ASSERT(!"NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        data->vtx = ctx->vtx_buf.v;
        data->vtx_count = ctx->vtx_buf.v_count;
        data->idx = ctx->vtx_buf.i;
        data->idx_count = ctx->vtx_buf.i_count;
        data->cmd_list_count = 0;

        unsigned int count = ctx->cmds_submit_count;
        unsigned int i;

        for(i = 0; i < count; ++i) {
                struct nbi_cmd_buf *cmd_buf = ctx->submited_cmds[i];

                if (data->cmd_list_count >= NB_ARR_COUNT(data->cmd_lists)) {
                        NB_ASSERT(!"Cmd list array full!");
                        break;
                }

                struct nb_render_cmd_list *list;
                list = data->cmd_lists + data->cmd_list_count;

                data->cmd_list_count++;

                list->cmds = cmd_buf->cmds;
                list->count = cmd_buf->cmd_count;
        }

        return NB_OK;
}

nb_result
nb_debug_set_font(
        struct nb_renderer_ctx * ctx,
        unsigned int idx)
{
        NB_ASSERT(ctx);
        NB_ASSERT(idx < ctx->font_count);

        ctx->debug_font_next = ctx->fonts + idx;

        return NB_OK;
}


/* -------------------------------------------------------------- Lifetime -- */


nb_result
nbr_ctx_create(
        nbr_ctx_t *c)
{
        if (!c) {
                NB_ASSERT(!"NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        struct nb_renderer_ctx * new_ctx = NB_ALLOC(sizeof(*new_ctx));

        if (!new_ctx) {
                NB_ASSERT(!"NB_FAIL");
                goto CTX_CLEANUP_AND_FAIL;
        }

        memset(new_ctx, 0, sizeof(*new_ctx));

        new_ctx->vtx_buf.v = NB_ALLOC(65536 * sizeof(float));
        new_ctx->vtx_buf.i = NB_ALLOC(65536 * sizeof(unsigned short));

        if(!new_ctx->vtx_buf.v || !new_ctx->vtx_buf.i) {
                NB_ASSERT(!"NB_FAIL");
                goto CTX_CLEANUP_AND_FAIL;
        }

        struct nbi_font_info { unsigned char * ttf; float height; };
        struct nbi_font_info fi[] = {
                { NB_OPEN_SANS_TTF, 16.0f, },
                { NB_PROGGY_TTF, 11.0f, },
        };

        new_ctx->font_count = NB_ARR_COUNT(fi);
        if(new_ctx->font_count > NB_ARR_COUNT(new_ctx->fonts)) {
                NB_ASSERT(!"More Fonts that storage, increase size");
                new_ctx->font_count = NB_ARR_COUNT(new_ctx->fonts);
        }

        int i;
        int f_count = new_ctx->font_count;

        for(i = 0; i < f_count; i++) {
                nbi_font_init(new_ctx->fonts + i, fi[i].ttf, fi[i].height);
        }

        new_ctx->font = new_ctx->fonts;

        *c = new_ctx;

        return NB_OK;

        /* Failed to create context, most likely allocation failure. */
        CTX_CLEANUP_AND_FAIL:

        if (new_ctx && new_ctx->vtx_buf.i) {
                NB_FREE(new_ctx->vtx_buf.i);
        }

        if (new_ctx && new_ctx->vtx_buf.v) {
                NB_FREE(new_ctx->vtx_buf.v);
        }

        if (new_ctx) {
                NB_FREE(new_ctx);
        }

        return NB_FAIL;
}


nb_result
nbr_ctx_destroy(
        nbr_ctx_t *c)
{
        if (!c) {
                NB_ASSERT(!"NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        return NB_FAIL;
}


nb_result
nbr_frame_begin(
        struct nb_renderer_ctx * ctx)
{
        if(!ctx) {
                NB_ASSERT(!"NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        ctx->vtx_buf.v_count = 0;
        ctx->vtx_buf.i_count = 0;

        /* clear cmds */
        unsigned int count = ctx->cmds_count;
        unsigned int i;

        for(i = 0; i < count; ++i) {
                ctx->cmds[i].cmd_count = 0;
        }

        ctx->cmds_count = 0;

        return NB_OK;
}


nb_result
nbr_frame_submit(
        struct nb_renderer_ctx * ctx,
        struct nbi_cmd_buf ** cmd_bufs,
        int cmd_buf_count)
{
        /* validate */
        if(!ctx) {
                NB_ASSERT(!"NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        if(cmd_buf_count > (int)NB_ARR_COUNT(ctx->submited_cmds)) {
                NB_ASSERT(!"NB_FAIL - To many cmd buffers submitted");
                return NB_FAIL;
        }

        if(cmd_buf_count > 0 && cmd_bufs == 0) {
                NB_ASSERT(!"NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        /* push cmds into queue */
        int i;
        for(i = 0; i < cmd_buf_count; ++i) {
                ctx->submited_cmds[i] = cmd_bufs[i];
        }

        ctx->cmds_submit_count = cmd_buf_count;

        return NB_OK;
}


/* ---------------------------------------------------------------- Config -- */


#undef NB_ASSERT
#undef NB_ALLOC
#undef NB_FREE
#undef NB_ZERO_MEM
#undef NB_ARR_COUNT
#undef NB_ARRAY_DATA


#endif
#endif
