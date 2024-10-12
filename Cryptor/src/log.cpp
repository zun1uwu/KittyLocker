#include <kitty/log.h>

void KittyLog(std::string s, bool quit)
{
	std::cout << s << "\n";

	if (quit)
		exit(0);
}