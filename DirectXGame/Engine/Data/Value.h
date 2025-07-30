#pragma once
#include <string>
#include <memory>
#include <vector>
#include <any>
#include <cassert>

class ValueBase {
public:
	ValueBase(std::string name) : name(name) {};
	virtual ~ValueBase() = default;

	template<typename T>
	T get() {
		std::any buffer = GetValueData();

		//tryの中でエラーが発生したらキャッチに飛ぶやつ
		try {
			return std::any_cast<T>(buffer);
		} catch (const std::bad_any_cast& e) {
			assert(false && "ValueBase::get() failed: Type mismatch");
			return T(); // 型が一致しない場合はデフォルト値を返す
		}

	};

	std::string name;		//変数名
	
protected:
	ValueBase() = default;

private:
	//std::anyはできれば使いたくないので、誰も使えないようにprivateにする
	
	//値を取得する
	virtual std::any GetValueData() = 0;	
};

template<typename T>
class Value : public ValueBase {
public:
	Value(T value, std::string name = "default") : ValueBase(name), value(value) {};
	virtual ~Value() = default;

	//値を変更する
	void set(const T& newValue) {
		value = newValue;
	};

	T value;				//値

private:
	//親クラスが値を取得する用関数
	std::any GetValueData() override {
		return value;
	}
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
