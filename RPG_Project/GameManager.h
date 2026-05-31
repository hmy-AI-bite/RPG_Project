#pragma once
#include "Character.h"
#include <vector>
#include <memory>
#include <cstdlib>

class GameManager
{
private:
    std::vector<std::shared_ptr<Character>> characters;
    std::shared_ptr<Character> currentCharacter;

public:
    GameManager();
    ~GameManager();

    // 角色管理
    void AddCharacter(std::shared_ptr<Character> character);
    void SetCurrentCharacter(int index);
    std::shared_ptr<Character> GetCurrentCharacter() const { return currentCharacter; }

    // 显示相关
    void ClearScreen() const;
    void DisplayDashboard() const;
    void DisplayMainMenu() const;
    void DisplayCharacterList() const;

    // 游戏循环
    void Run();

private:
    void HandleAction();
    void UpdateDashboard() const;
};
