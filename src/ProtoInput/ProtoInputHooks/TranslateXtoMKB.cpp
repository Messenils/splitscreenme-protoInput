//#include "XinputHook.h"
//#include <Xinput.h>
#include <OpenXinput.h>
#include "Gui.h"
#include <string>
#include <map>
//#include "OpenXinputWrapper.h"
#include "TranslateXtoMKB.h"
#include "RawInput.h"
#include <imgui.h>
#include <cmath>
#define NOMINMAX
#include <windows.h>
#include <algorithm>
#include "EasyHook.h"
#include <tlhelp32.h>
#include <tchar.h>
#include <iostream>
#include <vector>
#include <cstdio>  // for swprintf
#include <psapi.h>
#include <cstdlib> // For strtoul
#include <dwmapi.h>
#include <dinput.h>
#include <thread>
#include "HwndSelector.h"
#include "FakeCursor.h"
#include "GtoMnK_RawInput.h"
#include "FakeMouseKeyboard.h"
//#include "RawInput.h"
//#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#include <unordered_map>


//#include "dllmain.h"
//#include "PostKeyFunction.cpp"

#pragma comment(lib, "Xinput.lib")

namespace ScreenshotInput
{
    //from tunnell
    int TranslateXtoMKB::controllerID;
    bool TranslateXtoMKB::rawinputhook; //registerrawinputhook
    bool TranslateXtoMKB::registerrawinputhook; //registerrawinputhook

    int TranslateXtoMKB::Amapping;
    int TranslateXtoMKB::Bmapping;
    int TranslateXtoMKB::Xmapping;
    int TranslateXtoMKB::Ymapping;
    int TranslateXtoMKB::RSmapping;
    int TranslateXtoMKB::LSmapping;
    int TranslateXtoMKB::rightmapping;
    int TranslateXtoMKB::leftmapping;
    int TranslateXtoMKB::upmapping;
    int TranslateXtoMKB::downmapping;
    int TranslateXtoMKB::stickRpressmapping;
    int TranslateXtoMKB::stickLpressmapping;
    int TranslateXtoMKB::stickrightmapping;
    int TranslateXtoMKB::stickleftmapping;
    int TranslateXtoMKB::stickupmapping;
    int TranslateXtoMKB::stickdownmapping;
    int TranslateXtoMKB::optionmapping;
    int TranslateXtoMKB::startmapping;
    bool TranslateXtoMKB::lefthanded;

    int updatewindowtick = 300;
    int mode = 1;
    int AxisLeftsens;
    int AxisRightsens;
    int AxisUpsens;
    int AxisDownsens;
    int scrollspeed3;
    float radial_deadzone = 0.10f; // Circular/Radial Deadzone (0.0 to 0.3)
    float axial_deadzone = 0.00f; // Square/Axial Deadzone (0.0 to 0.3)
    const float max_threshold = 0.03f; // Max Input Threshold, an "outer deadzone" (0.0 to 0.15)
    const float curve_slope = 0.16f; // The linear portion of the response curve (0.0 to 1.0)
    const float curve_exponent = 5.00f; // The exponential portion of the curve (1.0 to 10.0)
    float sensitivity = 12.00f; // Base sensitivity / max speed (1.0 to 30.0)
    float accel_multiplier = 1.90f; // Look Acceleration Multiplier (1.0 to 3.0)

    /////////////////
    bool movedmouse;
	bool leftPressedold = false;
	bool rightPressedold = false;
    HMODULE g_hModule = nullptr;


    CRITICAL_SECTION critical; //window thread

    bool loop = true;
    int showmessage = 0; //0 = no message, 1 = initializing, 2 = bmp mode, 3 = bmp and cursor mode, 4 = edit mode   
    int counter = 0;


    //syncronization control
    HANDLE hMutex;

    POINT delta;
    //hooks


    //fake cursor

    int Xf = 0;
    int Yf = 0;


    int tick = 0;

    bool rawmouseWu = false;
    bool rawmouseWd = false;
	bool oldA = false;
	bool oldB = false;
	bool oldX = false;
	bool oldY = false;

