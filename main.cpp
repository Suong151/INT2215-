#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>

using namespace std;

// Các hằng số
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Khai báo biến toàn cục
SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
SDL_Texture* gBackgroundTexture = nullptr;
SDL_Texture* gAppleTexture = nullptr;
SDL_Texture* gBasketTexture = nullptr;
SDL_Texture* gHeartTexture = nullptr;
SDL_Texture* gStartBackgroundTexture = nullptr;

// Các biến âm thanh
Mix_Music* gBackgroundMusic = nullptr;
Mix_Chunk* gHappySound = nullptr;
Mix_Chunk* gSadSound = nullptr;

// Các biến trò chơi
int score = 0;
int lives = 3;
int appleFallCount = 0;

// Font chữ toàn cục
TTF_Font* gFont = nullptr;

// Hàm khởi tạo SDL
bool init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    gWindow = SDL_CreateWindow("Game Hứng Táo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == nullptr) {
        cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (gRenderer == nullptr) {
        cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << endl;
        return false;
    }

    // Khởi tạo SDL_ttf
    if (TTF_Init() == -1) {
        cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << endl;
        return false;
    }

    // Khởi tạo SDL_mixer
    if (Mix_Init(MIX_INIT_MP3) == 0) {
        cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }

    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }

    // Tải âm thanh
    gBackgroundMusic = Mix_LoadMUS("backgroundmusic.mp3");
    if (gBackgroundMusic == nullptr) {
        cerr << "Failed to load background music! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }

    gHappySound = Mix_LoadWAV("happysound.mp3");
    if (gHappySound == nullptr) {
        cerr << "Failed to load happy sound! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }

    gSadSound = Mix_LoadWAV("sadsound.mp3");
    if (gSadSound == nullptr) {
        cerr << "Failed to load sad sound! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }

    // Tải font chữ
    gFont = TTF_OpenFont("UTM Cookies.ttf", 40);  // Đảm bảo đường dẫn và tên font chính xác
    if (gFont == nullptr) {
        cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << endl;
        return false;
    }

    return true;
}

// Hàm hủy SDL
void close() {
    SDL_DestroyTexture(gAppleTexture);
    SDL_DestroyTexture(gBasketTexture);
    SDL_DestroyTexture(gBackgroundTexture);
    SDL_DestroyTexture(gHeartTexture);
    SDL_DestroyTexture(gStartBackgroundTexture);
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);

    TTF_CloseFont(gFont);  // Hủy font
    TTF_Quit();  // Đóng SDL_ttf

    Mix_FreeMusic(gBackgroundMusic);  // Giải phóng nhạc nền
    Mix_FreeChunk(gHappySound);  // Giải phóng âm thanh vui
    Mix_FreeChunk(gSadSound);  // Giải phóng âm thanh buồn

    IMG_Quit();
    SDL_Quit();
}

// Hàm tải hình ảnh
SDL_Texture* loadTexture(const string& path) {
    SDL_Texture* newTexture = nullptr;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr) {
        cerr << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << endl;
    } else {
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        SDL_FreeSurface(loadedSurface);
        if (newTexture == nullptr) {
            cerr << "Unable to create texture from " << path << "! SDL Error: " << SDL_GetError() << endl;
        }
    }
    return newTexture;
}

// Hàm vẽ background
void drawBackground() {
    SDL_RenderCopy(gRenderer, gBackgroundTexture, NULL, NULL);
}

// Hàm vẽ màn hình Game Start
void drawStartScreen() {
    SDL_RenderCopy(gRenderer, gStartBackgroundTexture, NULL, NULL);
}

// Hàm vẽ giỏ
void drawBasket(int x, int y) {
    SDL_Rect basketRect = {x, y, 250, 125};
    SDL_RenderCopy(gRenderer, gBasketTexture, NULL, &basketRect);
}

// Hàm vẽ táo
void drawApple(int x, int y) {
    SDL_Rect appleRect = {x, y, 100, 100};
    SDL_RenderCopy(gRenderer, gAppleTexture, NULL, &appleRect);
}

// Hàm vẽ trái tim (hiển thị số lượng tim)
void drawHearts(int lives) {
    int heartWidth = 40;
    int heartHeight = 40;
    for (int i = 0; i < lives; i++) {
        SDL_Rect heartRect = {20 + i * (heartWidth + 10), 20, heartWidth, heartHeight};
        SDL_RenderCopy(gRenderer, gHeartTexture, NULL, &heartRect);
    }
}

