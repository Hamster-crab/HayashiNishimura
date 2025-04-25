#include "raylib.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <thread>
#include <chrono>

namespace fs = std::filesystem;

void help()
{
    std::cout << "yajuiku  +  bootstrap     で環境構築(Linux)(開発中)\n"
    << "yajuiku  +  build     でビルド\n"
    << "yajuiku  +  run     で実行(開発中)\n"
    << "yajuiku  +  yajuiku     でyajuikuをビルド\n"
    << "yajuiku  +  builrun     でビルドと実行(開発中)\n"
    << "yajuiku  +  install     でこのコンピューターにインストール(実装予定)\n"
    << "yajuiku  +  ruun     でこのコンピューターにインストールされたMasoRPGを実行\n"
    << "yajuiku  +  remove     でこのコンピューターにインストールされたMasoRPGを削除(実装予定)\n"
    << "yajuiku  + reset     で、ビルドの設定とか削除" << std::endl;
}

void bootstrap()
{
        auto command_exists = [](const std::string& cmd) -> bool {
        return std::system((cmd + " --version > /dev/null 2>&1").c_str()) == 0;
    };

    if (command_exists("g++")) {
        std::cout << "g++ is already installed.\n";
        return;
    }

    std::cout << "g++ not found. Attempting to install...\n";

    if (command_exists("apt")) {
        std::system("sudo apt update && sudo apt install -y g++");
    } else if (command_exists("dnf")) {
        std::system("sudo dnf install -y g++");
    } else if (command_exists("yum")) {
        std::system("sudo yum install -y gcc-c++");
    } else if (command_exists("pacman")) {
        std::system("sudo pacman -Sy --noconfirm gcc");
    } else if (command_exists("zypper")) {
        std::system("sudo zypper install -y gcc-c++");
    } else {
        std::cerr << "Unsupported package manager. Please install g++ manually.\n";
    }
}

