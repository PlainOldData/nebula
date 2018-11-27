/*
 * Nebula Immediate UI
 */


#ifndef NEBULA_INTERFACE_INCLUDED
#define NEBULA_INTERFACE_INCLUDED


/* ---------------------------------------------------------- Common Types -- */
/*
 *  General types and identifiers for Nebula, checking/logging the return codes
 *  will generally reveal any issues in the calling code.
 */


typedef struct nb_ctx * nb_ctx_t;
typedef struct nb_view * nb_view_t;
typedef int nb_result;
typedef unsigned int nb_color;

typedef void(*nb_interaction_fn)(nb_ctx_t ctx, int interaction, void *ud);


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


/*
 *  2D envelop, represents the top left point, and its size.
 */
struct nb_env {
        float point[2];
        float size[2];
};


/* -------------------------------------------------------------- Lifetime -- */
/*
 *  All API entry points take a context as the first paramater. This will be
 *  your first call to the Nebula API.
 *
 */

struct nb_ctx_create_desc {
        int type_id;        /* must be NB_STRUCT_CTX_CREATE */
        void *ext;          /* no use */

        void *user_data;    /* user data */
};


/*
 * returns NB_OK if the ctx was created
 * returns NB_INVALID_PARAMS if desc was null
 * returns NB_INVALID_PARAMS if new_ctx was null
 * returns NB_INVALID_DESC if desc type_id is not NB_STRUCT_CTX_CREATE
 * returns NB_FAIL if an internal error occured
 */
nb_result
nb_ctx_create(
        struct nb_ctx_create_desc *desc,
        nb_ctx_t *new_ctx);


/*
 * returns NB_OK if the ctx was destroyed
 * returns NB_INVALID_PARAMS if destroy_ctx is null
 * returns NB_FAIL if an internal error occured
 */
nb_result
nb_ctx_destroy(
        nb_ctx_t *destroy_ctx);


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
        nb_ctx_t ctx);


/*
 * return NB_OK if the new frame was submitted
 * return NB_INVALID_PARAMS if ctx is not valid
 * return NB_FAIL if an internal error occured
 */
nb_result
nb_frame_submit(
        nb_ctx_t ctx);


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
        nb_ctx_t ctx,
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
        nb_ctx_t ctx,
        struct nb_viewport_desc *desc);

nb_result
nb_state_set_text_input(nb_ctx_t ctx, char * text);

nb_result
nb_state_set_dt(nb_ctx_t ctx, float dt);

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
 * returns NB_OK on success
 * returns NB_INVALID_PARAMS if ctx or out_state is null
 * returns NB_FAIL if an internal error occured
 */
nb_result
nb_state_get(
        nb_ctx_t ctx,
        struct nb_state *out_state);


/* ------------------------------------------------------------------ View -- */
/*
 *  Prototyping ideas for how views are created.
 *
 *  Unlike other imgui's nebular works on views, the advantage to this is it
 *  can be multithreaded. This isn't really so much a performance point of view
 *  but a practically of the modern world.
 *
 *  Internally I would see the view object holding the cached data. This also
 *  makes some things cleaner in the impl.
 */


enum nb_view_flags {
        NB_VIEW_RESIZE = 1 << 0,
        NB_VIEW_CLOSE = 1 << 1,
        NB_VIEW_MINIMIZE = 1 << 2,
        NB_VIEW_DRAG = 1 << 3,
        NB_VIEW_TITLE = 1 << 4,
};


enum nb_view_interaction_flags {
        NB_VIEW_INTERACTION_CLOSED = 1 << 0,
        NB_VIEW_INTERACTION_RESIZED = 1 << 1,
        NB_VIEW_INTERACTION_DRAGGED = 1 << 2,
        NB_VIEW_INTERACTION_HOVER = 1 << 3,
        NB_VIEW_INTERACTION_MINIMIZE = 1 << 4,
};


struct nb_view_desc {
        int type_id;
        void *ext;

        const char *name;
        int flags;
        int view_type; /* window or node or menubar */

        int width, height;
        int pos_x, pos_y;

        struct nb_style *style; /* override default styles */

        void *user_data; /* passed back in the callbacks */
};


nb_result
nb_view_create(
        nb_ctx_t ctx,
        struct nb_view_desc *desc,
        nb_view_t *out_view,
        int *out_view_interaction_flags);


nb_result
nb_view_submit(
        nb_view_t view);


/* ----------------------------------------------------------- UI Elements -- */
/*
 *  Prototyping ideas for how elements are created.
 *
 *  Hopefully this will work. A single entry point for all UI, the idea behind
 *  this is to make for a very stable interface, rather than having a bazzilion
 *  different entry points that may or may not change. however this means a more
 *  generalized solution which might not be straight forward.
 */


enum nb_element_flags {
        NB_ELEMENT_INTERACTABLE = 1 << 0,
        NB_ELEMENT_DRAGABLE = 1 << 1,
        NB_ELEMENT_DROPABLE = 1 << 2,
};


enum nb_element_interaction_flags {
        NB_INTERACTION_CLICKED = 1 << 0,
        NB_INTERACTION_HOVER = 1 << 1,
        NB_INTERACTION_DOWN = 1 << 2,
        NB_INTERACTION_DRAGGED_LEFT = 1 << 3,
        NB_INTERACTION_DRAGGED_RIGHT = 1 << 4,
        NB_INTERACTION_TEXT_STREAM = 1 << 5,
};

struct nb_element_desc {
        int type_id;
        void *ext;

        const char *name;
        unsigned int flags;

        void *content;
        int content_type;
        int content_size;

        void *data;
        int data_type;

        struct nb_style *style;
};


struct nb_interaction {
        unsigned int flags;
        void *dropped;
};

/* Icons
 * https://fontawesome.com/cheatsheet#solid
 * http://www.ltg.ed.ac.uk/~richard/utf-8.cgi
 */

#define NB_ICON_ANGLE_DOUBLE_RIGHT "\357\204\201"
#define NB_ICON_ANGLE_DOUBLE_LEFT "\357\204\200"

#define NB_ICON_MOUSE_POINTER "\357\211\205"
#define NB_ICON_EXPAND_ARROWS_ALT "\357\214\236"

#define NB_ICON_MINUS "\357\201\250"
#define NB_ICON_PLUS "\357\201\247"
#define NB_ICON_TIMES "\357\200\215"

#define NB_ICON_ALIGN_LEFT "\357\200\266"
#define NB_ICON_ALIGN_RIGHT "\357\200\270"
#define NB_ICON_ALIGN_CENTER "\357\200\267"

#define NB_ICON_CAT "\357\232\276"
#define NB_ICON_DOG "\357\233\223"
#define NB_ICON_KIWI_BIRD "\357\224\265"

#define NB_ICON_GHOST "\357\233\242"
#define NB_ICON_SKULL "\357\225\214"

#define NB_ICON_CODE "\357\204\241"


typedef enum nb_text_align {
        NB_TEXT_ALIGN_LEFT = 0,
        NB_TEXT_ALIGN_RIGHT = 1,
        NB_TEXT_ALIGN_CENTER = 2,

        _NB_TEXT_ALIGN_BIT_MASK = 3,
} nb_text_align;


struct nb_element_text {
        char * text;
        unsigned int len_max;
        unsigned int user_input;
        unsigned int align_type;
};


/*
 * Submits the data for a UI element, outputs interaction flags.
 * returns NB_OK on success
 * returns NB_FAIL on internal error
 */
nb_result
nb_element_create(
        nb_view_t view,
        struct nb_element_desc *desc,
        struct nb_interaction *out_interactions,
        float *out_pos_v2); /* optional */


/* ---------------------------------------------------------------- Styles -- */


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


nb_result
nb_style_default_get(
        nb_ctx_t ctx,
        nb_identifier selector,
        struct nb_style *out_style);


nb_result
nb_style_default_set(
        nb_ctx_t ctx,
        nb_identifier selector,
        const struct nb_style *style);


/* ------------------------------------------------------------- Rendering -- */
/*
 *  Nebula doesn't directly handle rendering of the UI, its generally for the
 *  calling code to deal with. This API is how to extract the data required
 *  for rendering.
 */


typedef enum nb_render_cmd_type {
        NB_RENDER_CMD_TYPE_TRIANGLES = 0,
        NB_RENDER_CMD_TYPE_LINES = 1,
        NB_RENDER_CMD_TYPE_SCISSOR = 2,
} nb_render_cmd_type;

struct nb_render_elem {
        unsigned int offset;
        unsigned int count;
};

union nb_render_cmd_data {
        struct nb_render_elem elem;
        short clip_rect[4];
};

struct nb_render_cmd {
        unsigned int type;
        union nb_render_cmd_data data;
};

struct nb_render_cmd_list {
        struct nb_render_cmd * cmds;
        unsigned int count;
};

struct nb_render_data {
        float * vtx;
        unsigned int vtx_count;

        unsigned short * idx;
        unsigned int idx_count;

        struct nb_render_cmd_list cmd_lists[64];
        unsigned int cmd_list_count;
};

/* This isn't going to stay around */
#define NB_FONT_COUNT_MAX 16

