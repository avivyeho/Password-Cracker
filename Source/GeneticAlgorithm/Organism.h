

#ifndef MB_TO_GROEP_ORGANISM_H
#define MB_TO_GROEP_ORGANISM_H

#include <vector>

using namespace std;

class Organism {
    double fitness = 0;
    vector<double> genome;

public:
    // Maakt een leeg organisme, zou enkel als placeholder gebruikt mogen worden
    //
    // ENSURE(genome.empty(),"organism born empty");
    // ENSURE(fitness == 0,"organism born with a fitness value of 0");
    Organism();

    // Maakt een nieuw organisme van een bestaande vector van genen
    //
    // REQUIRE(!genes.empty(),"must provide at least one gene when creating organism");
    // ENSURE(!genome.empty(),"organism born with genes");
    // ENSURE(fitness == 0,"organism born with a fitness value of 0");
    Organism(vector<double>& genes);

    // Maakt een organisme met "genomeSize" willekeurige(tussen 0 en 1) genen
    //
    // REQUIRE(genomeSize > 0,"organism must have at least one gene");
    // ENSURE(!genome.empty(),"organism born with genes");
    // ENSURE(genome.size() == genomeSize,"organism born with \"genomeSize\" random genes");
    // ENSURE(fitness == 0,"organism born with a fitness value of 0");
    Organism(int genomeSize);

    // Maakt een kind samen met spouse, waarvan de genen verdeeld zijn in twee (willekeurig) oneven helften
    //
    // REQUIRE(!genome.empty(),"organism not empty");
    // REQUIRE(!spouse.genome.empty(),"spouse not empty");
    // REQUIRE(genome.size() == spouse.genome.size(),"organism and spouse must have equal amount of genes");
    // ENSURE(!child.genome.empty(),"child is not empty");
    // ENSURE(child.genome.size() == genome.size() && child.genome.size() == spouse.genome.size(),"child has same amount
    // of genes as parents"); ENSURE(child.fitness == 0,"child born with a fitness value of 0");
    Organism crossover(Organism& spouse);

    // Elk geen zal "mutationrate" kans hebben om te muteren naar een willekeurige waarde(tussen 0 en 1)
    //
    // REQUIRE(mutationrate>=0 && mutationrate <= 100,"mutationrate must be in [0,100]");
    // REQUIRE(!genome.empty(),"organism not empty");
    void mutate(float mutationrate);

    // Getter voor fitness waarde van een organisme
    //
    // ENSURE(fitness>=0,"organism has a positive fitness value");
    double getFit() const;

    // Setter voor fitness waarde van een organisme
    //
    // REQUIRE(fit >= 0,"fitness must be positive");
    void setFit(double fit);

    // Getter voor genome lengte
    //
    // ENSURE(genome.size() > 0,"genome not empty");
    int genomeLength() const;

    // Getter voor een genome
    double getGene(int index) const;
};

#endif // MB_TO_GROEP_ORGANISM_H
