#include <SFML/Graphics.hpp>

#define WIDTH  200
#define HEIGHT 200

#define DEAD   75
#define ALIVE  0
#define CELL_SIZE 4

sf::Uint8 *cells      = new sf::Uint8[WIDTH * HEIGHT];
sf::Uint8 *cells_prev = new sf::Uint8[WIDTH * HEIGHT];
sf::Uint8 *pixels     = new sf::Uint8[4 * CELL_SIZE * CELL_SIZE * WIDTH * HEIGHT];

int get_index(int x, int y) {
    return y * WIDTH + x;
}

int adjacent(int x, int y) {
    int num_adjacent = 0;
    
    if ( cells_prev[ get_index(x-1, y-1) ] == ALIVE ) ++num_adjacent;
    if ( cells_prev[ get_index(x  , y-1) ] == ALIVE ) ++num_adjacent; // row above
    if ( cells_prev[ get_index(x+1, y-1) ] == ALIVE ) ++num_adjacent;
    
    if ( cells_prev[ get_index(x-1, y)   ] == ALIVE ) ++num_adjacent; // our row
    if ( cells_prev[ get_index(x+1, y)   ] == ALIVE ) ++num_adjacent;
    
    if ( cells_prev[ get_index(x-1, y+1) ] == ALIVE ) ++num_adjacent;
    if ( cells_prev[ get_index(x  , y+1) ] == ALIVE ) ++num_adjacent; // row below
    if ( cells_prev[ get_index(x+1, y+1) ] == ALIVE ) ++num_adjacent;
    return num_adjacent;
}

void update_cell(int x, int y) {
    int num_adjacent = adjacent(x,y);
    
    if (cells[get_index(x,y)] == DEAD) {
        if (num_adjacent == 3) {
            cells[ get_index(x,y) ] = ALIVE;
        }
    } else if (num_adjacent < 2 || num_adjacent > 3) {
        cells[ get_index(x,y) ] = DEAD;
    }
    
}

void update_cells() {
    for (int x = 1; x < WIDTH-1; ++x) {
        for (int y = 1; y < HEIGHT-1; ++y) {
            update_cell(x,y);
        }
    }
}

void draw_pixels(int x, int y) {
    sf::Uint8 dead_alive = cells[get_index(x, y)];
    
    for (int i = 0; i < CELL_SIZE; ++i) {
        for (int j = 0; j < CELL_SIZE; ++j) {
            int index = CELL_SIZE * (WIDTH * (CELL_SIZE * y + j) + x) + i;
            pixels[4*index]   = dead_alive;
            pixels[4*index+1] = dead_alive;
            pixels[4*index+2] = dead_alive;
            pixels[4*index+3] = 255;
        }
    }
}

void update_pixels() {
    for (int x = 0; x < WIDTH; ++x) {
        for (int y = 0; y < HEIGHT; y++) {
            draw_pixels(x,y);
        }
    }
}

void update() {
    std::copy(cells, cells + WIDTH * HEIGHT, cells_prev);
    update_cells();
    update_pixels();
}

int main() {
    srand(time(NULL));
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            cells[ get_index(x,y) ] = rand() % 2 == 0 ? ALIVE : DEAD;
        }
    }
    sf::Texture txt;
    txt.create(CELL_SIZE * WIDTH, CELL_SIZE * HEIGHT);
    
    sf::Sprite sprite;
    
    sf::RenderWindow window(sf::VideoMode(CELL_SIZE * WIDTH, CELL_SIZE * HEIGHT), "Game of Life");
    bool paused = false;
    while(window.isOpen()) {
        sf::Event e;
        while(window.pollEvent(e)) {
            switch (e.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::LostFocus:
                    paused = true;
                    break;
                case sf::Event::GainedFocus:
                    paused = false;
                    break;
                case sf::Event::MouseButtonPressed:
                    paused = !paused;
                    break;
                default:
                    break;
            }
        }
        window.clear();
        
        if (!paused) update();
        
        txt.update(pixels);
        sprite.setTexture(txt);
        window.draw(sprite);
        window.display();
        
        //sf::sleep(sf::milliseconds(30));
    }
    return 0;
}




















