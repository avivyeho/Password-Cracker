
#include "Converter.h"
#include <fstream>
#include <iostream>

Converter::Converter(std::istream& in) { create(in); }

Converter::Converter(const std::string& filename) {
    std::ifstream in{filename};
    create(in);
    in.close();
}

void Converter::create(std::istream& in) {
    std::string word;
    while (in >> word) {
        auto subStrings = splitString(word);
        baseStructures(subStrings);
        digitsAndSpecialStrings(subStrings);
        caseMask(subStrings);
    }

    calculateProbabilities();
    toPCFG();
    laPlaceSmoothing();
}

void Converter::baseStructures(const std::vector<std::string>& m) {
    std::pair<std::string, std::string> baseStructure = std::make_pair("S", "");
    addVariableC("S");

    for (auto& el : m) {
        baseStructure.second += typeofchar(el[0]);
        baseStructure.second += std::to_string(el.length());
    }
    addProductions(baseStructure);
}

void Converter::digitsAndSpecialStrings(const std::vector<std::string>& m) {
    std::vector<std::string> allstrings;

    for (auto& el : m) {
        if (!isalpha(el[0])) {
            // auto substring = allCombinations(el);
            // allstrings.insert(allstrings.end(), substring.begin(), substring.end());
            allstrings.push_back(el);
        }
    }

    for (auto& el : allstrings) {
        std::string var = isdigit(el[0]) ? "D" : "S";
        var += std::to_string(el.length());
        addProductions(std::make_pair(var, el));
        addVariableC(var);
    }
}

void Converter::caseMask(const std::vector<std::string>& m) {

    std::vector<std::string> allstrings;

    for (auto& el : m) {
        std::string str;
        if (isalpha(el[0])) {
            for (auto& ch : el) {
                if (ch >= 'A' && ch <= 'Z')
                    str += "U";
                else
                    str += "L";
            }
            allstrings.push_back(str);
        }
    }

    for (auto& el : allstrings) {
        if (el.size() >= 1) {
            std::string var = "L" + std::to_string(el.length());
            addProductions(std::make_pair(var, el));
            addVariableC(var);
        }
    }
}

void Converter::calculateProbabilities() {

    for (auto& var : variables) {
        for (auto& el : productions) {
            if (el.first.first == var.first) {
                el.second /= var.second;
            }
        }
    }
}

std::vector<std::string> Converter::allCombinations(const std::string& s) {

    std::vector<std::string> vec;

    for (int i = 0; i < s.length(); i++)
        for (int len = 1; len <= s.length() - i; len++)
            vec.push_back(s.substr(i, len));

    return vec;
}

std::vector<std::string> Converter::splitString(const std::string& s) {

    std::vector<std::string> substrings;
    for (int i = 0; i < s.size(); ++i) {
        std::string substr;
        char currentType = typeofchar(s[i]);

        while (currentType == typeofchar(s[i]) and i < s.size()) {
            substr += s[i];
            ++i;
        }
        substrings.push_back(substr);
        i--;
    }
    return substrings;
}

char Converter::typeofchar(char ch) {
    if (isdigit(ch))
        return 'D';
    else if (isalpha(ch))
        return 'L';
    else
        return 'S';
}

void Converter::addProductions(const std::pair<std::string, std::string>& input) {

    auto res = productions.find(input);

    if (res != productions.end()) // FOUND
        res->second++;
    else // NOT FOUND
        productions.insert(std::make_pair(input, 1));
}

void Converter::addVariableC(const std::string& var) {

    auto res = variables.find(var);

    if (res != variables.end()) // FOUND
        res->second++;
    else // NOT FOUND
        variables.insert(std::make_pair(var, 1));
}

void Converter::laPlaceSmoothing() {
    int n = 2;

    double total_num_values = variables.find("D" + std::to_string(n))->second;
    double total_num_cat = variables.size();

    for (int i = 0; i < 99; ++i) {
        double laplace = 0;

        if (notfound(std::to_string(i))) {
            laplace = (0 + alpha) / (total_num_values + (total_num_cat * alpha));
            PCFG::addProduction("D" + std::to_string(n), {std::to_string(i)}, laplace);
            PCFG::addVariable("D" + std::to_string(n));
            PCFG::addTerminal(std::to_string(i));
        }
    }
}

void Converter::toPCFG() {

    addTerminal("U");
    addTerminal("L");
    PCFG::setStartSymbol("S");
    for (const auto& el : productions) {
        PCFG::Body body;
        if (el.first.first[0] == 'S' and el.first.first.size() > 1) {
            body = {el.first.second};
            PCFG::addTerminal(body.front());
        } else {
            body = splitVariable(el.first.second);
        }
        PCFG::addVariable(el.first.first);
        PCFG::addProduction(el.first.first, body, el.second);
    }
}

PCFG::Body Converter::splitVariable(const std::string& var) {
    PCFG::Body body;

    for (int i = 0; i < var.size(); ++i) {
        std::string sub = std::string(1, var[i]);

        while (isdigit(var[i + 1]) and i < var.size()) {
            i++;
            sub += var[i];
        }
        if (!isalpha(sub[0]) and sub != "L" and sub != "U")
            PCFG::addTerminal(sub);
        body.push_back(sub);
    }

    return body;
}

bool Converter::notfound(const std::string& terminal) {

    for (auto& it : PCFG::getProductionRules()) {
        if (it.first == "D" + std::to_string(terminal.size()) and it.second.first.front() == terminal)
            return false;
    }

    return true;
}

std::string Converter::convertToBaseStructure(const std::string& input) {
    std::string rtn;

    auto subStrings = splitString(input);

    for (auto& el : subStrings) {
        rtn += typeofchar(el[0]);
        rtn += std::to_string(el.length());
    }

    return rtn;
}
