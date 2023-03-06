
#ifndef MB_TO_GROEP_NODE_H
#define MB_TO_GROEP_NODE_H

#include <algorithm>
#include <list>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>

#include "../PCFG/PCFG.h"
#include "SortedPCFG.h"

namespace cracker {
/**
 * Priority queue Node used by the cracker to generate next pre-terminal structures
 */
class Node {
public:
    using Ptr = std::shared_ptr<Node>;

public:
    Node(const PCFG::Rule& production, const SortedPCFG& pcfg);
    Node(const Node& node);
    Node& operator=(const Node&) = delete;

    /// Ordering based on probability
    bool operator<(const Node& that) const;

    /// Get probability of pre-terminal structure of this node
    double getProbability() const;

    double getPivot() const;
    std::size_t getSize() const;

    /**
     * Get next most probable node considering pivot value
     *
     * @param pivot     Determines which rule gets changed
     * @param pcfg      Sorted PCFG used to determine next most probable rule to apply
     *
     * @return next     most probable node or nullptr if there is none
     *
     * REQUIRE(pivot < getSize())
     * if (result) ENSURE(result->getPivot() == pivot)
     */
    Node::Ptr getNext(std::size_t pivot, const SortedPCFG& pcfg) const;

    double alternateParentProbability(std::size_t pivot, const SortedPCFG& pcfg) const;

    /// Get pre-terminal string of this node
    std::string getTerminalString() const;

private:
    // calculate probability of pre-terminal structure of this node
    double calculateProbability() const;

    // initial construction of most probable pre-terminal structure for base structure of this node
    void buildPreterminal(const SortedPCFG& pcfg);

private:
    using Group = std::pair<std::string, PCFG::Rule>;

private:
    std::vector<Group> m_structure;

    double m_prob = 0;
    int m_pivot = 0;

    mutable double prob_cache = -1;
};
} // namespace cracker

#endif // MB_TO_GROEP_NODE_H
