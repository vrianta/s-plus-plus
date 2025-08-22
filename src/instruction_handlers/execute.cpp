#include "build_config.cpp"
#include "instruction.cpp"

#include "../utils/log.h"
#include "../utils/path.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace spp {

class Execute {
private:
  Instruction &instruction;

#ifdef _WIN32
  std::string cmd = "where " + BuildConfig.compiler + " >nul 2>&1";
#else
  std::string cmd = "which " + BuildConfig.compiler + " >/dev/null 2>&1";
#endif

public:
  // Constructor initializes the reference in the initializer list
  Execute(Instruction &_instruction)
      : instruction(_instruction) // ✅ initialize reference here
  {
    run();
  }

private:
  // Example method to use instruction
  void run() {
    switch (instruction.type) {
    case Instruction::Type::Build:
      build();
      break;

    // TODO: handle other instruction types
    default:
      std::cout << "Instruction not implemented.\n";
      break;
    }
  }

  bool isCompilerAvailable(const std::string &compiler) {

    int result = std::system(cmd.c_str());
    return result == 0;
  }

  std::vector<std::string> collectFolders(const std::string &path) {
    std::vector<std::string> folders;
    for (auto &p : std::filesystem::recursive_directory_iterator(path)) {
      if (p.is_directory()) {
        folders.push_back(p.path().string());
      }
    }
    return folders;
  }

  std::vector<std::string> collectCppFolders(const std::string &path) {
    std::vector<std::string> folders;
    for (auto &p : std::filesystem::recursive_directory_iterator(path)) {
      if (p.is_directory()) {
        bool hasCppFile = false;
        // Check if the directory contains any .cpp files
        for (auto &file : std::filesystem::directory_iterator(p.path())) {
          if (file.is_regular_file() && file.path().extension() == ".cpp") {
            hasCppFile = true;
            break;
          }
        }
        if (hasCppFile) {
          folders.push_back(p.path().string());
        }
      }
    }
    return folders;
  }

  // Collect all .cpp files recursively
  std::vector<std::string> collectCppFiles(const std::string &path) {

    if (!std::filesystem::exists(path)) {
      std::cerr << "Path does not exist: " << path << "\n";
      return {};
    }

    std::vector<std::string> files;

    for (auto &p : std::filesystem::recursive_directory_iterator(path)) {
      if (p.is_regular_file() && p.path().extension() == ".cpp") {

        bool hasHeader = false;

        for (auto &p_j : std::filesystem::recursive_directory_iterator(path)) {
          std::string header_file_name = p.path().stem().string() + ".h";
          std::string header_file_name_hpp = p.path().stem().string() + ".hpp";
          if (header_file_name == p_j.path().filename().string()) {
            spp::Log::infoln("Found Header File:", p_j.path().string());
            hasHeader = true;
            break;
          }else if (header_file_name_hpp == p_j.path().filename().string()) {
            spp::Log::infoln("Found Header File:", p_j.path().string());
            hasHeader = true;
            break;
          }
        }

        if (!hasHeader) {
          std::cerr << "Warning: No header file found for "
                    << p.path().filename()
                    << " Expecting the file has been included directly"
                    << "\n";
          continue;
        }else {
            files.push_back(p.path().string());
            spp::Log::infoln("Found Source File:", p.path().string());
        }
        // std::cout << "Found Source File" << "\n";
      }
    }
    return files;
  }

  void build() {
    // Check compiler
    if (!isCompilerAvailable(BuildConfig.compiler)) {
      std::cerr << "Compiler not found: " << BuildConfig.compiler << "\n";
      return;
    }

    // Ensure build directory exists
    if (!std::filesystem::exists(BuildConfig.build_dir)) {
      std::filesystem::create_directories(BuildConfig.build_dir);
    } else {
      // Clean old files but keep lib folder
      for (auto &p :
           std::filesystem::directory_iterator(BuildConfig.build_dir)) {
        if (p.path().filename() != "lib") {
          std::filesystem::remove_all(p.path());
        }
      }
    }

    // --- Build libraries ---
    std::string lib_dir =
        spp::Path::build(spp::Path::Start::Current, BuildConfig.src_dir, "lib");
    std::string build_lib_dir = spp::Path::build(spp::Path::Start::Current,
                                                 BuildConfig.build_dir, "lib");
    if (!std::filesystem::exists(build_lib_dir)) {
      std::filesystem::create_directories(build_lib_dir);
    }

    bool lib_built = false; // will be used to check if any library was built
    // Build each library in its own folder
    if (std::filesystem::exists(lib_dir)) {
      for (auto &folder : std::filesystem::directory_iterator(lib_dir)) {

        if (folder.is_directory()) {
          std::vector<std::string> lib_sources;
          for (auto &file :
               std::filesystem::recursive_directory_iterator(folder.path())) {
            if (file.path().extension() == ".cpp") {
              lib_sources.push_back(file.path().string());
            }
          }
          if (!lib_sources.empty()) {
            std::string lib_name = folder.path().filename().string();
            std::string cmd = BuildConfig.compiler + " -shared -fPIC ";
            for (auto &src : lib_sources)
              cmd += src + " ";
            cmd += "-o " + build_lib_dir + "/lib" + lib_name + ".so";
            std::cout << "Building library: " << cmd << "\n";
            int result = std::system(cmd.c_str());
            if (result != 0)
              std::cerr << "Library build failed: " << lib_name << "\n";
          }
        }
      }
    }

    // Collect all folders recursively for main application
    std::vector<std::string> files = collectCppFiles(BuildConfig.src_dir);
    if (files.empty()) {
      spp::Log::errorln(
          "No source folders found in", BuildConfig.src_dir,
          "Make sure the main.cpp or the entry file must have a header file",
          "\nAnd the Header file must be in the same directory as the source file and with the same name with .h extension",
          "using .hpp extension is stupid",
          "\n");
      exit(-1);
      return;
    }

    // Build main application using folder paths
    std::string cmd = BuildConfig.compiler + " ";

    // addig the src directory seperately to include its cpp files
    // cmd += spp::Path::build(spp::Path::Start::Current, BuildConfig.src_dir,
    //                         "*.cpp ");

    for (auto &file : files) {
      cmd += spp::Path::build(spp::Path::Start::Current, file) + " ";
    }

    // Add C++ version flag
    cmd += "-std=c++" + std::to_string(BuildConfig.cxx_version) + " ";

    if (lib_built) {
      cmd += "-L " + build_lib_dir + " "; // add library path
    }

    for (auto &lib_file : std::filesystem::directory_iterator(build_lib_dir)) {
      if (lib_file.path().extension() == ".so") {
        std::string lib_name = lib_file.path().filename().string();
        lib_name = lib_name.substr(
            3, lib_name.size() - 6); // remove 'lib' prefix and '.so' suffix
        cmd += "-l" + lib_name + " ";
      }
    }
    cmd += "-o " + spp::Path::build(spp::Path::Start::Current,
                                    BuildConfig.build_dir, BuildConfig.name);

    std::cout << "Running: " << cmd << "\n";
    int result = std::system(cmd.c_str());
    if (result != 0)
      std::cerr << "Build failed!\n";
    else
      std::cout << "Build succeeded!\n";
  }
};

} // namespace spp
