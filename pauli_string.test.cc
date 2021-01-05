#include "gtest/gtest.h"
#include "pauli_string.h"
#include <random>

TEST(pauli_string, str) {
    auto p1 = PauliStringVal::from_str("+IXYZ");
    ASSERT_EQ(p1.str(), "+_XYZ");

    auto p2 = PauliStringVal::from_str("X");
    ASSERT_EQ(p2.str(), "+X");

    auto p3 = PauliStringVal::from_str("-XZ");
    ASSERT_EQ(p3.str(), "-XZ");

    auto s1 = PauliStringVal::from_pattern(
            true,
            24*24,
            [](size_t i) { return "_XYZ"[i & 3]; });
    ASSERT_EQ(s1.str(),
              "-"
              "_XYZ_XYZ_XYZ_XYZ_XYZ_XYZ" "_XYZ_XYZ_XYZ_XYZ_XYZ_XYZ"
              "_XYZ_XYZ_XYZ_XYZ_XYZ_XYZ" "_XYZ_XYZ_XYZ_XYZ_XYZ_XYZ"
              "_XYZ_XYZ_XYZ_XYZ_XYZ_XYZ" "_XYZ_XYZ_XYZ_XYZ_XYZ_XYZ"
              "_XYZ_XYZ_XYZ_XYZ_XYZ_XYZ" "_XYZ_XYZ_XYZ_XYZ_XYZ_XYZ"
              "_XYZ_XYZ_XYZ_XYZ_XYZ_XYZ" "_XYZ_XYZ_XYZ_XYZ_XYZ_XYZ"
              "_XYZ_XYZ_XYZ_XYZ_XYZ_XYZ" "_XYZ_XYZ_XYZ_XYZ_XYZ_XYZ"
              "_XYZ_XYZ_XYZ_XYZ_XYZ_XYZ" "_XYZ_XYZ_XYZ_XYZ_XYZ_XYZ"
              "_XYZ_XYZ_XYZ_XYZ_XYZ_XYZ" "_XYZ_XYZ_XYZ_XYZ_XYZ_XYZ"
              "_XYZ_XYZ_XYZ_XYZ_XYZ_XYZ" "_XYZ_XYZ_XYZ_XYZ_XYZ_XYZ"
              "_XYZ_XYZ_XYZ_XYZ_XYZ_XYZ" "_XYZ_XYZ_XYZ_XYZ_XYZ_XYZ"
              "_XYZ_XYZ_XYZ_XYZ_XYZ_XYZ" "_XYZ_XYZ_XYZ_XYZ_XYZ_XYZ"
              "_XYZ_XYZ_XYZ_XYZ_XYZ_XYZ" "_XYZ_XYZ_XYZ_XYZ_XYZ_XYZ");
}

TEST(pauli_string, log_i_scalar_byproduct) {
    auto id_val = PauliStringVal::from_str("_");
    auto x_val = PauliStringVal::from_str("X");
    auto y_val = PauliStringVal::from_str("Y");
    auto z_val = PauliStringVal::from_str("Z");
    PauliStringPtr id = id_val;
    PauliStringPtr x = x_val;
    PauliStringPtr y = y_val;
    PauliStringPtr z = z_val;

    ASSERT_EQ(id.log_i_scalar_byproduct(id), 0);
    ASSERT_EQ(id.log_i_scalar_byproduct(x), 0);
    ASSERT_EQ(id.log_i_scalar_byproduct(y), 0);
    ASSERT_EQ(id.log_i_scalar_byproduct(z), 0);

    ASSERT_EQ(x.log_i_scalar_byproduct(id), 0);
    ASSERT_EQ(x.log_i_scalar_byproduct(x), 0);
    ASSERT_EQ(x.log_i_scalar_byproduct(y), 1);
    ASSERT_EQ(x.log_i_scalar_byproduct(z), 3);

    ASSERT_EQ(y.log_i_scalar_byproduct(id), 0);
    ASSERT_EQ(y.log_i_scalar_byproduct(x), 3);
    ASSERT_EQ(y.log_i_scalar_byproduct(y), 0);
    ASSERT_EQ(y.log_i_scalar_byproduct(z), 1);

    ASSERT_EQ(z.log_i_scalar_byproduct(id), 0);
    ASSERT_EQ(z.log_i_scalar_byproduct(x), 1);
    ASSERT_EQ(z.log_i_scalar_byproduct(y), 3);
    ASSERT_EQ(z.log_i_scalar_byproduct(z), 0);

    ASSERT_EQ(PauliStringVal::from_str("XX").ptr().log_i_scalar_byproduct(
            PauliStringVal::from_str("XY")), 1);
    ASSERT_EQ(PauliStringVal::from_str("XX").ptr().log_i_scalar_byproduct(
            PauliStringVal::from_str("ZY")), 0);
    ASSERT_EQ(PauliStringVal::from_str("XX").ptr().log_i_scalar_byproduct(
            PauliStringVal::from_str("YY")), 2);
    for (size_t n : std::vector<size_t>{1, 499, 4999, 5000}) {
        auto all_x = PauliStringVal::from_pattern(false, n, [](size_t i) { return 'X'; });
        auto all_z = PauliStringVal::from_pattern(false, n, [](size_t i) { return 'Z'; });
        ASSERT_EQ(all_x.ptr().log_i_scalar_byproduct(all_z), -(int) n & 3);
    }
}

