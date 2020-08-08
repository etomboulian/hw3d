#include "App.h"
#include "Melon.h"
#include "Pyramid.h"
#include "Box.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"

// #define CHILI_DEBUG_MOUSE_POSITION
// #define CHILI_DEBUG_KEYBOARD_KEYPRESSED

App::App()
	:wnd(800, 600, "Test 3D Game")
{
	class Factory
	{
	public: 
		Factory(Graphics& gfx)
			: gfx(gfx)
		{
			// empty on purpose
		}
		std::unique_ptr<Drawable> operator()()
		{
			switch (typedist(rng))
			{
			case 0:
				return std::make_unique<Pyramid>(gfx, rng, adist, ddist, odist, rdist);
			case 1:
				return std::make_unique<Box>(gfx, rng, adist, ddist, odist, rdist, bdist);
			case 2:
				return std::make_unique<Melon>(gfx, rng, adist, ddist, odist, rdist, longdist, latdist);
			default:
				assert(false && "bad drawable type in factory");
				return {};
			}
		}

	private:
		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f, PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f, PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f, PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f, 20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f, 3.0f };
		std::uniform_int_distribution<int> latdist{ 5, 20 };
		std::uniform_int_distribution<int> longdist{ 10, 40 };
		std::uniform_int_distribution<int> typedist{ 0, 2 };


	};

	Factory f(wnd.Gfx());
	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, f);

	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

App::~App()
{}

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
	const auto dt = timer.Mark();
	wnd.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f);

	for (auto& d : drawables)
	{
		d->Update(dt);
		d->Draw(wnd.Gfx());
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


