#include <iostream>
#include "sort.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    int a[] = {12, 123, 321, 41, 11, 125, 1246, 2312, 125};
    int n = 9;

    myalg::sort(a, a + n, [](int a, int b) { return a < b; });

    for (int i = 0; i < n; i++) {
        std::cout << a[i] << ' ';
    }
    std::cout << std::endl;
    return 0;
}
