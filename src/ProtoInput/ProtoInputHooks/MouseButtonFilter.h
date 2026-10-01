#pragma once
#include <WinUser.h>
#include "MessageFilterBase.h"
#include "protoloader.h"

namespace Proto
{

class MouseButtonFilter : public MessageFilterBase<ProtoMessageFilterIDs::MouseButtonFilterID, WM_LBUTTONDOWN, WM_XBUTTONDBLCLK>
{
public:
	static constexpr unsigned int signature = 0x10000000;
	
	static bool Filter(unsigned int* message, unsigned int* lparam, unsigned int* wparam, intptr_t hwnd)
	{
		if (*message == WM_LBUTTONDOWN || *message == WM_LBUTTONUP || *message == WM_LBUTTONDBLCLK ||
			*message == WM_RBUTTONDOWN || *message == WM_RBUTTONUP || *message == WM_RBUTTONDBLCLK ||
			*message == WM_MBUTTONDOWN || *message == WM_MBUTTONUP || *message == WM_MBUTTONDBLCLK ||
			*message == WM_XBUTTONDOWN || *message == WM_XBUTTONUP || *message == WM_XBUTTONDBLCLK)
		{
			if ((*wparam & signature) != 0)
			{
				*wparam = (*wparam) & (~signature); //clean wparam
				if (InputMsgTranslator::PointerMessages)
				{ 
					if (*message == WM_LBUTTONDOWN)
					{ 
							*wparam = InputMsgTranslator::updatePointer(true, InputMsgTranslator::primary);
							if (*wparam == 0x20160001) //first button down
							{
								*message = WM_POINTERDOWN;
							}
							else 
							{
								*message = WM_POINTERUPDATE;
							}
					}
					if (*message == WM_LBUTTONUP)
					{
						*wparam = InputMsgTranslator::updatePointer(false, InputMsgTranslator::primary);
						if (*wparam == 0x00020001) //last button up
							*message = WM_POINTERUP;
						else *message = WM_POINTERUPDATE;
					}

					if (*message == WM_RBUTTONDOWN)
					{
						*wparam = InputMsgTranslator::updatePointer(true, InputMsgTranslator::secondary);

						if (*wparam == 0x20260001) //first button down
							*message = WM_POINTERDOWN;
						else *message = WM_POINTERUPDATE;
					}
					if (*message == WM_RBUTTONUP)
					{
						*wparam = InputMsgTranslator::updatePointer(false, InputMsgTranslator::secondary);

						if (*wparam == 0x00020001) //last button up
							*message = WM_POINTERUP;
						else *message = WM_POINTERUPDATE;
					}

					if (*message == WM_MBUTTONDOWN)
					{
						*wparam = InputMsgTranslator::updatePointer(true, InputMsgTranslator::third);

						if (*wparam == 0x20460001) //first button down
							*message = WM_POINTERDOWN;
						else *message = WM_POINTERUPDATE;
					}

					if (*message == WM_MBUTTONUP)
					{
						*wparam = InputMsgTranslator::updatePointer(false, InputMsgTranslator::third);

						if (*wparam == 0x00020001) //last button up
							*message = WM_POINTERUP;
						else *message = WM_POINTERUPDATE;
					}

					if (*message == WM_XBUTTONDOWN)
					{
						int fwButton = GET_XBUTTON_WPARAM(*wparam);
						if (fwButton == XBUTTON1)
						{
							*wparam = InputMsgTranslator::updatePointer(true, InputMsgTranslator::fourth);
							if (*wparam == 0x20860001) //first button down
								*message = WM_POINTERDOWN;
							else *message = WM_POINTERUPDATE;
						}
						if (fwButton == XBUTTON2)
						{
							*wparam = InputMsgTranslator::updatePointer(true, InputMsgTranslator::fifth);
							if (*wparam == 0x21060001) //first button down
								*message = WM_POINTERDOWN;
							else *message = WM_POINTERUPDATE;
						}
					}

					if (*message == WM_XBUTTONUP)
					{
						int fwButton = GET_XBUTTON_WPARAM(*wparam);
						if (fwButton == XBUTTON1)
						{
							*wparam = InputMsgTranslator::updatePointer(false, InputMsgTranslator::fourth);
						}
						if (fwButton == XBUTTON2)
						{
							*wparam = InputMsgTranslator::updatePointer(false, InputMsgTranslator::fifth);
						}
						if (*wparam == 0x00020001) //last button up
							*message = WM_POINTERUP;
						else *message = WM_POINTERUPDATE;
					}
				}

				if (InputMsgTranslator::PointerMessages || InputMsgTranslator::ScalingEnabled)
				{
					*lparam = InputMsgTranslator::ProcessedLparam(*lparam, (HWND)hwnd, InputMsgTranslator::PointerMessages, InputMsgTranslator::ScalingEnabled);
				}

			}
			else return false;
		}
		return true;
	}

	static const char* Name() { return "Mouse Buttons"; }
	static const char* Description()
	{
		return
			"Filters the mouse button press/release messages to only pass synthesized messages by Proto Input. "
			"This prevents the 'real' mouse cursor from interfering with the 'fake' synthesized input. ";
	}
};

}
