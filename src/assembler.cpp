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
        // std::vector<Output> outputLines;
        outputLines.clear();
        u16 byte_position = 0;
        u16 var_counter = 0;
        for (auto& tl : lines) {
            Token* first = &tl.tokens[0];
            Token* second = nullptr;
            if (tl.tokens.size() > 1) {
                second = &tl.tokens[1];
            }

            if (first->type == INVALID) {
                throw std::runtime_error("invalid token");
            }

            if (first->type == KEYWORD) {
                Output out;
                out.instruction = CPU::GetInstructionCode(first->value);

                if (second == nullptr) {
                    out.address_mode = CPU::ADDRESS_MODES::IMP;
                } else {
                    if (second->type == INVALID) {
                        throw std::runtime_error("invalid token");
                    }
                    out.address_mode = second->address_mode;

                    if (second->sub_value_type == VAR) {
                        out.valueIsVar = true;
                        if (!LabelExists(second->sub_value)) {
                            out.label = second->sub_value;
                            out.value = CPU_RAM_START+var_counter;
                            LabelSet(second->sub_value, out.value,true);
                            var_counter += 1;
                        } else {
                            auto& label = LabelGet(second->sub_value);
                            out.value = label.address;
                        }
                    }  else if (second->sub_value_type == LBL) {
                        out.label = second->sub_value;
                        out.valueIsLabel = true;
                        out.value = 0;
                    } else {
                        out.value = second->value_int;
                    }
                }
                out.address = byte_position;
                out.length = CPU::ADDRESS_MODE_LENGTH.at(out.address_mode);
                byte_position += out.length;
                outputLines.push_back(out);



            } else if (first->type == LABEL_DECLARE) {
                Output out;
                out.label = first->value;
                out.isDeclareLabel = true;
                out.length = 0;
                out.address = byte_position;
                LabelSet(out.label,out.address,false);
                
                outputLines.push_back(out);
            } else {
                //error
                throw std::runtime_error("incorrect type encountered:\n" + tl.line);
            }
        }


        for (int i = 0; i < outputLines.size(); i += 1) {
            auto& out = outputLines[i];
            if (out.isDeclareLabel) {
                for (int j = i + 1; j < outputLines.size(); j += 1) {
                    auto& next = outputLines[j];
                    if (next.instruction > 0) {
                        out.address = next.address;
                        break;
                    }
                }
            }
        }

        for (int i = 0; i < outputLines.size(); i += 1) {
            auto& out = outputLines[i];

            if (out.valueIsLabel) {
                out.value = LabelGet(out.label).address;
            }

            if (out.instruction > 0) {
                bytes.push_back(CPU::EncodeOpcode(out.instruction, out.address_mode));
                if (out.length > 1) {
                    bytes.push_back(out.value);
                }
            }
        }

        std::ofstream file;
        file.open(".\\programs\\out.faux", std::ios::out | std::ios_base::binary);
        if (!file.is_open()) throw std::runtime_error("file not open");

        for (auto b : bytes) {
            u8 bts[2];
            bts[0] = (b >> 8) & 0xff;
            bts[1] = b & 0xff;
            file.write((char*) &bts,2);

        }
        file.flush();
        file.close();

        
        return (outputLines.size() > 0);
    
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
                    tokens.push_back({elements[0], INVALID});
                }
            }
        } else if (elements.size() == 2) {
            if (CPU::GetInstructionCode(elements[0]) > 0) {
                tokens.push_back({elements[0], KEYWORD});
            } else {
                // error
                tokens.push_back({elements[0], INVALID});
            }

            tokens.push_back({elements[1], OTHER});
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

                if (tl.tokens.size() > 0) {
                    lines.push_back(tl);
                }

                
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


        if (type == OTHER) {
            for (auto& grammar : REGEX_GRAMMAR) {
                if (std::regex_match(value, grammar.rgx)) {
                    type            = grammar.token_type;
                    address_mode    = grammar.address_mode;
                    sub_value_type  = grammar.sub_value_type;
                    break;
                }
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
            value_int = CPU::MakeReg2(registerFrom, registerTo);
        }

        if (value_int > 0xffff || value_int < 0) {
            throw std::runtime_error(
                    std::format("INT [{}] out of range", value_int)
                );
        }
        //if we get down here and it's still "other",
        //then it's invalid
        if (type == OTHER) {
            type = INVALID;
        }

        

    }

    bool Assembler::LabelExists(std::string lbl) { 
        std::string k = GetKey(lbl);
        return Labels.contains(k);
    }
    Assembler::Label& Assembler::LabelGet(std::string lbl) { 
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