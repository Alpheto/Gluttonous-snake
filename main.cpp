#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#undef main//这样就可以解决undefwinmain的问题
// 游戏设置
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int CELL_SIZE = 20;

// 枚举方向
enum Direction { UP, DOWN, LEFT, RIGHT };

// 位置结构体
struct Position {
    int x, y;
};

// 游戏类
class SnakeGame {
public:
    SnakeGame();
    ~SnakeGame();
    void run();

private:
    void processInput();
    void update();
    void render();
    void generateFood();
    bool checkCollision();

    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    Direction dir;
    std::vector<Position> snake;
    Position food;
    bool growSnake;
};

SnakeGame::SnakeGame()
        : window(nullptr), renderer(nullptr), running(true), dir(RIGHT), growSnake(false) {
    // 初始化 SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        running = false;
        return;
    }

    // 创建窗口
    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        running = false;
        return;
    }

    // 创建渲染器
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        running = false;
        return;
    }

    // 初始化随机数种子
    srand(static_cast<unsigned int>(time(0)));

    // 初始化蛇
    snake.push_back({SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2});
    snake.push_back({SCREEN_WIDTH / 2 - CELL_SIZE, SCREEN_HEIGHT / 2});
    snake.push_back({SCREEN_WIDTH / 2 - 2 * CELL_SIZE, SCREEN_HEIGHT / 2});

    // 生成食物
    generateFood();
}

SnakeGame::~SnakeGame() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void SnakeGame::run() {
    const int FPS = 10;
    const int frameDelay = 1000 / FPS;
    Uint32 frameStart;
    int frameTime;

    while (running) {
        frameStart = SDL_GetTicks();

        processInput();
        update();
        render();

        // 控制帧率
        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
}

void SnakeGame::processInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            running = false;
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    if (dir != DOWN) dir = UP;
                    break;
                case SDLK_DOWN:
                    if (dir != UP) dir = DOWN;
                    break;
                case SDLK_LEFT:
                    if (dir != RIGHT) dir = LEFT;
                    break;
                case SDLK_RIGHT:
                    if (dir != LEFT) dir = RIGHT;
                    break;
            }
        }
    }
}

void SnakeGame::update() {
    // 移动蛇
    Position newHead = snake[0];
    switch (dir) {
        case UP: newHead.y -= CELL_SIZE; break;
        case DOWN: newHead.y += CELL_SIZE; break;
        case LEFT: newHead.x -= CELL_SIZE; break;
        case RIGHT: newHead.x += CELL_SIZE; break;
    }

    // 检查是否吃到食物
    if (newHead.x == food.x && newHead.y == food.y) {
        growSnake = true;
        generateFood();
    }

    // 检查碰撞
    if (checkCollision()) {
        running = false;
        return;
    }

    // 更新蛇的位置
    snake.insert(snake.begin(), newHead);
    if (!growSnake) {
        snake.pop_back();
    } else {
        growSnake = false;
    }
}

void SnakeGame::render() {
    // 清屏
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // 绘制蛇
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    for (const auto& segment : snake) {
        SDL_Rect rect = {segment.x, segment.y, CELL_SIZE, CELL_SIZE};
        SDL_RenderFillRect(renderer, &rect);
    }

    // 绘制食物
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect foodRect = {food.x, food.y, CELL_SIZE, CELL_SIZE};
    SDL_RenderFillRect(renderer, &foodRect);

    // 显示更新
    SDL_RenderPresent(renderer);
}

void SnakeGame::generateFood() {
    food.x = (rand() % (SCREEN_WIDTH / CELL_SIZE)) * CELL_SIZE;
    food.y = (rand() % (SCREEN_HEIGHT / CELL_SIZE)) * CELL_SIZE;
}

bool SnakeGame::checkCollision() {
    const Position& head = snake[0];

    // 检查是否撞墙
    if (head.x < 0 || head.x >= SCREEN_WIDTH || head.y < 0 || head.y >= SCREEN_HEIGHT) {
        return true;
    }

    // 检查是否撞到自己
    for (size_t i = 1; i < snake.size(); ++i) {
        if (head.x == snake[i].x && head.y == snake[i].y) {
            return true;
        }
    }

    return false;
}

int main(int argc, char* argv[]) {
    SnakeGame game;
    game.run();
    return 0;
}


