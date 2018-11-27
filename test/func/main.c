#define NEBULA_IMPL
#include <nebula/nebula.h>
#include <nebula/nebula_sugar.h>

#include <stdio.h>
#include <stdlib.h>

#define NEB_GLFW_IMPL
#include <nebula_glfw.h>

void
basic_demo(nb_ctx_t c, int *open)
{
        int size[] = { 200, 300 };
        const char *name = "Basic";

        nb_view_t vi = nbs_view_window_begin(c, name, size, NULL, -1, open);

        static int click_1 = 0;
        static int font_idx = 0;


        const char *but1_name = click_1 ? "*Clicked*" : "Click Me!";
        if(nbs_button2(vi, but1_name)) {
                printf("Clicked!\n");
                click_1 = click_1 ? 0 : 1;

                font_idx = (font_idx + 1) % nb_get_font_count(c);
                nb_debug_set_font(c, font_idx);
        }

        static int text_align = NB_TEXT_ALIGN_LEFT;

        const char * text_align_names[] = {
                [NB_TEXT_ALIGN_LEFT] = NB_ICON_ALIGN_LEFT " Left Aligned",
                [NB_TEXT_ALIGN_RIGHT] = NB_ICON_ALIGN_RIGHT " Right Aligned",
                [NB_TEXT_ALIGN_CENTER] = NB_ICON_ALIGN_CENTER " Centre Aligned",
        };

        if(nbs_button(vi, text_align_names[text_align])) {
                text_align += 1;
                text_align %= 3;
        }

        char * placeholder_text = ""
                "Lorem ipsum "
                "dolor sit amet, consectetur adipiscing elit. Sed quis "
                "aliquet ante, vel fringilla velit. Pellentesque pharetra "
                "mollis consectetur."
                "";
        nbs_text(vi, "Text", placeholder_text, text_align);

        static char user_text[128];
        nbs_text_input(vi, "Basic Text Input", user_text, sizeof(user_text), text_align);

        nbs_view_end(vi);
}


void
widget_demo(nb_ctx_t c, int *open)
{
        int size[] = { 200, 300 };
        const char *name = "Sugar";

        nb_view_t vi = nbs_view_window_begin(c, name, size, NULL, -1, open);
        static int click_1 = 0;

        const char *some_text = "A Text Widget!";
        nbs_text(vi, "Text", some_text, NB_TEXT_ALIGN_LEFT);

        const char *but1_name = click_1 ? "*Button*" : "Button";
        if (nbs_button(vi, but1_name)) {
                printf("Clicked!\n");
                click_1 = click_1 ? 0 : 1;
        }

        static int drag_int = 1;
        nbs_drag_int(vi, "Drag int", &drag_int);

        static float drag_flt = 10.2f;
        nbs_drag_float(vi, "Drag flt", &drag_flt);

        static char name_buf[32] = "user";
        nbs_text_field(vi, "username", &name_buf[0], sizeof(name_buf));

        static char pass_buf[32] = "pass";
        nbs_text_field(vi, "password", &pass_buf[0], sizeof(pass_buf));

        nbs_view_end(vi);
}


void
window_properties_demo(nb_ctx_t c, int *open)
{
        int size[] = { 200, 300 };
        int pos[] = { 30, 30 };
        const char * name = "View Properties";
        static int vi_flags =
                NB_VIEW_RESIZE |
                NB_VIEW_CLOSE |
                NB_VIEW_DRAG |
                NB_VIEW_TITLE;

        nb_view_t vi = nbs_view_window_begin(c, name, size, pos, vi_flags, open);

        const char *drag_name = vi_flags & NB_VIEW_DRAG ? "Drag" : "!Drag";
        if(nbs_button(vi, drag_name)) {
                vi_flags ^= NB_VIEW_DRAG;
        }

        const char *resize_name = vi_flags & NB_VIEW_RESIZE ? "Resize" : "!Resize";
        if(nbs_button(vi, resize_name)) {
                vi_flags ^= NB_VIEW_RESIZE;
        }

        const char *close_name = vi_flags & NB_VIEW_CLOSE ? "Close" : "!Close";
        if(nbs_button(vi, close_name)) {
                vi_flags ^= NB_VIEW_CLOSE;
        }

        const char *title_name = vi_flags & NB_VIEW_TITLE ? "Title" : "!Title";
        if (nbs_button(vi, title_name)) {
                vi_flags ^= NB_VIEW_TITLE;
        }

        const char *mini_title = vi_flags & NB_VIEW_MINIMIZE ? "Mini" : "!Mini";
        if (nbs_button(vi, mini_title)) {
                vi_flags ^= NB_VIEW_MINIMIZE;
        }

        nbs_view_end(vi);
}


