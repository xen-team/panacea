#include "game.hpp"
#include "scenes/forest.hpp"

using namespace panacea;

int main()
{

    Game panacea_game;

    panacea_game.load_scene(std::make_unique<ForestScene>());

    panacea_game.run();

    return EXIT_SUCCESS;
}