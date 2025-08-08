#include "Value.h"
#include <cassert>

uint8_t Value<int>::GetTypeID() const {
    return uint8_t(TypeID::Int);
}

uint8_t Value <float> ::GetTypeID() const {
    return uint8_t(TypeID::Float);
}

uint8_t Value<double>::GetTypeID() const {
    return uint8_t(TypeID::Double);
}

uint8_t Value <bool> ::GetTypeID() const {
    return uint8_t(TypeID::Bool);
}

uint8_t Value <std::string> ::GetTypeID() const {
    return uint8_t(TypeID::String);
}

uint8_t Value <Vector2> ::GetTypeID() const {
	return uint8_t(TypeID::Vector2);
}

uint8_t Value <Vector3> ::GetTypeID() const {
    return uint8_t(TypeID::Vector3);
}

uint8_t Value <Vector4> ::GetTypeID() const {
    return uint8_t(TypeID::Vector4);
}

#pragma region Operator

#pragma region 四則演算
template<typename T>
T operator+(const Value<T>& a, const Value<T>& b) {
    return static_cast<T>(a) + static_cast<T>(b);
}

template<typename T>
T operator-(const Value<T>& a, const Value<T>& b) {
    return static_cast<T>(a) - static_cast<T>(b);
}

template<typename T>
T operator*(const Value<T>& a, const Value<T>& b) {
    return static_cast<T>(a) * static_cast<T>(b);
}

template<typename T>
T operator/(const Value<T>& a, const Value<T>& b) {
    return static_cast<T>(a) / static_cast<T>(b);
}
#pragma endregion

#pragma region 代入演算子
template<typename T>
T operator+=(Value<T>& a, const Value<T>& b) {
    a = static_cast<T>(a) + static_cast<T>(b);
    return static_cast<T>(a);
}

template<typename T>
T operator-=(Value<T>& a, const Value<T>& b) {
    a = static_cast<T>(a) - static_cast<T>(b);
    return static_cast<T>(a);
}

template<typename T>
T operator*=(Value<T>& a, const Value<T>& b) {
    a = static_cast<T>(a) * static_cast<T>(b);
    return static_cast<T>(a);
}

template<typename T>
T operator/=(Value<T>& a, const Value<T>& b) {
    a = static_cast<T>(a) / static_cast<T>(b);
    return static_cast<T>(a);
}
#pragma endregion

#pragma region 比較演算子
template<typename T>
bool operator==(const Value<T>& a, const Value<T>& b) {
    return static_cast<T>(a) == static_cast<T>(b);
}

template<typename T>
bool operator!=(const Value<T>& a, const Value<T>& b) {
    return static_cast<T>(a) != static_cast<T>(b);
}

template<typename T>
bool operator<(const Value<T>& a, const Value<T>& b) {
    return static_cast<T>(a) < static_cast<T>(b);
}

template<typename T>
bool operator>(const Value<T>& a, const Value<T>& b) {
    return static_cast<T>(a) > static_cast<T>(b);
}

template<typename T>
bool operator<=(const Value<T>& a, const Value<T>& b) {
    return static_cast<T>(a) <= static_cast<T>(b);
}

template<typename T>
bool operator>=(const Value<T>& a, const Value<T>& b) {
    return static_cast<T>(a) >= static_cast<T>(b);
}

template<typename T>
bool operator!(Value<T>& a) {
    return !static_cast<T>(a);
}

template<typename T>
bool operator&&(const Value<T>& a, const Value<T>& b) {
    return static_cast<T>(a) && static_cast<T>(b);
}

template<typename T>
bool operator||(const Value<T>& a, const Value<T>& b) {
    return static_cast<T>(a) || static_cast<T>(b);
}
#pragma endregion

#pragma region インクリメント・デクリメント演算子
template<typename T>
Value<T> operator++(Value<T>& a) {
    a = static_cast<T>(a) + 1;
    return a;
}

template<typename T>
Value<T> operator--(Value<T>& a) {
    a = static_cast<T>(a) - 1;
    return a;
}
#pragma endregion

#pragma region ビット演算子
template<typename T>
Value<T> operator<<(const Value<T>& a, int shift) {
    return static_cast<T>(a) << shift;
}

template<typename T>
Value<T> operator>>(const Value<T>& a, int shift) {
    return static_cast<T>(a) >> shift;
}
#pragma endregion

#pragma endregion
