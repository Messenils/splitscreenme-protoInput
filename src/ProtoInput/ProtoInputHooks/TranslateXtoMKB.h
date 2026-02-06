#pragma once
#include "Hook.h"
#include "InstallHooks.h"

namespace ScreenshotInput
{

	class TranslateXtoMKB 
	{
	private:
	//	static bool usetraqnslation;

	public:
		static void Initialize(HMODULE g_hModule);
		static int controllerID;
		static bool rawinputhook;
		static bool registerrawinputhook;
	};

}