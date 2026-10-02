#pragma once
#include <WinUser.h>
#include "MessageFilterBase.h"
#include "protoloader.h"
#include "RawInput.h"
#include "InputMsgTranslator.h"

namespace Proto
{

class MouseWheelFilter : public MessageFilterBase<ProtoMessageFilterIDs::MouseWheelFilterID, WM_MOUSEWHEEL>
{
public:
	static constexpr unsigned int protoInputSignature = 0x100;

	
	static bool Filter(unsigned int* message, unsigned int* lparam, unsigned int* wparam, intptr_t hwnd)
	{
		if ((*wparam & protoInputSignature) != 0)
		{
			*wparam = (*wparam) & (~protoInputSignature);
			if (InputMsgTranslator::PointerMessages)
			{ 
				*message = WM_POINTERWHEEL;
				int zDelta = GET_WHEEL_DELTA_WPARAM(*wparam);

				if (zDelta == -120)
					*wparam = 0xFF880001;
				else *wparam = 0x00780001;
			}
			if (InputMsgTranslator::ScalingEnabled || InputMsgTranslator::PointerMessages)
			{
				*lparam = InputMsgTranslator::ProcessedLparam(*lparam, (HWND)hwnd, InputMsgTranslator::PointerMessages && !RawInput::rawInputState.sendMouseDblClkMessages, InputMsgTranslator::ScalingEnabled);
			}
			return true;
		}
		
		return false;
	}

	static const char* Name() { return "Mouse Wheel"; }
	static const char* Description()
	{
		return
			"Filters WM_MOUSEWHEEL messages to only pass those that are synthesized by Proto Input. "
			"This avoids the scroll wheel being sent twice. ";
	}
};

}
