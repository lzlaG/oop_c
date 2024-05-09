#include <iostream>
#include <vector>
#include <list>
#include "main.h"
#include "sqlite3.h"

using namespace std;

StregthOfHands GetRandomHandPower()
{
    switch (random()%3)
    {
        case 0:
            return StregthOfHands::High;
        case 1:
            return StregthOfHands::Medium;
        case 2:
            return StregthOfHands::Low;
    };
};

StregthOfLegs GetRandomLegPower()
{
    switch (random()%3)
    {
        case 0:
            return StregthOfLegs::High;
        case 1:
            return StregthOfLegs::Medium;
        case 2:
            return StregthOfLegs::Low;
    }; 
};

Age GetRandomAge()
{
    switch (random()%3)
    {
        case 0:
            return Age::Young;
        case 1:
            return Age::Old;
        case 2:
            return Age::Newborn;
    }; 
};

class Gargoyle : public Scum
{
    public:
        MutantType GetType() const {return MutantType::Gargoyle;};
        void Fly(int x) const {cout << "Горгулья пролетела " << x << " км" << endl;}
        void Summon()   const { cout << " Призвали горгулью " << endl;};
        void Kill() const { cout << " убили горгулью " << endl;};
};

class Wolfman : public Scum
{
    public:
        MutantType GetType() const {return MutantType::Wolfman;};
        void Run(int x) { cout << "Оборотень пробежал " << x << " км " << endl;}
        void Summon() const { cout << " Призвали оборотня " << "\n";};
        void Kill() const { cout << " убили оборотня " << "\n";};
};

