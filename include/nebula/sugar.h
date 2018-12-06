#ifndef NEB_SUGAR_INCLUDED
#define NEB_SUGAR_INCLUDED


#include <nebula/core.h>
#include <nebula/renderer.h>


struct nb_window {
        uint64_t unique_id;
        struct nb_rect rect;

        struct nbi_cmd_buf *cmd_buf;
};


struct nb_sugar_ctx {
        struct nb_renderer_ctx rdr_ctx;
        nbc_ctx_t core_ctx;

        struct nb_window windows[32];
};


/* -------------------------------------------------------------- Lifetime -- */


int
nbs_init(
        struct nb_sugar_ctx * ctx);         /* required */


int
nbs_frame_begin(
        struct nb_sugar_ctx *ctx);          /* required */


int
nbs_frame_submit(
        struct nb_sugar_ctx *ctx);          /* required */


/* -------------------------------------------------------- Window widgets -- */


const struct nb_window *
nbs_window_begin(
        struct nb_sugar_ctx *ctx,           /* required */
        const char *name);                  /* required */


void
nbs_window_end(
        struct nb_sugar_ctx *ctx,           /* required */
        const struct nb_window *win);       /* required */


/* returns 1 on click, 0 otherwise */
int
nbs_button(
        struct nb_sugar_ctx *ctx,           /* required */
        const struct nb_window *win,        /* required */
        const char *name);                  /* required */


/* --------------------------------------------------------- Other widgets -- */


#endif


/* ================================== IMPL ================================== */


#ifdef NEB_SUGAR_IMPL
#ifndef NEB_SUGAR_IMPL_INCLUDED
#define NEB_SUGAR_IMPL_INCLUDED


#include "nebula_font_open_sans.h"
#include "nebula_font_proggy.h"
#include "nebula_font_awesome.h"


#define NEB_CORE_IMPL
#include <nebula/core.h>


#define NEB_RENDERER_IMPL
#include <nebula/renderer.h>


/* ---------------------------------------------- Stdlib / Config / Macros -- */
/*
 * Nebula uses stdlib for some things, you can override them here.
 * Most of these macros are duplicated in core.h, renderer.h, sugar.h
 */


#include <assert.h>
#define NB_ASSERT(expr) assert(expr)

#include <stdlib.h>
#define NB_ALLOC(bytes) malloc(bytes)
#define NB_FREE(addr) free(addr)

#include <string.h>
#define NB_ZERO_MEM(ptr) do{memset((ptr), 0, sizeof((ptr)[0]));}while(0)

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


/* -------------------------------------------------------- Window widgets -- */


nb_bool
nbi_window_search(
        struct nb_window *window_array,     /* required */
        int arr_count,                      /* required - greater than zero */
        uint64_t hash_key,                  /* required - greater than zero */
        struct nb_window ** out_win,        /* required */
        int * out_idx)                      /* required */
{
        /* validate */
        if(!window_array || !arr_count || !hash_key) {
                NB_ASSERT(0 && "NB_INVALID_PARAMS");
                return NB_FALSE;
        }

        /* see if there is a cached version */
        int i;
        for(i = 0; i < arr_count; ++i) {
                if(window_array[i].unique_id == hash_key) {
                        *out_idx = i;
                        *out_win = &window_array[i];

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

                        *out_idx = i;
                        *out_win = &window_array[i];

                        return NB_TRUE;
                }
        }

        NB_ASSERT(0 && "NB_FAIL - Window buffer too small");
        return NB_FALSE;
}


const struct nb_window *
nbs_window_begin(
        struct nb_sugar_ctx *ctx,
        const char *name)
{
        if(!ctx || !name) {
                NB_ASSERT(0 && "NB_INVALID_PARAMS");
                return 0;
        }

        /* find or create a new window */
        uint64_t hash_key = nbi_hash_str(name);
        struct nb_window *window = 0;
        int win_idx = 0;
        
        nb_bool found = nbi_window_search(
                ctx->windows,
                NB_ARR_COUNT(ctx->windows),
                hash_key,
                &window,
                &win_idx);

        if(found == NB_FALSE) {
                NB_ASSERT(0 && "NB_FAIL - Failed to get a window");
                return 0;
        }

        nb_result ok = nb_render_cmd_create(&ctx->rdr_ctx, &window->cmd_buf);

        if(ok != NB_OK) {
                NB_ASSERT(0 && "NB_FAIL - Failed creating rdr cmd buffer");
                return 0;
        }

        /* imgui */
        struct nb_collider_desc coll_desc;
        coll_desc.index = win_idx;
        coll_desc.unique_id = window->unique_id;
        coll_desc.rect = &window->rect;

        struct nb_interaction inter;

        nbc_collider(ctx->core_ctx, &coll_desc, &inter);

        /* render */
        float recti[4];
        recti[0] = window->rect.x;
        recti[1] = window->rect.y;
        recti[2] = window->rect.w;
        recti[3] = window->rect.h;

        float color[4];
        color[0] = 1.f;
        color[1] = 0.f;
        color[2] = 0.f;
        color[3] = 1.f;

        if(inter.flags & NB_INTERACT_HOVER) {
                color[1] = 1.f;
        }

        if(inter.flags & NB_INTERACT_DRAGGED) {
                color[2] = 1.f;
                window->rect.x += inter.delta_x;
                window->rect.y += inter.delta_y;

                /* re-order cache if a background window was dragged */
                if(win_idx > 0) {
                        struct nb_window win_cpy = *window;
                        window->unique_id = 0;

                        void *dst = (void*)&ctx->windows[1];
                        void *src = (void*)&ctx->windows[0];
                        size_t size = sizeof(ctx->windows[0]) * (win_idx);

                        memmove(dst, src, size);

                        ctx->windows[0] = win_cpy;
                        window = &ctx->windows[0];
                }
        }

        if(inter.flags & NB_INTERACT_CLICKED) {
                color[0] = 1.f;
                color[1] = 1.f;
                color[2] = 1.f;
        }

        nbr_box(&ctx->rdr_ctx, window->cmd_buf, recti, color, 1.f);
        ctx->rdr_ctx.cmds_count += 1;

        return window;
}


