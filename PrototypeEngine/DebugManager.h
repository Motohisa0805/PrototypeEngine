#pragma once
#include "Typedefs.h"
#include "Math.h"
#include <iostream>
#include <cstdlib>

/*
* ===エンジン内部処理/Engine internal processing===
*/

//エラー、警告、デバッグ用のログを出力する関数をまとめている
//エンジン内部処理のため、ヘッダーファイルを分けていません
namespace Debug
{
    /// <summary> 
    /// エラー発生時のログを出力し強制終了させる関数 
    /// </summary> 
    /// <param name="description"> 
    /// エラー説明文字列 
    /// </param> 
    // 汎用的なErrorログ（引数があるバージョン）
    template <typename... Args>
    inline void ErrorLog(const char* format, Args... args)
    {
        std::string finalFormat = std::string("Error : ") + format;
        fprintf(stderr, finalFormat.c_str(), args...);
    }

    // 引数がないバージョン
    inline void ErrorLog(const char* message)
    {
        std::string finalMessage = std::string("Error : ") + message;
        fprintf(stderr, "%s", finalMessage.c_str());
    }

    /// <summary> 
    /// 警告発生時のログを出力する関数 
    /// </summary> 
    /// <param name="description"> 
    /// 警告説明文字列 
    /// </param> 
    // 汎用的なWarningログ（引数があるバージョン）
    template <typename... Args>
    inline void WarningLog(const char* format, Args... args)
    {
        std::string finalFormat = std::string("Warning : ") + format;
        fprintf(stderr, finalFormat.c_str(), args...);
    }

    // 引数がないバージョン
    inline void WarningLog(const char* message)
    {
        std::string finalMessage = std::string("Warning : ") + message;
        fprintf(stderr, "%s", finalMessage.c_str());
    }


    inline void Log(const char* format)
    {
        fprintf(stderr, "%s", format);
        fprintf(stderr, "\n");
    }

    template <typename... Args>
    inline void Log(const char* format, Args... args)
    {
        fprintf(stderr, format, args...);
        fprintf(stderr, "\n");
    }
};