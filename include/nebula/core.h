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


/* -------------------------------------------------------- Core Utilities -- */


struct nb_rect {
        int x, y;
        int w, h;
};


struct nb_rect
nb_rect_create(int x, int y, int w, int h) {
        struct nb_rect rect;
        rect.x = x; rect.y = y;
        rect.w = w; rect.h = h;

        return rect;
};


int
nb_rect_contains(
        struct nb_rect r,
        int x,
        int y)
{
        if(x < r.x || y < r.y) {
                return 0;
        }

        if(x > (r.x + r.w)) {
                return 0;
        }

        if(y > (r.y + r.h)) {
                return 0;
        }

        return 1;
}


/* ---------------------------------------------------------- Common Types -- */
/*
 *  General types and identifiers for Nebula, checking/logging the return codes
 *  will generally reveal any issues in the calling code.
 */


#include <assert.h>
#define NB_ASSERT(expr) assert(expr)

#include <stdlib.h>
#define NB_ALLOC(bytes) malloc(bytes)
#define NB_FREE(addr) free(addr)

#include <string.h>
#define NB_ZERO_MEM(ptr) \
  do{memset((ptr), 0, sizeof((ptr)[0]));}while(0)

#define NB_MEM_CPY(dst, src, size) \
        do { \
            unsigned _i; \
            for(_i = 0; _i < (unsigned)size; ++_i) { \
                    ((unsigned char*) dst)[_i] = ((unsigned char*) src)[_i]; \
            } \
        } while(0)


#define NB_ARR_COUNT(ARR) (sizeof((ARR)) / sizeof((ARR)[0]))
#define NB_ARRAY_DATA(ARR) &ARR[0]


typedef unsigned int nb_color;


typedef enum _nbi_ptr_state {
        NBI_PTR_UP,
        NBI_PTR_UP_EVENT,
        NBI_PTR_DOWN,
        NBI_PTR_DOWN_EVENT,
} nbi_ptr_state;


typedef enum nb_text_align {
        NB_TEXT_ALIGN_LEFT = 0,
        NB_TEXT_ALIGN_RIGHT = 1,
        NB_TEXT_ALIGN_CENTER = 2,

        _NB_TEXT_ALIGN_BIT_MASK = 3,
} nb_text_align;


typedef enum nbi_ptr_view_state {
        NBI_PTR_VIEW_WAITING,
        NBI_PTR_VIEW_DRAGGING,
        NBI_PTR_VIEW_RESIZING,
} nbi_ptr_view_state;


struct nbi_state {
        nbi_ptr_state ptr_state;
        int ptr_pos[2];
        int ptr_delta[2];
        int ptr_distance[2];

        unsigned long ptr_view;
        unsigned long ptr_ele;
        nbi_ptr_view_state ptr_view_action;

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


typedef enum _nbc_type_id {
        NB_STRUCT_CTX_CREATE,
        NB_STRUCT_VIEW,
        NB_STRUCT_ELEMENT,
        NB_STRUCT_POINTER,
        NB_STRUCT_VIEWPORT,
        NB_STRUCT_COLLIDER,
} nbc_type_id;


typedef enum _nb_result {
        NB_OK,
        NB_FAIL,
        NB_INVALID_DESC,
        NB_INVALID_PARAMS,
} nb_result;


typedef enum nb_identifier {
        /* nb_result */


        /* end */
        NB_ID_COUNT
} nb_identifier;


struct nbi_collider {
        unsigned long unique_id;
        int index;
        struct nb_rect rect;
};


struct nb_core_ctx {
        void *user_data;
        unsigned long tick;

        struct nbi_state state;

        /* colliders */
        struct nbi_collider colliders[512];
        int collider_count;

        /* interacting */
        int inter_idx;
        unsigned long inter_id;
};


/* ----------------------------------------------------------------- Frame -- */
/*
 *  Nebula works on frame, each frame you must submit what widows/ui to draw
 *  when the frame ends, you can query for the draw cmds.
 */

/*
 * return NB_OK if the new frame has started
 * return NB_INVALID_PARAMS if ctx if not valid
 * return NB_FAIL if an internal error occured
 */
nb_result
nb_frame_begin(
        struct nb_core_ctx * ctx);                /* required */


/*
 * return NB_OK if the new frame was submitted
 * return NB_INVALID_PARAMS if ctx is not valid
 * return NB_FAIL if an internal error occured
 */
nb_result
nb_frame_submit(
        struct nb_core_ctx * ctx);                /* required */


/* -------------------------------------------------------------- Collider -- */


struct nb_collider_desc {
        nbc_type_id type_id;
        void *ext;

