/*
**
**そっか！全部俺のせいなんだ！！ちょこっっとミスしただけで全部俺が悪いんだ！！勝手に練習辞めて俺のせいにして俺なんか先生首になって死んじゃえばいいんだ！！そう言いたいんだ！！あぁー「敵」！！「敵」「敵」「敵」お前「敵」！！
**
*/

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
#include <fstream>
#include <chrono>
#include <memory>

struct Rectangle {
    int x;
    int y;
    int Width;
    int Height;
};

struct PlayerData {
    int x;
    int y;
    int room;
    int hp;
    int attackone;
    int attacktwo;
    int attackthree;
};

struct enemy {
    std::string name;
    int hp;
    int attackDamage;
    int enemyNumber;

    enemy(std::string n, int h, int atk, int eN) {
        name = n;
        hp = h;
        attackDamage = atk;
        enemyNumber = eN;
    }
};

enemy* enemyOne = new enemy("slime", 10, 3, 1);
enemy* enemyTwo = new enemy("two", 10, 3, 1);
enemy* enemyThree = new enemy("three", 10, 3, 1);
enemy* enemyFive = new enemy("five", 10, 3, 1);
enemy* enemyFour = new enemy("four", 10, 3, 1);

enemy* boss = new enemy("boss", 10, 3, 1);

// テキスト描画関数
void drawText(SDL_Renderer* renderer, float R, float G, float B, const std::filesystem::path& fontPath, int fontSize, const char* Text, float x, float y)
{
    TTF_Font* font = TTF_OpenFont(fontPath.string().c_str(), fontSize);
    if (!font) {
    std::cerr << "フォントの読み込みに失敗しました: " << TTF_GetError() << std::endl;
    return;
    }

    SDL_Color color = {static_cast<Uint8>(R), static_cast<Uint8>(G), static_cast<Uint8>(B)};
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, Text, color);
    if (!surface) {
    std::cerr << "テキストのレンダリングに失敗しました: " << TTF_GetError() << std::endl;
    TTF_CloseFont(font);
    return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    int w = surface->w;
    int h = surface->h;
    SDL_FreeSurface(surface);

    SDL_Rect dstRect = {static_cast<int>(x), static_cast<int>(y), w, h};
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);

    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
}

// void drawNumber(SDL_Renderer* renderer, float R, float G, float B, TTF_Font* font, int number, float x, float y)
// {
//     // 数字を文字列に変換
//     std::string str = std::to_string(number);

//     // drawText に渡す（文字列を .c_str() で const char* に変換）
//     drawText(renderer, R, G, B, font, str.c_str(), x, y);
// }

PlayerData loadGame(const std::string& filename) {
    PlayerData data{0, 0, 0, 100}; // デフォルト値
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        if (std::getline(iss, key, '=')) {
            std::string valueStr;
            if (std::getline(iss, valueStr)) {
                int value = std::stoi(valueStr);
                if (key == "x") data.x = value;
                else if (key == "y") data.y = value;
                else if (key == "room") data.room = value;
                else if (key == "hp") data.hp = value;
                else if (key == "attackone") data.attackone = value;
                else if (key == "attacktwo") data.attacktwo = value;
                else if (key == "attackthree") data.attackthree = value;
            }
        }
    }

    return data;
}

bool isKeyPressed(SDL_Event& event, SDL_Keycode key, Uint32& nextAllowedTime) {
    Uint32 now = SDL_GetTicks();  // 現在時刻（ミリ秒）

    if (now < nextAllowedTime) return false;  // 無効化中

    if (event.type == SDL_KEYDOWN &&
        event.key.keysym.sym == key &&
        event.key.repeat == 0) {
        nextAllowedTime = now + 500;  // 0.5秒間無効にする
        return true;
    }

    return false;
}

bool isKeyDown(SDL_Event& event, SDL_Keycode key) {
    return (event.type == SDL_KEYDOWN && event.key.keysym.sym == key);
}

