#pragma once

namespace dl
{
    enum SocialClass
    {
        None,
        Slave,
        Plebeian,
        Ruler,
    };

    enum Metier
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

    struct SocietyMember
    {
        std::string society_id;
        SocialClass social_class;
        Metier metiers;
    };
}


