#include <iostream>
#include <vector>
#include <list>
# include "main.h"

using namespace std;

class Gargoyle : public Scum
{
    public:
        MutantType GetType() const {return MutantType::Gargoyle;}
        void Summon() { cout << " Призвали горгулью " << "\n";}
        void Kill() { cout << " убили горгулью " << "\n";}
};

class Wolfman : public Scum
{
    public:
        MutantType GetType() const {return MutantType::Wolfman;}
        void Summon() { cout << " Призвали оборотня " << "\n";}
        void Kill() { cout << " убили оборотня " << "\n";}
};

class Vampire : public Scum
{
    public:
        MutantType GetType() const {return MutantType::Vampire;}
        void Summon() { cout << " Призвали вампира " << "\n";}
        void Kill() { cout << " убили вампира" << "\n";}
};
int main()
{
    cout << "hello world" << "\n";
};