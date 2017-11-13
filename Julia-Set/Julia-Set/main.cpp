#include <SFML/Graphics.hpp>
#include <OpenCL/opencl.h>
#include "kernel.cl.h"
#include <iostream>

#define WINDOW_SIZE 800 // in pixels
#define ITERATIONS  250
#define MANDELBROT  100 // arbitrary value outside our viewing-axies

sf::Uint8  cells[WINDOW_SIZE * WINDOW_SIZE];
sf::Uint8 pixels[WINDOW_SIZE * WINDOW_SIZE * 4];

bool   draw = true, color_mode = true, follow_mouse = false;
double julia_constant_a = MANDELBROT, julia_constant_b = MANDELBROT;
double axies_size = 2.0;

/* Maps a pixel number [0..WINDOW_SIZE] to [-axies_size..axies_size] */
double pixel_to_coord(int x) {
    return axies_size * (2 * double(x) / WINDOW_SIZE - 1);
}

void open_cl_iterations() {
    dispatch_queue_t queue = gcl_create_dispatch_queue(CL_DEVICE_TYPE_CPU, NULL);
    void *cells_out = gcl_malloc(sizeof(cl_char) * WINDOW_SIZE * WINDOW_SIZE, NULL, CL_MEM_WRITE_ONLY);
    // allocate gcl array for our iteration calculation
    
    dispatch_sync(queue, ^{
        size_t wgs;
        gcl_get_kernel_block_workgroup_info(iteration_kernel, CL_KERNEL_WORK_GROUP_SIZE,
                                            sizeof(wgs), &wgs, NULL);
        cl_ndrange range = {
            1,
            {0,0,0},
            {WINDOW_SIZE * WINDOW_SIZE, 0,0},
            {wgs,0,0}
        };
        
        iteration_kernel(&range, (cl_char*) cells_out, axies_size,
                         julia_constant_a, julia_constant_b);
        // preform the iteration calulation for every pixel
        gcl_memcpy(cells, cells_out, sizeof(cl_char) * WINDOW_SIZE * WINDOW_SIZE);
        // copy the gcl array to our main memory
    });
    
    gcl_free(cells_out);
    dispatch_release(queue);
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
void set_pixels() {
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
        
        open_cl_iterations();
        set_pixels();
        txt.update(pixels);
        s.setTexture(txt);
        window.draw(s);
        window.display();
        draw = false;
    }
    return 0;
}
