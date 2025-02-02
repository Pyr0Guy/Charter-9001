#pragma once

class App
{
public:
	App(unsigned int Width, unsigned int Height, const std::string& title);

	void Update();
	void Draw();

	void Destroy();

	bool IsClosed();
};