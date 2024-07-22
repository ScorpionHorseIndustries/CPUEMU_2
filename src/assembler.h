#ifndef __ASSEMBLER_H_
#define __ASSEMBLER_H_

#include "common.h"

/*
[a-z][a-z0-9_]      identifier
@identifer          var_use
identifer:          label_declare
identifier          label_use
[a-z]{4}            Keyword
$[a-f\d]+           hex
\d+                 dec
hex|dec             literal
$literal            address literal
(label_use)         indirect with label
(literal)           indirect with literal
(var_use)           indirect with var



*/



namespace sh {
    
    
    
    
    
    

    class Assembler {
        private:
            std::string input_file_path;
            std::string output_file_path;
        public:
            Assembler() {
            }
            enum TOKEN_TYPE : u8 {
                NONE                = 0,
                // TEXT                ,
                NUMERIC_LITERAL     ,
                INDIRECT_ADDRESS    ,
                ADDRESS_LITERAL     ,
                KEYWORD             ,
                VAR_USE             ,
                LABEL_USE           ,
                LABEL_DECLARE       ,
                REGISTER            ,
                REGISTER_PAIR       ,
                OTHER               ,
                INVALID             = 0xff,

            };

            enum TOKEN_SUB_VALUE_TYPE : u8 {
                HEX=1,
                DEC,
                LBL,
                VAR,
                REG,
                RGP     //register pair
            };
            
            inline static const std::map<u8, std::string> TOKEN_TYPE_NAMES = {
                { NONE                  , "NONE"                    },
                { NUMERIC_LITERAL       , "NUMERIC_LITERAL"         },
                { INDIRECT_ADDRESS      , "INDIRECT_ADDRESS"        },
                { ADDRESS_LITERAL       , "ADDRESS_LITERAL"         },
                { KEYWORD               , "KEYWORD"                 },
                { VAR_USE               , "VAR_USE"                 },
                { LABEL_USE             , "LABEL_USE"               },
                { LABEL_DECLARE         , "LABEL_DECLARE"           },
                { REGISTER              , "REGISTER"                },
                { REGISTER_PAIR         , "REGISTER_PAIR"           },
                { OTHER                 , "OTHER"                   },
                { INVALID               , "INVALID"                 },
               
               
            };
            struct Token {
                std::string value;
                std::string sub_value;
                TOKEN_TYPE type;
                u8 sub_value_type;
                int value_int = 0;
                // std::string labelvar_name;
                u8 registerFrom = 0, registerTo = 0;
                Token(std::string _value, TOKEN_TYPE _type);
                u8 address_mode = 0;
                 
                std::string str() const;
            };

            struct Output {
                bool        isDeclareLabel  = false;
                bool        valueIsLabel    = false;
                bool        valueIsVar      = false;

                u16         opcode          = 0;
                u8          instruction     = 0;
                u8          address_mode    = 0;
                u8          length          = 0;
                u16         address         = 0;
                u16         value           = 0;
                std::string label           = "";
            };
            std::vector<Output> outputLines;
            
            struct TokenLine {
                std::string line;
                std::vector<Token> tokens;
            };

            
            std::vector<TokenLine> lines;
            std::string data;

            size_t tk_cursor = 0;
            std::string tk_current_string;
            char tk_current = '\0';
            bool tk_finished = false;
            void tk_advance();
            Token tk_read_number();
            Token rk_read_text();


            struct Label {
                u16 address;
                bool isVar;
            };
            std::map<std::string, Label> Labels;

            bool ParseFromFile(std::string input_path, std::string output_path);
            bool LoadFile(std::string path);
            bool Tokenise();
            bool Assemble();
            std::vector<Token> Tokenise(const std::string& ln);
            bool LabelExists(std::string lbl);
            Label& LabelGet(std::string lbl);
            void LabelSet(std::string lbl, u16 address, bool isVar = false);
            std::vector<u16> bytes;
            
     
            void PrintTokenLines() {
                std::cout << "Print token Lines:" << std::endl;
                for (const auto& tl : lines) {
                    std::cout << tl.line << std::endl;
                    for (const auto& t : tl.tokens) {
                        std::cout << "\t" << t.str() << std::endl;
                    }
                }
                std::cout << "End Print token Lines" << std::endl;
            }

            struct RegexGrammarToken {
                std::regex  rgx;
                TOKEN_TYPE  token_type;
                u8          address_mode;
                u8          sub_value_type;
            };
            
