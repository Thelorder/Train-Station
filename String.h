#pragma once
#include <fstream>
#include <cstring>
#include <stdexcept>
#include "Vector.hpp"

#define CUSTOM_INT_MAX 2147483647   
#define CUSTOM_INT_MIN (-2147483647 - 1) 

class String {
private:
	char* data;
	size_t length;
	
	void free();
	void copy(const char* str);
public:
	String();
	String(const char* str);
	String(const String& other);
	~String();

	size_t size() const;
	const char* c_str() const;
	bool empty()const;

	String& operator=(const String& other);
	String& operator=(const char* str);

	bool operator==(const String& other) const;
	bool operator!=(const String& other) const;
	bool operator<(const String& other) const;
	bool operator>(const String& other) const {
		return strcmp(c_str(), other.c_str()) > 0;
	}

	String operator+(const String& other) const;
	String& operator+=(const String& other);
	String& operator+=(const char* str);

	char& operator[](size_t index);
	const char& operator[](size_t index) const;

	void clear();

	String pad_left(size_t total_length, char pad_char) const;
	String trim() const;
	Vector<String> split(char delimiter) const;
	size_t count(char ch) const;
	static bool is_whitespace(char c);
	bool contains(const char* substr) const;
	String substr(size_t start, size_t len = -1) const;

	static String from_int(int value);
	static String from_float(float value, int precision = 2);
	int to_int() const;
	float to_float() const;
	
	void write_to_file(std::ofstream& file) const;
	static String read_from_file(std::ifstream& file);
	static String read_line(std::istream& is);

	friend std::istream& operator>>(std::istream& is, String& str);
	friend std::ostream& operator<<(std::ostream& os, const String& str);
};
