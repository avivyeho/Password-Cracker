

#ifndef MB_TO_GROEP_INTERFACE_H
#define MB_TO_GROEP_INTERFACE_H
#include "../Cracker/Cracker.h"
#include "../Evaluator/Evaluator.h"

class Interface {
private:
    static void evaluating(Evaluator& e);
    static void cracking(cracker::Cracker &cracker);
    static void outputting();
    static void mainMenu(int &x);
public:
    static void run();
};

#endif // MB_TO_GROEP_INTERFACE_H
