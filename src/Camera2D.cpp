#include "Camera2D.h"

Camera2D::Camera2D(int width, int height) 
    : screenWidth(width), screenHeight(height), x(0), y(0), mapWidth(0), mapHeight(0), viewX(0), viewY(0) {}

Camera2D::Camera2D(int screenWidth, int screenHeight, int mapWidth, int mapHeight) 
    : screenWidth(screenWidth), screenHeight(screenHeight), mapWidth(mapWidth), mapHeight(mapHeight), x(0), y(0), viewX(0), viewY(0) {}

void Camera2D::follow(const SDL_Rect& target) {
    // プレイヤーを画面の中央に持ってくる
    x = target.x + target.w / 2 - screenWidth / 2;
    y = target.y + target.h / 2 - screenHeight / 2;
}

SDL_Rect Camera2D::worldToScreen(const SDL_Rect& worldRect) const {
    // ワールド座標から画面座標への変換
    SDL_Rect screenRect;
    screenRect.x = worldRect.x - x;
    screenRect.y = worldRect.y - y;
    screenRect.w = worldRect.w;
    screenRect.h = worldRect.h;
    return screenRect;
}

void Camera2D::setPosition(int x, int y) {
    this->x = x;
    this->y = y;
}

void Camera2D::clampPosition(int mapWidth, int mapHeight) {
    // カメラがマップ外に出ないように制限
    if (x < 400) x = 400;
    if (y < 250) y = 250;
    if (x + screenWidth > mapWidth) x = mapWidth - screenWidth;
    if (y + screenHeight > mapHeight) y = mapHeight - screenHeight;
}

SDL_Rect Camera2D::getView() const {
    // カメラのビューポート（画面上の領域）を取得
    SDL_Rect view = { x, y, screenWidth, screenHeight };
    return view;
}
