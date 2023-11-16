#pragma once

namespace dl
{
    enum Sex
    {
        Male,
        Female,
    };

    struct Biology
    {
        double age_in_days;
        double height_in_cm;
        double mass_in_g;
        Sex sex;

        // STR:10;END:10;QI:10;SOCIAL:INTRO;
        std::string genetics;
    };
}

