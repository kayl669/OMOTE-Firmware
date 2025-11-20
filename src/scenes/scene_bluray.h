#pragma once

#include <string>
#include <stdint.h>

extern uint16_t SCENE_BLURAY;
// FORCE sends the start sequence again even if scene is already active
extern uint16_t SCENE_BLURAY_FORCE;

extern std::string scene_name_bluray;
void register_scene_bluray_commands(void);
void register_scene_bluray(void);