            inline static const std::vector<RegexGrammarToken> REGEX_GRAMMAR = {
                { std::regex( R"rgx987(#\$[\dA-Fa-f]+)rgx987"                  ,std::regex::icase), NUMERIC_LITERAL, CPU::ADDRESS_MODES::IMM, HEX },          
                { std::regex( R"rgx987(#\d+)rgx987"                            ,std::regex::icase), NUMERIC_LITERAL, CPU::ADDRESS_MODES::IMM, DEC },
                { std::regex( R"rgx987(\$[\dA-Fa-f]+)rgx987"                   ,std::regex::icase), ADDRESS_LITERAL, CPU::ADDRESS_MODES::ABS, HEX },          
                { std::regex( R"rgx987(\$[\dA-Fa-f]+,B)rgx987"                 ,std::regex::icase), ADDRESS_LITERAL, CPU::ADDRESS_MODES::ABB, HEX },        
                { std::regex( R"rgx987(\$[\dA-Fa-f]+,C)rgx987"                 ,std::regex::icase), ADDRESS_LITERAL, CPU::ADDRESS_MODES::ABC, HEX },
                { std::regex( R"rgx987(\d+)rgx987"                             ,std::regex::icase), ADDRESS_LITERAL, CPU::ADDRESS_MODES::ABS, DEC },
                { std::regex( R"rgx987(\d+,B)rgx987"                           ,std::regex::icase), ADDRESS_LITERAL, CPU::ADDRESS_MODES::ABB, DEC },
                { std::regex( R"rgx987(\d+,C)rgx987"                           ,std::regex::icase), ADDRESS_LITERAL, CPU::ADDRESS_MODES::ABC, DEC },
                { std::regex( R"rgx987(@[a-zA-Z][a-zA-Z_0-9]+)rgx987"          ,std::regex::icase), ADDRESS_LITERAL, CPU::ADDRESS_MODES::ABS, VAR },        
                { std::regex( R"rgx987(@[a-zA-Z][a-zA-Z_0-9]+,B)rgx987"        ,std::regex::icase), ADDRESS_LITERAL, CPU::ADDRESS_MODES::ABB, VAR },
                { std::regex( R"rgx987(@[a-zA-Z][a-zA-Z_0-9]+,C)rgx987"        ,std::regex::icase), ADDRESS_LITERAL, CPU::ADDRESS_MODES::ABC, VAR },
                { std::regex( R"rgx987([a-zA-Z][a-zA-Z_0-9]+)rgx987"           ,std::regex::icase), ADDRESS_LITERAL, CPU::ADDRESS_MODES::ABS, LBL },
                { std::regex( R"rgx987([a-zA-Z][a-zA-Z_0-9]+,B)rgx987"         ,std::regex::icase), ADDRESS_LITERAL, CPU::ADDRESS_MODES::ABB, LBL },
                { std::regex( R"rgx987([a-zA-Z][a-zA-Z_0-9]+,C)rgx987"         ,std::regex::icase), ADDRESS_LITERAL, CPU::ADDRESS_MODES::ABC, LBL },
                { std::regex( R"rgx987(\(\$[\dA-Fa-f]+\))rgx987"               ,std::regex::icase), ADDRESS_LITERAL, CPU::ADDRESS_MODES::IND, HEX },
                { std::regex( R"rgx987(\(\$[\dA-Fa-f]+,B\))rgx987"             ,std::regex::icase), ADDRESS_LITERAL, CPU::ADDRESS_MODES::INB, HEX },
                { std::regex( R"rgx987(\(\$[\dA-Fa-f]+\),C)rgx987"             ,std::regex::icase), ADDRESS_LITERAL, CPU::ADDRESS_MODES::INC, HEX },
                { std::regex( R"rgx987(\(\d+\))rgx987"                         ,std::regex::icase), ADDRESS_LITERAL, CPU::ADDRESS_MODES::ABS, DEC },
                { std::regex( R"rgx987(\(\d+,B\))rgx987"                       ,std::regex::icase), ADDRESS_LITERAL, CPU::ADDRESS_MODES::ABB, DEC },
                { std::regex( R"rgx987(\(\d+\),C)rgx987"                       ,std::regex::icase), ADDRESS_LITERAL, CPU::ADDRESS_MODES::ABC, DEC },
                { std::regex( R"rgx987(\(@[a-zA-Z][a-zA-Z_0-9]+\))rgx987"      ,std::regex::icase), ADDRESS_LITERAL, CPU::ADDRESS_MODES::ABS, VAR },        
                { std::regex( R"rgx987(\(@[a-zA-Z][a-zA-Z_0-9]+,B\))rgx987"    ,std::regex::icase), ADDRESS_LITERAL, CPU::ADDRESS_MODES::ABB, VAR },
                { std::regex( R"rgx987(\(@[a-zA-Z][a-zA-Z_0-9]+\),C)rgx987"    ,std::regex::icase), ADDRESS_LITERAL, CPU::ADDRESS_MODES::ABC, VAR },
                { std::regex( R"rgx987(\([a-zA-Z][a-zA-Z_0-9]+\))rgx987"       ,std::regex::icase), ADDRESS_LITERAL, CPU::ADDRESS_MODES::ABS, LBL },
                { std::regex( R"rgx987(\([a-zA-Z][a-zA-Z_0-9]+,B\))rgx987"     ,std::regex::icase), ADDRESS_LITERAL, CPU::ADDRESS_MODES::ABB, LBL },
                { std::regex( R"rgx987(\([a-zA-Z][a-zA-Z_0-9]+\),C)rgx987"     ,std::regex::icase), ADDRESS_LITERAL, CPU::ADDRESS_MODES::ABC, LBL },
                { std::regex( R"rgx987([ABCXYZ],[ABCXYZ])rgx987"               ,std::regex::icase), REGISTER_PAIR  , CPU::ADDRESS_MODES::RG2, RGP },
                { std::regex( R"rgx987(A)rgx987"                               ,std::regex::icase), REGISTER       , CPU::ADDRESS_MODES::RGA, REG },
                { std::regex( R"rgx987(B)rgx987"                               ,std::regex::icase), REGISTER       , CPU::ADDRESS_MODES::RGB, REG },
                { std::regex( R"rgx987(C)rgx987"                               ,std::regex::icase), REGISTER       , CPU::ADDRESS_MODES::RGC, REG },
                { std::regex( R"rgx987(X)rgx987"                               ,std::regex::icase), REGISTER       , CPU::ADDRESS_MODES::RGX, REG },
                { std::regex( R"rgx987(Y)rgx987"                               ,std::regex::icase), REGISTER       , CPU::ADDRESS_MODES::RGY, REG },
                { std::regex( R"rgx987(Z)rgx987"                               ,std::regex::icase), REGISTER       , CPU::ADDRESS_MODES::RGZ, REG },
            
            };


    };

}

#endif 