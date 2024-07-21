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

    bool Assembler::Assemble() {
        std::vector<Output> output;
        u16 byte_position = 0;
        for (auto& tl : lines) {
            Token* first = &tl.tokens[0];
            Token* second = nullptr;
            if (tl.tokens.size() > 1) {
                second = &tl.tokens[1];
            }

            if (first->type == KEYWORD) {
                Output out;
                out.instruction = CPU::GetInstructionCode(first->value);
                out.address = byte_position;

                if (second == nullptr) {
                    out.address_mode = CPU::ADDRESS_MODES::IMP;
                } else {
                    out.address_mode = second->address_mode;
                    
                    
                }



            } else if (first->type == LABEL_DECLARE) {
                Output out;
                out.label = first->value;
                out.isLabel = true;
                output.push_back(out);
            } else {
                //error
            }


        }
        return (output.size() > 0);
    
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

        if (elements.size() == 1) {
            if (elements[0].ends_with(':')) {
                //declare label
                tokens.push_back({elements[0], LABEL_DECLARE});
            } else {
                //keyword
                if (CPU::GetInstructionCode(elements[0]) > 0) {
                    tokens.push_back({elements[0], KEYWORD});
                } else {
                    //fucken error, innit
                }
            }
        } else if (elements.size() == 2) {

        }


        return tokens;
    }

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
        return std::format("[{}[{}][{}][{}]]",value, TOKEN_TYPE_NAMES.at(type), value_int, sub_value);
    }

    Assembler::Token::Token(std::string _value, TOKEN_TYPE _type) : value(_value), type(_type) {
        std::string int_to_parse = "";
        int base = 10;
        bool readHex = false;
        bool readDec = true;
        sub_value = "";

        for (auto& [rgx_string, adrm_type_pair] : REGEX_GRAMMAR) {
        
            std::regex rgx(rgx_string, std::regex_constants::icase);

            if (std::regex_match(value, rgx)) {
                address_mode = adrm_type_pair.first;
                sub_value_type = adrm_type_pair.second;
                break;
            }
        }    


        if (sub_value_type == HEX || sub_value_type == DEC) {
            bool readHex = sub_value_type == HEX;
            int base = readHex ? 16 : 10;
            for (char c : value) {
                if (c == ',') break;
                if (c == ')') break;
                if (CharInList(c, DECIMAL_CHARS)) {
                    sub_value += c;
                }

                if (readHex && CharInList(c, HEX_CHARS)) {
                    sub_value += c;
                }
            }

            if (sub_value.length() > 0) {
                value_int = std::stoi(sub_value, nullptr, base);
            }

 
        }

        if (sub_value_type == LBL || sub_value_type == VAR) {
            for (char c : value) {
                if (c == ',') break;
                if (c == ')') break;
                if (CharInList(c, LABEL_CHARS)) {
                    sub_value += c;
                }
            
            }
        
        }

        if (address_mode == CPU::ADDRESS_MODES::RG2) {
            registerFrom = CPU::REGISTER_NAMES.at(value[0]);
            registerTo = CPU::REGISTER_NAMES.at(value[2]);
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
        } else {
            throw std::runtime_error(std::format("label [{}]not found", lbl));
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