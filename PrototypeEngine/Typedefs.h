#pragma once
#include "StandardLibrary.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//文字列の簡易記述命令
typedef std::string string;

typedef char c_String;

typedef c_String* cp_String;

typedef const c_String* ccp_String;

typedef std::u8string u8string;

typedef std::stringstream stringstream;

typedef std::stringbuf stringbuf;
//vectorの簡易記述命令
template <typename T>
using vector = std::vector<T>;

//nlohmann jsonの簡易記述命令
using json = nlohmann::json;
