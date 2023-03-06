

#ifndef MB_TO_GROEP_SORTEDPCFG_H
#define MB_TO_GROEP_SORTEDPCFG_H

#include "../PCFG/PCFG.h"
#include <map>
#include <vector>

namespace cracker {
class SortedPCFG {
public:
    SortedPCFG(const PCFG& pcfg);

    bool variableExists(const std::string& name) const;
    bool variableHasProductions(const std::string& name) const;

    const std::vector<PCFG::Rule>& getProductions(const std::string& head) const;

    /**
     * Gets the next most probable rule for a certain head
     *
     * @param head      Head of the requested rule
     * @param previous  Previous most probable rule for head
     *
     * @return Next most probable rule or previous if no such rule exists
     */
    const PCFG::Rule& getMostProbable(const std::string& head, const PCFG::Rule& previous) const;

    /**
     * Get the most probable rule for a certain head
     *
     * @param head      Head of the requested rule
     *
     * @return  Most probable rule for head
     *
     * REQUIRE(variableExists(head));
     * \n REQUIRE(variableHasProductions(head));
     */
    const PCFG::Rule& getMostProbable(const std::string& head) const;

    const PCFG::Rule& getMoreProbable(const std::string& head, const PCFG::Rule& current) const;

private:
    std::map<std::string, std::vector<PCFG::Rule>> m_productions;
};

template <class T>
struct less_prod {
    bool operator()(const T& x, const T& y) const { return x.second < y.second; }
    typedef T first_argument_type;
    typedef T second_argument_type;
    typedef bool result_type;
};
} // namespace cracker

#endif // MB_TO_GROEP_SORTEDPCFG_H
