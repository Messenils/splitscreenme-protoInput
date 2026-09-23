#include "MoveWindowHook.h"
#include <imgui.h>
#include "HwndSelector.h"

namespace Proto
{

	int MoveWindowHook::width = 0;
	int MoveWindowHook::height = 0;
	int MoveWindowHook::posx = 0;
	int MoveWindowHook::posy = 0;

	bool MoveWindowHook::MoveWindowDontResize = false;
	bool MoveWindowHook::MoveWindowDontReposition = false;

	BOOL WINAPI Hook_MoveWindow(HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL bRepaint)
	{
		int Width = MoveWindowHook::MoveWindowDontResize ? nWidth : MoveWindowHook::width;
		int Height = MoveWindowHook::MoveWindowDontResize ? nHeight : MoveWindowHook::height;
		int PosX = MoveWindowHook::MoveWindowDontReposition ? X : MoveWindowHook::posx;
		int PosY = MoveWindowHook::MoveWindowDontReposition ? Y : MoveWindowHook::posy;

		return MoveWindow(hWnd, PosX, PosY, Width, Height, bRepaint);
	}

	void MoveWindowHook::RemoveBorders()
	{	//stolen from github.com/Turnerj/Borderless1942
		LONG style = GetWindowLong((HWND)HwndSelector::GetSelectedHwnd(), GWL_STYLE);
		style &= ~(int)(WS_THICKFRAME | WS_DLGFRAME | WS_BORDER);
		SetWindowLong((HWND)HwndSelector::GetSelectedHwnd(), GWL_STYLE, style);

		style = GetWindowLong((HWND)HwndSelector::GetSelectedHwnd(), GWL_EXSTYLE);
		style &= ~(int)(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
		SetWindowLong((HWND)HwndSelector::GetSelectedHwnd(), GWL_EXSTYLE, style);
		SendMessage((HWND)HwndSelector::GetSelectedHwnd(), WM_EXITSIZEMOVE, 0, 0);
	}

	void MoveWindowHook::SetPosition()
	{	//stolen from github.com/Turnerj/Borderless1942
		int Width = MoveWindowHook::width;
		int Height = MoveWindowHook::height;
		int PosX = MoveWindowHook::posx;
		int PosY = MoveWindowHook::posy;

		int flags = SWP_NOZORDER | SWP_NOACTIVATE |
			SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_FRAMECHANGED;

		SetWindowPos((HWND)HwndSelector::GetSelectedHwnd(), HWND_TOPMOST, PosX, PosY, Width, Height, flags);

		SendMessage((HWND)HwndSelector::GetSelectedHwnd(), WM_EXITSIZEMOVE, 0, 0);
	}

	void MoveWindowHook::ShowGuiStatus()
	{
		int pos[2] = { posx, posy };
		ImGui::SliderInt2("Position", &pos[0], -5000, 5000);
		posx = pos[0];
		posy = pos[1];

		int size[2] = { width, height };
		ImGui::SliderInt2("Size", &size[0], 0, 5000);
		width = size[0];
		height = size[1];

		ImGui::Checkbox("Dont Resize", &MoveWindowHook::MoveWindowDontResize);
		ImGui::Checkbox("Dont Resposition", &MoveWindowHook::MoveWindowDontReposition);

		if (ImGui::Button("Set Position"))//these need unique IDs or text
		{
			Hook_MoveWindow((HWND)HwndSelector::GetSelectedHwnd(), 0, 0, 0, 0, TRUE);
		}

	}

	void MoveWindowHook::InstallImpl()
	{
		hookInfo = std::get<1>(InstallNamedHook(L"user32", "MoveWindow", Hook_MoveWindow));
	}

	void MoveWindowHook::UninstallImpl()
	{
		UninstallHook(&hookInfo);
	}

}
