#ifndef __TESTS_H_
#define __TESTS_H_
#include "common.h"

namespace sh {

    

    enum class TestResultType : u8 {
        INVALID,
        REGISTER,
        MEMORY
    
    };

    
    struct TestResult {
        
        TestResultType type;
        u16 index;
        u16 value;
        TestResult( TestResultType _type, u16 _index, u16 _value) : 
            type(_type), index(_index), value(_value) {

        }



    
    };
    typedef TestResult TestSetup;

    struct Test {
        std::string name;
        std::vector<TestResult> expected;
        std::vector<TestSetup> setup;
        std::string code;
        Test() {
        }
        Test(const std::string& _name, std::vector<TestSetup> _setup, std::vector<TestResult> _expected, const std::string& _code) 
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
    };



}

#endif
