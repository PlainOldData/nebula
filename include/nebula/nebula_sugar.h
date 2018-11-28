#ifndef NEBULA_SUGAR_INCLUDED
#define NEBULA_SUGAR_INCLUDED


#include "nebula.h"


/* views */

nb_view_t nbs_view_window_begin(nb_ctx_t ctx, const char *name, int *size_vec2, int *pos_vec2, int flags, int *closed);
int nbs_view_end(nb_view_t view);

void nbs_bez(nb_view_t view, float *start_v2, float *end_v2);

/* elements */

int nbs_button(nb_view_t vi, const char *name);
void nbs_text(nb_view_t vi, const char *name, const char *txt, unsigned int align);
int nbs_text_field(nb_view_t vi, const char *name, char *txt_buffer, int txt_buffer_size);

int nbs_text_input(nb_view_t vi, const char *name, char * txt, unsigned int size, unsigned int align);

int nbs_drag_int(nb_view_t vi, const char *name, int *value);
int nbs_drag_float(nb_view_t vi, const char *name, float *value);

int nbs_node_input(nb_view_t vi, const char *name, void **other, float *pos_v2);
int nbs_node_output(nb_view_t vi, const char *name, void *other, float *pos_v2);

/* inc guard */
#endif


/* IMPL */


#ifdef NEBULA_IMPL
#ifndef NEBULA_SUGAR_IMPL_INCLUDED
#define NEBULA_SUGAR_IMPL_INCLUDED


#include <string.h>
#include <assert.h>


/* ---------------------------------------------------------------- Styles -- */


void
nbsi_style_button(struct nb_style *style) {
        memset(style, 0, sizeof(*style));
        style->bg_color = 0xD36745FF;
        style->bg_color_hover = 0xFAD595FF;
        style->text_color = 0xCFE4B6FF;
        style->padding = 0;
        style->margin = 0;
        style->radius = 4;
        style->height = 35;
        style->align = NB_TEXT_ALIGN_CENTER;
}


void
nbsi_style_text(struct nb_style *style) {
        memset(style, 0, sizeof(*style));
        style->bg_color = 0x00000000;
        style->bg_color_hover = 0x00000000;
        style->border_color = 0x00000000;
        style->text_color = 0xCFE4B6FF;
        style->padding = 0;
        style->margin = 0;
        style->radius = 4;
        style->height = -1;
        style->align = NB_TEXT_ALIGN_LEFT;
}


void
nbsi_style_text_field(struct nb_style *style) {
        memset(style, 0, sizeof(*style));
        style->bg_color = 0xB3B3B3FF;
        style->bg_color_hover = 0xD1D1D1FF;
        style->border_color = 0xD36745FF;
        style->text_color = 0xCFE4B6FF;
        style->padding = 0;
        style->margin = 0;
        style->radius = 5;
        style->height = -1;
        style->align = NB_TEXT_ALIGN_LEFT;
}


void
nbsi_style_view(struct nb_style *style) {
        memset(style, 0, sizeof(*style));
        style->bg_color = 0x575459FF;
        style->bg_color_hover = 0x6C726BFF;
        style->border_color = 0xD36745FF;
        style->text_color = 0xCFE4B6FF;
        style->border_size = 1;
        style->padding = 5;
        style->margin = 0;
        style->radius = 1;
        style->height = 35;
        style->align = NB_TEXT_ALIGN_LEFT;
}


/* ----------------------------------------------------------------- Sugar -- */


nb_view_t
nbs_view_window_begin(
        nb_ctx_t ctx,
        const char *name,
        int *size_vec2,
        int *pos_vec2,
        int flags,
        int *open)
{
        int default_flags =
                NB_VIEW_RESIZE |
                NB_VIEW_CLOSE |
                NB_VIEW_DRAG |
                NB_VIEW_TITLE;

        struct nb_style style;
        nbsi_style_view(&style);

        struct nb_view_desc view_desc;
        memset(&view_desc, 0, sizeof(view_desc));
        view_desc.type_id = NB_STRUCT_VIEW;
        view_desc.ext = 0;
        view_desc.flags = flags >= 0 ? flags : default_flags;
        view_desc.name = name;
        view_desc.height = size_vec2 ? size_vec2[1] : 200;
        view_desc.width = size_vec2 ? size_vec2[0] : 100;
        view_desc.pos_x = pos_vec2 ? pos_vec2[0] : 10;
        view_desc.pos_y = pos_vec2 ? pos_vec2[1] : 10;
        view_desc.user_data = 0;
        view_desc.view_type = 0; /* unsued atm */
        view_desc.style = &style;

        nb_view_t view = 0;
        int interaction;
        nb_result res = nb_view_create(ctx, &view_desc, &view, &interaction);
        assert(res == NB_OK);

        if ((interaction & NB_VIEW_INTERACTION_CLOSED) && open) {
                *open = 0;
        }

        return view;
}


