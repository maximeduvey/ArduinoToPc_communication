#include "ArduinoToPc.h"

#include <windows.h>
#include <string>
#include <stdio.h>
#include <iostream>

/// <summary>
/// 
/// </summary>
/// <param name="comPortName"> The com port Name is the one used to upload code on the arduino (example COM3) </param>
/// <param name="serialrate"> The serialRate is the frequency set on your arduino Serial.begin(x);, it must the same exact value </param>
ArduinoToPc::ArduinoToPc(const std::string &comPortName, const unsigned int serialRate/* = 9600 */) :
	_comPortName(comPortName), _serialRate(serialRate)
{
	if (_comPortName.empty())
		throw ArduinoToPc_Exception("Constructor", "CommunicationPort cannot be null.");
}

ArduinoToPc::~ArduinoToPc()
{
    if (_hSerial != nullptr)
        CloseHandle(_hSerial);
}

void ArduinoToPc::initConnection()
{
    _hSerial = CreateFileA(_comPortName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (_hSerial == INVALID_HANDLE_VALUE)
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
            throw ArduinoToPc_Exception("initConnection", "Handle could not be attached to port: "+_comPortName+" Not available. ");
        else
            throw ArduinoToPc_Exception("initConnection", _comPortName + " is already in use by another program (maybe arduino.exe serial monitoring?), or was not closed properly by previous program.");
    else
    {
        DCB dcbSerialParams = { 0 };

        if (!GetCommState(_hSerial, &dcbSerialParams))
            throw ArduinoToPc_Exception("initConnection", "Failed to get current serial parameters : " + GetLastError());
        else
        {
            // Set Serial Port specifications.
            dcbSerialParams.BaudRate = _serialRate;
            dcbSerialParams.ByteSize = 8;
            dcbSerialParams.StopBits = ONESTOPBIT;
            dcbSerialParams.Parity = NOPARITY;

            if (!SetCommState(_hSerial, &dcbSerialParams))
                throw ArduinoToPc_Exception("initConnection", "Could not set Serial Port parameters : " + GetLastError());
            else
            {
                _isconnected = true;
                std::cout << "Connection successful on port :" << _comPortName << std::endl;

                // we are now ready to communicate with it
            }
        }
    }
}

/// <summary>
/// This function return true if the communication is etablished with the Arduino
/// </summary>
/// <returns></returns>
bool ArduinoToPc::isConnected() const
{
	return _isconnected;
}