#ifndef NEB_CORE_INCLUDED
#define NEB_CORE_INCLUDED


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


#define NB_ARRAY_COUNT(ARRAY) (sizeof((ARRAY)) / sizeof((ARRAY)[0]))
#define NB_ARRAY_DATA(ARRAY) &ARRAY[0]


/* ---------------------------------------------------------- Common Types -- */
/*
 *  General types and identifiers for Nebula, checking/logging the return codes
 *  will generally reveal any issues in the calling code.
 */


typedef int nb_result;
typedef unsigned int nb_color;


typedef enum nbi_ptr_state {
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


struct nb_style {
        nb_color bg_color;
        nb_color bg_color_hover;
        nb_color text_color;
        nb_color border_color;
        int border_size;

        int offset_x;
        int offset_y;
        int disable_clip;

        int padding;
        int margin;
        int radius;
        int height;

        nb_text_align align;
};

typedef enum nbi_ptr_view_state {
        NBI_PTR_VIEW_WAITING,
        NBI_PTR_VIEW_DRAGGING,
        NBI_PTR_VIEW_RESIZING,
} nbi_ptr_view_state;

struct nbi_style {
        struct nb_style view;
        struct nb_style button;
        struct nb_style text_box;
};


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


typedef enum nb_identifier {
        /* nb_result */
        NB_OK,             /* success in the api call */
        NB_FAIL,           /* internal failure for instance an allocation */
        NB_INVALID_DESC,   /* a desciption struct is null or has invalid data */
        NB_INVALID_PARAMS, /* one of the parmaters is invalid */

        /* struct id */
        NB_STRUCT_CTX_CREATE,
        NB_STRUCT_VIEW,
        NB_STRUCT_ELEMENT,
        NB_STRUCT_POINTER,
        NB_STRUCT_VIEWPORT,

        /* interaction callbacks */
        NB_INTERACT_DRAGGED,    /* when a window or element is dragged */
        NB_INTERACT_RESIZED,    /* when a window is resized */
        NB_INTERACT_CLOSED,     /* when a window is closed */
        NB_INTERACT_MINIMIZED,  /* when a window is minimized */
        NB_INTERACT_CLICKED,    /* when an element is clicked */

        /* view ids */
        NB_VIEW_WINDOW,
        NB_VIEW_NODE,

        /* element ids */
        NB_ELEMENT_TEXT,
        NB_ELEMENT_BUTTON,
        NB_ELEMENT_DRAG_FIELD,

        NB_DATA_STRING,
        NB_DATA_FLOAT,
        NB_DATA_INT,

        /* styles */
        NB_STYLE_WINDOW,
        NB_STYLE_BUTTON,

        /* nothing past this - allows user to add their own id's */
        NB_ID_COUNT
} nb_identifier;




struct nb_buffer {
        unsigned char *mem;
        int size;
        int capacity;
};

struct nb_buffer*
nbi_buffer_alloc() {
        struct nb_buffer *buf = (struct nb_buffer*)NB_ALLOC(sizeof(buf[0]));
        NB_ZERO_MEM(buf);

        return buf; }

void nbi_buffer_free(struct nb_buffer *buf) { NB_FREE(buf); }


void* nbi_buffer_mem(struct nb_buffer *buf) { return buf->mem; }
int nbi_buffer_capcity(struct nb_buffer *buf) { return buf->capacity; }
int nbi_buffer_used(struct nb_buffer *buf) { return buf->size; }


void*
nbi_buffer_create(struct nb_buffer *buf, int capacity)
{
        NB_ASSERT(buf);
        NB_ASSERT(capacity);

        buf->mem = NB_ALLOC(capacity);
        NB_ZERO_MEM(buf->mem);

        if(buf->mem) {
                buf->size = 0;
                buf->capacity = capacity;
        }

        return (void*)&buf->mem[buf->size];
}


void nbi_buffer_destroy(struct nb_buffer *buf) {
        NB_FREE(buf->mem);
        buf->mem = 0;
}


void*
nbi_buffer_push(struct nb_buffer *buf, int size)
{
        NB_ASSERT(buf);
        NB_ASSERT(size);

        if(buf->size + size > buf->capacity) {
                int needed_space = buf->size + size;
                int capacity = 0;

                while(capacity < needed_space) {
                        capacity = buf->capacity * 2;
                }

                void *new_mem = NB_ALLOC(capacity);
                NB_MEM_CPY(new_mem, buf->mem, buf->size);

                buf->capacity = capacity;
                buf->mem = new_mem;
        }

        void *addr = (void*)&buf->mem[buf->size];
        buf->size += size;

        return addr;
}


void
nbi_buffer_clear(struct nb_buffer *buf)
{
        NB_ASSERT(buf);
        buf->size = 0;
}


// struct nbi_view_data {

//         struct nb_buffer *cached_e_last;
//         struct nb_buffer *cached_e_next;

//         struct nb_buffer cached_e_a; /* array nb_element */
//         struct nb_buffer cached_e_b; /* array nb_element */

//         struct nb_buffer cached_v; /* array nb_view */
// };

struct nb_core_ctx {
        void *user_data;
        unsigned long tick;

        struct nbi_state state;
        struct nbi_style styles;
        // struct nbi_view_data view_data;

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
        struct nb_core_ctx * ctx);


/*
 * return NB_OK if the new frame was submitted
 * return NB_INVALID_PARAMS if ctx is not valid
 * return NB_FAIL if an internal error occured
 */
nb_result
nb_frame_submit(
        struct nb_core_ctx * ctx);


/* ----------------------------------------------------------------- State -- */
/*
 *  Nebula has no concept of the world it lives in so it requires to be told
 *  about mouse events and so forth.
 */

struct nb_pointer_desc {
        int type_id;
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
        struct nb_core_ctx * ctx,
        struct nb_pointer_desc *desc);


struct nb_viewport_desc {
        int type_id;
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
        struct nb_core_ctx * ctx,
        struct nb_viewport_desc *desc);


nb_result
nb_state_set_text_input(struct nb_core_ctx * ctx, char * text);


nb_result
nb_state_set_dt(struct nb_core_ctx * ctx, float dt);


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
        struct nb_core_ctx * ctx,
        struct nb_state *out_state);


#endif


/* ================================== IMPL ================================== */


#ifdef NEB_CORE_IMPL
#ifndef NEB_CORE_IMPL_INCLUDED
#define NEB_CORE_IMPL_INCLUDED


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
                return NB_INVALID_PARAMS;
        }

        if(desc->type_id != NB_STRUCT_POINTER) {
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
                        state->ptr_state = NBI_PTR_UP_EVENT;                }
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
        unsigned int len_max = NB_ARRAY_COUNT(state->text_input) - 1;
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

        return NB_OK;
}


#endif
#endif
