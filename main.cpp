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
        Wolfman(int id_of_hand = rand()%3, int id_of_leg = rand()%3, int id_of_age = rand()%3 ) : Scum(id_of_hand,id_of_leg,id_of_age)
        {
            TypeOfMutant = MutantType::Wolfman;
        }
        void Summon() const { cout << " Призвали оборотня " << "\n";};
        void Kill() const { cout << " убили оборотня " << "\n";};
};

class Vampire : public Scum
{
    public:
        Vampire(int id_of_hand = rand()%3, int id_of_leg = rand()%3, int id_of_age = rand()%3 ) : Scum(id_of_hand,id_of_leg,id_of_age)
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
    MutantCountInDB++;
};

//-------- functions for iterator based on sqlite container ---------
ScumPointer UltraWildMutantContainerIterator::GetCurrent()
{
    
    sqlite3_stmt* stmt;
    
    const char *sql = "SELECT ID FROM Mutants ORDER BY ID ASC LIMIT 1;";
    int rc = sqlite3_prepare_v2(DB, sql, -1, &stmt, nullptr);
    rc = sqlite3_step(stmt);
    FirstId = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    ScumPointer current;
    int needed_id = FirstId+PosDB;
    const char *get_data = "SELECT MutantType,StrengthOfHands,StrengthOfLegs,Age FROM Mutants WHERE ID = ?;";
    rc = sqlite3_prepare_v2(DB, get_data, -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, needed_id);
    rc = sqlite3_step(stmt);
    int id_of_mutant = sqlite3_column_int(stmt,0);
    int hand_id = sqlite3_column_int(stmt,1);
    int leg_id = sqlite3_column_int(stmt, 2);
    int age_id = sqlite3_column_int(stmt, 3);
    switch(id_of_mutant)
    {
        case 0:
            current = new Gargoyle(hand_id,leg_id,age_id);
            break;
        case 1:
            current = new Wolfman(hand_id,leg_id,age_id);
            break;
        case 2:
            current = new Vampire(hand_id,leg_id,age_id);
            break;
    }
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
    int user_choice;
    for(;;)
    {
        cout << "Выберите тип контейнера: \n";
        cout << "1. Массив\n";
        cout << "2. Вектор\n";
        cout << "3. SQLITE\n";
        cout << "4. Выйти из программы\n";
        cout << "Ваш выбор: ";
        cin >> user_choice;
        if (user_choice == 4)
        {
            break;
        };

        MutantType mutant_choice;
        int z;
        cout << "Выберите тип мутанта: \n";
        cout << "1. ГОРГУЛЬЯ\n";
        cout << "2. ОБОРОТЕНЬ\n";
        cout << "3. ВАМПИР\n";
        cout << "Ваш выбор: ";
        cin >> z;
        switch(z)
        {
            case 1:
                mutant_choice = MutantType::Gargoyle;
                break;
            case 2:
                mutant_choice = MutantType::Wolfman;
                break;
            case 3:
                mutant_choice = MutantType::Vampire;
                break;
        };

        StregthOfHands power_hand_choice;
        cout << "Выберите силу рук: \n";
        cout << "1. Мощные руки\n";
        cout << "2. Средние руки\n";
        cout << "3. Слабые руки\n";
        cout << "Ваш выбор: ";
        cin >> z;
        switch(z)
        {
            case 1:
                power_hand_choice = StregthOfHands::High;
                break;
            case 2:
                power_hand_choice = StregthOfHands::Medium;
                break;
            case 3:
                power_hand_choice = StregthOfHands::Low;
                break;
        };

        StregthOfLegs power_leg_choice;
        cout << "Выберите силу рук: \n";
        cout << "1. Мощные ноги\n";
        cout << "2. Средние ноги\n";
        cout << "3. Слабые ноги\n";
        cout << "Ваш выбор: ";
        cin >> z;
        switch(z)
        {
            case 1:
                power_leg_choice = StregthOfLegs::High;
                break;
            case 2:
                power_leg_choice = StregthOfLegs::Medium;
                break;
            case 3:
                power_leg_choice = StregthOfLegs::Low;
                break;
        };

        Age age_choice;
        cout << "Выберите силу рук: \n";
        cout << "1. ПОЖИЛОЙ\n";
        cout << "2. МОЛОДОЙ\n";
        cout << "3. НОВОРОЖДЕННЫЙ\n";
        cout << "Ваш выбор: ";
        cin >> z;
        switch(z)
        {
            case 1:
                age_choice = Age::Old;
                break;
            case 2:
                age_choice = Age::Young;
                break;
            case 3:
                age_choice = Age::Newborn;
                break;
        };

        Iterator<ScumPointer> *OurIterator;
        int random_amount_of_mutant = rand()%(100-10+1)+1;
        cout << "Генерируем " << random_amount_of_mutant << " мутантов" << "\n";
        if (user_choice == 1)
        {
            MutantContainer scumcell_list(random_amount_of_mutant);
            for(int i=0; i<random_amount_of_mutant; i++)
            {
                scumcell_list.AddMutant(MutantFactory(MutantType(rand()%3)));
            };
            OurIterator = new DecoratorType(
                new DecoratorLegPower(
                new DecoratorHandsPower(
                new DecoratorAge(scumcell_list.GetIterator(), age_choice), power_hand_choice), power_leg_choice), mutant_choice);
            ItogTask(OurIterator);
        };
        if(user_choice == 2)
        {
            WildMutantContainer scumcell_vector;
            for(int i=0; i<random_amount_of_mutant; i++)
            {
                scumcell_vector.AddMutant(MutantFactory(MutantType(rand()%3)));
            };
            OurIterator = new DecoratorType(
                new DecoratorLegPower(
                new DecoratorHandsPower(
                new DecoratorAge(scumcell_vector.GetIterator(), age_choice), power_hand_choice), power_leg_choice), mutant_choice);
            ItogTask(OurIterator);
        };
        if( user_choice == 3)
        {
            string path_to_db;
            cout << "Введите путь до базы данных: ";
            cin >> path_to_db;
            UltraWildMutantContainer scumcell_sqlite(path_to_db);
            scumcell_sqlite.ClearDB();
            for(int i=0; i<random_amount_of_mutant; i++)
            {
                scumcell_sqlite.AddMutant(MutantFactory(MutantType(rand()%3)));
            };
            OurIterator = new DecoratorType(
                new DecoratorLegPower(
                new DecoratorHandsPower(
                new DecoratorAge(scumcell_sqlite.GetIterator(), age_choice), power_hand_choice), power_leg_choice), mutant_choice);
            ItogTask(OurIterator);
        };
    };
};
