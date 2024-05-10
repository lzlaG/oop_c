#ifndef ScumH
#define ScumH
#include <vector>
#include "sqlite3.h"
#include <string>
using namespace std;

template<class Type>
class Iterator
{
    protected:
        Iterator() {}
    public:
        virtual ~Iterator(){};
        virtual void First() = 0 ;
        virtual void Next() = 0;
        virtual bool IsDone() const = 0;
        virtual Type GetCurrent() = 0;
};

enum class MutantType : int {Gargoyle, Wolfman, Vampire};

enum class StregthOfHands : int {High, Medium, Low};

enum class StregthOfLegs : int {High, Medium, Low};

enum class Age : int {Old, Young, Newborn};

class Scum
{
    protected:
        StregthOfHands HandPower;
        StregthOfLegs LegPower;
        Age AgeOfMutant;
        MutantType TypeOfMutant;
        Scum(int id_of_hand = rand()%3, int id_of_leg = rand()%3, int id_of_age = rand()%3)
        {
            HandPower = StregthOfHands(id_of_hand);
            LegPower = StregthOfLegs(id_of_leg);
            AgeOfMutant = Age(id_of_age);
        };
    public:
        MutantType GetType() const {return TypeOfMutant;};
        StregthOfHands GetHandPower() {return HandPower;};
        StregthOfLegs GetLegPower() {return LegPower;};
        Age GetAgeOfMutant() {return AgeOfMutant;};
        virtual void Summon() const = 0;
        virtual void Kill() const = 0;
};

typedef Scum * ScumPointer;

class ScumContainer
{
    public:
        virtual void AddMutant(ScumPointer newMutant) = 0;
        virtual int GetCount() = 0;
};

class UltraWildMutantContainerIterator : public Iterator<ScumPointer>
{
    private:
        sqlite3* DB;
        int FirstId;
        int Count;
        int PosDB;
    public:
        UltraWildMutantContainerIterator(sqlite3* db, int count)
        {
            DB = db;
            Count = count;
        };
        void First() {PosDB = 0;};
        void Next() {PosDB+=1;}
        bool IsDone() const {return PosDB >= Count;};
        ScumPointer GetCurrent();
};

class UltraWildMutantContainer : public ScumContainer
{
    private:
        sqlite3* DB;
        int MutantCountInDB;
    public:
        UltraWildMutantContainer(const string& DB_path)
        {
            sqlite3_open(DB_path.c_str(), &DB);
            string createtable = "CREATE TABLE Mutants ("
                                "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                                "MutantType INTEGER,"
                                "StrengthOfHands INTEGER,"
                                "StrengthOfLegs INTEGER,"
                                "Age INTEGER"
                                ");";
            char *errMsg;
            sqlite3_exec(DB, createtable.c_str(), nullptr, nullptr, &errMsg);
            cout << errMsg << "\n";
            MutantCountInDB = 0;
        }
        void AddMutant(ScumPointer newMutant);
        int GetCount() { return MutantCountInDB;}
        void ClearDB();
        Iterator<ScumPointer> * GetIterator()
        {
            return new UltraWildMutantContainerIterator(DB, MutantCountInDB);
        };
};
// ------------------- container and iterator based on vector ------------------
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
        ScumPointer GetCurrent() {return *it;}
};

class WildMutantContainer : public ScumContainer
{
    private:
        vector<ScumPointer> ScumCell;
    public:
        void AddMutant(ScumPointer newMutant) {ScumCell.push_back(newMutant);}
        int GetCount() {return ScumCell.size();}
        Iterator<ScumPointer> *GetIterator()
        {
            return new WildMutantContainerIterator(&ScumCell);
        };
};

//------------------ container and iterator based on list -------------------------
class MutantContainerIterator : public Iterator<ScumPointer>
{
    private:
        const ScumPointer *ScumCell;
        int Pos;
        int Count;
    public:
        MutantContainerIterator(const ScumPointer *scumcell, int count)
        {
            ScumCell = scumcell;
            Count = count;
            Pos = 0;
        };
        void First() { Pos = 0; }
        void Next() { Pos++; }
        bool IsDone() const { return Pos >= Count;}
        ScumPointer GetCurrent() { return ScumCell[Pos];}
};

class MutantContainer : public ScumContainer
{
    private:
        ScumPointer * ScumCell;
        int MutantCount;
        int MaxSize;
    public:
        MutantContainer(int MaxSize);
        virtual ~MutantContainer();
        void AddMutant(ScumPointer newMutant);
        int GetCount() const {return MutantCount; };
        ScumPointer GetByIndex(int index) {return ScumCell[index];}
        Iterator<ScumPointer> * GetIterator()
        {
            return new MutantContainerIterator(ScumCell, MutantCount);
        };
};

template<class Type>
class Decorator : public Iterator<Type>
{
    protected:
        Iterator<Type> *It;
    public:
        Decorator (Iterator<Type> *it)
        {
            It = it;
        }
        virtual ~Decorator() {delete It;}
        void First() {It->First();}
        void Next() {It->Next();}
        bool IsDone() const {return It->IsDone();}
        Type GetCurrent() { return It->GetCurrent(); }
};

class DecoratorLegPower : public Decorator<ScumPointer>
{
    private:
        StregthOfLegs TargetLegs;
    public:
        DecoratorLegPower(Iterator<ScumPointer> *it, StregthOfLegs targetLegs) : Decorator(it)
        {
            TargetLegs = targetLegs;
        }
        void First()
        {
            It->First();//позиционируем в начало итератор
            while(!It->IsDone() && It-> GetCurrent()->GetLegPower() != TargetLegs)
            {
                It->Next();
            }
        }
        void Next()
        {
            do
            {
                It->Next();

            }while(!It->IsDone() && It->GetCurrent()->GetLegPower()!= TargetLegs);
        }
};

class DecoratorHandsPower : public Decorator<ScumPointer>
{
    private:
        StregthOfHands TargetHands;
    public:
        DecoratorHandsPower(Iterator<ScumPointer> *it, StregthOfHands targetHands) :Decorator(it)
        {
            TargetHands = targetHands;
        }
        void First()
        {
            It->First();//позиционируем в начало итератор
            while(!It->IsDone() && It-> GetCurrent()->GetHandPower() != TargetHands)
            {
                It->Next();
            }
        }
        void Next()
        {
            do
            {
                It->Next();

            }while(!It->IsDone() && It->GetCurrent()->GetHandPower()!= TargetHands);
        }
};

class DecoratorAge : public Decorator<ScumPointer>
{
    private:
        Age TargetAge;
    public:
        DecoratorAge(Iterator<ScumPointer> *it, Age targetAge ) : Decorator(it)
        {
            TargetAge = targetAge;
        }
        void First()
        {
            It->First();
            while(!It->IsDone() && It-> GetCurrent()->GetAgeOfMutant() != TargetAge)
            {
                It->Next();
            }
        }
        void Next()
        {
            do
            {
                It->Next();

            }while(!It->IsDone() && It->GetCurrent()->GetAgeOfMutant()!= TargetAge);
        }
};

class DecoratorType : public Decorator<ScumPointer>
{
    private:
        MutantType TargetType;
    public:
        DecoratorType(Iterator<ScumPointer> *it, MutantType targetType ) : Decorator(it)
        {
            TargetType = targetType;
        }
        void First()
        {
            It->First();
            while(!It->IsDone() && It-> GetCurrent()->GetType() != TargetType)
            {
                It->Next();
            }
        }
        void Next()
        {
            do
            {
                It->Next();

            }while(!It->IsDone() && It->GetCurrent()->GetType()!= TargetType);
        }
};
#endif ScumH