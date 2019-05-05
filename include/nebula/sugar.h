#ifndef NEB_SUGAR_INCLUDED
#define NEB_SUGAR_INCLUDED


#include <nebula/core.h>
#include <nebula/renderer.h>


typedef struct nbs_ctx * nbs_ctx_t;


/* -------------------------------------------------------------- Lifetime -- */


/*
 *  returns `NB_OK` on success
 *  returns `NB_INVALID_PARAMS` if ctx is null
 *  returns `NB_FAIL` if an internal error occured
 */
nb_result
nbs_ctx_create(
        nbs_ctx_t * ctx);


/*
 *  returns `NB_OK` on success
 *  returns `NB_INVALID_PARAMS` if ctx is null
 *  returns `NB_FAIL` if an internal error occured
 */
nb_result
nbs_ctx_destroy(
        nbs_ctx_t * ctx);


/*
 *  returns `NB_OK` on success
 *  returns `NB_FAIL` if an internal error occured
 */
nb_result
nbs_ctx_get_ctx(
        nbs_ctx_t ctx,                      /* required */
        nbc_ctx_t *out_nbc_ctx,             /* optional */
        nbr_ctx_t *out_nbr_ctx);            /* optional */


/* ----------------------------------------------------------------- Frame -- */


/*
 *  returns `NB_OK` on success
 *  returns `NB_INVALID_PARAMS` if ctx is null
 *  returns `NB_FAIL` if an internal error occured
 */
nb_result
nbs_frame_begin(
        nbs_ctx_t ctx);                      /* required */


/*
 *  returns `NB_OK` on success
 *  returns `NB_INVALID_PARAMS` if ctx is null
 *  returns `NB_FAIL` if an internal error occured
 */
nb_result
nbs_frame_end(
        nbs_ctx_t ctx);                     /* required */


/*
 *  returns `NB_OK` on success
 *  returns `NB_INVALID_PARAMS` if ctx is null
 */
nb_result
nbs_get_draw_data(
        nbs_ctx_t ctx,                     /* required */
        struct nbr_draw_data *data);       /* required */


/* -------------------------------------------------------- Window widgets -- */


void *
nbs_window_begin(
        nbs_ctx_t ctx,                      /* required */
        const char * name,                  /* required */
        uint32_t color_hex);


void
nbs_window_end(
        nbs_ctx_t ctx,                      /* required */
        void * win);                        /* required */


/* returns 1 on click, 0 otherwise */
int
nbs_button(
        nbs_ctx_t ctx,                      /* required */
        void * win,                         /* required */
        const char * name);                 /* required */


#endif


/* ================================== IMPL ================================== */


#ifdef NEB_SUGAR_IMPL
#ifndef NEB_SUGAR_IMPL_INCLUDED
#define NEB_SUGAR_IMPL_INCLUDED


#include "nebula_font_open_sans.h"
#include "nebula_font_proggy.h"
#include "nebula_font_awesome.h"

#include <nebula/core.h>
#include <nebula/renderer.h>


struct nb_window {
        uint64_t unique_id;
        int win_idx;
        int cursor;

        struct nb_rect rect;

        struct nbr_cmd_buf *cmd_buf;
};


struct nbs_ctx {
        nbc_ctx_t core_ctx;
        nbr_ctx_t rdr_ctx;

        uint8_t *window_mem;
        struct nbr_cmd_buf *window_bufs[32];
        struct nb_window windows[32];

        struct nbr_cmd_buf *draw_bufs[32];
        uint32_t draw_buf_count;
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


/* ------------------------------------------------------------- Utilities -- */


/* https://stackoverflow.com/questions/2535284/how-can-i-hash-a-string-to-an-int-using-c#13487193 */
static uint64_t
nbi_hash_str(const char *name) {
        uint64_t hash = 5381;
        int c;

        while (c = *name++, c) {
                hash = ((hash << 5) + hash) + c;
        }

        return hash;
}


/* ---------------------------------------------------------- Window Style -- */


#define NB_THEME_WIN_BG_COLD 0x393333FF
#define NB_THEME_WIN_BG_HOVER 0x3C3C39FF
#define NB_THEME_WIN_BG_DRAG 0x212321FF
#define NB_THEME_WIN_CORNER_RADIUS 2
#define NB_THEME_WIN_BORDER_SIZE 1
#define NB_THEME_WIN_PADDING 5
#define NB_THEME_WIN_PADDING_ELE 5
#define NB_THEME_WIN_BORDER_COLOR 0x555555FF
#define NB_THEME_WIN_TITLE_TXT_COLOR 0xEEEEEEFF

#define NB_THEME_BUT_BG_COLD 0x333333FF
#define NB_THEME_BUT_BG_HOVER 0x343434FF
#define NB_THEME_BUT_BG_DOWN 0x252525FF
#define NB_THEME_BUT_CORNER_RADIUS 2
#define NB_THEME_BUT_BORDER_SIZE 1
#define NB_THEME_BUT_BORDER_COLOR 0x666666FF
#define NB_THEME_BUT_TXT_COLOR 0xEFEFEFFF


/* -------------------------------------------------------- Window widgets -- */


nb_bool
nbi_window_search(
        struct nb_window *window_array,     /* required */
        int arr_count,                      /* required - greater than zero */
        uint64_t hash_key,                  /* required - greater than zero */
        struct nb_window **out_win)         /* required */
{
        /* validate */
        if(!window_array || !arr_count || !hash_key) {
                NB_ASSERT(!"NB_INVALID_PARAMS");
                return NB_FALSE;
        }

