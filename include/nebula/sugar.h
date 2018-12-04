#ifndef NEB_SUGAR_INCLUDED
#define NEB_SUGAR_INCLUDED


#include <nebula/core.h>
#include <nebula/renderer.h>
#include "nebula_font_open_sans.h"
#include "nebula_font_proggy.h"
#include "nebula_font_awesome.h"


struct nb_sugar_ctx {
        struct nb_renderer_ctx rdr_ctx;
        struct nb_core_ctx core_ctx;
};


/* -------------------------------------------------------------- Lifetime -- */


int
nbs_init(struct nb_sugar_ctx * ctx);


int
nbs_frame_begin(struct nb_sugar_ctx *ctx);


int
nbs_frame_submit(struct nb_sugar_ctx *ctx);


/* --------------------------------------------------------------- Widgets -- */


void *
nbs_window_begin(struct nb_sugar_ctx *ctx, const char *name);


void
nbs_window_end(struct nb_sugar_ctx *ctx, void *cmds);


#endif


/* ================================== IMPL ================================== */


#ifdef NEB_SUGAR_IMPL
#ifndef NEB_SUGAR_IMPL_INCLUDED
#define NEB_SUGAR_IMPL_INCLUDED


#define NEB_CORE_IMPL
#include <nebula/core.h>


#define NEB_RENDERER_IMPL
#include <nebula/renderer.h>


int
nbs_frame_begin(struct nb_sugar_ctx *ctx) {
        nb_frame_begin(&ctx->core_ctx);
        nbr_frame_begin(&ctx->rdr_ctx);
}


int
nbs_frame_submit(struct nb_sugar_ctx *ctx) {
        nb_frame_submit(&ctx->core_ctx);
        nbr_frame_submit(&ctx->rdr_ctx);
}


int
nbs_init(struct nb_sugar_ctx * ctx) {

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


void *
nbs_window_begin(struct nb_sugar_ctx *ctx, const char *name) {

        if(!ctx || !name) {
                return 0;
        }

        struct nb_rect rect = nb_rect_create(10, 10, 100, 100);

        /* imgui */
        struct nb_collider_desc coll_desc;
        coll_desc.type_id = NB_STRUCT_COLLIDER;
        coll_desc.ext = 0;
        coll_desc.index = 0;
        coll_desc.unique_id = 1;
        coll_desc.rect = &rect;

        struct nb_interaction inter;

        nbc_collider(&ctx->core_ctx, &coll_desc, &inter);

        /* render */
        float recti[4];
        recti[0] = 10;
        recti[1] = 10;
        recti[2] = 100;
        recti[3] = 100;

        float color[4];
        color[0] = 1.f;
        color[1] = 0.f;
        color[2] = 0.f;
        color[3] = 1.f;

        if(inter.hovered) {
                color[1] = 1.f;
        }

        nbr_box(&ctx->rdr_ctx, &ctx->rdr_ctx.cmds[0], recti, color, 1.f);
        ctx->rdr_ctx.cmds_count += 1;
}


void
nbs_window_end(struct nb_sugar_ctx *ctx, void *cmds) {

}


#endif
#endif
