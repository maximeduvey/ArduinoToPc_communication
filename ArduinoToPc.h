#pragma once

#include <exception>
#include <iostream>
#include <windows.h>


/* This custom exception manage problem for the aruinoToPcClass 
* It is made the way to have a future logic with logs, maybe some saved logged files
*/
class ArduinoToPc_Exception : public std::exception
{
public :
	ArduinoToPc_Exception(const std::string& name, const std::string& message) :
		std::exception((name + " -> " + message).c_str())
	{	}
};

class ArduinoToPc
{
public:
	ArduinoToPc(const std::string& comPortName, const unsigned int serialRate = 9600);
	~ArduinoToPc();

	void initConnection();

	bool isConnected() const;

	void readInfinitlyTheOutput();
	unsigned int readCom(char *buffer, unsigned int bufferSize);

private:
	//fct
	void setMode();


	// var
	const std::string _comPortName;
	const unsigned int _serialRate;
	bool _isconnected = false;
	HANDLE _hSerial = nullptr;
	COMSTAT status;

};

/* hope it's useful for you
	|\__/,|   (`\
  _.|o o  |_   ) )
-(((---(((--------
*/
