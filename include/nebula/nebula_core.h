#ifndef NEBULA_CORE_INCLUDED
#define NEBULA_CORE_INCLUDED


#ifndef __cplusplus
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
#else
#include <cstdint>
#include <cstddef>
#endif


typedef enum nbc_result {
        NBC_OK,
        NBC_FAIL,
} nbc_result;


struct nbc_vec2 {
        float x, y;
};

struct nbc_rect {
        struct nbc_vec2 point, size;
};


typedef enum nbc_ptr_state {
        PTR_DOWN,
        PTR_DOWN_EVENT,
        PTR_UP,
        PTR_UP_EVENT,
} nbc_ptr_state;


struct nbc_ctx {
        /* ptr state */
        struct nbc_vec2 ptr_pos;
        struct nbc_vec2 ptr_delta_pos;
        nbc_ptr_state ptr_state;
        uintptr_t ptr_colliding_with;

        /* array */ struct nbc_collider *colliders;
};


struct nbc_collider {
        int unique_id;
        int flags;
};


struct nbc_interactions {
        int interaction_flags;
};


struct nbc_collider_desc {
        struct nbc_rect area;
};


nbc_result
nbc_collider_add(
        struct nbc_ctx *ctx,
        struct nbc_collider_desc *desc,
        struct nbc_interactions *out_interactions);


static struct nbc_vec2
nbc_vec2_init(float x, float y) {
        struct nbc_vec2 vec;
        vec.x = x;
        vec.y = y;

        return vec;
}

#endif


/* -- IMPL -- */


#ifdef NEBULA_CORE_IMPL
#ifndef NEBULA_CORE_IMPL_INCLUDED
#define NEBULA_CORE_IMPL_INCLUDED


nbc_result
nbc_collider_add(
        struct nbc_ctx *ctx,
        struct nbc_collider_desc *desc,
        struct nbc_interactions *out_interactions)
{
        return NBC_FAIL;
}


#endif
#endif


