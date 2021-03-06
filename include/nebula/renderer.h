#ifndef NEBULA_RENDERER_INCLUDED
#define NEBULA_RENDERER_INCLUDED


#include <nebula/core.h>
#include "stb_truetype.h"


#ifndef NBR_INDEX_SIZE
#define NBR_INDEX_SIZE 16
#endif


#if NBR_INDEX_SIZE == 8
#define NBR_VERTEX_COUNT_MAX 0xFF
typedef uint8_t nbr_idx;
#elif NBR_INDEX_SIZE == 16
#define NBR_VERTEX_COUNT_MAX 0xFFFF
typedef uint16_t nbr_idx;
#elif NBR_INDEX_SIZE == 32
#define NBR_VERTEX_COUNT_MAX 0xFFFFFFFF
typedef uint32_t nbr_idx;
#else
#error "Nebula: NBR_INDEX_SIZE must be 8, 16 or 32 bytes!"
#endif


#define NBR_FONT_COUNT_MAX 16
#define NB_TAU 6.2831853071


typedef enum nb_text_align {
        NB_TEXT_ALIGN_LEFT = 0,
        NB_TEXT_ALIGN_RIGHT = 1,
        NB_TEXT_ALIGN_CENTER = 2,

        _NB_TEXT_ALIGN_BIT_MASK = 3,
} nb_text_align;


typedef enum nbr_cmd_type {
        NBR_CMD_TYPE_TRIANGLES = 0,
        NBR_CMD_TYPE_LINES = 1,
        NBR_CMD_TYPE_SCISSOR = 2,
} nbr_cmd_type;


struct nbr_cmd_elem {
        uint32_t offset;
        uint32_t count;
};


union nbr_cmd_data {
        struct nbr_cmd_elem elem;
        int16_t clip_rect[4];
};


struct nbr_cmd {
        uint32_t type;
        union nbr_cmd_data data;
};


struct nbr_vtx {
        float x, y;
        float u, v;
        uint32_t c;
};


struct nbr_cmd_limits {
        uint32_t cmd_count_max;
        uint32_t vtx_count_max;
        uint32_t idx_count_max;
};


struct nbr_vtx_buf {
        struct nbr_vtx *vtx;
        uint32_t vtx_count;
        uint32_t vtx_count_max;

        nbr_idx *idx;
        uint32_t idx_count;
        uint32_t idx_count_max;
};


struct nbr_cmd_buf {
        struct nbr_cmd *cmds;
        uint32_t cmd_count;
        uint32_t cmd_count_max;

        struct nbr_vtx_buf vtx_buf;
};


struct nbr_draw_data {
        struct nbr_cmd_buf **cmd_bufs;
        uint32_t cmd_buf_count;
};


struct nb_font_tex {
        uint8_t *mem;
        uint32_t width;
};


struct nbi_font_range {
        uint32_t start;
        uint32_t end;
};


struct nbi_font {
        struct nb_font_tex tex;

        stbtt_packedchar *range_data[4];
        struct nbi_font_range ranges[4];
        uint32_t range_count;

        float height;
        float ascent;
        float space_width;
};


typedef struct nb_renderer_ctx * nbr_ctx_t;

struct nb_renderer_ctx {
        struct nbi_font fonts[NBR_FONT_COUNT_MAX];
        uint32_t font_count;
        struct nbi_font *font;
        struct nbi_font *debug_font_next;

        uint32_t width, height;
};


/* -------------------------------------------------------------- Lifetime -- */


nb_result
nbr_ctx_create(
        nbr_ctx_t *out_ctx);


nb_result
nbr_ctx_destroy(
        nbr_ctx_t *c);


/* ----------------------------------------------------------------- Fonts -- */


uint32_t
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
        uint32_t idx);


/* DEBUG!! */
uint32_t
nb_debug_get_font(
        struct nb_renderer_ctx *ctx);


/* ------------------------------------------------------- Render Commands -- */


uint32_t
nbr_cmd_buf_get_size(struct nbr_cmd_limits lim);


nb_result
nbr_cmd_buf_init(struct nbr_cmd_buf **out_buf, struct nbr_cmd_limits lim, void *mem);


void
nbr_cmd_buf_clear(struct nbr_cmd_buf *buf);


void
nbr_cmd_buf_array_clear(struct nbr_cmd_buf **bufs, uint32_t buf_count);


