

#ifndef MB_TO_GROEP_CRACKER_H
#define MB_TO_GROEP_CRACKER_H

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../../Libs/DesignByContract.h"
#include "Node.h"
#include "SortedPCFG.h"

namespace cracker {
// compare object that compares the objects instead of the pointers
template <class T>
struct less_ptr {
    bool operator()(const T& x, const T& y) const { return *x < *y; }
    typedef T first_argument_type;
    typedef T second_argument_type;
    typedef bool result_type;
};

/**
 * Cracker used to generate password guesses
 */
class Cracker {
public:
    /// Constructor
    explicit Cracker(bool useDeadbeatDad = true);
    Cracker(const Cracker&) = delete;
    Cracker& operator=(const Cracker&) = delete;

    /// Loads the PCFG used to generate pre-terminal structures
    void loadPCFG(const PCFG& pcfg);
    void loadPCFG(std::istream& json);
    void loadPCFG(const std::string& filename);

    /// Checks if a next pre-terminal structure can get generated
    bool good() const;

    /**
     * Generates the next pre-terminal structure: combination of L, U, <n digits>, <n specials>
     *
     * @return  The next most probable pre-terminal structure
     *
     * REQUIRE(good());
     */
    std::string nextPreterminal();

    /**
     * Generates all possible passwords using a dictionary
     * @param bString base string (e.g. LLLDDLSDDL)
     * @param n amount of generated passwords
     * @param filter a filter to reduce possible passwords combinations
     * @return set with all possible passwords for base string
     */
    std::vector<std::string> generate_pStrings(const std::string& bString, int n, int filter);
    void loadDictionary(const std::string &filename);

    //
    static std::vector<std::string> preterimalVector(const std::string& preterminal);

private:
    // check if parent can safely abandon its child (used for deadbeat dad algorithm)
    bool abandonChild(const Node& child, const Node& parent, std::size_t pivot) const;
    std::vector<std::string> combinePossibilities(std::map<int, std::vector<std::string>> index, int n, int filter);

private:
    bool m_useDeadbeatDad;
    std::unique_ptr<SortedPCFG> m_pcfg;
    std::priority_queue<Node::Ptr, std::vector<Node::Ptr>, less_ptr<Node::Ptr>> m_queue;

    std::map<int, std::vector<std::string>> wordList;
};
} // namespace cracker

#endif // MB_TO_GROEP_CRACKER_H
