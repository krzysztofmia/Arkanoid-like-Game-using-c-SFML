#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <array> // prosta tablica o stalym rozmiarze - dla klasy Brick
#include <vector>
#include <fstream>
#include <string>

// --------------------------------- klasa BRICK ---------------------------------
//klasa Brick dziedziczy po klasie RectangleShape
class Brick : public sf::RectangleShape {
private:
    int hp; //punkty zycia: 0-3
    bool isDestroyed; //sprawdzenie czy klocek jest zniszczony - jesli tak to true
    static const std::array<sf::Color, 4> colorLUT; //tablica LOOK-UP //static - tablica tworzona raz i dzielona między wszystkimi klockami klasy brick
public:
    Brick(sf::Vector2f startPosition, sf::Vector2f size, int L);
    void changeColor(); //funkcja aktualizuje kolor w zaleznosci od liczby zyc
    void hit(); //funkcja aktualizuje hp
    void draw(sf::RenderTarget &window);
    bool ifDestroyed();
	//--- GETTERY ---
    sf::FloatRect getBounds() const { return getGlobalBounds(); }
    int getHP() const { return hp; }

};

Brick::Brick(sf::Vector2f startPosition, sf::Vector2f size, int L)
{
    hp = L;
    isDestroyed = false;

    setPosition(startPosition);
    setSize(size);
    setOutlineColor(sf::Color::Black); // zapobieganie przed niepoprawnym odbiciem w szczelinie
    setOutlineThickness(2);

    changeColor();
}

const std::array<sf::Color, 4> Brick::colorLUT ={
    sf::Color::Transparent,
    sf::Color::Yellow,
    sf::Color::Magenta,
    sf::Color::Red,
};

bool Brick::ifDestroyed() {
    return isDestroyed;
}

void Brick::hit()
{
    if (isDestroyed == true)
    {
        return;
    }
    hp--;
    changeColor();
    if (hp <= 0)
    {
        isDestroyed = true;
    }  
}

void Brick::changeColor()
{
    if (hp >= 0 && hp <=3)
    {
        this->setFillColor(colorLUT[hp]);
        this->setOutlineColor(sf::Color::Transparent);
    }
}

void Brick::draw(sf::RenderTarget &window)
{
    window.draw(*this);
}

// --------------------------------- klasa BALL ---------------------------------
class Ball
{
private: 
	sf::CircleShape shape;
	sf::Vector2f predkosc{ 200.f, 0.f };

    sf::SoundBuffer bounceBuffer;
    sf::SoundBuffer hitBuffer;
    sf::Sound sound;

public:
	Ball(); //Konstruktor
	void draw(sf::RenderTarget  &window);
	// void ruch(sf::Vector2f predkosc, float WindowWidth, float WindowHeight, float PaddleX, float PaddleY, float PaddleWidth, float PaddleHeight); //--- STARA WERSJA ---
	void ruch(float WindowWidth, float WindowHeight, float PaddleX, float PaddleY, float PaddleWidth, float PaddleHeight, std::vector<Brick>& bloki);
    void setPosition(sf::Vector2f position, sf::Vector2f velocity);
	//--- GETTERY ---
    sf::Vector2f getPosition(){return shape.getPosition();}
	float getX(){return shape.getPosition().x;}
	float getY(){return shape.getPosition().y;}
    sf::Vector2f getV(){return predkosc;}
	float getVX(){return predkosc.x;}
	float getVY(){return predkosc.y;}
	float getR(){return shape.getRadius();}
};

Ball::Ball()
{
    // --- RANDOMIZER PRĘDKOŚCI ---
    std::srand(std::time(nullptr));
    float vx = 5.f + static_cast<float>(rand()) / RAND_MAX * 2.0f;
    float vy = 7.f + static_cast<float>(rand()) / RAND_MAX * 3.0f;
    if (rand() % 2 == 0)
    {
        vx=-vx;
    }
	predkosc = { vx, -vy };

	shape.setPosition({ 400.f, 500.f });
	shape.setRadius(10.f);
	shape.setFillColor(sf::Color::Red);
	shape.setOrigin(sf::Vector2f(10.f, 10.f));

    //zaladuj dzwieki
    if (!bounceBuffer.loadFromFile("sounds/bounce1.wav"))
    {
        std::cout << "Nie znaleziono bounce1.wav!\n";
    }
    if (!hitBuffer.loadFromFile("sounds/bounce2.wav"))
    {
        std::cout << "Nie znaleziono bounce2.wav!\n";
    }
}

