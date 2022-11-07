#pragma once

#include "../components/position.hpp"
#include "../components/velocity.hpp"

namespace dl
{
  class Body
  {
    public:
      Position position;
      Velocity velocity;
      bool is_static = false;

      Body();
  };
}

