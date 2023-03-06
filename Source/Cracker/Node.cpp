
#include "Node.h"

cracker::Node::Node(const PCFG::Rule& production, const SortedPCFG& pcfg) {
    for (const auto& s : production.first) {
        m_structure.emplace_back(s, PCFG::Rule());
    }
    m_prob = production.second;

    buildPreterminal(pcfg);
}

cracker::Node::Node(const cracker::Node& node)
    : m_structure(node.m_structure), m_prob(node.m_prob), m_pivot(node.m_pivot) {}

bool cracker::Node::operator<(const cracker::Node& that) const {
    return this->getProbability() < that.getProbability();
}

double cracker::Node::getPivot() const { return m_pivot; }
std::size_t cracker::Node::getSize() const { return m_structure.size(); }

double cracker::Node::getProbability() const {
    if (prob_cache < 0) {
        prob_cache = calculateProbability();
    }

    assert(prob_cache == calculateProbability());
    return prob_cache;
}

double cracker::Node::calculateProbability() const {
    double result = m_prob;
    for (const auto& g : m_structure) {
        result *= g.second.second;
    }
    return result;
}

void cracker::Node::buildPreterminal(const SortedPCFG& pcfg) {
    for (auto& group : m_structure) {
        // if the PCFG has productions for this symbol -> use the most probable production
        // else -> just use symbol
        const auto& symbol = group.first;
        if (pcfg.variableHasProductions(symbol)) {
            group.second = pcfg.getMostProbable(symbol);
        } else {
            group.second = {{symbol}, 1};
        }
    }
}

cracker::Node::Ptr cracker::Node::getNext(std::size_t pivot, const SortedPCFG& pcfg) const {
    assert(pivot < getSize());

    // create new node
    auto result = std::make_shared<Node>(*this);
    result->m_pivot = pivot;

    // get next most probable pre-terminal structure on pivot
    auto& g = result->m_structure.at(pivot);
    const auto& next = pcfg.getMostProbable(g.first, g.second);

    // check if it found a next pre-terminal structure
    if (g.second == next)
        result = nullptr;
    else
        g.second = next;

    if (result)
        assert(result->getPivot() == pivot);
    return result;
}

std::string cracker::Node::getTerminalString() const {
    std::string result;
    for (const auto& g : m_structure) {
        result += to_string(g.second.first);
    }
    return result;
}
double cracker::Node::alternateParentProbability(std::size_t pivot, const cracker::SortedPCFG& pcfg) const {
    assert(pivot < getSize());

    // get more probable pre-terminal structure on pivot
    const auto& g = m_structure.at(pivot);
    const auto& prev = pcfg.getMoreProbable(g.first, g.second);

    // return 1 if no more probable pre-terminal structures exist
    if (g.second == prev) {
        return 1;
    }

    // return parent chance
    Node n{*this};
    n.m_structure.at(pivot).second = prev;
    return n.getProbability();
}
