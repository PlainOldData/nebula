#ifndef NEB_CORE_INCLUDED
#define NEB_CORE_INCLUDED


#ifdef _MSC_VER
        typedef signed   __int8  int8_t;
        typedef unsigned __int8  uint8_t;
        typedef signed   __int16 int16_t;
        typedef unsigned __int16 uint16_t;
        typedef signed   __int32 int32_t;
        typedef unsigned __int32 uint32_t;
        typedef signed   __int64 int64_t;
        typedef unsigned __int64 uint64_t;
        #include <stddef.h>
#else
        #include <stdint.h>
        #include <stddef.h>
#endif


/* ---------------------------------------------------------- Common Types -- */
/*
 *  General types and identifiers for Nebula, checking/logging the return codes
 *  will generally reveal any issues in the calling code.
 */


typedef struct nb_core_ctx * nbc_ctx_t;


typedef enum _nb_result {
        NB_OK,
        NB_FAIL,
        NB_INVALID_DESC,
        NB_INVALID_PARAMS,
        NB_CORRUPT_CALL,
} nb_result;


typedef enum _nb_bool {
        NB_FALSE,
        NB_TRUE,
} nb_bool;


/* -------------------------------------------------------- Core Utilities -- */
/*
 * Utilities used throughout Core, Renderer and Sugar code.
 */

struct nb_rect {
        int x, y;                           /* top left of rect */
        int w, h;                           /* dimentions of rect */
};


/*
 * returns a rect constructed by the paramaters
 */
struct nb_rect
nb_rect_from_point_size(
        int x,                              /* top left x position */
        int y,                              /* top left y position */
        int w,                              /* width of rect */
        int h);                             /* height of rect */


/*
 * returns NB_TRUE if the point is inside the rect, NB_FALSE if the point
 * is outside.
 */
nb_bool
nb_rect_contains(
        struct nb_rect r,
        int x,
        int y);


struct nb_color {
        float r,g,b,a;                      /* color components */
};


/*
 * returns a color, constructed by a hex, the hex is defined as 0xRRGGBBAA
 * where RR is red, GG is green, BB is blue, and AA is alpha.
 */
struct nb_color
nb_color_from_int(
        uint32_t hex);                      /* hex in format 0xRRGGBBAA, eg 0x00FF00FF is full green. */


/*
 * returns a color, constructed by an array of 4 floats, in the order rgba.
 */
struct nb_color
nb_color_from_float_arr(
        float *arr);                        /* required - must be float[4] */


/* -------------------------------------------------------------- Lifetime -- */
/*
 *  Nebula uses an opaque type for its context so as not to pollute the users
 *  namespace. You have to remember to create and destroy this context.
 */


/*
 * return NB_OK if the new frame has started
 * return NB_INVALID_PARAMS if ctx if not valid
 * return NB_FAIL if an internal error occured
 */
nb_result
nbc_ctx_create(
        nbc_ctx_t *ctx);                    /* required */


/*
 * return NB_OK if the new frame has started
 * return NB_INVALID_PARAMS if ctx if not valid
 * return NB_FAIL if an internal error occured
 */
nb_result
nbc_ctx_destroy(
        nbc_ctx_t *ctx);                    /* required */


/* ----------------------------------------------------------------- Frame -- */
/*
 *  Nebula works on frames, at the start of all nebula commands you must call
 *  `nbc_frame_begin()` followed by your nebula commands. Once finished you must
 *  call `nbc_frame_submit()` The next frame you will be informed of any
 *  interactions the pointer had with colliders.
 */

/*
 * return NB_OK if the new frame has started
 * return NB_INVALID_PARAMS if ctx if not valid
 * return NB_FAIL if an internal error occured
 */
nb_result
nbc_frame_begin(
        nbc_ctx_t ctx);                     /* required */


/*
 * return NB_OK if the new frame was submitted
 * return NB_INVALID_PARAMS if ctx is not valid
 * return NB_FAIL if an internal error occured
 */
nb_result
nbc_frame_submit(
        nbc_ctx_t ctx);                     /* required */