        int index;
        unsigned long unique_id;
        struct nb_rect * rect;
};


typedef enum _nb_interactions_flags {
        NB_INTERACT_DRAGGED = 1 << 0,
        NB_INTERACT_CLICKED = 1 << 1,
        NB_INTERACT_HOVER = 1 << 2,
} nb_interaction_flags;


struct nb_interaction {
        uint32_t flags;
        float delta_x;
        float delta_y;
};


/*
 * returns NB_OK if the collider was added.
 * returns NB_INVALID_PARAMS if ctx or desc are null.
 * returns NB_INVALID_DESC if desc type_id is not NB_STRUCT_COLLIDER.
 * returns NB_FAIL if an internal error occured.
 */
nb_result
nbc_collider(
        struct nb_core_ctx * ctx,                 /* required */
        struct nb_collider_desc * desc,           /* required */
        struct nb_interaction * out_inter);       /* optional */


/* ----------------------------------------------------------------- State -- */
/*
 *  Nebula has no concept of the world it lives in so it requires to be told
 *  about mouse events and so forth.
 */

struct nb_pointer_desc {
        nbc_type_id type_id;
        void *ext;

        int x;
        int y;
        float scroll_y;
        int interact;
};


/*
 * returns NB_OK on success
 * returns NB_INVALID_PARAMS if ctx or desc is null
 * returns NB_INVALID_DESC if desc type_id is not NB_STRUCT_POINTER
 * returns NB_FAIL if an internal error occured
 */
nb_result
nb_state_set_pointer(
        struct nb_core_ctx * ctx,                 /* required */
        struct nb_pointer_desc * desc);           /* required */


struct nb_viewport_desc {
        nbc_type_id type_id;
        void *ext;