int main(int argc, char* argv[]) {
    std::filesystem::path basePath = std::filesystem::current_path();
    
    std::filesystem::path ikisugiMusicPath; //musicNumber 1
    std::filesystem::path lethal_chinpoMusicPath; // musicNumber2
    std::filesystem::path lethal_dealMusicPath; // musicNumber3
    
    std::filesystem::path noJapaneseFontsPath;
    std::filesystem::path dotGothicFontsPath;

    std::filesystem::path woodLightImagePath;
    std::filesystem::path waruImagePath;

    std::filesystem::path oneSavePath;
    std::filesystem::path twoSavePath;
    std::filesystem::path threeSavePath;

    std::vector<std::string> args(argv + 1, argv + argc);
    for (const std::string& arg : args) {
        if (arg == "debug")
        {
            ikisugiMusicPath = basePath / "compiler" / "run" / "data" / "music" / "ikisugiyou.wav";
            lethal_chinpoMusicPath = basePath / "compiler" / "run" / "data" / "music" / "lethalchinpo.wav";
            lethal_dealMusicPath = basePath / "compiler" / "run" / "data" / "music" / "LETHAL_DEAL.wav";
            noJapaneseFontsPath = basePath / "compiler"  / "run" / "data" / "fonts" / "8-bit-no-ja" / "8bitOperatorPlus8-Bold.ttf";
            dotGothicFontsPath = basePath / "compiler"  / "run" / "data" / "fonts" / "ja-16-bit" / "DotGothic16-Regular.ttf";
            woodLightImagePath = basePath / "compiler"  / "run" / "data" / "image" / "woodLight.png";
            waruImagePath = basePath / "compiler"  / "run" / "data" / "image" / "Isee!It'sallmyfault!Imadeasmallmistakeandit'sallmyfault!.png";
            oneSavePath = basePath / "compiler" / "run" / "etc" / "save" / "one.txt";
            twoSavePath = basePath / "compiler" / "run" / "etc" / "save" / "two.txt";
            threeSavePath = basePath / "compiler" / "run" / "etc" / "save" / "three.txt";
        }
        else
        {
            ikisugiMusicPath = std::filesystem::path("opt") / "masorpg" / "run" / "data" / "music" / "ikisugiyou.wav";
            lethal_chinpoMusicPath = std::filesystem::path("opt") / "masorpg" / "run" / "data" / "music" / "lethalchinpo.wav";
            noJapaneseFontsPath = std::filesystem::path("opt") / "masorpg" / "run" / "data" / "fonts" / "8-bit-no-ja" / "8bitOperatorPlus8-Bold.ttf";
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

    int title = 5; // 1 -> タイトル,  2 -> ゲーム,  3 -> 設定,  4 -> ワールド設定   5 -> エンドロール
    // bool roomNumberEditMusicTF = false;
    int roomNumber = 1; // 1 = 村   2 = ボス城付近   3 = ボス城   4 = ボス城最上階   5 = ボス  6 -> sampleFight
    int musicNumber = 1;
    bool playStop = false;

    Rectangle player = {50, 50, 400, 250};
    SDL_Rect playerRect = { player.x, player.y, player.Width, player.Height };
    int attackOne, attackTwo, attackThree;
    Uint32 enterCooldown = 0;

    SDL_Rect rect;
    rect.x = 800 / 2 - WindowSise.Width / 2;
    rect.y = 600 / 2 - WindowSise.Height / 2;

    TTF_Font* noJapaneseFontTitle = TTF_OpenFont(noJapaneseFontsPath.string().c_str(), 50);
    TTF_Font* japaneseFontTitle = TTF_OpenFont(dotGothicFontsPath.string().c_str(), 50);
    TTF_Font* noJapaneseFont = TTF_OpenFont(noJapaneseFontsPath.string().c_str(), 24);
    TTF_Font* japaneseFont = TTF_OpenFont(dotGothicFontsPath.string().c_str(), 24);

    SDL_Surface* woodLightImage = IMG_Load(woodLightImagePath.string().c_str());
    SDL_Surface* waruImage = IMG_Load(waruImagePath.string().c_str());
    if (!waruImage)std::cout << "そっか！全部俺のせいなんだ！！ちょこっっとミスしただけで全部俺が悪いんだ！！勝手に練習辞めて俺のせいにして俺なんか先生首になって死んじゃえばいいんだ！！そう言いたいんだ！！あぁー「敵」！！「敵」「敵」「敵」お前「敵」！！" << std::endl;

    SDL_Texture* woodLightTexture = SDL_CreateTextureFromSurface(renderer, woodLightImage);
    SDL_FreeSurface(woodLightImage);

    SDL_Texture* waruiTexture = SDL_CreateTextureFromSurface(renderer, waruImage);
    SDL_FreeSurface(waruImage);

    Camera2D camera(WindowSise.Width, WindowSise.Height, 10000, 10000);

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_KEYDOWN) {
                if (title == 1) {
                    titleCursor.x = 3;
                    if (isKeyPressed(event, SDLK_DOWN, enterCooldown)) {
                        titleCursor.y += 30.0f;
                    }
                    if (isKeyPressed(event, SDLK_UP, enterCooldown)) {
                        titleCursor.y -= 30.0f;
                    }
                    
                    if (isKeyPressed(event, SDLK_RETURN, enterCooldown) && titleCursor.y == 250)
                    {
                        title = 4;
                        titleCursor.y = 100;
                        titleCursor.x = WindowSise.Width / 2 - 60;
                    }
                    else if (isKeyPressed(event, SDLK_RETURN, enterCooldown) && titleCursor.y == 280) title = 3;
                    else if (isKeyPressed(event, SDLK_RETURN, enterCooldown) && titleCursor.y == 310) running = false;
                }
                if (title == 2)
                {
                    if (isKeyPressed(event, SDLK_ESCAPE, enterCooldown))
                    {
                        if (!playStop) playStop = true;
                        else if (playStop) playStop = false;
                    }
                    if (playStop)
                    {
                        titleCursor.x = 3;
                        if (isKeyPressed(event, SDLK_DOWN, enterCooldown)) {
                            titleCursor.y += 50.0f;
                        }
                        if (isKeyPressed(event, SDLK_UP, enterCooldown)) {
                            titleCursor.y -= 50.0f;
                        }
                    }
                }
                if (title == 3)
                {
                    if (isKeyPressed(event, SDLK_ESCAPE, enterCooldown)) title = 1;
                }
                if (title == 4)
                {
                    if (isKeyPressed(event, SDLK_DOWN, enterCooldown)) {
                        titleCursor.y += 50.0f;
                    }
                    if (isKeyPressed(event, SDLK_UP, enterCooldown)) {
                        titleCursor.y -= 50.0f;
                    }
                    if (isKeyPressed(event, SDLK_ESCAPE, enterCooldown))
                    {
                        title = 1;
                        titleCursor.x = 3;
                        titleCursor.y = 250;
                    }
                    if (isKeyPressed(event, SDLK_RETURN, enterCooldown))
                    {
                        if (titleCursor.y == 100)
                        {
                            PlayerData playerSaveData = loadGame(oneSavePath);
                            playerRect.x = playerSaveData.x;
                            playerRect.y = playerSaveData.y;
                            attackOne = playerSaveData.attackone;
                            attackTwo = playerSaveData.attacktwo;
                            attackThree = playerSaveData.attackthree;
                            title = 2;
                            std::cout << playerSaveData.room << std::endl;
                            if (playerSaveData.room == 1) roomNumber = 1;
                            else if (playerSaveData.room == 2) roomNumber = 2;
                            else if (playerSaveData.room == 3) roomNumber = 3;
                            else if (playerSaveData.room == 4) roomNumber = 4;
                            else if (playerSaveData.room == 5) roomNumber = 5;
                            // playerHP = player.hp;
                        }
                        else if (titleCursor.y == 150)
                        {}
                        else if (titleCursor.y == 200)
                        {}
                    }
                }
            }
        }

        if (title == 1)
        {
            if (musicNumber == 2)
            {}
            else
            {
                Mix_HaltMusic();
                musicNumber = 2;
                std::cout << roomNumber << "\n";
                if (!Mix_PlayingMusic()) Mix_PlayMusic(lethal_chinpo, -1);
            }
        }
        else if (title == 2)
        {
            if (roomNumber == 5)
            {
                if (musicNumber == 3)
                {}
                else
                {
                    Mix_HaltMusic();
                    musicNumber = 3;
                    std::cout << roomNumber << "\n";
                    if (!Mix_PlayingMusic()) Mix_PlayMusic(lethal_deal, -1);
                }
            }
        }

        if (title == 1) {
            SDL_SetRenderDrawColor(renderer, 0, 184, 255, 255);
            SDL_RenderClear(renderer);
            SDL_Rect warauXYWH = {0, 0, 791, 421};
            SDL_RenderCopy(renderer, waruiTexture, nullptr, &warauXYWH);

            if (titleCursor.y < 250) titleCursor.y = 250;
            if (titleCursor.y > 310) titleCursor.y = 310;

            drawText(renderer, 0, 0, 0, noJapaneseFontsPath, 50, "MasoRPG", 100, 100);
            drawText(renderer, 0, 0, 0, dotGothicFontsPath, 24, "スタート", 100, 100);
            drawText(renderer, 0, 0, 0, dotGothicFontsPath, 24, "設定", 100, 100);
            drawText(renderer, 0, 0, 0, dotGothicFontsPath, 24, "おわり", 100, 100);
            drawText(renderer, 0, 0, 0, dotGothicFontsPath, 24, ">", 100, 100);

            SDL_RenderPresent(renderer);
            SDL_Delay(8);
        }
        else if (title == 2)
        {
            camera.follow(playerRect);
            camera.setPosition(playerRect.x, playerRect.y);
            camera.clampPosition(10000, 10000);
            SDL_RenderClear(renderer);

            SDL_Rect screenRect = camera.worldToScreen(playerRect);

            if (playerRect.x <= -15) playerRect.x = -15;
            if (playerRect.y <= -10) playerRect.y = -10;
            if (playerRect.x >= 755) playerRect.x = 755;
            if (playerRect.y >= 450) playerRect.y = 450;

            if (roomNumber == 1)
            {
                if (playStop)
                {
                    if (titleCursor.y < 50) titleCursor.y = 50;
                    if (titleCursor.y > 250) titleCursor.y = 250;
                    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
                    drawText(renderer, 0, 0, 0, dotGothicFontsPath, 24, "セーブ", 20, 50);
                    drawText(renderer, 0, 0, 0, dotGothicFontsPath, 24, "ステータス", 20, 100);
                    drawText(renderer, 0, 0, 0, dotGothicFontsPath, 24, "スキル", 20, 150);
                    drawText(renderer, 0, 0, 0, dotGothicFontsPath, 24, "hayasHi", 20, 200);
                    drawText(renderer, 0, 0, 0, dotGothicFontsPath, 24, "終わろう", 20, 250);
                    drawText(renderer, 0, 0, 0, dotGothicFontsPath, 24, ">", titleCursor.x, titleCursor.y);
                    // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // 白
                    // SDL_RenderFillRect(renderer, &rect);
                }
                else if (!playStop)
                {
                    if (isKeyDown(event, SDLK_UP)) playerRect.y -= 5;
                    if (isKeyDown(event, SDLK_DOWN)) playerRect.y += 5;
                    if (isKeyDown(event, SDLK_LEFT)) playerRect.x -= 5;
                    if (isKeyDown(event, SDLK_RIGHT)) playerRect.x += 5;
                    // drawText(renderer, 0, 0, 0, dotGothicFontsPath, 24, "X: ", 10, 100);
                    // drawText(renderer, 0, 0, 0, dotGothicFontsPath, 24, "Y: ", 10, 130);
                    // drawNumber(renderer, 0.0f, 0.0f, 0.0f, japaneseFont, playerRect.x, 40.0f, 100.0f);
                    // drawNumber(renderer, 0.0f, 0.0f, 0.0f, japaneseFont, playerRect.y, 40.0f, 130.0f);
                    SDL_RenderCopy(renderer, woodLightTexture, nullptr, &screenRect);
                }
            }
            else if (roomNumber == 5)
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            }
            else if (roomNumber == 6)
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                SDL_RenderClear(renderer);
                SDL_RenderPresent(renderer);
                SDL_Delay(16);
            }
            else
            {}
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }
        else if (title == 3)
        {
            SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
            SDL_RenderClear(renderer);

            if (titleCursor.y < 250) titleCursor.y = 250;
            if (titleCursor.y > 310) titleCursor.y = 310;

            // drawText(renderer, 0, 0, 0, dotGothicFontsPath, 24, "", 20, 250);
            // drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFont, "何もないよ", 20, 250.0f);
            // drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFont, "設定", 20, 280.0f);
            // drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFont, "おわり", 20, 310.0f);
            // drawText(renderer, 255.0f, 0.0f, 0.0f, japaneseFont, ">", titleCursor.x, titleCursor.y);

            SDL_RenderPresent(renderer);
            SDL_Delay(8);
        }
        else if (title == 4)
        {
            SDL_SetRenderDrawColor(renderer, 0, 184, 255, 255);
            SDL_RenderClear(renderer);

            if (titleCursor.y < 100) titleCursor.y = 100;
            if (titleCursor.y > 200) titleCursor.y = 200;

            drawText(renderer, 0, 0, 0, noJapaneseFontsPath, 50, "World", WindowSise.Width / 2 - 80, 20);

            drawText(renderer, 0, 0, 0, dotGothicFontsPath, 50, "World 1", WindowSise.Width / 2 - 40, 100);
            drawText(renderer, 0, 0, 0, dotGothicFontsPath, 50, "World 2", WindowSise.Width / 2 - 40, 150);
            drawText(renderer, 0, 0, 0, dotGothicFontsPath, 50, "World 3", WindowSise.Width / 2 - 40, 200);
            drawText(renderer, 0, 0, 0, dotGothicFontsPath, 24, ">", titleCursor.x, titleCursor.y);

            SDL_RenderPresent(renderer);
            SDL_Delay(8);
        }
        else if (title == 5)
        {
            SDL_SetRenderDrawColor(renderer, 0, 184, 255, 255);
            SDL_RenderClear(renderer);

            if (titleCursor.y < 100) titleCursor.y = 100;
            if (titleCursor.y > 200) titleCursor.y = 200;
            drawText(renderer, 0, 0, 0, noJapaneseFontsPath, 50, "MasoRPG", WindowSise.Width / 2 - 130, 30);
            drawText(renderer, 0, 0, 0, dotGothicFontsPath, 15, "カルパスコンブ", WindowSise.Width / 2, 60.0f);
            drawText(renderer, 0, 0, 0, dotGothicFontsPath, 15, "© 2025 ~ ", WindowSise.Width / 2 - 120, 60.0f);

            // drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFontTitle, "プロデューサー(githubのid)", WindowSise.Width / 2 - 40, 100.0f);
            // drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFont, "RainbowPuiPuiMolcar", WindowSise.Width / 2 - 40, 100.0f);
            // drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFont, "hamutaro1221(旧MeimaruNishimura328)", WindowSise.Width / 2 - 40, 100.0f);
            // drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFontTitle, "奴隷(githubのid)", WindowSise.Width / 2 - 40, 100.0f);
            // drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFont, "Hamster-crab", WindowSise.Width / 2 - 40, 150.0f);
            // drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFontTitle, "デザイナー(githubのid)", WindowSise.Width / 2 - 40, 200.0f);
            // drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFont, "RainbowPuiPuiMolcar", WindowSise.Width / 2 - 40, 200.0f);
            // drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFont, "hamutaro1221(旧MeimaruNishimura328)", WindowSise.Width / 2 - 40, 200.0f);
            // drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFontTitle, "音楽(githubのid)", WindowSise.Width / 2 - 40, 200.0f);
            // drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFont, "RainbowPuiPuiMolcar", WindowSise.Width / 2 - 40, 200.0f);
            // drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFont, "Hamster-crab", WindowSise.Width / 2 - 40, 200.0f);
            // drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFontTitle, "ストーリー担当(githubのid)", WindowSise.Width / 2 - 40, 200.0f);
            // drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFont, "RainbowPuiPuiMolcar", WindowSise.Width / 2 - 40, 200.0f);
            // drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFont, "hamutaro1221(旧MeimaruNishimura328)", WindowSise.Width / 2 - 40, 200.0f);
            // drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFontTitle, "テスター", WindowSise.Width / 2 - 40, 200.0f);
            // drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFont, "いない", WindowSise.Width / 2 - 40, 200.0f);
            // drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFont, "", WindowSise.Width / 2 - 40, 200.0f);

            SDL_RenderPresent(renderer);
            SDL_Delay(8);
        }
    }

    delete boss;
    delete enemyOne;
    delete enemyTwo;
    delete enemyThree;
    delete enemyFour;
    delete enemyFive;
    TTF_CloseFont(noJapaneseFontTitle);
    TTF_CloseFont(japaneseFontTitle);
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