/* -------------------------------------------------------------- Collider -- */
/*
 * Adds a collider to the environment, if a collider has hit the pointer,
 *
 * Note: you must only add a collider between `nbc_frame_begin()` and
 * `nbc_frame_submit()`
 */


struct nb_collider_desc {
        int index;                          /* Order of the colliders, if the duplicates then newer one has higher priority. */
        uint64_t unique_id;                 /* Used to determine what has collided. */
        struct nb_rect * rect;              /* Area of the collider. */
};


typedef enum _nb_interactions_flags {
        NB_INTERACT_DRAGGED = 1 << 0,       /* The pointer is held down on a collider. */
        NB_INTERACT_CLICKED = 1 << 1,       /* The pointer is released on a collider. */
        NB_INTERACT_HOVER = 1 << 2,         /* The pointer if hovering over a collider, will not be set if `NB_INTERACT_DRAGGED` is set. */
} nb_interaction_flags;


struct nb_interaction {
        uint32_t flags;                     /* Contains `nb_interaction_flags` bits. */
        float delta_x;                      /* If bit `NB_INTERACT_DRAGGED` is set then this is delta x for last frame */
        float delta_y;                      /* If bit `NB_INTERACT_DRAGGED` is set then this is delta y for last frame */
};


/*
 * returns NB_OK if the collider was added.
 * returns NB_INVALID_PARAMS if ctx or desc are null.
 * returns NB_CORRUPT_CALL if not called between `nbc_frame_begin` and `nbc_frame_submit`
 * returns NB_FAIL if an internal error occured.
 */
nb_result
nbc_collider(
        nbc_ctx_t ctx,                      /* required */
        struct nb_collider_desc * desc,     /* required */
        struct nb_interaction * out_inter); /* optional */


/* ----------------------------------------------------------------- State -- */
/*
 *  Nebula has no concept of the world it lives in so it requires to be told
 *  about mouse events and so forth.
 */

struct nb_pointer_desc {
        int x;
        int y;
        float scroll_y;
        int interact;
};


/*
 * returns `NB_OK` on success
 * returns `NB_INVALID_PARAMS` if ctx or desc is null
 * returns `NB_FAIL` if an internal error occured
 */
nb_result
nbc_state_set_pointer(
        nbc_ctx_t ctx,                      /* required */
        struct nb_pointer_desc * desc);     /* required */


struct nb_viewport_desc {
        int width;                          /* Width of the viewport. */
        int height;                         /* Height of the viewport. */
};


/*
 * returns `NB_OK` on success
 * returns `NB_INVALID_PARAMS` if ctx or desc is null
 * returns `NB_FAIL` if an internal error occured
 */
nb_result
nbc_state_set_viewport(
        nbc_ctx_t ctx,                      /* required */
        struct nb_viewport_desc *desc);     /* required */


nb_result
nbc_state_set_text_input(
        nbc_ctx_t ctx,                      /* required */
        char * text);                       /* optional */


nb_result
nbc_state_set_dt(
        nbc_ctx_t ctx,                      /* required */
        float dt);


struct nb_state {
        int ptr_x, ptr_y;
        int ptr_dx, ptr_dy;
        int interaction;

        int vp_width;
        int vp_height;

        unsigned long long hover_view_hash;
        unsigned long long hover_element_hash;
};


/*
 * returns `NB_OK` on success
 * returns `NB_INVALID_PARAMS` if ctx or desc is null
 * returns `NB_FAIL` if an internal error occured
 */
nb_result
nbc_state_get(
        nbc_ctx_t ctx,                      /* required */
        struct nb_state *out_state);        /* required */


#endif


/* ================================== IMPL ================================== */


#ifdef NEB_CORE_IMPL
#ifndef NEB_CORE_IMPL_INCLUDED
#define NEB_CORE_IMPL_INCLUDED


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

#define NB_COLLIDER_MAX 512


/* ------------------------------------------------- Internal Common Types -- */


typedef enum _nbi_ptr_state {
        NBI_PTR_UP,
        NBI_PTR_UP_EVENT,
        NBI_PTR_DOWN,
        NBI_PTR_DOWN_EVENT,
} nbi_ptr_state;


