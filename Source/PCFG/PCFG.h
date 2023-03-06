

#ifndef TO_PCFG_H
#define TO_PCFG_H

#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>

#include "../../Libs/nlohmann/json.h"

class PCFG {
public:
    using Body = std::list<std::string>;
    using Rule = std::pair<Body, double>;

public:
    bool variableExists(const std::string& name) const;
    bool terminalExists(const std::string& name) const;

    void clear();

    void addTerminal(const std::string& name);
    void setStartSymbol(const std::string& name);
    void addVariable(const std::string& name);
    void addProduction(const std::string& head, const Body& body, double prob);

    std::string showGrammer();
    const std::string& getStartSymbol() const;
    const std::multimap<std::string, Rule>& getProductionRules() const;

    PCFG toCNF2();
    void toCNF();

    /**
     * Attempts to import a PCFG from a json file
     *
     * @param in    Input stream with valid json
     * @param err   Output stream for potential errors
     *
     * @return      True if the import was successful and false otherwise
     */
    bool importJSON(std::istream& in, std::ostream& err = std::cerr);

    void exportJSON(std::ostream& out) const;

    double CYK(const std::vector<std::string>& basestructure);

private:
    void importVariables(const nlohmann::json& variables);
    void importTerminals(const nlohmann::json& terminals);
    void importProductions(const nlohmann::json& productions);

    static std::string terminalSplit(std::map<std::string, std::string>& map, const std::string& b);

    double hasProduction(const std::string& variable, const Body& body);

private:
    std::set<std::string> variables; // V
    std::set<std::string> terminals; // Σ
    std::string startSymbol;
    std::multimap<std::string, Rule> productionRules;
    static std::string getChar(const std::string& c);
};

std::string to_string(const PCFG::Body& body);
std::string to_string(const PCFG::Rule& body);

std::ostream& operator<<(std::ostream& out, const PCFG::Body& body);
std::ostream& operator<<(std::ostream& out, const PCFG::Rule& Production);

#endif // TO_PCFG_H
