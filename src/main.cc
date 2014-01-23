#include <iostream>
#include <string>
#include <stdexcept>

#include "Application.hh"

// Main program: One ran Application in a try-catch wrapper
int main()
{
	try
	{
		Application app;
		app.run();
	}
	catch (std::exception& e)
	{
		std::cout << std::endl << "Exception: " << e.what() << std::endl;
		return 1;
	}

    return 0;
}

