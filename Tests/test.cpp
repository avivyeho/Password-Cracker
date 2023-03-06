
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "../Libs/DesignByContract.h"
#include "../Libs/doctest/doctest.h"
#include "Cracker/next.cpp"
//#include "GeneticAlgorithm/Organism.test"
//#include "GeneticAlgorithm/Trainer.test"
#include "PCFG/PCFGTest.cpp"
#include "PCFG/json.cpp"

#define DOCTEST_CONFIG_NO_POSIX_SIGNALS

TEST_CASE("sanity check") {
    REQUIRE(true);
    REQUIRE_FALSE(false);
    CHECK_EQ(0, 0);
}
