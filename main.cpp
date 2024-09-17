#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>  // 新增 SDL2_image 用于加载图片
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#undef main  // 解决 undefwinmain 的问题

// 游戏设置
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int CELL_SIZE = 40;
const int SPEED = 5;  // 新增速度变量，表示每次移动多少像素
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
    SDL_Texture* loadTexture(const std::string& path);

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* foodTexture;  // 食物纹理
    SDL_Texture* bodyTexture;  // 蛇身体纹理
    bool running;
    Direction dir;
    std::vector<Position> snake;
    Position food;
    bool growSnake;
};

// 加载图片的函数
SDL_Texture* SnakeGame::loadTexture(const std::string& path) {
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr) {
        std::cerr << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    SDL_FreeSurface(loadedSurface);
    return newTexture;
}

SnakeGame::SnakeGame()
        : window(nullptr), renderer(nullptr), foodTexture(nullptr), bodyTexture(nullptr), running(true), dir(RIGHT), growSnake(false) {
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

    // 初始化 SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        running = false;
        return;
    }

    // 加载食物和蛇身体图片
    foodTexture = loadTexture("C:\\Users\\ds555\\Desktop\\ProgramDesigning\\ProjectTeam\\Gluttonous-snake\\picture\\SpanishSeafoodNoddles.png");
    bodyTexture = loadTexture("C:\\Users\\ds555\\Desktop\\ProgramDesigning\\ProjectTeam\\Gluttonous-snake\\picture\\body1.png");
    if (!foodTexture || !bodyTexture) {
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
    SDL_DestroyTexture(foodTexture);
    SDL_DestroyTexture(bodyTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

void SnakeGame::run() {
    const int FPS = 5;
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
    for (const auto& segment : snake) {
        SDL_Rect rect = {segment.x, segment.y, CELL_SIZE, CELL_SIZE};
        SDL_RenderCopy(renderer, bodyTexture, nullptr, &rect);  // 使用蛇身体图片
    }

    // 绘制食物
    SDL_Rect foodRect = {food.x, food.y, CELL_SIZE, CELL_SIZE};
    SDL_RenderCopy(renderer, foodTexture, nullptr, &foodRect);  // 使用食物图片

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
