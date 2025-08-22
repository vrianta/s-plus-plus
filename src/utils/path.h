/**
 * File to build paths and do path related modifications 
 */

 #pragma once

/**
 * path.h
 * 
 * File to build paths and do path-related modifications
 */

#include <string>
#include <vector>

namespace spp {

class Path {
public:
    enum class Start {
        Current, 
        Root     
    };

    /**
     * Joins multiple path components into a single path string.
     * The start_path_type enum determines the initial directory.
     * @param -> Start in a emun with (Current, Root)
     * @param -> Args is all the paths one by one
     */
    template<typename... Args>
    static std::string build(Start start_path_type, Args... args) {
        std::vector<std::string> paths{args...};
        std::string full;

        switch (start_path_type) {
            case Start::Current:
                full = getCurrentPath();
                break;
            case Start::Root:
                full = getRootPath();
                break;
        }

        for (const auto& p : paths) {
            if (!full.empty() && full.back() != PATH_SEP) {
                full += PATH_SEP;
            }
            full += p;
        }

        return full;
    }

private:
#ifdef _WIN32
    inline static const char PATH_SEP = '\\';
    inline static const std::string ROOT = "C:\\";
#else
    inline static const char PATH_SEP = '/';
    inline static const std::string ROOT = "/";
#endif

    inline static const std::string CURRENT = std::string(".") + PATH_SEP;

    static std::string getCurrentPath() { return CURRENT; }
    static std::string getRootPath() { return ROOT; }
};

} // namespace spp
