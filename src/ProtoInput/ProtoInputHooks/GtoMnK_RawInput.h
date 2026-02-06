#pragma once
#include <vector>
#include <Windows.h>

namespace ScreenshotInput {
    class RawInput {
	    public:
        // To start the RawInput method.
        static void Initialize();

        static void Shutdown();

        static void InjectFakeRawInput(const RAWINPUT& fakeInput);
        static void TranslateKeyboardAction(int actionCode, int& outVkCode, bool& outIsExtended);
        static void GenerateRawMouseButton(int actionCode, bool press);
        static void GenerateRawKey(int vkCode, bool press, bool isExtended);
        static void SendActionDelta(int deltaX, int deltaY);
        static const int RAWINPUT_BUFFER_SIZE = 20;
        static  RAWINPUT g_inputBuffer[RAWINPUT_BUFFER_SIZE];

        static std::vector<HWND> g_forwardingWindows;
        static HWND g_rawInputHwnd;
        static bool createdWindowIsOwned;
    };
}