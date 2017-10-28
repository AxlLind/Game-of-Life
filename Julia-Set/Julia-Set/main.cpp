#include <SFML/Graphics.hpp>

#define WINDOW_SIZE 800
#define ITERATIONS 10

sf::Uint8 *cells =  new sf::Uint8[WINDOW_SIZE * WINDOW_SIZE];
sf::Uint8 *pixels = new sf::Uint8[WINDOW_SIZE * WINDOW_SIZE * 4];

double min = -2, max = 2;

int iterations(double x, double y) {
    int n = 0;
    double a = 0, b = 0;
    
    while (n < ITERATIONS+1) {
        n++;
        a = a*a - b*b + x;
        b = 2*a*b + y;
        if (a*a + b*b > 4.0) break;
    }

    return n;
}

void calculate_set() {
    for (int y = 0; y < WINDOW_SIZE; ++y) {
        for (int x = 0; x < WINDOW_SIZE; ++x) {
            double a = (max - min) * double(x) / WINDOW_SIZE + min;
            double b = (max - min) * double(y) / WINDOW_SIZE + min;
            cells[y*WINDOW_SIZE + x] = iterations(a, b);
        }
    }
}

void calculate_pixels() {
    int step = 255 / ITERATIONS;
    for (int i = 0; i < WINDOW_SIZE * WINDOW_SIZE; ++i) {
        int index = 4 * i;
        pixels[index++] = step * cells[i];
        pixels[index++] = step * cells[i];
        pixels[index++] = step * cells[i];
        pixels[index++] = 255;
    }
}

void calculate() {
    calculate_set();
    calculate_pixels();
}

int main() {
    sf::Event e;
    sf::Sprite sprite;
    sf::Texture txt;
    txt.create(WINDOW_SIZE, WINDOW_SIZE);
    
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "Julia Set");
    calculate();
    while(window.isOpen()) {
        while(window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) window.close();
            if (e.type == sf::Event::KeyPressed) {
                double scale = 1;
                if (e.key.code == sf::Keyboard::Up) scale = 1.1;
                if (e.key.code == sf::Keyboard::Down) scale = 1.0/1.1;
                min *= scale;
                max *= scale;
                if (min < -2) min = -2;
                if (max > 2)  max = 2;
                calculate();
            }
        }
        sf::sleep(sf::milliseconds(30));
        
        txt.update(pixels);
        sprite.setTexture(txt);
        window.draw(sprite);
        
        window.display();
    }
    
    return 0;
}
