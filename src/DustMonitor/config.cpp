#include <Preferences.h>
#include "config.h"
Preferences pf;

void ConfigSetup() {
  pf.begin(CF_NAME,false);
}
