#ifndef config_h
#define config_h
#include <Preferences.h>
extern Preferences pf;

#define CF_NAME "config"

#define config_get_str(key,default) pf.getString(key,default)
#define config_set_str(key,value) pf.putString(key,value)

void ConfigSetup();

#endif
