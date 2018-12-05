#ifndef NEB_SUGAR_INCLUDED
#define NEB_SUGAR_INCLUDED


#include <nebula/core.h>
#include <nebula/renderer.h>


struct nb_window {
        uint64_t unique_id;
        struct nb_rect rect;
};


struct nb_sugar_ctx {
        struct nb_renderer_ctx rdr_ctx;
        struct nb_core_ctx core_ctx;

        struct nb_window windows[32];
};


/* -------------------------------------------------------------- Lifetime -- */


int
nbs_init(
        struct nb_sugar_ctx * ctx);               /* required */


int
nbs_frame_begin(
        struct nb_sugar_ctx *ctx);                /* required */


int
nbs_frame_submit(
        struct nb_sugar_ctx *ctx);                /* required */


/* -------------------------------------------------------- Window widgets -- */


struct nb_window *
nbs_window_begin(
        struct nb_sugar_ctx *ctx,                 /* required */
        const char *name);                        /* required */


void
nbs_window_end(
        struct nb_sugar_ctx *ctx,                 /* required */
        struct nb_window *win);                   /* required */


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



struct nb_window *
nbi_window_search(
        struct nb_window *window_array,
        int arr_count,
        uint64_t hash_key)
{
        /* validate */
        if(!window_array || !arr_count || !hash_key) {
                NB_ASSERT(0 && "NB_INVALID_PARAMS");
                return 0;
        }

        /* see if there is a cached version */
        int i;
        for(i = 0; i < arr_count; ++i) {
                if(window_array[i].unique_id == hash_key) {
                        return &window_array[i];
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

                        return &window_array[i];
                }
        }

        NB_ASSERT(0 && "NB_FAIL - Window buffer too small");

        return 0;
}


struct nb_window *
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
        struct nb_window *window = nbi_window_search(
                ctx->windows,
                NB_ARR_COUNT(ctx->windows),
                hash_key);

        if(!window) {
                NB_ASSERT(0 && "NB_FAIL - Failed to get buffer");
                return 0;
        }

        /* imgui */
        struct nb_collider_desc coll_desc;
        coll_desc.type_id = NB_STRUCT_COLLIDER;
        coll_desc.ext = 0;
        coll_desc.index = 0;
        coll_desc.unique_id = window->unique_id;
        coll_desc.rect = &window->rect;

        struct nb_interaction inter;

        nbc_collider(&ctx->core_ctx, &coll_desc, &inter);

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
        }

        if(inter.flags & NB_INTERACT_CLICKED) {
                color[0] = 1.f;
                color[1] = 1.f;
                color[2] = 1.f;
        }

        nbr_box(&ctx->rdr_ctx, &ctx->rdr_ctx.cmds[0], recti, color, 1.f);
        ctx->rdr_ctx.cmds_count += 1;

        return 0;
}


void
nbs_window_end(
        struct nb_sugar_ctx * ctx,
        struct nb_window * win)
{
        (void)ctx;
        (void)win;
}


/* --------------------------------------------------------- Other widgets -- */





/* -------------------------------------------------------------- Lifetime -- */


int
nbs_frame_begin(
        struct nb_sugar_ctx *ctx)
{
        nb_frame_begin(&ctx->core_ctx);
        nbr_frame_begin(&ctx->rdr_ctx);
        return 0;
}


int
nbs_frame_submit(
        struct nb_sugar_ctx *ctx)
{
        nb_frame_submit(&ctx->core_ctx);
        nbr_frame_submit(&ctx->rdr_ctx);
        return 0;
}


int
nbs_init(
        struct nb_sugar_ctx * ctx)
{

        // if(!desc || !new_ctx) {
        //         NB_ASSERT(0 && "NB_INVALID_PARAMS");
        //         return NB_INVALID_PARAMS;
        // }

        // if(desc->type_id != NB_STRUCT_CTX_CREATE) {
        //         NB_ASSERT(0 && "NB_INVALID_DESC");
        //         return NB_INVALID_DESC;
        // }

        NB_ZERO_MEM(ctx);

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


#endif
#endif