int
nbs_view_end(
        nb_view_t view)
{
        nb_view_submit(view);

        return 0;
}


void
nbs_bez(nb_view_t view, float *start_v2, float *end_v2)
{
        struct nb_view *vi = (struct nb_view*)view;

        float color[] = {1,0,0,1};
        float cp_1[2];
        cp_1[0] = start_v2[0] + 200;
        cp_1[1] = start_v2[1];

        float cp_2[2];
        cp_2[0] = end_v2[0] - 200;
        cp_2[1] = end_v2[1];

        nbi_bez(vi->cmds, start_v2, cp_1, cp_2, end_v2, color);
}


int
nbs_button(nb_view_t view, const char *name)
{
        struct nb_style style;
        nbsi_style_button(&style);

        struct nb_element_desc ele_desc;
        memset(&ele_desc, 0, sizeof(ele_desc));

        ele_desc.type_id = NB_STRUCT_ELEMENT;
        ele_desc.flags = NB_ELEMENT_INTERACTABLE;
        ele_desc.name = name;
        ele_desc.content = (void *)name;
        ele_desc.style = &style;

        struct nb_interaction interaction;
        nb_element_create(view, &ele_desc, &interaction, 0);

        return interaction.flags & NB_INTERACTION_CLICKED ? 1 : 0;
}


/* Experimenting with an exposed renderer */
int
nbs_button2(nb_view_t vi, const char *name) {
        assert(name);

        if(vi->mini) {
                return 0;
        }

        struct nb_ctx * ctx = vi->ctx;

        struct nb_element * ele = 0;
        ele = nbi_buffer_push(vi->ctx->view_data.cached_e_next, sizeof(*ele));
        ele->view_hash = vi->hash;
        ele->hash = nbi_hash_str(name);

        int has_inter = nbi_has_interaction(ctx, vi->hash, ele->hash);

        float clip[4];
        nbi_get_element_clip(vi, clip);

        struct nb_rect rect = {
                .x = (float)vi->cursor[0],
                .y = (float)vi->cursor[1],
                .w = (float)vi->inner_size[0],
                .h = 25.0f,
        };

        /* color */
        uint32_t color32 = 0xD36745FF;
        if(has_inter && ctx->state.ptr_state == NBI_PTR_UP) {
                color32 = 0xFAD595FF;
        }
        float color[4];
        nbi_color_u32_to_flt(color32, color);

        nbi_scissor_set(vi->cmds, clip);
        nbi_box(vi->cmds, (float *)&rect, color, 4.0f);

        float text_pad = 5.0f;
        struct nb_rect text_rect = {
                .x = rect.x + text_pad,
                .y = rect.y + 16.0f,
                .w = rect.w - text_pad * 2.0f,
                .h = rect.h,
        };

        float text_clip[4];
        nbi_rect_intersect(clip, (float *)&rect, text_clip);

        uint32_t text_flags = NB_TEXT_ALIGN_CENTER | NBI_TEXT_FLAGS_TERM;

        nbi_scissor_set(vi->cmds, text_clip);
        nbi_text(vi->cmds, (float *)&text_rect, text_flags, NB_COLOR_WHITE, name);
        nbi_scissor_clear(vi->cmds);

        nbi_scissor_clear(vi->cmds);

        vi->cursor[1] += (int)rect.h + 5;

        /* set the ele for collisions */
        ele->pos[0] = (int)rect.x; ele->pos[1] = (int)rect.y;
        ele->size[0] = (int)rect.w; ele->size[1] = (int)rect.h;

        int clicked = 0;
        if(has_inter && ctx->state.ptr_state == NBI_PTR_UP_EVENT) {
                clicked = 1;
        }
        return clicked;
}


int
nbs_drag_int(nb_view_t view, const char *name, int *value)
{
        struct nb_style style;
        nbsi_style_button(&style);

        struct nb_element_desc ele_desc;
        memset(&ele_desc, 0, sizeof(ele_desc));

        char str[128];
        sprintf(
                str,
                "%s %s: %d %s",
                NB_ICON_ANGLE_DOUBLE_LEFT,
                name,
                *value,
                NB_ICON_ANGLE_DOUBLE_RIGHT);

        ele_desc.type_id = NB_STRUCT_ELEMENT;
        ele_desc.flags = NB_ELEMENT_INTERACTABLE;
        ele_desc.name = name;
        ele_desc.content = str;
        ele_desc.style = &style;

        struct nb_interaction interaction;
        nb_element_create(view, &ele_desc, &interaction, 0);

        int step = 1;

        if (interaction.flags & NB_INTERACTION_DRAGGED_LEFT) {
                *value -= step;
                return -1;
        }
        else if (interaction.flags & NB_INTERACTION_DRAGGED_RIGHT) {
                *value += step;
                return +1;
        }

        return 0;
}


