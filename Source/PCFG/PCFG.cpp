

#include "PCFG.h"

#include "../../Libs/DesignByContract.h"

#include <utility>

bool PCFG::variableExists(const std::string& name) const {
    return std::find(variables.begin(), variables.end(), name) != variables.end();
}

bool PCFG::terminalExists(const std::string& name) const {
    return std::find(terminals.begin(), terminals.end(), name) != terminals.end();
}

void PCFG::addTerminal(const std::string& name) { terminals.insert(name); }

void PCFG::addVariable(const std::string& name) { variables.insert(name); }

void PCFG::addProduction(const std::string& head, const Body& body, double prob) {
    productionRules.insert(std::make_pair(head, std::make_pair(body, prob)));
}

void PCFG::setStartSymbol(const std::string& name) { startSymbol = name; }

std::string PCFG::showGrammer() {

    std::string output = "PCFG \n variable characters: \n";

    for (auto& s : variables) {
        output += " " + s;
    }

    output += "\n terminal characters: \n";
    for (auto& v : terminals) {
        output += " " + v;
    }

    output += "\n start symbol: \n " + startSymbol;

    output += "\n production rules: \n";
    for (auto& prod : productionRules) {
        output += " " + prod.first + " --> ";

        for (auto& it : prod.second.first) {
            output += " " + it;
        }

        output += "   " + std::to_string(prod.second.second) + "\n";
    }

    return output;
}

const std::string& PCFG::getStartSymbol() const { return startSymbol; }

const std::multimap<std::string, PCFG::Rule>& PCFG::getProductionRules() const { return productionRules; }

PCFG PCFG::toCNF2() {
    PCFG result;

    // add all terminals and symbols
    result.terminals = terminals;
    result.variables = variables;
    result.setStartSymbol(getStartSymbol());

    std::size_t counter = 0; // used for extra unique variable names

    std::queue<std::pair<std::string, Rule>> unitProductions; // used to store unit productions
    std::queue<std::pair<std::string, Rule>> XProductions;    // used to store productions that still need splitting
    std::map<std::string, std::string> terminalMap;           // used to store variables that produce 1 terminal

    // process all productions
    for (auto& p : productionRules) {
        const std::string& head = p.first;
        auto& body = p.second.first;
        double prob = p.second.second;

        if (body.size() == 1) {
            if (terminalExists(body.front()))
                result.addProduction(head, body, prob); // CNF yay! (this is the case where head is L1 or D<n> or S<n>)
            else
                unitProductions.push(p); // unit production (this is the case where head is S)
        } else
            XProductions.push({head, p.second}); // probably not CNF -> add to split queue
    }

    // keep splitting until all have been split
    while (!XProductions.empty()) {
        auto p = XProductions.front();
        XProductions.pop();

        const auto& head = p.first;
        const auto& body = p.second.first;
        double prob = p.second.second;

        // add the production "head -> b1 b2"

        // terminalSplit create or uses variables that point to 1 terminal
        // this way we can guarantee that b1 and b2 are variables

        std::string b1 = terminalExists(body.front()) ? terminalSplit(terminalMap, body.front()) : body.front();
        std::string b2 = body.back();

        if (p.second.first.size() == 2) {
            if (terminalExists(b2))
                b2 = terminalSplit(terminalMap, b2);
        } else {
            // the body contains too many variables to split in two, so an extra split is needed
            b2 = "X" + std::to_string(counter);
            ++counter;
            XProductions.push({b2, {{std::next(body.begin()), body.end()}, 1.0}});
        }
        result.addProduction(head, {b1, b2}, prob);
        result.addVariable(head);
    }

    // add back unit productions
    // this is done at the end so the direct substitution doesn't create double work
    while (!unitProductions.empty()) {
        auto p = unitProductions.front();
        unitProductions.pop();

        double prob = p.second.second;

        // this is the single variable in the production
        const std::string& var = p.second.first.front();

        // find it and add its bodies
        auto itpair = result.productionRules.equal_range(var);
        for (auto it = itpair.first; it != itpair.second; ++it) {
            const auto& b = it->second.first;
            const auto& pr = it->second.second;
            result.addProduction(p.first, b, pr * prob);
        }
    }

    // finally add terminal productions
    // these are the variables that produce 1 terminal that were created by terminalSplit()
    for (const auto& mapItem : terminalMap) {
        result.addVariable(mapItem.second);
        result.addProduction(mapItem.second, {mapItem.first}, 1.0);
    }

    return result;
}

