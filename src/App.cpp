#include "App.h"
#include <string>
#include <iostream>


App::App()
	:wnd(800, 600, "Test 3D Game")
{
	// empty on purpose
}

int App::Go()
{
	
	while (true)
	{
		if (const auto ecode = Window::ProcessMessages())
		{
			return *ecode;
		}
		DoFrame();
	}

}

void App::DoFrame()
{
	const float c = sin(timer.Peek()) / 2.0f +0.5f;
	wnd.Gfx().ClearBuffer(c, c, 1.0f);
	wnd.Gfx().DrawTestTriangle(
		timer.Peek(),
		wnd.mouse.GetPosX() / 400.0f - 1.0f,
		-wnd.mouse.GetPosY() / 300.0f + 1.0f
	);
	
#ifdef CHILI_DEBUG_MOUSE_POSITION
	std::ostringstream oss;
	oss << "Mouse at: (" << wnd.mouse.GetPosX() << "," << wnd.mouse.GetPosY() << ")" << std::endl;
	OutputDebugString(oss.str().c_str());
#endif

#ifdef CHILI_DEBUG_KEYBOARD_KEYPRESSED
	if (!wnd.kbd.CharIsEmpty())
	{
		std::ostringstream oss;
		oss << "Key: " << wnd.kbd.ReadChar() << " was pressed." << std::endl;
		OutputDebugString(oss.str().c_str());
	}
#endif

	wnd.Gfx().EndFrame();

}


