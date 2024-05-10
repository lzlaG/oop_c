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
        virtual Type GetCurrent() const = 0;
};

enum class MutantType : int {Gargoyle, Wolfman, Vampire};

enum class StregthOfHands : int {High, Medium, Low};

enum class StregthOfLegs : int {High, Medium, Low};

enum class Age : int {Old, Young, Newborn};

StregthOfHands GetRandomHandPower();
StregthOfLegs GetRandomLegPower();
Age GetRandomAge();
class Scum
{
    protected:
        StregthOfHands HandPower;
        StregthOfLegs LegPower;
        Age AgeOfMutant;
        Scum()
        {
            HandPower = StregthOfHands(rand()%3);
            LegPower = StregthOfLegs(rand()%3);
            AgeOfMutant = Age(rand()%3);
        };
    public:
        StregthOfHands GetHandPower() {return HandPower;};
        StregthOfLegs GetLegPower() {return LegPower;};
        Age GetAgeOfMutant() {return AgeOfMutant;};
        virtual void Summon() const = 0;
        virtual void Kill() const = 0;
        virtual MutantType GetType() const = 0;
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
        int CurrentId;
        int Count;
        int Pos;
    public:
        UltraWildMutantContainerIterator(sqlite3* db, int count)
        {
            DB = db;
            Pos = 0;
            Count = count;
        };
        void First();
        void Next()
        {
            CurrentId++;
            Pos++;
        };
        bool IsDone() const {return Pos>= Count;};
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
        }
        void AddMutant(ScumPointer newMutant);
        int GetCount() {
            sqlite3_stmt* stmt;
            int result = sqlite3_prepare_v2(DB, "SELECT COUNT(*) FROM Mutants", -1, &stmt, 0);
            result = sqlite3_step(stmt);
            int count = sqlite3_column_int(stmt, 0);
            //MutantCountInDB = count;
            sqlite3_finalize(stmt);
            return count;
        };
        void ClearDB();
        /*
        Iterator<ScumPointer> * GetIterator()
        {
            return new UltraWildMutantContainerIterator(DB, );
        };*/
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
        ScumPointer GetCurrent() const {return *it;}
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
        ScumPointer GetCurrent() const { return ScumCell[Pos];}
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
        Type GetCurrent() const { return It->GetCurrent(); }
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