#include "common.h"

namespace sh {

    TestRunner::TestRunner() {
        

    }

    TestComponent TestRunner::ParseTestComponent(const std::string& line) {
        TestComponent test(TestComponentType::INVALID, 0,0);
        std::string ln = std::string(line);
        ln = Funcs::ToUpper(ln);
        if (ln.starts_with("MEM")) {
            ln = ln.substr(4);
            auto v = Funcs::SplitString(ln,'=');
            test.index = Funcs::GetInt(v[0]);
            test.value = Funcs::GetInt(v[1]);
            test.type = TestComponentType::MEMORY;
        } else if (ln.starts_with("FLAG:")) {
            char flag = ln[5];

            test.type = TestComponentType::FLAG;
            test.index = 


        } else if (Funcs::CharInList(line[0], REGNAMES)) {
            test.index = CPU::REGISTER_NAMES.at(line[0]);
            ln = ln.substr(2);
            test.value = Funcs::GetInt(ln);
            test.type = TestComponentType::REGISTER;
        }



        return test;


    }
    void TestRunner::LoadFromFile(const std::string& path) {
        std::ifstream file(path);

        if (file.is_open()) {
            std::string line;

            Test* test = nullptr;
            int idx = 0;
            bool reading_results = false;
            bool reading_code = false;
            bool reading_setup = false;
            while (std::getline(file, line, '\n')) {

                if (line.length() == 0) continue;
                Funcs::Trim(line);
                Funcs::Squeeze(line);
                if (line.starts_with("//TEST")) {

                    if (test != nullptr) {
                        tests.push_back(*test);
                        test = nullptr;
                    }

                    test = new Test();
                    std::string name = "";
                    int pos = line.find_first_of(' ');
                    if (pos != std::string::npos) {
                        test->name = std::format("{}[{}]", line.substr(pos+1), idx);
                    } else {
                        test->name = std::format("TEST[{}]", idx);
                    }
                    idx += 1;
                    reading_setup = true;
                    
                } else if (line.starts_with("//CODE")) {
                    reading_code = true;
                    reading_results = false;
                    reading_setup = false;
                } else if (line.starts_with("//RESULT")) {
                    reading_results = true;
                    reading_code = false;
                    reading_setup = false;
                } else if (reading_setup) {
                    TestComponent t = ParseTestComponent(line);
                    if (t.type != TestComponentType::INVALID) {
                        test->setup.push_back(t);

                    } else {
                        throw std::runtime_error("Invalid Test Component" + line);
                    }
                } else if (reading_code) {
                    test->code += "\n" + line;
                } else if (reading_results) {
                    TestComponent t = ParseTestComponent(line);
                    if (t.type != TestComponentType::INVALID) {
                        test->expected.push_back(t);

                    } else {
                        throw std::runtime_error("Invalid Test Component" + line);
                    }                    
                }
            }

        } else {
            //fucken die
            throw std::runtime_error("File not open");
        }
        
    }

    void TestRunner::Load() {
        // tests.push_back(
        //     {
        //         "ADDC B",
        //         {{TestComponentType::REGISTER, CPU::REG_INDEX::RIA, 1}, {TestComponentType::REGISTER, CPU::REG_INDEX::RIB, 2}},
        //         {{TestComponentType::REGISTER, CPU::REG_INDEX::RIA, 3}, {TestComponentType::REGISTER, CPU::REG_INDEX::RIB, 2}},
        //         "ADDC B"
        //     }
        // );

        // tests.push_back(
        //     {
        //         "SUBC B",
        //         {{TestComponentType::REGISTER, CPU::REG_INDEX::RIA, 3}, {TestComponentType::REGISTER, CPU::REG_INDEX::RIB, 2}},
        //         {{TestComponentType::REGISTER, CPU::REG_INDEX::RIA, 1}, {TestComponentType::REGISTER, CPU::REG_INDEX::RIB, 2}},
        //         "FCST\nSUBC B"
        //     }
        // );        
    }

    void TestRunner::Run() {
        bool ok = true;
        int idx = 0;
        for (auto& test : tests) {
            Assembler ass;
            CPU cpu;

            std::string fn = std::string(test.name) ;
            Funcs::MakeSafeFileName(fn);
            
            std::cout << "Running Test [" << test.name << "]\n";
            std::string output_name = std::format("execs\\tests\\test-{}-{}.mem", fn,idx);
            idx += 1;
            ok = ass.ParseFromText(test.code, output_name);
            

            if (ok) {
                ok = cpu.LoadProgram(output_name);
            }

            if (ok) {
                for (auto& setup : test.setup) {
                    if (setup.type == TestComponentType::MEMORY) {
                        cpu.memory[setup.index] = setup.value;
                    } else if (setup.type == TestComponentType::REGISTER) {
                        *cpu.regPointers[setup.index] = setup.value;
                    }
                } 
            }

            if (ok) {
                int ticks = 0;
                while (!cpu.flags.HALT) {
                    cpu.Tick();
                    ticks += 1;
                }

                std::cout << "\t\tTicks: " << ticks << "\n";
            }
            if (ok) {
                for (auto& expected : test.expected) {
                    if (expected.type == TestComponentType::MEMORY) {
                        ok = cpu.memory[expected.index] == expected.value;

                        if (!ok) {
                            std::cout << "Address " << expected.index << " does not = " << expected.value << "\n" <<
                                "\tValue = " << cpu.memory[expected.index] << "\n";

                            cpu.PrintNonZeroMemory();
                            
                        }


                        
                    } else if (expected.type == TestComponentType::REGISTER) {
                        ok = *cpu.regPointers[expected.index] == expected.value;
                        if (!ok) {
                            std::cout << "Regiser " << CPU::REGISTER_NAMES_BY_INDEX.at(expected.index) << " does not = " << expected.value << "\n" <<
                                "\tValue = " << *cpu.regPointers[expected.index] << "\n";

                            cpu.PrintNonZeroMemory();
                        }                        
                    }          

                    if (!ok) break;          
                }
            }

            if (!ok) {
                std::cout << "test failed " << test.name << "\n";
                break;
            } else {
                std::cout << "test passed\n";
            }
            
        
        }
    }

    // void TestRunner::Load() {
    //     const auto path = std::filesystem::path("tests");

    //     namespace fs = std::filesystem;

    //     for (const auto& item : fs::directory_iterator(path)) {
    //         const auto file_name = item.path().filename().string();
    //         if (item.is_directory()) {
    //             //do nothing
    //         } else if (item.is_regular_file()) {
    //             std::cout << "file: " << file_name << "\n";
    //         }
    //      }

    // }


}