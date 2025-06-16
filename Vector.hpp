#pragma once

#include <stdexcept>

template<typename T>
class Vector
{
private:

	T* data;
	size_t capacity;
	size_t size;

	void resize(size_t new_capacity);
public:
	Vector();
	Vector(size_t initial_capacity);
	Vector(const Vector& other);
    Vector& operator=(const Vector& other);
	~Vector();

	T& operator[](size_t index);
	const T& operator[](size_t index) const;
	T& back();
	const T& back() const;

	void push_back(const T& value);
	void pop_back();
	void remove(size_t index);
	void clear();

	size_t get_size()const;
	size_t get_capacity() const;
	bool empty() const;
};

template <typename T>
Vector<T>::Vector() : data(nullptr), capacity(0), size(0) {}

template <typename T>
Vector<T>::Vector(size_t initial_capacity) :
    data(new T[initial_capacity]), capacity(initial_capacity), size(0) {}

template <typename T>
Vector<T>::Vector(const Vector& other) :
    data(new T[other.capacity]), capacity(other.capacity), size(other.size) {
    for (size_t i = 0; i < size; ++i) {
        data[i] = other.data[i];
    }
}

template<typename T>
Vector<T>& Vector<T>::operator=(const Vector& other)
{
    if (this != &other) {
        Vector temp(other);

        std::swap(data, temp.data);
        std::swap(size, temp.size);
        std::swap(capacity, temp.capacity);
    }
    return *this;
}

template <typename T>
Vector<T>::~Vector() {
    delete[] data;
}

template <typename T>
void Vector<T>::resize(size_t new_capacity) {
    if (new_capacity < size) {
        throw std::invalid_argument("New capacity cannot be less than current size");
    }

    T* new_data = new T[new_capacity]; 
    for (size_t i = 0; i < size; ++i) {
        new_data[i] = data[i];
    }

    delete[] data;
    data = new_data;
    capacity = new_capacity;
}

template <typename T>
void Vector<T>::push_back(const T& value) {
    if (size >= capacity) {
        resize(capacity == 0 ? 1 : capacity * 2);
    }
    data[size++] = value;
}

template <typename T>
void Vector<T>::pop_back() {
    if (size == 0) throw std::out_of_range("Vector is empty");
    --size;
}

template <typename T>
void Vector<T>::remove(size_t index) {
    if (index >= size) throw std::out_of_range("Index out of range");
    for (size_t i = index; i < size - 1; ++i) {
        data[i] = data[i + 1];
    }
    --size;
}

template <typename T>
void Vector<T>::clear() {
    size = 0;
}

template <typename T>
T& Vector<T>::operator[](size_t index) {
    if (index >= size) throw std::out_of_range("Index out of range");
    return data[index];
}

template <typename T>
const T& Vector<T>::operator[](size_t index) const {
    if (index >= size) throw std::out_of_range("Index out of range");
    return data[index];
}

template <typename T>
T& Vector<T>::back() {
    if (size == 0) throw std::out_of_range("Vector is empty");
    return data[size - 1];
}

template <typename T>
const T& Vector<T>::back() const {
    if (size == 0) throw std::out_of_range("Vector is empty");
    return data[size - 1];
}

template <typename T>
size_t Vector<T>::get_size() const {
    return size;
}

template <typename T>
size_t Vector<T>::get_capacity() const {
    return capacity;
}

template <typename T>
bool Vector<T>::empty() const {
    return size == 0;
}