#include"gtest/gtest.h"
#include"../src/my_obj_loader/my_obj_loader.h"

#ifdef _DEBUG   // DEBUG mode for visual studio debug
#define GET_ASSET(name) "../../assets/"#name
#else           // RELEASE mode for unit test
#define GET_ASSET(name) "../../../assets/"#name
#endif

TEST(my_obj_loader, triangle)
{
    my_obj_elements mesh;
    int result = my_obj_get_mesh(GET_ASSET(triangle.obj), &mesh);
    EXPECT_EQ(result, 0);
    EXPECT_EQ(mesh.num_v, 3);
    EXPECT_EQ(mesh.num_vn, 0);
    EXPECT_EQ(mesh.num_f, 1);
    EXPECT_EQ(mesh.f_buffer[0].v1, 1);
    EXPECT_EQ(mesh.f_buffer[0].v2, 2);
    EXPECT_EQ(mesh.f_buffer[0].v3, 3);
    my_obj_free_mesh(&mesh);
}

TEST(my_obj_loader, cube)
{
    my_obj_elements mesh;
    int result = my_obj_get_mesh(GET_ASSET(cube.obj), &mesh);
    EXPECT_EQ(result, 0);
    EXPECT_EQ(mesh.num_v, 8);
    EXPECT_EQ(mesh.num_vn, 6);
    EXPECT_EQ(mesh.num_f, 12);
    my_obj_free_mesh(&mesh);
}

TEST(my_obj_loader, bunny)
{
    my_obj_elements elements;
    int result = my_obj_get_mesh(GET_ASSET(bunny.obj), &elements);
    ASSERT_EQ(result, 0);
    EXPECT_EQ(34820 - 3, elements.num_v);
    EXPECT_FLOAT_EQ(elements.v_buffer[0].x, (float)0.1102022);
    EXPECT_FLOAT_EQ(elements.v_buffer[0].y, (float)0.74011);
    EXPECT_FLOAT_EQ(elements.v_buffer[0].z, (float)1.132398);

    EXPECT_FLOAT_EQ(elements.v_buffer[34816].x, (float)-1.21614);
    EXPECT_FLOAT_EQ(elements.v_buffer[34816].y, (float)2.41936);
    EXPECT_FLOAT_EQ(elements.v_buffer[34816].z, (float)-1.214766);

    EXPECT_EQ(69637 - 34820, elements.num_vn);

    EXPECT_FLOAT_EQ(elements.vn_buffer[0].x, (float)0.205353);
    EXPECT_FLOAT_EQ(elements.vn_buffer[0].y, (float)0.264789);
    EXPECT_FLOAT_EQ(elements.vn_buffer[0].z, (float)0.942187);

    EXPECT_FLOAT_EQ(elements.vn_buffer[34816].x, (float)0.993931);
    EXPECT_FLOAT_EQ(elements.vn_buffer[34816].y, (float)-0.0189374);
    EXPECT_FLOAT_EQ(elements.vn_buffer[34816].z, (float)0.108366);

    EXPECT_EQ(139267 - 69637, elements.num_f);

    EXPECT_EQ(elements.f_buffer[0].v1, 11250);
    EXPECT_EQ(elements.f_buffer[0].v2, 4406);
    EXPECT_EQ(elements.f_buffer[0].v3, 31248);

    EXPECT_EQ(elements.f_buffer[0].vt1, -1);
    EXPECT_EQ(elements.f_buffer[0].vt2, -1);
    EXPECT_EQ(elements.f_buffer[0].vt3, -1);

    EXPECT_EQ(elements.f_buffer[0].vn1, 11250);
    EXPECT_EQ(elements.f_buffer[0].vn2, 4406);
    EXPECT_EQ(elements.f_buffer[0].vn3, 31248);

    EXPECT_EQ(elements.f_buffer[69629].v1, 49);
    EXPECT_EQ(elements.f_buffer[69629].v2, 27123);
    EXPECT_EQ(elements.f_buffer[69629].v3, 34817);

    EXPECT_EQ(elements.f_buffer[69629].vt1, -1);
    EXPECT_EQ(elements.f_buffer[69629].vt2, -1);
    EXPECT_EQ(elements.f_buffer[69629].vt3, -1);

    EXPECT_EQ(elements.f_buffer[69629].vn1, 49);
    EXPECT_EQ(elements.f_buffer[69629].vn2, 27123);
    EXPECT_EQ(elements.f_buffer[69629].vn3, 34817);

    my_obj_free_mesh(&elements);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    my_obj_set_allocator(malloc, free);
    return RUN_ALL_TESTS();
}