        /* see if there is a cached version */
        int i;
        for(i = 0; i < arr_count; ++i) {
                if(window_array[i].unique_id == hash_key) {
                        *out_win = &window_array[i];
                        (*out_win)->win_idx = i;

                        return NB_TRUE;
                }
        }

        /* can we create a new one */
        for(i = 0; i < arr_count; ++i) {
                if(window_array[i].unique_id == 0) {
                        /* set some defaults */
                        static int offset = 0;
                        offset += 1;

                        window_array[i].unique_id = hash_key;
                        window_array[i].rect.x = offset * 10;
                        window_array[i].rect.y = offset * 10;
                        window_array[i].rect.w = 100;
                        window_array[i].rect.h = 150;

                        *out_win = &window_array[i];
                        (*out_win)->win_idx = i;

                        return NB_TRUE;
                }
        }

        NB_ASSERT(!"NB_FAIL - Window buffer too small");
        return NB_FALSE;
}


void *
nbs_window_begin(
        nbs_ctx_t ctx,
        const char *name,
        uint32_t color_hex)
{
        (void)color_hex;

        /* validate */
        if(!ctx || !name) {
                NB_ASSERT(!"NB_INVALID_PARAMS");
                return 0;
        }

        /* find or create a new window */
        uint64_t hash_key = nbi_hash_str(name);
        struct nb_window *window = 0;
        uint32_t bgcol = NB_THEME_WIN_BG_COLD;

        nb_bool found = nbi_window_search(
                ctx->windows,
                NB_ARR_COUNT(ctx->windows),
                hash_key,
                &window);

        if(found == NB_FALSE) {
                NB_ASSERT(!"NB_FAIL - Failed to get a window");
                return 0;
        }

        window->cmd_buf = ctx->window_bufs[window->win_idx];

        /* imgui */
        struct nb_collider_desc coll_desc;
        coll_desc.index = window->win_idx;
        coll_desc.unique_id = window->unique_id;
        coll_desc.rect = &window->rect;

        struct nb_interaction inter;
        nbc_collider(ctx->core_ctx, &coll_desc, &inter);

        /* render */
        if(inter.flags & NB_INTERACT_HOVER) {
                bgcol = NB_THEME_WIN_BG_HOVER;
        }

        if(inter.flags & NB_INTERACT_DRAGGED) {
                window->rect.x += (int)inter.delta_x;
                window->rect.y += (int)inter.delta_y;

                int idx = window->win_idx;

                /* re-order cache if a background window was dragged */
                if(idx > 0) {
                        struct nb_window win_cpy = *window;
                        window->unique_id = 0;

                        void *dst = (void*)&ctx->windows[1];
                        void *src = (void*)&ctx->windows[0];
                        size_t size = sizeof(ctx->windows[0]) * idx;

                        memmove(dst, src, size);

                        ctx->windows[0] = win_cpy;
                        window = &ctx->windows[0];
                }

                bgcol = NB_THEME_WIN_BG_DRAG;
        }

        if(inter.flags & NB_INTERACT_CLICKED) {
                bgcol = NB_THEME_WIN_BG_DRAG;
        }

        struct nb_rect wrect = window->rect;

        /* border */
        int bo_rad = NB_THEME_WIN_CORNER_RADIUS + NB_THEME_WIN_BORDER_SIZE;
        struct nb_color bocolor = nb_color_from_int(NB_THEME_WIN_BORDER_COLOR);
        struct nb_rect nbrect = nb_rect_expand(wrect, NB_THEME_WIN_BORDER_SIZE);

        nbr_box(ctx->rdr_ctx, window->cmd_buf, nbrect, bocolor, bo_rad);

        /* body */
        int b_rad = NB_THEME_WIN_CORNER_RADIUS;
        struct nb_color bgcolor = nb_color_from_int(bgcol);

        nbr_box(ctx->rdr_ctx, window->cmd_buf, wrect, bgcolor, b_rad);

        /* title */
        struct nb_rect trect = nb_rect_expand(wrect, -NB_THEME_WIN_PADDING);
        struct nb_color txtc = nb_color_from_int(NB_THEME_WIN_TITLE_TXT_COLOR);

        nbr_scissor_set(window->cmd_buf, trect);
        nbr_text(ctx->rdr_ctx, window->cmd_buf, trect, 0, txtc, name);

        nbr_scissor_clear(window->cmd_buf);

        window->cursor = 30;

        /* return handle */
        return (void*)window;
}


void
nbs_window_end(
        nbs_ctx_t ctx,
        void * pwin)
{
        (void)ctx;
        (void)pwin;
}


int
nbs_button(
        nbs_ctx_t ctx,
        void * pwin,
        const char *name)
{
        if(!ctx || !pwin || !name || strlen(name) == 0) {
                NB_ASSERT(!"NB_INVALID_PARAMS");
                return 0;
        }

        struct nb_window *win = (struct nb_window*)pwin;

        /* build some data */
        uint64_t hash_key = nbi_hash_str(name);

        float txt_size[2];
        uint32_t txt_flags = NB_TEXT_ALIGN_CENTER;
        nbr_get_text_size(ctx->rdr_ctx, (float)win->rect.w, txt_flags, name, txt_size);

        struct nb_rect rect;
        rect.x = win->rect.x + NB_THEME_WIN_PADDING;
        rect.y = win->rect.y + win->cursor;
        rect.w = win->rect.w - (NB_THEME_WIN_PADDING * 2);
        rect.h = (int)txt_size[1];

        win->cursor += rect.h + NB_THEME_WIN_PADDING_ELE;

        uint32_t bgcol = NB_THEME_BUT_BG_COLD;

        /* collider */
        struct nb_collider_desc bcoll;
        bcoll.index = win->win_idx;
        bcoll.unique_id = hash_key;
        bcoll.rect = &rect;

        struct nb_interaction inter;
        nbc_collider(ctx->core_ctx, &bcoll, &inter);

        if(inter.flags & NB_INTERACT_HOVER) {
                bgcol = NB_THEME_BUT_BG_HOVER;
        }
        if(inter.flags & NB_INTERACT_DRAGGED) {
                bgcol = NB_THEME_BUT_BG_DOWN;
        }
        if(inter.flags & NB_INTERACT_CLICKED) {
                bgcol = NB_THEME_BUT_BG_DOWN;
        }

        /* border */
        int bo_rad = NB_THEME_BUT_CORNER_RADIUS + NB_THEME_BUT_BORDER_SIZE;
        struct nb_color bocolor = nb_color_from_int(NB_THEME_BUT_BORDER_COLOR);
        struct nb_rect nbrect = nb_rect_expand(rect, NB_THEME_BUT_BORDER_SIZE);

        nbr_box(ctx->rdr_ctx, win->cmd_buf, nbrect, bocolor, bo_rad);

        /* body */
        int b_rad = NB_THEME_BUT_CORNER_RADIUS;
        struct nb_color bgcolor = nb_color_from_int(bgcol);

        nbr_box(ctx->rdr_ctx, win->cmd_buf, rect, bgcolor, b_rad);

        /* text */
        struct nb_color txtc = nb_color_from_int(NB_THEME_BUT_TXT_COLOR);

        nbr_scissor_set(win->cmd_buf, rect);
        nbr_text(ctx->rdr_ctx, win->cmd_buf, rect, txt_flags, txtc, name);

        nbr_scissor_clear(win->cmd_buf);

        if(inter.flags & NB_INTERACT_CLICKED) {
                return 1;
        };

        return 0;
}



/* --------------------------------------------------------- Other widgets -- */





/* -------------------------------------------------------------- Lifetime -- */


nb_result
nbs_frame_begin(
        nbs_ctx_t ctx)
{
        if(!ctx) {
                NB_ASSERT(!"NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        nb_result ok = NB_OK;

        /* core */
        ok = nbc_frame_begin(ctx->core_ctx);

        if(ok != NB_OK) {
                NB_ASSERT(!"NB_FAIL - Failed to start core frame");
                return NB_FAIL;
        };

        nbr_cmd_buf_array_clear(ctx->window_bufs, NB_ARR_COUNT(ctx->window_bufs));
        ctx->draw_buf_count = 0;

        return NB_OK;
}

nb_result
nbs_frame_end(
        nbs_ctx_t ctx)
{
        if(!ctx) {
                NB_ASSERT(!"NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        nb_result ok = NB_OK;

        /* core frame */
        ok = nbc_frame_submit(ctx->core_ctx);

        if(ok != NB_OK) {
                NB_ASSERT(!"NB_FAIL - Failed to submit core frame");
                return NB_FAIL;
        }

        /* build array of cmd bufs */
        ctx->draw_buf_count = 0;

        int win_count = NB_ARR_COUNT(ctx->windows);
        int i;

        for(i = win_count - 1; i >= 0; --i) {
                if(ctx->windows[i].unique_id > 0) {
                        ctx->draw_bufs[ctx->draw_buf_count] = ctx->windows[i].cmd_buf;
                        ctx->draw_buf_count += 1;

                        if(ctx->draw_buf_count >= (int)NB_ARR_COUNT(ctx->draw_bufs)) {
                                break;
                        }
                }
        }

        return NB_OK;
}

nb_result
nbs_get_draw_data(
        nbs_ctx_t ctx,
        struct nbr_draw_data *data)
{
        if(!ctx || !data) {
                NB_ASSERT(!"NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        data->cmd_bufs = ctx->draw_bufs;
        data->cmd_buf_count = ctx->draw_buf_count;

        return NB_OK;

}

/* -------------------------------------------------------------- Lifetime -- */


nb_result
nbs_ctx_create(
        nbs_ctx_t * ctx)
{
        if(!ctx) {
                NB_ASSERT(!"NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        struct nbs_ctx *new_ctx = NB_ALLOC(sizeof(*new_ctx));
        NB_ZERO_MEM(new_ctx);

        if (!new_ctx) {
                goto CTX_FAIL_CLEANUP;
        }

        nb_result ok = NB_OK;

        new_ctx->core_ctx = 0;
        nbc_ctx_create(&new_ctx->core_ctx);
        NB_ASSERT(new_ctx->core_ctx && "Failed to create core ctx");

        if (ok != NB_OK) {
                goto CTX_FAIL_CLEANUP;
        }

        new_ctx->rdr_ctx = 0;
        ok = nbr_ctx_create(&new_ctx->rdr_ctx);
        NB_ASSERT(new_ctx->rdr_ctx && "Failed to create renderer ctx");

        if (ok != NB_OK) {
                goto CTX_FAIL_CLEANUP;
        }

        struct nbr_cmd_limits cmd_lim = { 4096, 65536, 65536, };
        uint32_t cmd_buf_size = nbr_cmd_buf_get_size(cmd_lim);
        new_ctx->window_mem = NB_ALLOC(cmd_buf_size * NB_ARR_COUNT(new_ctx->window_bufs));

        if(!new_ctx->window_mem) {
                goto CTX_FAIL_CLEANUP;
        }

        uint32_t i;
        uint8_t *ptr = new_ctx->window_mem;
        for(i = 0; i < NB_ARR_COUNT(new_ctx->window_bufs); ++i) {
                nbr_cmd_buf_init(new_ctx->window_bufs + i, cmd_lim, ptr);
                ptr += cmd_buf_size;
        }

        *ctx = new_ctx;

        return NB_OK;

        /* Fail and cleanup */
        CTX_FAIL_CLEANUP:
        NB_ASSERT(!"NBS Failed to create ctx");

        if (new_ctx && new_ctx->core_ctx) {
                nbc_ctx_destroy(&new_ctx->core_ctx);
        }

        if (new_ctx && new_ctx->rdr_ctx) {
                nbr_ctx_destroy(&new_ctx->rdr_ctx);
        }

        if(new_ctx && new_ctx->window_mem) {
                NB_FREE(new_ctx->window_mem);
        }

        if (new_ctx) {
                NB_FREE(new_ctx);
        }

        return NB_FAIL;
}



nb_result
nbs_ctx_destroy(
        nbs_ctx_t * ctx)
{
        if (!ctx && !*ctx) {
                NB_ASSERT(!"NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        struct nbs_ctx *kill_ctx = *ctx;
        NB_FREE(kill_ctx);

        *ctx = 0;

        return NB_OK;
}


nb_result
nbs_ctx_get_ctx(
        nbs_ctx_t octx,
        nbc_ctx_t *out_nbc_ctx,
        nbr_ctx_t *out_nbr_ctx)
{
        if(!octx) {
                NB_ASSERT(!"NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        struct nbs_ctx *ctx = (struct nbs_ctx*)octx;

        if(out_nbc_ctx) {
                *out_nbc_ctx = ctx->core_ctx;
        }

        if(out_nbr_ctx) {
                *out_nbr_ctx = ctx->rdr_ctx;
        }

        return NB_OK;
}


/* ---------------------------------------------------------------- Config -- */


#undef NB_ASSERT
#undef NB_ALLOC
#undef NB_FREE
#undef NB_ZERO_MEM
#undef NB_ARR_COUNT
#undef NB_ARRAY_DATA

#undef NB_THEME_WINDOW_CLICK


#endif
#endif
