#pragma once
#include <string>
#include <memory>
#include <vector>
#include <any>
#include <cassert>
#include <istream>
#include <ostream>
#include <Data/Transform.h>

enum class TypeID : uint8_t {
	Int = 0x01,
	Float = 0x02,
	Bool = 0x03,
	String = 0x04,
	Double = 0x05,

	Vector2 = 0x10,
	Vector3 = 0x11,
	Vector4 = 0x12,

	Custom = 0x80

};

class ValueBase {
public:
	ValueBase(std::string name) : name(name) {}
	virtual ~ValueBase() = default;

	template<typename T>
	T get() {
		std::any buffer = GetValueData();

		//tryの中でエラーが発生したらキャッチに飛ぶやつ
		try {
			return std::any_cast<T>(buffer);
		} catch (const std::bad_any_cast& e) {
			assert(false && "ValueBase::get() failed: Type mismatch");
			e;
			return T(); // 型が一致しない場合はデフォルト値を返す
		}

	};

	virtual uint8_t GetTypeID() const = 0;

	virtual void Serialize(std::ostream& out) const = 0;
	virtual void Deserialize(std::istream& in) = 0;

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

	uint8_t GetTypeID() const override;

	void Serialize(std::ostream& out) const override {
		out.write(reinterpret_cast<const char*>(&value), sizeof(T));
	}

	void Deserialize(std::istream& in) override {
		in.read(reinterpret_cast<char*>(&value), sizeof(T));
	}

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
