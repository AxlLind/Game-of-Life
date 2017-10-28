#include <SFML/Graphics.hpp>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 800
#define CELL_SIZE 4

#define WIDTH  WINDOW_WIDTH / CELL_SIZE
#define HEIGHT WINDOW_WIDTH / CELL_SIZE

#define DEAD   75
#define ALIVE  0

sf::Uint8 *cells      = new sf::Uint8[WIDTH * HEIGHT];
sf::Uint8 *cells_prev = new sf::Uint8[WIDTH * HEIGHT];
sf::Uint8 *pixels     = new sf::Uint8[4 * CELL_SIZE * WIDTH * CELL_SIZE * HEIGHT];

int get_index(int x, int y) {
    return y * WIDTH + x;
}

int adjacent(int x, int y) {
    int num_adjacent = 0;
    if ( cells_prev[ get_index(x-1, y-1) ] == ALIVE ) ++num_adjacent;
    if ( cells_prev[ get_index(x  , y-1) ] == ALIVE ) ++num_adjacent;
    if ( cells_prev[ get_index(x+1, y-1) ] == ALIVE ) ++num_adjacent;
    if ( cells_prev[ get_index(x-1, y  ) ] == ALIVE ) ++num_adjacent;
    if ( cells_prev[ get_index(x+1, y  ) ] == ALIVE ) ++num_adjacent;
    if ( cells_prev[ get_index(x-1, y+1) ] == ALIVE ) ++num_adjacent;
    if ( cells_prev[ get_index(x  , y+1) ] == ALIVE ) ++num_adjacent;
    if ( cells_prev[ get_index(x+1, y+1) ] == ALIVE ) ++num_adjacent;
    return num_adjacent;
}

void update_cell(int x, int y) {
    int num_adjacent = adjacent(x,y);
    int i = get_index(x,y);
    
    if (cells[i] == DEAD) {
        if (num_adjacent == 3)
            cells[i] = ALIVE;
    } else if (num_adjacent < 2 || num_adjacent > 3) {
        cells[i] = DEAD;
    }
    
}

void draw_cell(int x, int y, sf::Uint8 color) {
    for (int i = 0; i < CELL_SIZE; ++i) {
        for (int j = 0; j < CELL_SIZE; ++j) {
            int index = 4 * (CELL_SIZE * (WIDTH * (CELL_SIZE * y + j) + x) + i);
            pixels[index++] = color;
            pixels[index++] = color;
            pixels[index++] = color;
            pixels[index++] = 255;
        }
    }
}

void update_simulation() {
    std::copy(cells, cells + WIDTH * HEIGHT, cells_prev);
    for (int x = 1; x < WIDTH-1; ++x) {
        for (int y = 1; y < HEIGHT-1; ++y) {
            update_cell(x,y);
            draw_cell(x,y,cells[get_index(x, y)]);
            // by skipping the drawing of border cells we get a black border
        }
    }
}

int main() {
    if (WINDOW_WIDTH % CELL_SIZE != 0 || WINDOW_HEIGHT % CELL_SIZE != 0) return 1;
    
    srand((unsigned int) time(NULL));
    for (int i = 0; i < WIDTH * HEIGHT; ++i) {
        cells[i] = rand() % 2 == 0 ? ALIVE : DEAD;
    }
    
    sf::Event e;
    sf::Sprite sprite;
    sf::Texture txt;
    txt.create(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Game of Life");
    bool paused = false;
    
    while(window.isOpen()) {
        while(window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) window.close();
            if (e.type == sf::Event::MouseButtonPressed) paused = !paused;
        }
        sf::sleep(sf::milliseconds(20)); // too fast otherwise
        
        if (paused) continue;
        
        update_simulation();
        txt.update(pixels);
        sprite.setTexture(txt);
        window.draw(sprite);
        
        window.display();
    }
    
    return 0;
}
