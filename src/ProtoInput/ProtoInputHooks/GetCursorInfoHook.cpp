#include "GetCursorInfoHook.h"
#include "FakeMouseKeyboard.h"
#include "HwndSelector.h"
#include "InputMsgTranslator.h"
namespace Proto
{

	BOOL WINAPI Hook_GetCursorInfo(PCURSORINFO pci)
	{
		if (GetCursorInfo(pci))
		{
		
			const auto& state = FakeMouseKeyboard::GetMouseState();
			POINT clientPos = { state.x, state.y };

			if (InputMsgTranslator::ScalingEnabled)
				clientPos = InputMsgTranslator::getfactor(clientPos);

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
