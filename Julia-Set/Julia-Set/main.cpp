#include <SFML/Graphics.hpp>

#define WINDOW_SIZE 800
#define ITERATIONS 255
#define MANDELBROTH 5

sf::Uint8 *cells =  new sf::Uint8[WINDOW_SIZE * WINDOW_SIZE];
sf::Uint8 *pixels = new sf::Uint8[WINDOW_SIZE * WINDOW_SIZE * 4];


bool draw = true, follow_mouse = false, color_mode = false;
double max = 2.0, min = -max;
double set_a = MANDELBROTH, set_b = MANDELBROTH;

double pixel_to_coord(int x) {
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
            double a = pixel_to_coord(x);
            double b = pixel_to_coord(y);
            cells[y*WINDOW_SIZE + x] = iterations(a, b);
        }
    }
}

void calc_rgb(int *color, int val) {
    if (!color_mode) {
        color[0] = val * 255 / ITERATIONS;
        color[1] = color[0]; color[2] = color[0];
        return;
    }
    
    double r = 1.0, g = 1.0, b = 1.0, iter = double(val);
    if (iter < (0.2 * ITERATIONS)) {
        r = 0;
        g = 4.0 * iter / ITERATIONS;
    } else if (iter < (0.4 * ITERATIONS)) {
        r = 0;
        b = 1.0 + 4 * (0.25 * ITERATIONS - iter) / ITERATIONS;
    } else if (iter < (0.5 * ITERATIONS)) {
        r = 4 * (iter - 0.5 * ITERATIONS) / ITERATIONS;
        b = 0;
    } else {
        g = 1.0 + 4.0 * (0.6 * ITERATIONS - iter) / ITERATIONS;
        b = 0;
    }
    
    color[0] = int(255*r); color[1] = int(255*g); color[2] = int(255*b);
}

void calculate_pixels() {
    for (int i = 0; i < WINDOW_SIZE * WINDOW_SIZE; ++i) {
        int color[] = {0,0,0};
        if (cells[i] != ITERATIONS && cells[i] != 0) calc_rgb(color, cells[i]);
        
        int index = 4 * i;
        pixels[index++] = color[0];
        pixels[index++] = color[1];
        pixels[index++] = color[2];
        pixels[index] = 255;
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
        case sf::Keyboard::C:
            color_mode = !color_mode;
            break;
        case sf::Keyboard::Escape:
            set_a = MANDELBROTH;
            set_b = MANDELBROTH;
            max =  2.0;
            min = -max;
            color_mode = false;
            follow_mouse = false;
            break;
        default: break;
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
                default: break;
            }
        }
        if (follow_mouse) {
            mouse_pos = sf::Mouse::getPosition(window);
            set_a = pixel_to_coord(mouse_pos.x);
            set_b = pixel_to_coord(mouse_pos.y);
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
