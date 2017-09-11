#ifndef UPNP_GLOBAL_H
#define UPNP_GLOBAL_H 1

/*! Defines export or import keyword for windows dll. */

#ifdef _OS_WIN

#ifdef UPNP_EXPORTS
#define UPNP_API __declspec(dllexport)
#else
#define UPNP_API __declspec(dllimport)
#endif

#else
#define UPNP_API
#endif

#endif
