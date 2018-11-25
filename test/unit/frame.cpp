#include <catch/catch.hpp>
#include <nebula/nebula.h>


TEST_CASE("Frame")
{
        struct nb_ctx_create_desc desc{};
        desc.type_id = NB_STRUCT_CTX_CREATE;

        nb_ctx_t ctx = nullptr;

        auto res = nb_ctx_create(&desc, &ctx);
        REQUIRE(res == NB_OK);

        SECTION("New Frame - OK")
        {
                res = nb_frame_begin(ctx);
                REQUIRE(res == NB_OK);

                res = nb_frame_submit(ctx);
                REQUIRE(res == NB_OK);
        }

        nb_ctx_destroy(&ctx);
}
