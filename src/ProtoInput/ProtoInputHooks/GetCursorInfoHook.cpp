#include "GetCursorInfoHook.h"
#include "FakeMouseKeyboard.h"
#include "HwndSelector.h"

namespace Proto
{

	BOOL WINAPI Hook_GetCursorInfo(PCURSORINFO pci)
	{
		if (pci)
		{
			const auto& state = FakeMouseKeyboard::GetMouseState();
			POINT clientPos = pci->ptScreenPos;
			ScreenToClient((HWND)HwndSelector::GetSelectedHwnd(), &clientPos);
			clientPos.x = state.x;
			clientPos.y = state.y;

			clientPos.x = state.x;
			clientPos.y = state.y;
			
			
			if (FakeMouseKeyboard::PutMouseInsideWindow)
			{
				int clientWidth = HwndSelector::windowWidth;
				int clientHeight = HwndSelector::windowHeight;
				if (!FakeMouseKeyboard::DefaultTopLeftMouseBounds)
				{
					if (clientPos.y < 1)
						clientPos.y = 0;  // Top edge
					if (clientPos.x < 1)
						clientPos.x = 0;  // Left edge
				}
				if (!FakeMouseKeyboard::DefaultBottomRightMouseBounds)
				{
					if (clientPos.y > clientHeight - 1)
						clientPos.y = clientHeight - 1;  // Bottom edge
					if (clientPos.x > clientWidth - 1)
						clientPos.x = clientWidth - 1;  // Right edge
				}
			}
			ClientToScreen((HWND)HwndSelector::GetSelectedHwnd(), &clientPos);
			pci->ptScreenPos.x = clientPos.x;
			pci->ptScreenPos.y = clientPos.y;
		}

		return true;
	}

	void GetCursorInfoHook::ShowGuiStatus()
	{

	}

	void GetCursorInfoHook::InstallImpl()
	{
		hookInfo = std::get<1>(InstallNamedHook(L"user32", "GetCursorInfo", Hook_GetCursorInfo));
	}

	void GetCursorInfoHook::UninstallImpl()
	{
		UninstallHook(&hookInfo);
	}

}
