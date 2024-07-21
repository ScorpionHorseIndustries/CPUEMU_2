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
                HEX=1,
                DEC,
                LBL,
                VAR,
                REG
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
                std::string sub_value;
                TOKEN_TYPE type;
                u8 sub_value_type;
                int value_int = 0;
                // std::string labelvar_name;
                int registerFrom = 0, registerTo = 0;
                Token(std::string _value, TOKEN_TYPE _type);
                u8 address_mode = 0;
                 
                std::string str() const;
            };

            struct Output {
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
            
            inline static const std::map<std::string, std::pair<u8,u8>> REGEX_GRAMMAR = {
                {R"B09OPA3(#\$[\da-f]+)B09OPA3",                {CPU::ADDRESS_MODES::IMM,HEX}},          
                {R"B09OPA3(#\d+)B09OPA3",                       {CPU::ADDRESS_MODES::IMM,DEC}},
                {R"B09OPA3(\$[\da-f]+)B09OPA3",                 {CPU::ADDRESS_MODES::ABS,HEX}},          
                {R"B09OPA3(\$[\da-f]+,B)B09OPA3",               {CPU::ADDRESS_MODES::ABB,HEX}},        
                {R"B09OPA3(\$[\da-f]+,C)B09OPA3",               {CPU::ADDRESS_MODES::ABC,HEX}},
                {R"B09OPA3(\d+)B09OPA3",                        {CPU::ADDRESS_MODES::ABS,DEC}},
                {R"B09OPA3(\d+,B)B09OPA3",                      {CPU::ADDRESS_MODES::ABB,DEC}},
                {R"B09OPA3(\d+,C)B09OPA3",                      {CPU::ADDRESS_MODES::ABC,DEC}},
                {R"B09OPA3(@[a-z][a-z_0-9]+)B09OPA3",           {CPU::ADDRESS_MODES::ABS,VAR}},        
                {R"B09OPA3(@[a-z][a-z_0-9]+,B)B09OPA3",         {CPU::ADDRESS_MODES::ABB,VAR}},
                {R"B09OPA3(@[a-z][a-z_0-9]+,C)B09OPA3",         {CPU::ADDRESS_MODES::ABC,VAR}},
                {R"B09OPA3([a-z][a-z_0-9]+)B09OPA3",            {CPU::ADDRESS_MODES::ABS,LBL}},
                {R"B09OPA3([a-z][a-z_0-9]+,B)B09OPA3",          {CPU::ADDRESS_MODES::ABB,LBL}},
                {R"B09OPA3([a-z][a-z_0-9]+,C)B09OPA3",          {CPU::ADDRESS_MODES::ABC,LBL}},
                {R"B09OPA3(\(\$[\da-f]+\))B09OPA3",             {CPU::ADDRESS_MODES::IND,HEX}},
                {R"B09OPA3(\(\$[\da-f]+,B\))B09OPA3",           {CPU::ADDRESS_MODES::INB,HEX}},
                {R"B09OPA3(\(\$[\da-f]+\),C)B09OPA3",           {CPU::ADDRESS_MODES::INC,HEX}},
                {R"B09OPA3(\(\d+\))B09OPA3",                    {CPU::ADDRESS_MODES::ABS,DEC}},
                {R"B09OPA3(\(\d+,B\))B09OPA3",                  {CPU::ADDRESS_MODES::ABB,DEC}},
                {R"B09OPA3(\(\d+\),C)B09OPA3",                  {CPU::ADDRESS_MODES::ABC,DEC}},
                {R"B09OPA3(\(@[a-z][a-z_0-9]+\))B09OPA3",       {CPU::ADDRESS_MODES::ABS,VAR}},        
                {R"B09OPA3(\(@[a-z][a-z_0-9]+,B\))B09OPA3",     {CPU::ADDRESS_MODES::ABB,VAR}},
                {R"B09OPA3(\(@[a-z][a-z_0-9]+\),C)B09OPA3",     {CPU::ADDRESS_MODES::ABC,VAR}},
                {R"B09OPA3(\([a-z][a-z_0-9]+\))B09OPA3",        {CPU::ADDRESS_MODES::ABS,LBL}},
                {R"B09OPA3(\([a-z][a-z_0-9]+,B\)B09OPA3",       {CPU::ADDRESS_MODES::ABB,LBL}},
                {R"B09OPA3(\([a-z][a-z_0-9]+\),C)B09OPA3",      {CPU::ADDRESS_MODES::ABC,LBL}},
                {R"B09OPA3([ABCXYZ],[ABCXYZ])B09OPA3",          {CPU::ADDRESS_MODES::RG2,REG}},
                {R"B09OPA3(A)B09OPA3",                          {CPU::ADDRESS_MODES::RGA,REG}},
                {R"B09OPA3(B)B09OPA3",                          {CPU::ADDRESS_MODES::RGB,REG}},
                {R"B09OPA3(C)B09OPA3",                          {CPU::ADDRESS_MODES::RGC,REG}},
                {R"B09OPA3(X)B09OPA3",                          {CPU::ADDRESS_MODES::RGX,REG}},
                {R"B09OPA3(Y)B09OPA3",                          {CPU::ADDRESS_MODES::RGY,REG}},
                {R"B09OPA3(Z)B09OPA3",                          {CPU::ADDRESS_MODES::RGZ,REG}},
            
            };


    };

}

#endif 