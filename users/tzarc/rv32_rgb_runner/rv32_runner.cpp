#include <array>

extern "C" int cpp_test(void) {
    std::array<int, 3> arr = {1, 2, 3};
    int ret = 0;
    for(auto&& i : arr) {
        ret += i;
    }
    return ret;
}
