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
        Biology(const Sex sex, const double base_speed) : sex(sex), base_speed(base_speed), current_speed(base_speed) {}

        /* double age_in_days; */
        /* double height_in_cm; */
        /* double mass_in_g; */
        Sex sex;

        // STR:10;END:10;QI:10;SOCIAL:INTRO;
        /* std::string genetics; */
        double base_speed;
        double current_speed;
    };
}