// Hàm vẽ điểm
void drawScore(int score) {
    string scoreText = "Score: " + to_string(score);

    // Tạo một surface từ văn bản
    SDL_Color textColor = {255, 255, 255}; // Màu trắng
    SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, scoreText.c_str(), textColor);

    // Tạo texture từ surface
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
    SDL_FreeSurface(textSurface);  // Giải phóng surface sau khi tạo texture

    // Vị trí hiển thị điểm số
    SDL_Rect textRect = {SCREEN_WIDTH - 150, 20, 100, 30};  // Điều chỉnh vị trí và kích thước
    SDL_RenderCopy(gRenderer, textTexture, NULL, &textRect);

    // Giải phóng texture
    SDL_DestroyTexture(textTexture);
}
// Hàm phát nhạc nền
void playBackgroundMusic() {
    if (Mix_PlayMusic(gBackgroundMusic, -1) == -1) {
        cerr << "Mix_PlayMusic error: " << Mix_GetError() << endl;
    }
}

// Hàm phát âm thanh khi hứng được táo
void playHappySound() {
    if (Mix_PlayChannel(-1, gHappySound, 0) == -1) {
        cerr << "Mix_PlayChannel error: " << Mix_GetError() << endl;
    }
}

// Hàm phát âm thanh khi táo trượt khỏi giỏ
void playSadSound() {
    if (Mix_PlayChannel(-1, gSadSound, 0) == -1) {
        cerr << "Mix_PlayChannel error: " << Mix_GetError() << endl;
    }
}

// Hàm hiển thị màn hình Game Over và điểm số, với nút Replay và Quit
bool showGameOver() {
    string gameOverText = "Game Over!";
    string scoreText = "Final Score: " + to_string(score);
    string replayText = "Replay!";
    string quitText = "Quit";

    SDL_Color textColor = {255, 0, 0}; // Màu đỏ cho "Game Over"
    SDL_Color scoreColor = {255, 255, 255}; // Màu trắng cho điểm số
    SDL_Color replayColor = {0, 255, 0}; // Màu xanh cho nút replay
    SDL_Color quitColor = {255, 255, 0};  // Màu vàng cho Quit

    // Tạo một surface từ văn bản Game Over
    SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, gameOverText.c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
    SDL_FreeSurface(textSurface);

    // Tạo một surface từ điểm số
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(gFont, scoreText.c_str(), scoreColor);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(gRenderer, scoreSurface);
    SDL_FreeSurface(scoreSurface);

    // Tạo một surface từ hướng dẫn Replay
    SDL_Surface* replaySurface = TTF_RenderText_Solid(gFont, replayText.c_str(), replayColor);
    SDL_Texture* replayTexture = SDL_CreateTextureFromSurface(gRenderer, replaySurface);
    SDL_FreeSurface(replaySurface);

    // Tạo một surface từ hướng dẫn Quit
    SDL_Surface* quitSurface = TTF_RenderText_Solid(gFont, quitText.c_str(), quitColor);
    SDL_Texture* quitTexture = SDL_CreateTextureFromSurface(gRenderer, quitSurface);
    SDL_FreeSurface(quitSurface);

    // Vị trí hiển thị Game Over
    SDL_Rect textRect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 150, 200, 50};
    // Vị trí hiển thị điểm số
    SDL_Rect scoreRect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, 200, 50};
    // Vị trí hiển thị nút replay
    SDL_Rect replayRect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 50, 200, 50};
    // Vị trí hiển thị nút quit
    SDL_Rect quitRect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 150, 200, 50};

    // Vẽ Game Over và điểm số lên màn hình
    SDL_RenderCopy(gRenderer, textTexture, NULL, &textRect);
    SDL_RenderCopy(gRenderer, scoreTexture, NULL, &scoreRect);
    SDL_RenderCopy(gRenderer, replayTexture, NULL, &replayRect);
    SDL_RenderCopy(gRenderer, quitTexture, NULL, &quitRect);

    SDL_DestroyTexture(textTexture);
    SDL_DestroyTexture(scoreTexture);
    SDL_DestroyTexture(replayTexture);
    SDL_DestroyTexture(quitTexture);

    SDL_RenderPresent(gRenderer);

    // Chờ người chơi nhấp chuột vào nút replay hoặc quit
    SDL_Event e;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
                return false;  // Nếu thoát, trả về false
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);

                // Kiểm tra nếu người chơi nhấp vào nút replay
                if (x >= replayRect.x && x <= replayRect.x + replayRect.w &&
                    y >= replayRect.y && y <= replayRect.y + replayRect.h) {
                    return true;  // Nếu nhấp vào nút replay, trả về true để chơi lại
                }

                // Kiểm tra nếu người chơi nhấp vào nút quit
                if (x >= quitRect.x && x <= quitRect.x + quitRect.w &&
                    y >= quitRect.y && y <= quitRect.y + quitRect.h) {
                    SDL_Quit();  // Tắt SDL
                    exit(0);  // Thoát ứng dụng
                }
            }
        }
    }

    return false;  // Nếu không nhấp vào replay hoặc quit, trả về false
}

