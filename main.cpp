#include <iostream>
#include <vector>
#include <list>
#include "main.h"
#include "sqlite3.h"

using namespace std;

// -------------- define classes ---------------
class Gargoyle : public Scum
{
    public:
        Gargoyle(int id_of_hand = rand()%3, int id_of_leg = rand()%3, int id_of_age = rand()%3 ) : Scum(id_of_hand,id_of_leg,id_of_age)
        {
            TypeOfMutant = MutantType::Gargoyle;
        };
        void Summon()   const { cout << " Призвали горгулью " << endl;};
        void Kill() const { cout << " убили горгулью " << endl;};
};

class Wolfman : public Scum
{
    public:
        Wolfman() : Scum()
        {
            TypeOfMutant = MutantType::Wolfman;
        }
        void Summon() const { cout << " Призвали оборотня " << "\n";};
        void Kill() const { cout << " убили оборотня " << "\n";};
};

class Vampire : public Scum
{
    public:
        Vampire() : Scum()
        {
            TypeOfMutant = MutantType::Vampire;
        }
        void Summon() const { cout << " Призвали вампира " << "\n";}
        void Kill() const { cout << " убили вампира" << "\n";}
};

// -------- functions for container based on sqlite ----
void UltraWildMutantContainer::ClearDB()
{
    char *errmsg;
    sqlite3_exec(DB,"DELETE FROM Mutants", NULL, NULL, &errmsg);
};
void UltraWildMutantContainer::AddMutant(ScumPointer newMutant)
{
    int mutanttype = (int)newMutant->GetType(); 
    int handpower = (int)newMutant->GetHandPower();
    int legpower = (int)newMutant->GetLegPower();
    int ageofmutant = (int)newMutant->GetAgeOfMutant();
    sqlite3_stmt* stmt;
    string insert_query = "INSERT INTO Mutants (MutantType,StrengthOfHands,StrengthOfLegs, Age)"
                            "VALUES (:mutanttype,:power_of_hands,:power_of_legs, :age);";
    sqlite3_prepare_v2(DB, insert_query.c_str(), -1, &stmt, NULL);
    sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, ":mutanttype"), mutanttype);
    sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, ":power_of_hands"), handpower);
    sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, ":power_of_legs"), legpower);
    sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, ":age"), ageofmutant);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
};

//-------- functions for iterator based on sqlite container ---------
void UltraWildMutantContainerIterator::First()
{
    sqlite3_stmt *stmt;
    const char *sql = "SELECT ID FROM Mutants ORDER BY ID ASC LIMIT 1;";
    int rc = sqlite3_prepare_v2(DB, sql, -1, &stmt, nullptr);
    rc = sqlite3_step(stmt);
    CurrentId = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
};

ScumPointer UltraWildMutantContainerIterator::GetCurrent()
{
    /*sqlite3_stmt* stmt;
    const char *get_data = "SELECT (MutantType,StrengthOfHands,StrengthOfLegs,Age) FROM Mutants WHERE ID = ?;";
    int rc = sqlite3_prepare_v2(DB, get_data, -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, CurrentId);
    rc = sqlite3_step(stmt);
    int id_of_mutant = sqlite3_column_int(stmt,0);*/
    ScumPointer current = new Gargoyle(1,1,1);
    return current;
};

//--------- functions for container based on list -------------- 
void MutantContainer::AddMutant(ScumPointer newMutant)
{
    ScumCell[MutantCount] = newMutant;
    MutantCount++;
}

MutantContainer::MutantContainer(int maxSize)
{
    ScumCell = new ScumPointer[maxSize];
    for (int i = 0; i<maxSize; i++)
    {
        ScumCell[i] = NULL;
    };
    MutantCount = 0;
    MaxSize = maxSize;
};

MutantContainer::~MutantContainer()
{
    for (int i = 0; i<MaxSize; i++)
    {
        if( ScumCell[i] != NULL)
        {
            delete ScumCell[i];
            ScumCell[i] = NULL;
        };
    };
    delete[] ScumCell;
};

// --------- prints of parametrs -------------------
string PrintLegPower (const StregthOfLegs type)
{
    switch (type)
    {
        case StregthOfLegs::Low: return "Слабые ноги";
        case StregthOfLegs::Medium: return "Средние ноги";
        case StregthOfLegs::High: return "Сильные ноги";
    }
}

string PrintMutantType(const MutantType type)
{
    switch(type)
	{
		case MutantType::Gargoyle: return  "ГОРГУЛЬЯ";
		case MutantType::Vampire: return  "ВАМПИР";
		case MutantType::Wolfman: return  "ОБОРОТЕНЬ";
	}
}

string PrintAgeOfMutant(const Age type)
{
    switch(type)
    {
        case Age::Old: return "ПОЖИЛОЙ";
        case Age::Young: return "МОЛОДОЙ";
        case Age::Newborn: return "Новорожденный";
    }
}

string PrintHandPower (const StregthOfHands type)
{
    switch (type)
    {
        case StregthOfHands::Low: return "Слабые руки";
        case StregthOfHands::Medium: return "Средние руки";
        case StregthOfHands::High: return "Мощные руки";
    }
}

// --------- function for task --------------
void ItogTask(Iterator<ScumPointer> *it)
{
    for(it->First(); !it->IsDone(); it->Next())
    {
        const ScumPointer currentMutant = it->GetCurrent(); 
        cout << "----------------------------------" << "\n";
        cout << PrintMutantType(currentMutant->GetType()) << "\n";
        cout << PrintHandPower(currentMutant->GetHandPower()) << "\n";
        cout << PrintLegPower(currentMutant->GetLegPower()) << "\n";
        cout << PrintAgeOfMutant(currentMutant->GetAgeOfMutant()) << "\n";
    }
}

// ----------- factory method ---------------
Scum *MutantFactory(MutantType newMutant)
{
    switch(newMutant)
    {
        case MutantType::Gargoyle: return  new Gargoyle;
		case MutantType::Vampire: return  new Vampire;
		case MutantType::Wolfman: return  new Wolfman;
    }
}

int main()
{
    srand(time(NULL));

    //MutantContainer scumcell(100);
    //WildMutantContainer scumcell;
    UltraWildMutantContainer scumcell("mutant.db");
    scumcell.ClearDB();
    int random_amount_of_mutant = random()%(100-10+1)+1;
    cout << "Генерируем " << random_amount_of_mutant << " мутантов" << "\n";
    for (int i=0; i<random_amount_of_mutant; i++)
    {
        scumcell.AddMutant(MutantFactory(MutantType(rand()%3)));
    };
    scumcell.GetCount();
    Iterator<ScumPointer> *it =  scumcell.GetIterator();
    ItogTask(it);
    //Kill_vampires(it);*/
};
