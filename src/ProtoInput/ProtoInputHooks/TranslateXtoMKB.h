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
		//static bool TranslateXinputtoMKB;
		//static POINT delta;
		//static bool LMB;
		//static bool RMB;
		static HWND pointerWindows;
        static int clipcursorhook;
		static void Initialize(HMODULE g_hModule);
		static int controllerID;
		static int drawfakecursor;
		static int getkeystatehook;
	static int getasynckeystatehook;
		static int getcursorposhook;
		static int setcursorposhook;
		static int setcursorhook;
		static int rawinputhook;
		static int GetKeyboardStateHook;
		static bool registerrawinputhook;
		static int showcursorhook;

		//void PollLoop();
		//void Sendinput();
	//	bool HasGuiStatus() const override { return true; }
	//	void ShowGuiStatus() override;
	//	void InstallImpl() override;
	//	void UninstallImpl() override;
	};

}