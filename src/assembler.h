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
                     
        public:
            Assembler() {
            }
            enum TOKEN_TYPE : u8 {
                NONE                = 0,
                TEXT                ,
                NUMERIC_LITERAL     ,
                INDIRECT_ADDRESS    ,
                ADDRESS_LITERAL     ,
                KEYWORD             ,
                VAR_USE             ,
                LABEL_USE           ,
                LABEL_DECLARE       ,
                REGISTER            ,
                REGISTER_PAIR       ,
                INVALID             = 0xff,

            };

            enum TOKEN_SUB_VALUE_TYPE : u8 {
                TSVT_NONE = 0,
                TSVT_LABEL,
                TSVT_VAR,
                TSVT_INT
            
            };
            inline static const std::map<u8, std::string> TOKEN_TYPE_NAMES = {
                { NONE                  , "NONE"                    },
                { TEXT                  , "TEXT"                    },
                { NUMERIC_LITERAL       , "NUMERIC_LITERAL"         },
                { INDIRECT_ADDRESS      , "INDIRECT_ADDRESS"        },
                { ADDRESS_LITERAL       , "ADDRESS_LITERAL"         },
                { KEYWORD               , "KEYWORD"                 },
                { VAR_USE               , "VAR_USE"                 },
                { LABEL_USE             , "LABEL_USE"               },
                { LABEL_DECLARE         , "LABEL_DECLARE"           },
                { REGISTER              , "REGISTER"                },
                { REGISTER_PAIR         , "REGISTER_PAIR"           },
                { INVALID               , "INVALID"                 },
               
               
            };
            struct Token {
                std::string value;
                TOKEN_TYPE type;
                TOKEN_SUB_VALUE_TYPE sub_value_type;
                int value_int = 0;
                std::string labelvar_name;
                int registerFrom = 0, registerTo = 0;
                Token(std::string _value, TOKEN_TYPE _type);

                std::string str() const;
            };

            struct Something {
                bool        isLabel         = 0;
                u16         opcode          = 0;
                u8          instruction     = 0;
                u8          address_mode    = 0;
                u8          length          = 0;
                u16         address         = 0;
                u16         value           = 0;
                std::string label              ;


            
            };

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


            


            bool LoadFile(std::string path);
            bool Tokenise();
            bool Assemble();
            std::vector<Token> Tokenise(const std::string& ln);
            bool LabelExists(std::string lbl);
            Label LabelGet(std::string lbl);
            void LabelSet(std::string lbl, u16 address, bool isVar = false);

     
            void PrintTokenLines() {
                for (const auto& tl : lines) {
                    std::cout << tl.line << std::endl;
                    for (const auto& t : tl.tokens) {
                        std::cout << "\t" << t.str() << std::endl;
                    }
                }
            }


    };

}

#endif 