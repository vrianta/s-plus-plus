#pragma once


#include <iostream>
#include <cstdio>

namespace spp {

class Log {
private:
    // ANSI color codes
    inline static constexpr const char* RESET  = "\033[0m";
    inline static constexpr const char* RED    = "\033[31m";
    inline static constexpr const char* GREEN  = "\033[32m";
    inline static constexpr const char* YELLOW = "\033[33m";

public:
    // ---- println style (space-separated) ----
    template <typename... Args>
    static void infoln(Args&&... args) {
        std::cout << GREEN;
        ((std::cout << args << " "), ...);
        std::cout << RESET << std::endl;
    }

    template <typename... Args>
    static void warnln(Args&&... args) {
        std::cout << YELLOW;
        ((std::cout << args << " "), ...);
        std::cout << RESET << std::endl;
    }

    template <typename... Args>
    static void errorln(Args&&... args) {
        std::cout << RED;
        ((std::cout << args << " "), ...);
        std::cout << RESET << std::endl;
    }

    // ---- printf-style formatters ----
    template <typename... Args>
    static void infof(const char* fmt, Args... args) {
        std::printf("%s", GREEN);
        std::printf(fmt, args...);
        std::printf("%s\n", RESET);
    }

    template <typename... Args>
    static void warnf(const char* fmt, Args... args) {
        std::printf("%s", YELLOW);
        std::printf(fmt, args...);
        std::printf("%s\n", RESET);
    }

    template <typename... Args>
    static void errorf(const char* fmt, Args... args) {
        std::printf("%s", RED);
        std::printf(fmt, args...);
        std::printf("%s\n", RESET);
    }
};

} // namespace spp