void
style_properties_demo(nb_ctx_t c, int *open)
{
        static int window_radius = 1;
        static int window_border_size = 1;
        static int window_padding = 1;
        static int window_margin = 1;

        int size[2];
        size[0] = 200;
        size[1] = 400;

        int pos[2];
        pos[0] = 50;
        pos[1] = 50;

        int flags =
                NB_VIEW_RESIZE |
                NB_VIEW_CLOSE |
                NB_VIEW_DRAG |
                NB_VIEW_TITLE;

        struct nb_style style;
        nbsi_style_view(&style);
        style.radius = window_radius;
        style.border_size = window_border_size;
        style.padding = window_padding;
        style.margin = window_margin;

        struct nb_view_desc view_desc;
        memset(&view_desc, 0, sizeof(view_desc));
        view_desc.type_id = NB_STRUCT_VIEW;
        view_desc.ext = 0;
        view_desc.flags = flags;
        view_desc.name = "Styles";
        view_desc.height = size[0];
        view_desc.width = size[1];
        view_desc.pos_x = pos[0];
        view_desc.pos_y = pos[1];
        view_desc.user_data = 0;
        view_desc.view_type = 0; /* unsued atm */
        view_desc.style = &style;

        nb_view_t vi = 0;
        int interactions = 0;
        nb_result res = nb_view_create(c, &view_desc, &vi, &interactions);
        assert(res == NB_OK);

        if ((interactions & NB_VIEW_INTERACTION_CLOSED) && open) {
                *open = 0;
        }

        /* buttons */
        if(nbs_drag_int(vi, "Window Corners", &window_radius)) {
                window_radius = window_radius < 0 ? 0 : window_radius;
        }

        if (nbs_drag_int(vi, "Window Border Size", &window_border_size)) {
                window_border_size = window_border_size < 0 ? 0 : window_border_size;
        }

        if (nbs_drag_int(vi, "Window Padding Size", &window_padding)) {
                window_padding = window_padding < 0 ? 0 : window_padding;
        }

        if (nbs_drag_int(vi, "Window Margin Size", &window_margin)) {
                window_margin = window_margin < 0 ? 0 : window_margin;
        }

        /* how do I demo elements */


        nbs_view_end(vi);
}


void
inspector_demo(nb_ctx_t c, int *open)
{
        struct nb_state state;
        nb_state_get(c, &state);

        int size[2];
        size[0] = 300;
        size[1] = state.vp_height;

        int pos[2];
        pos[0] = state.vp_width - size[0];
        pos[1] = 0;

        char buffer[2048]; buffer[0] = 0;
        char * buf_pos = buffer;
        buf_pos += sprintf(buf_pos, "ptr state : %d\n", state.interaction);
        buf_pos += sprintf(buf_pos, "ptr pos (xy) : %d %d\n", state.ptr_x, state.ptr_y);
        buf_pos += sprintf(buf_pos, "ptr delta (dxdy): %d %d\n", state.ptr_dx, state.ptr_dy);
        buf_pos += sprintf(buf_pos, "view hash: %llu\n", state.hover_view_hash);
        buf_pos += sprintf(buf_pos, "ele hash: %llu\n", state.hover_element_hash);
        buf_pos += sprintf(buf_pos, "viewport (xy): %d %d\n", state.vp_width, state.vp_height);

        if(buf_pos > buffer) {
                buf_pos--;
                if(*buf_pos == '\n') {
                        *buf_pos = 0;
                }
        }

        const char *name = NB_ICON_CODE "##Inspector";

        int flags = NB_VIEW_TITLE | NB_VIEW_CLOSE | NB_VIEW_MINIMIZE;
        nb_view_t vi = nbs_view_window_begin(c, name, size, pos, flags, open);

        /* TEMP!! */
        static int font_idx = 0;
        if(nbs_button2(vi, "Button2")) {
                font_idx = (font_idx + 1) % nb_get_font_count(c);
                nb_debug_set_font(c, font_idx);
        }

        nbs_text(vi, "State", buffer, NB_TEXT_ALIGN_LEFT);

#if 0
        static char user_text[128];
        nbs_text_input(vi, "Text Input", user_text, sizeof(user_text), NB_TEXT_ALIGN_LEFT);
#endif

        nbs_view_end(vi);
}


