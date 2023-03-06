
#include "Interface.h"
void Interface::run() {
    bool exit = false;
    Evaluator e;
    cracker::Cracker cracker;
    std::cout << "88                                88                              \n"
                 "88                                88                              \n"
                 "88                                88                              \n"
                 "88,dPPYba,  ,adPPYYba,  ,adPPYba, 88   ,d8  ,adPPYba, 8b,dPPYba,  \n"
                 "88P'    \"8a \"\"     `Y8 a8\"     \"\" 88 ,a8\"  a8P_____88 88P'   \"Y8  \n"
                 "88       88 ,adPPPPP88 8b         8888[    8PP\"\"\"\"\"\"\" 88          \n"
                 "88       88 88,    ,88 \"8a,   ,aa 88`\"Yba, \"8b,   ,aa 88          \n"
                 "88       88 `\"8bbdP\"Y8  `\"Ybbd8\"' 88   `Y8a `\"Ybbd8\"' 88 "
              << std::endl
              << std::endl
              << string(100, '-') << std::endl;
    while (!exit) {
        int x = 0;
        mainMenu(x);
        if (x == 0) {
            exit = true;
        } else if (x == 1) {
            evaluating(e);
        } else if (x == 2) {
            cracking(cracker);
        } else if (x == 3) {
            outputting();
        }
    }
}

