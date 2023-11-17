#pragma once

#include <string>
#include "../../graphics/sprite.hpp"

namespace dl
{
    struct Visibility
    {
        std::shared_ptr<Sprite> sprite = nullptr;

        Visibility(const std::string& resource_id, const int frame)
        {
            sprite = std::make_shared<Sprite>(resource_id, frame);
        };
    };
}