        int width;
        int height;
};


/*
 * returns NB_OK on success
 * returns NB_INVALID_PARAMS if ctx or desc is null
 * returns NB_INVALID_DESC if desc type_id is not NB_STRUCT_VIEWPORT
 * returns NB_FAIL if an internal error occured
 */
nb_result
nb_state_set_viewport(
        struct nb_core_ctx * ctx,                 /* required */
        struct nb_viewport_desc *desc);           /* required */


nb_result
nb_state_set_text_input(
        struct nb_core_ctx * ctx,                 /* required */
        char * text);                             /* optional */


nb_result
nb_state_set_dt(
        struct nb_core_ctx * ctx,                 /* required */
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


nb_result
nb_state_get(
        struct nb_core_ctx * ctx,                 /* required */
        struct nb_state *out_state);              /* required */


#endif


/* ================================== IMPL ================================== */


#ifdef NEB_CORE_IMPL
#ifndef NEB_CORE_IMPL_INCLUDED
#define NEB_CORE_IMPL_INCLUDED


/* -------------------------------------------------------------- Collider -- */


nb_result
nbc_collider(
        struct nb_core_ctx * ctx,
        struct nb_collider_desc * desc,
        struct nb_interaction * out_inter)
{
        /* validate params */
        if(!ctx || !desc) {
                NB_ASSERT(0 && "NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        if(desc->type_id != NB_STRUCT_COLLIDER) {
                NB_ASSERT(0 && "NB_INVALID_DESC");
                return NB_INVALID_DESC;
        }

        int desired_idx = desc->index;
        int insert_idx = 0;
        int count = ctx->collider_count;
        int capacity = NB_ARR_COUNT(ctx->colliders);
        int i;

        /* find insert point */
        if(!capacity) {
                NB_ASSERT(0 && "NB_FAIL");
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
                NB_ASSERT(0 && "NB_FAIL");
                return NB_FAIL;
        }

        /* shuffle array */
        int dst_i = insert_idx + 1;
        int src_i = insert_idx;

        if(dst_i > capacity || src_i > dst_i) {
                NB_ASSERT(0 && "NB_FAIL");
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

        /* interacting */
        if(out_inter) {
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
                                out_inter->delta_x = ctx->state.ptr_delta[0];
                                out_inter->delta_y = ctx->state.ptr_delta[1];
                        }
                }
        }

        return NB_OK;
}


/* ----------------------------------------------------------------- State -- */


nb_result
nb_state_set_pointer(
        struct nb_core_ctx * ctx,
        struct nb_pointer_desc *desc)
{
        NB_ASSERT(desc);  /* cannot be null */
        NB_ASSERT(ctx);   /* cannot be null */
        NB_ASSERT(desc->type_id == NB_STRUCT_POINTER);  /* must be correct id */

        if(!desc || !ctx) {
                NB_ASSERT(0 && "NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        if(desc->type_id != NB_STRUCT_POINTER) {
                NB_ASSERT(0 && "NB_INVALID_DESC");
                return NB_INVALID_DESC;
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
nb_state_set_viewport(
        struct nb_core_ctx * ctx,
        struct nb_viewport_desc *desc)
{
        NB_ASSERT(ctx);   /* required valid ctx */
        NB_ASSERT(desc);  /* cannot be null */
        NB_ASSERT(desc->type_id == NB_STRUCT_VIEWPORT); /* must be correct id */

        if(!desc || !ctx) {
                return NB_INVALID_PARAMS;
        }

        if(desc->type_id != NB_STRUCT_VIEWPORT) {
                return NB_INVALID_DESC;
        }

        ctx->state.vp_size[0] = desc->width;
        ctx->state.vp_size[1] = desc->height;

        return NB_OK;
}

nb_result
nb_state_set_text_input(struct nb_core_ctx * ctx, char * text) {
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

nb_result nb_state_set_dt(struct nb_core_ctx * ctx, float dt) {
        NB_ASSERT(ctx);
        ctx->state.dt = dt;
        return NB_OK;
}


nb_result
nb_state_get(
        struct nb_core_ctx * ctx,
        struct nb_state *out_state)
{
        if(!ctx || !out_state) {
                NB_ASSERT(0 && "NB_INVALID_PARAMS");
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


nb_result
nb_frame_begin(
        struct nb_core_ctx * ctx)
{
        // if(!ctx) {
        //         NB_ASSERT(0 && "NB_INVALID_PARAMS");
        //         return NB_INVALID_PARAMS;
        // }

        // struct nbi_state *st = &ctx->state;

        // if(st->ptr_state != NBI_PTR_DOWN) {
        //         st->ptr_view = 0;
        //         st->ptr_ele = 0;
        // }

        // struct nb_buffer *old_next = ctx->view_data.cached_e_next;
        // struct nb_buffer *old_last = ctx->view_data.cached_e_last;

        // nbi_buffer_clear(old_last);

        // ctx->view_data.cached_e_next = old_last;
        // ctx->view_data.cached_e_last = old_next;

        // if(st->focus_ele) {
        //         struct nb_element * focus_ele = nbi_find_element_by_hash(ctx->view_data.cached_e_last, st->focus_ele);
        //         if(focus_ele) {
        //                 st->focus_time++;
        //         }
        //         else {
        //                 st->focus_ele = 0;
        //         }
        // }

        // /* interaction */
        // if(st->ptr_view == 0) {
        //         nbi_frame_interaction(ctx);
        // }

        // ctx->state.text_cursor_time += ctx->state.dt;

        // /* DEBUG!! */
        // if(ctx->debug_font_next) {
        //         ctx->font = ctx->debug_font_next;
        //         ctx->debug_font_next = 0;
        // }

        // ctx->vtx_buf.v_count = 0;
        // ctx->vtx_buf.i_count = 0;

        /* clear cmds */
        // struct nb_buffer *buf = &ctx->view_data.cached_v;
        // struct nb_view *view = nbi_buffer_mem(buf);

        // unsigned int stride = sizeof(*view);
        // unsigned int count = nbi_buffer_used(buf) / stride;
        // unsigned int i;

        // for(i = 0; i < count; ++i) {
        //         view[i].cmds->cmd_count = 0;
        // }

        // ctx->tick += 1;

        return NB_OK;
}


nb_result
nb_frame_submit(
        struct nb_core_ctx * ctx)
{
        if(!ctx) {
                NB_ASSERT(0 && "NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        ctx->inter_idx = 0;
        ctx->inter_id = 0;

        /* check collisions */
        int i;
        int coll_count = NB_ARR_COUNT(ctx->colliders);

        for(i = 0; i < coll_count; ++i) {
                int contains = nb_rect_contains(
                        ctx->colliders[i].rect,
                        (int)ctx->state.ptr_pos[0],
                        (int)ctx->state.ptr_pos[1]);

                if(contains == 1) {
                        ctx->inter_idx = ctx->colliders[i].index;
                        ctx->inter_id = ctx->colliders[i].unique_id;
                }
        }

        ctx->collider_count = 0;

        return NB_OK;
}


#endif
#endif