struct nbi_state {
        nbi_ptr_state ptr_state;
        int ptr_pos[2];
        int ptr_delta[2];
        int ptr_distance[2];

        unsigned long ptr_view;
        unsigned long ptr_ele;

        unsigned long ptr_ele_drop;
        void* ptr_ele_drag;

        unsigned long focus_ele;
        unsigned int focus_time;

        int vp_size[2];

        char text_input[4096];
        unsigned int text_cursor;
        float text_cursor_time;

        float dt;
};


struct nbi_collider {
        uint64_t unique_id;
        int index;
        struct nb_rect rect;
};


struct nb_core_ctx {
        void *user_data;
        unsigned long tick;

        struct nbi_state state;

        /* colliders */
        struct nbi_collider colliders[NB_COLLIDER_MAX];
        int collider_count;

        /* interacting */
        int inter_idx;
        uint64_t inter_id;

        /* frame open */
        int frame_open;
};


/* -------------------------------------------------------------- Collider -- */


nb_result
nbc_collider(
        nbc_ctx_t ctx,
        struct nb_collider_desc * desc,
        struct nb_interaction * out_inter)
{
        /* validate params and state */
        if(!ctx || !desc) {
                NB_ASSERT(!"NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        if(ctx->frame_open != NB_TRUE) {
                /* Call this between`nbc_frame_begin()` and `nbc_frame_begin()` */
                NB_ASSERT(!"NB_CORRUPT_CALL");
                return NB_CORRUPT_CALL;
        }

        int desired_idx = desc->index;
        unsigned int insert_idx = 0;
        int count = ctx->collider_count;
        int capacity = NB_ARR_COUNT(ctx->colliders);
        int i;

        /* find insert point */
        if(!capacity) {
                NB_ASSERT(!"NB_FAIL - No capacity in colliders");
                return NB_FAIL;
        }

        for(i = 0; i < count; ++i) {
                if(ctx->colliders[i].index > desired_idx) {
                        insert_idx = i;
                        break;
                };
        };

        /* check if space */
        if(insert_idx > NB_ARR_COUNT(ctx->colliders)) {
                NB_ASSERT(!"NB_FAIL - Out of collider space");
                return NB_FAIL;
        }

        /* shuffle array */
        int dst_i = insert_idx + 1;
        int src_i = insert_idx;

        if(dst_i > capacity || src_i > dst_i) {
                NB_ASSERT(!"NB_FAIL - Cannot shuffle array");
                return NB_FAIL;
        }

        void *dst = (void*)&ctx->colliders[insert_idx + 1];
        void *src = (void*)&ctx->colliders[insert_idx];
        size_t size = sizeof(ctx->colliders[0]) * (capacity - insert_idx - 1);
        memmove(dst, src, size);

        /* insert new collider */
        ctx->colliders[insert_idx].rect = *desc->rect;
        ctx->colliders[insert_idx].unique_id = desc->unique_id;
        ctx->collider_count += 1;

        /* early bail if not interacting */
        if(!out_inter) {
                return NB_OK;
        }

        /* interacting */
        out_inter->flags = 0;

        if(desc->unique_id == ctx->inter_id) {
                out_inter->flags |= NB_INTERACT_HOVER;

                if(ctx->state.ptr_state == NBI_PTR_DOWN) {
                        out_inter->flags |= NB_INTERACT_DRAGGED;
                }

                if(ctx->state.ptr_state == NBI_PTR_UP_EVENT) {
                        out_inter->flags |= NB_INTERACT_CLICKED;
                }

                if(ctx->state.ptr_state == NBI_PTR_DOWN) {
                        out_inter->flags |= NB_INTERACT_DRAGGED;
                        out_inter->delta_x = (float)ctx->state.ptr_delta[0];
                        out_inter->delta_y = (float)ctx->state.ptr_delta[1];
                }
        }


        return NB_OK;
}


/* ----------------------------------------------------------------- State -- */


nb_result
nbc_state_set_pointer(
        nbc_ctx_t ctx,
        struct nb_pointer_desc *desc)
{
        if(!desc || !ctx) {
                NB_ASSERT(!"NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        struct nbi_state *state = &ctx->state;

        /* ms ptr */
        state->ptr_delta[0] = desc->x - state->ptr_pos[0];
        state->ptr_delta[1] = desc->y - state->ptr_pos[1];
        state->ptr_pos[0] = desc->x;
        state->ptr_pos[1] = desc->y;
        state->ptr_distance[0] += state->ptr_delta[0];
        state->ptr_distance[1] += state->ptr_delta[1];

        state->ptr_ele_drop = 0;

        if (desc->interact) {
                if (state->ptr_state == NBI_PTR_DOWN_EVENT) {
                        state->ptr_state = NBI_PTR_DOWN;
                }
                else if(state->ptr_state < NBI_PTR_DOWN) {
                        state->ptr_state = NBI_PTR_DOWN_EVENT;
                        state->ptr_distance[0] = 0;
                        state->ptr_distance[1] = 0;
                }
        }
        else {
                if (state->ptr_state == NBI_PTR_UP_EVENT) {
                        state->ptr_state = NBI_PTR_UP;
                        state->ptr_ele_drop = state->ptr_ele;
                }
                else if(state->ptr_state >= NBI_PTR_DOWN) {
                        state->ptr_state = NBI_PTR_UP_EVENT;
                }
                else {
                        state->ptr_state = NBI_PTR_UP;
                }
        }

        return NB_OK;
}


nb_result
nbc_state_set_viewport(
        nbc_ctx_t ctx,
        struct nb_viewport_desc *desc)
{
        /* validate */
        if(!desc || !ctx) {
                NB_ASSERT(!"NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        ctx->state.vp_size[0] = desc->width;
        ctx->state.vp_size[1] = desc->height;

        return NB_OK;
}

nb_result
nbc_state_set_text_input(nbc_ctx_t ctx, char * text) {
        struct nbi_state *state = &ctx->state;

        char * src = text;
        char * dst = state->text_input;
        unsigned int len_max = NB_ARR_COUNT(state->text_input) - 1;
        unsigned int len = 0;
        while(*src && len < len_max) {
                *dst++ = *src++;
                len++;
        }
        *dst = 0;

        return NB_OK;
}

nb_result
nbc_state_set_dt(nbc_ctx_t ctx, float dt) {
        NB_ASSERT(ctx);
        ctx->state.dt = dt;
        return NB_OK;
}


nb_result
nbc_state_get(
        nbc_ctx_t ctx,
        struct nb_state *out_state)
{
        if(!ctx || !out_state) {
                NB_ASSERT(!"NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        out_state->interaction = ctx->state.ptr_state;
        out_state->ptr_dx = ctx->state.ptr_delta[0];
        out_state->ptr_dy = ctx->state.ptr_delta[1];
        out_state->ptr_x = ctx->state.ptr_pos[0];
        out_state->ptr_y = ctx->state.ptr_pos[1];
        out_state->interaction = ctx->state.ptr_state;

        out_state->vp_width = ctx->state.vp_size[0];
        out_state->vp_height = ctx->state.vp_size[1];

        out_state->hover_view_hash = ctx->state.ptr_view;
        out_state->hover_element_hash = ctx->state.ptr_ele;

        return NB_FAIL;
}


/* ----------------------------------------------------------------- Frame -- */


nb_result
nbc_frame_begin(
        nbc_ctx_t ctx)
{
        if(!ctx) {
                NB_ASSERT(!"NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        if(ctx->frame_open == NB_TRUE) {
                /* Was `nbc_frame_submit()` last frame? */
                NB_ASSERT(!"NB_CORRUPT_CALL");
                return NB_CORRUPT_CALL;
        }

        ctx->frame_open = NB_TRUE;

        return NB_OK;
}


nb_result
nbc_frame_submit(
        nbc_ctx_t ctx)
{
        if(!ctx) {
                NB_ASSERT(!"NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        if(ctx->frame_open == NB_FALSE) {
                /* Was `nbc_frame_begin()` called */
                NB_ASSERT(!"NB_CORRUPT_CALL");
                return NB_CORRUPT_CALL;
        }

        ctx->frame_open = NB_FALSE;

        /* clear intermitant state */
        ctx->state.ptr_delta[0] = 0;
        ctx->state.ptr_delta[1] = 0;

        if(ctx->state.ptr_state == NBI_PTR_DOWN_EVENT) {
                ctx->state.ptr_state = NBI_PTR_DOWN;
        } else if(ctx->state.ptr_state == NBI_PTR_UP_EVENT) {
                ctx->state.ptr_state = NBI_PTR_UP;
        }

        /* bail if the pointer is being dragged */
        if(ctx->state.ptr_state == NBI_PTR_DOWN) {
                ctx->collider_count = 0;
                return NB_OK;
        }

        ctx->inter_idx = 0;
        ctx->inter_id = 0;

        /* check collisions */
        int i;
        int coll_count = ctx->collider_count;

        for(i = 0; i < coll_count; ++i) {
                nb_bool contains = nb_rect_contains(
                        ctx->colliders[i].rect,
                        (int)ctx->state.ptr_pos[0],
                        (int)ctx->state.ptr_pos[1]);

                if(contains == NB_TRUE) {
                        ctx->inter_idx = ctx->colliders[i].index;
                        ctx->inter_id = ctx->colliders[i].unique_id;
                }
        }

        ctx->collider_count = 0;

        return NB_OK;
}


/* -------------------------------------------------------- Core Utilities -- */


struct nb_rect
nb_rect_from_point_size(
        int x,
        int y,
        int w,
        int h)
{
        struct nb_rect rect;

        rect.x = x; rect.y = y;
        rect.w = w; rect.h = h;

        return rect;
};


nb_bool
nb_rect_contains(
        struct nb_rect r,
        int x,
        int y)
{
        if(x < r.x || y < r.y) {
                return NB_FALSE;
        }

        if(x > (r.x + r.w)) {
                return NB_FALSE;
        }

        if(y > (r.y + r.h)) {
                return NB_FALSE;
        }

        return NB_TRUE;
}


struct nb_color
nb_color_from_int(
        uint32_t hex)
{
        struct nb_color color;
        
        uint8_t c0 = (hex >> 24) & 0xFF;
        color.r = (float)(c0) / 255.f;
        
        uint8_t c1 = (hex >> 16) & 0xFF;
        color.g = (float)(c1) / 255.f;
        
        uint8_t c2 = (hex >> 8) & 0xFF;
        color.b = (float)(c2) / 255.f;
        
        uint8_t c3 = (hex >> 0) & 0xFF;
        color.a = (float)(c3) / 255.f;
        
        return color;
}


struct nb_color
nb_color_from_float_arr(float *arr) {
        struct nb_color color;

        color.r = arr[0]; color.g = arr[1];
        color.b = arr[2]; color.a = arr[3];

        return color;
};



/* -------------------------------------------------------------- Lifetime -- */


nb_result
nbc_ctx_create(
        nbc_ctx_t *ctx)
{
        if(!ctx) {
                NB_ASSERT(!"NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        struct nb_core_ctx *new_ctx = 0;
        new_ctx = (struct nb_core_ctx*)NB_ALLOC(sizeof(*new_ctx));

        if(!new_ctx) {
                NB_ASSERT(!"NB_FAIL - failed to allocate memory");
                return NB_FAIL;
        }

        NB_ZERO_MEM(new_ctx);
        *ctx = new_ctx;

        return NB_OK;
}


nb_result
nbc_ctx_destroy(
        nbc_ctx_t *ctx)
{
        if(!ctx) {
                NB_ASSERT(!"NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        struct nb_core_ctx *kill_ctx = *ctx;
        NB_FREE(kill_ctx);

        *ctx = 0;

        return NB_OK;
}


/* ---------------------------------------------------------------- Config -- */


#undef NB_ASSERT
#undef NB_ALLOC
#undef NB_FREE
#undef NB_ZERO_MEM
#undef NB_ARR_COUNT
#undef NB_ARRAY_DATA
#undef NB_COLLIDER_MAX


#endif
#endif
