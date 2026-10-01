#pragma once
#include "Hook.h"
#include "InstallHooks.h"

namespace Proto
{

	class InputMsgTranslator
	{
	public:
		static POINT getfactor(POINT pp);
		static LPARAM ProcessedLparam(LPARAM lParam, HWND hwnd, bool clienttoscreen, bool scale);
		static  WPARAM updatePointer(bool pressed, WPARAM button);

		static const WPARAM defaultmove;
		static const WPARAM primary;
		static const WPARAM secondary;
		static const WPARAM third;
		static const WPARAM fourth;
		static const WPARAM fifth;

		//checks
		static bool IsFirstTouch();
		//IsEnabled
		static bool ScalingEnabled;
		static bool PointerMessages;

		//Enable
		static void PointerInMouse(bool enable); //convert mouse message to pointer message
		static void Settings(int oldX, int oldY, int newX, int newY); //scale up or down messages //0.0.0.0 = disabling
	};

}
