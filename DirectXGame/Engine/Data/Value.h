#pragma once
#include <string>
#include <memory>
#include <vector>
#include "Vector.h"

template<typename T>
class Value {
public:
	Value(T value, std::string name = "default") : name(name), value(value) {};
	virtual ~Value() = default;

	operator T() const {
		return value;
	}
	void operator=(const T& newValue) {
		value = newValue;
	}

	std::string name;		//変数名
	T value;				//値
};

template<typename T>
T operator+(const Value<T>& a, const Value<T>& b);
template<typename T>
T operator-(const Value<T>& a, const Value<T>& b);
template<typename T>
T operator*(const Value<T>& a, const Value<T>& b);
template<typename T>
T operator/(const Value<T>& a, const Value<T>& b);
template<typename T>
T operator+=(Value<T>& a, const Value<T>& b);
template<typename T>
T operator-=(Value<T>& a, const Value<T>& b);
template<typename T>
T operator*=(Value<T>& a, const Value<T>& b);
template<typename T>
T operator/=(Value<T>& a, const Value<T>& b);
template<typename T>
bool operator==(const Value<T>& a, const Value<T>& b);
template<typename T>
bool operator!=(const Value<T>& a, const Value<T>& b);
template<typename T>
bool operator<(const Value<T>& a, const Value<T>& b);
template<typename T>
bool operator>(const Value<T>& a, const Value<T>& b);
template<typename T>
bool operator<=(const Value<T>& a, const Value<T>& b);
template<typename T>
bool operator>=(const Value<T>& a, const Value<T>& b);
template<typename T>
Value<T> operator++(Value<T>& a);
template<typename T>
Value<T> operator--(Value<T>& a);
template<typename T>
bool operator!(Value<T>& a);
template<typename T>
bool operator&&(const Value<T>& a, const Value<T>& b);
template<typename T>
bool operator||(const Value<T>& a, const Value<T>& b);
template<typename T>
Value<T> operator<<(const Value<T>& a, int shift);
template<typename T>
Value<T> operator>>(const Value<T>& a, int shift);
