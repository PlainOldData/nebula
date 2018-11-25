#include <nebula/nebula.h>
#include <catch/catch.hpp>


TEST_CASE("State") {

        struct nb_ctx_create_desc desc_ctx{};
        desc_ctx.type_id = NB_STRUCT_CTX_CREATE;

        nb_ctx_t ctx = nullptr;
        auto ctx_res = nb_ctx_create(&desc_ctx, &ctx);

        REQUIRE(ctx_res == NB_OK);
        REQUIRE(ctx != nullptr);

        SECTION("Set Pointer - INVALID PARAMS 1")
        {
                auto res = nb_state_set_pointer(nullptr, nullptr);
                REQUIRE(res == NB_INVALID_PARAMS);       
        }

        SECTION("Set Pointer - INVALID PARAMS 2")
        {
                auto res = nb_state_set_pointer(ctx, nullptr);
                REQUIRE(res == NB_INVALID_PARAMS);
        }

        SECTION("Set Pointer - INVALID PARAMS 3")
        {
                struct nb_pointer_desc desc{};
                desc.type_id = NB_STRUCT_POINTER;

                auto res = nb_state_set_pointer(nullptr, &desc);
                REQUIRE(res == NB_INVALID_PARAMS);
        }

        SECTION("Set Pointer - INVALID DESC")
        {
                struct nb_pointer_desc desc{};

                auto res = nb_state_set_pointer(ctx, &desc);
                REQUIRE(res == NB_INVALID_DESC);
        }

        SECTION("Set Pointer - OK")
        {
                struct nb_pointer_desc desc{};
                desc.type_id = NB_STRUCT_POINTER;

                auto res = nb_state_set_pointer(ctx, &desc);

                REQUIRE(res == NB_OK);
        }

        SECTION("Set Pointer data")
        {
                struct nb_pointer_desc desc{};
                desc.type_id = NB_STRUCT_POINTER;
                desc.x = 123;
                desc.y = 234;

                nb_state_set_pointer(ctx, &desc);

                struct nb_state state{};

                nb_state_get(ctx, &state);

                REQUIRE(state.ptr_x == 123);
                REQUIRE(state.ptr_y == 234);
        }

        SECTION("Set Viewport - INVALID PARAMS 1")
        {
                auto res = nb_state_set_viewport(nullptr, nullptr);
                REQUIRE(res == NB_INVALID_PARAMS);
        }

        SECTION("Set Viewport - INVALID PARAMS 2")
        {
                auto res = nb_state_set_viewport(ctx, nullptr);
                REQUIRE(res == NB_INVALID_PARAMS);
        }

        SECTION("Set Viewport - INVALID PARAMS 3")
        {
                struct nb_viewport_desc desc{};
                desc.type_id = NB_STRUCT_VIEWPORT;

                auto res = nb_state_set_viewport(nullptr, &desc);
                REQUIRE(res == NB_INVALID_PARAMS);
        }

        SECTION("Set Viewport - INVALID DESC")
        {
                struct nb_viewport_desc desc{};

                auto res = nb_state_set_viewport(ctx, &desc);
                REQUIRE(res == NB_INVALID_DESC);
        }

        SECTION("Set Viewport - OK")
        {
                struct nb_viewport_desc desc{};
                desc.type_id = NB_STRUCT_VIEWPORT;

                auto res = nb_state_set_viewport(ctx, &desc);

                REQUIRE(res == NB_OK);
        }

        SECTION("Set Viewport data")
        {
                struct nb_viewport_desc desc{};
                desc.type_id = NB_STRUCT_VIEWPORT;
                desc.width = 1234;
                desc.height = 4567;

                nb_state_set_viewport(ctx, &desc);

                struct nb_state state{};
                nb_state_get(ctx, &state);

                REQUIRE(state.vp_width == 1234);
                REQUIRE(state.vp_height == 4567);
        }

        SECTION("Get state - INVALID PARAMS 1")
        {
                auto res = nb_state_get(nullptr, nullptr);

                REQUIRE(res == NB_INVALID_PARAMS);
        }

        SECTION("Get state - INVALID PARAMS 2")
        {
                auto res = nb_state_get(ctx, nullptr);

                REQUIRE(res == NB_INVALID_PARAMS);
        }

        SECTION("Get state - INVALID PARAMS 3")
        {
                struct nb_state state{};
                auto res = nb_state_get(nullptr, &state);

                REQUIRE(res == NB_INVALID_PARAMS);
        }


        ctx_res = nb_ctx_destroy(&ctx);
        REQUIRE(ctx_res == NB_OK);
        REQUIRE(ctx == nullptr);
}
