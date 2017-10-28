#include <SFML/Graphics.hpp>

#define WINDOW_SIZE 800
#define ITERATIONS 100
#define MANDELBROTH 3

sf::Uint8 *cells =  new sf::Uint8[WINDOW_SIZE * WINDOW_SIZE];
sf::Uint8 *pixels = new sf::Uint8[WINDOW_SIZE * WINDOW_SIZE * 4];


bool draw = true, follow_mouse = false;
double min = -2, max = 2;
double set_a = MANDELBROTH, set_b = MANDELBROTH;

double to_complex(int x) {
    return (max - min) * double(x) / WINDOW_SIZE + min;
}

int iterations(double x, double y) {
    double x_offset = (set_a == MANDELBROTH ? x : set_a);
    double y_offset = (set_b == MANDELBROTH ? y : set_b);
    double a = x, b = y;
    
    int n = 0;
    while (n < ITERATIONS && a*a + b*b < 4.0) {
        double tmp = 2*a*b + y_offset;
        a = a*a - b*b + x_offset;
        b = tmp;
        n++;
    }

    return n;
}

void calculate_set() {
    for (int y = 0; y < WINDOW_SIZE; ++y) {
        for (int x = 0; x < WINDOW_SIZE; ++x) {
            double a = to_complex(x);
            double b = to_complex(y);
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
    draw = true;
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
        case sf::Keyboard::Up: // fall
        case sf::Keyboard::W:
            scale_viewer(1.0/1.1);
            break;
        case sf::Keyboard::Down: // fall
        case sf::Keyboard::S:
            scale_viewer(1.1);
            break;
        case sf::Keyboard::M:
            follow_mouse = !follow_mouse;
            break;
        case sf::Keyboard::Escape:
            set_a = MANDELBROTH;
            set_b = MANDELBROTH;
            min = -2;
            max =  2;
            break;
        default:
            break;
    }
}

int main() {
    sf::Vector2i mouse_pos;
    sf::Event e;
    sf::Sprite sprite;
    sf::Texture txt;
    txt.create(WINDOW_SIZE, WINDOW_SIZE);
    
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "Julia-set Viewer");
    redraw();
    while(window.isOpen()) {
        while(window.pollEvent(e)) {
            switch (e.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed:
                    handle_key_press(e.key.code);
                    redraw();
                    break;
                default:
                    break;
            }
        }
        
        if (follow_mouse) {
            mouse_pos = sf::Mouse::getPosition(window);
            set_a = to_complex(mouse_pos.x);
            set_b = to_complex(mouse_pos.y);
            redraw();
        }
        
        if (!draw) continue;
        
        txt.update(pixels);
        sprite.setTexture(txt);
        window.draw(sprite);
        
        window.display();
        draw = false;
    }
    
    return 0;
}