int
nbs_drag_float(nb_view_t view, const char *name, float *value)
{
        struct nb_style style;
        nbsi_style_button(&style);

        struct nb_element_desc ele_desc;
        memset(&ele_desc, 0, sizeof(ele_desc));

        char str[128];
        sprintf(
                str,
                "%s %s: %.3f %s",
                NB_ICON_ANGLE_DOUBLE_LEFT,
                name,
                *value,
                NB_ICON_ANGLE_DOUBLE_RIGHT);

        ele_desc.type_id = NB_STRUCT_ELEMENT;
        ele_desc.name = name;
        ele_desc.content = str;
        ele_desc.style = &style;

        struct nb_interaction interaction;
        nb_element_create(view, &ele_desc, &interaction, 0);

        float step = 0.01f;

        if (interaction.flags & NB_INTERACTION_DRAGGED_LEFT) {
                *value -= step;
                return -1;
        }
        else if (interaction.flags & NB_INTERACTION_DRAGGED_RIGHT) {
                *value += step;
                return +1;
        }

        return 0;
}


int
nbs_radio_buttons(nb_view_t view, const char **names, int *selection) {
        (void)selection;
        (void)names;
        (void)view;
        struct nb_element_desc ele_desc;
        memset(&ele_desc, 0, sizeof(ele_desc));

        return 0;
}


int
nbs_text_field(nb_view_t vi, const char *name,  char *txt_buf, int txt_size) {
        struct nb_style style;
        nbsi_style_text_field(&style);

        struct nb_element_desc ele_desc;
        memset(&ele_desc, 0, sizeof(ele_desc));

        ele_desc.type_id = NB_STRUCT_ELEMENT;
        ele_desc.name = name;
        ele_desc.content = txt_buf;
        ele_desc.content_type = 1;
        ele_desc.content_size = txt_size;

        struct nb_interaction interaction;
        nb_element_create(vi, &ele_desc, &interaction, 0);

        return interaction.flags & NB_INTERACTION_TEXT_STREAM ? 1 : 0;
}


void
nbs_text(nb_view_t vi, const char *name, const char * txt, unsigned int align) {
        struct nb_style style;
        nbsi_style_text(&style);
        style.align = align;

        struct nb_element_desc ele_desc;
        memset(&ele_desc, 0, sizeof(ele_desc));

        ele_desc.type_id = NB_STRUCT_ELEMENT;
        ele_desc.name = name;
        ele_desc.content = (void *)txt;
        ele_desc.style = &style;

        nb_element_create(vi, &ele_desc, 0, 0);
}


int
nbs_node_input(nb_view_t vi, const char *name, void **dropped, float *pos_v2) {
        struct nb_style style;
        nbsi_style_button(&style);

        style.disable_clip = 1;
        style.offset_x = -20;

        struct nb_element_desc ele_desc;
        memset(&ele_desc, 0, sizeof(ele_desc));

        ele_desc.flags = NB_ELEMENT_DROPABLE;
        ele_desc.type_id = NB_STRUCT_ELEMENT;
        ele_desc.name = name;
        ele_desc.style = &style;
        ele_desc.data = 0;

        struct nb_interaction interaction;
        nb_element_create(vi, &ele_desc, &interaction, pos_v2);

        if(interaction.dropped && dropped) {
                *dropped = interaction.dropped;
        }

        return interaction.dropped ? 1 : 0;
}


int
nbs_node_output(nb_view_t vi, const char *name, void *other, float *pos_v2) {
        struct nb_style style;
        nbsi_style_button(&style);

        style.disable_clip = 1;
        style.offset_x = 20;

        struct nb_element_desc ele_desc;
        memset(&ele_desc, 0, sizeof(ele_desc));

        ele_desc.type_id = NB_STRUCT_ELEMENT;
        ele_desc.flags = NB_ELEMENT_DRAGABLE;
        ele_desc.name = name;
        ele_desc.style = &style;
        ele_desc.data = other;

        nb_element_create(vi, &ele_desc, 0, pos_v2);

        return 1;
}


/* impl and imple guard */
#endif
#endif
