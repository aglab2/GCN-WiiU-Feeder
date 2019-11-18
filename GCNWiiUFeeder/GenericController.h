#pragma once

namespace Generic
{
    struct Controller
    {
        unsigned short Buttons;
        float AnalogX;
        float AnalogY;
        float CStickX;
        float CStickY;
        float LeftTrigger;
        float RightTrigger;
    };
}