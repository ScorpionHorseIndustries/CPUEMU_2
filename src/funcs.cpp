#include "common.h"


namespace sh {

    std::string GetKey(const std::string str) {
        std::string k = ToUpper(str);
        k.erase(std::remove(k.begin(), k.end(), ':'), k.end());
        k.erase(std::remove(k.begin(), k.end(), '#'), k.end());
        k.erase(std::remove(k.begin(), k.end(), '@'), k.end());

        return k;


    }

    float MapFloat(float n, float start1, float stop1, float start2, float stop2) {

        return (n - start1) / (stop1 - start1) * (stop2 - start2) + start2;
            
    }

    std::string ToUpper(const std::string str) {
        std::string k = str.substr(0);
        
        std::transform(str.begin(), str.end(), k.begin(), ::toupper);

        return k;
    }

    void Trim(std::string &s) {
        LTrim(s);
        RTrim(s);
        
    }
    void Remove(std::string& s, char remove_me) {   
        s.erase(std::remove(s.begin(), s.end(), remove_me), s.end());
    }
    
    void Replace(std::string& s, char replace_me, char with_me) {
        std::replace(s.begin(), s.end(), replace_me, with_me);
    }
    bool BothAreSpaces(char lhs, char rhs) {
        return (lhs == rhs) && (lhs == ' ');
    }
    void Squeeze(std::string& s) {
        std::string::iterator new_end = std::unique(s.begin(), s.end(), BothAreSpaces);
        s.erase(new_end, s.end());
    }

    void LTrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
    }

    // trim from end (in place)
    void RTrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    u16 StoreF16AsU16(f16 f) {
        u16 rt = *(u16*)&f;
        return rt;
    }
    f16 GetF16FromU16Bytes(u16 n) {
        f16 f = *(f16*)&n;
        return f;
    }
    u16 StoreIntAsU16(i16 i) {
        u16 rt = *(u16*)&i;
        return rt;
    }
    i16 GetInt16FromU16Bytes(u16 u) {
        i16 rt = *(i16*)&u;
        return rt;
    }

    std::vector<std::string> SplitString(const std::string& str, char delim) {
        std::vector<std::string> items;
        std::string item;
        std::istringstream inputStream(str);
        while (std::getline(inputStream, item, delim)) {

            items.push_back(item);
        }
        return items;

     }

    bool CharInList(char c, const std::string list) {
        return (list.find_first_of(c) != std::string::npos);
    }

    bool AllInList(const std::string& str, const std::string list) {
        for (char c : str) {
            if (!CharInList(c,list)) {
                return false;
            }
        }

        return true;
    }


    bool IsIdentStart(char c) {
        return CharInList(c, IDENT_START);
    }
    bool IsIdentMiddle(char c) {
        return CharInList(c,IDENT_MIDDLE );
    }
    bool IsNumericStart(char c) {
        return CharInList(c, NUMERIC_START);
    }
    bool IsNumericMiddle(char c) {
        return CharInList(c, NUMERIC_MIDDLE);
    }


    char* GetCmdOption(char ** begin, char ** end, const std::string& option)
    {
        char ** itr = std::find(begin, end, option);
        //we found the value and there is a following value
        if (itr != end && itr+1 != end) {
            //get the next token
            itr += 1;
            return *itr;
        }
        return 0;
    }

    bool CmdOptionExists(char** begin, char** end, const std::string& option)
    {
        return std::find(begin, end, option) != end;
    }

}