void Ball::draw(sf::RenderTarget& window)
{
	window.draw(shape);
}

// void Ball::ruch(sf::Vector2f velo, float WindowWidth, float WindowHeight, float PaddleX, float PaddleY, float PaddleWidth, float PaddleHeight) //--- STARA WERSJA ---
// {
// 	if (shape.getPosition().x - shape.getRadius() > 0 && shape.getPosition().x + shape.getRadius() < WindowWidth && shape.getPosition().y - shape.getRadius() > 0 && shape.getPosition().y + shape.getRadius() < WindowHeight)
// 	{
// 		shape.move(velo);
// 	} else
// 	if (shape.getPosition().x - shape.getRadius() <= 0 || shape.getPosition().x + shape.getRadius() >= WindowWidth)
// 	{
// 		velo.x = -velo.x;
// 		predkosc.x = velo.x;
// 		shape.move(velo);
// 	} else
// 	if (shape.getPosition().y - shape.getRadius()<= 0)
// 	{
// 		velo.y = -velo.y;
// 		predkosc.y = velo.y;
// 		shape.move(velo);
// 	} else
// 	float ballX = shape.getPosition().x;
// 	float ballY = shape.getPosition().y;
// 	float r = shape.getRadius();
// 	if (shape.getPosition().x + shape.getRadius() >= PaddleX - PaddleWidth / 2 &&
// 		shape.getPosition().x - shape.getRadius() <= PaddleX + PaddleWidth / 2 &&
// 		shape.getPosition().y + shape.getRadius() >= PaddleY - PaddleHeight / 2 &&
// 		shape.getPosition().y - shape.getRadius() <= PaddleY + PaddleHeight / 2)
// 	{
// 		// Odbicie piłki w pionie
// 		velo.y = -velo.y;
// 		predkosc.y = velo.y;
// 		shape.move(velo);
// 	} else
// 	if (shape.getPosition().y + shape.getRadius() >= WindowHeight)
// 	{
// 		std::cout << "GAME OVER" << std::endl;
// 	}
// }

void Ball::ruch(float WindowWidth, float WindowHeight, float PaddleX, float PaddleY, float PaddleWidth, float PaddleHeight, std::vector<Brick>& bloki)
{
    float x = getX();
    float y = getY();
    float r = getR();
    float vx = getVX();
    float vy = getVY();


    sf::Vector2f v({vx, vy});

    if (x - r > 0 && x + r < WindowWidth && y - r > 0 && y + r < WindowHeight)
    {
        shape.move(v);
    } else
    if (x - r <= 0 || x + r >= WindowWidth)
    {
        v.x = -v.x;
        predkosc.x = v.x;
        sound.stop();
        sound.setBuffer(bounceBuffer);
        sound.play();
        shape.move(v);
    } else 
    if (y - r <= 0)
    {
        shape.setPosition(x, r); //zabezbieczenie przed zablokowaniem sie pilki na gornej granicy
        v.y = -v.y;
        predkosc.y = v.y;
        sound.stop();
        sound.setBuffer(bounceBuffer);
        sound.play();
        shape.move(v);
    } 
    // else
    // // Kolizja z dolną granicą okna - przegrana
    // if (y + r >= WindowHeight)
    // {
    //     // std::cout << "GAME OVER" << std::endl;
    // }

    // Kolizja z paletką
    if (x + r >= PaddleX - PaddleWidth / 2 && x - r <= PaddleX + PaddleWidth / 2 && y + r >= PaddleY - PaddleHeight / 2 && y - r <= PaddleY + PaddleHeight / 2)
    {
        shape.setPosition(x, PaddleY - PaddleHeight / 2 - r); //nad paletką
        v.y = -std::abs(v.y); //zawsze odbicie w górę
        predkosc.y = v.y;
        sound.stop();
        sound.setBuffer(hitBuffer);
        sound.play();
        shape.move(v);
    }

    // Kolizja z cegiełką
    for (Brick &brick : bloki)
    {
        if (brick.ifDestroyed()) continue;

        sf::FloatRect brickBounds = brick.getBounds();
        sf::FloatRect ballBounds = shape.getGlobalBounds();

        if (ballBounds.intersects(brickBounds))
        {
            float brickLeft   = brickBounds.left;
            float brickRight  = brickBounds.left + brickBounds.width;
            float brickTop    = brickBounds.top;
            float brickBottom = brickBounds.top + brickBounds.height;

            // obliczamy jak głęboko piłka weszła z każdej strony
            float overlapLeft   = (x + r)  - brickLeft;
            float overlapRight  = brickRight - (x - r);
            float overlapTop    = (y + r) - brickTop;
            float overlapBottom = brickBottom - (y - r);

            // wybór osi minimalnego wgłębienia
            bool odbicieX = std::min(overlapLeft, overlapRight) < std::min(overlapTop, overlapBottom);

            if (odbicieX)
            {
                v.x = -v.x;
                predkosc.x = v.x;
            }
            else
            {
                v.y = -v.y;
                predkosc.y = v.y;
            }

            brick.hit();
            sound.stop();
            sound.setBuffer(bounceBuffer);
            sound.play();
            shape.move(v);
            break;
        }
    }

}

