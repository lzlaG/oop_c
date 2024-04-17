#ifndef ScumH
#define ScumH

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

enum class MutantType : int {Gargoyle, Wolfman, Vampire, Unknown};

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
            HandPower = GetRandomHandPower();
            LegPower = GetRandomLegPower();
            AgeOfMutant = GetRandomAge();
        };
    public:

        StregthOfHands GetHandPower() const{return HandPower;};
        StregthOfLegs GetLegPower() const{return LegPower;};
        Age GetAgeOfMutant() const{return AgeOfMutant;};
        virtual void Summon() const = 0;
        virtual void Kill() const = 0;
        virtual MutantType GetType() const = 0;
};

typedef Scum * ScumPointer;


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

class MutantContainer
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
        ScumPointer GetByIndex(int index) const {return ScumCell[index];}
        Iterator<ScumPointer> * GetIterator()
        {
            return new MutantContainerIterator(ScumCell, MutantCount);
        };
};

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



#endif ScumH