//
// Created by ko on 12.09.2020.
//

#ifndef ARRAY_AND_LIST_ARRAY_H
#define ARRAY_AND_LIST_ARRAY_H

namespace myalg {
    template<typename T>
    class Array final {
        T* _data = nullptr;
        int _capacity = 0;
        int _size = 0;

    private:
        void create_new_array(int capacity) {
            _data = new T[capacity];
            this->_capacity = capacity;
        }

        void size_check() {
            if (_size >= _capacity) {
                T* old_data = _data;
                int old_size = _capacity;
                create_new_array(_capacity * 2);
                std::move(old_data, old_data + old_size, _data);
                delete[] old_data;
            }
        }

    public:
        explicit Array(int capacity) {
            create_new_array(capacity);
        }

        Array() {
            create_new_array(16);
        }

        Array(const Array<T> &array) {
            create_new_array(array._capacity);
            _size = array._size;
            std::copy(array._data, array._data + _size, _data);
        }

        ~Array() {
            _size = 0;
            _capacity = 0;
            delete _data;
        }

        Array<T>& operator=(Array<T> array) {
            std::swap(_data, array._data);
            std::swap(_size, array._size);
            std::swap(_capacity, array._capacity);
        }

        void insert(int index, const T &value) {
            _size = std::max(_size + 1, index + 1);
            size_check();
            for (int i = _size - 2; i >= index; i--) {
                _data[i + 1] = std::move(_data[i]);
            }
            _data[index] = value;
        }

        void insert(const T &value) {
            insert(_size, value);
        }

        void remove(int index) {
            _size--;
            for (int i = index; i < _size; i++) {
                _data[i] = std::move(_data[i + 1]);
            }
        }

        T& operator[](int i) {
            return _data[i];
        }

        const T& operator[](int i) const {
            return _data[i];
        }

        int size() {
            return _size;
        }

        int last() {
            return _size - 1;
        }

        class Iterator {
            friend Array;

            int _index = 0;
            Array<T> &_array;

            explicit Iterator(Array<T> &array) : _array(array) {}

        public:

            const T& get() const {
                return _array[_index];
            }

            void set(const T &value) {
                _array[_index] = value;
            }

            void insert(const T &value) {
                _array.insert(_index, value);
            }

            void remove() {
                _array.remove(_index);
            }

            void next() {
                _index++;
            }

            void prev() {
                _index--;
            }

            void toIndex(int index) {
                _index = index;
            }

            bool hasNext() const {
                return _index < _array.last();
            }

            bool hasPrev() const {
                return _index > 0;
            }
        };

        Iterator iterator() {
            return std::move(Iterator(*this));
        }

        const Iterator iterator() const {
            return std::move(Iterator(*this));
        }
    };
}

#endif //ARRAY_AND_LIST_ARRAY_H