void Ball::setPosition(sf::Vector2f position, sf::Vector2f velocity)
{
    shape.setPosition(position);
    predkosc = velocity;
}

// --------------------------------- klasa PADDLE ---------------------------------
class Paddle
{
private:
	sf::RectangleShape shape;
	sf::Vector2f predkosc{ 200.f, 0.f };

public:
	Paddle(); //Konstruktor
	void draw(sf::RenderTarget &window);
    void ruch(float WindowWidth);
	//--- GETTERY ---
	int getWidth(){return shape.getSize().x;}
	int getHeight(){return shape.getSize().y;}
	int getX(){return shape.getPosition().x;}
	int getY(){return shape.getPosition().y;}
    sf::Vector2f getPosition(){return shape.getPosition();}
    void setPosition(sf::Vector2f position);
};

Paddle::Paddle()
{
	predkosc = { 0.f, 0.f };
	shape.setPosition({ 400.f, 550.f });
	shape.setSize({ 120.f, 20.f });
	shape.setFillColor(sf::Color::Blue);
	shape.setOrigin(60.f, 10.f);
}

void Paddle::ruch(float WindowWidth){
    if (((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))) && (getX() > getWidth()/2.f))
    {
			shape.move(sf::Vector2f{-10.f,0.f});
    } else
    if (((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))) && (getX() < WindowWidth-getWidth()/2.f))
    {
			shape.move(sf::Vector2f{10.f,0.f});
    }
}

void Paddle::draw(sf::RenderTarget& window)
{
	window.draw(shape);
}

void Paddle::setPosition(sf::Vector2f position)
{
    shape.setPosition(position);
}

// --------------------------------- klasa GAMESTATE ---------------------------------
class GameState
{
private:
    sf::Vector2f paddlePosition; 
    sf::Vector2f ballPosition; 
    sf::Vector2f ballVelocity; 
    struct BlockData {
        float x, y;
        int hp;
    };
    std::vector<BlockData> blocks; 
public: 
    GameState();
    GameState(Paddle& p, Ball& b, std::vector<Brick>& blk);
    void getPaddle(const Paddle&);
    bool saveToFile(const std::string& filename);
    bool loadFromFile(const std::string& filename);
    void apply(Paddle& p, Ball& b, std::vector<Brick>& blk);
};

// Konstruktor domyslny
GameState::GameState() {}

// Konstruktor przechowujacy stan
GameState::GameState(Paddle& p, Ball& b, std::vector<Brick>& blk) :
    paddlePosition(p.getPosition()),
    ballPosition(b.getPosition()),
    ballVelocity(b.getV())
{
    for (const auto& block : blk)
    {
        BlockData d;
        d.x  = block.getPosition().x;
        d.y  = block.getPosition().y;
        d.hp = block.getHP();
        blocks.push_back(d);
    }
}

