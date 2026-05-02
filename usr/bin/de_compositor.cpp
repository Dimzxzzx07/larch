#include <cstdint>
#include <cstddef>

class Window {
private:
    int x, y, width, height;
    uint32_t* buffer;
public:
    Window(int w, int h) : x(0), y(0), width(w), height(h) {
        buffer = new uint32_t[w * h];
    }
    
    void move(int new_x, int new_y) {
        x = new_x;
        y = new_y;
    }
    
    void draw() {
        for (int i = 0; i < width * height; i++) {
            ((uint32_t*)0xB8000)[(y + i / width) * 1024 + x + i % width] = buffer[i];
        }
    }
    
    ~Window() {
        delete[] buffer;
    }
};

class Compositor {
private:
    static const int MAX_WINDOWS = 64;
    Window* windows[MAX_WINDOWS];
    int window_count;
    
public:
    Compositor() : window_count(0) {
        for (int i = 0; i < MAX_WINDOWS; i++) windows[i] = nullptr;
    }
    
    void add_window(Window* win) {
        if (window_count < MAX_WINDOWS) {
            windows[window_count++] = win;
        }
    }
    
    void render() {
        for (int i = 0; i < window_count; i++) {
            if (windows[i]) windows[i]->draw();
        }
    }
};

extern "C" int main() {
    Compositor comp;
    Window main_win(800, 600);
    comp.add_window(&main_win);
    comp.render();
    while(1);
    return 0;
}