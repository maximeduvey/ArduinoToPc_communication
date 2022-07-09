#include <iostream>
#include "ArduinoToPc.h"

int main()
{
    const int sizeofBuffer = 1024;
    char* buffer = new char[sizeofBuffer];
    buffer[0] = 0;

    try
    {
        ArduinoToPc atp("COM3", 9600);

        atp.initConnection();
        //atp.readInfinitlyTheOutput();

        try
        {
            while (true)
            {
                size_t retSize = atp.readCom(buffer, sizeofBuffer);

                for (size_t i = 0; i < retSize; ++i)
                    std::cout << buffer[i];

                atp.writeToArduino("Hello world");
            }
        }
        catch (ArduinoToPc_Exception& e)
        {
            throw e;
        }
    }
    catch (ArduinoToPc_Exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}