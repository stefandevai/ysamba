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
        double height;
        double weight;
        Sex sex;

        // STR:10;END:10;QI:10;SOCIAL:INTRO;
        std::string genetics;
    };
}