TEST(pauli_string, equality) {
    ASSERT_TRUE(PauliStringVal::from_str("") == PauliStringVal::from_str(""));
    ASSERT_FALSE(PauliStringVal::from_str("") != PauliStringVal::from_str(""));
    ASSERT_FALSE(PauliStringVal::from_str("") == PauliStringVal::from_str("-"));
    ASSERT_FALSE(PauliStringVal::from_str("X") == PauliStringVal::from_str(""));
    ASSERT_TRUE(PauliStringVal::from_str("XX") == PauliStringVal::from_str("XX"));
    ASSERT_FALSE(PauliStringVal::from_str("XX") == PauliStringVal::from_str("XY"));
    ASSERT_FALSE(PauliStringVal::from_str("XX") == PauliStringVal::from_str("XZ"));
    ASSERT_FALSE(PauliStringVal::from_str("XX") == PauliStringVal::from_str("X_"));

    auto all_x1 = PauliStringVal::from_pattern(false, 1000, [](size_t i) { return 'X'; });
    auto all_x2 = PauliStringVal::from_pattern(false, 1000, [](size_t i) { return 'X'; });
    auto all_z = PauliStringVal::from_pattern(false, 1000, [](size_t i) { return 'Z'; });
    ASSERT_EQ(all_x1, all_x2);
    ASSERT_NE(all_x1, all_z);
}

TEST(pauli_string, multiplication) {
    auto x = PauliStringVal::from_str("X");
    auto y = PauliStringVal::from_str("Y");
    auto z = PauliStringVal::from_str("Z");

    auto lhs = x;
    uint8_t log_i = lhs.ptr().inplace_right_mul_with_scalar_output(y);
    ASSERT_EQ(log_i, 1);
    ASSERT_EQ(lhs, z);

    auto xxi = PauliStringVal::from_str("XXI");
    auto yyy = PauliStringVal::from_str("YYY");
    xxi.ptr() *= yyy;
    ASSERT_EQ(xxi, PauliStringVal::from_str("-ZZY"));
}

TEST(pauli_string, identity) {
    ASSERT_EQ(PauliStringVal::identity(5).str(), "+_____");
}

TEST(pauli_string, gather) {
    auto p = PauliStringVal::from_str("-____XXXXYYYYZZZZ");
    auto p2_val = PauliStringVal::identity(4);
    PauliStringPtr p2 = p2_val;
    p.ptr().gather_into(p2, {0, 1, 2, 3});
    ASSERT_EQ(p2, PauliStringVal::from_str("+IIII"));
    p.ptr().gather_into(p2, {4, 7, 8, 9});
    ASSERT_EQ(p2, PauliStringVal::from_str("+XXYY"));
}

TEST(pauli_string, scatter) {
    auto s1 = PauliStringVal::from_str("-_XYZ");
    auto s2 = PauliStringVal::from_str("+XXZZ");
    auto p_val = PauliStringVal::identity(8);
    PauliStringPtr p = p_val;
    s1.ptr().scatter_into(p, {1, 3, 5, 7});
    ASSERT_EQ(p, PauliStringVal::from_str("-___X_Y_Z"));
    s1.ptr().scatter_into(p, {1, 3, 5, 7});
    ASSERT_EQ(p, PauliStringVal::from_str("+___X_Y_Z"));
    s2.ptr().scatter_into(p, {1, 3, 5, 7});
    ASSERT_EQ(p, PauliStringVal::from_str("+_X_X_Z_Z"));
    s2.ptr().scatter_into(p, {4, 5, 6, 7});
    ASSERT_EQ(p, PauliStringVal::from_str("+_X_XXXZZ"));
}

TEST(pauli_string, move_copy_assignment) {
    PauliStringVal x = PauliStringVal::from_str("XYZ");

    // Move.
    x = std::move(PauliStringVal::from_str("XXY"));
    ASSERT_EQ(x, PauliStringVal::from_str("XXY"));
    x = std::move(PauliStringVal::from_str("-IIX"));
    ASSERT_EQ(x, PauliStringVal::from_str("-IIX"));

    // Copy.
    auto y = PauliStringVal::from_str("ZZZ");
    x = y;
    ASSERT_EQ(x, PauliStringVal::from_str("ZZZ"));
    y = PauliStringVal::from_str("-ZZZ");
    x = y;
    ASSERT_EQ(x, PauliStringVal::from_str("-ZZZ"));
}

TEST(pauli_string, foreign_memory) {
    size_t bits = 2048;
    auto buffer = aligned_bits256(bits);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned long long> dis(
            std::numeric_limits<std::uint64_t>::min(),
            std::numeric_limits<std::uint64_t>::max());
    for (size_t k = 0; k < (bits / 64); k++) {
        buffer.data[k] = dis(gen);
    }
    bool sign1 = false;
    bool sign2 = false;

    auto p1 = PauliStringPtr(500, &sign1, &buffer.data[0], &buffer.data[16]);
    auto p1b = new PauliStringPtr(500, &sign1, &buffer.data[0], &buffer.data[16]);
    auto p2 = PauliStringPtr(500, &sign2, &buffer.data[32], &buffer.data[64]);
    PauliStringVal copy_p1 = p1;
    // p1 aliases p1b.
    ASSERT_EQ(p1, *p1b);
    ASSERT_EQ(p1, copy_p1);
    p1.inplace_right_mul_with_scalar_output(p2);
    ASSERT_EQ(p1, *p1b);
    ASSERT_NE(p1, copy_p1);
    // Deleting p1b shouldn't delete the backing buffer. So p1 survives.
    copy_p1 = p1;
    ASSERT_EQ(p1, copy_p1);
    delete p1b;
    ASSERT_EQ(p1, copy_p1);
}
