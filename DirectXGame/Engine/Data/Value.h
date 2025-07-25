#pragma once
#include <string>
#include <memory>
#include <vector>
#include "Vector.h"

class Value {
public:
	Value(std::string naming) : name(naming) {};
	virtual ~Value() = default;
	std::string name;
private:
};

class IntValue final : public Value {
public:
	IntValue(std::string name, int value) : Value(name), value(value) {}
	int value = 0;
};

class FloatValue final : public Value {
public:
	FloatValue(std::string name, float value) : Value(name), value(value) {}
	float value = 0.0f;
};

class Vec2Value final : public Value {
public:
	Vec2Value(std::string name, Vector2 value) : Value(name), value(value) {}
	Vector2 value{};
};

class Vec3Value final : public Value {
public:
	Vec3Value(std::string name, Vector3 value) : Value(name), value(value) {}
	Vector3 value{};
};

class Vec4Value final : public Value {
public:
	Vec4Value(std::string name, Vector4 value) : Value(name), value(value) {}
	Vector4 value{};
};

std::shared_ptr<Value> MakeValue(std::string argName, std::vector<float> values);
