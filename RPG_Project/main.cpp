#include "GameManager.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

int main()
{
    // 设置控制台编码为 UTF-8，让中文正常显示
    system("chcp 65001 > nul");

    // 初始化随机数种子
    srand(static_cast<unsigned int>(time(nullptr)));

    // 创建游戏管理器（游戏总控制器）
    GameManager gameManager;

    // ========== 显示标题画面 ==========
    gameManager.ShowTitleScreen();

    return 0;
}