// Hàm hiển thị màn hình Game Start và chờ người chơi nhấp Start
bool showStartScreen() {
    string startText = "Click to Start!";
    SDL_Color textColor = {255, 255, 255}; // Màu trắng cho "Click to Start!"

    // Tạo một surface từ văn bản "Click to Start"
    SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, startText.c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
    SDL_FreeSurface(textSurface);

    // Vị trí hiển thị "Click to Start"
    SDL_Rect textRect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, 200, 50};

    // Vẽ background và text lên màn hình
    drawStartScreen();
    SDL_RenderCopy(gRenderer, textTexture, NULL, &textRect);

    SDL_DestroyTexture(textTexture);

    SDL_RenderPresent(gRenderer);

    // Chờ người chơi nhấp chuột vào nút Start
    SDL_Event e;
    bool startGame = false;
    while (!startGame) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                return false;  // Nếu thoát, trả về false
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                startGame = true;  // Khi nhấp chuột, bắt đầu game
            }
        }
    }

    return true;  // Trả về true để bắt đầu chơi
}

int main(int argc, char* args[]) {
    // Khởi tạo SDL và các thư viện cần thiết
    if (!init()) {
        cerr << "Failed to initialize!" << endl;
        return -1;
    }

    // Tải các hình ảnh
    gBackgroundTexture = loadTexture("background.png");
    gAppleTexture = loadTexture("apple.png");
    gBasketTexture = loadTexture("basket.png");
    gHeartTexture = loadTexture("heart.png");
    gStartBackgroundTexture = loadTexture("bgstart.png");

    if (gBackgroundTexture == nullptr || gAppleTexture == nullptr || gBasketTexture == nullptr || gHeartTexture == nullptr || gStartBackgroundTexture == nullptr) {
        cerr << "Failed to load images!" << endl;
        return -1;
    }

    // Phát nhạc nền
    playBackgroundMusic();

    bool quit = false;
    bool playAgain = true;

    // Hiển thị màn hình Game Start
    bool startGame = showStartScreen();  // Chờ người chơi nhấp Start

    if (!startGame) {
        cout << "Game Over!" << endl;
        close();
        return 0;  // Nếu người chơi không nhấp Start, thoát game
    }

    while (!quit) {
        // Reset lại các biến trò chơi
        score = 0;
        lives = 3;
        int basketX = (SCREEN_WIDTH - 250) / 2;
        int basketY = SCREEN_HEIGHT - 125;
        int appleX = rand() % (SCREEN_WIDTH - 100);
        int appleY = -100;

        bool playAgain = false;
        while (!playAgain && !quit) {
            SDL_Event e;
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                    playAgain = false;
                }
            }

            const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

            // Điều khiển giỏ
            if (currentKeyStates[SDL_SCANCODE_LEFT] && basketX > 0) {
                basketX -= 15;
            }
            if (currentKeyStates[SDL_SCANCODE_RIGHT] && basketX < SCREEN_WIDTH - 250) {
                basketX += 15;
            }

            // Rơi táo
            appleY += 3 + (score/5);
            if (appleY > SCREEN_HEIGHT) {
                appleY = -100;
                appleX = rand() % (SCREEN_WIDTH - 100);
                lives--;  // Mất một tim mỗi khi táo rơi ra ngoài
                playSadSound();  // Phát âm thanh khi táo trượt khỏi giỏ
                if (lives <= 0) {
                    // Game over khi không còn mạng
                    playAgain = showGameOver();  // Hiển thị màn hình Game Over và hỏi người chơi có muốn chơi lại
                }
            }

            if (appleY + 100 >= basketY && appleY + 100 <= basketY + 125 &&
                appleX + 100 > basketX && appleX < basketX + 250) {
                score++;
                appleY = -100;
                appleX = rand() % (SCREEN_WIDTH - 100);
                playHappySound();  // Phát âm thanh khi hứng được táo
            }

            // Xóa màn hình và vẽ lại các đối tượng
            SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
            SDL_RenderClear(gRenderer);

            drawBackground();
            drawBasket(basketX, basketY);
            drawApple(appleX, appleY);
            drawHearts(lives);  // Vẽ lại trái tim
            drawScore(score);

            SDL_RenderPresent(gRenderer);

            SDL_Delay(16);
        }
    }

    cout << "Game Over!" << endl;
    close();
    return 0;
}