    bool oldC = false;
    bool oldD = false;
    bool oldE = false;
    bool oldF = false;

    bool olddown = false;
    bool oldup = false;
    bool oldleft = false;
    bool oldright = false;

    // Add a fake key event to the array


    bool SendMouseClick(int x, int y, int z) {
        // Create a named mutex
        RAWMOUSE muusjn = { 0 };
        POINT heer;
        muusjn.usButtonFlags = 0;

        muusjn.lLastX = x;
        muusjn.lLastY = y;

        if (z == 3) {
            muusjn.usButtonFlags |= RI_MOUSE_LEFT_BUTTON_DOWN;
            Proto::FakeMouseKeyboard::ReceivedKeyPressOrRelease(VK_LBUTTON, true);
            RawInput::GenerateRawMouseButton(-1, true);
        }
        if (z == 4)
        {
            muusjn.usButtonFlags |= RI_MOUSE_LEFT_BUTTON_UP;
            Proto::FakeMouseKeyboard::ReceivedKeyPressOrRelease(VK_LBUTTON, false);
            RawInput::GenerateRawMouseButton(-1, false);
        }
        if (z == 5) {
            muusjn.usButtonFlags |= RI_MOUSE_RIGHT_BUTTON_DOWN;
            Proto::FakeMouseKeyboard::ReceivedKeyPressOrRelease(VK_RBUTTON, true);
            RawInput::GenerateRawMouseButton(-2, true);
        }
        if (z == 6)
        {
            muusjn.usButtonFlags |= RI_MOUSE_RIGHT_BUTTON_UP;
            Proto::FakeMouseKeyboard::ReceivedKeyPressOrRelease(VK_RBUTTON, false);
            RawInput::GenerateRawMouseButton(-2, false);

        }
        if (z == 20 || z == 21) //WM_mousewheel need desktop coordinates
        {
        }
        else if (z == 8 || z == 10 || z == 11) //only mousemove
        {
            RawInput::SendActionDelta(x, y);
            Proto::FakeMouseKeyboard::AddMouseDelta(x, y);

        }
        Proto::RawInput::SendInputMessages(muusjn);
        return true;
    }

        void ButtonStateImpulse(int vk, bool state)
        {
            RAWKEYBOARD data{};
            data.MakeCode = MapVirtualKey(vk, MAPVK_VK_TO_VSC);
            data.VKey = vk;
            data.ExtraInformation = 0;

            data.Flags = state ? 0 : RI_KEY_BREAK;
            data.Message = state ? WM_KEYDOWN : WM_KEYUP;

            // Extended key?
            switch (vk)
            {
            case VK_LEFT:
            case VK_RIGHT:
            case VK_UP:
            case VK_DOWN:
            case VK_INSERT:
            case VK_DELETE:
            case VK_HOME:
            case VK_END:
            case VK_PRIOR:
            case VK_NEXT:
            case VK_RCONTROL:
            case VK_RMENU:
            case VK_DIVIDE:
            case VK_NUMLOCK:
                data.Flags |= RI_KEY_E0;
                break;
            }
        Proto::FakeMouseKeyboard::ReceivedKeyPressOrRelease(vk, state);
        Proto::RawInput::SendKeyMessage(data, state);
        RawInput::GenerateRawKey(vk, state, false);

    }
    std::string UGetExecutableFolder() {
        char path[MAX_PATH];
        GetModuleFileNameA(NULL, path, MAX_PATH);

        std::string exePath(path);

        size_t lastSlash = exePath.find_last_of("\\/");
        return exePath.substr(0, lastSlash);
    }


    std::wstring WGetExecutableFolder() {
        wchar_t path[MAX_PATH];
        GetModuleFileNameW(NULL, path, MAX_PATH);
        std::wstring exePath(path);
        size_t lastSlash = exePath.find_last_of(L"\\/");

        if (lastSlash == std::wstring::npos)
            return L"";
        return exePath.substr(0, lastSlash);
    }


