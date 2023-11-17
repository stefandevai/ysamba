#pragma once

#include <glm/glm.hpp>

namespace dl
{
  struct Transform
  {
    Transform() {}

    glm::vec3 rotation = glm::vec3 (0.f);
    glm::vec3 scale = glm::vec3 (1.f);
    glm::vec3 pivot = glm::vec3 (0.f);
  };
}

