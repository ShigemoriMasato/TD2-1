#include "BinaryInput.h"
#include <Data/Transform.h>
#include <d3d12.h>
#include <Core/MyPSO.h>

namespace {
	template<typename T>
	std::shared_ptr<Value<T>> ValueDeserialize(std::string name, std::istream& in) {
		auto val = std::make_shared<Value<T>>(T(), name);
		val->Deserialize(in);
		return val;
	}
}

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
		return ValueDeserialize<int>(name, in);
	}
	case TypeID::Float:
	{
		return ValueDeserialize<float>(name, in);
	}
	case TypeID::String:
	{
		return ValueDeserialize<std::string>(name, in);
	}
	case TypeID::Bool:
	{
		return ValueDeserialize<bool>(name, in);
	}
	case TypeID::Vector2:
	{
		return ValueDeserialize<Vector2>(name, in);
	}
	case TypeID::Vector3:
	{
		return ValueDeserialize<Vector3>(name, in);
	}
	case TypeID::Vector4:
	{
		return ValueDeserialize<Vector4>(name, in);
	}
	case TypeID::PSODesc:
	{
		return ValueDeserialize<PSODescData>(name, in);
	}
	// 他の型も同様に追加
	default:
		assert(false && "Unknown TypeID in ReadVBin");
		return nullptr;
	}
}
