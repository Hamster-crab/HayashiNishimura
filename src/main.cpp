#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <SDL_image.h>
#include "Camera2D.h"
#include <SDL_mixer.h>
#include <filesystem>
#include <cstdlib>
#include <string>
#include <vector>

// プレイヤーやオブジェクトの矩形
struct Rectangle {
    int x;
    int y;
    int Width;
    int Height;
};

// テキスト描画関数
void drawText(SDL_Renderer* renderer, float R, float G, float B, TTF_Font* font, const char* Text, float x, float y)
{
    SDL_Color color = {static_cast<Uint8>(R), static_cast<Uint8>(G), static_cast<Uint8>(B)};
    
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, Text, color);
    if (!surface) {
        std::cerr << "テキストのレンダリングに失敗しました: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    int w = surface->w;
    int h = surface->h;
    SDL_FreeSurface(surface); // SDL_Surfaceを解放するのはサイズ取得後

    SDL_Rect dstRect = {static_cast<int>(x), static_cast<int>(y), w, h};
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);

    SDL_DestroyTexture(texture);
}

void drawNumber(SDL_Renderer* renderer, float R, float G, float B, TTF_Font* font, int number, float x, float y)
{
    // 数字を文字列に変換
    std::string str = std::to_string(number);

    // drawText に渡す（文字列を .c_str() で const char* に変換）
    drawText(renderer, R, G, B, font, str.c_str(), x, y);
}

// キーが押されたかどうかをチェックする関数
bool isKeyPressed(SDL_Event& event, SDL_Keycode key) {
    return (event.type == SDL_KEYDOWN && event.key.keysym.sym == key);
}

