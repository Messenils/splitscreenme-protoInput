#include "SetCursorPosHook.h"
#include <imgui.h>
#include "HwndSelector.h"
#include "FakeMouseKeyboard.h"
#include "FakeCursor.h"
#include "XinputHook.h"
#include "WindowMsgHook.h"

namespace Proto
{

bool SetCursorPosHook::blockSettingCursorPos = false;
bool SetCursorPosHook::MessageCursorPosSet = false; //also send mousemove with new coordinates after hook call
POINT SetCursorPosHook::mousesethere;
BOOL WINAPI Hook_SetCursorPos(int X, int Y)
{

	if (!SetCursorPosHook::blockSettingCursorPos)
	{
		POINT p;
		p.x = X;
		p.y = Y;

		//SetCursorPos require screen coordinates (relative to 0,0 of monitor)
		ScreenToClient((HWND)HwndSelector::GetSelectedHwnd(), &p);

		if (XinputHook::TranslateMKBtoXinput)
		{
			SetCursorPosHook::mousesethere.x = p.x;
			SetCursorPosHook::mousesethere.y = p.y;
		}
		else
			FakeMouseKeyboard::SetMousePos(p.x, p.y, SetCursorPosHook::MessageCursorPosSet);

		FakeCursor::NotifyUpdatedCursorPosition();
	}
	return TRUE;
}

void SetCursorPosHook::ShowGuiStatus()
{
	ImGui::TextWrapped("Enable this to prevent Set Cursor calls from setting the 'fake' cursor (i.e. the calls do nothing)");
	ImGui::Checkbox("Block calls", &blockSettingCursorPos);
	ImGui::TextWrapped("Enable this to send cursor pos message to game with new coordinates each call. Can fix mouse always moving problem)");
	ImGui::Checkbox("Send Mousemove on call", &SetCursorPosHook::MessageCursorPosSet);
}

void SetCursorPosHook::InstallImpl()
{
	hookInfo = std::get<1>(InstallNamedHook(L"user32", "SetCursorPos", Hook_SetCursorPos));
}

void SetCursorPosHook::UninstallImpl()
{
	UninstallHook(&hookInfo);
}

}
