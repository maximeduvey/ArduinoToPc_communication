#include <iostream>
#include "ArduinoToPc.h"

int main()
{
    try
    {
        ArduinoToPc atp("COM3", 9600);

        atp.initConnection();
        atp.readInfinitlyTheOutput();
    }
    catch (ArduinoToPc_Exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}