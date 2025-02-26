#include "MenuLevel.h"
#include "../Engine/Engine/Engine.h"
#include "Level/GameLevel.h"

#include <string>

MenuLevel::MenuLevel(std::string str)
	: str(str)
{
	menuItems.emplace_back(new MenuItem("Resume Game", []() { Engine::Get().LoadLevel(new GameLevel()); }));
	menuItems.emplace_back(new MenuItem("Quit Game", []() { Engine::Get().QuitGame(); }));
	length = (int)menuItems.size();
}

MenuLevel::~MenuLevel()
{
	for (auto* item : menuItems)
	{
		delete item;
	}
}

void MenuLevel::Update(float deltaTime)
{
	if (Engine::Get().GetKeyDown(VK_UP))
	{
		currentIndex = (currentIndex - 1 + length) % length;
	}
	if (Engine::Get().GetKeyDown(VK_DOWN))
	{
		currentIndex = (currentIndex + 1) % length;
	}

	if (Engine::Get().GetKeyDown(VK_RETURN))
	{
		menuItems[currentIndex]->onSelected();
	}
}

void MenuLevel::Draw()
{
	Super::Draw();

	int txtX = 0;
	int txtY = 0;

	Engine::Get().Draw(Vector2(txtX, txtY), str.c_str(), Color::White);

	// 메뉴 로그
	for (int i = 0; i < length; ++i)
	{
		Color textColor = (i == currentIndex) ? selectedColor : unselectedColor;
		Engine::Get().Draw(Vector2(txtX, txtY + i + 2), menuItems[i]->menuText, textColor);
	}
}
