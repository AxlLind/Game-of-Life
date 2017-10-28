#include <SFML/Graphics.hpp>

#define WINDOW_SIZE 800
#define ITERATIONS 25

sf::Uint8 *cells =  new sf::Uint8[WINDOW_SIZE * WINDOW_SIZE];
sf::Uint8 *pixels = new sf::Uint8[WINDOW_SIZE * WINDOW_SIZE * 4];

double min = -2, max = 2;

int iterations(double x, double y) {
    int n = 0;
    double a = 0, b = 0;
    
    while (n < ITERATIONS && a*a + b*b < 4.0) {
        double tmp = 2*a*b + y;
        a = a*a - b*b + x;
        b = tmp;
        n++;
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
        int c = cells[i] == ITERATIONS ? 0 : cells[i];
        pixels[index++] = step * c;
        pixels[index++] = step * c;
        pixels[index++] = step * c;
        pixels[index++] = 255;
    }
}

void redraw() {
    calculate_set();
    calculate_pixels();
}

void scale_viewer(double scl) {
    min *= scl;
    max *= scl;
    if (min < -2)   min = -2;
    if (max >  2)   max =  2;
    if (min > -0.5) min = -0.5;
    if (max <  0.5) max =  0.5;
}

void handle_key_press(sf::Keyboard::Key code) {
    switch (code) {
        case sf::Keyboard::W:
            scale_viewer(1.0/1.1);
            break;
        case sf::Keyboard::S:
            scale_viewer(1.1);
            break;
        default:
            break;
    }
    redraw();
}

int main() {
    sf::Event e;
    sf::Sprite sprite;
    sf::Texture txt;
    txt.create(WINDOW_SIZE, WINDOW_SIZE);
    
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "Julia Set");
    redraw();
    while(window.isOpen()) {
        while(window.pollEvent(e)) {
            switch (e.type) {
                case sf::Event::Closed: window.close();
                case sf::Event::KeyPressed: handle_key_press(e.key.code);
                default:
                    break;
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
