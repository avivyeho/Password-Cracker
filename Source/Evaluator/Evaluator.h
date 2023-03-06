

#ifndef MB_TO_GROEP_EVALUATOR_H
#define MB_TO_GROEP_EVALUATOR_H

#include "../BaseConversion/Converter.h"
#include "../GeneticAlgorithm/GeneticGenerator.h"
#include "../PCFG/PCFG.h"
#include <iostream>
#include <map>
#include <set>
#include <stack>
#include <string>

class Evaluator {
public:
    // checks for use of alpha/digit/special characters
    static double bruteforceCheck(const std::string& input);

    /// Load a list of common words
    void loadWords(std::istream& in);
    /// Load a list of common passwords
    void loadPasswords(std::istream& in);

    double weirEvaluate(const std::string& password);
    double modifiedEvaluate(const std::string& password);
    double hybridEvaluate(const std::string& password);
    bool loadWeir(const std::string& filename);
    bool loadModified(const std::string& filename);

    /**
     * Checks for use common words
     *
     * @param input     String to be tested
     * @return          The number of characters that are part of a common word (eg: 'qmy_house' -> 2+5)
     */
    std::size_t commonWordCheck(const std::string& input) const;

    /**
     * Checks for common passwords.
     *
     * @param input     String to be tested
     * @return          True if the input occurs exactly in the loaded password list and false otherwise
     */
    bool commonPasswordCheck(const std::string& input) const;

    /// krijgt een password string, checkt de output van Weir CYK -> output kans
    /// idem, met modified CYK

    static std::vector<std::string> getWeirVector(const std::string& s);
    static std::vector<std::string> getModifiedVector(const std::string& s);

private:
    // find the position and size of the longest common word in the input string, if such a word exists
    bool findLongestWord(const std::string& input, std::size_t& index, std::size_t& length) const;

private:
    std::unique_ptr<PCFG> weirPCFG;
    PCFG* modifiedPCFG = nullptr;

    std::set<std::string> passwords;

    // sorted in descending order of length
    std::map<std::size_t, std::set<std::string>, std::greater<>> words;
};

#endif // MB_TO_GROEP_EVALUATOR_H
