

#ifndef TO_CONVERTER_H
#define TO_CONVERTER_H
#include "../PCFG/PCFG.h"
#include <map>
#include <set>
#include <string>
#include <vector>

class Converter : public PCFG {
public:
    Converter(std::istream& in);
    Converter(const std::string& filename = "../Data/passwordFile01.txt");
    static std::string convertToBaseStructure(const std::string& input);

private:
    void create(std::istream& in);

    void addProductions(const std::pair<std::string, std::string>&);
    void addVariableC(const std::string&);
    void baseStructures(const std::vector<std::string>& m);
    void caseMask(const std::vector<std::string>& m);
    void calculateProbabilities();
    void digitsAndSpecialStrings(const std::vector<std::string>& m);
    static std::vector<std::string> allCombinations(const std::string&);
    static std::vector<std::string> splitString(const std::string& s);
    static char typeofchar(char ch);

    void laPlaceSmoothing();
    bool notfound(const std::string&);
    double alpha = 0.5; // laplace

    std::multimap<std::pair<std::string, std::string>, double> productions;
    std::map<std::string, double> variables;

    void toPCFG();
    PCFG::Body splitVariable(const std::string& var);
};

#endif // TO_CONVERTER_H
