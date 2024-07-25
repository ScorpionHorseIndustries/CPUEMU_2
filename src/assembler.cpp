#include "common.h"

namespace sh {

    bool Assembler::ParseFromFile(std::string input_path, std::string output_path) {

        input_file_path = input_path;
        output_file_path = output_path;
        Funcs::Message(std::format("Input File  [{}] ", input_path));
        Funcs::Message(std::format("Output File [{}] ", output_path));

        
        bool ok = LoadFile(input_file_path);
        if (ok) {
            ok = Tokenise();
        }

        if (ok) {
            ok = Assemble();
        }

        if (ok) {
            Funcs::Message("Process Complete");
        }  else {
            Funcs::Message("Process Failed");
        }



        return ok;
    }

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
        Funcs::Message("Attempting `Assemlby`...");
        BuildConstants();
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
                throw std::runtime_error("invalid token " + first->value);
            }

            if (first->type == KEYWORD) {
                Output out;
                out.instruction = CPU::GetInstructionCode(first->value);

                if (second == nullptr) {
                    out.address_mode = CPU::ADDRESS_MODES::IMP;
                } else {
                    if (second->type == INVALID) {
                        throw std::runtime_error("invalid token " + second->value);
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
                    } else if (second->sub_value_type == CNS) {
                        out.label = second->sub_value;
                        out.value = ConstantGet(out.label);
                    
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
        file.open(output_file_path, std::ios::out | std::ios_base::binary);
        if (!file.is_open()) throw std::runtime_error("file not open");

        for (auto b : bytes) {
            u8 bts[2];
            bts[0] = (b >> 8) & 0xff;
            bts[1] = b & 0xff;
            file.write((char*) &bts,2);

        }
        file.flush();
        file.close();

        Funcs::Message(std::format("Wrote {} bytes to [{}]", bytes.size(), output_file_path));
        return (outputLines.size() > 0);
    
    }

    std::vector<Assembler::Token> Assembler::Tokenise(const std::string& ln) {
        std::vector<Token> tokens;

        std::string cpy = std::string(ln);
        cpy = Funcs::ToUpper(cpy);

        if (cpy.starts_with(';')) {
            //entire line is a comment
            return tokens;
        }

        size_t comment_pos = cpy.find_first_of(';');

        if (comment_pos != std::string::npos) {
            cpy = cpy.substr(0,comment_pos);
        }


        auto elements = Funcs::SplitString(cpy,' ');

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
        Funcs::Message("Attempting to Tokenise input data");
        lines.clear();
        if (data.length() > 0) {
            std::stringstream strm(data);
            std::string ln;
            std::vector<std::string> strLines;
            while (std::getline(strm, ln, '\n')) {
                
                Funcs::Remove(ln, '\n');
                Funcs::Remove(ln, '\r');
                Funcs::Replace(ln, '\t', ' ');
                Funcs::Squeeze(ln);
                Funcs::Trim(ln);
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
                if (Funcs::CharInList(c, DECIMAL_CHARS)) {
                    sub_value += c;
                }

                if (readHex && Funcs::CharInList(c, HEX_CHARS)) {
                    sub_value += c;
                }
            }

            if (sub_value.length() > 0) {
                value_int = std::stoi(sub_value, nullptr, base);
            }

 
        }

        if (sub_value_type == LBL || sub_value_type == VAR || sub_value_type == CNS) {
            for (char c : value) {
                if (c == ',') break;
                if (c == ')') break;
                if (Funcs::CharInList(c, LABEL_CHARS)) {
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

    u16 Assembler::ConstantGet(std::string constant_name) {
        std::string k = Funcs::GetKey(constant_name);

        if (Constants.contains(k)) {
            return Constants[k];
        } else {
            throw std::runtime_error(std::format("constant [{}] not found", constant_name));
        }

    }

    bool Assembler::LabelExists(std::string lbl) { 
        std::string k = Funcs::GetKey(lbl);
        return Labels.contains(k);
    }
    Assembler::Label& Assembler::LabelGet(std::string lbl) { 
        std::string k = Funcs::GetKey(lbl);
        if (LabelExists(lbl)) {
            return Labels[k];
        } else {
            throw std::runtime_error(std::format("label [{}] not found", lbl));
        }
    }
    void Assembler::LabelSet(std::string lbl, u16 address, bool isVar) { 
        std::string k = Funcs::GetKey(lbl);
        if (LabelExists(k)) {
            Labels[k].address = address;
            Labels[k].isVar = isVar;
        } else {
            Labels.insert({k, {address,isVar}});
        }
    }


    void Assembler::BuildConstants() {


        Constants.clear();
        Constants.insert_or_assign( "KEY_A_MASK"                 , 0x0001 );
        Constants.insert_or_assign( "KEY_A_ADDRESS"              , 0xBCF0 );
        Constants.insert_or_assign( "KEY_B_MASK"                 , 0x0002 );
        Constants.insert_or_assign( "KEY_B_ADDRESS"              , 0xBCF0 );
        Constants.insert_or_assign( "KEY_C_MASK"                 , 0x0004 );
        Constants.insert_or_assign( "KEY_C_ADDRESS"              , 0xBCF0 );
        Constants.insert_or_assign( "KEY_D_MASK"                 , 0x0008 );
        Constants.insert_or_assign( "KEY_D_ADDRESS"              , 0xBCF0 );
        Constants.insert_or_assign( "KEY_E_MASK"                 , 0x0010 );
        Constants.insert_or_assign( "KEY_E_ADDRESS"              , 0xBCF0 );
        Constants.insert_or_assign( "KEY_F_MASK"                 , 0x0020 );
        Constants.insert_or_assign( "KEY_F_ADDRESS"              , 0xBCF0 );
        Constants.insert_or_assign( "KEY_G_MASK"                 , 0x0040 );
        Constants.insert_or_assign( "KEY_G_ADDRESS"              , 0xBCF0 );
        Constants.insert_or_assign( "KEY_H_MASK"                 , 0x0080 );
        Constants.insert_or_assign( "KEY_H_ADDRESS"              , 0xBCF0 );
        Constants.insert_or_assign( "KEY_I_MASK"                 , 0x0100 );
        Constants.insert_or_assign( "KEY_I_ADDRESS"              , 0xBCF0 );
        Constants.insert_or_assign( "KEY_J_MASK"                 , 0x0200 );
        Constants.insert_or_assign( "KEY_J_ADDRESS"              , 0xBCF0 );
        Constants.insert_or_assign( "KEY_K_MASK"                 , 0x0400 );
        Constants.insert_or_assign( "KEY_K_ADDRESS"              , 0xBCF0 );
        Constants.insert_or_assign( "KEY_L_MASK"                 , 0x0800 );
        Constants.insert_or_assign( "KEY_L_ADDRESS"              , 0xBCF0 );
        Constants.insert_or_assign( "KEY_M_MASK"                 , 0x1000 );
        Constants.insert_or_assign( "KEY_M_ADDRESS"              , 0xBCF0 );
        Constants.insert_or_assign( "KEY_N_MASK"                 , 0x2000 );
        Constants.insert_or_assign( "KEY_N_ADDRESS"              , 0xBCF0 );
        Constants.insert_or_assign( "KEY_O_MASK"                 , 0x4000 );
        Constants.insert_or_assign( "KEY_O_ADDRESS"              , 0xBCF0 );
        Constants.insert_or_assign( "KEY_P_MASK"                 , 0x8000 );
        Constants.insert_or_assign( "KEY_P_ADDRESS"              , 0xBCF0 );
        Constants.insert_or_assign( "KEY_Q_MASK"                 , 0x0001 );
        Constants.insert_or_assign( "KEY_Q_ADDRESS"              , 0xBCF1 );
        Constants.insert_or_assign( "KEY_R_MASK"                 , 0x0002 );
        Constants.insert_or_assign( "KEY_R_ADDRESS"              , 0xBCF1 );
        Constants.insert_or_assign( "KEY_S_MASK"                 , 0x0004 );
        Constants.insert_or_assign( "KEY_S_ADDRESS"              , 0xBCF1 );
        Constants.insert_or_assign( "KEY_T_MASK"                 , 0x0008 );
        Constants.insert_or_assign( "KEY_T_ADDRESS"              , 0xBCF1 );
        Constants.insert_or_assign( "KEY_U_MASK"                 , 0x0010 );
        Constants.insert_or_assign( "KEY_U_ADDRESS"              , 0xBCF1 );
        Constants.insert_or_assign( "KEY_V_MASK"                 , 0x0020 );
        Constants.insert_or_assign( "KEY_V_ADDRESS"              , 0xBCF1 );
        Constants.insert_or_assign( "KEY_W_MASK"                 , 0x0040 );
        Constants.insert_or_assign( "KEY_W_ADDRESS"              , 0xBCF1 );
        Constants.insert_or_assign( "KEY_X_MASK"                 , 0x0080 );
        Constants.insert_or_assign( "KEY_X_ADDRESS"              , 0xBCF1 );
        Constants.insert_or_assign( "KEY_Y_MASK"                 , 0x0100 );
        Constants.insert_or_assign( "KEY_Y_ADDRESS"              , 0xBCF1 );
        Constants.insert_or_assign( "KEY_Z_MASK"                 , 0x0200 );
        Constants.insert_or_assign( "KEY_Z_ADDRESS"              , 0xBCF1 );
        Constants.insert_or_assign( "KEY_NUM0_MASK"              , 0x0400 );
        Constants.insert_or_assign( "KEY_NUM0_ADDRESS"           , 0xBCF1 );
        Constants.insert_or_assign( "KEY_NUM1_MASK"              , 0x0800 );
        Constants.insert_or_assign( "KEY_NUM1_ADDRESS"           , 0xBCF1 );
        Constants.insert_or_assign( "KEY_NUM2_MASK"              , 0x1000 );
        Constants.insert_or_assign( "KEY_NUM2_ADDRESS"           , 0xBCF1 );
        Constants.insert_or_assign( "KEY_NUM3_MASK"              , 0x2000 );
        Constants.insert_or_assign( "KEY_NUM3_ADDRESS"           , 0xBCF1 );
        Constants.insert_or_assign( "KEY_NUM4_MASK"              , 0x4000 );
        Constants.insert_or_assign( "KEY_NUM4_ADDRESS"           , 0xBCF1 );
        Constants.insert_or_assign( "KEY_NUM5_MASK"              , 0x8000 );
        Constants.insert_or_assign( "KEY_NUM5_ADDRESS"           , 0xBCF1 );
        Constants.insert_or_assign( "KEY_NUM6_MASK"              , 0x0001 );
        Constants.insert_or_assign( "KEY_NUM6_ADDRESS"           , 0xBCF2 );
        Constants.insert_or_assign( "KEY_NUM7_MASK"              , 0x0002 );
        Constants.insert_or_assign( "KEY_NUM7_ADDRESS"           , 0xBCF2 );
        Constants.insert_or_assign( "KEY_NUM8_MASK"              , 0x0004 );
        Constants.insert_or_assign( "KEY_NUM8_ADDRESS"           , 0xBCF2 );
        Constants.insert_or_assign( "KEY_NUM9_MASK"              , 0x0008 );
        Constants.insert_or_assign( "KEY_NUM9_ADDRESS"           , 0xBCF2 );
        Constants.insert_or_assign( "KEY_ESCAPE_MASK"            , 0x0010 );
        Constants.insert_or_assign( "KEY_ESCAPE_ADDRESS"         , 0xBCF2 );
        Constants.insert_or_assign( "KEY_LCONTROL_MASK"          , 0x0020 );
        Constants.insert_or_assign( "KEY_LCONTROL_ADDRESS"       , 0xBCF2 );
        Constants.insert_or_assign( "KEY_LSHIFT_MASK"            , 0x0040 );
        Constants.insert_or_assign( "KEY_LSHIFT_ADDRESS"         , 0xBCF2 );
        Constants.insert_or_assign( "KEY_LALT_MASK"              , 0x0080 );
        Constants.insert_or_assign( "KEY_LALT_ADDRESS"           , 0xBCF2 );
        Constants.insert_or_assign( "KEY_LSYSTEM_MASK"           , 0x0100 );
        Constants.insert_or_assign( "KEY_LSYSTEM_ADDRESS"        , 0xBCF2 );
        Constants.insert_or_assign( "KEY_RCONTROL_MASK"          , 0x0200 );
        Constants.insert_or_assign( "KEY_RCONTROL_ADDRESS"       , 0xBCF2 );
        Constants.insert_or_assign( "KEY_RSHIFT_MASK"            , 0x0400 );
        Constants.insert_or_assign( "KEY_RSHIFT_ADDRESS"         , 0xBCF2 );
        Constants.insert_or_assign( "KEY_RALT_MASK"              , 0x0800 );
        Constants.insert_or_assign( "KEY_RALT_ADDRESS"           , 0xBCF2 );
        Constants.insert_or_assign( "KEY_RSYSTEM_MASK"           , 0x1000 );
        Constants.insert_or_assign( "KEY_RSYSTEM_ADDRESS"        , 0xBCF2 );
        Constants.insert_or_assign( "KEY_MENU_MASK"              , 0x2000 );
        Constants.insert_or_assign( "KEY_MENU_ADDRESS"           , 0xBCF2 );
        Constants.insert_or_assign( "KEY_LBRACKET_MASK"          , 0x4000 );
        Constants.insert_or_assign( "KEY_LBRACKET_ADDRESS"       , 0xBCF2 );
        Constants.insert_or_assign( "KEY_RBRACKET_MASK"          , 0x8000 );
        Constants.insert_or_assign( "KEY_RBRACKET_ADDRESS"       , 0xBCF2 );
        Constants.insert_or_assign( "KEY_SEMICOLON_MASK"         , 0x0001 );
        Constants.insert_or_assign( "KEY_SEMICOLON_ADDRESS"      , 0xBCF3 );
        Constants.insert_or_assign( "KEY_COMMA_MASK"             , 0x0002 );
        Constants.insert_or_assign( "KEY_COMMA_ADDRESS"          , 0xBCF3 );
        Constants.insert_or_assign( "KEY_PERIOD_MASK"            , 0x0004 );
        Constants.insert_or_assign( "KEY_PERIOD_ADDRESS"         , 0xBCF3 );
        Constants.insert_or_assign( "KEY_APOSTROPHE_MASK"        , 0x0008 );
        Constants.insert_or_assign( "KEY_APOSTROPH_ADDRESS"      , 0xBCF3 );
        Constants.insert_or_assign( "KEY_SLASH_MASK"             , 0x0010 );
        Constants.insert_or_assign( "KEY_SLASH_ADDRESS"          , 0xBCF3 );
        Constants.insert_or_assign( "KEY_BACKSLASH_MASK"         , 0x0020 );
        Constants.insert_or_assign( "KEY_BACKSLASH_ADDRESS"      , 0xBCF3 );
        Constants.insert_or_assign( "KEY_GRAVE_MASK"             , 0x0040 );
        Constants.insert_or_assign( "KEY_GRAVE_ADDRESS"          , 0xBCF3 );
        Constants.insert_or_assign( "KEY_EQUAL_MASK"             , 0x0080 );
        Constants.insert_or_assign( "KEY_EQUAL_ADDRESS"          , 0xBCF3 );
        Constants.insert_or_assign( "KEY_HYPHEN_MASK"            , 0x0100 );
        Constants.insert_or_assign( "KEY_HYPHEN_ADDRESS"         , 0xBCF3 );
        Constants.insert_or_assign( "KEY_SPACE_MASK"             , 0x0200 );
        Constants.insert_or_assign( "KEY_SPACE_ADDRESS"          , 0xBCF3 );
        Constants.insert_or_assign( "KEY_ENTER_MASK"             , 0x0400 );
        Constants.insert_or_assign( "KEY_ENTER_ADDRESS"          , 0xBCF3 );
        Constants.insert_or_assign( "KEY_BACKSPACE_MASK"         , 0x0800 );
        Constants.insert_or_assign( "KEY_BACKSPACE_ADDRESS"      , 0xBCF3 );
        Constants.insert_or_assign( "KEY_TAB_MASK"               , 0x1000 );
        Constants.insert_or_assign( "KEY_TAB_ADDRESS"            , 0xBCF3 );
        Constants.insert_or_assign( "KEY_PAGEUP_MASK"            , 0x2000 );
        Constants.insert_or_assign( "KEY_PAGEUP_ADDRESS"         , 0xBCF3 );
        Constants.insert_or_assign( "KEY_PAGEDOWN_MASK"          , 0x4000 );
        Constants.insert_or_assign( "KEY_PAGEDOWN_ADDRESS"       , 0xBCF3 );
        Constants.insert_or_assign( "KEY_END_MASK"               , 0x8000 );
        Constants.insert_or_assign( "KEY_END_ADDRESS"            , 0xBCF3 );
        Constants.insert_or_assign( "KEY_HOME_MASK"              , 0x0001 );
        Constants.insert_or_assign( "KEY_HOME_ADDRESS"           , 0xBCF4 );
        Constants.insert_or_assign( "KEY_INSERT_MASK"            , 0x0002 );
        Constants.insert_or_assign( "KEY_INSERT_ADDRESS"         , 0xBCF4 );
        Constants.insert_or_assign( "KEY_DELETE_MASK"            , 0x0004 );
        Constants.insert_or_assign( "KEY_DELETE_ADDRESS"         , 0xBCF4 );
        Constants.insert_or_assign( "KEY_ADD_MASK"               , 0x0008 );
        Constants.insert_or_assign( "KEY_ADD_ADDRESS"            , 0xBCF4 );
        Constants.insert_or_assign( "KEY_SUBTRACT_MASK"          , 0x0010 );
        Constants.insert_or_assign( "KEY_SUBTRACT_ADDRESS"       , 0xBCF4 );
        Constants.insert_or_assign( "KEY_MULTIPLY_MASK"          , 0x0020 );
        Constants.insert_or_assign( "KEY_MULTIPLY_ADDRESS"       , 0xBCF4 );
        Constants.insert_or_assign( "KEY_DIVIDE_MASK"            , 0x0040 );
        Constants.insert_or_assign( "KEY_DIVIDE_ADDRESS"         , 0xBCF4 );
        Constants.insert_or_assign( "KEY_LEFT_MASK"              , 0x0080 );
        Constants.insert_or_assign( "KEY_LEFT_ADDRESS"           , 0xBCF4 );
        Constants.insert_or_assign( "KEY_RIGHT_MASK"             , 0x0100 );
        Constants.insert_or_assign( "KEY_RIGHT_ADDRESS"          , 0xBCF4 );
        Constants.insert_or_assign( "KEY_UP_MASK"                , 0x0200 );
        Constants.insert_or_assign( "KEY_UP_ADDRESS"             , 0xBCF4 );
        Constants.insert_or_assign( "KEY_DOWN_MASK"              , 0x0400 );
        Constants.insert_or_assign( "KEY_DOWN_ADDRESS"           , 0xBCF4 );
        Constants.insert_or_assign( "KEY_NUMPAD0_MASK"           , 0x0800 );
        Constants.insert_or_assign( "KEY_NUMPAD0_ADDRESS"        , 0xBCF4 );
        Constants.insert_or_assign( "KEY_NUMPAD1_MASK"           , 0x1000 );
        Constants.insert_or_assign( "KEY_NUMPAD1_ADDRESS"        , 0xBCF4 );
        Constants.insert_or_assign( "KEY_NUMPAD2_MASK"           , 0x2000 );
        Constants.insert_or_assign( "KEY_NUMPAD2_ADDRESS"        , 0xBCF4 );
        Constants.insert_or_assign( "KEY_NUMPAD3_MASK"           , 0x4000 );
        Constants.insert_or_assign( "KEY_NUMPAD3_ADDRESS"        , 0xBCF4 );
        Constants.insert_or_assign( "KEY_NUMPAD4_MASK"           , 0x8000 );
        Constants.insert_or_assign( "KEY_NUMPAD4_ADDRESS"        , 0xBCF4 );
        Constants.insert_or_assign( "KEY_NUMPAD5_MASK"           , 0x0001 );
        Constants.insert_or_assign( "KEY_NUMPAD5_ADDRESS"        , 0xBCF5 );
        Constants.insert_or_assign( "KEY_NUMPAD6_MASK"           , 0x0002 );
        Constants.insert_or_assign( "KEY_NUMPAD6_ADDRESS"        , 0xBCF5 );
        Constants.insert_or_assign( "KEY_NUMPAD7_MASK"           , 0x0004 );
        Constants.insert_or_assign( "KEY_NUMPAD7_ADDRESS"        , 0xBCF5 );
        Constants.insert_or_assign( "KEY_NUMPAD8_MASK"           , 0x0008 );
        Constants.insert_or_assign( "KEY_NUMPAD8_ADDRESS"        , 0xBCF5 );
        Constants.insert_or_assign( "KEY_NUMPAD9_MASK"           , 0x0010 );
        Constants.insert_or_assign( "KEY_NUMPAD9_ADDRESS"        , 0xBCF5 );
        Constants.insert_or_assign( "KEY_F1_MASK"                , 0x0020 );
        Constants.insert_or_assign( "KEY_F1_ADDRESS"             , 0xBCF5 );
        Constants.insert_or_assign( "KEY_F2_MASK"                , 0x0040 );
        Constants.insert_or_assign( "KEY_F2_ADDRESS"             , 0xBCF5 );
        Constants.insert_or_assign( "KEY_F3_MASK"                , 0x0080 );
        Constants.insert_or_assign( "KEY_F3_ADDRESS"             , 0xBCF5 );
        Constants.insert_or_assign( "KEY_F4_MASK"                , 0x0100 );
        Constants.insert_or_assign( "KEY_F4_ADDRESS"             , 0xBCF5 );
        Constants.insert_or_assign( "KEY_F5_MASK"                , 0x0200 );
        Constants.insert_or_assign( "KEY_F5_ADDRESS"             , 0xBCF5 );
        Constants.insert_or_assign( "KEY_F6_MASK"                , 0x0400 );
        Constants.insert_or_assign( "KEY_F6_ADDRESS"             , 0xBCF5 );
        Constants.insert_or_assign( "KEY_F7_MASK"                , 0x0800 );
        Constants.insert_or_assign( "KEY_F7_ADDRESS"             , 0xBCF5 );
        Constants.insert_or_assign( "KEY_F8_MASK"                , 0x1000 );
        Constants.insert_or_assign( "KEY_F8_ADDRESS"             , 0xBCF5 );
        Constants.insert_or_assign( "KEY_F9_MASK"                , 0x2000 );
        Constants.insert_or_assign( "KEY_F9_ADDRESS"             , 0xBCF5 );
        Constants.insert_or_assign( "KEY_F10_MASK"               , 0x4000 );
        Constants.insert_or_assign( "KEY_F10_ADDRESS"            , 0xBCF5 );
        Constants.insert_or_assign( "KEY_F11_MASK"               , 0x8000 );
        Constants.insert_or_assign( "KEY_F11_ADDRESS"            , 0xBCF5 );
        Constants.insert_or_assign( "KEY_F12_MASK"               , 0x0001 );
        Constants.insert_or_assign( "KEY_F12_ADDRESS"            , 0xBCF6 );
        Constants.insert_or_assign( "KEY_F13_MASK"               , 0x0002 );
        Constants.insert_or_assign( "KEY_F13_ADDRESS"            , 0xBCF6 );
        Constants.insert_or_assign( "KEY_F14_MASK"               , 0x0004 );
        Constants.insert_or_assign( "KEY_F14_ADDRESS"            , 0xBCF6 );
        Constants.insert_or_assign( "KEY_F15_MASK"               , 0x0008 );
        Constants.insert_or_assign( "KEY_F15_ADDRESS"            , 0xBCF6 );
        Constants.insert_or_assign( "KEY_PAUSE_MASK"             , 0x0010 );
        Constants.insert_or_assign( "KEY_PAUSE_ADDRESS"          , 0xBCF6 );

        
        Constants.insert_or_assign( "CPU_SCREEN_SIZE"    , CPU_SCREEN_SIZE    );
        Constants.insert_or_assign( "CPU_LAST_INDEX"     , CPU_LAST_INDEX     );
        Constants.insert_or_assign( "CPU_VRAM_START"     , CPU_VRAM_START     );
        Constants.insert_or_assign( "CPU_STACK_START"    , CPU_STACK_START    );
        Constants.insert_or_assign( "CPU_ROM_START"      , CPU_ROM_START      );
        Constants.insert_or_assign( "CPU_RAM_START"      , CPU_RAM_START      );
        Constants.insert_or_assign( "CPU_RAND_01"        , CPU_RAND_01        );
        Constants.insert_or_assign( "CPU_RAND_02"        , CPU_RAND_02        );
        Constants.insert_or_assign( "CPU_RAND_03"        , CPU_RAND_03        );
        Constants.insert_or_assign( "CPU_RAND_04"        , CPU_RAND_04        );
        Constants.insert_or_assign( "CPU_KEYSTATE_START" , CPU_KEYSTATE_START );

    }

}