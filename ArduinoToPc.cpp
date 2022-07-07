#include "ArduinoToPc.h"

#include <string>
#include <stdio.h>
#include <iostream>
#include <bitset>

/// <summary>
/// 
/// </summary>
/// <param name="comPortName"> The com port Name is the one used to upload code on the arduino (example COM3) </param>
/// <param name="serialrate"> The serialRate is the frequency set on your arduino Serial.begin(x);, it must the same exact value </param>
ArduinoToPc::ArduinoToPc(const std::string& comPortName, const unsigned int serialRate/* = 9600 */) :
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
			throw ArduinoToPc_Exception("initConnection", "Handle could not be attached to port: " + _comPortName + " Not available. (maybe disconnected cables?) ");
		else
			throw ArduinoToPc_Exception("initConnection", _comPortName + " is already in use by another program (maybe arduino.exe serial monitoring?), or was not closed properly by previous program.");
	DCB dcbSerialParams = { 0 };

	if (!GetCommState(_hSerial, &dcbSerialParams))
		throw ArduinoToPc_Exception("initConnection", "Failed to get current serial parameters : " + GetLastError());

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

	//Set the type of data to be caught.(RXCHAR -> Data available on RX pin.) * A character was received and placed in the input buffer.
	if (SetCommMask(_hSerial, EV_RXCHAR) == 0)
		throw ArduinoToPc_Exception("readInfinitlyTheOutput", "SetCommMask failed : " + GetLastError());

	// we make sure the readFile will return when data is available, and not when it readed the specified maximum amount
	// wich happen if the timeout is 0
	COMMTIMEOUTS s_timeout;
	if (GetCommTimeouts(_hSerial , &s_timeout) == FALSE)
		throw ArduinoToPc_Exception("readInfinitlyTheOutput", " could not get GetCommTimeouts : " + GetLastError());
	s_timeout.ReadIntervalTimeout = 50;
	if (SetCommTimeouts(_hSerial, &s_timeout) == FALSE)
		throw ArduinoToPc_Exception("readInfinitlyTheOutput", "SetCommTimeouts failed : " + GetLastError());
}

// should not be used for now
void ArduinoToPc::setMode()
{
	// Get the current console mode
	DWORD mode;
	GetConsoleMode(_hSerial, &mode);

	// Save the current mode, so we can restore it later
	DWORD original_mode = mode;

	// Disable the line input mode
	mode |= ENABLE_LINE_INPUT;

	// And set the new mode
	SetConsoleMode(_hSerial, mode);

	std::cout << "bits " << std::bitset<64>(original_mode) << std::endl;
	std::cout << "bits " << std::bitset<64>(mode) << std::endl;
}

/// <summary>
/// This function return true if the communication is etablished with the Arduino
/// </summary>
/// <returns></returns>
bool ArduinoToPc::isConnected() const
{
	return _isconnected;
}

/// <summary>
/// This function will read on the com socket, fill the buffer and return the number of character retrieved
/// </summary>
/// <param name="buffer"></param>
/// <param name="bufferSize"></param>
/// <returns></returns>
unsigned int ArduinoToPc::readCom(char* buffer, unsigned int bufferSize)
{
	DWORD bytesRead;
	DWORD dwCommModemStatus;

	// Wait for an event to occur for the port.
	if (WaitCommEvent(_hSerial, &dwCommModemStatus, 0) == 0)
		throw ArduinoToPc_Exception("readCom", "WaitCommEvent failed : " + GetLastError());
	else if (dwCommModemStatus & EV_RXCHAR) // mask to see if the flag EV_RXCHAR (0x0001 : A character was received and placed in the input buffer.) is here
	{
		// Reads data from the specified file or input/output (I/O) device. Reads occur at the position specified by the file pointer if supported by the device.
		if (ReadFile(_hSerial, buffer, bufferSize, &bytesRead, 0) == TRUE) //success
		{
			if (bytesRead < sizeof(buffer))
				buffer[bytesRead] = '\0';
			return bytesRead;
		}
		else
		{
			throw ArduinoToPc_Exception("readCom", "Error while trying to read socket, maybe something went wrong : " + GetLastError());
			return 0;
		}
	}
	return 0;
}

/// <summary>
/// This function is an infinite loop reading and outputing on cout, the readed serial
/// </summary>
void ArduinoToPc::readInfinitlyTheOutput()
{
	const int sizeofBuffer = 1024;
	char* buffer = new char[sizeofBuffer];
	buffer[0] = 0;

	while (_hSerial != INVALID_HANDLE_VALUE && _isconnected == true)
	{
		try
		{
			size_t retSize = readCom(buffer, sizeofBuffer);

			for (size_t i = 0; i < retSize; ++i)
				std::cout << buffer[i];
		}
		catch (ArduinoToPc_Exception& e)
		{
			throw e;
		}
	}
}