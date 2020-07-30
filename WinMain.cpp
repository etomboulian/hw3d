#include "Window.h"
#include <sstream>


int CALLBACK WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	LPSTR		lpCmdLine,
	int			nCmdShow)
{
	try {
		Window wnd(800, 600, "Test Window");
		MSG msg;
		BOOL gResult;
		int counter = 0;
		while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			
			while (!wnd.mouse.IsEmpty())
			{
				const auto e = wnd.mouse.Read();
				switch (e.GetType())
				{
				case Mouse::Event::Type::WheelUp:
					++counter;
					break;
				case Mouse::Event::Type::WheelDown:
					--counter;
					break;									
				}
				std::ostringstream oss;
				oss << ((counter > 0) ? "Up: " : "Down: ") << counter << std::endl;
				wnd.SetTitle(oss.str());

			}
		}

		if (gResult == -1)
		{
			return -1;
		}

		// wParam here is the value passed to PostQuit Message
		return msg.wParam;
	}
	catch (const ChiliException& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}