bool GameState::saveToFile(const std::string& filename)
{
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    // Zapis Paletki
    file << paddlePosition.x << " " << paddlePosition.y << "\n";

    // Zapis Piłki
    file << ballPosition.x << " " << ballPosition.y << " " 
        << ballVelocity.x << " " << ballVelocity.y << "\n";

    // Zapis liczby bloków
    int alive = 0;
    for (auto& block : blocks)
        if (block.hp > 0) alive++;

    file << alive << "\n";
    // Zapis bloków
    for (const auto& block : blocks) 
    {
        if (block.hp > 0)   // zapisuj TYLKO żywe
                file << block.x << " " << block.y << " " << block.hp << "\n";
    }

    file.close();
    return true;
}

bool GameState::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    
    // 1. Wczytaj Paletkę
    if (file >> paddlePosition.x >> paddlePosition.y) {
        // Dane wczytane do zmiennych tymczasowych w GameState
    }

    // 2. Wczytaj Piłkę
    if (file >> ballPosition.x >> ballPosition.y >> ballVelocity.x >> ballVelocity.y) {
        // ...
    }

    // 3. Wczytaj Bloki
    int blocksCount;
    file >> blocksCount;
    
    blocks.clear(); // Czyścimy stare dane
    for (int i = 0; i < blocksCount; ++i) {
        float x, y;
        int hp;
        file >> x >> y >> hp;
        blocks.push_back({x, y, hp});
    }

    file.close();
    return true;
}

void GameState::apply(Paddle& p, Ball& b, std::vector<Brick>& blk)
{
    // 1. Przywróć paletkę
    p.setPosition(paddlePosition);

    // 2. Przywróć piłkę
    b.setPosition(ballPosition, ballVelocity);

    // 3. Przywróć bloki
    blk.clear();
    sf::Vector2f size((800 - (12 - 1)*2.f)/12, 20.f);

    for (const auto& data : blocks)
    {
        blk.emplace_back(
            sf::Vector2f(data.x, data.y),
            size,
            data.hp
        );
    }
}

// --------------------------------- klasa GAME ---------------------------------
class Game
{
private: 
	sf::RenderWindow window;
    const int WindowWidth = 800;
    const int WindowHeight = 600;
    sf::Clock zegar;
    sf::Event event;
    Paddle m_paletka;
    Ball m_pilka;
    std::vector<Brick> bloki;
    //tekst zapisywania
    sf::Font font;
    sf::Text saveMessage;
    sf::Text lostMessage;
    sf::Text wonMessage;
    bool showSaveMessage = false;
    bool showWonMessage = false;
    bool showLostMessage = false;
    float ls; //letter spacing dla zapisywania
    sf::Clock saveClock;
    bool gameRunning = true;

    void initBricks();
    void processEvents();
    void gameOver();
    void render();
public:
	Game();
	void run();
    //---GETTERY---
    Paddle& getPaddle() { return m_paletka; }
    Ball& getBall() { return m_pilka; }
    std::vector<Brick>& getBricks() { return bloki; }
};

Game::Game(): window(sf::VideoMode(800, 600), "Arkanoid v3.00"),
      m_paletka(),
      m_pilka()
{
    window.setFramerateLimit(60);
    initBricks();     //inicjalizacja bloków

    font.loadFromFile("arial.ttf");

    //Wiadomosc zapisywania
    saveMessage.setFont(font);
    saveMessage.setLetterSpacing(2);
    saveMessage.setString("Zapisano...");
    saveMessage.setCharacterSize(24);
    saveMessage.setFillColor(sf::Color::White);
    saveMessage.setPosition(20, 20);

    //Wiadomosc przegranej
    lostMessage.setFont(font);
    lostMessage.setLetterSpacing(2);
    lostMessage.setString("GAME OVER! YOU LOST\n   Press ENTER to exit");
    lostMessage.setCharacterSize(24);
    lostMessage.setFillColor(sf::Color::Red);
    lostMessage.setPosition(250, 250);

    //Wiadomosc wygranej
    wonMessage.setFont(font);
    wonMessage.setLetterSpacing(2);
    wonMessage.setString("CONGRATS! YOU WON\n  Press ENTER to exit");
    wonMessage.setCharacterSize(24);
    wonMessage.setFillColor(sf::Color::Green);
    wonMessage.setPosition(250, 250);

}

