

#include "GeneticGenerator.h"
#include "../../Libs/DesignByContract.h"
#include "../../Libs/nlohmann/json.h"
#include <iostream>

using json = nlohmann::json;
using namespace std;

// Initieert de generator met
// passwordFile: pad naar file met input wachtwoorden
// configFile: pad naar valid config file met parameters voor de genetic algorithm
GeneticGenerator::GeneticGenerator(const string& passwordFile, const string& configFile) {
    ifstream inputF(passwordFile);
    REQUIRE(inputF.is_open(), "inputFile can't be opened");

    copy(istream_iterator<std::string>(inputF), istream_iterator<std::string>(), back_inserter(inputFile));
    inputF.close();

    for (const auto& password : inputFile) {
        // convert password to basestructure
        string base = baseStructure(password);
        auto it = baseStructures.find(base);

        if (it == baseStructures.end()) {
            baseStructures[base] = 0;
        }
        baseStructures[base]++;
    }

    ifstream configF(configFile);
    REQUIRE(configF.is_open(), "configFile can't be opened");
    json config;
    configF >> config;
    configF.close();
    REQUIRE(config["populationSize"] != nullptr, "configFile can't be read as json");

    int populationSize = config["populationSize"];
    int eliteSize = config["eliteSize"];
    int cutoffSize = config["cutoffSize"];
    int tournamentSize = config["tournamentSize"];
    float mutationRate = config["mutationRate"];
    int genomeLength = config["genomeLength"];
    trainLength = config["trainLength"];

    trainer = Trainer(populationSize, eliteSize, cutoffSize, tournamentSize, mutationRate, genomeLength, this);
    trainer.initPopulation();
}

PCFG GeneticGenerator::modTemplate() {
    PCFG mod;

    mod.addTerminal("s");
    mod.addTerminal("l");
    mod.addTerminal("d");

    mod.addVariable("S");
    mod.addVariable("L");
    mod.addVariable("D");

    mod.addVariable("[S]");
    mod.setStartSymbol("[S]");

    return mod;
}

void GeneticGenerator::setOrganicProductions(Organism& organism, PCFG& pcfg) {

    std::array<std::string, 4> heads = {"[S]", "S", "L", "D"};
    std::array<PCFG::Body, 6> bodies = {
        PCFG::Body{"D", "D"}, PCFG::Body{"d"},      PCFG::Body{"s"},
        PCFG::Body{"S", "S"}, PCFG::Body{"L", "L"}, PCFG::Body{"l"},
    };

    for (std::size_t i = 0; i < heads.size(); ++i) {
        double remainder = 1;
        for (std::size_t j = 0; j < bodies.size() - 1; ++j) {
            std::size_t index = i * (bodies.size() - 1) + j;
            pcfg.addProduction(heads[i], bodies[j], organism.getGene(index) * remainder);
            remainder -= organism.getGene(index) * remainder;
        }
        pcfg.addProduction(heads[i], bodies.back(), remainder);
    }

    //    pcfg.addProduction("[S]", {"D", "D"}, organism.getGene(0) * rem);
    //    rem -= organism.getGene(0) * rem;
    //    pcfg.addProduction("[S]", {"d"}, organism.getGene(1) * rem);
    //    rem -= organism.getGene(1) * rem;
    //    pcfg.addProduction("[S]", {"s"}, organism.getGene(2) * rem);
    //    rem -= organism.getGene(2) * rem;
    //    pcfg.addProduction("[S]", {"S", "S"}, organism.getGene(3) * rem);
    //    rem -= organism.getGene(3) * rem;
    //    pcfg.addProduction("[S]", {"L", "L"}, organism.getGene(4) * rem);
    //    rem -= organism.getGene(4) * rem;
    //    pcfg.addProduction("[S]", {"l"}, rem);
    //
    //    rem = 1;
    //    pcfg.addProduction("S", {"D", "D"}, organism.getGene(5) * rem);
    //    rem -= organism.getGene(5) * rem;
    //    pcfg.addProduction("S", {"d"}, organism.getGene(6) * rem);
    //    rem -= organism.getGene(6) * rem;
    //    pcfg.addProduction("S", {"s"}, organism.getGene(7) * rem);
    //    rem -= organism.getGene(7) * rem;
    //    pcfg.addProduction("S", {"S", "S"}, organism.getGene(8) * rem);
    //    rem -= organism.getGene(8) * rem;
    //    pcfg.addProduction("S", {"L", "L"}, organism.getGene(9) * rem);
    //    rem -= organism.getGene(9) * rem;
    //    pcfg.addProduction("S", {"l"}, rem);
    //
    //    rem = 1;
    //    pcfg.addProduction("L", {"D", "D"}, organism.getGene(10) * rem);
    //    rem -= organism.getGene(10) * rem;
    //    pcfg.addProduction("L", {"d"}, organism.getGene(11) * rem);
    //    rem -= organism.getGene(11) * rem;
    //    pcfg.addProduction("L", {"s"}, organism.getGene(12) * rem);
    //    rem -= organism.getGene(12) * rem;
    //    pcfg.addProduction("L", {"S", "S"}, organism.getGene(13) * rem);
    //    rem -= organism.getGene(13) * rem;
    //    pcfg.addProduction("L", {"L", "L"}, organism.getGene(14) * rem);
    //    rem -= organism.getGene(14) * rem;
    //    pcfg.addProduction("L", {"l"}, rem);
    //
    //    rem = 1;
    //    pcfg.addProduction("D", {"D", "D"}, organism.getGene(15) * rem);
    //    rem -= organism.getGene(15) * rem;
    //    pcfg.addProduction("D", {"d"}, organism.getGene(16) * rem);
    //    rem -= organism.getGene(16) * rem;
    //    pcfg.addProduction("D", {"s"}, organism.getGene(17) * rem);
    //    rem -= organism.getGene(17) * rem;
    //    pcfg.addProduction("D", {"S", "S"}, organism.getGene(18) * rem);
    //    rem -= organism.getGene(18) * rem;
    //    pcfg.addProduction("D", {"L", "L"}, organism.getGene(19) * rem);
    //    rem -= organism.getGene(19) * rem;
    //    pcfg.addProduction("D", {"l"}, rem);
}

// zal de genetisch algoritme uitvoeren en de beste PCFG teruggeven
PCFG GeneticGenerator::Generate(bool debug, double stop_if) {
    PCFG mod = modTemplate();

    Organism bestOrganism = trainer.train(trainLength, debug, stop_if);
    setOrganicProductions(bestOrganism, mod);

    ENSURE(!mod.getProductionRules().empty(), "PCFG has production rules generated");
    return mod;
}

double GeneticGenerator::calcFit(Organism& organism) {
    double fit = 0;
    PCFG mod = modTemplate();
    setOrganicProductions(organism, mod);

    for (const auto& base : baseStructures) {
        double log = mod.CYK(splitBase(base.first));
        if (log <= 0) {
            fit += exp(log) * base.second;
        }
    }

    return fit;
}

string GeneticGenerator::baseStructure(const string& input) {
    string rtn;

    for (const auto& c : input) {
        if (c == ' ') {
            continue;
        }
        if (isalpha(c)) {
            rtn.push_back('l');
        } else if (isdigit(c)) {
            rtn.push_back('d');
        } else {
            rtn.push_back('s');
        }
    }

    return rtn;
}

vector<string> GeneticGenerator::splitBase(const string& input) {
    vector<string> rtn;
    for (const auto& c : input) {
        rtn.emplace_back(1, c);
    }
    return rtn;
}
