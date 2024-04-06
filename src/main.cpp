#include "core/game.hpp"

auto main() -> int
{
  dl::Game game{};

  game.load();
  game.run();
  return 0;
}
