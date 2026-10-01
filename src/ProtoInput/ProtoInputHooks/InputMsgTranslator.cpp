#include "InputMsgTranslator.h"
#include "windowsx.h"


namespace Proto
{
    int scalewidth = 0;
    int scaleheight = 0;
    int origwidth = 0;
    int origheight = 0;

    const WPARAM InputMsgTranslator::defaultmove = 0x00060001;
    const WPARAM InputMsgTranslator::primary = 0x00100000;
    const WPARAM InputMsgTranslator::secondary = 0x00200000;
    const WPARAM InputMsgTranslator::third = 0x00400000;
    const WPARAM InputMsgTranslator::fourth = 0x00800000;
    const WPARAM InputMsgTranslator::fifth = 0x01000000;

    bool leftdown, rightdown, tredown, firdown, femdown = false;

    bool InputMsgTranslator::ScalingEnabled = false;
    bool InputMsgTranslator::PointerMessages = false;

    bool InputMsgTranslator::IsFirstTouch(){
		return !leftdown && !rightdown && !tredown && !firdown && !femdown;
    }
    WPARAM InputMsgTranslator::updatePointer(bool pressed, WPARAM button)
    {
        if (button == 0) //mousemove
        {
            if (!rightdown && !leftdown && !tredown && !firdown && !femdown)
            {
                return 0x20020001;
			}
        }

        if (button == primary)
        {
            if (pressed) leftdown = true; 
            else  leftdown = false;

			if (!rightdown && !tredown && !firdown && !femdown)
			{
				if (pressed)
				{
					return 0x20160001;
				}
				else return 0x00020001;
			}
        }

        else if (button == secondary)
        {
            if (pressed) rightdown = true;
            else rightdown = false;

            if (!leftdown && !tredown && !firdown && !femdown)
            {
                if (pressed)
                {
                    return 0x20260001;
                }
                else return 0x00020001;
            }
        }

        else if (button == third)
        {
            if (pressed) tredown = true;  
            else tredown = false;

            if (!leftdown && !rightdown && !firdown && !femdown)
            {
                if (pressed)
                {
                    return 0x20460001;
                }
                else return 0x00020001;
            }
        }

        else if (button == fourth)
        {
            if (pressed) firdown = true;
            else firdown = false;

            if (!leftdown && !rightdown && !tredown && !femdown)
            {
                if (pressed)
                {
                    return 0x20860001;
                }
                else return 0x00020001;
            }

        }

        else if (button == fifth)
        {
            if (pressed) femdown = true;
            else femdown = false;

            if (!leftdown && !rightdown && !tredown && !firdown)
            {
                if (pressed)
                {
                    return 0x21060001;
                }
                else return 0x00020001;
            }

        }

        WPARAM returned = 0;
        if (leftdown) returned += primary;
        if (rightdown) returned += secondary;
        if (tredown) returned += third;
        if (firdown) returned += fourth;
        if (femdown) returned += fifth;
        return returned += defaultmove;
    }

    POINT InputMsgTranslator::getfactor(POINT pp)
    {
        if (pp.x != 0 && pp.y != 0)
        { 
            float scalex = float(origwidth) / float(scalewidth);
            float scaley = float(origheight) / float(scaleheight);
            pp.x = static_cast<int>(std::lround(pp.x * scalex));
            pp.y = static_cast<int>(std::lround(pp.y * scaley));
        }
		return pp;
    }

    LPARAM InputMsgTranslator::ProcessedLparam(LPARAM lParam, HWND hwnd, bool clienttoscreen, bool scale)
    {
        POINT clientPos = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        if (scale)
            clientPos = InputMsgTranslator::getfactor(clientPos);
        if (clienttoscreen)
            ClientToScreen(hwnd, &clientPos);
        return MAKELPARAM(clientPos.x, clientPos.y);
    }



    void InputMsgTranslator::Settings(int oldX, int oldY, int newX, int newY)
    {
		scalewidth = newX;
	    scaleheight = newY;
        origwidth = oldX;
        origheight = oldY;

        //validating settings
        if (scalewidth > 5 && scaleheight > 5 && origwidth > 5 && origheight > 5)
        { 
            InputMsgTranslator::ScalingEnabled = true;
        } 
        else InputMsgTranslator::ScalingEnabled = false;
    }

    void InputMsgTranslator::PointerInMouse(bool enable) //convert mouse message to pointer message
    {
        if (enable)
            InputMsgTranslator::PointerMessages = true;
        else  InputMsgTranslator::PointerMessages = false;
    }

}  
