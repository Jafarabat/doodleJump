
#include <SFML/Graphics.hpp> 
#include <SFML/Audio.hpp> 
#include <time.h> 
#include <iostream> 
#include <fstream> 
#include <cmath>

 
using namespace sf; 
using namespace std; 
 
struct point 
{ 
    int x, y; 
}; 
 
int main() 
{ 
    srand(time(0)); 
 
    RenderWindow app(VideoMode(400, 533), "Doodle Jump"); 
    Font font; 
    if (!font.loadFromFile("arial.ttf"))  
    { 
        cerr << "Failed to load font"; 
    } 
 
    app.setFramerateLimit(60); 
 
    Texture t1, t2, t3; 
    t1.loadFromFile("images/background.png"); 
    t2.loadFromFile("images/platform.png"); 
    t3.loadFromFile("images/doodle.png"); 

    sf::Vector2u textureSize={80,80};
 
    Sprite sBackground(t1), sPlat(t2), sPers(t3); 
 
    point plat[20]; 
 
    for (int i = 0; i < 10; i++) 
    { 
        plat[i].x = rand() % 400; 
        plat[i].y = rand() % 533; 
    } 
 
    int x = 100, y = 100, h = 200, score = 0, lastPlatform = -1, bestScore = 0; 
    float dx = 0, dy = 0, rotation=0, rotationSpeed=5; 
    bool canJump = true; 
    bool isMovingRight = false; 
    bool isPaused = false; 
    bool isMusicPlaying= false;
    bool endGame=false;
    bool collision=true;
    float rotationRadians;
    
    Clock timer;

 
    ifstream inputFile("best_score.txt"); 
    if (inputFile.is_open()) 
    { 
        inputFile >> bestScore; 
        inputFile.close(); 
    } 
 
    Text scoreT, bestScoreT; 
    scoreT.setFont(font); 
    scoreT.setCharacterSize(24); 
    scoreT.setFillColor(sf::Color::Black); 
    scoreT.setPosition(10, 10); 
 
    bestScoreT.setFont(font); 
    bestScoreT.setCharacterSize(24); 
    bestScoreT.setFillColor(sf::Color::Black); 
    bestScoreT.setPosition(10, 40); 
 

    SoundBuffer jumpBuffer; 
    if (!jumpBuffer.loadFromFile("sounds/jump.ogg")) 
    { 
        cerr << "Failed to load sound jump"; 
    } 
    Sound jump(jumpBuffer); 


    SoundBuffer failBuffer;
    if (!failBuffer.loadFromFile("sounds/fail.ogg"))
    {
        cerr<<"Failed to load sound fail";
    }
    Sound fail(failBuffer);
 
    Music ambient; 
    ambient.openFromFile("sounds/ambient1.ogg"); 
    ambient.play(); 
    ambient.setLoop(true); 
    ambient.setVolume(30.f); 
 
    while (app.isOpen()) 
    { 
 
        Event e; 
        while (app.pollEvent(e)) 
        { 
            if (e.type == Event::Closed) 
                app.close(); 
            if (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape){ 
                isPaused = !isPaused;
                if (isPaused) {
                    isMusicPlaying = ambient.getStatus() == Music::Playing; 
                    if (isMusicPlaying) 
                        ambient.pause(); 
                } 

                else{
                    if (isMusicPlaying) 
                        ambient.play(); 
                }
            } 
                                              
        } 
 
        
        if (!isPaused)
        {      
            if (collision){          
            if (Keyboard::isKeyPressed(Keyboard::D)) 
            { 
                x += 3; 
                isMovingRight = true; 
            } 
            if (Keyboard::isKeyPressed(Keyboard::A)) 
            { 
                x -= 3; 
                isMovingRight = false; 
            } 
            }

            if (isMovingRight){
                sPers.setTextureRect(sf::IntRect(textureSize.x, 0, -textureSize.x, textureSize.y));
            }
            if (!isMovingRight){
                sPers.setTextureRect(sf::IntRect(0,0, textureSize.x, textureSize.y));
                
            }
 
            if (x > 400) 
                x = 0; 
            if (x < -70) 
                x = 390; 
            if (y > 530 && !endGame){
                
                endGame=true;  
                fail.play();
                collision=false;
            }
            if (endGame) { 
                rotationSpeed *= .99f; 
                sPers.rotate(rotationSpeed);
                jump.stop();
                ambient.stop();

                // if (timer.getElapsedTime().asSeconds() >= 5) { 
                //     app.close();
                // } GetTicks()
            }
            dy += 0.2; 
            y += dy;
            if (y>530){
                dy-=5;
            }
             
 
            if (y < h)
                for (int i = 0; i < 10; i++)
                {
                    y = h;
                    plat[i].y = plat[i].y - dy;
                    if (plat[i].y > 533)
                    { 
                        plat[i].y = 0;
                        plat[i].x = rand() % 400; 
                    } 
                } 
 
            canJump = false; 
            if(collision){
            for (int i = 0; i < 10; i++) 
            { 
                if ((x + 50 > plat[i].x) && (x + 20 < plat[i].x + 68) && (y + 70 > plat[i].y) && (y + 70 < plat[i].y + 14) && (dy > 0)) 
                { 
                    jump.play(); 
                    dy = -10; 
                    if (i != lastPlatform) 
                    { 
                        canJump = true; 
 
                        lastPlatform = i; 
                    } 
                } 
            } 
            }
 
            if (canJump) 
            { 
                score++; 
                if (score > bestScore) 
                { 
                    bestScore = score; 
                } 
            } 
            sPers.setPosition(x, y);
            
            
            
 
            app.draw(sBackground); 
            app.draw(sPers);
            for (int i = 0; i < 10; i++) 
            { 
                sPlat.setPosition(plat[i].x, plat[i].y); 
                app.draw(sPlat); 
            } 
            scoreT.setString("Score: " + std::to_string(score)); 
            bestScoreT.setString("Best score: " + std::to_string(bestScore)); 
            app.draw(scoreT); 
            app.draw(bestScoreT); 
            app.display(); 
        }
        
    } 
    ofstream outputFile("best_score.txt"); 
    if (outputFile.is_open()) 
    { 
        outputFile << bestScore; 
        outputFile.close(); 
    } 
}