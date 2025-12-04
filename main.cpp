#include <SFML/Graphics.hpp>
#include "game.hpp"

// stara wersja - gra uruchamiana bez menu
// int main()
// {
//     Game game;
//     game.run();
//     return 0;
// }

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Arkanoid v4.00");
    Menu menu(window);
    menu.run();
    return 0;

    // stara wersja - funkcja przekazywala wybor do zmiennej; teraz wczytywanie zapisu jest wykonywane wewnatrz funkcji
    // int wybor = menu.run();
    // if (wybor == 1)  // Wczytaj zapis
    // {
    //     Game gra(window);
    //     GameState state;
    //     if (state.loadFromFile("zapis.txt"))
    //     {
    //         state.apply(gra.getPaddle(), gra.getBall(), gra.getBricks());
    //     }
    //     gra.run(window);
    // }
}