void
nbr_box(
        struct nb_renderer_ctx *ctx,        /* required */
        struct nbr_cmd_buf *buf,           /* required */
        struct nb_rect rect,
        uint32_t color,
        uint32_t radius);


void
nbr_line(
        struct nb_renderer_ctx *ctx,
        struct nbr_cmd_buf * buf,
        float * p,
        float * q,
        uint32_t color);


void
nbr_bez(
        struct nb_renderer_ctx *ctx,
        struct nbr_cmd_buf * buf,
        float * p0,
        float * p1,
        float * p2,
        float * p3,
        uint32_t color);


enum nbr_text_flags {
        NBI_TEXT_FLAGS_CURSOR = 1 << 15,
        NBI_TEXT_FLAGS_WRAP = 1 << 14,
        NBI_TEXT_FLAGS_TERM = 1 << 13,
};


void
nbr_get_text_size(
        struct nb_renderer_ctx * ctx,
        float width,
        uint32_t flags,
        const char *str,
        float *out_size);


void
nbr_text(
        struct nb_renderer_ctx *ctx,
        struct nbr_cmd_buf *buf,
        struct nb_rect pos,
        uint32_t flags,
        uint32_t color,
        const char *str);


void
nbr_scissor_set(
        struct nbr_cmd_buf *buf,
        struct nb_rect rect);


void
nbr_scissor_clear(
        struct nbr_cmd_buf *buf);


/* ---------------------------------------------------------- Render State -- */


nb_result
nbr_viewport_set(
        nbr_ctx_t ctx,
        uint32_t width,
        uint32_t height);


nb_result
nbr_viewport_get(
        nbr_ctx_t ctx,
        uint32_t *width,
        uint32_t *height);


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


/* ----------------------------------------------------------- Text / Font -- */


static uint32_t
nbi_get_font_range_idx(
        struct nbi_font * font,
        uint32_t cp)
{
        uint32_t result = font->range_count;
        uint32_t i;

        for(i = 0; i < font->range_count; i++) {
                struct nbi_font_range * range = font->ranges + i;
                if(cp >= range->start && cp < range->end) {
                        result = i;
                        break;
                }
        }
        return result;
}


static uint32_t
nbi_char_valid(struct nbi_font * font, uint32_t cp) {
        uint32_t range_idx = nbi_get_font_range_idx(font, cp);
        uint32_t result = range_idx < font->range_count ? 1 : 0;
        return result;
}


static void
nbi_get_glyph_quad(
        struct nbi_font * font,
        uint32_t cp,
        float * x,
        float * y,
        stbtt_aligned_quad * q)
{
        /*cp = 0xF588;*/

        uint32_t range_idx = nbi_get_font_range_idx(font, cp);
        if(range_idx < font->range_count) {
                stbtt_packedchar * data = font->range_data[range_idx];
                uint32_t size = font->tex.width;
                int glyph = (int)(cp - font->ranges[range_idx].start);
                stbtt_GetPackedQuad(data, size, size, glyph, x, y, q, 1);
        }
}

static float
nbi_get_glyph_width(struct nbi_font * font, uint32_t cp) {
        float result = 0.0f;
        if(nbi_char_valid(font, cp)) {
                float y = 0.0f;
                stbtt_aligned_quad q;
                nbi_get_glyph_quad(font, cp, &result, &y, &q);
        }
        return result;
}


uint32_t
nb_debug_get_font(
        struct nb_renderer_ctx * ctx)
{
        NB_ASSERT(ctx);

        struct nbi_font * font = ctx->debug_font_next;
        if(!font) {
                font = ctx->font;
        }
        NB_ASSERT(font);

        uint32_t result = (uint32_t)(font - ctx->fonts);
        NB_ASSERT(result < ctx->font_count);
        return result;
}


static void
nbi_push_font_range(
        struct nbi_font *font,
        stbtt_pack_context *stbtt,
        uint8_t *ttf,
        uint32_t start,
        uint32_t end,
        float height)
{
        if(font->range_count < NB_ARR_COUNT(font->ranges)) {
                uint32_t idx = font->range_count++;
                struct nbi_font_range *range = font->ranges + idx;
                range->start = start;
                range->end = end;

                uint32_t char_count = range->end - range->start;
                font->range_data[idx] = NB_ALLOC(sizeof(stbtt_packedchar) * char_count);
                stbtt_PackFontRange(stbtt, ttf, 0, height, range->start, char_count, font->range_data[idx]);
        }
        else {
                NB_ASSERT(!"nbi_push_font_range: font full!");
        }
}


