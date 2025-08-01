#pragma once
#include <Data/Value.h>
#include <typeindex>

struct Function {
public:

	Function(std::string name, std::vector<ValueBase*> args, std::type_index returnType);
	~Function();

	bool Execute(std::vector<ValueBase*> args);

	std::vector<ValueBase*> GetArgs();
	std::type_index GetReturnType();

	std::string name;

private:

	std::type_index returnType; // 戻り値の型
	const std::vector<ValueBase*> baseArgs;

	/// <summary>
	/// 実際に実行する関数
	/// </summary>
	void* ExecuteFunction(std::vector<ValueBase*> args);

};
