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

#endif ScumH