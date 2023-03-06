
#include "SortedPCFG.h"

cracker::SortedPCFG::SortedPCFG(const PCFG& pcfg) {
    auto entire = pcfg.getProductionRules();

    for (const auto& mapItem : entire) {
        if (!variableExists(mapItem.first))
            m_productions[mapItem.first] = std::vector<PCFG::Rule>();

        m_productions[mapItem.first].push_back(mapItem.second);
    }

    // sort all values in descending order of probability
    for (auto& prod : m_productions) {
        std::sort(prod.second.begin(), prod.second.end(), less_prod<PCFG::Rule>());
        std::reverse(prod.second.begin(), prod.second.end());
    }
}

bool cracker::SortedPCFG::variableExists(const std::string& name) const {
    return m_productions.find(name) != m_productions.end();
}
bool cracker::SortedPCFG::variableHasProductions(const std::string& name) const {
    return variableExists(name) and !getProductions(name).empty();
}

const std::vector<PCFG::Rule>& cracker::SortedPCFG::getProductions(const std::string& head) const {
    assert(variableExists(head));
    return m_productions.at(head);
}

const PCFG::Rule& cracker::SortedPCFG::getMostProbable(const std::string& head, const PCFG::Rule& previous) const {
    // if this head doesn't have productions, return the previous body
    if (!variableHasProductions(head))
        return previous;

    // search for previous body and return next
    const auto& rules = getProductions(head);
    for (auto it = rules.begin(); it != rules.end(); ++it) {
        // skip higher probability productions
        if (it->second > previous.second)
            continue;

        if (previous.first == it->first and std::next(it) != rules.end())
            return *std::next(it);
    }

    // if nothing found, return the previous body
    return previous;
}

const PCFG::Rule& cracker::SortedPCFG::getMostProbable(const std::string& head) const {
    assert(variableHasProductions(head));

    return getProductions(head).front();
}

const PCFG::Rule& cracker::SortedPCFG::getMoreProbable(const std::string& head, const PCFG::Rule& current) const {
    // if this head doesn't have productions, return the current body
    if (!variableHasProductions(head))
        return current;

    // search for current body and return previous
    const auto& rules = getProductions(head);
    for (auto it = rules.begin(); it != rules.end(); ++it) {
        // skip higher probability productions
        if (it->second > current.second)
            continue;

        if (current.first == it->first and it != rules.begin())
            return *std::prev(it);
    }

    // if nothing found, return the current body
    return current;
}
