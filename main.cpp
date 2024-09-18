#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#undef main // 这样就可以解决 undefwinmain 的问题
// 游戏设置
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int CELL_SIZE = 20;

// 枚举方向
enum Direction { UP, DOWN, LEFT, RIGHT };

// 枚举游戏的状态
enum GameState { MENU, PLAYING, SETTING };

// 位置结构体
struct Position {
    int x, y;
};

// 加载图片为纹理的函数
SDL_Texture* loadTexture(const std::string& path, SDL_Renderer* renderer) {
    SDL_Texture* newTexture = nullptr;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr) {
        std::cerr << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << std::endl;
    } else {
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (newTexture == nullptr) {
            std::cerr << "Unable to create texture from " << path << "! SDL Error: " << SDL_GetError() << std::endl;
        }
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
}

// 游戏类
class SnakeGame {
public:
    SnakeGame();
    ~SnakeGame();
    void run();

private:
    SDL_Texture* backgroundTexture;
    SDL_Texture* startButtonTexture;
    SDL_Texture* menuButtonTexture;

    SDL_Texture* snakeHeadTexture;
    SDL_Texture* snakeBodyTexture;
    SDL_Texture* snakeTailTexture;

    void processInput();
    void update();
    void render();
    void renderMenu();
    void renderGame();
    void generateFood();
    bool checkCollision();
    bool isButtonClicked(int x, int y, int btnx, int btny, int btnw, int btnh);

    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    GameState gameState;
    Direction dir;
    std::vector<Position> snake;
    Position food;
    bool growSnake;
};

SnakeGame::SnakeGame()
        : window(nullptr), renderer(nullptr), running(true), gameState(MENU), dir(RIGHT), growSnake(false),
          backgroundTexture(nullptr), startButtonTexture(nullptr), menuButtonTexture(nullptr),
          snakeHeadTexture(nullptr), snakeBodyTexture(nullptr), snakeTailTexture(nullptr) {
    // 初始化 SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        running = false;
        return;
    }
    // 初始化 SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
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
    // 加载图片纹理
    backgroundTexture = loadTexture("picture\\background.png", renderer);
    startButtonTexture = loadTexture("picture\\startbutton.png", renderer);
    menuButtonTexture = loadTexture("picture\\setting.png", renderer);

    snakeHeadTexture = loadTexture("picture\\Snakehead.png", renderer);
    snakeBodyTexture = loadTexture("picture\\Snakebody.png", renderer);
    snakeTailTexture = loadTexture("picture\\Snaketail.png", renderer);

    if (backgroundTexture == nullptr || startButtonTexture == nullptr || menuButtonTexture == nullptr ||
        snakeHeadTexture == nullptr || snakeBodyTexture == nullptr || snakeTailTexture == nullptr) {
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
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(startButtonTexture);
    SDL_DestroyTexture(menuButtonTexture);

    SDL_DestroyTexture(snakeHeadTexture);
    SDL_DestroyTexture(snakeBodyTexture);
    SDL_DestroyTexture(snakeTailTexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
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
        } else if (event.type == SDL_MOUSEBUTTONDOWN && gameState == MENU) {
            int x, y;
            SDL_GetMouseState(&x, &y);

            if (isButtonClicked(x, y, SCREEN_WIDTH / 2 - 50, 300, 100, 50)) {
                gameState = PLAYING;
            }

            if (isButtonClicked(x, y, SCREEN_WIDTH / 2 - 50, 400, 100, 50)) {
                gameState = SETTING;
            }
        } else if (event.type == SDL_KEYDOWN && gameState == PLAYING) {
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
    if (gameState == PLAYING) {
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
}

void SnakeGame::render() {
    if (gameState == MENU) {
        renderMenu();
    } else if (gameState == PLAYING) {
        renderGame();
    }
}

void SnakeGame::renderMenu() {
    // 清屏
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // 渲染背景
    SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);  // 将整个窗口渲染为背景图片

    // 渲染“开始游戏”按钮
    SDL_Rect startButtonRect = {SCREEN_WIDTH / 2 - 50, 300, 80, 50};  // 定义按钮的位置和大小
    SDL_RenderCopy(renderer, startButtonTexture, nullptr, &startButtonRect);

    // 渲染“设置”按钮
    SDL_Rect settingsButtonRect = {SCREEN_WIDTH / 2 - 50, 400, 80, 50};
    SDL_RenderCopy(renderer, menuButtonTexture, nullptr, &settingsButtonRect);

    // 显示渲染的内容
    SDL_RenderPresent(renderer);
}

void SnakeGame::renderGame() {
    // 清屏
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // 绘制蛇
    for (size_t i = 0; i < snake.size(); ++i) {
        SDL_Rect rect = {snake[i].x, snake[i].y, CELL_SIZE, CELL_SIZE};
        SDL_Texture* texture = nullptr;
        double angle = 0.0;

        if (i == 0) {
            // 蛇头
            texture = snakeHeadTexture;
            // 根据方向设置角度
            switch (dir) {
                case UP: angle = 90.0; break;
                case DOWN: angle = 270.0; break;
                case LEFT: angle = 0.0; break;
                case RIGHT: angle = 180.0; break;
            }
        }else if (i == snake.size() - 1) {
            // 蛇尾
            texture = snakeTailTexture;
            if (snake.size() > 1) {
                // 当前段
                const Position& currentSegment = snake[i];
                // 前一个段
                const Position& prevSegment = snake[i + 1];

                // 计算前一个段与当前段之间的方向
                if (prevSegment.x == currentSegment.x) {
                    // 垂直
                    if (prevSegment.y < currentSegment.y) {
                        // 向下
                        angle = 90.0;
                    } else {
                        // 向上
                        angle = 270.0;
                    }
                } else {
                    // 水平
                    if (prevSegment.x < currentSegment.x) {
                        // 向右
                        angle = 0.0;
                    } else {
                        // 向左
                        angle = 180.0;
                    }
                }
            }
        } else {
            // 蛇身
            texture = snakeBodyTexture;
            // 这里你可以设置蛇身的角度（如果需要）
        }

        if (texture != nullptr) {
            SDL_RenderCopyEx(renderer, texture, nullptr, &rect, angle, nullptr, SDL_FLIP_NONE);
        }
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

bool SnakeGame::isButtonClicked(int x, int y, int btnX, int btnY, int btnW, int btnH) {
    return x >= btnX && x <= btnX + btnW && y >= btnY && y <= btnY + btnH;
}

int main(int argc, char* argv[]) {
    SnakeGame game;
    game.run();
    return 0;
}
