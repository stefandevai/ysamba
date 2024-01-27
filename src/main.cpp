#include "core/game.hpp"

int main()
{
  dl::Game game{};

  game.load();
  game.run();
  return 0;
}
