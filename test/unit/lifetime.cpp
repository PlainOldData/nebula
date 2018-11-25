#include <nebula/nebula.h>
#include <catch/catch.hpp>


TEST_CASE("lifetime") {

        SECTION("Create - INVALID PARAM 1")
        {
                nb_ctx_t ctx = nullptr;
                auto res = nb_ctx_create(nullptr, &ctx);

                REQUIRE(ctx == nullptr);
                REQUIRE(res == NB_INVALID_PARAMS);
        }

        SECTION("Create - INVALID PARAM 2")
        {
                struct nb_ctx_create_desc desc{};
            
                auto res = nb_ctx_create(&desc, nullptr);

                REQUIRE(res == NB_INVALID_PARAMS);
        }

        SECTION("Create - INVALID PARAM 3")
        {
                auto res = nb_ctx_create(nullptr, nullptr);

                REQUIRE(res == NB_INVALID_PARAMS);
        }

        SECTION("Create - INVALID PARAM 3")
        {
                struct nb_ctx_create_desc desc{};

                auto res = nb_ctx_create(&desc, nullptr);
                REQUIRE(res == NB_INVALID_PARAMS);
        }

        SECTION("Create - OK")
        {
                struct nb_ctx_create_desc desc{};
                desc.type_id = NB_STRUCT_CTX_CREATE;

                nb_ctx_t ctx = nullptr;

                auto res = nb_ctx_create(&desc, &ctx);
                REQUIRE(res == NB_OK);
        }

        SECTION("Destroy - INVALID PARAM 1")
        {
                auto res = nb_ctx_destroy(nullptr);
                REQUIRE(res == NB_INVALID_PARAMS);
        }

        SECTION("Destroy - OK")
        {
                nb_ctx_t ctx = nullptr;
                struct nb_ctx_create_desc desc{};
                desc.type_id = NB_STRUCT_CTX_CREATE;

                nb_ctx_create(&desc, &ctx);

                auto res = nb_ctx_destroy(&ctx);
                REQUIRE(res == NB_OK);
                REQUIRE(ctx == nullptr);
        }

}
