#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>

#ifndef DESIGNBYCONTRACT_H
#define DESIGNBYCONTRACT_H

class Contracts {
public:
    static void ENSURE(bool assertion, const char* what, const char* file, int line) {
        if (!assertion) {
            std::string exception = std::to_string(line) + ": " + what + "\n" + "In file: " + file;
            throw std::runtime_error(exception);
            //__assert(what, file, line);
        }
    }

    static void REQUIRE(bool assertion, const char* what, const char* file, int line) {
        if (!assertion) {
            std::string exception = "Line " + std::to_string(line) + ": " + what + "\n" + "In file: " + file;
            throw std::runtime_error(exception);
            //__assert(what, file, line);
        }
    }

    static bool fileCompare(const std::string& filename, const std::string& s) {
        std::ifstream file(filename);
        auto it = s.begin();

        bool result = true;
        char lhs, rhs;

        while (result and file.good() and it != s.end()) {
            lhs = file.get();
            rhs = *it;
            it++;

            if (lhs != rhs) {
                result = false;
            }
        }

        if (result) {
            result = (file.get() == EOF and it == s.end());
        }

        file.close();

        if (!result) {
            std::cerr << s << std::endl;
        }

        return result;
    }

    static bool file_exist(const char* name) {
        struct stat buffer;
        return (stat(name, &buffer) == 0);
    }
};

#define ENSURE(assertion, what) Contracts::ENSURE(assertion, what, __FILE__, __LINE__)
#define REQUIRE(assertion, what) Contracts::REQUIRE(assertion, what, __FILE__, __LINE__)

#endif // DESIGNBYCONTRACT_H