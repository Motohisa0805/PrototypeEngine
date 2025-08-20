#pragma once
#include "Typedefs.h"
#include "Math.h"
#include "StandardLibrary.h"


//AudioSystemに使うbankのパスを管理するクラス
//パスはcpp側に
// Ctrl + F 「AudioBankProperty.cpp」
class AudioBankProperty
{
private:
	static string mMasterBankString;

	static string mMasterBank;
public:
	static string GetMasterBankString() { return mMasterBankString; }

	static string GetMasterBank() { return mMasterBank; }
};

