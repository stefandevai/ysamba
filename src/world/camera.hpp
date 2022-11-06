#pragma once

#include "../components/position.hpp"
#include "../components/size.hpp"

namespace dl
{
  class Camera
  {
    public:
      Position position;
      Size size;

      Camera();
      void update(const uint32_t delta);
  };
}