void build(std::filesystem::path compilerPath)
{
    std::filesystem::path dataPath = compilerPath / "run" / "data";
    std::filesystem::path binPath = compilerPath / "run" / "bin" / "main";

    std::string command = "g++ -std=c++17 -o " + binPath.string() +
        " src/main.cpp src/Camera2D.cpp $(pkg-config --cflags --libs sdl2 SDL2_image SDL2_ttf SDL2_mixer)";
    system(command.c_str());

    try {
        std::filesystem::copy("fonts", dataPath / "fonts",
            std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
        std::filesystem::copy("image", dataPath / "image",
            std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
        std::filesystem::copy("music", dataPath / "music",
            std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
        std::filesystem::copy("fentanyL", compilerPath / "run" / "bin" / "fentanyL",
            std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "コピーエラー: " << e.what() << std::endl;
    }
}

void run()
{
    std::cout << "※開発中" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    fs::path execPath = fs::current_path() / "compiler" / "run" / "bin" / "main";
    std::string command = execPath.string() + " debug";
    system(command.c_str());
}

void yajuiku()
{
    fs::path outputPath = fs::current_path() / "yajuiku";
    fs::path sourcePath = fs::current_path() / "compiler" / "src" / "main.cpp";

    std::string command = "g++ -std=c++17 " + sourcePath.string() + " -o " + outputPath.string();
    system(command.c_str());
}

void install(std::filesystem::path compilerPath)
{
    std::string command = "sudo cp -r " + compilerPath.string() + " /opt/masoRpgDebugData";
    system(command.c_str());

    std::string commandi = "g++ -std=c++17 -o opt/compiler/run/bin/main src/main.cpp src/Camera2D.cpp $(pkg-config --cflags --libs sdl2 SDL2_image SDL2_ttf SDL2_mixer)";
    system(commandi.c_str());

    try {
        std::filesystem::copy("fonts", "/opt/masoRpgDebugData/compiler/run/data/fonts",
            std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
        std::filesystem::copy("image", "/opt/masoRpgDebugData/compiler/run/data/image",
            std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
        std::filesystem::copy("music", "/opt/masoRpgDebugData/compiler/run/data/music",
            std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
        std::filesystem::copy("fentanyL", "/opt/masoRpgDebugData/compiler/run/data/fentanyL",
            std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);

    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "コピーエラー: " << e.what() << std::endl;
    }
}

void remove()
{
    std::string command = "sudo rm -rf /opt/masoRpgDebugData";
    system(command.c_str());
}

void ruun()
{
    system("./opt/masorpg/bin/main");
}

void reset()
{
    fs::path compilerPath = fs::current_path() / "compiler";
    fs::path runPath = compilerPath / "run";
    fs::path samplePath = compilerPath / "sample";

    try {
        // compiler/run を削除
        if (fs::exists(runPath)) {
            fs::remove_all(runPath);
        }

        // compiler/sample を compiler/run にコピー
        fs::copy(samplePath, runPath,
            fs::copy_options::recursive | fs::copy_options::overwrite_existing);
    } catch (const fs::filesystem_error& e) {
        std::cerr << "リセットエラー: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[])
{
    std::filesystem::path basePath = std::filesystem::current_path();
    std::filesystem::path compilerPath = basePath / "compiler";
    std::vector<std::string> args(argv + 1, argv + argc);
    const int screenWidth = 900;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "fentanyL");

    Rectangle buildR = { 80, 230, 190, 60 };
    bool buildcolor = false;
    bool buildtools = false;

    Rectangle runR = { 500, 230, 190, 60 };
    bool runcolor = false;
    bool runsummon = false;

    Rectangle bootstrapR = { 80, 150, 360, 60 };
    bool bootstrapcolor = false;
    bool bootstrapsummon = false;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        Vector2 mousePoint = GetMousePosition();
        if (CheckCollisionPointRec(mousePoint, buildR))
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                buildcolor = false;
                buildtools = true;
            }
            else
            {
                buildcolor = true;
            }
        }
        else buildcolor = false;

        if (CheckCollisionPointRec(mousePoint, runR))
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                runcolor = false;
                runsummon = true;
            }
            else
            {
                runcolor = true;
            }
        }
        else runcolor = false;

        if (CheckCollisionPointRec(mousePoint, bootstrapR))
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                bootstrapcolor = false;
                bootstrapsummon = true;
            }
            else
            {
                bootstrapcolor = true;
            }
        }
        else bootstrapcolor = false;
        
        if (buildtools)
        {
            build(compilerPath);
            buildtools = false;
        }

        if (runsummon)
        {
            run();
            runsummon = false;
        }

        if (bootstrapsummon)
        {
            bootstrap();
            bootstrapsummon = false;
        }
        

        BeginDrawing();

        ClearBackground(BLACK);

        DrawRectangle(buildR.x, buildR.y, buildR.width, buildR.height, GRAY);
        DrawRectangle(buildR.x + 5, buildR.y + 5, buildR.width - 10, buildR.height - 10, WHITE);
        if (buildcolor) DrawText("BUILD", buildR.x + 5, buildR.y + 5, 51, GRAY);
        else if (!buildcolor) DrawText("BUILD", buildR.x + 5, buildR.y + 5, 51, BLACK);

        DrawRectangle(runR.x, runR.y, runR.width, runR.height, GRAY);
        DrawRectangle(runR.x + 5, runR.y + 5, runR.width - 10, runR.height - 10, WHITE);
        if (runcolor) DrawText("RUN", runR.x + 5, runR.y + 5, 51, GRAY);
        else if (!runcolor) DrawText("RUN", runR.x + 5, runR.y + 5, 51, BLACK);

        DrawRectangle(bootstrapR.x, bootstrapR.y, bootstrapR.width, bootstrapR.height, GRAY);
        DrawRectangle(bootstrapR.x + 5, bootstrapR.y + 5, bootstrapR.width - 10, bootstrapR.height - 10, WHITE);
        if (bootstrapcolor) DrawText("BOOTSTRAP", bootstrapR.x + 5, bootstrapR.y + 5, 51, GRAY);
        else if (!bootstrapcolor) DrawText("BOOTSTRAP", bootstrapR.x + 5, bootstrapR.y + 5, 51, BLACK);
        
        EndDrawing();
    }

    CloseWindow();

    return 0;
}