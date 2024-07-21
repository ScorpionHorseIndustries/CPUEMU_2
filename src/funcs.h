#ifndef __FUNCS_H_
#define __FUNCS_H_
#include "common.h"


namespace sh {


    using string = std::string;
    std::string ToUpper(const std::string str);
    std::string GetKey(const std::string str);
    float MapFloat(float n, float start1, float stop1, float start2, float stop2);
    void LTrim(std::string& s);
    void RTrim(std::string& s);
    void Trim(std::string& s);

    u16 StoreF16AsU16(f16 f);
    f16 GetF16FromU16Bytes(u16 n);
    u16 StoreIntAsU16(i16 i);
    i16 GetInt16FromU16Bytes(u16 u);
    bool BothAreSpaces(char lhs, char rhs);
    void Squeeze(string& s);
    void Remove(string &s, char remove_me);
    void Replace(string& s, char replace_me, char with_me);
    std::vector<std::string> SplitString(const std::string& str, char delim);

    bool CharInList(char c, const std::string list);
    bool IsIdentStart(char c);
    bool IsIdentMiddle(char c);
    bool IsNumericStart(char c);
    bool IsNumericMiddle(char c);

    bool CmdOptionExists(char** begin, char** end, const std::string& option);
    char* GetCmdOption(char ** begin, char ** end, const std::string& option);
    bool AllInList(const std::string& str, const std::string list);
}   

#endif
