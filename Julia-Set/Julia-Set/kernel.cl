
#define WINDOW_SIZE 800
#define ITERATIONS 250
#define MANDELBROT 100

kernel void iteration(global char *output, float axies_size,
                       float julia_a, float julia_b) {
    int i = get_global_id(0);
    int x_coord = i % WINDOW_SIZE;
    int y_coord = (i - x_coord) / WINDOW_SIZE;
    float x = axies_size * ((2 * float(x_coord) / WINDOW_SIZE) - 1);
    float y = axies_size * ((2 * float(y_coord) / WINDOW_SIZE) - 1);
    
    float x_offset = (julia_a == MANDELBROT ? x : julia_a);
    float y_offset = (julia_b == MANDELBROT ? y : julia_b);
    float a = x, b = y;
    float aa = a*a, bb = b*b;
    
    int n = 0;
    while (n < ITERATIONS && aa + bb < 4.0) {
        b *= a;
        b += b + y_offset;
        a  = aa - bb + x_offset;
        aa = a*a; bb = b*b;
        n++;
    }
    output[i] = char(n);
}
