#pragma once

#include <string>
#include <vector>
#include <algorithm>

namespace spp {

class Instruction {
public:
    // Scoped enum inside the class
    enum class Type {
        Create, Build, Debug, Run, Fmt, Tidy, Get, Unknown
    };

    Type type;
    std::vector<std::string> args; // files, flags, etc.

    // Default constructor
    Instruction() : type(Type::Unknown) {}

    // Constructor that takes argc/argv directly
    Instruction(int argc, char* argv[]) {
        parse(argc, argv);
    }

    // Parse command from argc/argv
    void parse(int argc, char* argv[]) {
        if (argc < 1) {
            type = Type::Unknown;
            return;
        }

        // Convert argv to vector<string>
        std::vector<std::string> input_args(argv, argv + argc);

        // Map first argument to Type
        std::string t = input_args[0];
        std::transform(t.begin(), t.end(), t.begin(), ::tolower);

        if (t == "create") type = Type::Create;
        else if (t == "build") type = Type::Build;
        else if (t == "debug") type = Type::Debug;
        else if (t == "run") type = Type::Run;
        else if (t == "fmt") type = Type::Fmt;
        else if (t == "tidy") type = Type::Tidy;
        else if (t == "get") type = Type::Get;
        else type = Type::Unknown;

        // Store remaining arguments
        args.clear();
        if (input_args.size() > 1) {
            args.insert(args.end(), input_args.begin() + 1, input_args.end());
        }
    }

    // Helper to convert Type -> string
    static std::string typeToString(Type t) {
        switch (t) {
            case Type::Create: return "create";
            case Type::Build:  return "build";
            case Type::Debug:  return "debug";
            case Type::Run:    return "run";
            case Type::Fmt:    return "fmt";
            case Type::Tidy:   return "tidy";
            case Type::Get:    return "get";
            default:           return "unknown";
        }
    }
};

} // namespace spp