static void
nbi_font_init(
        struct nbi_font *font,
        uint8_t *ttf,
        float height)
{
        font->tex.width = 512;
        font->tex.mem = NB_ALLOC(font->tex.width * font->tex.width);

        stbtt_fontinfo info;
        stbtt_InitFont(&info, ttf, 0);

        int ascent, descent, line_gap;
        stbtt_GetFontVMetrics(&info, &ascent, &descent, &line_gap);

        float scale = stbtt_ScaleForPixelHeight(&info, height);

        font->height = height;
        font->ascent = scale * (float)ascent;

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
nbi_push_vtx_uv(
        struct nbr_vtx_buf *buf,
        float x,
        float y,
        float u,
        float v,
        uint32_t c)
{
        if(buf->vtx_count < buf->vtx_count_max) {
                uint32_t i = buf->vtx_count++;
                buf->vtx[i].x = x;
                buf->vtx[i].y = y;
                buf->vtx[i].u = u;
                buf->vtx[i].v = v;
                buf->vtx[i].c = c;
        }
        else {
                NB_ASSERT(!"nbi_push_vtx_uv: vtx buf full!");
        }
}


static void
nbi_push_vtx(
        struct nbr_vtx_buf *buf,
        float x,
        float y,
        uint32_t c)
{
        nbi_push_vtx_uv(buf, x, y, 0.0f, 0.0f, c);
}


static void
nbi_push_idx(struct nbr_vtx_buf *buf, nbr_idx idx) {
        if(buf->idx_count < buf->idx_count_max) {
                buf->idx[buf->idx_count++] = idx;
        }
        else {
                NB_ASSERT(!"nbi_push_idx: vtx buf full!");
        }
}

static void
nbi_push_quad_idxs(
        struct nbr_vtx_buf *buf,
        nbr_idx top_lft,
        nbr_idx bot_lft,
        nbr_idx bot_rgt,
        nbr_idx top_rgt)
{
        if((buf->idx_count + 6) <= buf->idx_count_max) {
                buf->idx[buf->idx_count++] = top_lft;
                buf->idx[buf->idx_count++] = bot_lft;
                buf->idx[buf->idx_count++] = bot_rgt;

                buf->idx[buf->idx_count++] = top_lft;
                buf->idx[buf->idx_count++] = bot_rgt;
                buf->idx[buf->idx_count++] = top_rgt;
        }
        else {
                NB_ASSERT(!"nbi_push_quad_idxs: vtx buf full!");
        }
}

static nbr_idx
nbi_push_quad(
        struct nbr_vtx_buf *buf,
        nbr_idx vtx,
        float *rect,
        uint32_t color)
{
        nbi_push_quad_idxs(buf, vtx, vtx + 1, vtx + 2, vtx + 3);
        nbi_push_vtx(buf, rect[0], rect[1], color);
        nbi_push_vtx(buf, rect[0], rect[1] + rect[3], color);
        nbi_push_vtx(buf, rect[0] + rect[2], rect[1] + rect[3], color);
        nbi_push_vtx(buf, rect[0] + rect[2], rect[1], color);
        return 4;
}

static uint32_t
nbi_push_round_corner(
        struct nbr_vtx_buf *buf,
        float px,
        float py,
        uint32_t color,
        nbr_idx seg_count,
        uint32_t radius, float angle)
{
        uint32_t result = 0;
        if(seg_count) {
                nbr_idx i;

                nbr_idx vtx = (nbr_idx)buf->vtx_count;

                for(i = 0; i < seg_count; i++) {
                        nbi_push_idx(buf, vtx);
                        nbi_push_idx(buf, vtx + (i + 1));
                        nbi_push_idx(buf, vtx + (i + 2));
                }

                nbi_push_vtx(buf, px, py, color);

                float d_angle = ((float)NB_TAU * 0.25f) / (float)seg_count;

                for(i = 0; i < (seg_count + 1); i++) {
                        float t = angle + d_angle * (float)i;
                        float x = px + cosf(t) * (float)radius;
                        float y = py - sinf(t) * (float)radius;
                        nbi_push_vtx(buf, x, y, color);
                }

                result = seg_count + 2;
        }
        else {
                NB_ASSERT(!"nbi_push_round_corner: trying to push a corner with no segments!");
        }
        return result;
}


uint32_t
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

        uint32_t i;
        for(i = 0; i < ctx->font_count; i++) {
                tex_list[i] = ctx->fonts[i].tex;
        }

        return NB_OK;
}


