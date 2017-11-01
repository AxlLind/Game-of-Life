#include <SFML/Graphics.hpp>
#include <thread>

#define WINDOW_SIZE 800 // in pixels
#define ITERATIONS  200
#define MANDELBROT  100 // arbitrary value outside our viewing-axies

sf::Uint8 *cells =  new sf::Uint8[WINDOW_SIZE * WINDOW_SIZE];
sf::Uint8 *pixels = new sf::Uint8[WINDOW_SIZE * WINDOW_SIZE * 4];

bool   draw = true, color_mode = true, follow_mouse = false;
double julia_constant_a = MANDELBROT, julia_constant_b = MANDELBROT;
double axies_size = 2.0;

/* Point (x,y) to index for that point in a 1d-array */
inline int point_to_index(int x, int y) {
    return y * WINDOW_SIZE + x;
}

/* Maps a pixel number [0..WINDOW_SIZE] to [-axies_size..axies_size] */
inline double pixel_to_coord(int x) {
    return axies_size * (2 * double(x) / WINDOW_SIZE - 1);
}

/* Returns the number of iterations until the point (x,y) will diverge */
int iterations(double x, double y) {
    double x_offset = (julia_constant_a == MANDELBROT ? x : julia_constant_a);
    double y_offset = (julia_constant_b == MANDELBROT ? y : julia_constant_b);
    double a = x, b = y;
    double a_sqr = a*a, b_sqr = b*b;
    
    int n = 0;
    while (n < ITERATIONS && a_sqr + b_sqr < 4.0) {
        b *= a;
        b += b + y_offset; // *2 + y_offset
        a = a_sqr - b_sqr + x_offset;
        a_sqr = a*a; b_sqr = b*b;
        n++;
    }
    return n;
}

/* Calculate the number of iterations for parts of the image */
void calculate_partial(int y_start, int y_end) {
    int index = point_to_index(0, y_start);
    for (int y = y_start; y < y_end; ++y) {
        for (int x = 0; x < WINDOW_SIZE; ++x) {
            double val = iterations( pixel_to_coord(x) , pixel_to_coord(y) );
            int symmetric_x = julia_constant_a == MANDELBROT ? x : WINDOW_SIZE-x;
            cells[index++] = val;
            cells[ point_to_index(symmetric_x, WINDOW_SIZE-y-1) ] = val;
            // julia-set symmetric, mirrored and mandelbrot is symmetric along x=0
        }
    }
}

/* Calculates the number of iterations for every pixel using multithreading */
void calculate_set(int num_threads) {
    std::thread threads[num_threads];
    int start = 0, step = (WINDOW_SIZE / 2) / num_threads;
    for (int i = 0; i < num_threads-1; ++i) {
        threads[i] = std::thread(calculate_partial, start, start + step);
        start += step;
    }
    threads[num_threads-1] = std::thread(calculate_partial, start, WINDOW_SIZE/2);
    for (int i = 0; i < num_threads; ++i)
        threads[i].join(); // wait for every thread
}

/* If in color-mode we map a value [0..ITERATIONS] to an RGB-color
   in the range blue to red, otherwise the grayscale value */
void calc_rgb(int *color, int val) {
    if (!color_mode) {
        color[0] = val * 255 / ITERATIONS;
        color[1] = color[0]; color[2] = color[0];
        return;
    }
    
    double r = 1.0, g = 1.0, b = 1.0, iter = double(val);
    if (iter < 0.2 * ITERATIONS) {
        r = 0;
        g = 4.0 * iter / ITERATIONS;
    } else if (iter < 0.4 * ITERATIONS) {
        r = 0;
        b = 1.0 + 4 * (0.25 * ITERATIONS - iter) / ITERATIONS;
    } else if (iter < 0.5 * ITERATIONS) {
        r = 4 * (iter - 0.5 * ITERATIONS) / ITERATIONS;
        b = 0;
    } else {
        g = 1.0 + 4.0 * (0.6 * ITERATIONS - iter) / ITERATIONS;
        b = 0;
    }
    
    color[0] = int(255*r); color[1] = int(255*g); color[2] = int(255*b);
}

/* calculate the color of every pixel based on the number of iterations of that point */
void calculate_pixels() {
    int index = 0;
    for (int i = 0; i < WINDOW_SIZE * WINDOW_SIZE; ++i) {
        int color[] = {0,0,0};
        if (cells[i] != ITERATIONS && cells[i] != 0) calc_rgb(color, cells[i]);
        pixels[index++] = color[0];
        pixels[index++] = color[1];
        pixels[index++] = color[2];
        pixels[index++] = 255;
    }
}

/* Scales the coordinate-axies by a specified factor */
void scale_axies(double scl) {
    axies_size *= scl;
    if (axies_size > 2.0) axies_size = 2.0;
    if (axies_size < 0.5) axies_size = 0.5;
}

/* Handels the various key-presses */
void handle_key_press(sf::Keyboard::Key code) {
    switch (code) {
        case sf::Keyboard::W: // fall
        case sf::Keyboard::Up:
            scale_axies(1.0/1.1);
            break;
        case sf::Keyboard::S: // fall
        case sf::Keyboard::Down:
            scale_axies(1.1);
            break;
        case sf::Keyboard::M:
            follow_mouse = !follow_mouse;
            break;
        case sf::Keyboard::C:
            color_mode = !color_mode;
            break;
        case sf::Keyboard::Escape:
            // reset to initial state
            julia_constant_a = MANDELBROT;
            julia_constant_b = MANDELBROT;
            axies_size =  2.0;
            color_mode = true;
            follow_mouse = false;
            break;
        default: break;
    }
    draw = true;
}

int main() {
    sf::Texture txt; sf::Event e; sf::Sprite s;
    txt.create(WINDOW_SIZE, WINDOW_SIZE);
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "Julia-set Viewer");
    unsigned int num_threads = std::thread::hardware_concurrency();
    while(window.isOpen()) {
        while(window.pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                window.close();
            if (e.type == sf::Event::KeyPressed)
                handle_key_press(e.key.code);
            if (e.type == sf::Event::MouseMoved && follow_mouse) {
                sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
                julia_constant_a = pixel_to_coord(mouse_pos.x);
                julia_constant_b = pixel_to_coord(mouse_pos.y);
                draw = true;
            }
        }
        if (!draw) continue;
        
        calculate_set(num_threads);
        calculate_pixels();
        txt.update(pixels);
        s.setTexture(txt);
        window.draw(s);
        window.display();
        draw = false;
    }
    return 0;
}