    bool IsTriggerPressed(BYTE triggerValue) {
        BYTE threshold = 175;
        return triggerValue > threshold;
    }


    // Helper: Get stick magnitude
    float GetStickMagnitude(SHORT x, SHORT y) {
        return sqrtf(static_cast<float>(x) * x + static_cast<float>(y) * y);
    }

    // Helper: Clamp value to range [-1, 1]
    float Clamp(float v) {
        if (v < -1.0f) return -1.0f;
        if (v > 1.0f) return 1.0f;
        return v;
    }
    // #define DEADZONE 8000
    // #define MAX_SPEED 30.0f        // Maximum pixels per poll
    // #define ACCELERATION 2.0f      // Controls non-linear ramp (higher = more acceleration)


    POINT CalculateUltimateCursorMove(
        SHORT stickX, SHORT stickY,
        float c_deadzone,
        float s_deadzone,
        float max_threshold,
        float curve_slope,
        float curve_exponent,
        float sensitivity,
        float accel_multiplier
    ) {
        static double mouseDeltaAccumulatorX = 0.0;
        static double mouseDeltaAccumulatorY = 0.0;

        double normX = static_cast<double>(stickX) / 32767.0;
        double normY = static_cast<double>(stickY) / 32767.0;

        double magnitude = std::sqrt(normX * normX + normY * normY);
        if (magnitude < c_deadzone) {
            return { 0, 0 }; // Inside circular deadzone
        }
        if (std::abs(normX) < s_deadzone) {
            normX = 0.0; // Inside axial deadzone for X
        }
        if (std::abs(normY) < s_deadzone) {
            normY = 0.0; // Inside axial deadzone for Y
        }
        magnitude = std::sqrt(normX * normX + normY * normY);
        if (magnitude < 1e-6) {
            return { 0, 0 };
        }

        double effectiveRange = 1.0 - max_threshold - c_deadzone;
        if (effectiveRange < 1e-6) effectiveRange = 1.0;

        double remappedMagnitude = (magnitude - c_deadzone) / effectiveRange;
        remappedMagnitude = (std::max)(0.0, (std::min)(1.0, remappedMagnitude));

        double curvedMagnitude = curve_slope * remappedMagnitude + (1.0 - curve_slope) * std::pow(remappedMagnitude, curve_exponent);

        double finalSpeed = sensitivity * accel_multiplier;

        double dirX = normX / magnitude;
        double dirY = normY / magnitude;
        double finalMouseDeltaX = dirX * curvedMagnitude * finalSpeed;
        double finalMouseDeltaY = dirY * curvedMagnitude * finalSpeed;

        mouseDeltaAccumulatorX += finalMouseDeltaX;
        mouseDeltaAccumulatorY += finalMouseDeltaY;
        LONG integerDeltaX = static_cast<LONG>(mouseDeltaAccumulatorX);
        LONG integerDeltaY = static_cast<LONG>(mouseDeltaAccumulatorY);

        mouseDeltaAccumulatorX -= integerDeltaX;
        mouseDeltaAccumulatorY -= integerDeltaY;

        return { integerDeltaX, -integerDeltaY };
    }

   
    std::wstring ToWString(const std::string& s)
    {
        return std::wstring(s.begin(), s.end());
    }
    bool readsettings() {
        char buffer[16]; //or only 4 maybe

        // settings reporting
        std::string iniPath = UGetExecutableFolder() + "\\Xinput.ini";
        std::string iniSettings = "Settings";

        return true;
    }
    void ThreadFunction(HMODULE hModule)
    {
        Proto::AddThreadToACL(GetCurrentThreadId());

        Sleep(2000);
        if (readsettings() == false)
        {
            //messagebox? settings not read
        }
        while (loop == true)
        {
            movedmouse = false; //reset
            XINPUT_STATE state;
            ZeroMemory(&state, sizeof(XINPUT_STATE));
            // Check controller 0
            DWORD dwResult = OpenXInputGetState(TranslateXtoMKB::controllerID, &state);
            if (dwResult == ERROR_SUCCESS)
            {
                WORD buttons = state.Gamepad.wButtons;

                if (mode > 0)
                {
                        //fake cursor poll
                    int Xaxis = 0;
                    int Yaxis = 0;
                    int scrollXaxis = 0;
                    int scrollYaxis = 0;
                    int Yscroll = 0;
                    int Xscroll = 0;
                    bool leftPressed = IsTriggerPressed(state.Gamepad.bLeftTrigger);
                    bool rightPressed = IsTriggerPressed(state.Gamepad.bRightTrigger);

                    if (TranslateXtoMKB::lefthanded == 1) {
                        Xaxis = state.Gamepad.sThumbRX;
                        Yaxis = state.Gamepad.sThumbRY;
                        scrollXaxis = state.Gamepad.sThumbLX;
                        scrollYaxis = state.Gamepad.sThumbLY;
                    }
                    else
                    {
                        Xaxis = state.Gamepad.sThumbLX;
                        Yaxis = state.Gamepad.sThumbLY;
                        scrollXaxis = state.Gamepad.sThumbRX;
                        scrollYaxis = state.Gamepad.sThumbRY;
                    }


                    delta = CalculateUltimateCursorMove(
                        Xaxis, Yaxis,
                        radial_deadzone, axial_deadzone, max_threshold,
                        curve_slope, curve_exponent,
                        sensitivity, accel_multiplier
                    );
                    if (delta.x != 0 || delta.y != 0) {
                        Xf += delta.x;
                        Yf += delta.y;
                        movedmouse = true;
                    }

                    if (movedmouse == true) //fake cursor move message
                    {
                        SendMouseClick(delta.x, delta.y, 8);
                            
                        Proto::FakeCursor::NotifyUpdatedCursorPosition();
                    }

                    if (leftPressedold)
                    {
                        if (!leftPressed)
                        {

                            SendMouseClick(Xf, Yf, 6); //double click
                            leftPressedold = false;
                        }
                    }
                    else if (leftPressed)
                    {
                        if (leftPressedold == false)
                        {
                             SendMouseClick(Xf, Yf, 5); //4 skal vere 3
                             leftPressedold = true;
                        }
                    }

                    if (rightPressedold)
                    {
                        if (!rightPressed)
                        {
                            SendMouseClick(Xf, Yf, 4);
                            rightPressedold = false;
                        }
                    } //if rightpress
                    else if (rightPressed)
                    {
                        if (rightPressedold == false)
                        {
                           SendMouseClick(Xf, Yf, 3);
                           rightPressedold = true;

                        }
                    }

                    //buttons
                    if (oldA)
                    {
                       if (buttons & XINPUT_GAMEPAD_A)
                       {
                       }
                       else{
                           oldA = false;
                           ButtonStateImpulse(TranslateXtoMKB::Amapping, false);//release
                       }
                    }
                    else if (buttons & XINPUT_GAMEPAD_A)
                    {
                        oldA = true;
                        ButtonStateImpulse(TranslateXtoMKB::Amapping, true);//down
                    }


                    if (oldB)
                    {
                        if (buttons & XINPUT_GAMEPAD_B)
                        {
                        }
                        else {
                            oldB = false;
                            ButtonStateImpulse(TranslateXtoMKB::Bmapping, false);//release
                        }
                    }
                    else if (buttons & XINPUT_GAMEPAD_B)
                    {
                        oldB = true;
                        ButtonStateImpulse(TranslateXtoMKB::Bmapping, true);//down
                    }


                    if (oldX)
                    {
                        if (buttons & XINPUT_GAMEPAD_X)
                        {
                        }
                        else {
                            oldX = false;
                            ButtonStateImpulse(TranslateXtoMKB::Xmapping, false);//release
                        }
                    }
                    else if (buttons & XINPUT_GAMEPAD_X)
                    {
                        oldX = true;
                        ButtonStateImpulse(TranslateXtoMKB::Xmapping, true);//down
                    }


                    if (oldY)
                    {
                        if (buttons & XINPUT_GAMEPAD_Y)
                        {
                        }
                        else {
                            oldY = false;
                            ButtonStateImpulse(TranslateXtoMKB::Ymapping, false);//release
                        }
                    }
                    else if (buttons & XINPUT_GAMEPAD_Y)
                    {
                        oldY = true;
                        ButtonStateImpulse(TranslateXtoMKB::Ymapping, true);//down
                    }


                    if (oldC)
                    {
                        if (buttons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
                        {
                        }
                        else {
                            oldC = false;
                            ButtonStateImpulse(TranslateXtoMKB::RSmapping, false); //release
                        }
                    }
                    else if (buttons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
                    {
                        oldC = true;
                        ButtonStateImpulse(TranslateXtoMKB::RSmapping, true); //down
                    }


                    if (oldD)
                    {
                        if (buttons & XINPUT_GAMEPAD_LEFT_SHOULDER)
                        {
                        }
                        else {
                            oldD = false;
                            ButtonStateImpulse(TranslateXtoMKB::LSmapping, false);//release
                        }
                    }
                    else if (buttons & XINPUT_GAMEPAD_LEFT_SHOULDER)
                    {
                        oldD = true;
                        ButtonStateImpulse(TranslateXtoMKB::LSmapping, true);//down
                    }


                    if (oldleft)
                    {
                        if (buttons & XINPUT_GAMEPAD_DPAD_LEFT)
                        {
                        }
                        else {
                            oldleft = false;
                            ButtonStateImpulse(TranslateXtoMKB::leftmapping, false); //release
                        }
                    }
                    else if (buttons & XINPUT_GAMEPAD_DPAD_LEFT)
                    {
                        oldleft = true;
                        ButtonStateImpulse(TranslateXtoMKB::leftmapping, true);//down
                    }


                    if (oldright)
                    {
                        if (buttons & XINPUT_GAMEPAD_DPAD_RIGHT)
                        {
                        }
                        else {
                            oldright = false;
                            ButtonStateImpulse(TranslateXtoMKB::rightmapping, false);//release
                        }
                    }
                    else if (buttons & XINPUT_GAMEPAD_DPAD_RIGHT)
                    {
                        oldright = true;
                        ButtonStateImpulse(TranslateXtoMKB::rightmapping, true);//down
                    }


                    if (oldup)
                    {
                        if (buttons & XINPUT_GAMEPAD_DPAD_UP)
                        {
                        }
                        else {
                            oldup = false;
                            ButtonStateImpulse(TranslateXtoMKB::upmapping, false);//release
                        }
                    }
                    else if (buttons & XINPUT_GAMEPAD_DPAD_UP)
                    {
                        oldup = true;
                        ButtonStateImpulse(TranslateXtoMKB::upmapping, true);//down
                    }


                    if (olddown)
                    {
                        if (buttons & XINPUT_GAMEPAD_DPAD_DOWN)
                        {
                        }
                        else {
                            olddown = false;
                            ButtonStateImpulse(TranslateXtoMKB::downmapping, false);//release
                        }
                    }
                    else if (buttons & XINPUT_GAMEPAD_DPAD_DOWN)
                    {
                        olddown = true;
						ButtonStateImpulse(TranslateXtoMKB::downmapping, true);//down
                    }

                } //if mode above 0
            } //if controller
            else {
                showmessage = 12;
            }

            if (TranslateXtoMKB::rawinputhook == 1)
            {
               
            }
            if (tick < updatewindowtick)
                tick++;
            else {
                Proto::HwndSelector::UpdateMainHwnd(false);
                Proto::HwndSelector::UpdateWindowBounds();
                tick = 0;
            }

            if (mode == 0)
                Sleep(10);
            if (mode > 0) {
                Sleep(1);
            }
            if (showmessage == 99)
                Sleep(15);
        } //loop end but endless
        return;
    }

    void TranslateXtoMKB::Initialize(HMODULE hModule)
    {
            RawInput::Initialize();
            g_hModule = hModule;
            InitializeCriticalSection(&critical);
            std::thread one(ThreadFunction, g_hModule);
            one.detach();
            return;
    } 
}