static uint32_t
nbi_decode_utf8_cp(
        char *utf8,
        uint32_t *cp)
{
        *cp = 0;

        uint8_t *it = (uint8_t *)utf8;
        uint8_t byte = *it++;
        if((byte & 0x80) == 0) {
                *cp = (uint32_t)byte;
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

        uint32_t result = (uint32_t)((char *)it - utf8);
        return result;
}


/* ------------------------------------------------------- Render Commands -- */


uint32_t
nbr_cmd_buf_get_size(struct nbr_cmd_limits lim) {
        uint32_t result = sizeof(struct nbr_cmd_buf);
        result += sizeof(struct nbr_cmd) * lim.cmd_count_max;
        result += sizeof(struct nbr_vtx) * lim.vtx_count_max;
        result += sizeof(nbr_idx) * lim.idx_count_max;
        return result;
}


nb_result
nbr_cmd_buf_init(struct nbr_cmd_buf **out_buf, struct nbr_cmd_limits lim, void *mem) {
        nb_result result = NB_OK;
        if(out_buf && mem) {
                uint8_t *ptr = (uint8_t *)mem;

                struct nbr_cmd_buf *buf = (struct nbr_cmd_buf *)ptr;
                ptr += sizeof(struct nbr_cmd_buf);

                buf->cmds = (struct nbr_cmd *)ptr;
                ptr += sizeof(struct nbr_cmd) * lim.cmd_count_max;
                buf->cmd_count = 0;
                buf->cmd_count_max = lim.cmd_count_max;

                buf->vtx_buf.vtx = (struct nbr_vtx *)ptr;
                ptr += sizeof(struct nbr_vtx) * lim.vtx_count_max;
                buf->vtx_buf.vtx_count = 0;
                buf->vtx_buf.vtx_count_max = lim.vtx_count_max;

                buf->vtx_buf.idx = (nbr_idx *)ptr;
                ptr += sizeof(nbr_idx) * lim.idx_count_max;
                buf->vtx_buf.idx_count = 0;
                buf->vtx_buf.idx_count_max = lim.idx_count_max;

                *out_buf = buf;
        }
        else {
                NB_ASSERT(!"nbr_cmd_buf_init: NB_INVALID_PARAMS");
                result = NB_INVALID_PARAMS;
        }
        return result;
}


void
nbr_cmd_buf_array_clear(struct nbr_cmd_buf **bufs, uint32_t buf_count) {
        if(bufs && buf_count) {
                uint32_t i;
                for(i = 0; i < buf_count; i++) {
                        struct nbr_cmd_buf *buf = bufs[i];
                        if(buf) {
                                buf->cmd_count = 0;
                                buf->vtx_buf.vtx_count = 0;
                                buf->vtx_buf.idx_count = 0;
                        }
                        else {
                                NB_ASSERT(!"nbr_cmd_buf_array_clear: buf is null!");
                        }
                }
        }
}


void
nbr_cmd_buf_clear(struct nbr_cmd_buf *buf) {
        if(buf) {
                nbr_cmd_buf_array_clear(&buf, 1);
        }
}


static struct nbr_cmd *
nbi_cmd_push(struct nbr_cmd_buf *buf) {
        struct nbr_cmd *result = 0;
        if(buf) {
                if(buf->cmd_count < buf->cmd_count_max) {
                        result = buf->cmds + buf->cmd_count++;
                        struct nbr_cmd empty_cmd = { 0 };
                        *result = empty_cmd;
                }
                else {
                        NB_ASSERT(!"nbi_cmd_push: cmd buf full!");
                }
        }
        else {
                NB_ASSERT(!"nbi_cmd_push: cmd buf is null!");
        }
        return result;
}


static struct nbr_cmd *
nbi_cmd_begin(
        struct nbr_cmd_buf * buf,
        struct nbr_vtx_buf * data,
        uint32_t type,
        nbr_idx * vtx)
{
        struct nbr_cmd *result = nbi_cmd_push(buf);
        if(result) {
                result->type = type;
                result->data.elem.offset = data->idx_count;

                if(vtx) {
                        *vtx = (nbr_idx)data->vtx_count;
                }
        }
        return result;
}

static void
nbi_cmd_end(struct nbr_vtx_buf *data, struct nbr_cmd *cmd) {
        if(cmd) {
                if(data) {
                        cmd->data.elem.count = data->idx_count - cmd->data.elem.offset;
                }
                else {
                        NB_ASSERT(!"nbi_cmd_end: data cannot be null!");
                }
        }
}


void
nbr_box(
        struct nb_renderer_ctx *ctx,
        struct nbr_cmd_buf *buf,
        struct nb_rect rec,
        uint32_t color,
        uint32_t radius)
{
        (void)ctx;

        struct nbr_vtx_buf *data = &buf->vtx_buf;

        nbr_idx vtx;
        struct nbr_cmd *cmd = nbi_cmd_begin(buf, data, NBR_CMD_TYPE_TRIANGLES, &vtx);

        float rect[4];
        rect[0] = (float)rec.x; rect[1] = (float)rec.y;
        rect[2] = (float)rec.w; rect[3] = (float)rec.h;

        uint32_t extent_min = (rec.w > 0 && rec.h > 0) ? (uint32_t)((rec.w < rec.h ? rec.w : rec.h) / 2) : 0;
        if(radius > extent_min) {
                radius = extent_min;
        }

        nbr_idx subdivs = radius >= 1 ? (nbr_idx)radius : 0;
        if(subdivs > 7) {
                subdivs = 7;
        }

        /* sharp corners - shortcut */
        if(!subdivs) {
                nbi_push_quad(data, vtx, rect, color);
                nbi_cmd_end(data, cmd);
                return;
        }

        nbr_idx cv0 = vtx;
        nbr_idx cv1 = vtx + (subdivs + 2);
        nbr_idx cv2 = vtx + (subdivs + 2) * 2;
        nbr_idx cv3 = vtx + (subdivs + 2) * 3;

        nbi_push_quad_idxs(data, cv1 + 1, cv1, cv0, cv0 + (subdivs + 1));
        nbi_push_quad_idxs(data, cv2 + 1, cv2, cv1, cv1 + (subdivs + 1));
        nbi_push_quad_idxs(data, cv3 + 1, cv3, cv2, cv2 + (subdivs + 1));
        nbi_push_quad_idxs(data, cv0 + 1, cv0, cv3, cv3 + (subdivs + 1));
        nbi_push_quad_idxs(data, cv1, cv2, cv3, cv0);

        float cx, cy;

        cx = rect[0] + rect[2] - (float)radius;
        cy = rect[1] + (float)radius;
        nbi_push_round_corner(data, cx, cy, color, subdivs, radius, NB_TAU);

        cx = rect[0] + (float)radius;
        cy = rect[1] + (float)radius;
        nbi_push_round_corner(data, cx, cy, color, subdivs, radius, NB_TAU * 0.25f);

        cx = rect[0] + (float)radius;
        cy = rect[1] + rect[3] - (float)radius;
        nbi_push_round_corner(data, cx, cy, color, subdivs, radius, NB_TAU * 0.5f);

        cx = rect[0] + rect[2] - (float)radius;
        cy = rect[1] + rect[3] - (float)radius;
        nbi_push_round_corner(data, cx, cy, color, subdivs, radius, NB_TAU * 0.75f);

        nbi_cmd_end(data, cmd);
}


void
nbr_line(
        struct nb_renderer_ctx *ctx,
        struct nbr_cmd_buf *buf,
        float *p,
        float *q,
        uint32_t color)
{
        (void)ctx;

        struct nbr_vtx_buf *data = &buf->vtx_buf;

        nbr_idx vtx;
        struct nbr_cmd *cmd = nbi_cmd_begin(buf, data, NBR_CMD_TYPE_LINES, &vtx);

        nbi_push_idx(data, vtx);
        nbi_push_idx(data, vtx + 1);

        nbi_push_vtx(data, p[0], p[1], color);
        nbi_push_vtx(data, q[0], q[1], color);

        nbi_cmd_end(data, cmd);
}


void
nbr_bez(
        struct nb_renderer_ctx *ctx,
        struct nbr_cmd_buf *buf,
        float *p0,
        float *p1,
        float *p2,
        float *p3,
        uint32_t color)
{
        (void)ctx;

        struct nbr_vtx_buf * data = &buf->vtx_buf;

        nbr_idx vtx;
        struct nbr_cmd * cmd = nbi_cmd_begin(buf, data, NBR_CMD_TYPE_LINES, &vtx);

        nbr_idx seg_count = 16;
        nbr_idx i;

        for(i = 0; i < seg_count; i++) {
                nbi_push_idx(data, vtx + i);
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
        struct nbi_font *font;

        float start_x;
        float end_x;
        float max_x;
        float x;
        float y;
        float space;

        uint32_t vtx_start;
        uint32_t align_type;
};


static void
nbi_line_adv(struct nbr_vtx_buf * buf, struct nbi_text_out * out) {
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
                        uint32_t i;
                        for(i = out->vtx_start; i < buf->vtx_count; i++) {
                                buf->vtx[i].x += offset;
                        }

                        out->vtx_start = buf->vtx_count;
                }
        }

        if(out->x > out->max_x) {
                out->max_x = out->x;
        }

        out->x = out->start_x;
        out->y += out->font->height;
        out->space = 0.0f;
}


