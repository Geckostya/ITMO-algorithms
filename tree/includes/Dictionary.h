//
// Created by ko on 22.07.2020.
//

#ifndef ALGORITHMS_TREE_H
#define ALGORITHMS_TREE_H

#include <algorithm>

namespace myalg {
    template<typename K, typename V>
    struct node {
        K k;
        V v;
        int rank = 0;

        node *p = nullptr, *l = nullptr, *r = nullptr;

        node(const K &k, const V &v) : k(k), v(v) {}

    private:
        static void data_swap(node *n1, node *n2) {
            std::swap(n1->k, n2->k);
            std::swap(n1->v, n2->v);
        }

        void set(node* nl, node *nr, node *np) {
            l = nl;
            r = nr;
            p = np;
            update_rank(this);
        }

        static int get_rank(node* n) {
            return n ? n->rank : -1;
        }

        static node* fix_parent(node* n, node* x) {
            if (n->p) {
                (n->p->l == x ? n->p->l : n->p->r) = n;
            }
            return n;
        }

        static node* update_rank(node* n) {
            n->rank = std::max(get_rank(n->r), get_rank(n->l)) + 1;
            return n;
        }

        static node* big_rotate(node *p, node *x, node *y, node *z) {
            node *b = z->l, *c = z->r;
            y->set(y->l, b, z);
            x->set(c, x->r, z);
            z->set(y, x, p);
            if (b) b->p = y;
            if (c) c->p = x;
            return z;
        }

        static node* big_rotate_right(node *x) {
            node* y = x->l, *z = y->r;
            node* res = big_rotate(x->p, x, y, z);

            return res;
        }

        static node* big_rotate_left(node *y) {
            node* x = y->r, *z = x->l;
            return big_rotate(y->p, x, y, z);
        }

        static node* rotate_right(node *x) {
            if (get_rank(x->l->r) > get_rank(x->l->l)) {
                return big_rotate_right(x);
            }
            node *y = x->l, *b = y->r, *p = x->p;
            x->set(b, x->r, y);
            y->set(y->l, x, p);
            if (b) b->p = x;
            return y;
        }

        static node* rotate_left(node *y) {
            if (get_rank(y->r->l) > get_rank(y->r->r)) {
                return big_rotate_left(y);
            }
            node *x = y->r, *b = x->l, *p = y->p;
            y->set(y->l, b, x);
            x->set(y, x->r, p);
            if (b) b->p = y;
            return x;
        }

        static bool disbalance(node *a, node *b) {
            return get_rank(a) > get_rank(b) + 1;
        }

        static node* relax(node *n) {
           return disbalance(n->l, n->r) ? fix_parent(rotate_right(n), n)
                    : disbalance(n->r, n->l) ? fix_parent(rotate_left(n), n)
                    : update_rank(n);
        }

    public:

        void del_rec() {
            if (r) {
                r->del_rec();
                delete r;
            }
            if (l) {
                l->del_rec();
                delete l;
            }
        }

        static node* first(node *root) {
            node* n = root;
            for (; n && n->l; n = n->l);
            return n;
        }

        static node* next(node *n) {
            if (n->r) {
                n = n->r;
                while (n->l) n = n->l;
            } else {
                while (n->p && n->p->r == n) n = n->p;
                n = n->p;
            }
            return n;
        }

        static node* prev(node *n) {
            if (n->l) {
                n = n->l;
                while (n->r) n = n->r;
            } else {
                while (n->p && n->p->l == n) n = n->p;
                n = n->p;
            }
            return n;
        }

        static node* add(node *&n, const K &k, const V &v) {
            if (!n)
                n = new node<K, V>(k, v);
            else
                add(k > n->k ? n->r : n->l, k, v)->p = n;
            assert(!(n->r) || n->k < n->r->k && n->r->p == n);
            assert(!(n->l) || n->k > n->l->k && n->l->p == n);
            return n = relax(n);
        }

        static node* find(node *n, const K &k) {
            if (!n || n->k == k) return n;
            return find(k > n->k ? n->r : n->l, k);
        }

        static bool del(node *& root, node *n) {
            if (!n) return false;
            if (n->l && n->r) {
                node *next = node::next(n);
                data_swap(n, next);
                return del(root, next);
            }
            node *p = n->p;
            node *child = n->l ? n->l : n->r;
            if (child) child->p = p;
            if (p) {
                (p->l == n ? p->l : p->r) = child;
            }
            delete n;
            root = child;
            while (p) {
                p = (root = relax(p))->p;
                assert(!p || (!p->r || p->r->p == p) && (!p->l || p->l->p == p));
            }
            return true;
        }
    };

    template<typename K, typename V>
    class Dictionary {
        typedef node<K, V> my_node;

        my_node* root = nullptr;

        int my_size = 0;

    private:
        void put_new(const K &k, const V &v) {
            root = my_node::add(root, k, v);
            my_size++;
        }

    public:
        ~Dictionary() {
            if (root) {
                root->del_rec();
                delete root;
            }
        }

        my_node* find(const K &k) const {
            return my_node::find(root, k);
        }
        
        V& find(const K &k, const V &default_value) const {
            my_node* n = find(k);
            return n ? n->v : default_value;
        }

        void remove(const K &k) {
            if (my_node::del(root, find(k))) {
                my_size--;
            }
        }

        bool contains(const K &k) const {
            return find(k);
        }

        const V& at(const K &k) const {
            return find(k)->v;
        }

        V& operator[](const K &k) {
            auto *node = find(k);
            if (node != nullptr) return node->v;
            put_new(k, V());
            return find(k)->v;
        }

        void put(const K &k, const V &v) {
            this->operator[](k) = v;
        }

        int size() const {
            return my_size;
        }

        class Iterator {
            node<K, V> *my_node;
            friend class Dictionary;
            explicit Iterator(node<K, V> *node) : my_node(node) {}

        public:
            const K& key() const {
                return my_node->k;
            }

            V& operator*() {
                return my_node->v;
            }

            const V& operator*() const {
                return my_node->v;
            }

            void next() {
                my_node = my_node::next(my_node);
            }

            void prev() {
                my_node = my_node::prev(my_node);
            }

            bool isEnd() {
                return !my_node;
            }
        };

        Iterator iterator() {
            return Iterator(my_node::first(root));
        }
    };
}

#endif //ALGORITHMS_TREE_H
