#include "String.h"
#include <iostream>
#pragma warning(disable : 4996)

void String::free() {
    delete[] data;
    data = nullptr;
    length = 0;
}

void String::copy(const char* str) {
    if (!str) str = "";
    length = strlen(str);
    data = new char[length + 1];
    strcpy(data, str);
}

String::String() : data(nullptr), length(0) {
    data = new char[1];
    data[0] = '\0';
}

String::String(const char* str) : data(nullptr), length(0) {
    copy(str);
}

String::String(const String& other) : data(nullptr), length(0) {
    copy(other.data);
}

String::~String() {
    free();
}

size_t String::size() const {
    return length;
}

const char* String::c_str() const {
    return data ? data : "";
}

bool String::empty() const {
    return length == 0;
}

String& String::operator=(const String& other) {
    if (this != &other) {
        free();
        copy(other.data);
    }
    return *this;
}

String& String::operator=(const char* str) {
    free();
    copy(str);
    return *this;
}

bool String::operator==(const String& other) const {
    return strcmp(c_str(), other.c_str()) == 0;
}

bool String::operator!=(const String& other) const {
    return !(*this == other);
}

bool String::operator<(const String& other) const {
    return strcmp(c_str(), other.c_str()) < 0;
}

String String::operator+(const String& other) const {
    String result;
    result.length = length + other.length;
    result.data = new char[result.length + 1];
    strcpy(result.data, c_str());
    strcat(result.data, other.c_str());
    return result;
}

String& String::operator+=(const String& other) {
    *this = *this + other;
    return *this;
}

String& String::operator+=(const char* str) {
    if (!str) return *this;
    size_t str_len = strlen(str);
    char* new_data = new char[length + str_len + 1];
    strcpy(new_data, c_str());
    strcat(new_data, str);
    free();
    data = new_data;
    length += str_len;
    return *this;
}

char& String::operator[](size_t index) {
    if (index >= length) throw std::out_of_range("String index out of range");
    return data[index];
}

const char& String::operator[](size_t index) const {
    if (index >= length) throw std::out_of_range("String index out of range");
    return data[index];
}

void String::clear() {
    free();
    data = new char[1];
    data[0] = '\0';
}

String String::pad_left(size_t total_length, char pad_char) const {
    if (length >= total_length) return *this;

    size_t pad_size = total_length - length;
    char* new_data = new char[total_length + 1];

    for (size_t i = 0; i < pad_size; ++i) {
        new_data[i] = pad_char;
    }

    strcpy(new_data + pad_size, data);

    String result(new_data);
    delete[] new_data;
    return result;
}

String String::trim() const {
    if (!data || length == 0) return String();

    size_t start = 0;
    size_t end = length - 1;

    while (start <= end && is_whitespace(data[start])) start++;
    while (end >= start && is_whitespace(data[end])) end--;

    if (start > end) return String();

    size_t new_length = end - start + 1;
    char* new_data = new char[new_length + 1];
    strncpy(new_data, data + start, new_length);
    new_data[new_length] = '\0';

    return String(new_data);
}

Vector<String> String::split(char delimiter) const {
    Vector<String> result;
    if (!data || length == 0) return result;

    size_t start = 0;
    for (size_t i = 0; i <= length; ++i) {
        if (data[i] == delimiter || data[i] == '\0') {
            if (i > start) {
                size_t substr_len = i - start;
                char* substr = new char[substr_len + 1];
                strncpy(substr, data + start, substr_len);
                substr[substr_len] = '\0';
                result.push_back(String(substr));
                delete[] substr;
            }
            start = i + 1;
        }
    }

    return result;
}

size_t String::count(char ch) const {
    size_t counter = 0;
    for (size_t i = 0; i < length; ++i) {
        if (data[i] == ch) counter++;
    }
    return counter;
}