std::string PCFG::terminalSplit(std::map<std::string, std::string>& map, const std::string& b) {
    std::string result;
    auto it = map.find(b);
    if (it != map.end()) {
        result = it->second;
    } else {
        result = "T" + b;
        map[b] = result;
    }

    return result;
}

void PCFG::toCNF() {

    addProduction("A", {"L"}, 1.0);
    addProduction("B", {"U"}, 1.0);

    int count = 0;

    for (auto& prod : productionRules) {

        if (prod.first == "L2") {

            prod.second.first = {getChar(prod.second.first.front()), getChar(prod.second.first.back())};

        } else if (prod.first[0] == 'L' and prod.second.first.size() > 1) {
            Body res = prod.second.first;
            prod.second.first = {getChar(prod.second.first.front()), "X" + std::to_string(count)};
            res.pop_front();

            while (res.size() > 2) {
                addProduction("X" + std::to_string(count - 1), {getChar(res.front()), "X" + std::to_string(count + 1)},
                              1.0);
                res.pop_front();
                addVariable("X" + std::to_string(count - 1));
            }

            addProduction("X" + std::to_string(count), {getChar(res.front()), getChar(res.back())}, 1.0);
            addVariable("X" + std::to_string(count - 1));

            count += 1;

        } else if (prod.first == "S" and prod.second.first.size() > 2) {

            Body res = prod.second.first;
            prod.second.first = {res.front(), "X" + std::to_string(count)};
            res.pop_front();

            while (res.size() > 2) {
                addProduction("X" + std::to_string(count - 1), {res.front(), "X" + std::to_string(count + 1)}, 1.0);
                res.pop_front();
                count++;
                addVariable("X" + std::to_string(count - 1));
            }

            addProduction("X" + std::to_string(count), {res.front(), res.back()}, 1.0);
            addVariable("X" + std::to_string(count));

            count += 1;

        } else if (prod.first == "S" and prod.second.first.size() == 1) {

            int counttimes = 0;

            for (auto& it : productionRules) {

                if (it.first == prod.second.first.front()) {
                    if (counttimes == 0) {
                        counttimes++;
                        prod.second.first = it.second.first;
                        prod.second.second *= it.second.second;
                    } else
                        addProduction(prod.first, it.second.first, prod.second.second * it.second.second);
                }
            }
        }
    }
}

std::string PCFG::getChar(const std::string& c) { return c == "L" ? "A" : "B"; }

bool PCFG::importJSON(std::istream& in, std::ostream& err) {
    nlohmann::json j;
    bool success = true;

    in >> j;

    try {
        importVariables(j["Variables"]);
        importTerminals(j["Terminals"]);
        importProductions(j["Productions"]);

        // set starting symbol
        auto s = j["Start"].get<std::string>();
        if (!variableExists(s))
            throw(std::runtime_error("Starting symbol `" + s + "` is not a valid variable."));

        setStartSymbol(s);
    } catch (const std::runtime_error& e) {
        err << e.what() << std::endl;
        success = false;
        clear();
    }

    return success;
}

void PCFG::importVariables(const nlohmann::json& vars) {
    for (const auto& v : vars) {
        auto name = v.get<std::string>();

        if (variableExists(name))
            throw(std::runtime_error("Variable `" + name + "` was already added."));

        addVariable(name);
    }
}

void PCFG::importTerminals(const nlohmann::json& terms) {
    for (const auto& t : terms) {
        auto name = t.get<std::string>();

        if (terminalExists(name)) {
            throw(std::runtime_error("Terminal `" + name + "` was already added."));
        }

        addTerminal(name);
    }
}

