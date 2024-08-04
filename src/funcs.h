#ifndef __FUNCS_H_
#define __FUNCS_H_
#include "common.h"


namespace sh {

    class Funcs {
        public:
        static inline int                       CmdArgumentCount;
        static inline std::vector<std::string>  CmdArguments;

        static std::string ToUpper(const std::string str);
        static std::string GetKey(const std::string str);
        static float MapFloat(float n, float start1, float stop1, float start2, float stop2);
        static void LTrim(std::string& s);
        static void RTrim(std::string& s);
        static void Trim(std::string& s);

        static u16 StoreF16AsU16(f16 f);
        static f16 GetF16FromU16Bytes(u16 n);
        static u16 StoreIntAsU16(i16 i);
        static i16 GetInt16FromU16Bytes(u16 u);
        static bool BothAreSpaces(char lhs, char rhs);
        static void Squeeze(std::string& s);
        static void Remove(std::string &s, char remove_me);
        static void Replace(std::string& s, char replace_me, char with_me);
        static std::vector<std::string> SplitString(const std::string& str, char delim);
        static void MakeSafeFileName(std::string& s);

        static bool CharInList(char c, const std::string list);
        static bool IsIdentStart(char c);
        static bool IsIdentMiddle(char c);
        static bool IsNumericStart(char c);
        static bool IsNumericMiddle(char c);

        static bool CmdOptionExists(const std::string& option0, const std::string& option1);
        static bool CmdOptionExists(const std::string& option);
        // bool CmdOptionExists(char** begin, char** end, const std::string& option);
        static std::string GetCmdOption(const std::string& option);
        static std::string GetCmdOption(const std::string& option0, const std::string& option1);
        // char* GetCmdOption(char ** begin, char ** end, const std::string& option);
        static bool AllInList(const std::string& str, const std::string list);
        static sf::Color GetColour(u16 c);
        static int GetInt(std::string& str);
        static void Message(std::string msg);
    };
}   

#endif
