#include "../../Source/PCFG/PCFG.h"

TEST_CASE("PCFG : Export JSON") {
    PCFG test;
    std::stringstream output;

    std::string path = "../Tests/PCFG/json-export/";

    SUBCASE("happy day") {
        test.addVariable("S");
        test.addVariable("A");
        test.addVariable("B");
        test.addTerminal("0");
        test.addTerminal("1");
        test.addTerminal("a");
        test.addTerminal("b");

        test.addProduction("S", {"A", "B"}, 0.1);
        test.addProduction("S", {"B", "A", "B"}, 0.9);
        test.addProduction("A", {"a", "A"}, 0.5);
        test.addProduction("A", {"0"}, 0.5);
        test.addProduction("B", {"b", "B"}, 0.5);
        test.addProduction("B", {"1"}, 0.5);

        test.setStartSymbol("S");

        test.exportJSON(output);

        REQUIRE(Contracts::fileCompare(path + "output-happy_day.json", output.str()));
    }
}

TEST_CASE("PCFG : Import JSON") {
    PCFG test;
    std::stringstream err;
    std::ifstream input;

    std::string path = "../Tests/PCFG/json-import/";

    SUBCASE("happy day") {
        input.open(path + "input-happy_day.json");
        test.importJSON(input, err);
        REQUIRE(Contracts::fileCompare(path + "output-happy_day.txt", test.showGrammer()));
        REQUIRE(err.str().empty());
    }
    SUBCASE("duplicate symbols") {
        SUBCASE("duplicate variable") {
            input.open(path + "input-duplicate_variable.json");
            REQUIRE_FALSE(test.importJSON(input, err));
            REQUIRE(Contracts::fileCompare(path + "output-duplicate_variable.txt", err.str()));
        }
        SUBCASE("duplicate terminal") {
            input.open(path + "input-duplicate_terminal.json");
            REQUIRE_FALSE(test.importJSON(input, err));
            REQUIRE(Contracts::fileCompare(path + "output-duplicate_terminal.txt", err.str()));
        }
    }
    SUBCASE("missing head") {
        input.open(path + "input-missing_head.json");
        REQUIRE_FALSE(test.importJSON(input, err));
        REQUIRE(Contracts::fileCompare(path + "output-missing_head.txt", err.str()));
    }
    SUBCASE("missing symbol") {
        input.open(path + "input-missing_symbol.json");
        REQUIRE_FALSE(test.importJSON(input, err));
        REQUIRE(Contracts::fileCompare(path + "output-missing_symbol.txt", err.str()));
    }
    SUBCASE("invalid prob") {
        SUBCASE("") {
            input.open(path + "input-invalid_prob0.json");
            REQUIRE_FALSE(test.importJSON(input, err));
            REQUIRE(Contracts::fileCompare(path + "output-invalid_prob0.txt", err.str()));
        }
        SUBCASE("") {
            input.open(path + "input-invalid_prob1.json");
            REQUIRE_FALSE(test.importJSON(input, err));
            REQUIRE(Contracts::fileCompare(path + "output-invalid_prob1.txt", err.str()));
        }
    }
}