#include "../../Source/PCFG/PCFG.h"

TEST_CASE("PCFG : CNF") {
    std::string path = "../Tests/PCFG/";

    PCFG test;
    {
        std::ifstream json(path + "in.json");
        test.importJSON(json);
        json.close();
    }

    PCFG result = test.toCNF2();

    std::stringstream output;
    result.exportJSON(output);

    REQUIRE(Contracts::fileCompare(path + "cnf.json", output.str()));
}