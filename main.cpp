#include <SFML/Graphics.hpp>
#include "game.hpp"

// int main()
// {
//     Game game;
//     game.run();
//     return 0;
// }

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Arkanoid Menu");
    Menu menu(window);
    int wybor = menu.run();
    if (wybor == 1)  // Wczytaj zapis
    {
        Game gra;
        GameState state;
        if (state.loadFromFile("zapis.txt"))
        {
            state.apply(gra.getPaddle(), gra.getBall(), gra.getBricks());
        }
        gra.run();
    }
    return 0;
}