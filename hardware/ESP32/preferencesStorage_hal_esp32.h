#pragma once

#include <string>
#include "applicationInternal/backlightSignal.h"

BacklightSignalSettings get_backlightSignalSettings_HAL();
void set_backlightSignalSettings_HAL(BacklightSignalSettings aSettings);

void init_preferences_HAL(void);
void save_preferences_HAL(void);

std::string get_activeScene_HAL();
void set_activeScene_HAL(std::string anActiveScene);
