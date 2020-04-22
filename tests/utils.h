#include <vector>

std::string BuildRandomNumber(int maxdigits, int seed);

template<typename T>
std::vector<unsigned char> to_byte_array(T value) {
    std::vector<unsigned char> v;
    v.reserve(sizeof(T));
    for (int i = 0; i < sizeof(T); i++) {
        v.push_back((value >> 8 * i) & 0xFF);
    }
    return v;
}

#define EXPECT_THROW_WITH_MESSAGE(stmt, etype, whatstring) EXPECT_THROW( \
        try { \
            stmt; \
        } catch (const etype& ex) { \
            EXPECT_EQ(std::string(ex.what()), whatstring); \
            throw; \
        } \
    , etype)