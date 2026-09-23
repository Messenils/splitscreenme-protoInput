#include "HwndSelector.h"
#include "SetWindowPosHook.h"
#include "WindowSetup.h"

namespace Proto
{

	void WindowSetup::RemoveBorders()
	{	//stolen from github.com/Turnerj/Borderless1942
		LONG style = GetWindowLong((HWND)HwndSelector::GetSelectedHwnd(), GWL_STYLE);
		style &= ~(int)(WS_THICKFRAME | WS_DLGFRAME | WS_BORDER);
		SetWindowLong((HWND)HwndSelector::GetSelectedHwnd(), GWL_STYLE, style);

		style = GetWindowLong((HWND)HwndSelector::GetSelectedHwnd(), GWL_EXSTYLE);
		style &= ~(int)(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
		SetWindowLong((HWND)HwndSelector::GetSelectedHwnd(), GWL_EXSTYLE, style);
		SendMessage((HWND)HwndSelector::GetSelectedHwnd(), WM_EXITSIZEMOVE, 0, 0);
	}

	void WindowSetup::SetPosition()
	{	//stolen from github.com/Turnerj/Borderless1942
		int Width = SetWindowPosHook::width;
		int Height = SetWindowPosHook::height;
		int PosX = SetWindowPosHook::posx;
		int PosY = SetWindowPosHook::posy;

		int flags = SWP_NOZORDER | SWP_NOACTIVATE |
			SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_FRAMECHANGED;

		SetWindowPos((HWND)HwndSelector::GetSelectedHwnd(), HWND_NOTOPMOST, PosX, PosY, Width, Height, flags);

		SendMessage((HWND)HwndSelector::GetSelectedHwnd(), WM_EXITSIZEMOVE, 0, 0);
	}

}