class Vampire : public Scum
{
    public:
        MutantType GetType() const {return MutantType::Vampire;}
        void Blood(int x) {cout << "Вампир выпил " << x << " литров крови " << endl;}
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

string PrintAgeOfMutant(const Age type)
{
    switch(type)
    {
        case Age::Old: return "ПОЖИЛОЙ";
        case Age::Young: return "МОЛОДОЙ";
        case Age::Newborn: return "Новорожденный";
    }
};

class WildMutantContainerIterator : public Iterator<ScumPointer>
{
    private:
        const vector<ScumPointer> * ScumCell;
        vector<ScumPointer>::const_iterator it;
    public:
        WildMutantContainerIterator (const vector<ScumPointer> * scumcell)
        {
            ScumCell = scumcell;
            it = ScumCell->begin();
        }
        void First() { it = ScumCell->begin();}
        void Next() {it++;}
        bool IsDone() const {return it == ScumCell->end();}
        ScumPointer GetCurrent() const {return *it;}
};

int DBMutantContainerIterator::GetCount()
{
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(DB, "SELECT COUNT(*) FROM Mutants", -1, &stmt, 0);
    result = sqlite3_step(stmt);
    int count = sqlite3_column_int(stmt, 0);
    Count = count;
    return count;
};

void DBMutantContainerIterator::First()
{
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id FROM Mutants ORDER BY id ASC LIMIT 1;";
    int rc = sqlite3_prepare_v2(DB, sql, -1, &stmt, nullptr);
    rc = sqlite3_step(stmt);
    CurrentId = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    cout << "ID первой записи:" << CurrentId << "\n";
};

void DBMutantContainerIterator::GetCurrent()
{
    sqlite3_stmt* pStmt = nullptr;
    string outStr;
    string sql2("SELECT `StrengthOfHands` FROM `Mutants` WHERE `ID` = ?1;");
    int result = sqlite3_prepare_v2(DB, sql2.c_str(), -1, &pStmt, nullptr);
    if (result != SQLITE_OK) {
        cout << "Prepare error\n";
    };
    result = sqlite3_bind_int(pStmt, 1 /*?1*/, CurrentId);
    if (result != SQLITE_OK) {
        cout << "Bind text error\n";
    };
    result = sqlite3_step(pStmt);
    outStr.assign(reinterpret_cast<const char*>(sqlite3_column_text(pStmt, 0)));
    cout << "Result:" << outStr << "\n";
};

class WildMutantContainer : public ScumContainer
{
    private:
        vector<ScumPointer> ScumCell;
    public:
        void AddMutant(ScumPointer newMutant) {ScumCell.push_back(newMutant);}
        int GetCount() const {return ScumCell.size();}
        Iterator<ScumPointer> *GetIterator()
        {
            return new WildMutantContainerIterator(&ScumCell);
        };
};
/*
string quotesql( const string& s ) {
    return string("'") + s + string("'");
};*/

string PrintLegPower (const StregthOfLegs type)
{
    switch (type)
    {
        case StregthOfLegs::Low: return "Слабые ноги";
        case StregthOfLegs::Medium: return "Средние ноги";
        case StregthOfLegs::High: return "Сильные ноги";
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
/*
void DBMutantContainer::ClearDB()
{
    char *errmsg;
    sqlite3_exec(DB,"DELETE FROM Mutants", NULL, NULL, &errmsg);
};

void DBMutantContainer::AddMutant(ScumPointer newMutant)
{
    string mutanttype = PrintMutantType(newMutant->GetType());
    string handpower = PrintHandPower(newMutant->GetHandPower());
    string legpower = PrintLegPower(newMutant->GetLegPower());
    string ageofmutant = PrintAgeOfMutant(newMutant->GetAgeOfMutant());
    string query = "INSERT INTO Mutants (MutantType, StrengthOfHands, StrengthOfLegs, Age) VALUES ("
    +quotesql(mutanttype)+","
    +quotesql(handpower)+","
    +quotesql(legpower)+","
    +quotesql(ageofmutant)+");";
    char *errmsg;
    sqlite3_exec(DB, query.c_str(), NULL, NULL, &errmsg);
    cout << errmsg << "\n";
};
*/
void SelfSpec (ScumPointer currentMutant)
{
    ;
    int x = random()%(5000-10+1)+1;
    if(currentMutant->GetType() == MutantType::Wolfman)
    {
        Wolfman* wolfman = dynamic_cast<Wolfman*>(currentMutant);
        wolfman->Run(x);
    }
    if (currentMutant->GetType() == MutantType::Vampire)
    {
        Vampire* vampire = dynamic_cast<Vampire*>(currentMutant);
        vampire->Blood(x);
    }
    if (currentMutant->GetType() == MutantType::Gargoyle)
    {
        Gargoyle* gargoyle = dynamic_cast<Gargoyle*>(currentMutant);
        gargoyle->Fly(x);
    }
}

void ItogTask(Iterator<ScumPointer> *it)
{
    for(it->First(); !it->IsDone(); it->Next())
    {
        const ScumPointer currentMutant = it->GetCurrent();
        //SelfSpec(currentMutant); 
        cout << "----------------------------------" << "\n";
        cout << PrintMutantType(currentMutant->GetType()) << "\n";
        cout << PrintHandPower(currentMutant->GetHandPower()) << "\n";
        cout << PrintLegPower(currentMutant->GetLegPower()) << "\n";
        PrintAgeOfMutant(currentMutant->GetAgeOfMutant());

    }
}

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
    DBMutantContainerIterator it("mydb.db");
    it.First();
    it.GetCurrent();
    cout << it.GetCount() << "\n";
    cout << it.IsDone() << "\n";
    /*
    int random_amount_of_mutant = random()%(100-10+1)+1;
    cout << "Генерируем " << random_amount_of_mutant << " мутантов" << "\n";
    for (int i=0; i<=random_amount_of_mutant; i++)
    {
        scumcell.AddMutant(MutantFactory(MutantType(rand()%3)));
    };
    Iterator<ScumPointer> *it =  scumcell.GetIterator();
    //PrintMutantType()
    Iterator<ScumPointer> *sorting_it = new DecoratorAge(it, Age::Old);
    for(;;)
    {
        int choise;
        cout << "Вывести отсортированный или нет?(1,2-выбор вывода 3-выход)" << "\n";
        cin >> choise;
        if (choise == 3)
        {
            break;
        }
        switch(choise)
        {
            case 1:
                cout << "Вывод отсортированного " << "\n";
                ItogTask(sorting_it);
                break;
            case 2:
                cout << "Вывод несортированного " << "\n";
                ItogTask(it);
                break;
        }
    }
    //Kill_vampires(it);
    */
};