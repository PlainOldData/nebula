#ifndef NEB_CORE_INCLUDED
#define NEB_CORE_INCLUDED


struct nb_collider {
        int x,y,w,h;
        uint64_t unique_id;
};


struct nb_window {
        struct nb_collider base;
        struct nb_collider colliders[256];
};


struct nb_ptr_state {
        int x,y,dx,dy;
        int state;
};


struct nb_core_ctx {
        /* array */ struct nb_window *windows[256];
        struct nb_ptr_state state;
};


struct nb_interaction {
        int dx,dy;
        int clicked;
};




#endif

#ifdef NEB_CORE_IMPL
#ifndef NEB_CORE_IMPL_INCLUDED
#define NEB_CORE_IMPL_INCLUDED





#endif
#endif



struct nb_controller {
        struct nb_view *view;
        struct nb_model *model;
};



struct nb_view_ctx {
        struct nb_view *views[256];
};


struct nb_model_ctx {
        struct nb_models *models[256];
};



struct nb_ctx {
        struct nb_view_ctx *rdr;
        struct nb_model_ctx *state;

        struct nb_controller *controllers[256];
};



struct nb_controller*
nbc_window_begin(nb_ctx *ctx, const char *name) {

        find_controller(name);
        else create();

        
};