struct nb_font_tex {
        unsigned char * mem;
        unsigned int width;
};


/*
 * Get the render cmds
 * returns NB_OK on success
 * returns NB_FAIL if an internal error occured
 * returns NB_INVALID_PARAMS if ctx or data is null
 */
nb_result
nb_get_render_data(nb_ctx_t ctx, struct nb_render_data * data);

unsigned int
nb_get_font_count(nb_ctx_t ctx);

nb_result
nb_get_font_tex_list(nb_ctx_t ctx, struct nb_font_tex * tex_list);

/* DEBUG!! */
nb_result
nb_debug_set_font(nb_ctx_t ctx, unsigned int idx);

/* DEBUG!! */
unsigned int
nb_debug_get_font(nb_ctx_t ctx);


/* ------------------------------------------------------- Private Access -- */
/*
 * This is to allow unit testing of private interfaces.
 */


#ifdef NB_PRIVATE_ACCESS


struct nb_buffer;

struct nb_buffer* nbi_buffer_alloc();
void nbi_buffer_free(struct nb_buffer *buf);

void* nbi_buffer_create(struct nb_buffer *buf, int capacity);
void nbi_buffer_destroy(struct nb_buffer *buf);
void* nbi_buffer_push(struct nb_buffer *buf, int size);
void* nbi_buffer_mem(struct nb_buffer *buf);
int nbi_buffer_capcity(struct nb_buffer *buf);
int nbi_buffer_used(struct nb_buffer *buf);

/* private access */
#endif


/* inc guard */
#endif


/* -- IMPL -- */


#ifdef NEBULA_IMPL
#ifndef NEBULA_IMPL_INCLUDED
#define NEBULA_IMPL_INCLUDED


/* -------------------------------------------------------------- Internal -- */


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

#define NB_NULL 0

#include <stdlib.h>
#define NB_ALLOC(bytes) malloc(bytes)
#define NB_FREE(addr) free(addr)


#ifndef NB_NO_ASSERT
#include <assert.h>
#define NB_ASSERT(expr) assert(expr)
#else
#define NB_ASSERT(expr)
#endif


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


#include <math.h>

#define NB_TAU 6.2831853071f

#define NB_ARRAY_COUNT(ARRAY) (sizeof((ARRAY)) / sizeof((ARRAY)[0]))
#define NB_ARRAY_DATA(ARRAY) &ARRAY[0]


static float NB_COLOR_WHITE[4] = { 1.0f, 1.0f, 1.0f, 1.0f, };
static float NB_COLOR_DARK_GRAY[4] = { 0.3f, 0.3f, 0.3f, 1.0f, };
static float NB_COLOR_LIGHT_GRAY[4] = { 0.7f, 0.7f, 0.7f, 1.0f, };

#include "nebula_font_awesome.h"
#include "nebula_font_open_sans.h"
#include "nebula_font_proggy.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include <stdio.h>

/* https://stackoverflow.com/questions/2535284/how-can-i-hash-a-string-to-an-int-using-c#13487193 */
uint64_t
nbi_hash_str(const char *name) {
        uint64_t hash = 5381;
        int c;

        while (c = *name++, c) {
                hash = ((hash << 5) + hash) + c;
        }

        return hash;
}


/* https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key#12996028 */
uint64_t
nbi_hash_uint(uint64_t x) {
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
        x = x ^ (x >> 31);
        return x;
}


uint64_t
nbi_u64_pack(uint16_t a, uint16_t b, uint16_t c, uint16_t d) {
        uint64_t x = 0;
        x |= (uint64_t)a << 48;
        x |= (uint64_t)b << 32;
        x |= (uint64_t)c << 16;
        x |= (uint64_t)d;
        return x;
}


static void
nbi_color_u32_to_flt(uint32_t c, float *color_4)
{
        uint8_t c1 = (c >> 24) & 0xFF;
        color_4[0] = (float)(c1) / 255.f;

        uint8_t c2 = (c >> 16) & 0xFF;
        color_4[1] = (float)(c2) / 255.f;

        uint8_t c3 = (c >> 8) & 0xFF;
        color_4[2] = (float)(c3) / 255.f;

        uint8_t c4 = (c >> 0) & 0xFF;
        color_4[3] = (float)(c4) / 255.f;
}


static void
nbi_color_copy(float * src, float * dst) {
        memcpy(dst, src, sizeof(float) * 4);
}


/* ---------------------------------------------------------------- Buffer -- */
/*
 *  Buffers are how nebula store arrays of information
 */

/* some sorta buffer impl */
struct nb_buffer {
        unsigned char *mem;
        int size;
        int capacity;
};


/* allows unit test to create a buffer */
#ifdef NB_PRIVATE_ACCESS

struct nb_buffer*
nbi_buffer_alloc() {
        struct nb_buffer *buf = (struct nb_buffer*)NB_ALLOC(sizeof(buf[0]));
        NB_ZERO_MEM(buf);

        return buf; }

void nbi_buffer_free(struct nb_buffer *buf) { NB_FREE(buf); }

#endif

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


/* --------------------------------------------------- Internal State Data -- */


struct nbi_view_data {

        struct nb_buffer *cached_e_last;
        struct nb_buffer *cached_e_next;

        struct nb_buffer cached_e_a; /* array nb_element */
        struct nb_buffer cached_e_b; /* array nb_element */

        struct nb_buffer cached_v; /* array nb_view */
};


/* -------------------------------------------------- Internal Render Cmds -- */

struct nbi_vtx_buf {
        float * v;
        unsigned int v_count;

        unsigned short * i;
        unsigned int i_count;
};

struct nbi_cmd_buf {
        struct nb_ctx * ctx;

        struct nb_render_cmd cmds[4096];
        uint32_t cmd_count;
};

void
nbi_box(struct nbi_cmd_buf * buf, float * rect, float * color, float radius);


void
nbi_line(struct nbi_cmd_buf * buf, float * p, float * q, float * color);


void
nbi_bez(struct nbi_cmd_buf * buf,
        float * p0,
        float * p1,
        float * p2,
        float * p3,
        float * color);

enum nbi_text_flags {
        NBI_TEXT_FLAGS_CURSOR = 1 << 15,
        NBI_TEXT_FLAGS_WRAP = 1 << 14,
        NBI_TEXT_FLAGS_TERM = 1 << 13,
};

void
nbi_get_text_size(struct nb_ctx * ctx, float width, uint32_t flags, const char * str, float * out_size);

void
nbi_text(struct nbi_cmd_buf * buf, float * pos, uint32_t flags, float * color, const char * str);


void nbi_scissor_set(struct nbi_cmd_buf * buf, float * rect);
void nbi_scissor_clear(struct nbi_cmd_buf * buf);


/* ---------------------------------------------------------- Common Types -- */

struct nb_view {
        nb_ctx_t ctx;
        uint64_t hash;
        uint64_t user_desc_hash;
        void *user_data;
        int pos[2];
        int size[2];
        int inner_size[2];

        int mini;
        uint64_t exists;

        struct nbi_cmd_buf * cmds;

        int cursor[2];

        int is_interacting;
};


struct nb_element {
        uint64_t view_hash;
        uint64_t hash;
        void *user_data;
        int pos[2];
        int size[2];
};


typedef enum nbi_ptr_state {
        NBI_PTR_UP,
        NBI_PTR_UP_EVENT,
        NBI_PTR_DOWN,
        NBI_PTR_DOWN_EVENT,
} nbi_ptr_state;


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

        uint64_t ptr_view;
        uint64_t ptr_ele;
        nbi_ptr_view_state ptr_view_action;

        uint64_t ptr_ele_drop;
        void* ptr_ele_drag;

        uint64_t focus_ele;
        uint32_t focus_time;

        int vp_size[2];

        char text_input[4096];
        uint32_t text_cursor;
        float text_cursor_time;

        float dt;
};

struct nbi_font_range {
        uint32_t start;
        uint32_t end;
        float height;
};

#define NBI_FONT_RANGE_COUNT_MAX 4

struct nbi_font {
        struct nb_font_tex tex;

        stbtt_packedchar * range_data[NBI_FONT_RANGE_COUNT_MAX];
        struct nbi_font_range ranges[NBI_FONT_RANGE_COUNT_MAX];
        uint32_t range_count;

        float height;
        float space_width;
};

struct nbi_style {
        struct nb_style view;
        struct nb_style button;
        struct nb_style text_box;
};

struct nb_ctx {
        void *user_data;
        uint64_t tick;

        struct nbi_state state;
        struct nbi_style styles;
        struct nbi_view_data view_data;

        struct nbi_font fonts[NB_FONT_COUNT_MAX];
        unsigned int font_count;
        struct nbi_font * font;
        struct nbi_font * debug_font_next;

        struct nbi_vtx_buf vtx_buf;
};


/* -------------------------------------------------------------- Lifetime -- */

