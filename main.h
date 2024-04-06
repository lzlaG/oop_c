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

enum class Hands {One, Two, Three, Four};

enum class MutantType {Ghoul, Vampire, Crap, Werewolf};

enum class Legs {One, Two, Three, Four};

class Scum
{
    protected:
        bool HasWings;
        Scum();
    public:
        bool IsHasWings() const {return HasWings;};
        virtual Hands GetHandsAmount() const = 0;
        virtual MutantType GetMutantType() const = 0;
        virtual Legs GetLegsAmount() const = 0;
};

typedef Scum * ScumPointer;

#endif ScumH