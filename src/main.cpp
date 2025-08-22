#include "instruction_handlers/instruction.cpp"
#include "instruction_handlers/build_config.cpp"
#include "instruction_handlers/execute.cpp"
#include <iostream>

int main(int argc, char* argv[]) {
    // Skip argv[0] which is the program name
    if (argc < 2) {
        std::cout << "No command provided.\n";
        return 1;
    }

    // persing .build file
    spp::BuildConfig.parse_build_file();


    spp::Instruction step(argc - 1, argv + 1);

    
    auto exe = spp::Execute(step);

    return 0;
}