bool String::is_whitespace(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool String::contains(const char* substr) const
{
    if (!data || !substr) return false;
    const char* ptr = strstr(data, substr);
    return ptr != nullptr;
}

String String::substr(size_t start, size_t len) const
{
    if (start >= length) return String();
    size_t actual_len = (len == (size_t)-1) ? length - start : std::min(len, length - start);
    char* buffer = new char[actual_len + 1];
    strncpy(buffer, data + start, actual_len);
    buffer[actual_len] = '\0';
    String result(buffer);
    delete[] buffer;
    return result;
}

// Numeric Conversions
String String::from_int(int value) {
    if (value == 0) return String("0");

    bool isNegative = value < 0;
    if (isNegative) value = -value;

    int digits = 0;
    for (int temp = value; temp != 0; temp /= 10) digits++;

    char* buffer = new char[digits + (isNegative ? 1 : 0) + 1];
    int i = digits + (isNegative ? 1 : 0);
    buffer[i--] = '\0';

    do {
        buffer[i--] = '0' + (value % 10);
        value /= 10;
    } while (value != 0);

    if (isNegative) buffer[i] = '-';

    String result(buffer);
    delete[] buffer;
    return result;
}

String String::from_float(float value, int precision) {
    if (precision < 0) precision = 2;

    String intPart = from_int(static_cast<int>(value));
    String result = intPart + ".";

    float fractional = value - static_cast<int>(value);
    if (fractional < 0) fractional = -fractional;

    for (int i = 0; i < precision; i++) {
        fractional *= 10;
        int digit = static_cast<int>(fractional) % 10;
        result += from_int(digit);
    }

    return result;
}

int String::to_int() const {
    if (empty()) throw std::invalid_argument("Empty string");

    int result = 0;
    bool negative = false;
    size_t i = 0;

    if (data[0] == '-') {
        negative = true;
        i++;
    }
    else if (data[0] == '+') {
        i++;
    }

    for (; i < length; i++) {
        if (data[i] < '0' || data[i] > '9')
            throw std::invalid_argument("Invalid character in number");

        if (result > (CUSTOM_INT_MAX / 10) || (result == CUSTOM_INT_MAX / 10 && (data[i] - '0') > CUSTOM_INT_MAX % 10))
            throw std::overflow_error("Integer overflow");

        result = result * 10 + (data[i] - '0');
    }

    return negative ? -result : result;
}

float String::to_float() const {
    if (empty()) throw std::invalid_argument("Empty string");

    float result = 0.0f;
    float fraction = 0.1f;
    bool negative = false;
    bool decimalFound = false;
    size_t i = 0;

    if (data[0] == '-') {
        negative = true;
        i++;
    }
    else if (data[0] == '+') {
        i++;
    }

    for (; i < length; i++) {
        if (data[i] == '.') {
            if (decimalFound) throw std::invalid_argument("Multiple decimal points");
            decimalFound = true;
            continue;
        }

        if (data[i] < '0' || data[i] > '9')
            throw std::invalid_argument("Invalid character in number");

        int digit = data[i] - '0';

        if (decimalFound) {
            result += digit * fraction;
            fraction *= 0.1f;
        }
        else {
            result = result * 10 + digit;
        }
    }

    return negative ? -result : result;
}

void String::write_to_file(std::ofstream& file) const {
    if (!file.is_open()) throw std::runtime_error("File not open");
    file << c_str();
}

String String::read_from_file(std::ifstream& file) {
    if (!file.is_open()) throw std::runtime_error("File not open");

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    char* buffer = new char[size + 1];
    file.read(buffer, size);
    buffer[size] = '\0';

    String result(buffer);
    delete[] buffer;
    return result;
}

String String::read_line(std::istream& is) {
    char buffer[1024];
    if (is.getline(buffer, sizeof(buffer))) {
        return String(buffer);
    }
    return String();
}

std::istream& operator>>(std::istream& is, String& str) {
    char buffer[1024];
    if (is >> buffer) {
        str = buffer;
    }
    return is;
}

std::ostream& operator<<(std::ostream& os, const String& str) {
    return os << str.c_str();
}
