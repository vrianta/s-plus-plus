#pragma once


#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace spp {

class buildConfig {
private:

  std::string build_file = ".build";

  // Helper to trim whitespace
  static std::string trim(const std::string &s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
  }

public:


  int cxx_version = 20;
  std::string build_dir = "bin/build";
  std::string debug_dir = "build/debug";
  std::string src_dir = "src";
  std::string optimisation_lavel = "o1";
  std::string compiler = "clang++";
  std::string name = "app"; // app name


  void parse_build_file(const std::string &filepath = ".build") {
    std::ifstream file(filepath);
    if (!file.is_open()) {
      return;
    }

    std::string line;
    while (std::getline(file, line)) {
      line = trim(line);

      // Skip empty lines or comments
      if (line.empty() || line[0] == '#')
        continue;

      size_t pos = line.find('=');
      if (pos == std::string::npos)
        continue;

      std::string key = trim(line.substr(0, pos));
      std::string value = trim(line.substr(pos + 1));

      // Assign to member variables
      if (key == "cxx_version")
        cxx_version = std::stoi(value);
      else if (key == "build_dir")
        build_dir = value;
      else if (key == "debug_dir")
        debug_dir = value;
      else if (key == "src_dir")
        src_dir = value;
      else if (key == "optimisation_lavel")
        optimisation_lavel = value;
      else if (key == "name")
        name = value;
      else {
        std::cerr << "Unknown config key: " << key << "\n";
      }
    }
  }

  // Getters
  int getCxxVersion() const { return cxx_version; }
  const std::string &getBuildDir() const { return build_dir; }
  const std::string &getDebugDir() const { return debug_dir; }
  const std::string &getSrcDir() const { return src_dir; }
  const std::string &getOptimisationLevel() const { return optimisation_lavel; }
  const std::string &getOutput() const { return name; }
} BuildConfig;

} // namespace spp
