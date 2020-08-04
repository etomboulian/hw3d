#pragma once
#include "Box.h"
#include "Window.h"
#include "ChiliTimer.h"
#include <sstream>

class App
{
public:
	App();
	// master frame / message loop
	int Go();
	
private:
	void DoFrame();
private:
	Window wnd;
	ChiliTimer timer;
	std::vector<std::unique_ptr<Box>> boxes;
};