void Game::initBricks()
{
    const int WindowWidth = 800;
    const int WindowHeight = 600;
    //poziom trudnosci
    const int ILOSC_KOLUMN = 12;
    const int ILOSC_WIERSZY = 7;
    float ROZMIAR_BLOKU_X = (WindowWidth - (ILOSC_KOLUMN - 1)*2.f)/ILOSC_KOLUMN; // uzaleznic od szerokosci ekranu
    float ROZMIAR_BLOKU_Y = 20.f;
    //--procedura rysujaca bloki --
    for (int y = 0; y < ILOSC_WIERSZY; y++)
    {
        for (int x = 0; x < ILOSC_KOLUMN; x++)
        {
            float posX = x * (ROZMIAR_BLOKU_X + 2.f);//2px przerwy
            float posY = y * (ROZMIAR_BLOKU_Y + 2.f) + 60.f;
            //algorytm ustalania koloru wzgledem zycia L
            int zycie = 0;
            if (y < 3)
            {
                zycie = 3;
            }
            if (y>=3)
            {
                zycie = 2;
            }
            bloki.emplace_back(
                sf::Vector2f(posX, posY),
                sf::Vector2f(ROZMIAR_BLOKU_X, ROZMIAR_BLOKU_Y),
                zycie
            );        
        }
    }
}

void Game::render()
{
    //--- Rysowanie ---
    window.clear(sf::Color::Black); // tło
    m_pilka.draw(window);
    m_paletka.draw(window);
    if (showSaveMessage)
    {
        window.draw(saveMessage);
        saveMessage.setLetterSpacing(ls);
        ls = ls + 0.01;
    }
    if (showLostMessage)
    {
        window.draw(lostMessage);
    }
    if (showWonMessage)
    {
        window.draw(wonMessage);
    }
    //--- Rysowanie blokow ---
    for (auto& blk : bloki)
    {
        blk.draw(window);
    }

    window.display();
}

void Game::processEvents()
{
    if (!gameRunning)
        return;
    //Ruch paletki
    if (event.type == sf::Event::KeyPressed)
        m_paletka.ruch(WindowWidth);
    //Ruch pilki
    m_pilka.ruch(WindowWidth, WindowHeight, m_paletka.getX(), m_paletka.getY(), m_paletka.getWidth(), m_paletka.getHeight(), bloki);

    // if (event.key.code == sf::Keyboard::P)
    // {
    //     GameState state(m_paletka, m_pilka, bloki);
    //     if (state.saveToFile("zapis.txt"))
    //     {
    //         std::cout << "Gra zapisana!\n";
    //     }
    // }
    if (event.key.code == sf::Keyboard::P)
    {
        GameState state(m_paletka, m_pilka, bloki);
        if (state.saveToFile("zapis.txt"))
        {
            showSaveMessage = true;
            saveClock.restart();   // licz czas komunikatu
        }
    }
    if (showSaveMessage && saveClock.getElapsedTime().asSeconds() > 2)
    {
        showSaveMessage = false;
        saveClock.restart();
        ls = 2;
    }
}

void Game::gameOver()
{
    //sprawdzanie czy wszystkie bloki zostały zniszczone
    bool allDestroyed = true;
    for (auto& blk : bloki)
    {
        if (!blk.ifDestroyed())
        {
            allDestroyed = false;
            break;
        }
    }

    if (allDestroyed)
    {
        // std::cout << "YOU WIN!" << std::endl;
        // window.close();
        showWonMessage = true;
        gameRunning = false;
    }

    if (m_pilka.getY() + m_pilka.getR() > WindowHeight)
    {
        // std::cout << "MISS! GAME OVER" << std::endl;
        // window.close();
        showLostMessage = true;
        gameRunning = false;
    }
}

void Game::run()
{
    while (window.isOpen())
    {
        if (zegar.getElapsedTime().asMilliseconds() > 16.66666f)
        {
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();
                if (!gameRunning)
                {
                    if (event.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter))
                        window.close();
                } 
            }
            render();
            processEvents();
            gameOver();
            zegar.restart();
        }
    }
}

