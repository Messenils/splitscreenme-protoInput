#pragma once
#include <windows.h>

namespace ScreenshotInput {
	class RawInputHooks {
	public:
		//UINT WINAPI GetRawInputDataHook(HRAWINPUT hRawInput, UINT uiCommand, LPVOID pData, PUINT pcbSize, UINT cbSizeHeader);
		//BOOL WINAPI RegisterRawInputDevicesHook(PCRAWINPUTDEVICE pRawInputDevices, UINT uiNumDevices, UINT cbSize);
		static void InstallHooks();
	};
}
