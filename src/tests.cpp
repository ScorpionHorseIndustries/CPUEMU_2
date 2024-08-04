#include "common.h"

namespace sh {

    TestRunner::TestRunner() {
        

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
                        test->name = line.substr(pos+1);
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

                } else if (reading_code) {
                    test->code += "\n" + line;
                } else if (reading_results) {
                }
            }

        } else {
            //fucken die
        }
        
    }

    void TestRunner::Load() {
        tests.push_back(
            {
                "ADDC B",
                {{TestResultType::REGISTER, CPU::REG_INDEX::RIA, 1}, {TestResultType::REGISTER, CPU::REG_INDEX::RIB, 2}},
                {{TestResultType::REGISTER, CPU::REG_INDEX::RIA, 3}, {TestResultType::REGISTER, CPU::REG_INDEX::RIB, 2}},
                "ADDC B"
            }
        );

        tests.push_back(
            {
                "SUBC B",
                {{TestResultType::REGISTER, CPU::REG_INDEX::RIA, 3}, {TestResultType::REGISTER, CPU::REG_INDEX::RIB, 2}},
                {{TestResultType::REGISTER, CPU::REG_INDEX::RIA, 1}, {TestResultType::REGISTER, CPU::REG_INDEX::RIB, 2}},
                "FCST\nSUBC B"
            }
        );        
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
                    if (setup.type == TestResultType::MEMORY) {
                        cpu.memory[setup.index] = setup.value;
                    } else if (setup.type == TestResultType::REGISTER) {
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
                    if (expected.type == TestResultType::MEMORY) {
                        ok = cpu.memory[expected.index] == expected.value;

                        if (!ok) {
                            std::cout << "Address " << expected.index << " does not = " << expected.value << "\n" <<
                                "\tValue = " << cpu.memory[expected.index] << "\n";

                            cpu.PrintNonZeroMemory();
                            
                        }


                        
                    } else if (expected.type == TestResultType::REGISTER) {
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