

#include "Cracker.h"
#include <fstream>
#include <random>

cracker::Cracker::Cracker(bool useDeadbeatDad) : m_useDeadbeatDad(useDeadbeatDad) {}

void cracker::Cracker::loadPCFG(const PCFG& pcfg) {
    m_pcfg = std::make_unique<SortedPCFG>(pcfg);

    // populate priority queue
    for (const auto& p : m_pcfg->getProductions(pcfg.getStartSymbol())) {
        auto n = std::make_shared<Node>(p, *m_pcfg);
        m_queue.push(std::move(n));
    }
}

void cracker::Cracker::loadPCFG(std::istream& json) {
    PCFG pcfg;
    pcfg.importJSON(json);
    loadPCFG(pcfg);
}

void cracker::Cracker::loadPCFG(const std::string& filename) {
    std::ifstream json;
    json.open(filename);
    loadPCFG(json);
    json.close();
}

bool cracker::Cracker::good() const { return m_pcfg and !m_queue.empty(); }

std::string cracker::Cracker::nextPreterminal() {
    assert(good());

    auto current = m_queue.top();
    m_queue.pop();

    if (m_useDeadbeatDad) {
        for (std::size_t i = 0; i < current->getSize(); ++i) {
            auto child = current->getNext(i, *m_pcfg);
            if (child and !abandonChild(*child, *current, i))
                m_queue.push(std::move(child));
        }

    } else {
        for (std::size_t i = current->getPivot(); i < current->getSize(); ++i)
            if (auto child = current->getNext(i, *m_pcfg))
                m_queue.push(std::move(child));
    }

    return current->getTerminalString();
}

bool cracker::Cracker::abandonChild(const cracker::Node& child, const cracker::Node& parent, std::size_t pivot) const {
    for (std::size_t i = 0; i < child.getSize(); ++i) {
        // skip parent
        if (i == pivot)
            continue;

        // check for eligible other parents
        double otherParent = child.alternateParentProbability(i, *m_pcfg);
        if (otherParent < parent.getProbability() or (otherParent == parent.getProbability() and i > pivot)) {
            return true;
        }
    }
    return false;
}
std::vector<std::string> cracker::Cracker::generate_pStrings(const std::string& bString, int n, int filter) {
    std::vector<std::string> lower(26);
    std::iota(lower.begin(), lower.end(), 'a');
    std::vector<std::string> upper(26);
    std::iota(upper.begin(), upper.end(), 'A');


    std::vector<std::string> cPart1 = preterimalVector(bString);
    std::vector<std::string> cPart2;
    std::map<int,std::vector<std::string>> index;
    std::string toAdd;
    for(auto i= 0; i!= cPart1.size();i++){
        if(i>=cPart1.size()) break;
        if(cPart1[i] == "L" or cPart1[i] == "U"){
            toAdd.push_back(cPart1[i][0]);
            while(cPart1[i+1] == "L" or cPart1[i+1] == "U") {
                toAdd.push_back(cPart1[i+1][0]);
                i++;
            }
            cPart2.push_back(toAdd);
            toAdd.clear();
        }
        else{
            cPart2.push_back(cPart1[i]);
        }

    }
    for (int i = 0; i != cPart2.size(); i++) {
        int wordSize = cPart2[i].size();
        char cType = cPart2[i][0];
        if (cType == 'L') {
            if(wordSize == 1) index[i] = lower;
            else {
                index[i] = wordList[wordSize];
                std::set<int> toUpper;
                for(auto j=0;j<cPart2[i].size();j++){
                    if(cPart2[i][j] == 'U') toUpper.insert(j);
                }
                if(!toUpper.empty()) {
                    for (auto& word : index[i]) {
                        for (auto& tu : toUpper) {
                            word[tu] = toupper(word[tu]);
                        }
                    }
                }

            }

        }
        else if(cType == 'U'){
            if(wordSize == 1) index[i] = upper;
            else {
                index[i] = wordList[wordSize];
                std::set<int> toUpper;
                for(auto j=0;j<cPart2[i].size();j++){
                    if(cPart2[i][j] == 'U') toUpper.insert(j);
                }
                if(!toUpper.empty()) {
                    for (auto& word : index[i]) {
                        for (auto& tu : toUpper) {
                            word[tu] = toupper(word[tu]);
                        }
                    }
                }
            }
        }
        else{
            index[i] = {cPart2[i]};
        }
        ENSURE(!index[i].empty(), "Empty set");
    }
    if (index.size() == 1)
        return index[0];
    return combinePossibilities(index, n, filter);


}
void cracker::Cracker::loadDictionary(const std::string &filename) {
    std::fstream wordDic(filename);
    std::string word;

    while (wordDic >> word) {
        wordList[word.size()].push_back(word);
    }
}
std::vector<std::string> cracker::Cracker::combinePossibilities(std::map<int, std::vector<std::string>> index, int n,
                                                                int filter) {
    std::vector<std::string> combinations;
    std::vector<std::string> c1;
    std::vector<std::string> c2;
    if (index.size() == 2) {
        for (auto& lhs : index[0]) {
            for (auto& rhs : index[1]) {
                combinations.push_back(lhs + rhs);
            }
        }
    }

    else {
        c1 = index[0];
        // slice container from begin ( containersize -1)
        for (int i = 0; i != index.size() - 1; i++) {
            index[i] = index[i + 1]; // shift container to left
        }
        index.erase(index.size() - 1);                    // remove identical copy at the end
        c2 = combinePossibilities(index, filter, filter); // recursion with a smaller container

        int progressbar = 0;
        //std::cout << "Starting on set with " << c1.size() * c2.size() << " possible outcomes." << std::endl;
        for (auto& lhs : c1) {
            for (auto& rhs : c2) {
                combinations.push_back(lhs + rhs); // cross product

                progressbar++;
                double progress = progressbar / (double)(c1.size() * c2.size()) * 100;
                if (std::trunc(progress) == progress) {
                    if ((int)progress % 5 == 0 and progress != 0) {
                        //std::cout << "====> " << progress << "%" << std::endl;
                    }
                }
            }
        }
    }
    std::shuffle(combinations.begin(), combinations.end(),
                 std::mt19937(std::random_device()())); // shuffle for a bit of randomness
    if (n > combinations.size())
        n = combinations.size();
    return std::vector<std::string>(combinations.begin(),
                                    combinations.begin() + n); // return wanted amount of generated passwords
}

std::vector<std::string> cracker::Cracker::preterimalVector(const std::string& preterminal) {
    std::vector<std::string> result;
    std::string acc;
    for (char c : preterminal) {
        if (c == 'U' or c == 'L') {
            if (!acc.empty()) {
                result.push_back(acc);
                acc.clear();
            }
            result.emplace_back(1, c);
        } else {
            acc += c;
        }
    }
    if (!acc.empty()) {
        result.push_back(acc);
    }
    return result;
}
