#include "Function.h"
#include <type_traits>

Function::Function(std::string name, std::vector<ValueBase*> args, std::type_index returnType) :
baseArgs(args),
returnType(returnType) {
	this->name = name;
}

Function::~Function() {
	for (auto arg : baseArgs) {
		delete arg;
	}
}

bool Function::Execute(std::vector<ValueBase*> args) {

	if (args.size() != this->baseArgs.size()) {
		return false;
	}

	for (int i = 0; i < args.size(); ++i) {
		//型が一致しているか
		if (!std::is_same<decltype(*this->baseArgs[i]), decltype(*args[i])>()) {
			return false;
		}
	}

	//実行
	ExecuteFunction(args);

	return true;
}

std::vector<ValueBase*> Function::GetArgs() {
	return baseArgs;
}

std::type_index Function::GetReturnType() {
	return returnType;
}

void* Function::ExecuteFunction(std::vector<ValueBase*> args) {
	return nullptr;
}
