#ifndef __TESTS_H_
#define __TESTS_H_
#include "common.h"

namespace sh {

    

    enum class TestComponentType : u8 {
        INVALID,
        REGISTER,
        MEMORY,
        FLAG,
    
    };

    
    struct TestComponent {
        
        TestComponentType type;
        u16 index;
        u16 value;
        TestComponent( TestComponentType _type, u16 _index, u16 _value) : 
            type(_type), index(_index), value(_value) {

        }



    
    };
    

    struct Test {
        std::string name;
        std::vector<TestComponent> expected;
        std::vector<TestComponent> setup;
        std::string code;
        Test() {
        }
        Test(const std::string& _name, std::vector<TestComponent> _setup, std::vector<TestComponent> _expected, const std::string& _code) 
            : setup(_setup), expected(_expected), code(_code), name(_name) {

            
        }
        
        
        void Load(std::string& _code);
        void Run();
    
    };

    class TestRunner {
        public:
            std::vector<Test> tests;

            TestRunner();
            void Load();
            void LoadFromFile(const std::string& path);
            void Run();
            TestComponent ParseTestComponent(const std::string& line);
    };



}

#endif