void PCFG::importProductions(const nlohmann::json& prods) {
    for (const auto& p : prods) {
        auto head = p["head"].get<std::string>();
        auto body = p["body"].get<Body>();
        auto prob = p["prob"].get<double>();

        // check if it can be added safely
        if (!variableExists(head)) {
            throw(std::runtime_error("Head of production " + head + " -> " + to_string(p["body"]) +
                                     " is not a valid variable."));
        }
        for (const auto& s : body) {
            if (!variableExists(s) && !terminalExists(s))
                throw(
                    std::runtime_error("Unknown symbol in production body " + to_string(p["body"]) + ": `" + s + "`."));
        }
        if (prob > 1.0f or prob < 0.0f)
            throw(std::runtime_error("Probability of production " + head + " -> " + to_string(p["body"]) + " (" +
                                     std::to_string(prob) + ") is not valid."));

        addProduction(head, body, prob);
    }
}

void PCFG::exportJSON(std::ostream& out) const {

    // add variables
    nlohmann::json json_variables = nlohmann::json::array();
    for (const auto& s : variables)
        json_variables.push_back(s);

    // add terminals
    nlohmann::json json_terminals = nlohmann::json::array();
    for (const auto& s : terminals)
        json_terminals.push_back(s);

    // add productions
    nlohmann::json json_productions = nlohmann::json::array();
    for (const auto& mapItem : productionRules) {
        nlohmann::json production;
        production["head"] = mapItem.first;
        for (const auto& t : mapItem.second.first) {
            production["body"].push_back(t);
        }
        production["prob"] = mapItem.second.second;
        json_productions.push_back(production);
    }

    // create json
    nlohmann::json result;

    result["Variables"] = json_variables;
    result["Terminals"] = json_terminals;
    result["Productions"] = json_productions;
    result["Start"] = startSymbol;

    out << result;
}
void PCFG::clear() {
    variables.clear();
    terminals.clear();
    startSymbol.clear();
    productionRules.clear();
}

std::string to_string(const PCFG::Body& body) {
    std::string result;
    for (auto& s : body) {
        result += s;
    }
    return result;
}

std::string to_string(const PCFG::Rule& production) {
    return to_string(production.first) + " (" + std::to_string(production.second) + ")";
}

std::ostream& operator<<(std::ostream& out, const PCFG::Body& body) {
    out << to_string(body);
    return out;
}

std::ostream& operator<<(std::ostream& out, const PCFG::Rule& production) {
    out << to_string(production);
    return out;
}

double PCFG::CYK(const std::vector<std::string>& input) {

    using Entry = std::map<std::string, double>;
    using Row = std::vector<Entry>;
    std::vector<Row> table;

    // first row
    {
        Row r;
        for (const auto& c : input) {
            Entry entry;
            for (const auto& v : variables) {
                double prob = hasProduction(v, {c});
                if (prob > 0) {
                    entry.emplace(v, std::log(prob));
                }
            }
            r.push_back(entry);
        }
        table.push_back(r);
    }

    // other rows
    for (std::size_t i = 1; i < input.size(); ++i) {
        Row r;
        for (std::size_t j = 0; j < input.size() - i; ++j) {

            // this entry will be at row i and col j
            Entry entry;

            for (std::size_t k = 0; k < i; ++k) {

                // pick the right pair of variables
                Entry A = table[k][j];
                Entry B = table[i - (k + 1)][j + (k + 1)];

                // check combinations
                for (const auto& var : variables) {
                    bool found = false;
                    for (const auto& a : A) {
                        for (const auto& b : B) {
                            double prob = hasProduction(var, {a.first, b.first});
                            if (prob > 0) {
                                entry.emplace(var, std::log(prob) + a.second + b.second);
                                found = true;
                                continue;
                            }
                        }
                        if (found)
                            continue;
                    }
                }
            }
            r.push_back(entry);
        }
        table.push_back(r);
    }

    auto it = table.back().front().find(getStartSymbol());
    if (it != table.back().front().end()) {
        return it->second;
    } else {
        return 1.0;
    }
}

double PCFG::hasProduction(const std::string& variable, const PCFG::Body& body) {
    auto range = productionRules.equal_range(variable);
    for (auto it = range.first; it != range.second; ++it) {
        if (it->second.first == body)
            return it->second.second;
    }
    return 0.0;
}