void
nbr_text_(
        struct nb_renderer_ctx *ctx,
        struct nbr_cmd_buf *buf,
        struct nbi_font *font,
        struct nb_rect rect,
        uint32_t flags,
        uint32_t color,
        const char *text,
        float *out_size)
{
        (void)ctx;

        if(!text) {
                if(out_size) {
                        out_size[0] = 0.0f;
                        out_size[1] = font->height;
                }

                return;
        }

        uint32_t wrap = flags & NBI_TEXT_FLAGS_WRAP;
        uint32_t term_tag = flags & NBI_TEXT_FLAGS_TERM;

        struct nbi_text_out out = { 0 };
        out.font = font;
        out.start_x = (float)rect.x;
        out.end_x = (float)rect.x + (float)rect.w;
        out.max_x = (float)rect.x;
        out.x = (float)rect.x;
        out.y = (float)rect.y + font->ascent;
        out.align_type = flags & _NB_TEXT_ALIGN_BIT_MASK;

        struct nbr_vtx_buf * data = 0;
        nbr_idx vtx = 0;
        struct nbr_cmd * cmd = 0;
        if(buf) {
                data = &buf->vtx_buf;
                cmd = nbi_cmd_begin(buf, data, NBR_CMD_TYPE_TRIANGLES, &vtx);

                out.vtx_start = data->vtx_count;
        }

        char * it = (char *)text;
        while(*it) {
                uint32_t cp;
                uint32_t cp_size = nbi_decode_utf8_cp(it, &cp);

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
                        uint32_t i;

                        char * word = it;
                        uint32_t word_size = 0;
                        while(*it && *it != '\n' && *it != ' ') {
                                if(term_tag && strncmp(it, "##", 2) == 0) {
                                        word_size = (uint32_t)(it - word);
                                        it += strlen(it);
                                        break;
                                }

                                uint32_t word_cp;
                                uint32_t word_cp_size = nbi_decode_utf8_cp(it, &word_cp);
                                if(nbi_char_valid(out.font, word_cp)) {
                                        it += word_cp_size;
                                        word_size = (uint32_t)(it - word);
                                }
                                else {
                                        word_size = (uint32_t)(it - word);
                                        break;
                                }
                        }


                        if(wrap && out.x > out.start_x) {
                                float word_end = out.x + out.space;
                                for(i = 0; i < word_size;) {
                                        uint32_t word_cp;
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
                                uint32_t word_cp;
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
                        uint32_t cursor_on = 1; /* ((uint32_t)(buf->ctx->state.text_cursor_time * 2.0f) & 1) == 0; */
                        if(cursor_on) {
                                float cursor_rect[4];
                                cursor_rect[0] = out.x;
                                cursor_rect[1] = out.y - font->ascent;
                                cursor_rect[2] = cursor_width;
                                cursor_rect[3] = font->height;
                                vtx += nbi_push_quad(data, vtx, cursor_rect, color);
                        }
                }

                out.x += cursor_width;
        }

        nbi_line_adv(data, &out);

        if(out_size) {
                out_size[0] = out.max_x - rect.x;
                out_size[1] = out.y - (rect.y + font->ascent);
        }

        if(data) {
                nbi_cmd_end(data, cmd);
        }
}

void
nbr_get_text_size(
        struct nb_renderer_ctx * ctx,
        float width,
        uint32_t flags,
        const char *text,
        float *out_size)
{
        if(!ctx || !out_size) {
                NB_ASSERT(!"Invalid params");
                return;
        }

        struct nbi_font * font = ctx->font;
        struct nb_rect rect;
        rect.x = 0; rect.y = 0; rect.w = (int)width; rect.h = 0;
        nbr_text_(ctx, 0, font, rect, flags, 0, text, out_size);
}


void
nbr_text(
        struct nb_renderer_ctx *ctx,
        struct nbr_cmd_buf *buf,
        struct nb_rect rect,
        uint32_t flags,
        uint32_t color,
        const char *text)
{
        struct nbi_font *font = ctx->font;
        nbr_text_(ctx, buf, font, rect, flags, color, text, 0);
}


void
nbr_scissor_set(
        struct nbr_cmd_buf *buf,
        struct nb_rect rect)
{
        struct nbr_cmd *cmd = nbi_cmd_push(buf);
        if(cmd) {
                cmd->type = NBR_CMD_TYPE_SCISSOR;
                cmd->data.clip_rect[0] = (int16_t)rect.x;
                cmd->data.clip_rect[1] = (int16_t)rect.y;
                cmd->data.clip_rect[2] = (int16_t)rect.w;
                cmd->data.clip_rect[3] = (int16_t)rect.h;
        }
}


void
nbr_scissor_clear(struct nbr_cmd_buf *buf) {
        struct nbr_cmd *cmd = nbi_cmd_push(buf);
        if(cmd) {
                cmd->type = NBR_CMD_TYPE_SCISSOR;
                cmd->data.clip_rect[0] = 0;
                cmd->data.clip_rect[1] = 0;
                cmd->data.clip_rect[2] = 0x7FFF;
                cmd->data.clip_rect[3] = 0x7FFF;
        }
}


nb_result
nb_debug_set_font(
        struct nb_renderer_ctx * ctx,
        uint32_t idx)
{
        NB_ASSERT(ctx);
        NB_ASSERT(idx < ctx->font_count);

        ctx->debug_font_next = ctx->fonts + idx;

        return NB_OK;
}


/* -------------------------------------------------------------- Lifetime -- */


nb_result
nbr_ctx_create(
        nbr_ctx_t *out_ctx)
{
        if (!out_ctx) {
                NB_ASSERT(!"NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        struct nb_renderer_ctx *ctx = NB_ALLOC(sizeof(*ctx));

        if (!ctx) {
                NB_ASSERT(!"NB_FAIL");
                goto CTX_CLEANUP_AND_FAIL;
        }

        NB_ZERO_MEM(ctx);

        struct nbi_font_info { uint8_t * ttf; float height; };
        struct nbi_font_info fi[] = {
                { NB_OPEN_SANS_TTF, 16.0f, },
                { NB_PROGGY_TTF, 11.0f, },
        };

        ctx->font_count = NB_ARR_COUNT(fi);
        if(ctx->font_count > NB_ARR_COUNT(ctx->fonts)) {
                NB_ASSERT(!"More Fonts that storage, increase size");
                ctx->font_count = NB_ARR_COUNT(ctx->fonts);
        }

        uint32_t i;
        for(i = 0; i < ctx->font_count; i++) {
                nbi_font_init(ctx->fonts + i, fi[i].ttf, fi[i].height);
        }

        ctx->font = ctx->fonts;

        *out_ctx = ctx;

        return NB_OK;

        /* Failed to create context, most likely allocation failure. */
        CTX_CLEANUP_AND_FAIL:

        if (ctx) {
                NB_FREE(ctx);
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


/* ---------------------------------------------------------- Render State -- */


nb_result
nbr_viewport_set(
        nbr_ctx_t ctx,
        uint32_t width,
        uint32_t height)
{
        if(!ctx) {
                return NB_INVALID_PARAMS;
        }

        ctx->width = width;
        ctx->height = height;

        return NB_OK;
}


nb_result
nbr_viewport_get(
        nbr_ctx_t ctx,
        uint32_t *width,
        uint32_t *height)
{
        if(!ctx) {
                return NB_INVALID_PARAMS;
        }

        if(width) {
                *width = ctx->width;
        }

        if(height) {
                *height = ctx->height;
        }

        return NB_OK;
}


/* ---------------------------------------------------------------- Config -- */


#undef NB_ASSERT
#undef NB_ALLOC
#undef NB_FREE
#undef NB_ZERO_MEM
#undef NB_ARR_COUNT


#endif
#endif
