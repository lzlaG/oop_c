#include <iostream>
#include <vector>
#include <list>
# include "main.h"

using namespace std;

StregthOfHands GetRandomHandPower()
{
    int x = random()%(3-1+1)+1;
    switch (x)
    {
        case 1:
            return StregthOfHands::High;
        case 2:
            return StregthOfHands::Medium;
        case 3:
            return StregthOfHands::Low;
    };
};

StregthOfLegs GetRandomLegPower()
{
   int x = random()%(3-1+1)+1;
    switch (x)
    {
        case 1:
            return StregthOfLegs::High;
        case 2:
            return StregthOfLegs::Medium;
        case 3:
            return StregthOfLegs::Low;
    }; 
};

Age GetRandomAge()
{
    int x = random()%(3-1+1)+1;
    switch (x)
    {
        case 1:
            return Age::Young;
        case 2:
            return Age::Old;
        case 3:
            return Age::Newborn;
    }; 
};

class Gargoyle : public Scum
{
    public:
        MutantType GetType() const {return MutantType::Gargoyle;};
        void Summon()   const { cout << " Призвали горгулью " << endl;};
        void Kill() const { cout << " убили горгулью " << endl;};
};

class Wolfman : public Scum
{
    public:
        MutantType GetType() const {return MutantType::Wolfman;};
        void Summon() const { cout << " Призвали оборотня " << "\n";};
        void Kill() const { cout << " убили оборотня " << "\n";};
};

class Vampire : public Scum
{
    public:
        MutantType GetType() const {return MutantType::Vampire;}
        void Summon() const { cout << " Призвали вампира " << "\n";}
        void Kill() const { cout << " убили вампира" << "\n";}
};

string PrintMutantType(const MutantType type)
{
    switch(type)
	{
		case MutantType::Gargoyle: return  "ГОРГУЛЬЯ";
		case MutantType::Vampire: return  "ВАМПИР";
		case MutantType::Wolfman: return  "ОБОРОТЕНЬ";
		default: return "неизвестный";
	}
};

void Task1(MutantContainer *scumcell)
{
    for (int i=0; i<scumcell->GetCount(); i++)
    {
        const ScumPointer currentMutant = scumcell->GetByIndex(i);
        if (currentMutant->GetType() == MutantType::Wolfman)
        {
            currentMutant->Kill(); 
        };
    };
};

void Task1_it(Iterator<ScumPointer> * it)
{
    for (it->First(); !it->IsDone(); it->Next())
    {
        const ScumPointer currentMutant = it->GetCurrent();
        if(currentMutant->GetType() == MutantType::Wolfman)
        {
            currentMutant->Kill();
        };
    };
};

int main()
{
    //cout << "hello world" << "\n";
    MutantContainer scumcell(100);
    for (int i = 0; i<25; i++)
    {
        scumcell.AddMutant(new Wolfman);
    };
    for (int i = 26; i<25; i++)
    {
        scumcell.AddMutant(new Vampire);
    };
    for (int i = 52; i<40; i++)
    {
        scumcell.AddMutant(new Gargoyle);
    };
    //Task1(&scumcell);
    Iterator<ScumPointer> *it = scumcell.GetIterator();
    Task1_it(it);
};