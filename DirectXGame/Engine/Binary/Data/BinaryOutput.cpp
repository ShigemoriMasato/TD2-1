#include "BinaryOutput.h"

void BinaryOutput::WriteVBin(std::ostream& out, const ValueBase* value) {
	//型名を出力
	uint8_t typeID = value->GetTypeID();
	out.write(reinterpret_cast<const char*>(&typeID), sizeof(typeID));

	//変数名を出力
	uint8_t nameLength = static_cast<uint8_t>(value->name.size());
	out.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
	out.write(value->name.data(), nameLength);

	//値を出力
	value->Serialize(out);
}
