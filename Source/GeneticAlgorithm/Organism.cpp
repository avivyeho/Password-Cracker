

#include "Organism.h"
#include "../../Libs/DesignByContract.h"
#include "../../Libs/effolkronium/random.hpp"

using Random = effolkronium::random_static;

// Maakt een nieuw organisme van een bestaande vector van genen
Organism::Organism(vector<double>& genes) {
    REQUIRE(!genes.empty(), "must provide at least one gene when creating organism");

    genome = genes;

    ENSURE(!genome.empty(), "organism born with genes");
    ENSURE(fitness == 0, "organism born with a fitness value of 0");
}

// Getter voor fitness waarde van een organisme
double Organism::getFit() const { return fitness; }

// Setter voor fitness waarde van een organisme
void Organism::setFit(double fit) { fitness = fit; }

// Maakt een kind samen met spouse, waarvan de genen verdeeld zijn in twee (willekeurig) oneven helften
Organism Organism::crossover(Organism& spouse) {
    REQUIRE(!genome.empty(), "organism not empty");
    REQUIRE(!spouse.genome.empty(), "spouse not empty");
    REQUIRE(genome.size() == spouse.genome.size(), "organism and spouse must have equal amount of genes");

    vector<double> child_genome = vector<double>();

    int mid = Random::get(0, int(genome.size() - 1)); // mid-point tussen this/spouse genen

    for (int i = 0; i < mid; i++) {
        child_genome.push_back(genome[i]);
    }
    for (int j = mid; j < genome.size(); j++) {
        child_genome.push_back(spouse.genome[j]);
    }

    Organism child = Organism(child_genome);

    ENSURE(!child.genome.empty(), "child is not empty");
    ENSURE(child.genome.size() == genome.size() && child.genome.size() == spouse.genome.size(),
           "child has same amount of genes as parents");
    ENSURE(child.fitness == 0, "child born with a fitness value of 0");

    return child;
}

// Elk geen zal "mutationrate" kans hebben om te muteren naar een willekeurige waarde(tussen 0 en 1)
void Organism::mutate(float mutationrate) {
    REQUIRE(mutationrate >= 0 && mutationrate <= 100, "mutationrate must be in [0,100]");
    REQUIRE(!genome.empty(), "organism not empty");

    for (int i = 0; i < genome.size(); i++) {
        float mutationchance = Random::get(0.f, 100.f);

        if (mutationchance < mutationrate) {
            double val = Random::get(0.0, 1.0);
            genome[i] = val;
        }
    }
}

// Maakt een organisme met "genomeSize" willekeurige(tussen 0 en 1) genen
Organism::Organism(int genomeSize) {
    REQUIRE(genomeSize > 0, "organism must have at least one gene");

    for (int i = 0; i < genomeSize; i++) {
        double val = Random::get(0.0, 1.0);
        genome.push_back(val);
    }

    ENSURE(!genome.empty(), "organism born with genes");
    ENSURE(genome.size() == genomeSize, "organism born with \"genomeSize\" random genes");
    ENSURE(fitness == 0, "organism born with a fitness value of 0");
}

// Getter voor genome lengte
int Organism::genomeLength() const {
    ENSURE(genome.size() > 0, "genome not empty");
    return genome.size();
}

// Getter voor een genome
double Organism::getGene(int index) const { return genome[index]; }

// Maakt een leeg organisme, zou enkel als placeholder gebruikt mogen worden
Organism::Organism() {
    ENSURE(genome.empty(), "organism born empty");
    ENSURE(fitness == 0, "organism born with a fitness value of 0");
}
