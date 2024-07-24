#include "common.h"


namespace sh {

    std::string Funcs::GetKey(const std::string str) {
        std::string k = ToUpper(str);
        k.erase(std::remove(k.begin(), k.end(), ':'), k.end());
        k.erase(std::remove(k.begin(), k.end(), '#'), k.end());
        k.erase(std::remove(k.begin(), k.end(), '@'), k.end());

        return k;


    }

    float Funcs::MapFloat(float n, float start1, float stop1, float start2, float stop2) {

        return (n - start1) / (stop1 - start1) * (stop2 - start2) + start2;
            
    }

    std::string Funcs::ToUpper(const std::string str) {
        std::string k = str.substr(0);
        
        std::transform(str.begin(), str.end(), k.begin(), ::toupper);

        return k;
    }

    void Funcs::Trim(std::string &s) {
        LTrim(s);
        RTrim(s);
        
    }
    void Funcs::Remove(std::string& s, char remove_me) {   
        s.erase(std::remove(s.begin(), s.end(), remove_me), s.end());
    }
    
    void Funcs::Replace(std::string& s, char replace_me, char with_me) {
        std::replace(s.begin(), s.end(), replace_me, with_me);
    }
    bool Funcs::BothAreSpaces(char lhs, char rhs) {
        return (lhs == rhs) && (lhs == ' ');
    }
    void Funcs::Squeeze(std::string& s) {
        std::string::iterator new_end = std::unique(s.begin(), s.end(), BothAreSpaces);
        s.erase(new_end, s.end());
    }

    void Funcs::LTrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
    }

    // trim from end (in place)
    void Funcs::RTrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    u16 Funcs::StoreF16AsU16(f16 f) {
        u16 rt = *(u16*)&f;
        return rt;
    }
    f16 Funcs::GetF16FromU16Bytes(u16 n) {
        f16 f = *(f16*)&n;
        return f;
    }
    u16 Funcs::StoreIntAsU16(i16 i) {
        u16 rt = *(u16*)&i;
        return rt;
    }
    i16 Funcs::GetInt16FromU16Bytes(u16 u) {
        i16 rt = *(i16*)&u;
        return rt;
    }

    std::vector<std::string> Funcs::SplitString(const std::string& str, char delim) {
        std::vector<std::string> items;
        std::string item;
        std::istringstream inputStream(str);
        while (std::getline(inputStream, item, delim)) {

            items.push_back(item);
        }
        return items;

     }

    bool Funcs::CharInList(char c, const std::string list) {
        return (list.find_first_of(c) != std::string::npos);
    }

    bool Funcs::AllInList(const std::string& str, const std::string list) {
        for (char c : str) {
            if (!CharInList(c,list)) {
                return false;
            }
        }

        return true;
    }

    void Funcs::Message(std::string msg) {

        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count() % 1000;

        auto t = std::time(nullptr);
        auto now = *std::localtime(&t);
        
        std::cout << std::put_time(&now, "%Y-%m-%d %H:%M:%S") << "." << std::format("{:03}", ms);
        std::cout << " " << msg << "\n";
    }

    bool Funcs::IsIdentStart(char c) {
        return CharInList(c, IDENT_START);
    }

    bool Funcs::IsIdentMiddle(char c) {
        return CharInList(c,IDENT_MIDDLE );
    }

    bool Funcs::IsNumericStart(char c) {
        return CharInList(c, NUMERIC_START);
    }

    bool Funcs::IsNumericMiddle(char c) {
        return CharInList(c, NUMERIC_MIDDLE);
    }

    std::string Funcs::GetCmdOption( const std::string& option0, const std::string& option1) {
        if (CmdOptionExists(option0)) {
            return GetCmdOption(option0);

        } else if (CmdOptionExists(option1)) {
            return GetCmdOption(option1);
        } else {
            return "";
        }


    }

    std::string Funcs::GetCmdOption( const std::string& option)
    {
        // char ** begin = *sh::CmdArguments;
        // char ** end   = *sh::CmdArguments+CmdArgumentCount;

        auto pos = std::find(CmdArguments.begin(), CmdArguments.end(), option);

        if (pos == CmdArguments.end()) {
            
            return "";
        } else {
            pos += 1;
            if (pos == CmdArguments.end()) {
                return "";
            } else {
                int idx = std::distance(CmdArguments.begin(), pos);
                return CmdArguments[idx];

            }

        }
        
    }



    bool Funcs::CmdOptionExists(const std::string& option0, const std::string& option1) {
        return CmdOptionExists(option0) || CmdOptionExists(option1);
    }

    bool Funcs::CmdOptionExists(const std::string& option) {
        return std::find(CmdArguments.begin(), CmdArguments.end(), option) != CmdArguments.end();
    }


    sf::Color Funcs::GetColour(u16 c) {
        u8 blu = c       & 0b11111;
        u8 grn = (c>>5)  & 0b11111;
        u8 red = (c>>10) & 0b11111;

        return sf::Color(red * 8, grn * 8, blu * 8);

    }

}