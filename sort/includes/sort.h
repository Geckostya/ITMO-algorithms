#ifndef SORT_SORT_H
#define SORT_SORT_H

#include <cassert>
#include <algorithm>

namespace myalg {
    bool use_insertion_sort = true;
    const int INSERTION_SORT_LENGTH = 11;

    template<typename T, typename Compare>
    void partition(T *&first, T *&last, T m, Compare comp) {
        while (first < last) {
            while (comp(*first, m) && !comp(m, *first)) first++;
            while (comp(m, *(last - 1)) && !comp(*(last - 1), m)) last--;
            if (first < last) std::iter_swap(first++, --last);
        }
    }

    template<typename T, typename Compare>
    void sort3(T &a, T &b, T &c, Compare comp) {
        if (comp(b, a)) std::swap(a, b);
        if (comp(c, a)) std::swap(a, c);
        if (comp(c, b)) std::swap(b, c);
    }

    template<typename T, typename Compare>
    void insertion_sort(T *first, T *last, Compare comp) {
        for (T *i = first; i < last; i++) {
            for (T *j = i; j > first && comp(*j, *(j - 1)); j--)
                std::iter_swap(j - 1, j);
        }
    }

    template<typename T, typename Compare>
    void sort(T *first, T *last, Compare comp) {
        while (first < last) {
            int n = last - first;

            if (use_insertion_sort && n <= INSERTION_SORT_LENGTH) {
                insertion_sort(first, last, comp);
                return;
            }

            T a0 = *first, a1 = *(last - 1), a2 = *(first + n / 2);
            sort3(a0, a1, a2, comp);

            T *f = first, *l = last;
            partition(f, l, a1, comp);

            if (l - first < last - f) {
                std::swap(first, f);
            } else {
                std::swap(last, l);
            }
            sort(f, l, comp);
        }
    }
}

#endif //SORT_SORT_H