void
nbs_window_end(
        struct nb_sugar_ctx * ctx,
        const struct nb_window * win)
{
        (void)ctx;
        (void)win;
}


int
nbs_button(
        struct nb_sugar_ctx *ctx,
        const struct nb_window *win,
        const char *name)
{
        if(!ctx || !win || !name || strlen(name) == 0) {
                NB_ASSERT(0 && "NB_INVALID_PARAMS");
                return 0;
        }

        uint64_t hash_key = nbi_hash_str(name);
        
        float recti[4];
        recti[0] = 10;
        recti[1] = 10;
        recti[2] = 70;
        recti[3] = 30;

        float color[4];
        color[0] = 1.f;
        color[1] = 1.f;
        color[2] = 1.f;
        color[3] = 1.f;

        nbr_box(&ctx->rdr_ctx, win->cmd_buf, recti, color, 1.f);

        return 0;
}



/* --------------------------------------------------------- Other widgets -- */





/* -------------------------------------------------------------- Lifetime -- */


int
nbs_frame_begin(
        struct nb_sugar_ctx *ctx)
{
        nb_frame_begin(ctx->core_ctx);
        nbr_frame_begin(&ctx->rdr_ctx);
        return 0;
}


int
nbs_frame_submit(
        struct nb_sugar_ctx *ctx)
{
        /* core frame */
        nb_frame_submit(ctx->core_ctx);

        /* renderer frame */
        struct nbi_cmd_buf *cmds[32];
        int cmd_count = 0;

        int win_count = NB_ARR_COUNT(ctx->windows);
        int i;

        /* build array of cmds */
        for(i = 0; i < win_count; ++i) {
                if(ctx->windows[i].unique_id > 0) {
                        cmds[i] = ctx->windows[i].cmd_buf;
                        cmd_count += 1;

                        if(cmd_count >= NB_ARR_COUNT(cmds)) {
                                break;
                        }
                }
        }

        nbr_frame_submit(
                &ctx->rdr_ctx,
                &cmds[0],
                cmd_count);

        return 0;
}


int
nbs_init(
        struct nb_sugar_ctx * ctx)
{
        NB_ZERO_MEM(ctx);

        ctx->core_ctx = 0;
        nbc_ctx_create(&ctx->core_ctx);
        NB_ASSERT(ctx->core_ctx);

        // if(!desc || !new_ctx) {
        //         NB_ASSERT(0 && "NB_INVALID_PARAMS");
        //         return NB_INVALID_PARAMS;
        // }

        // if(desc->type_id != NB_STRUCT_CTX_CREATE) {
        //         NB_ASSERT(0 && "NB_INVALID_DESC");
        //         return NB_INVALID_DESC;
        // }

        ctx->rdr_ctx.vtx_buf = (struct nbi_vtx_buf) {
                .v = NB_ALLOC(65536 * sizeof(float)),
                .i = NB_ALLOC(65536 * sizeof(unsigned short)),
        };

        struct nbi_font_info { unsigned char * ttf; float height; };
        struct nbi_font_info finfo[] = {
                { .ttf = NB_OPEN_SANS_TTF, .height = 16.0f, },
                { .ttf = NB_PROGGY_TTF, .height = 11.0f, },
        };

        ctx->rdr_ctx.font_count = NB_ARR_COUNT(finfo);
        if(ctx->rdr_ctx.font_count > NB_ARR_COUNT(ctx->rdr_ctx.fonts)) {
                NB_ASSERT(!"Specfied more fonts than can fit in fixed-size font array!!");
                ctx->rdr_ctx.font_count = NB_ARR_COUNT(ctx->rdr_ctx.fonts);
        }

        unsigned int i;
        for(i = 0; i < ctx->rdr_ctx.font_count; i++) {
                nbi_font_init(ctx->rdr_ctx.fonts + i, finfo[i].ttf, finfo[i].height);
        }

        ctx->rdr_ctx.font = ctx->rdr_ctx.fonts;

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
