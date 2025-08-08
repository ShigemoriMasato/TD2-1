#include "BinaryInput.h"
#include <Data/Transform.h>

std::shared_ptr<ValueBase> BinaryInput::ReadVBin(std::istream& in) {
	// 1. TypeID
	uint8_t typeID;
	in.read(reinterpret_cast<char*>(&typeID), sizeof(typeID));

	// 2. Name長 + Name文字列
	uint8_t nameLength;
	in.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));

	std::string name(nameLength, '\0');
	in.read(&name[0], nameLength);

	// 3. 型ごとのValue生成
	switch (static_cast<TypeID>(typeID)) {
	case TypeID::Int:
	{
		auto val = std::make_shared<Value<int>>(0, name);
		val->Deserialize(in);
		return val;
	}
	case TypeID::Float:
	{
		auto val = std::make_shared<Value<float>>(0.0f, name);
		val->Deserialize(in);
		return val;
	}
	case TypeID::String:
	{
		auto val = std::make_shared<Value<std::string>>("", name);
		val->Deserialize(in);
		return val;
	}
	case TypeID::Bool:
	{
		auto val = std::make_shared<Value<bool>>(false, name);
		val->Deserialize(in);
		return val;
	}
	case TypeID::Vector2:
	{
		auto val = std::make_shared<Value<Vector2>>(Vector2(0.0f, 0.0f), name);
		val->Deserialize(in);
		return val;
	}
	case TypeID::Vector3:
	{
		auto val = std::make_shared<Value<Vector3>>(Vector3(0.0f, 0.0f, 0.0f), name);
		val->Deserialize(in);
		return val;
	}
	case TypeID::Vector4:
	{
		auto val = std::make_shared<Value<Vector4>>(Vector4(0.0f, 0.0f, 0.0f, 0.0f), name);
		val->Deserialize(in);
		return val;
	}
	// 他の型も同様に追加
	default:
		assert(false && "Unknown TypeID in ReadVBin");
		return nullptr;
	}
}
