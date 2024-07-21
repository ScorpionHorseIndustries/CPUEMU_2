#include "common.h"

namespace sh {
    bool Assembler::LoadFile(std::string path) {
        std::ifstream file(path);
        if (file.is_open()) {
            std::ostringstream ss;
            ss << file.rdbuf();
            data = ss.str();
            return true;
        } else {
            return false;
        
        }

        
    }


    void Assembler::tk_advance() {
        if (tk_cursor >= tk_current_string.length()) {
            tk_finished = true;
        } else {
            tk_current = tk_current_string[tk_cursor];

            tk_cursor += 1;


        
        }
    }
    std::vector<Assembler::Token> Assembler::Tokenise(const std::string& ln) {
        std::vector<Token> tokens;

        std::string cpy = std::string(ln);
        cpy = ToUpper(cpy);

        if (cpy.starts_with(';')) {
            //entire line is a comment
            return tokens;
        }

        size_t comment_pos = cpy.find_first_of(';');

        if (comment_pos != std::string::npos) {
            cpy = cpy.substr(0,comment_pos);
        }


        auto elements = SplitString(cpy,' ');

        
        for (auto& elem : elements) {
            
            if (elem.length() == 0) continue;

            if (elem.length() == 1 && AllInList(elem, REGNAMES)) {
                tokens.push_back({elem, REGISTER});
            } else if (elem.length() == 3 && CharInList(elem[0],REGNAMES) && elem[1] == ',' && CharInList(elem[2], REGNAMES)) {
                tokens.push_back({elem, REGISTER_PAIR});   
            } else if (elem.starts_with('$')) {
                tokens.push_back({elem, ADDRESS_LITERAL});
            } else if (elem.ends_with(':')) {
                tokens.push_back({elem, LABEL_DECLARE});
            } else if (elem.starts_with('#')) {
                tokens.push_back({elem, NUMERIC_LITERAL});
            } else if (elem.starts_with('(')) { 
                tokens.push_back({elem, INDIRECT_ADDRESS});
            } else if (elem.starts_with('@')) {
                tokens.push_back({elem, VAR_USE});
            } else if (AllInList(elem, KEYWORD_CHARS)) {            

                u8 instr_code = CPU::GetInstructionCode(elem);

                if (instr_code > 0) {
                    tokens.push_back({elem, KEYWORD });
                } else {
                    tokens.push_back({elem, LABEL_USE});
                }

            } else if (AllInList(elem, LABEL_CHARS)) {
                tokens.push_back({elem, LABEL_USE});
            } else {
                //must be decimal address literal, right?
                tokens.push_back({elem, ADDRESS_LITERAL});
            }
        }

        return tokens;

    }
        // tk_current_string = std::string(ln);
        // tk_cursor = 0;
        // tk_current = '\0';
        // tk_finished = false;
        // std::vector<Token> tokens;

        // tk_advance();
        // std::string value = "";
        // TOKEN_TYPE type = TOKEN_TYPE::NONE;
        // while (!tk_finished) {
        //     if (tk_current == ';') {
        //         tk_finished = true;

        //     } else if (tk_current == ' ' || tk_current == '\t') {
        //         tk_advance();
        //     } else if (IsIdentStart(tk_current)) {
        //         value = "";
        //         value += tk_current;
        //         type = TOKEN_TYPE::TEXT;
        //         tk_advance();
        //         while (!tk_finished && IsIdentMiddle(tk_current)) {
        //             value += tk_current;
        //             tk_advance();
        //         }
        //         tokens.push_back({value,type});


                
        //     } else if (IsNumericStart(tk_current)) {
        //         value = "";
        //         value += tk_current;
        //         type = TOKEN_TYPE::NUMERIC;
        //         tk_advance();
        //         while (!tk_finished && IsNumericMiddle(tk_current)) {
        //             value += tk_current;
        //             tk_advance();
        //         }
        //         tokens.push_back({value,type});            
            
            
        //     } else {
        //         tk_advance();
        //     }
        
        // }

    //     return tokens;



    // }
    bool Assembler::Tokenise() {
        lines.clear();
        if (data.length() > 0) {
            std::stringstream strm(data);
            std::string ln;
            std::vector<std::string> strLines;
            while (std::getline(strm, ln, '\n')) {
                
                Remove(ln, '\n');
                Remove(ln, '\r');
                Replace(ln, '\t', ' ');
                Squeeze(ln);
                Trim(ln);
                if (ln.length() == 0) continue;

                TokenLine tl;
                tl.line = ln;
                tl.tokens = Tokenise(ln);
                lines.push_back(tl);

                
            }




        }

        return lines.size() > 0;
    }

    std::string Assembler::Token::str() const {
        return std::format("[{}[{}][{}]]",value, TOKEN_TYPE_NAMES.at(type), value_int);
    }

    Assembler::Token::Token(std::string _value, TOKEN_TYPE _type) : value(_value), type(_type) {
        std::string int_to_parse = "";
        int base = 10;
        bool readHex = false;
        bool readDec = true;
        sub_value_type = TSVT_NONE;
        // std::string cpy = std::string(value);
        // Remove(cpy, '(');
        // Remove(cpy, ')');
        if (type == INDIRECT_ADDRESS) {
            sub_value_type = TSVT_INT;
            for (char c : value) {
                if (c == '(') continue;
                if (c == '$') {
                    sub_value_type = TSVT_INT;
                    break;
                }
                if (c == '@') {
                    sub_value_type = TSVT_VAR;
                    break;
                }

                if (CharInList(c, IDENT_START)) {
                    sub_value_type = TSVT_LABEL;
                    break;
                }
            
            }
        } else if (type == ADDRESS_LITERAL || type == NUMERIC_LITERAL) {
            sub_value_type = TSVT_INT;
        }

        if (sub_value_type == TSVT_INT) {
            for (char c : value) {

                if (c == ')') break;
                if (c == ',') break;
                if (c == '@') break;

                if (c == '$') {
                    readHex = true;
                    base = 16;
                    continue;
                }

                if (readHex && CharInList(c, HEX_CHARS)) {
                    
                    int_to_parse += c;
                }

                if (readDec && CharInList(c, DECIMAL_CHARS)) {
                    int_to_parse += c;
                }

                
            }
        }

        if (int_to_parse.length() > 0) {
            value_int = std::stoi(int_to_parse, nullptr, base);
        }

        


    }

    bool Assembler::LabelExists(std::string lbl) { 
        std::string k = GetKey(lbl);
        return Labels.contains(k);
    }
    Assembler::Label Assembler::LabelGet(std::string lbl) { 
        std::string k = GetKey(lbl);
        if (LabelExists(lbl)) {
            return Labels[k];
        }
    }
    void Assembler::LabelSet(std::string lbl, u16 address, bool isVar) { 
        std::string k = GetKey(lbl);
        if (LabelExists(k)) {
            Labels[k].address = address;
            Labels[k].isVar = isVar;
        } else {
            Labels.insert({k, {address,isVar}});
        }
    }

}