void Interface::evaluating(Evaluator& e) {
    e.loadModified("../Data/modifiedPCFG.json");
    e.loadWeir("../Data/weirPCFG.json");
    std::cout << "Which evaluating would you like to use?" << std::endl
              << "\t- 1: Weir evaluating" << std::endl
              << "\t- 2: Modified evaluating" << std::endl
              << "\t- 3: Hybrid evaluating" << std::endl
              << "\t- 4: Brute force evaluating" << std::endl
              << "\t- 0: Go back to the main screen" << std::endl;
    int eChoice;
    while (true) {
        std::cin >> eChoice;
        if (eChoice >= 0 and eChoice < 5) {
            break;
        }
        std::cout << "Please pick your desired choice of operation:"
                  << "\t- 1: Weir evaluating" << std::endl
                  << "\t- 2: Modified evaluating" << std::endl
                  << "\t- 3: Hybrid evaluating" << std::endl
                  << "\t- 4: Brute force evaluating" << std::endl
                  << "\t- 0: Go back to the main screen" << std::endl;
    }
    if (eChoice == 0) {
        std::cout << string(100, '-') << std::endl;

        return;
    }
    std::string password;
    std::cout << "Please present the password you would like to evaluate: ";
    std::cin >> password;
    while (password.empty()) {
        std::cout << "Please make sure your password has at least one character." << std::endl;
        std::cin >> password;
    }
    switch (eChoice) {
    case 1: {
        double chance;
        chance = e.weirEvaluate(password);
        std::cout << chance << std::endl;
        break;
    }
    case 2: {
        double chance;
        chance = e.modifiedEvaluate(password);
        std::cout << chance << std::endl;
        break;
    }
    case 3: {
        ifstream inp("../Data/words.txt");
        e.loadWords(inp);

        string inputtest;
        double score = e.hybridEvaluate(password);
        cout << "Final score of " << score << endl;
        break;
    }
    case 4: {
        unsigned long long int comb = 1.0 / Evaluator::bruteforceCheck(password);

        cout << "1 in " << fixed << comb << " chance that " << password << " would randomly be guessed" << endl;
        long double speed = 12000000000;
        cout << "With 2002 achieved record speed, this would take " << comb / speed << " seconds to guess" << endl
             << endl;
        break;
    }
    default: {
        break;
    }
    }
    std::cout << string(100, '-') << std::endl;
}
void Interface::cracking(cracker::Cracker& cracker) {
    std::vector<std::string> generatedPasswords;
    std::cout << "Please pick your desired choice of operation:" << std::endl
              << "\t- 1: Cracking demo" << std::endl
              << "\t- 2: Generate passwords with a word dictionary" << std::endl;
    int cChoice;
    while (true) {
        std::cin >> cChoice;
        if (cChoice < 3 and cChoice > 0) {
            break;
        }
        std::cout << "Please pick your desired choice of operation:" << std::endl
                  << "\t- 1: Cracking demo" << std::endl
                  << "\t- 2: Generate passwords with a word dictionary" << std::endl;
    }
    switch (cChoice) {
    case 1: {
        cracker.loadDictionary("../Data/words10k.txt");
        cracker.loadPCFG("../Data/weirPCFG.json");

        int counter = 0;
        std::cout << "Please present the output file name: ";
        std::string ofile;
        std::cin >> ofile;
        std::string output = "../Output/";
        ofile.insert(ofile.begin(), output.begin(), output.end());
        ofile += ".txt";
        std::ofstream ostream(ofile);

        std::cout << "How many base structures would you like to generate?: ";
        int n;
        std::cin >> n;
        std::cout << "Commencing generation of base structures:\n";
        std::list<std::string> baseStructs;
        ostream << "Base structures: " << std::endl << std::endl;
        while (cracker.good() and counter != n) {
            const auto& s = cracker.nextPreterminal();
            ostream << s << "\n";
            baseStructs.push_back(s);
            counter++;
        }
        std::cout << "Base structure generation completed succesfully\n"
                  << "Commencing password string generation.\n";
        ostream << "\nPasswords:\n\n";

        for (auto& structure : baseStructs) {
            auto pwds = cracker.generate_pStrings(structure, 100, 100000);
            for (auto& pwd : pwds) {
                ostream << pwd << "\n";
            }
        }
        std::cout << "Password string generation completed succesfully\n";
        ostream.close();
        return;
    }
    case 2: {
        std::cout << "Please present the file to load the crackerPCFG: ";
        std::string cpcfg;
        std::cin >> cpcfg;
        cracker.loadPCFG(cpcfg);
        std::cout << "Please present the file to load the word dictionary: ";
        std::string wdic;
        std::cin >> wdic;
        cracker.loadDictionary(wdic);
        std::string base;
        std::cout << "Please present the base string structure for passwords to be generated: ";
        std::cin >> base;
        generatedPasswords = cracker.generate_pStrings(base, 100000000, 10000000);
        std::string printChoice;
        std::cout << "How many generated passwords would you like to see printed?: ";
        int pwds;
        cin >> pwds;
        int printed = 0;
        for (auto i = 0; i < pwds and i < generatedPasswords.size(); i++) {
            std::cout << generatedPasswords[i] << std::endl;
            printed++;
        }
        std::cout << printed << " passwords were printed out on the screen." << std::endl;
    }
    }

    std::cout << string(100, '-') << std::endl;
}
void Interface::outputting() {
    std::cout << "Which PCFG would you like to output?: " << std::endl
              << "\t- 1: Weir PCFG" << std::endl
              << "\t- 2: Modified PCFG" << std::endl
              << "\t- 0: Go back to the main screen" << std::endl;
    int outChoice;
    while (true) {
        cin >> outChoice;
        if (outChoice == 1 or outChoice == 2 or outChoice == 3 or outChoice == 0) {
            break;
        }
        std::cout << "Please pick your desired choice of operation:" << std::endl
                  << "\t- 1: Weir PCFG" << std::endl
                  << "\t- 2: Modified PCFG" << std::endl
                  << "\t- 0: Go back to the main screen" << std::endl;
    }
    switch (outChoice) {
    case 1: {
        std::cout << "Please present the passwordlist dictionary: ";
        std::string in_passwords_file;
        std::cin >> in_passwords_file;
        PCFG* wpcfg = new Converter(in_passwords_file);

        std::cout << "Please present the output file name: ";
        std::string ofile;
        std::cin >> ofile;
        std::string output = "../Output/";
        ofile.insert(ofile.begin(), output.begin(), output.end());
        ofile += ".json";
        std::ofstream ostream(ofile);
        wpcfg->exportJSON(ostream);
        ostream.close();
        break;
    }
    case 2: {
        std::cout << "Please present the passwordlist dictionary: ";
        std::string pwl;
        std::cin >> pwl;
        std::cout << "Please present the Genetic Algorithm config file: ";
        std::string cfgfile;
        std::cin >> cfgfile;
        std::cout << "Please present the output file name: ";
        std::string ofile;
        std::cin >> ofile;
        std::string output = "../Output/";
        ofile.insert(ofile.begin(), output.begin(), output.end());
        ofile += ".json";
        std::ofstream ostream(ofile);
        GeneticGenerator ng(pwl, cfgfile);
        PCFG(ng.Generate(true)).exportJSON(ostream);
        ostream.close();
        break;
    }
    case 0: {
        std::cout << string(100, '-') << std::endl;
        break;
    }
    default: {
        break;
    }
    }
}
void Interface::mainMenu(int& x) {

    std::cout << "Welcome to our hacker-tool." << std::endl
              << "Please pick your desired choice of operation:" << std::endl
              << "\t- 1: Evaluator " << std::endl
              << "\t- 2: Cracker " << std::endl
              << "\t- 3: Write out PCFG's " << std::endl
              << "\t- 0: Exit " << std::endl;
    while (true) {
        std::cin >> x;
        if (x == 1 or x == 2 or x == 3 or x == 0) {
            break;
        }
        std::cout << "Please pick your desired choice of operation:" << std::endl
                  << "\t- 1: Evaluator " << std::endl
                  << "\t- 2: Cracker " << std::endl
                  << "\t- 3: Write out PCFG's " << std::endl
                  << "\t- 0: Exit " << std::endl;
    }
}
