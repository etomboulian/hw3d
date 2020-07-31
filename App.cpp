#include "App.h"


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
	const float t = timer.Peek();
	std::ostringstream oss;
	oss << "Time elapsed: " << std::fixed << t << "s";
	wnd.SetTitle(oss.str());
}


