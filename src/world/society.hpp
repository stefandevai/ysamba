#pragma once

#include <string>

namespace dl
{
    enum Predominance
    {
        Patriarchy,
        Matriarchy,
    };

    enum SocietyType
    {
        Nomadic,
        Sedentary,
    };

    enum ModeOfProduction
    {
        PrimitiveCommunism,
        Slavery,
    };

    struct Society
    {
        std::string id;
        std::string name;
        unsigned int age;
        Predominance predominance;
        SocietyType type;
        ModeOfProduction mode_of_production;

        template<class Archive> 
        void serialize(Archive& archive)
        {
          archive(id, name, age, predominance, type, mode_of_production);
        }
    };
}

