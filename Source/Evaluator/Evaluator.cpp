

#include "Evaluator.h"
#include "../Cracker/Cracker.h"
#include <cmath>
#include <vector>

double Evaluator::bruteforceCheck(const std::string& input) {
    int alpha = 0;
    int digit = 0;
    int special = 0;

    for (char c : input) {
        if (isalpha(c)) {
            alpha++;
        } else if (isdigit(c)) {
            digit++;
        } else {
            special++;
        }
    }
    return pow(1.0 / 52, alpha) * pow(1.0 / 10, digit) * pow(1.0 / 33, special);
}

void Evaluator::loadWords(std::istream& in) {
    std::string s;
    while (in >> s) {
        if (words.find(s.size()) == words.end()) {
            words[s.size()] = std::set<std::string>();
        }
        words[s.size()].insert(s);
    }
}

void Evaluator::loadPasswords(std::istream& in) {
    std::string s;
    while (in >> s) {
        passwords.insert(s);
    }
}

std::size_t Evaluator::commonWordCheck(const std::string& input) const {
    std::size_t index = std::string::npos;
    std::size_t length = 0;

    if (findLongestWord(input, index, length)) {
        std::size_t left = commonWordCheck(input.substr(0, index));
        std::size_t right = commonWordCheck(input.substr(index + length, input.size() - length - index));
        return length + left + right;
    } else
        return 0;
}

bool Evaluator::commonPasswordCheck(const std::string& input) const { return passwords.find(input) != passwords.end(); }

bool Evaluator::findLongestWord(const std::string& input, size_t& index, size_t& length) const {
    if (input.empty())
        return false;

    // start with words of the same length as input, then shorter and shorter sizes
    for (auto it = words.find(input.size()); it != words.end(); ++it) {

        // check if the input contains any common word of the current length
        for (const auto& s : it->second) {
            std::size_t i = input.find(s);
            if (i != std::string::npos) {
                length = s.size();
                index = i;
                return true;
            }
        }
    }
    return false;
}
double Evaluator::weirEvaluate(const std::string& password) {
    ENSURE(weirPCFG != nullptr, "WeirPCFG not loaded.");
    return std::exp(weirPCFG->CYK(getWeirVector(password)));
}

double Evaluator::modifiedEvaluate(const std::string& password) {
    ENSURE(modifiedPCFG != nullptr, "ModifiedPCFG not loaded.");

    return std::exp(modifiedPCFG->CYK(getModifiedVector(password)));
}

double Evaluator::hybridEvaluate(const std::string &password) {
    ENSURE(weirPCFG != nullptr, "WeirPCFG not loaded.");
    ENSURE(modifiedPCFG != nullptr, "ModifiedPCFG not loaded.");
    ENSURE(!words.empty(), "Dictionary not loaded.");

    double final_score;

    double cyk = weirPCFG->CYK(getWeirVector(password));

    if ( cyk <= 0 ){
        cout << "Password base structure found using Weir method with CYK value of " << std::exp(cyk) << endl;
        final_score = -1*cyk;
    }else{
        cyk = modifiedPCFG->CYK(getModifiedVector(password));
        cout << "Password not found using Weir method" << endl;
        cout << "The modifiedPCFG returns a cyk value of " << std::exp(cyk) << endl;
        final_score = -0.5*cyk;
    }

    int found = commonWordCheck(password);
    double ratioD = (double)found / (double)password.size();
    cout << ratioD * 100 << "% of the password consists of dictionary words" << endl;

    return final_score * (1 - ratioD);
}

bool Evaluator::loadWeir(const std::string& filename) {
    std::ifstream file(filename);
    PCFG p;
    p.importJSON(file);
    weirPCFG = std::make_unique<PCFG>(p.toCNF2());
    return true;
}

bool Evaluator::loadModified(const std::string& filename) {
    std::ifstream file(filename);
    modifiedPCFG = new PCFG();
    modifiedPCFG->importJSON(file);
    return true;
}

std::vector<std::string> Evaluator::getWeirVector(const std::string& s) {
    std::vector<std::string> inputString;
    std::string nonAlpha;
    for (auto i = 0; i != s.size(); i++) {
        if (i >= s.size())
            break;
        std::string evalString;
        evalString += s[i];
        if (isdigit(s[i])) {
            while (isdigit(s[i])) {
                nonAlpha.push_back(s[i]);
                i++;
                if (i >= s.size())
                    break;
            }
            inputString.push_back(nonAlpha);
            nonAlpha.clear();
            i--;
        } else if (isalpha(s[i])) {
            if (islower(s[i]))
                inputString.emplace_back("L");
            else
                inputString.emplace_back("U");
        } else {
            while (!isalnum(s[i])) {
                nonAlpha.push_back(s[i]);
                i++;
                if (i >= s.size())
                    break;
            }
            inputString.push_back(nonAlpha);
            nonAlpha.clear();
            i--;
        }
    }
    return inputString;
}

std::vector<std::string> Evaluator::getModifiedVector(const string& s) {
    vector<std::string> inputString;
    for (auto& c : s) {
        if (isdigit(c)) {
            inputString.emplace_back("d");
        } else if (isalpha(c)) {
            inputString.emplace_back("l");
        } else
            inputString.emplace_back("s");
    }
    return inputString;
}