// --------------------------------- klasa MENU ---------------------------------
#define MAX_LICZBA_POZIOMOW 3

class Menu
{

private:
	sf::Font font;
	sf::Text menu[MAX_LICZBA_POZIOMOW];//maksymalna liczba poziomow
	int selectedItem = 0;
	sf::RenderWindow &window;
public:
	Menu(sf::RenderWindow &window);
	~Menu() {};
	void przesunG();//przesun do gory
	void przesunD();//przesun w dol
	int getSelectedItem() { return selectedItem; }//zwroc poziom menu
	void draw(sf::RenderWindow &window);//rysuj menu w oknie
    int run();
};

Menu::Menu(sf::RenderWindow &win) : window(win)
{
    //laduj czcionke
	if (!font.loadFromFile("arial.ttf"))
	{
		return;
	}
	//rysowanie elementow menu
	menu[0].setFont(font);	
	menu[0].setFillColor(sf::Color::Cyan);
	menu[0].setString("Nowa gra");
	menu[0].setPosition(sf::Vector2f(window.getSize().x / 3, window.getSize().y / (MAX_LICZBA_POZIOMOW + 1) * 1));
	menu[1].setFont(font);
	menu[1].setFillColor(sf::Color::White);
	menu[1].setString("Wczytaj zapis");
	menu[1].setPosition(sf::Vector2f(window.getSize().x / 3, window.getSize().y / (MAX_LICZBA_POZIOMOW + 1) * 2));
	menu[2].setFont(font);
	menu[2].setFillColor(sf::Color::White);
	menu[2].setString("Wyjscie");
	menu[2].setPosition(sf::Vector2f(window.getSize().x / 3, window.getSize().y / (MAX_LICZBA_POZIOMOW + 1) * 3));
}

void Menu::draw(sf::RenderWindow &window)
{
	for (int i = 0; i < MAX_LICZBA_POZIOMOW; i++)
	{
		window.draw(menu[i]);
	}
}

void Menu::przesunG()
{
	if (selectedItem >= 0 && selectedItem < MAX_LICZBA_POZIOMOW)
	{
		menu[selectedItem].setFillColor(sf::Color::White);
		menu[selectedItem].setStyle(sf::Text::Regular);
		selectedItem--;
		if (selectedItem < 0)
			selectedItem = MAX_LICZBA_POZIOMOW - 1;
		menu[selectedItem].setFillColor(sf::Color::Cyan);
		menu[selectedItem].setStyle(sf::Text::Bold);
	}
	
	
}

void Menu::przesunD()
{
	if (selectedItem >= 0 && selectedItem < MAX_LICZBA_POZIOMOW)
	{
		menu[selectedItem].setFillColor(sf::Color::White);
		menu[selectedItem].setStyle(sf::Text::Regular);
		selectedItem++;
		if (selectedItem >= MAX_LICZBA_POZIOMOW)
			selectedItem = 0;
		menu[selectedItem].setFillColor(sf::Color::Cyan);
		menu[selectedItem].setStyle(sf::Text::Bold);
	}
	
}

void myDelay(int opoznienie)
{
	sf::Clock zegar;
	sf::Time czas;
	while (1)
	{
		czas = zegar.getElapsedTime();
		if (czas.asMilliseconds() > opoznienie)
		{
			zegar.restart();
			break;
		}
	}	
}

int Menu::run()
{
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Up)
                    przesunG();
                if (event.key.code == sf::Keyboard::Down)
                    przesunD();

                if (event.key.code == sf::Keyboard::Enter)
                {
                    int opcja = getSelectedItem();

                    if (opcja == 0)      // Nowa gra
                    {
                        window.close();  // zamykamy menu
                        
                        Game gra;        // tworzymy obiekt gry
                        gra.run();       // odpalamy grę

                        return 0;
                    }
                    else if (opcja == 1) // Wczytaj grę
                    {
                        return 1;   // zwracamy kod opcji
                    }
                    else if (opcja == 2) // Wyjście
                    {
                        window.close();
                        return 0;
                    }
                }
            }
        }

        window.clear(sf::Color::Black);
        draw(window);
        window.display();
    }
    return 0;
}
