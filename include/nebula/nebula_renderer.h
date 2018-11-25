#ifndef NEBULA_RENDERER_INCLUDED
#define NEBULA_RENDERER_INCLUDED


#include "nebula_core.h"


struct nbr_rdr_cmd {
        float rect[4];
        float color[4];
};


struct nbr_ctx {
        struct nbr_rdr_cmd cmds[256];
        int count;
};



void
nbr_rect(struct nbr_ctx *ctx, struct nbc_rect rect);


#endif


/* IMPL */


#ifdef NEBULA_RENDERER_IMPL
#ifndef NEBULA_RENDERER_IMPL_INCLUDED
#define NEBULA_RENDERER_IMPL_INCLUDED


void
nbr_rect(struct nbr_ctx *ctx, struct nbc_rect rect) {
        int count = ctx->count;
        ctx->cmds[count].rect[0] = rect.point.x;
        ctx->cmds[count].rect[1] = rect.point.y;
        ctx->cmds[count].rect[2] = rect.size.x;
        ctx->cmds[count].rect[3] = rect.size.y;
}


#endif
#endif
