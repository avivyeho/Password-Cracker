

#include "Trainer.h"
#include "../../Libs/DesignByContract.h"
#include "../../Libs/effolkronium/random.hpp"
#include "GeneticGenerator.h"
#include <algorithm>
#include <iostream>
#include <omp.h>

using Random = effolkronium::random_static;

using namespace std;

// Maakt een trainer met de nodige parameters voor de genetische algoritme
// populationSize : vast aantal organismen per generatie
// eliteSize : aantal beste organismen die gegarandeerd overleven naar de volgende generatie
// cutoffSize : aantal slechste organismen die gegarandeerd sterven elke generatie
// mutationRate : % kans voor elk geen dat die zou muteren
// genomeLength : aantal genen(kansen) per organisme
// generator : pointer naar een classe met functie de organismen zal evalueren een een niet-negatieve fitness score
// geven
Trainer::Trainer(int populationSize, int eliteSize, int cutoffSize, int tournamentSize, float mutationRate,
                 int genomeLength, GeneticGenerator* generator) {

    REQUIRE(populationSize > 0, "populationsize must be >0");
    REQUIRE(eliteSize >= 0 && eliteSize <= populationSize, "eliteSize must be in [0, populationSize]");
    REQUIRE(cutoffSize >= 0 && cutoffSize <= populationSize, "cutoffSize must be in [0, populationSize]");
    REQUIRE(cutoffSize + eliteSize <= populationSize, "cutoffSize + eliteSize must be < or = populationSize");
    REQUIRE(tournamentSize > 0, "must choose at least one organism per tournament");
    REQUIRE(mutationRate >= 0 && mutationRate <= 100, "mutationrate must be in [0,100]");
    REQUIRE(genomeLength > 0, "each organism must have at least one gene");

    popSize = populationSize;
    Trainer::eliteSize = eliteSize;
    Trainer::cutoffSize = cutoffSize;
    Trainer::tournamentSize = tournamentSize;
    Trainer::mutationRate = mutationRate;
    Trainer::genomeLength = genomeLength;
    Trainer::generator = generator;
}

// Maakt population leeg en vult het met willekeurige organismen, hoeft slechts 1 keer uitgevoerd te worden
// om te kunnen trainen, kan vervolgens dezelfde population blijven trainen zonder reset
void Trainer::initPopulation() {
    population.clear();

    for (int i = 0; i < popSize; i++) {
        population.push_back(Organism(genomeLength)); // willekeurig organisme
    }
    ENSURE(population.size() == popSize, "population is fully filled");
}

// Traint de huidige population een aantal generaties en returned de beste organisme
// generations : hoeveel generaties lang te trainen
// debug : print fitness data voor elke generatie
Organism Trainer::train(int maxgen, bool debug, double stop_if) {
    REQUIRE(maxgen >= 0, "maxgen can't be negative");
    REQUIRE(population.size() == popSize, "must have a population to train, use initPopulation() first");
    int gen = 0;
    for (gen = 0; gen < maxgen; gen++) {

#pragma omp parallel default(none)
        {
            int t = omp_get_num_threads();
            int orgs = popSize / t;
            int num = omp_get_thread_num();
            int start = num * orgs;
            int end = (num + 1) * orgs;
            if (num == t - 1) {
                end = popSize;
            }

            for (int i = start; i < end; i++) {
                population[i].setFit(generator->calcFit(population[i]));
            }
        }
        // for (Organism& organism : population) { // evalueert de huidige generatie
        //    organism.setFit(generator->calcFit(organism));
        //}

        sortPopulation(); // sorteert van beste -> slechtste

        if (debug) {
            cout << "Generation " << gen << " best fitness: " << population[0].getFit() << endl;
        }
        if (stop_if > 0 and population[0].getFit() > stop_if) {
            break;
        }

        naturalSelection(); // maakt de volgende generatie
    }

    sortPopulation(); // sorteert van beste -> slechtste

    cout << "Finished " << gen << " generations, with best fitness: " << population[0].getFit() << endl;

    ENSURE(population[0].genomeLength() > 0, "returns non empty organism");

    return population[0]; // geeft beste terug
}

bool sortByFitness(const Organism& lhs, const Organism& rhs) { return lhs.getFit() > rhs.getFit(); }

// sort population van hoog->laag fitness
void Trainer::sortPopulation() { sort(population.begin(), population.end(), sortByFitness); }

// maakt de volgende generatie
void Trainer::naturalSelection() {
    vector<Organism> nextPopulation = vector<Organism>(popSize);

    for (int i = 0; i < popSize; i++) {
        if (i < eliteSize) // de eerste "eliteSize" beste organismen blijven behouden in de volgende generatie
        {
            nextPopulation[i] = population[i];
        } else // de rest van de generatie wordt opgevuld met kinderen van de eerste "popSize - cutoff" besten
        {
            int mommy = tournamentSelection();
            int daddy = tournamentSelection();

            Organism child = population[mommy].crossover(population[daddy]);
            child.mutate(mutationRate);

            nextPopulation[i] = child;
        }
    }

    population = nextPopulation;
}

// kiest een voortplantings partner uit de eerste "popsize-cutoff" besten via een tournament
int Trainer::tournamentSelection() { // return index uit de population vector
    vector<int> tournament = vector<int>(tournamentSize);

    for (int n = 0; n < tournamentSize; n++) { // "tournamentSize" willekeurige organismen uit population(zonder cutoff)
        int chosen_index = Random::get(0, popSize - cutoffSize);
        tournament[n] = chosen_index;
    }

    int best_index = tournament[0];

    for (int i = 0; i < tournamentSize; i++) { // beste organisme nemen uit de tournament
        if (population[tournament[i]].getFit() > population[best_index].getFit()) {
            best_index = tournament[i];
        }
    }

    return best_index;
}

Trainer::Trainer() {}
