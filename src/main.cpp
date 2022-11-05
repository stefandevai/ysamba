#include "./core/game.hpp"

int main(int argc, char* argv[])
{
  dl::Game game{};

  game.load(argc, argv);
  game.run();
}
