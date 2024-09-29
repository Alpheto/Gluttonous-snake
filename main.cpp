#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm> // std::find
#undef main // 这样就可以解决 undefwinmain 的问题
// 游戏设置
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int CELL_SIZE = 20;
const int BARRIERS_NUM = 10;


// 枚举方向
enum Direction { UP, DOWN, LEFT, RIGHT };

// 枚举游戏的状态
enum GameState { MENU, PLAYING, SETTING, GAME_OVER };

// 位置结构体
struct Position {
    int x, y;

    // 重载运算符以便 std::find 能够比较 Position 对象
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
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

// 文本渲染函数
SDL_Texture* drawText(const std::string& text, TTF_Font* font, SDL_Color color, SDL_Renderer* renderer) {
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (textSurface == nullptr) {
        std::cerr << "Failed to render text! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    return texture;
}

// 游戏类
class SnakeGame {
public:
    SnakeGame();
    ~SnakeGame();
    void run();

private:
    SDL_Texture* menu_backgroundTexture;
    SDL_Texture* game_backgroundTexture;
    SDL_Texture* startButtonTexture;
    SDL_Texture* menuButtonTexture;
    SDL_Texture* additionTexture;
    SDL_Texture* subtractionTexture;
    SDL_Texture* backTexture;
    SDL_Texture* setting_backgroundTexture;
    SDL_Texture* snakeHeadTexture;
    SDL_Texture* snakeBodyTexture;
    SDL_Texture* snakeTailTexture;
    SDL_Texture* foodTexture;
    SDL_Texture* barrierTexture;

    Mix_Music* bgm;
    TTF_Font* font; // 字体

    void processInput();
    void update();
    void resetGame();
    void render();
    void renderMenu();
    void renderGame();
    void renderSetting();
    void renderGameOver();
    void generateFood();
    void generateBarriers();
    bool checkCollision();
    bool isButtonClicked(int x, int y, int btnx, int btny, int btnw, int btnh);

    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    GameState gameState;
    Direction dir;
    std::vector<Position> snake;
    Position food;
    std::vector<Position> barriers;
    Uint32 gameOverStartTime;
    bool growSnake;
};

SnakeGame::SnakeGame()
        : window(nullptr), renderer(nullptr), running(true), gameState(MENU), dir(RIGHT), growSnake(false),
        menu_backgroundTexture(nullptr), startButtonTexture(nullptr), menuButtonTexture(nullptr),
        snakeHeadTexture(nullptr), snakeBodyTexture(nullptr), snakeTailTexture(nullptr), foodTexture(nullptr),barrierTexture(nullptr),
        bgm(nullptr), additionTexture(nullptr), subtractionTexture(nullptr), backTexture(nullptr), setting_backgroundTexture(nullptr),
        game_backgroundTexture(nullptr), font(nullptr){
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
    // 初始化 SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        running = false;
        return;
    }
    // 初始化 SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
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
    menu_backgroundTexture = loadTexture("picture\\Menu_background.png", renderer);
    game_backgroundTexture = loadTexture("picture\\gamebackground.jpg", renderer);
    startButtonTexture = loadTexture("picture\\startbutton.png", renderer);
    menuButtonTexture = loadTexture("picture\\setting.png", renderer);
    additionTexture = loadTexture("picture\\addition.png", renderer); // 增加音量按钮的图片
    subtractionTexture = loadTexture("picture\\subtraction.png", renderer); // 减少音量按钮的图片
    backTexture = loadTexture("picture\\back.png", renderer);
    setting_backgroundTexture = loadTexture("picture\\settingbackground.jpg", renderer);
    foodTexture = loadTexture("picture\\food.png", renderer);
    barrierTexture = loadTexture("picture\\barrier.png", renderer);
    snakeHeadTexture = loadTexture("picture\\Snakehead.png", renderer);
    snakeBodyTexture = loadTexture("picture\\Snakebody.png", renderer);
    snakeTailTexture = loadTexture("picture\\Snaketail.png", renderer);
    bgm = Mix_LoadMUS("music\\MenuBGM.mp3");
    if (bgm == nullptr) {
        std::cerr << "Failed to load BGM! SDL_mixer Error: " << Mix_GetError() << std::endl;
        running = false;
        return;
    }

    Mix_PlayMusic(bgm, -1);  // 无限循环播放

    // 加载字体
    font = TTF_OpenFont("font\\English.ttf", 24); // 替换为你的字体文件路径
    if (font == nullptr) {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        running = false;
        return;
    }

    if (menu_backgroundTexture == nullptr || startButtonTexture == nullptr || menuButtonTexture == nullptr ||
        snakeHeadTexture == nullptr || snakeBodyTexture == nullptr || snakeTailTexture == nullptr || foodTexture == nullptr || barrierTexture == nullptr) {
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

    // 生成障碍物
    generateBarriers();
}

SnakeGame::~SnakeGame() {
    Mix_FreeMusic(bgm);
    Mix_CloseAudio();
    TTF_CloseFont(font); // 关闭字体

    SDL_DestroyTexture(menu_backgroundTexture);
    SDL_DestroyTexture(game_backgroundTexture);
    SDL_DestroyTexture(setting_backgroundTexture);
    SDL_DestroyTexture(startButtonTexture);
    SDL_DestroyTexture(menuButtonTexture);
    SDL_DestroyTexture(additionTexture);
    SDL_DestroyTexture(subtractionTexture);
    SDL_DestroyTexture(backTexture);
    SDL_DestroyTexture(foodTexture);
    SDL_DestroyTexture(barrierTexture);
    SDL_DestroyTexture(snakeHeadTexture);
    SDL_DestroyTexture(snakeBodyTexture);
    SDL_DestroyTexture(snakeTailTexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
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
                resetGame();
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
        } else if (event.type == SDL_MOUSEBUTTONDOWN && gameState == SETTING){
            int x,y;
            SDL_GetMouseState(&x, &y);

            if(isButtonClicked(x, y, SCREEN_WIDTH / 2 - 150, 300, 80, 50)) {
                int volume = Mix_VolumeMusic(-1);
                volume = std::min(128, volume + 28);
                Mix_VolumeMusic(volume);
            }

            if (isButtonClicked(x, y, SCREEN_WIDTH / 2 + 50, 300, 80, 50)) {
                int volume = Mix_VolumeMusic(-1);  // 获取当前音量
                volume = std::max(0, volume - 28);  // 减少音量
                Mix_VolumeMusic(volume);
            }

            if(isButtonClicked(x, y, 0, 0, 50, 50)) {
                gameState = MENU;
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
            generateBarriers();// 障碍物也进行更新
        }

        // 检查是否碰到障碍物以及边界
        // 检查碰撞
        if (checkCollision()) {
            gameState = GAME_OVER;
            gameOverStartTime = SDL_GetTicks();
            return;
        }

        // 更新蛇的位置
        snake.insert(snake.begin(), newHead);
        if (!growSnake) {
            snake.pop_back();
        } else {
            growSnake = false;
        }
    } else if(gameState == GAME_OVER) {
        if(SDL_GetTicks() - gameOverStartTime >= 5000) {
            gameState = MENU;
        }
    }
}

void SnakeGame::render() {
    if (gameState == MENU) {
        renderMenu();
    } else if (gameState == PLAYING) {
        renderGame();
    } else if (gameState == SETTING) {
        renderSetting();
    } else if(gameState == GAME_OVER) {
        renderGameOver();
    }
}

void SnakeGame::renderMenu() {
    // 清屏
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // 渲染背景
    SDL_RenderCopy(renderer, menu_backgroundTexture, nullptr, nullptr);  // 将整个窗口渲染为背景图片

    // 渲染“开始游戏”按钮
    SDL_Rect startButtonRect = {SCREEN_WIDTH / 2 - 50, 300, 80, 50};  // 定义按钮的位置和大小
    SDL_RenderCopy(renderer, startButtonTexture, nullptr, &startButtonRect);

    // 渲染“设置”按钮
    SDL_Rect settingsButtonRect = {SCREEN_WIDTH / 2 - 50, 400, 80, 50};
    SDL_RenderCopy(renderer, menuButtonTexture, nullptr, &settingsButtonRect);

    // 显示渲染的内容
    SDL_RenderPresent(renderer);
}

void SnakeGame:: renderSetting()
{
    // 清屏
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    //绘制设置界面
    SDL_RenderCopy(renderer, setting_backgroundTexture, nullptr, nullptr);//背景

    //声音加减按钮的渲染
    SDL_Rect voiceButtonAddition = {SCREEN_WIDTH / 2 - 150, 300, 80, 50};
    SDL_RenderCopy(renderer, additionTexture, nullptr, &voiceButtonAddition);
    SDL_Rect voiceButtonSubtraction = {SCREEN_WIDTH / 2 +50, 300, 80, 50};
    SDL_RenderCopy(renderer, subtractionTexture, nullptr, &voiceButtonSubtraction);

    //返回键的操作
    SDL_Rect backButton = {0, 0, 50, 50};
    SDL_RenderCopy(renderer, backTexture, nullptr, &backButton);
    //显示渲染的内容
    SDL_RenderPresent(renderer);

}

void SnakeGame::renderGame() {
    // 清屏
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, game_backgroundTexture, nullptr, nullptr);//背景
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
    SDL_Rect foodRect = {food.x, food.y, CELL_SIZE, CELL_SIZE};
    SDL_RenderCopy(renderer, foodTexture, nullptr, &foodRect); // 使用纹理绘制食物

    // 绘制障碍物
    for (const auto& barrier : barriers) {
        SDL_Rect barrierRect = {barrier.x, barrier.y, CELL_SIZE, CELL_SIZE};
        SDL_RenderCopy(renderer, barrierTexture, nullptr, &barrierRect); 
    }
    // 显示更新
    SDL_RenderPresent(renderer);
}

// 添加 renderGameOver 方法
void SnakeGame::renderGameOver() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Color textColor = {255, 0, 0}; // 红色
    SDL_Texture* textTexture = drawText("DIE", font, textColor, renderer); // 渲染文本
    if (textTexture) {
        SDL_Rect textRect = {SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 25, 100, 50}; // 设置文本位置和大小
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect); // 绘制文本
        SDL_DestroyTexture(textTexture); // 释放纹理
    }

    SDL_RenderPresent(renderer);
}

