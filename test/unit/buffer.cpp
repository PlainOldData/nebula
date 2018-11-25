#include <nebula/nebula.h>
#include <catch/catch.hpp>


TEST_CASE("Buffer")
{
        nb_buffer *buffer = nullptr;
        buffer = nbi_buffer_alloc();

        REQUIRE(buffer != nullptr);

        SECTION("General use")
        {
                int capacity = sizeof(int) * 4;
                int *some_data = (int*)nbi_buffer_create(buffer, capacity);
                *some_data = 123;
                int *check_data = (int*)nbi_buffer_mem(buffer);
                REQUIRE(*some_data == *check_data);

                int check_capacity = nbi_buffer_capcity(buffer);
                REQUIRE(capacity == check_capacity);

                int *new_data = (int*)nbi_buffer_push(buffer, sizeof(int));
                int *check_new_data = (int*)nbi_buffer_mem(buffer);
                REQUIRE(*new_data == *check_new_data);

                nbi_buffer_destroy(buffer);
                void *mem = nbi_buffer_mem(buffer);
                REQUIRE(mem == nullptr);
        }

        SECTION("Fill to capacity then realloc")
        {
                int count = 4;

                int capacity = sizeof(int) * count;
                int *start = (int*)nbi_buffer_create(buffer, capacity);

                for (int i = 0; i < count; ++i)
                {
                        int *data = (int*)nbi_buffer_push(buffer, sizeof(int));
                        *data = i;
                }

                for (int i = 0; i < count; ++i) {
                        REQUIRE(start[i] == i);
                }

                int check_capacity = nbi_buffer_capcity(buffer);
                REQUIRE(check_capacity == capacity);

                int check_size = nbi_buffer_used(buffer);
                REQUIRE(check_size == check_capacity);

                /* break buffer size */
                int *data = (int*)nbi_buffer_push(buffer, sizeof(int));
                *data = count;

                start = (int*)nbi_buffer_mem(buffer);

                int check_new_capacity = nbi_buffer_capcity(buffer);
                REQUIRE(check_capacity < check_new_capacity);

                int check_new_size = nbi_buffer_used(buffer);
                REQUIRE(check_size < check_new_size);

                for (int i = 0; i < count + 1; ++i) {
                        REQUIRE(start[i] == i);
                }
        }

        nbi_buffer_free(buffer);
        buffer = 0;
}
