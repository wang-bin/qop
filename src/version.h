#ifndef VERSION_H
#define VERSION_H

template<typename T>
char* version_str(T major, T minor, T patch)
{
	char *ver_str = new char[9];
	sprintf(ver_str, "%d.%d.%d", major, minor, patch);
	return ver_str;
}

#define APP_NAME "qop"

#define APP_VERSION 0x000208

#define MAJOR ((APP_VERSION&0xff0000)>>16)
#define MINOR ((APP_VERSION&0xff00)>>8)
#define PATCH (APP_VERSION&0xff)

#define APP_VERSION_STR version_str(MAJOR, MINOR, PATCH)

#endif // VERSION_H
