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


static int
nbs_frame_begin(struct nb_sugar_ctx *ctx) {
	nb_frame_begin(&ctx->core_ctx);
	nbr_frame_begin(&ctx->rdr_ctx);
}


static int
nbs_frame_submit(struct nb_sugar_ctx *ctx) {
	nb_frame_submit(&ctx->core_ctx);
	nbr_frame_submit(&ctx->rdr_ctx);
}


static int
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

        nbi_buffer_create(&ctx->rdr_ctx.view_data.cached_e_a, 1024);
        nbi_buffer_create(&ctx->rdr_ctx.view_data.cached_e_b, 1024);

        ctx->rdr_ctx.view_data.cached_e_last = &ctx->rdr_ctx.view_data.cached_e_a;
        ctx->rdr_ctx.view_data.cached_e_next = &ctx->rdr_ctx.view_data.cached_e_b;

        nbi_buffer_create(&ctx->rdr_ctx.view_data.cached_v, 1024);

        ctx->rdr_ctx.vtx_buf = (struct nbi_vtx_buf) {
                .v = NB_ALLOC(65536 * sizeof(float)),
                .i = NB_ALLOC(65536 * sizeof(unsigned short)),
        };

        struct nbi_font_info { unsigned char * ttf; float height; };
        struct nbi_font_info finfo[] = {
                { .ttf = NB_OPEN_SANS_TTF, .height = 16.0f, },
                { .ttf = NB_PROGGY_TTF, .height = 11.0f, },
        };

        ctx->rdr_ctx.font_count = NB_ARRAY_COUNT(finfo);
        if(ctx->rdr_ctx.font_count > NB_ARRAY_COUNT(ctx->rdr_ctx.fonts)) {
                NB_ASSERT(!"Specfied more fonts than can fit in fixed-size font array!!");
                ctx->rdr_ctx.font_count = NB_ARRAY_COUNT(ctx->rdr_ctx.fonts);
        }

        unsigned int i;
        for(i = 0; i < ctx->rdr_ctx.font_count; i++) {
                nbi_font_init(ctx->rdr_ctx.fonts + i, finfo[i].ttf, finfo[i].height);
        }

		ctx->rdr_ctx.font = ctx->rdr_ctx.fonts;

        /* create default styles */
        ctx->core_ctx.styles.view.bg_color = 0x575459FF;
        ctx->core_ctx.styles.view.bg_color_hover = 0x6C726BFF;
        ctx->core_ctx.styles.view.border_color = 0xD36745FF;
        ctx->core_ctx.styles.view.text_color = 0xCFE4B6FF;
        ctx->core_ctx.styles.view.border_size = 2;
        ctx->core_ctx.styles.view.padding = 5;
        ctx->core_ctx.styles.view.margin = 0;
        ctx->core_ctx.styles.view.radius = 10;

        ctx->core_ctx.styles.button.bg_color = 0xD36745FF;
        ctx->core_ctx.styles.button.bg_color_hover = 0xFAD595FF;
        ctx->core_ctx.styles.button.text_color = 0xCFE4B6FF;
        ctx->core_ctx.styles.button.padding = 0;
        ctx->core_ctx.styles.button.margin = 0;
        ctx->core_ctx.styles.button.radius = 10;

        ctx->core_ctx.styles.text_box = (struct nb_style) {
                .bg_color = 0xB3B3B3FF,
                .bg_color_hover = 0xD1D1D1FF,
                .text_color = 0xCFE4B6FF,
                .border_color = 0xD36745FF,
                .radius = 5,
        };


        return NB_OK;
}


void *
nbs_window_begin(struct nb_sugar_ctx *ctx, const char *name) {

		float rect[4];
		rect[0] = 10;
		rect[1] = 10;
		rect[2] = 100;
		rect[3] = 100;

		float color[4];
		color[0] = 1.f;
		color[1] = 0.f;
		color[2] = 0.f;
		color[3] = 1.f;

		nbr_box(&ctx->rdr_ctx, &ctx->rdr_ctx.cmds[0], rect, color, 1.f);
		ctx->rdr_ctx.cmds_count += 1;
}


void
nbs_window_end(struct nb_sugar_ctx *ctx, void *cmds) {

}


#endif


/* -- IMPL -- */


#ifdef NEB_SUGAR_IMPL
#ifndef NEB_SUGAR_IMPL_INCLUDED
#define NEB_SUGAR_IMPL_INCLUDED





#endif
#endif
