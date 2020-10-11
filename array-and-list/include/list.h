//
// Created by ko on 12.09.2020.
//

#ifndef ARRAY_AND_LIST_LIST_H
#define ARRAY_AND_LIST_LIST_H

#include "array.h"

namespace myalg {
    template<typename T>
    class List final {
        class Node {
        public:
            Array<T> data;
            Node *prev = nullptr, *next = nullptr;

            static const int CHUNK_SIZE = 4;

            Node(Node *prev, Node *next) : prev(prev), next(next), data(CHUNK_SIZE) {
                (prev != nullptr ? prev->next : this->prev) = this;
                (next != nullptr ? next->prev : this->next) = this;
            }

            ~Node() {
                prev->next = isTail() ? prev->next : next;
                next->prev = isHead() ? next->prev : prev;
            }

            bool isFull() {
                return data.size() >= CHUNK_SIZE;
            }

            bool isEmpty() {
                return data.size() == 0;
            }

            bool isTail() {
                return this == next;
            }

            bool isHead() {
                return this == prev;
            }
        };

        Node *_head, *_tail = nullptr;
        int _size = 0;

    public:
        List() {
            _head = _tail = new Node(nullptr, nullptr);
            _size = 0;
        }

        List(const List& node) = delete;

        ~List() {
            for (Node* node = _head; !node->isTail();) {
                Node* currentNode = node;
                node = node->next;
                delete currentNode;
            }
            delete _tail;
        }

        void insertHead(const T& value) {
            if (_head->isFull()) {
                _head = new Node(nullptr, _head);
            }
            _head->data.insert(0, value);
            _size++;
        }

        void insertTail(const T& value) {
            if (_tail->isFull()) {
                _tail = new Node(_tail, nullptr);
            }
            _tail->data.insert(value);
            _size++;
        }

        int size() {
            return _size;
        }

        void removeHead() {
            _head->data.remove(0);
            if (_head->isEmpty() && !_head->isTail()) {
                _head = _head->next;
                delete _head->prev;
                _head->prev = _head;
            }
            _size--;
        }

        void removeTail() {
            _tail->data.remove(_tail->data.size() - 1);
            if (_tail->isEmpty() && !_tail->isHead()) {
                _tail = _tail->prev;
                delete _tail->next;
                _tail->next = _tail;
            }
            _size--;
        }

        const T& head() {
            return _head->data[0];
        }

        const T& tail() {
            return _tail->data[_tail->data.last()];
        }

        class Iterator {
            List<T> *list;
            Node *_node;
            int _index;

            explicit Iterator(List<T> *list) : list(list), _node(list->_head), _index(0) {}
            friend class List;
        public:
            const T& get() const {
                return _node->data[_index];
            }

            void set(const T &value) {
                _node->data[_index] = value;
            }

            void insert(const T &value) {
                if (_node->isFull()) {
                    if (_node->isTail()) {
                        list->_tail = new Node(_node, nullptr);
                    } else {
                        new Node(_node, _node->next);
                    }
                    for (int i = _index; i < Node::CHUNK_SIZE; i++) {
                        _node->next->data.insert(std::move(_node->data[i]));
                        _node->data.remove(_node->data.last());
                    }
                }
                _node->data.insert(_index, value);
                list->_size++;
            }

            void remove() {
                _node->data.remove(_index);
                list->_size--;
                if (_node->isEmpty() || _index >= _node->data.size()) {
                    Node *prev = _node;
                    _node = _node->next;
                    _index = 0;
                    if (prev->isEmpty() && !prev->isTail()) {
                        if (prev->isHead()) list->_head = _node;
                        delete prev;
                    }
                }
            }

            void next() {
                if (_index < _node->data.last()) {
                    _index++;
                } else {
                    if (_node->isTail()) {
                        _index = _node->data.size();
                    } else {
                        _node = _node->next;
                        _index = 0;
                    }
                }
            }

            void prev() {
                if (_index > 0) {
                    _index--;
                } else {
                    if (_node->isHead()) {
                        _index = -1;
                    } else {
                        _node = _node->prev;
                        _index = _node->data.last();
                    }
                }
            }

            bool hasNext() const {
                return _index < _node->data.last() || !_node->isTail();
            }

            bool hasPrev() const {
                return _index > 0 || !_node->isHead();
            }

            bool isOutOfRange() const {
                return _index == _node->data.size() && _node->isTail() || _index < 0 && _node ->isHead();
            }
        };

        Iterator iterator() {
            return Iterator(this);
        }
    };
}

#endif //ARRAY_AND_LIST_LIST_H