void SnakeGame::resetGame() {
    // 清空蛇的身体
    snake.clear();
    // 重置蛇的位置
    snake.push_back({SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2});
    snake.push_back({SCREEN_WIDTH / 2 - CELL_SIZE, SCREEN_HEIGHT / 2});
    snake.push_back({SCREEN_WIDTH / 2 - 2 * CELL_SIZE, SCREEN_HEIGHT / 2});

    // 生成新的食物
    generateFood();

    // 生成新的障碍物
    generateBarriers();

    // 重置游戏状态
    gameState = PLAYING;
}

void SnakeGame::generateFood() {
    food.x = (rand() % (SCREEN_WIDTH / CELL_SIZE)) * CELL_SIZE;
    food.y = (rand() % (SCREEN_HEIGHT / CELL_SIZE)) * CELL_SIZE;
}

void SnakeGame::generateBarriers() {
    barriers.clear();
    for (int i = 0; i < BARRIERS_NUM; ++i) {
        int posX = rand() % (SCREEN_WIDTH / CELL_SIZE) * CELL_SIZE;
        int posY = rand() % (SCREEN_HEIGHT / CELL_SIZE) * CELL_SIZE;
        Position pos = {posX, posY};
        // 检查生成的障碍物是否与蛇重叠
        if (std::find(snake.begin(), snake.end(), pos) != snake.end()) {
            continue;// 如果重叠，重新生成
        }
        // 检查生成的障碍物是否与食物重叠
        if (posX == food.x && posY == food.y) {
            continue;// 如果重叠，重新生成
        }
        // 检查生成的障碍物是否与蛇头过于接近
        if (abs(posX - snake[0].x) <= 10*CELL_SIZE && abs(posY - snake[0].y) <= 10*CELL_SIZE) {
            continue;// 如果接近，重新生成
        }
        //将障碍物添加到障碍物列表中
        barriers.push_back({rand() % (SCREEN_WIDTH / CELL_SIZE) * CELL_SIZE, rand() % (SCREEN_HEIGHT / CELL_SIZE) * CELL_SIZE});
    }
}

bool SnakeGame::checkCollision() {
    const Position& head = snake[0];

    // 检查是否撞墙
    if (head.x < 0 || head.x >= SCREEN_WIDTH || head.y < 0 || head.y >= SCREEN_HEIGHT) {
        std::cout<<"collision with wall";
        return true;
    }

    // 检查是否撞到障碍物
    for (const Position& barrier : barriers) {
        if (head.x == barrier.x && head.y == barrier.y) {
            std::cout<<"collision with barrier";
            return true;
        }
    }

    // 检查是否撞到自己
    for (size_t i = 1; i < snake.size(); ++i) {
        if (head.x == snake[i].x && head.y == snake[i].y) {
            std::cout<<"collision with self";
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
