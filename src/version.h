#ifndef VERSION_H
#define VERSION_H

template<int major, int minor, int patch>
char* version_str()
{
	char *ver_str = new char[9];
	sprintf(ver_str, "%d.%d.%d", major, minor, patch);
	return ver_str;
}

template<int ver>
struct version {
	enum{
		value = ver,
		major = ((ver&0xff0000)>>16),
		minor = ((ver&0xff00)>>8),
		patch = (ver&0xff)
	};
	//static char* string;
};

#define APP_NAME "qop"

#define APP_VERSION 0x000208

#define MAJOR ((APP_VERSION&0xff0000)>>16)
#define MINOR ((APP_VERSION&0xff00)>>8)
#define PATCH (APP_VERSION&0xff)

#define APP_VERSION_STR version_str<MAJOR, MINOR, PATCH>()
//#define APP_VERSION_STR version<APP_VERSION>::string


#endif // VERSION_H
