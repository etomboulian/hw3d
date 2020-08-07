#include "App.h"
#include "Box.h"
#include <memory>

// #define CHILI_DEBUG_MOUSE_POSITION
// #define CHILI_DEBUG_KEYBOARD_KEYPRESSED

App::App()
	:wnd(800, 600, "Test 3D Game")
{
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> adist(0.0f, 3.1215f / 2.0f);
	std::uniform_real_distribution<float> ddist(0.0f, 3.1415f / 2.0f);
	std::uniform_real_distribution<float> odist(0.0f, 3.1415f / 2.0f);
	std::uniform_real_distribution<float> rdist(6.0f, 20.0f);
	
	for (auto i = 0; i < 80; ++i)
	{
		boxes.push_back(
			std::make_unique<Box>(
			wnd.Gfx(), rng, adist, ddist, odist, rdist
			)
		);
	}

	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
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
	auto dt = timer.Mark();
	wnd.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f);

	for (auto& b : boxes)
	{
		b->Update(dt);
		b->Draw(wnd.Gfx());
	}
	
// Start Input debug options (mouse and keyboard)
#ifdef CHILI_DEBUG_MOUSE_POSITION
	std::ostringstream oss;
	oss << "Mouse at: (" << wnd.mouse.GetPosX() << "," << wnd.mouse.GetPosY() << ")" << std::endl;
	OutputDebugString(oss.str().c_str());
#endif

#ifdef CHILI_DEBUG_KEYBOARD_KEYPRESSED
	if (!wnd.kbd.CharIsEmpty()) // if key is pressed
	{
		std::ostringstream oss;
		oss << "Key: " << wnd.kbd.ReadChar() << " was pressed." << std::endl;
		OutputDebugString(oss.str().c_str());
	}
#endif
// End Input debug options (mouse and keyboard)

	wnd.Gfx().EndFrame();

}


