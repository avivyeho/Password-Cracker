

#include "../../Source/Cracker/Cracker.h"

TEST_CASE("Cracker : Next") {

    std::stringstream out;
    std::string path = "../Tests/Cracker/next/";

    std::vector<std::string> tests = {"test1", "test2"};
    for (int i = 0; i < 2; ++i) {
        bool deadbeat = i == 1;
        cracker::Cracker cracker(deadbeat);
        for (const auto& test : tests) {
            std::string name = (deadbeat ? "Deadbeat Dad - " : "Next - ") + test;
            SUBCASE(name.c_str()) {
                std::ifstream json(path + test + ".json");
                cracker.loadPCFG(json);
                json.close();

                while (cracker.good()) {
                    out << cracker.nextPreterminal() << std::endl;
                }
                REQUIRE(Contracts::fileCompare(path + test + (deadbeat ? "dd" : "") + ".txt", out.str()));
            }
        }
    }
}