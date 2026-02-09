//#include "pch.h"
#include "GtoMnK_RawInputHooks.h"
#include "GtoMnK_RawInput.h"
#include "KeyboardButtonFilter.h"
//#include "Mouse.h"
//#include "Keyboard.h"
#include "EasyHook.h"
#include "gui.h"
// Thanks to ProtoInput.

namespace ScreenshotInput
{
   // HOOK_TRACE_INFO g_getRawInputDataHook = { NULL };
   // HOOK_TRACE_INFO g_registerRawInputDevicesHook = { NULL };


    UINT WINAPI RawInputHooks::GetRawInputDataHookX(HRAWINPUT hRawInput, UINT uiCommand, LPVOID pData, PUINT pcbSize, UINT cbSizeHeader) {
        UINT handleValue = (UINT)(UINT_PTR)hRawInput;
        if ((handleValue & 0xFF000000) == 0xAB000000) {

            UINT bufferIndex = handleValue & 0x00FFFFFF;
            if (bufferIndex >= ScreenshotInput::RawInput::RAWINPUT_BUFFER_SIZE) {
                return GetRawInputData(hRawInput, uiCommand, pData, pcbSize, cbSizeHeader);
            }

            if (pData == NULL) {
                *pcbSize = sizeof(RAWINPUT);
                return 0;
            }

            RAWINPUT* storedData = &ScreenshotInput::RawInput::g_inputBuffer[bufferIndex];
            memcpy(pData, storedData, sizeof(RAWINPUT));
            return sizeof(RAWINPUT);

        }
        else {
            return GetRawInputData(hRawInput, uiCommand, pData, pcbSize, cbSizeHeader);
        }
    }

    BOOL WINAPI RawInputHooks::RegisterRawInputDevicesHookX(PCRAWINPUTDEVICE pRawInputDevices, UINT uiNumDevices, UINT cbSize) {
        for (UINT i = 0; i < uiNumDevices; ++i) {
            HWND targetHwnd = pRawInputDevices[i].hwndTarget;

            if (targetHwnd != NULL && targetHwnd != ScreenshotInput::RawInput::g_rawInputHwnd) {
                auto& windows = ScreenshotInput::RawInput::g_forwardingWindows;
                if (std::find(windows.begin(), windows.end(), targetHwnd) == windows.end()) {
                    //LOG("Captured new game window HWND: 0x%p", targetHwnd);
                    windows.push_back(targetHwnd);
                }
            }
        }
        return TRUE;
    }
  //  void RawInputHooks::InstallHooks() {
   //     // Install GetRawInputData hook
  //      HMODULE hUser32 = GetModuleHandleA("user32");
   //     LhInstallHook(GetProcAddress(hUser32, "GetRawInputData"), GetRawInputDataHook, NULL, &g_getRawInputDataHook);
   //     LhInstallHook(GetProcAddress(hUser32, "RegisterRawInputDevices"), RegisterRawInputDevicesHook, NULL, &g_registerRawInputDevicesHook);
   //     ULONG ACLEntries[1] = { 0 };
   //     LhSetExclusiveACL(ACLEntries, 1, &g_getRawInputDataHook);
  //      LhSetExclusiveACL(ACLEntries, 1, &g_registerRawInputDevicesHook);
  //  }
}