uint32_t
nbi_get_font_range_idx(struct nbi_font * font, uint32_t cp) {
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

uint32_t
nbi_char_valid(struct nbi_font * font, uint32_t cp) {
        uint32_t range_idx = nbi_get_font_range_idx(font, cp);
        uint32_t result = range_idx < font->range_count ? 1 : 0;
        return result;
}

void
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

float
nbi_get_glyph_width(struct nbi_font * font, uint32_t cp) {
        float result = 0.0f;
        if(nbi_char_valid(font, cp)) {
                float y = 0.0f;
                stbtt_aligned_quad q;
                nbi_get_glyph_quad(font, cp, &result, &y, &q);
        }
        return result;
}

void
nbi_push_font_range(struct nbi_font * font, stbtt_pack_context * stbtt, uint8_t * ttf, uint32_t start, uint32_t end, float height) {
        uint32_t idx = font->range_count++;
        struct nbi_font_range * range = font->ranges + idx;
        *range = (struct nbi_font_range) {
                .start = start,
                .end = end,
                .height = height,
        };

        uint32_t char_count = range->end - range->start;
        font->range_data[idx] = NB_ALLOC(sizeof(stbtt_packedchar) * char_count);
        stbtt_PackFontRange(stbtt, ttf, 0, height, range->start, char_count, font->range_data[idx]);
}

void
nbi_font_init(struct nbi_font * font, uint8_t * ttf, float height) {
        font->tex.width = 512;
        font->tex.mem = NB_ALLOC(font->tex.width * font->tex.width);

        font->height = height;

        stbtt_pack_context stbtt;
        stbtt_PackBegin(&stbtt, font->tex.mem, font->tex.width, font->tex.width, font->tex.width, 1, 0);
        /*stbtt_PackSetOversampling(&stbtt, 2, 2);*/

        font->range_count = 0;
        nbi_push_font_range(font, &stbtt, ttf, 32, 127, height);
        nbi_push_font_range(font, &stbtt, NB_FONT_AWESOME_TTF, NB_FA_CODE_MIN, NB_FA_CODE_MAX, 12.0f);

        stbtt_PackEnd(&stbtt);

        font->space_width = nbi_get_glyph_width(font, ' ');
}

nb_result
nb_ctx_create(
        struct nb_ctx_create_desc *desc,
        nb_ctx_t *new_ctx)
{
        struct nb_ctx *ctx = 0;

        if(!desc || !new_ctx) {
                NB_ASSERT(0 && "NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        if(desc->type_id != NB_STRUCT_CTX_CREATE) {
                NB_ASSERT(0 && "NB_INVALID_DESC");
                return NB_INVALID_DESC;
        }

        ctx = NB_ALLOC(sizeof(*ctx));

        if(!ctx) {
                return NB_FAIL;
        }

        NB_ZERO_MEM(ctx);

        nbi_buffer_create(&ctx->view_data.cached_e_a, 1024);
        nbi_buffer_create(&ctx->view_data.cached_e_b, 1024);

        ctx->view_data.cached_e_last = &ctx->view_data.cached_e_a;
        ctx->view_data.cached_e_next = &ctx->view_data.cached_e_b;

        nbi_buffer_create(&ctx->view_data.cached_v, 1024);

        ctx->vtx_buf = (struct nbi_vtx_buf) {
                .v = NB_ALLOC(65536 * sizeof(float)),
                .i = NB_ALLOC(65536 * sizeof(uint16_t)),
        };

        struct nbi_font_info { uint8_t * ttf; float height; };
        struct nbi_font_info finfo[] = {
                { .ttf = NB_OPEN_SANS_TTF, .height = 16.0f, },
                { .ttf = NB_PROGGY_TTF, .height = 11.0f, },
        };

        ctx->font_count = NB_ARRAY_COUNT(finfo);
        if(ctx->font_count > NB_ARRAY_COUNT(ctx->fonts)) {
                NB_ASSERT(!"Specfied more fonts than can fit in fixed-size font array!!");
                ctx->font_count = NB_ARRAY_COUNT(ctx->fonts);
        }

        uint32_t i;
        for(i = 0; i < ctx->font_count; i++) {
                nbi_font_init(ctx->fonts + i, finfo[i].ttf, finfo[i].height);
        }

        ctx->font = ctx->fonts;

        /* create default styles */
        ctx->styles.view.bg_color = 0x575459FF;
        ctx->styles.view.bg_color_hover = 0x6C726BFF;
        ctx->styles.view.border_color = 0xD36745FF;
        ctx->styles.view.text_color = 0xCFE4B6FF;
        ctx->styles.view.border_size = 2;
        ctx->styles.view.padding = 5;
        ctx->styles.view.margin = 0;
        ctx->styles.view.radius = 10;

        ctx->styles.button.bg_color = 0xD36745FF;
        ctx->styles.button.bg_color_hover = 0xFAD595FF;
        ctx->styles.button.text_color = 0xCFE4B6FF;
        ctx->styles.button.padding = 0;
        ctx->styles.button.margin = 0;
        ctx->styles.button.radius = 10;

        ctx->styles.text_box = (struct nb_style) {
                .bg_color = 0xB3B3B3FF,
                .bg_color_hover = 0xD1D1D1FF,
                .text_color = 0xCFE4B6FF,
                .border_color = 0xD36745FF,
                .radius = 5,
        };

        *new_ctx = ctx;

        return NB_OK;
}

nb_result
nb_ctx_destroy(
        nb_ctx_t *destroy_ctx)
{
        if(!destroy_ctx || !*destroy_ctx) {
                NB_ASSERT(0 && "NB_INVALID_DESC");
                return NB_INVALID_PARAMS;
        }

        printf("leaking views and view cmds");

        NB_FREE((void*)*destroy_ctx);
        *destroy_ctx = 0;

        return NB_OK;
}


/* ----------------------------------------------------------------- Frame -- */

int
nbi_point_inside_rect(int *point, int *rxy, int *rwh) {
        int result = 0;
        int rx = rxy[0]; int ry = rxy[1];
        int rw = rwh[0]; int rh = rwh[1];
        int px = point[0]; int py = point[1];

        if (px > rx && px < (rx + rw)) {
                if (py > ry && py < (ry + rh)) {
                        result = 1;
                }
        }
        return result;
}

static float
nbi_clamp_f32(float x, float min_val, float max_val) {
        float result = x;
        if(x < min_val) {
                result = min_val;
        }
        else if(x > max_val) {
                result = max_val;
        }
        return result;
}

void
nbi_rect_intersect(float * bound, float * rect, float * out) {
        float bound_x2 = bound[0] + bound[2];
        float bound_y2 = bound[1] + bound[3];

        out[0] = nbi_clamp_f32(rect[0], bound[0], bound_x2);
        out[1] = nbi_clamp_f32(rect[1], bound[1], bound_y2);
        out[2] = nbi_clamp_f32(rect[0] + rect[2], bound[0], bound_x2) - out[0];
        out[3] = nbi_clamp_f32(rect[1] + rect[3], bound[1], bound_y2) - out[1];
}

int
nbi_ptr_inside_ele(
        struct nb_ctx *ctx,
        struct nb_element *ele,
        uint64_t view_hash)
{
        int result = 0;
        if (ele->view_hash == view_hash) {
                int * ptr_pos = ctx->state.ptr_pos;
                result = nbi_point_inside_rect(ptr_pos, ele->pos, ele->size);
        }
        return result;
}


int
nbi_ptr_inside_view(
        struct nb_ctx *ctx,
        struct nb_view *view)
{
        int * ptr_pos = ctx->state.ptr_pos;
        int result = nbi_point_inside_rect(ptr_pos, view->pos, view->size);

        return result;
}


void
nbi_frame_interaction(struct nb_ctx *ctx) {
        struct nbi_state *st = &ctx->state;

        struct nb_buffer *buf = &ctx->view_data.cached_v;
        struct nb_view *view = nbi_buffer_mem(buf);

        unsigned int stride = sizeof(*view);
        unsigned int count = nbi_buffer_used(buf) / stride;
        unsigned int i;

        struct nb_view *iview = 0;

        /* find with hash */
        if(st->ptr_view) {
                NB_ASSERT(0); /* did this get called */
                // printf("search for %llu\n", st->ptr_view);
                for(i = 0; i < count; ++i) {
                        int this_tick = view[i].exists == ctx->tick;
                        int this_hash = view[i].hash == st->ptr_view;

                        if (this_tick && this_hash) {
                                iview = &view[i];
                                break;
                        }
                }
        }
        /* pick a view */
        else {
                for (i = 0; i < count; ++i) {
                        struct nb_view * it = view + i;

                        int this_tick = view[i].exists == ctx->tick;

                        if (!this_tick) {
                                continue;
                        }

                        if(nbi_ptr_inside_view(ctx, it)) {
                                /*printf("picked up: %llu\n", it->hash);*/
                                st->ptr_view = it->hash;
                                iview = it;
                                break;
                        }
                }
        }

        uint64_t focus_ele = 0;

        /* check to see if we are interacting with an element */
        if(iview) {
                /* pick an element */
                buf = ctx->view_data.cached_e_last;
                struct nb_element *element = nbi_buffer_mem(buf);
                stride = sizeof(*element);
                count = nbi_buffer_used(buf) / stride;

                for (i = 0; i < count; ++i) {
                        struct nb_element *ele = &element[i];

                        if (nbi_ptr_inside_ele(ctx, ele, st->ptr_view)) {
                                uint64_t hash = ele->hash;
                                st->ptr_ele = hash;
                                focus_ele = hash;

                                break;
                        }
                }
        }

        if(st->ptr_state == NBI_PTR_DOWN_EVENT) {
                if(st->focus_ele != focus_ele) {
                        st->focus_ele = focus_ele;
                        st->focus_time = 0;
                }
        }
}


struct nb_element *
nbi_find_element_by_hash(struct nb_buffer * buf, uint64_t hash_key) {
        struct nb_element * result = 0;

        struct nb_element * elems = nbi_buffer_mem(buf);
        uint32_t elem_count = nbi_buffer_used(buf) / sizeof(*elems);
        uint32_t i;

        for(i = 0; i < elem_count; i++) {
                struct nb_element * it = elems + i;
                if(it->hash == hash_key) {
                        result = it;
                        break;
                }
        }

        return result;
}

nb_result
nb_frame_begin(
        nb_ctx_t ctx)
{
        if(!ctx) {
                NB_ASSERT(0 && "NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        struct nbi_state *st = &ctx->state;

        if(st->ptr_state != NBI_PTR_DOWN) {
                st->ptr_view = 0;
                st->ptr_ele = 0;
        }

        struct nb_buffer *old_next = ctx->view_data.cached_e_next;
        struct nb_buffer *old_last = ctx->view_data.cached_e_last;

        nbi_buffer_clear(old_last);

        ctx->view_data.cached_e_next = old_last;
        ctx->view_data.cached_e_last = old_next;

        if(st->focus_ele) {
                struct nb_element * focus_ele = nbi_find_element_by_hash(ctx->view_data.cached_e_last, st->focus_ele);
                if(focus_ele) {
                        st->focus_time++;
                }
                else {
                        st->focus_ele = 0;
                }
        }

        /* interaction */
        if(st->ptr_view == 0) {
                nbi_frame_interaction(ctx);
        }

        ctx->state.text_cursor_time += ctx->state.dt;

        /* DEBUG!! */
        if(ctx->debug_font_next) {
                ctx->font = ctx->debug_font_next;
                ctx->debug_font_next = 0;
        }

        ctx->vtx_buf.v_count = 0;
        ctx->vtx_buf.i_count = 0;

        /* clear cmds */
        struct nb_buffer *buf = &ctx->view_data.cached_v;
        struct nb_view *view = nbi_buffer_mem(buf);

        unsigned int stride = sizeof(*view);
        unsigned int count = nbi_buffer_used(buf) / stride;
        unsigned int i;

        for(i = 0; i < count; ++i) {
                view[i].cmds->cmd_count = 0;
        }

        ctx->tick += 1;

        return NB_OK;
}


nb_result
nb_frame_submit(
        nb_ctx_t ctx)
{
        if(!ctx) {
                NB_ASSERT(0 && "NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        return NB_OK;
}


/* ----------------------------------------------------------------- State -- */


nb_result
nb_state_set_pointer(
        nb_ctx_t ctx,
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
        nb_ctx_t ctx,
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
nb_state_set_text_input(nb_ctx_t ctx, char * text) {
        struct nbi_state *state = &ctx->state;

        char * src = text;
        char * dst = state->text_input;
        uint32_t len_max = NB_ARRAY_COUNT(state->text_input) - 1;
        uint32_t len = 0;
        while(*src && len < len_max) {
                *dst++ = *src++;
                len++;
        }
        *dst = 0;

        return NB_OK;
}

nb_result nb_state_set_dt(nb_ctx_t ctx, float dt) {
        NB_ASSERT(ctx);
        ctx->state.dt = dt;
        return NB_OK;
}


nb_result
nb_state_get(
        nb_ctx_t ctx,
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


/* ------------------------------------------------------------------ View -- */

static void
nbi_get_element_clip(struct nb_view * view, float * out_rect) {
        out_rect[0] = (float)view->pos[0];
        out_rect[1] = (float)view->pos[1];
        out_rect[2] = (float)view->size[0];
        out_rect[3] = (float)view->size[1];
}

void
nbi_resize_handle_data(
        const struct nb_view *view,
        float *out_pos2,
        float *out_size2,
        float *out_color4)
{
        if(out_pos2) {
                out_pos2[0] = (float)(view->pos[0] + view->size[0] - 25);
                out_pos2[1] = (float)(view->pos[1] + view->size[1] - 25);
        }

        if(out_size2) {
                out_size2[0] = 20.f;
                out_size2[1] = 20.f;
        }

        if(out_color4) {
                nbi_color_copy(NB_COLOR_LIGHT_GRAY, out_color4);
        }
}


void
nbi_close_handle_data(
        const struct nb_view *view,
        float *out_pos2,
        float *out_size2,
        float *out_color4)
{
        if(out_pos2) {
                out_pos2[0] = (float)(view->pos[0] + view->size[0] - 25);
                out_pos2[1] = (float)(view->pos[1] + 5);
        }

        if(out_size2) {
                out_size2[0] = 20.f;
                out_size2[1] = 20.f;
        }

        if(out_color4) {
                nbi_color_copy(NB_COLOR_LIGHT_GRAY, out_color4);
        }
}


void
nbi_min_handle_data(
        const struct nb_view *view,
        float *out_pos2,
        float *out_size2,
        float *out_color4)
{
        if(out_pos2) {
                out_pos2[0] = (float)(view->pos[0] + view->size[0] - 55);
                out_pos2[1] = (float)(view->pos[1] + 5);
        }

        if(out_size2) {
                out_size2[0] = 20.f;
                out_size2[1] = 20.f;
        }

        if(out_color4) {
                nbi_color_copy(NB_COLOR_LIGHT_GRAY, out_color4);
        }
}

void
nbi_view_border_data(
        const struct nb_style *style,
        const struct nb_view *view,
        float *out_pos2,
        float *out_size2,
        float *out_color4)
{
        int size = style->border_size;

        if(out_pos2) {
                out_pos2[0] = (float)view->pos[0] - size;
                out_pos2[1] = (float)view->pos[1] - size;
        }

        if (out_size2 && view->mini) {
                out_size2[0] = (float)(view->size[0] + size * 2);
                out_size2[1] = 30.0f + (float)(size * 2);
        }
        else if (out_size2) {
                out_size2[0] = (float)(view->size[0] + size * 2);
                out_size2[1] = (float)(view->size[1] + size * 2);
        }

        if(out_color4) {
                nbi_color_u32_to_flt(style->border_color, out_color4);
        }
}

void
nbi_push_view_border(
        struct nb_style *style,
        struct nb_view *view)
{
        float radius = style->radius ? style->radius + (float)style->border_size: 0.0f;
        float rect[4]; float color[4];
        nbi_view_border_data(style, view, rect, rect + 2, color);
        nbi_box(view->cmds, rect, color, radius);
}

void
nbi_view_background_data(
        const struct nb_style *style,
        const struct nb_view *view,
        float *out_pos2,
        float *out_size2,
        float *out_col4,
        int hover)
{
        if (out_pos2) {
                out_pos2[0] = (float)view->pos[0];
                out_pos2[1] = (float)view->pos[1];
        }

        if (out_size2 && view->mini) {
                out_size2[0] = (float)(view->size[0]);
                out_size2[1] = 30.f;
        }
        else if (out_size2) {
                out_size2[0] = (float)(view->size[0]);
                out_size2[1] = (float)(view->size[1]);
        }

        if (out_col4 && hover) {
                nbi_color_u32_to_flt(style->bg_color_hover, out_col4);
        }
        else if (out_col4) {
                nbi_color_u32_to_flt(style->bg_color, out_col4);
        }
}

void
nbi_push_view_background(
        struct nb_style *style,
        struct nb_view *view,
        int hover)
{
        float rect[4]; float color[4];
        nbi_view_background_data(style, view, rect, rect + 2, color, hover);
        nbi_box(view->cmds, rect, color, (float)style->radius);
}

void
nbi_push_view_title(
        struct nb_style *style,
        struct nb_view *view,
        const char * name)
{
        float rect[4] = {
                (float)view->pos[0], (float)view->pos[1],
                (float)view->size[0], 30.0f,
        };
        nbi_box(view->cmds, rect, NB_COLOR_DARK_GRAY, (float)style->radius);

        float text_pad = 9.0f;
        float text_rect[4] = { rect[0] + text_pad, rect[1] + 18.0f, 0.0f, 0.0f };
        nbi_scissor_set(view->cmds, rect);
        nbi_text(view->cmds, text_rect, NBI_TEXT_FLAGS_TERM, NB_COLOR_WHITE, name);
        nbi_scissor_clear(view->cmds);
}


struct nb_view *
nbi_find_cached_view(
        struct nb_ctx *ctx,
        uint64_t hash_key)
{
        struct nb_buffer *buf = &ctx->view_data.cached_v;
        struct nb_view *view = nbi_buffer_mem(buf);

        unsigned int stride = sizeof(*view);
        unsigned int count = nbi_buffer_used(buf) / stride;
        unsigned int i;

        for (i = 0; i < count; ++i) {
                if (view[i].hash == hash_key) {
                        return &view[i];
                }
        }

        return 0;
}


struct nb_view *
nbi_move_to_front(
        struct nb_ctx *ctx,
        uint64_t hash_key)
{
        struct nb_buffer *buf = &ctx->view_data.cached_v;
        struct nb_view *view = nbi_buffer_mem(buf);

        unsigned int stride = sizeof(*view);
        unsigned int count = nbi_buffer_used(buf) / stride;
        unsigned int i;
        int found = 0;

        for (i = 0; i < count; ++i) {
                if (view[i].hash == hash_key) {
                        found = 1;
                        break;
                }
        }

        NB_ASSERT(found);

        struct nb_view tmp = view[i];
        memmove(&view[1], &view[0], stride * i);

        view[0] = tmp;

        return &view[0];
}


int
nbi_is_dragging(
        struct nb_ctx *ctx,
        uint64_t hash_key)
{
        if(ctx->state.ptr_state != NBI_PTR_DOWN) {
                return 0;
        }

        if (ctx->state.ptr_ele) {
                return 0;
        }

        return hash_key == ctx->state.ptr_view ? 1 : 0;
}


int
nbi_is_resizing(
        struct nb_ctx *ctx,
        uint64_t hash_key,
        struct nb_view *view)
{
        if (ctx->state.ptr_state != NBI_PTR_DOWN) {
                return 0;
        }

        if (ctx->state.ptr_ele) {
                return 0;
        }

        if (hash_key != ctx->state.ptr_view) {
                return 0;
        }

        /* copy and move if it where moving */
        struct nb_view cpy = *view;
        cpy.size[0] += ctx->state.ptr_delta[0];
        cpy.size[1] += ctx->state.ptr_delta[1];

        float pos[2]; float dim[2]; float color[4];
        nbi_resize_handle_data(&cpy, pos, dim, color);

        int posi[2];
        posi[0] = (int)pos[0]; posi[1] = (int)pos[1];

        int dimi[2];
        dimi[0] = (int)dim[0]; dimi[1] = (int)dim[1];

        return nbi_point_inside_rect(ctx->state.ptr_pos, posi, dimi);
}


int
nbi_is_hovered(
        struct nb_ctx *ctx,
        uint64_t hash_key)
{
        return hash_key == ctx->state.ptr_view ? 1 : 0;
}


int
nbi_is_closing(
        struct nb_ctx *ctx,
        uint64_t hash_key,
        struct nb_view *view)
{
        if (ctx->state.ptr_state != NBI_PTR_UP_EVENT) {
                return 0;
        }

        if (ctx->state.ptr_ele) {
                return 0;
        }

        if (hash_key != ctx->state.ptr_view) {
                return 0;
        }

        float pos[2]; float dim[2];
        nbi_close_handle_data(view, pos, dim, NULL);

        int posi[2];
        posi[0] = (int)pos[0]; posi[1] = (int)pos[1];

        int dimi[2];
        dimi[0] = (int)dim[0]; dimi[1] = (int)dim[1];

        return nbi_point_inside_rect(ctx->state.ptr_pos, posi, dimi);
}


int
nbi_is_minimizing(
        struct nb_ctx *ctx,
        uint64_t hash_key,
        struct nb_view *view)
{
        if (ctx->state.ptr_state != NBI_PTR_UP_EVENT) {
                return 0;
        }

        if (ctx->state.ptr_ele) {
                return 0;
        }

        if (hash_key != ctx->state.ptr_view) {
                return 0;
        }

        float pos[2]; float dim[2];
        nbi_min_handle_data(view, pos, dim, NULL);

        int posi[2]; int dimi[2];
        posi[0] = (int)pos[0]; posi[1] = (int)pos[1];
        dimi[0] = (int)dim[0]; dimi[1] = (int)dim[1];

        return nbi_point_inside_rect(ctx->state.ptr_pos, posi, dimi);
}


int
nbi_is_clicked(
        struct nb_ctx *ctx,
        uint64_t hash_key,
        struct nb_view *view)
{
        (void)view;

        int hovered = hash_key == ctx->state.ptr_view ? 1 : 0;

        int result = hovered && ctx->state.ptr_state == NBI_PTR_DOWN;

        return result;
}


nb_result
nb_view_create(
        nb_ctx_t ctx,
        struct nb_view_desc *desc,
        nb_view_t *out_view,
        int *out_view_interaction_flags)
{
        if (!ctx || !out_view || !desc) {
                NB_ASSERT(0 && "NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        if (desc->type_id != NB_STRUCT_VIEW || !desc->name) {
                NB_ASSERT(0 && "NB_INVALID_DESC");
                return NB_INVALID_DESC;
        }

        /* window properties */
        uint64_t hash_key = nbi_hash_str(desc->name);
        uint64_t hash_desc = nbi_u64_pack(
                (uint16_t)desc->width,
                (uint16_t)desc->height,
                (uint16_t)desc->pos_x,
                (uint16_t)desc->pos_y);

        int can_resize = desc->flags & NB_VIEW_RESIZE;
        int can_drag = desc->flags & NB_VIEW_DRAG;
        int can_close = desc->flags & NB_VIEW_CLOSE;
        int can_mini = desc->flags & NB_VIEW_MINIMIZE;
        int has_title = desc->flags & NB_VIEW_TITLE;
        int interactions = 0;

        /* find this style */
        struct nb_style *vstyle = 0;
        vstyle = desc->style ? desc->style : &ctx->styles.view;

        /* find cache */
        struct nb_view *cached_view = nbi_find_cached_view(ctx, hash_key);

        /* create a new view or use cached */
        struct nb_view *nview = cached_view;

        if(!nview) {
                unsigned int bytes = sizeof(*nview);
                nview = nbi_buffer_push(&ctx->view_data.cached_v, bytes);
                NB_ASSERT(nview);

                nview->hash = hash_key;
                nview->user_desc_hash = hash_desc;
                nview->pos[0] = desc->pos_x;
                nview->pos[1] = desc->pos_y;
                nview->size[0] = desc->width;
                nview->size[1] = desc->height;
                nview->mini = 0;

                nview->cmds = NB_ALLOC(sizeof(struct nbi_cmd_buf));
        }

        struct nbi_cmd_buf *cmds = nview->cmds;

        NB_ASSERT(cmds);
        cmds->ctx = ctx;
        cmds->cmd_count = 0;
        nview->exists = ctx->tick;

        /* did the user update client position * size */
        /* issue with this, they might not mean to have both updated*/
        if (nview->user_desc_hash != hash_desc) {
                nview->user_desc_hash = hash_desc;
                nview->pos[0] = desc->pos_x;
                nview->pos[1] = desc->pos_y;
                nview->size[0] = desc->width;
                nview->size[1] = desc->height;
        }

        nview->ctx = ctx;

        /* move to front */
        if (nbi_is_clicked(ctx, hash_key, nview)) {
                nview = nbi_move_to_front(ctx, hash_key);
        }

        /* interactions */
        if (nbi_is_resizing(ctx, hash_key, nview) && can_resize) {
                nview->size[0] += ctx->state.ptr_delta[0];
                nview->size[1] += ctx->state.ptr_delta[1];

                nview->size[0] = nview->size[0] < 40 ? 40 : nview->size[0];
                nview->size[1] = nview->size[1] < 40 ? 40 : nview->size[1];

                interactions |= NB_VIEW_INTERACTION_RESIZED;
        }
        else if(nbi_is_dragging(ctx, hash_key) && can_drag) {
                nview->pos[0] += ctx->state.ptr_delta[0];
                nview->pos[1] += ctx->state.ptr_delta[1];

                interactions |= NB_VIEW_INTERACTION_DRAGGED;
        }
        else if (nbi_is_closing(ctx, hash_key, nview) && can_close) {
                interactions |= NB_VIEW_INTERACTION_CLOSED;
        }
        else if (nbi_is_minimizing(ctx, hash_key, nview) && can_mini) {
                nview->mini = nview->mini ? 0 : 1;

                interactions |= NB_VIEW_INTERACTION_MINIMIZE;
        }

        /* after resize */
        nview->inner_size[0] = nview->size[0] - (vstyle->padding * 2);

        /* create rdr cmds */
        nbi_push_view_border(vstyle, nview);

        int is_hover = nbi_is_hovered(ctx, hash_key);
        nbi_push_view_background(vstyle, nview, is_hover);

        if (has_title) {
                nbi_push_view_title(vstyle, nview, desc->name);
        }

        if (can_close) {
                float rect[4]; float color[4];
                nbi_close_handle_data(nview, rect, rect + 2, color);
                nbi_box(cmds, rect, color, (float)vstyle->radius);

                rect[0] += 1; rect[1] += 14;
                uint32_t flags = NB_TEXT_ALIGN_CENTER;
                char * icon = NB_ICON_TIMES;
                nbi_text(cmds, rect, flags, NB_COLOR_WHITE, icon);
        }

        if (can_resize && !nview->mini) {
                float rect[4]; float color[4];
                nbi_resize_handle_data(nview, rect, rect + 2, color);
                nbi_box(cmds, rect, color, (float)vstyle->radius);

                rect[0] += 1; rect[1] += 14;
                uint32_t flags = NB_TEXT_ALIGN_CENTER;
                char * icon = NB_ICON_EXPAND_ARROWS_ALT;
                nbi_text(cmds, rect, flags, NB_COLOR_WHITE, icon);
        }

        if(can_mini) {
                float rect[4]; float color[4];
                nbi_min_handle_data(nview, rect, rect + 2, color);
                nbi_box(cmds, rect, color, (float)vstyle->radius);

                rect[0] += 1; rect[1] += 14;
                char * icon = nview->mini ? NB_ICON_PLUS : NB_ICON_MINUS;
                uint32_t flags = NB_TEXT_ALIGN_CENTER;
                nbi_text(cmds, rect, flags, NB_COLOR_WHITE, icon);
        }

        nview->cursor[0] = nview->pos[0] + vstyle->padding;
        nview->cursor[1] = nview->pos[1] + 30 + vstyle->padding;

        *out_view = nview;

        if (out_view_interaction_flags) {
                *out_view_interaction_flags = interactions;
        }

        return NB_OK;
}


nb_result
nb_view_submit(
        nb_view_t view)
{
        NB_ASSERT(view);

        if (!view) {
                return NB_INVALID_PARAMS;
        }

        return NB_OK;
}


/* ------------------------------------------------------------ UI Element -- */

void
nbi_push_element_text(
        struct nb_style * style,
        struct nb_view * view,
        struct nb_element *ele,
        struct nb_element_text * data,
        int inter,
        uint32_t focus)
{
        float clip[4];
        nbi_get_element_clip(view, clip);
        nbi_scissor_set(view->cmds, clip);

        float width_max = (float)view->size[0] - 10.0f;

        const char * text = 0;
        if(!data->user_input || data->len_max) {
                text = data->text;
        }

        uint32_t tflags = data->align_type & _NB_TEXT_ALIGN_BIT_MASK;
        tflags |= NBI_TEXT_FLAGS_WRAP;
        if(focus) {
                tflags |= NBI_TEXT_FLAGS_CURSOR;
        }

        float tpad = 5.0f;
        float twidth = width_max - tpad * 2.0f;
        float tsize[2];
        nbi_get_text_size(view->ctx, twidth, tflags, text, tsize);

        float rect[4] = {
                (float)view->cursor[0], (float)view->cursor[1],
                width_max, tsize[1] + tpad,
        };

        uint32_t color32 = style->bg_color;
        if(inter & NB_INTERACTION_HOVER) {
                color32 = style->bg_color_hover;
        }

        float color[4];
        nbi_color_u32_to_flt(color32, color);

        if(focus) {
                /* border */
                float size = 2.0f;
                float brect[4] = {
                        rect[0] - size, rect[1] - size,
                        rect[2] + size * 2.0f, rect[3] + size * 2.0f,
                };
                float bcolor[4];
                nbi_color_u32_to_flt(style->border_color, bcolor);
                float radius = (float)style->radius;
                float bradius = radius != 0.f ? radius + size : 0.f;
                nbi_box(view->cmds, brect, bcolor, bradius);
        }

        nbi_box(view->cmds, rect, color, (float)style->radius);

        if(text) {
                float trect[4] = {
                        rect[0] + tpad, rect[1] + 14.0f,
                        twidth, rect[3],
                };

                float tclip[4];
                nbi_rect_intersect(clip, rect, tclip);

                nbi_scissor_set(view->cmds, tclip);
                nbi_text(view->cmds, trect, tflags, NB_COLOR_WHITE, text);
                nbi_scissor_clear(view->cmds);
        }

        view->cursor[1] += (int)rect[3] + 5;

        /* set the ele for collisions */
        ele->pos[0] = (int)rect[0]; ele->pos[1] = (int)rect[1];
        ele->size[0] = (int)rect[2]; ele->size[1] = (int)rect[3];

        nbi_scissor_clear(view->cmds);
}


void
nbi_push_element(
        struct nb_style *style,
        int inter,
        struct nb_view *view,
        struct nb_element *ele,
        char *stream_buf,
        int stream_buf_size,
        float *out_pos_v2)
{
        /* other */
        struct nb_ctx *ctx = view->ctx;
        struct nb_element_text data_ = {
                .text = stream_buf,
                .len_max = stream_buf_size,
                .user_input = 1,
                .align_type = style->align,
        };

        struct nb_element_text * data = &data_;

        uint32_t focus = 0;
        if(stream_buf_size) {
                focus = (ctx->state.focus_ele == ele->hash && data->user_input) ? 1 : 0;
                if (focus && data->text && data->len_max) {
                        uint32_t cursor = ctx->state.text_cursor;

                        if (ctx->state.focus_time == 0) {
                                cursor = 0;

                                /* This assumes data->text is null-terminated by default!! */
                                const char * it = data->text;
                                while (*it) {
                                        it++;
                                        cursor++;
                                }

                                ctx->state.text_cursor_time = 0.0f;
                        }

                        char * src = ctx->state.text_input;
                        while (*src) {
                                char c = *src++;
                                /* This isn't going to scale to handling other key events!! */
                                if (c == '\b') {
                                        if (cursor) {
                                                cursor--;
                                        }
                                }
                                else {
                                        if (cursor < data->len_max) {
                                                data->text[cursor++] = c;
                                        }
                                }

                                ctx->state.text_cursor_time = 0.0f;
                        }
                        data->text[cursor] = 0;

                        ctx->state.text_cursor = cursor;
                }
        }

        float clip[4];
        nbi_get_element_clip(view, clip);

        if(!style->disable_clip) {
                nbi_scissor_set(view->cmds, clip);
        }

        float x_pos = (float)(view->cursor[0] + style->offset_x);
        float y_pos = (float)(view->cursor[1] + style->offset_y);

        float rect[4] = {
                x_pos, y_pos, /* position */
                view->inner_size[0] - (style->padding * 2.f), 25.0f, /* size */
        };

        if(out_pos_v2) {
                out_pos_v2[0] = x_pos + (rect[2] / 2);
                out_pos_v2[1] = y_pos + (rect[3] / 2);
        }

        uint32_t tflags = style->align & _NB_TEXT_ALIGN_BIT_MASK;
        tflags |= NBI_TEXT_FLAGS_TERM;
        if(focus) {
                tflags |= NBI_TEXT_FLAGS_CURSOR;
        }

        float tpad = 5.0f;
        float twidth = rect[2] - tpad * 2.0f;

        if(stream_buf) {
                float tsize[2];
                nbi_get_text_size(view->ctx, twidth, tflags, stream_buf, tsize);

                float height = tsize[1] + tpad;
                if(rect[3] < height) {
                        rect[3] = height;
                }
        }

        /* color */
        uint32_t color32 = style->bg_color;
        if(inter & NB_INTERACTION_HOVER) {
                color32 = style->bg_color_hover;
        }
        float color[4];
        nbi_color_u32_to_flt(color32, color);

        view->cursor[1] += (int)rect[3] + 5;

        nbi_box(view->cmds, rect, color, (float)style->radius);

        if(stream_buf) {
                float trect[4] = {
                        rect[0] + tpad, rect[1] + 16.0f,
                        twidth, rect[3],
                };

                float tclip[4];
                nbi_rect_intersect(clip, rect, tclip);

                nbi_scissor_set(view->cmds, tclip);
                nbi_text(view->cmds, trect, tflags, NB_COLOR_WHITE, stream_buf);
                nbi_scissor_clear(view->cmds);
        }

        /* set the ele for collisions */
        ele->pos[0] = (int)rect[0]; ele->pos[1] = (int)rect[1];
        ele->size[0] = (int)rect[2]; ele->size[1] = (int)rect[3];

        nbi_scissor_clear(view->cmds);
}


int
nbi_has_interaction(
        struct nb_ctx *ctx,
        uint64_t view_hash,
        uint64_t ui_hash)
{
        if (ctx->state.ptr_view != view_hash) { return 0; }
        if (ctx->state.ptr_ele == 0) { return 0; }

        return ctx->state.ptr_ele == ui_hash ? 1 : 0;
}


nb_result
nb_element_create(
        nb_view_t vi,
        struct nb_element_desc *desc,
        struct nb_interaction *out_interactions,
        float *out_pos_v2)
{
        if(!vi || !desc) {
                NB_ASSERT(0 && "NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        if((desc->type_id != NB_STRUCT_ELEMENT) || (!desc->name)) {
                NB_ASSERT(0 && "NB_INVALID_DESC");
                return NB_INVALID_DESC;
        }

        if (!vi->ctx) {
                NB_ASSERT(0 && "NB_FAIL");
                return NB_FAIL;
        }

        struct nb_ctx *ctx = vi->ctx;
        uint64_t vi_hash = vi->hash;
        uint64_t ui_hash = nbi_hash_str(desc->name);
        int has_inter = nbi_has_interaction(ctx, vi_hash, ui_hash);
        int inter = 0;
        void *dropped_data = 0;

        /* if minimized bail */
        if (vi->mini) {
                return NB_OK;
        }

        /* interactions */
        if (desc->flags & NB_ELEMENT_INTERACTABLE) {

                if(has_inter && ctx->state.ptr_state == NBI_PTR_UP_EVENT) {
                        inter |= NB_INTERACTION_CLICKED;
                }
                if (has_inter && ctx->state.ptr_state == NBI_PTR_UP) {
                        inter |= NB_INTERACTION_HOVER;
                }
                if (has_inter && ctx->state.ptr_state == NBI_PTR_DOWN) {
                        inter |= NB_INTERACTION_DOWN;

                        int dx = ctx->state.ptr_delta[0];

                        if (dx > 0) {
                                inter |= NB_INTERACTION_DRAGGED_RIGHT;
                        }
                        else if (dx < 0) {
                                inter |= NB_INTERACTION_DRAGGED_LEFT;
                        }
                }
                if(has_inter && ctx->state.text_input[0]) {
                        inter |= NB_INTERACTION_TEXT_STREAM;
                }
        }

        if (desc->flags & NB_ELEMENT_DRAGABLE &&
            ctx->state.ptr_state == NBI_PTR_DOWN &&
            has_inter) {
                desc->style->offset_x += ctx->state.ptr_distance[0];
                desc->style->offset_y += ctx->state.ptr_distance[1];

                ctx->state.ptr_ele_drag = desc->data;
        }

        if(desc->flags & NB_ELEMENT_DROPABLE && has_inter) {
                dropped_data = ctx->state.ptr_ele_drag;
                ctx->state.ptr_ele_drag = 0;
        }

        struct nb_element *ele = 0;
        ele = nbi_buffer_push(vi->ctx->view_data.cached_e_next, sizeof(*ele));
        ele->view_hash = vi_hash;
        ele->hash = ui_hash;

        struct nb_style *dstyle = desc->style;
        struct nb_style *cstyle = &ctx->styles.button;
        struct nb_style *st = dstyle ? dstyle : cstyle;

        nbi_push_element(st, inter, vi, ele, desc->content, desc->content_size, out_pos_v2);

        if (out_interactions) {
                out_interactions->flags = inter;
                out_interactions->dropped = dropped_data;
        }

        return NB_OK;
}


/* ---------------------------------------------------------------- Styles -- */


nb_result
nb_style_default_get(
        nb_ctx_t ctx,
        nb_identifier selector,
        struct nb_style *out_style)
{
        if (!ctx || !out_style) {
                NB_ASSERT(0 && "NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        if (selector == NB_STYLE_WINDOW) {
                *out_style = ctx->styles.view;
                return NB_OK;
        }
        else if (selector == NB_STYLE_BUTTON) {
                *out_style = ctx->styles.button;
                return NB_OK;
        }

        return NB_INVALID_PARAMS;
}


nb_result
nb_style_default_set(
        nb_ctx_t ctx,
        nb_identifier selector,
        const struct nb_style *style)
{
        if (!ctx || !style) {
                NB_ASSERT(0 && "NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        if (selector == NB_STYLE_WINDOW) {
                ctx->styles.view = *style;
                return NB_OK;
        }
        else if (selector == NB_STYLE_BUTTON) {
                ctx->styles.button = *style;
                return NB_OK;
        }

        return NB_INVALID_PARAMS;
}

/* ------------------------------------------------------------- Rendering -- */

nb_result
nb_get_render_data(nb_ctx_t ctx, struct nb_render_data * data) {
        if(!ctx || !data) {
                NB_ASSERT(0 && "NB_INVALID_PARAMS");
                return NB_INVALID_PARAMS;
        }

        data->vtx = ctx->vtx_buf.v;
        data->vtx_count = ctx->vtx_buf.v_count;
        data->idx = ctx->vtx_buf.i;
        data->idx_count = ctx->vtx_buf.i_count;
        data->cmd_list_count = 0;

        struct nb_buffer *buf = &ctx->view_data.cached_v;
        struct nb_view *view = nbi_buffer_mem(buf);

        unsigned int stride = sizeof(*view);
        unsigned int count = nbi_buffer_used(buf) / stride;
        unsigned int i;

        for(i = 0; i < count; ++i) {
                struct nbi_cmd_buf *cmd_buf = view[count - i - 1].cmds;

                if (data->cmd_list_count >= NB_ARRAY_COUNT(data->cmd_lists)) {
                        NB_ASSERT(!"Cmd list array full!");
                        break;
                }

                struct nb_render_cmd_list *list;
                list = data->cmd_lists + data->cmd_list_count;

                data->cmd_list_count++;

                list->cmds = cmd_buf->cmds;
                list->count = cmd_buf->cmd_count;
        }

        return NB_OK;
}

unsigned int
nb_get_font_count(nb_ctx_t ctx) {
        NB_ASSERT(ctx);
        return ctx->font_count;
}

nb_result
nb_get_font_tex_list(nb_ctx_t ctx, struct nb_font_tex * tex_list) {
        NB_ASSERT(ctx);
        NB_ASSERT(tex_list);

        uint32_t i;
        for(i = 0; i < ctx->font_count; i++) {
                tex_list[i] = ctx->fonts[i].tex;
        }

        return NB_OK;
}

/* DEBUG!! */
nb_result
nb_debug_set_font(nb_ctx_t ctx, unsigned int idx) {
        NB_ASSERT(ctx);
        NB_ASSERT(idx < ctx->font_count);

        ctx->debug_font_next = ctx->fonts + idx;

        return NB_OK;
}

/* DEBUG!! */
unsigned int
nb_debug_get_font(nb_ctx_t ctx) {
        NB_ASSERT(ctx);

        struct nbi_font * font = ctx->debug_font_next;
        if(!font) {
                font = ctx->font;
        }
        NB_ASSERT(font);

        unsigned int result = (unsigned int)(font - ctx->fonts);
        NB_ASSERT(result < ctx->font_count);
        return result;
}

/* -------------------------------------------------- Internal Render Cmds -- */

static void
nbi_push_vtx(struct nbi_vtx_buf * buf, float x, float y, float * color) {
        buf->v[buf->v_count++] = x;
        buf->v[buf->v_count++] = y;

        buf->v[buf->v_count++] = 0.0f;
        buf->v[buf->v_count++] = 0.0f;

        buf->v[buf->v_count++] = color[0];
        buf->v[buf->v_count++] = color[1];
        buf->v[buf->v_count++] = color[2];
        buf->v[buf->v_count++] = color[3];
}

static void
nbi_push_vtx_uv(
        struct nbi_vtx_buf * buf,
        float x,
        float y,
        float u,
        float v,
        float * color)
{
        buf->v[buf->v_count++] = x;
        buf->v[buf->v_count++] = y;

        buf->v[buf->v_count++] = u;
        buf->v[buf->v_count++] = v;

        buf->v[buf->v_count++] = color[0];
        buf->v[buf->v_count++] = color[1];
        buf->v[buf->v_count++] = color[2];
        buf->v[buf->v_count++] = color[3];
}

static void
nbi_push_quad_idxs(
        struct nbi_vtx_buf * buf,
        uint16_t top_lft,
        uint16_t bot_lft,
        uint16_t bot_rgt,
        uint16_t top_rgt)
{
        buf->i[buf->i_count++] = top_lft;
        buf->i[buf->i_count++] = bot_lft;
        buf->i[buf->i_count++] = bot_rgt;

        buf->i[buf->i_count++] = top_lft;
        buf->i[buf->i_count++] = bot_rgt;
        buf->i[buf->i_count++] = top_rgt;
}

static uint16_t
nbi_push_quad(
        struct nbi_vtx_buf * buf,
        uint16_t vtx,
        float * rect,
        float * color)
{
        nbi_push_quad_idxs(buf, vtx, vtx + 1, vtx + 2, vtx + 3);
        nbi_push_vtx(buf, rect[0], rect[1], color);
        nbi_push_vtx(buf, rect[0], rect[1] + rect[3], color);
        nbi_push_vtx(buf, rect[0] + rect[2], rect[1] + rect[3], color);
        nbi_push_vtx(buf, rect[0] + rect[2], rect[1], color);
        return 4;
}

static uint16_t
nbi_push_round_corner(
        struct nbi_vtx_buf * buf,
        float * pos,
        float * color,
        uint16_t seg_count,
        float radius, float angle)
{
        NB_ASSERT(seg_count);

        uint16_t i;

        uint16_t vtx = (uint16_t)(buf->v_count / 8);

        for(i = 0; i < seg_count; i++) {
                buf->i[buf->i_count++] = vtx;
                buf->i[buf->i_count++] = vtx + (i + 1);
                buf->i[buf->i_count++] = vtx + (i + 2);
        }

        nbi_push_vtx(buf, pos[0], pos[1], color);

        float d_angle = (NB_TAU * 0.25f) / (float)seg_count;

        for(i = 0; i < (seg_count + 1); i++) {
                float t = angle + d_angle * (float)i;
                float x = pos[0] + cosf(t) * radius;
                float y = pos[1] - sinf(t) * radius;
                nbi_push_vtx(buf, x, y, color);
        }

        return seg_count + 2;
}

static struct nb_render_cmd *
nbi_cmd_begin(
        struct nbi_cmd_buf * buf,
        struct nbi_vtx_buf * data,
        uint32_t type,
        uint16_t * vtx)
{
        struct nb_render_cmd * cmd = buf->cmds + buf->cmd_count++;
        cmd->type = type;
        cmd->data.elem.offset = data->i_count;

        if(vtx) {
                *vtx = (uint16_t)(data->v_count / 8);
        }

        return cmd;
}

static void
nbi_cmd_end(struct nbi_vtx_buf * data, struct nb_render_cmd * cmd) {
        if(cmd) {
                cmd->data.elem.count = data->i_count - cmd->data.elem.offset;
        }
}

void
nbi_box(struct nbi_cmd_buf * buf, float * rect, float * color, float radius) {
        struct nbi_vtx_buf * data = &buf->ctx->vtx_buf;

        int prim = NB_RENDER_CMD_TYPE_TRIANGLES;
        uint16_t vtx;

        struct nb_render_cmd * cmd = nbi_cmd_begin(buf, data, prim, &vtx);

        /* sharp corners - shortcut */
        if(radius <= 0.0f) {
                nbi_push_quad(data, vtx, rect, color);
                nbi_cmd_end(data, cmd);
                return;
        }

        float extent_min = (rect[2] < rect[3] ? rect[2] : rect[3]) * 0.5f;
        if(radius > extent_min) {
                radius = extent_min;
        }

        uint16_t seg_count = 7;

        uint16_t cv0 = vtx;
        uint16_t cv1 = vtx + (seg_count + 2);
        uint16_t cv2 = vtx + (seg_count + 2) * 2;
        uint16_t cv3 = vtx + (seg_count + 2) * 3;

        nbi_push_quad_idxs(data, cv1 + 1, cv1, cv0, cv0 + (seg_count + 1));
        nbi_push_quad_idxs(data, cv2 + 1, cv2, cv1, cv1 + (seg_count + 1));
        nbi_push_quad_idxs(data, cv3 + 1, cv3, cv2, cv2 + (seg_count + 1));
        nbi_push_quad_idxs(data, cv0 + 1, cv0, cv3, cv3 + (seg_count + 1));
        nbi_push_quad_idxs(data, cv1, cv2, cv3, cv0);

        float c0[2] = { rect[0] + rect[2] - radius, rect[1] + radius, };
        float ang = NB_TAU * 0.f;
        nbi_push_round_corner(data, c0, color, seg_count, radius, ang);

        float c1[2] = { rect[0] + radius, rect[1] + radius, };
        ang = NB_TAU * 0.25f;
        nbi_push_round_corner(data, c1, color, seg_count, radius, ang);

        float c2[2] = { rect[0] + radius, rect[1] + rect[3] - radius, };
        ang = NB_TAU * 0.50f;
        nbi_push_round_corner(data, c2, color, seg_count, radius, ang);

        float c3[2] = { rect[0] + rect[2] - radius, rect[1] + rect[3] - radius, };
        ang = NB_TAU * 0.75f;
        nbi_push_round_corner(data, c3, color, seg_count, radius, ang);

        nbi_cmd_end(data, cmd);
}

void
nbi_line(struct nbi_cmd_buf * buf, float * p, float * q, float * color) {
        struct nbi_vtx_buf * data = &buf->ctx->vtx_buf;

        uint16_t vtx;
        struct nb_render_cmd * cmd = nbi_cmd_begin(buf, data, NB_RENDER_CMD_TYPE_LINES, &vtx);

        data->i[data->i_count++] = vtx;
        data->i[data->i_count++] = vtx + 1;

        nbi_push_vtx(data, p[0], p[1], color);
        nbi_push_vtx(data, q[0], q[1], color);

        nbi_cmd_end(data, cmd);
}

void
nbi_bez(
        struct nbi_cmd_buf * buf,
        float * p0,
        float * p1,
        float * p2,
        float * p3,
        float * color)
{
        struct nbi_vtx_buf * data = &buf->ctx->vtx_buf;

        uint16_t vtx;
        struct nb_render_cmd * cmd = nbi_cmd_begin(buf, data, NB_RENDER_CMD_TYPE_LINES, &vtx);

        uint16_t seg_count = 16;
        uint16_t si, i;

        for(si = 0; si < seg_count; si++) {
                data->i[data->i_count++] = vtx + si;
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

static uint32_t
nbi_decode_utf8_cp(char * utf8, uint32_t * cp) {
        *cp = 0;

        uint8_t * it = (uint8_t *)utf8;
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

struct nbi_text_out {
        struct nbi_font * font;

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
nbi_line_adv(struct nbi_vtx_buf * buf, struct nbi_text_out * out) {
        if(out->align_type != NB_TEXT_ALIGN_LEFT) {
                float offset = out->end_x - out->x;
                if(offset < 0.0f) {
                        offset = 0.0f;
                }
                if(out->align_type == NB_TEXT_ALIGN_CENTER) {
                        offset *= 0.5f;
                }
                offset = (float)((int32_t)offset);

                if(buf) {
                        uint32_t i;
                        for(i = out->vtx_start; i < buf->v_count; i += 8) {
                                float * v = buf->v + i;
                                *v += offset;
                        }

                        out->vtx_start = buf->v_count;
                }
        }

        if(out->x > out->max_x) {
                out->max_x = out->x;
        }

        out->x = out->start_x;
        /* Use actual font metrics to do vertical advance!! */
        out->y += out->font->height;
        out->space = 0.0f;
}

void
nbi_text_(struct nbi_cmd_buf * buf, struct nbi_font * font, float * rect, uint32_t flags, float * color, const char * text, float * out_size) {
        if(!text) {
                if(out_size) {
                        out_size[0] = 0.0f;
                        out_size[1] = font->height;
                }
                return;
        }

        uint32_t wrap = flags & NBI_TEXT_FLAGS_WRAP;
        uint32_t term_tag = flags & NBI_TEXT_FLAGS_TERM;

        struct nbi_text_out out = {
                .font = font,

                .start_x = rect[0],
                .end_x = rect[0] + rect[2],
                .max_x = rect[0],
                .x = rect[0],
                .y = rect[1],

                .align_type = flags & _NB_TEXT_ALIGN_BIT_MASK,
        };

        struct nbi_vtx_buf * data = 0;
        uint16_t vtx = 0;
        struct nb_render_cmd * cmd = 0;
        if(buf) {
                data = &buf->ctx->vtx_buf;
                cmd = nbi_cmd_begin(buf, data, NB_RENDER_CMD_TYPE_TRIANGLES, &vtx);

                out.vtx_start = data->v_count;
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
                        uint32_t cursor_on = ((uint32_t)(buf->ctx->state.text_cursor_time * 2.0f) & 1) == 0;
                        if(cursor_on) {
                                /* Font size is baked into these numbers */
                                float cursor_rect[4] = {
                                        out.x, out.y - 12.0f,
                                        cursor_width, 15.0f,
                                };
                                vtx += nbi_push_quad(data, vtx, cursor_rect, color);
                        }
                }

                out.x += cursor_width;
        }

        nbi_line_adv(data, &out);

        if(out_size) {
                out_size[0] = out.max_x - rect[0];
                out_size[1] = out.y - rect[1];
        }

        if(data) {
                nbi_cmd_end(data, cmd);
        }
}

void nbi_get_text_size(struct nb_ctx * ctx, float width, uint32_t flags, const char * text, float * out_size) {
        if(out_size) {
                struct nbi_font * font = ctx->font;
                float rect[4] = { 0.0f, 0.0f, width, 0.0f, };
                nbi_text_(0, font, rect, flags, 0, text, out_size);
        }
}

void
nbi_text(struct nbi_cmd_buf * buf, float * rect, uint32_t flags, float * color, const char * text) {
        struct nbi_font * font = buf->ctx->font;
        nbi_text_(buf, font, rect, flags, color, text, 0);
}

int16_t
nbi_clamp_f32_to_i16(float x) {
        return x < 32767.0f ? (x > -32768.0f ? (int16_t)x : -32768) : 32767;
}

void nbi_scissor_set(struct nbi_cmd_buf * buf, float * rect) {
        buf->cmds[buf->cmd_count++] = (struct nb_render_cmd) {
                .type = NB_RENDER_CMD_TYPE_SCISSOR,
                .data.clip_rect[0] = nbi_clamp_f32_to_i16(rect[0]),
                .data.clip_rect[1] = nbi_clamp_f32_to_i16(rect[1]),
                .data.clip_rect[2] = nbi_clamp_f32_to_i16(rect[2]),
                .data.clip_rect[3] = nbi_clamp_f32_to_i16(rect[3]),
        };
}

void nbi_scissor_clear(struct nbi_cmd_buf * buf) {
        buf->cmds[buf->cmd_count++] = (struct nb_render_cmd) {
                .type = NB_RENDER_CMD_TYPE_SCISSOR,
                .data.clip_rect[0] = 0,
                .data.clip_rect[1] = 0,
                .data.clip_rect[2] = 0x7FFF,
                .data.clip_rect[3] = 0x7FFF,
        };
}

/* impl guard and flag */
#endif
#endif

