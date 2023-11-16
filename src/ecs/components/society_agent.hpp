#pragma once

namespace dl
{
    enum class SocialClass
    {
        None,
        Slave,
        Plebeian,
        Ruler,
    };

    enum class Metier
    {
        None,
        Farmer,
        Warrior,
        Priest,
        Bricklayer,
        Dressmaker,
        Cook,
        Cacique,
    };


    struct SocietyAgent
    {
    private:
        enum class State
        {
            Idle,
            Walking,
        };

    public:
        std::string id;
        std::string society_id;
        std::string name;
        SocialClass social_class;
        Metier metiers;
        State state = State::Idle;
    };
}