int main(int argc, char* argv[]) {
    std::filesystem::path basePath = std::filesystem::current_path();
    
    std::filesystem::path ikisugiMusicPath; //musicNumber 1
    std::filesystem::path lethal_chinpoMusicPath; // musicNumber2
    std::filesystem::path lethal_dealMusicPath; // musicNumber3
    
    std::filesystem::path noJapaneseFontFontsPath;
    std::filesystem::path dotGothicFontsPath;

    std::filesystem::path woodLightImagePath;

    std::vector<std::string> args(argv + 1, argv + argc);
    for (const std::string& arg : args) {
        if (arg == "debug")
        {
            ikisugiMusicPath = basePath / "compiler" / "run" / "data" / "music" / "ikisugiyou.wav";
            lethal_chinpoMusicPath = basePath / "compiler" / "run" / "data" / "music" / "lethalchinpo.wav";
            lethal_dealMusicPath = basePath / "compiler" / "run" / "data" / "music" / "LETHAL_DEAL.wav";
            noJapaneseFontFontsPath = basePath / "compiler"  / "run" / "data" / "fonts" / "8-bit-no-ja" / "8bitOperatorPlus8-Bold.ttf";
            dotGothicFontsPath = basePath / "compiler"  / "run" / "data" / "fonts" / "ja-16-bit" / "DotGothic16-Regular.ttf";
            woodLightImagePath = basePath / "compiler"  / "run" / "data" / "image" / "woodLight.png";
        }
        else
        {
            ikisugiMusicPath = std::filesystem::path("opt") / "masorpg" / "run" / "data" / "music" / "ikisugiyou.wav";
            lethal_chinpoMusicPath = std::filesystem::path("opt") / "masorpg" / "run" / "data" / "music" / "lethalchinpo.wav";
            noJapaneseFontFontsPath = std::filesystem::path("opt") / "masorpg" / "run" / "data" / "fonts" / "8-bit-no-ja" / "8bitOperatorPlus8-Bold.ttf";
            dotGothicFontsPath = std::filesystem::path("opt") / "masorpg" / "run" / "data" / "fonts" / "ja-16-bit" / "DotGothic16-Regular.ttf";
            woodLightImagePath = std::filesystem::path("opt") / "masorpg" / "run" / "data" / "image" / "woodLight.png";
        }
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (TTF_Init() == -1) {
      std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
      SDL_Quit();
      return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer Error: " << Mix_GetError() << std::endl;
        return 1;
    }

    // 音楽ファイルの読み込み
    Mix_Music* ikisugi = Mix_LoadMUS(ikisugiMusicPath.string().c_str());
    Mix_Music* lethal_chinpo = Mix_LoadMUS(lethal_chinpoMusicPath.string().c_str());
    Mix_Music* lethal_deal = Mix_LoadMUS(lethal_dealMusicPath.string().c_str());

    Rectangle WindowSise = { 0, 0, 800, 500 };
    Rectangle titleCursor = { 3, 250, 10, 10};

    SDL_Window* window = SDL_CreateWindow(
        "SDL Window",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WindowSise.Width,
        WindowSise.Height,
        SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool title = true;
    bool roomNumberEditMusicTF = false;
    int roomNumber = 5; // 1 = 村   2 = ボス城付近   3 = ボス城   4 = ボス城最上階   5 = ボス
    int musicNumber;

    Rectangle player = {5000, 5000, 50, 50};
    SDL_Rect playerRect = { player.x, player.y, player.Width, player.Height };

    TTF_Font* noJapaneseFontTitle = TTF_OpenFont(noJapaneseFontFontsPath.string().c_str(), 50);
    TTF_Font* noJapaneseFont = TTF_OpenFont(noJapaneseFontFontsPath.string().c_str(), 24);
    TTF_Font* japaneseFont = TTF_OpenFont(dotGothicFontsPath.string().c_str(), 24);

    SDL_Surface* woodLightImage = IMG_Load(woodLightImagePath.string().c_str());

    SDL_Texture* woodLightTexture = SDL_CreateTextureFromSurface(renderer, woodLightImage);
    SDL_FreeSurface(woodLightImage);

    Camera2D camera(WindowSise.Width, WindowSise.Height, 10000, 10000);

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }

                if (title) {
                    if (isKeyPressed(event, SDLK_DOWN)) {
                        titleCursor.y += 30.0f;
                    }
                    if (isKeyPressed(event, SDLK_UP)) {
                        titleCursor.y -= 30.0f;
                    }
                    
                    if (isKeyPressed(event, SDLK_RETURN) && titleCursor.y == 250) {
                        title = false;
                    }
                }
            }
        }

        if (title)
        {
            if (musicNumber == 2)
            {}
            else
            {
                Mix_HaltMusic();
                musicNumber = 2;
                if (!Mix_PlayingMusic()) Mix_PlayMusic(lethal_chinpo, -1);
            }
        }
        else if (!title)
        {
            if (roomNumber == 5)
            {
                if (musicNumber == 3)
                {}
                else
                {
                    Mix_HaltMusic();
                    musicNumber = 3;
                    if (!Mix_PlayingMusic()) Mix_PlayMusic(lethal_deal, -1);
                }
            }
        }

        if (title) {
            SDL_SetRenderDrawColor(renderer, 0, 184, 255, 255);
            SDL_RenderClear(renderer);

            if (titleCursor.y < 250) titleCursor.y = 250;
            if (titleCursor.y > 310) titleCursor.y = 310;

            drawText(renderer, 0.0f, 0.0f, 0.0f, noJapaneseFontTitle, "MasoRPG", 10, 30.0f);
            drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFont, "スタート", 20, 250.0f);
            drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFont, "設定", 20, 280.0f);
            drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFont, "おわり", 20, 310.0f);
            drawText(renderer, 255.0f, 0.0f, 0.0f, japaneseFont, ">", titleCursor.x, titleCursor.y);

            SDL_RenderPresent(renderer);
            SDL_Delay(8);
        } else {
            camera.follow(playerRect);
            camera.setPosition(playerRect.x, playerRect.y);
            camera.clampPosition(10000, 10000);

            SDL_RenderClear(renderer);

            SDL_Rect screenRect = camera.worldToScreen(playerRect);
            SDL_RenderCopy(renderer, woodLightTexture, nullptr, &screenRect);

            drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFont, "X: ", 10.0f, 100.0f);
            drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFont, "Y: ", 10.0f, 130.0f);
            drawNumber(renderer, 0.0f, 0.0f, 0.0f, japaneseFont, playerRect.x, 40.0f, 100.0f);
            drawNumber(renderer, 0.0f, 0.0f, 0.0f, japaneseFont, playerRect.y, 40.0f, 130.0f);
            if (isKeyPressed(event, SDLK_UP)) playerRect.y -= 5;
            if (isKeyPressed(event, SDLK_DOWN)) playerRect.y += 5;
            if (isKeyPressed(event, SDLK_LEFT)) playerRect.x -= 5;
            if (isKeyPressed(event, SDLK_RIGHT)) playerRect.x += 5;

            if (playerRect.x <= -15) playerRect.x = -15;
            if (playerRect.y <= -10) playerRect.y = -10;
            if (playerRect.x >= 755) playerRect.x = 755;
            if (playerRect.y >= 450) playerRect.y = 450;

            if (roomNumber == 1)
            {}
            else if (roomNumber == 2)
            {}
            else if (roomNumber == 3)
            {}
            else if (roomNumber == 4)
            {}
            else if (roomNumber == 5)
            {}
            SDL_RenderCopy(renderer, woodLightTexture, nullptr, &screenRect);
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }
    }

    TTF_CloseFont(noJapaneseFontTitle);
    TTF_CloseFont(noJapaneseFont);
    TTF_CloseFont(japaneseFont);
    SDL_DestroyTexture(woodLightTexture);
    Mix_FreeMusic(ikisugi);
    Mix_FreeMusic(lethal_chinpo);
    Mix_FreeMusic(lethal_deal);
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
