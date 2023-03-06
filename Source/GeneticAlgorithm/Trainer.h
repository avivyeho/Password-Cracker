

#ifndef MB_TO_GROEP_TRAINER_H
#define MB_TO_GROEP_TRAINER_H

#include "Organism.h"

class GeneticGenerator;

class Trainer {
    int popSize;
    int eliteSize;
    int cutoffSize;
    int tournamentSize;
    float mutationRate;
    int genomeLength;
    vector<Organism> population;
    GeneticGenerator* generator;

    // sort population van hoog->laag fitness
    void sortPopulation();

    // maakt de volgende generatie
    void naturalSelection();

    // kiest een voortplantings partner uit de eerste "popsize-cutoff" besten via een tournament
    int tournamentSelection();

public:
    // Maakt een trainer met de nodige parameters voor de genetische algoritme
    // populationSize : vast aantal organismen per generatie
    // eliteSize : aantal beste organismen die gegarandeerd overleven naar de volgende generatie
    // cutoffSize : aantal slechste organismen die gegarandeerd sterven elke generatie
    // mutationRate : % kans voor elk geen dat die zou muteren
    // genomeLength : aantal genen(kansen) per organisme
    // generator : pointer naar een classe met functie de organismen zal evalueren een een niet-negatieve fitness score
    // geven
    //
    // REQUIRE(populationSize > 0,"populationsize must be >0");
    // REQUIRE(eliteSize >= 0 && eliteSize <= populationSize,"eliteSize must be in [0, populationSize]");
    // REQUIRE(cutoffSize >= 0 && cutoffSize <= populationSize,"cutoffSize must be in [0, populationSize]");
    // REQUIRE(cutoffSize + eliteSize <= populationSize,"cutoffSize + eliteSize must be < or = populationSize");
    // REQUIRE(tournamentSize > 0,"must choose at least one organism per tournament");
    // REQUIRE(mutationRate >=0 && mutationRate <= 100,"mutationrate must be in [0,100]");
    // REQUIRE(genomeLength > 0,"each organism must have at least one gene");
    Trainer(int populationSize, int eliteSize, int cutoffSize, int tournamentSize, float mutationRate, int genomeLength,
            GeneticGenerator* generator);

    Trainer();

    // Maakt population leeg en vult het met willekeurige organismen, hoeft slechts 1 keer uitgevoerd te worden
    // om te kunnen trainen, kan vervolgens dezelfde population blijven trainen zonder reset
    //
    // ENSURE(population.size() == popSize, "population is fully filled");
    void initPopulation();

    // Traint de huidige population een aantal generaties en returned de beste organisme
    // generations : hoeveel generaties lang te trainen
    // debug : print fitness data voor elke generatie
    //
    // REQUIRE(maxgen >= 0, "maxgen can't be negative");
    // REQUIRE(population.size() == popSize,"must have a population to train, use initPopulation() first");
    // ENSURE(population[0].genomeLength() > 0, "returns non empty organism");
    Organism train(int generations, bool debug = false, double stop_if = 0);
};

#endif // MB_TO_GROEP_TRAINER_H
