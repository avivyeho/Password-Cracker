

#ifndef MB_TO_GROEP_GENETICGENERATOR_H
#define MB_TO_GROEP_GENETICGENERATOR_H

#include "../PCFG/PCFG.h"
#include "Trainer.h"
#include <array>
#include <fstream>
#include <string>

using namespace std;

class GeneticGenerator {
    friend class Trainer;

    vector<string> inputFile;
    map<string, size_t> baseStructures;
    Trainer trainer;
    int trainLength = 0;

    virtual double calcFit(Organism& organism);

    static PCFG modTemplate();

    static void setOrganicProductions(Organism& organism, PCFG& pcfg);

public:
    // Initieert de generator met
    // passwordFile: pad naar file met input wachtwoorden
    // configFile: pad naar valid config file met parameters voor de genetic algorithm
    //
    // REQUIRE(inputF.is_open(), "inputFile can't be opened");
    // REQUIRE(configF.is_open(), "configFile can't be opened");
    // REQUIRE(config["populationSize"] != nullptr, "configFile can't be read as json");
    GeneticGenerator(const string& passwordFile, const string& configFile);

    // zal de genetisch algoritme uitvoeren en de beste PCFG teruggeven
    //
    // ENSURE(!mod.getProductionRules().empty(),"PCFG has production rules generated");
    PCFG Generate(bool debug = false, double stop_if = 0);

    // placeholder constructor voor tests
    GeneticGenerator() = default;

    static string baseStructure(const string& input);
    static vector<string> splitBase(const string& input);
};

#endif // MB_TO_GROEP_GENETICGENERATOR_H