void
node_demo(nb_ctx_t c, int *open) {


        int size[] = { 200, 300 };
        const char *node_1 = "Node 1";
        nb_view_t vi;
        void *other = 0;
        float pos[2];

        struct node_data {
                struct node_data *output;
                float o_pos[2];

                struct node_data *input;
                float i_pos[2];
        };

        static struct node_data a, b;

        /* node 1 */

        vi = nbs_view_window_begin(c, node_1, size, NULL, -1, open);

        if (nbs_node_input(vi, "input", &other, pos)) {
                struct node_data *other_node = (struct node_data*)other;
                b.input = other_node;
                other_node->output = &b;
        }

        if(b.input) {
                b.i_pos[0] = pos[0];
                b.i_pos[1] = pos[1];
        }

        nbs_view_end(vi);

        /* node 2 */

        const char *node_2 = "Node 2";
        vi = nbs_view_window_begin(c, node_2, size, NULL, -1, open);

        if (nbs_node_output(vi, "output", &a, pos)) {
                /* nbs_bez */
                // struct node_data *other_node = (struct node_data*)other;
        }

        nbs_view_end(vi);

        /* bez view */
        struct nb_state state;
        nb_state_get(c, &state);

        int default_flags = 0;

        struct nb_style style;
        nbsi_style_view(&style);
        style.border_size = 0;

        struct nb_view_desc view_desc;
        memset(&view_desc, 0, sizeof(view_desc));
        view_desc.type_id = NB_STRUCT_VIEW;
        view_desc.ext = 0;
        view_desc.flags = default_flags;
        view_desc.name = "Hidden Bez View";
        view_desc.height = 0;
        view_desc.width = 0;
        view_desc.pos_x = -150;
        view_desc.pos_y = -150;
        view_desc.user_data = 0;
        view_desc.view_type = 0; /* unsued atm */
        view_desc.style = &style;

        int interaction;
        nb_result res = nb_view_create(c, &view_desc, &vi, &interaction);
        assert(res == NB_OK);

        if ((interaction & NB_VIEW_INTERACTION_CLOSED) && open) {
                *open = 0;
        }

        if (a.output) {
                a.o_pos[0] = pos[0];
                a.o_pos[1] = pos[1];
        }

        if (a.output) {
                nbs_bez(vi, a.o_pos, b.i_pos);
        }

        nb_view_submit(vi);
}


void
sidebar_demo(nb_ctx_t c, const char **sh_names, const char **fu_names, int *values, int count)
{
        struct nb_state state;
        nb_state_get(c, &state);

        static int width = 75;

        int size[2];
        size[0] = width;
        size[1] = state.vp_height;

        int pos[] = {0, 0};

        const char *name = NB_ICON_SKULL " Demos";
        nb_view_t vi = nbs_view_window_begin(c, name, size, pos, NB_VIEW_TITLE, NULL);

        /*const char *expand_name = width == 75 ? ">>" : "<<";*/
        const char *expand_name = width == 75 ? NB_ICON_ANGLE_DOUBLE_RIGHT : NB_ICON_ANGLE_DOUBLE_LEFT;
        const char **names = width == 75 ? sh_names : fu_names;

        if (nbs_button(vi, expand_name)) {
                width = width == 75 ? 200 : 75;
        }

        int i;
        for(i = 0; i < count; ++i) {
                if (nbs_button(vi, names[i])) {
                        values[i] = values[i] ? 0 : 1;
                }
        }

        nbs_view_end(vi);
}


typedef void(*demo_func_fn)(nb_ctx_t, int *);
#define DEMO_COUNT 6


int
main()
{
        struct nb_ctx_create_desc ctx_desc;
        memset(&ctx_desc, 0, sizeof(ctx_desc));
        ctx_desc.type_id = NB_STRUCT_CTX_CREATE;

        nb_ctx_t ctx = 0;
        nb_ctx_create(&ctx_desc, &ctx);

        struct nb_glfw_ctx rdr_ctx;
        nb_glfw_setup(&rdr_ctx, ctx);

        const char *demo_short_names[DEMO_COUNT] = {
                "Basic",
                "Sugar",
                "View",
                "Styles",
                "Nodes",
                "Inspector"};

        const char *demo_full_names[DEMO_COUNT] = {
                "Basic Example",
                "Sugar Widgets",
                "View Properties",
                "Style Properties",
                "Node Example",
                "Inspector" };

        demo_func_fn fns[DEMO_COUNT] = {
                basic_demo,
                widget_demo,
                window_properties_demo,
                style_properties_demo,
                node_demo,
                inspector_demo};

        int demo_results[DEMO_COUNT] = {
                0,
                0,
                0,
                0,
                0,
                1};

        while(nb_glfw_tick(&rdr_ctx, ctx)) {

                /* demo nebuala code */
                nb_frame_begin(ctx);

                sidebar_demo(ctx, demo_short_names, demo_full_names, demo_results, DEMO_COUNT);

                int i;
                for (i = 0; i < DEMO_COUNT; ++i) {
                        if (demo_results[i]) {
                                fns[i](ctx, &demo_results[i]);
                        }
                }

                nb_frame_submit(ctx);

                /* clear buffer */
                glClearColor(0.157, 0.153, 0.161, 1);
                glClear(GL_COLOR_BUFFER_BIT);

                nbgl_render(rdr_ctx.display_width, rdr_ctx.display_height, &rdr_ctx.gl_ctx, ctx);
        }


        return 0;
}
