#include "HRErrorHandler.h"
#include <iostream>
#include <iomanip>

inline void HRErrorHandler::Throw(HRESULT hr)
{
	std::cerr << "HRESULT: 0x"
		<< std::hex          // Switch to hexadecimal output
		<< std::uppercase    // Use uppercase letters (A-F)
		<< std::setfill('0') // Pad with zeros if the number is short
		<< std::setw(8)      // Ensure the output is exactly 8 characters wide
		<< hr
		<< std::dec          // Reset stream to decimal (good practice)
		<< std::endl;
}
