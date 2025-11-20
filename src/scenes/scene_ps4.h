#pragma once

#include <string>
#include <stdint.h>

extern uint16_t SCENE_PS4;
// FORCE sends the start sequence again even if scene is already active
extern uint16_t SCENE_PS4_FORCE;

extern std::string scene_name_ps4;
void register_scene_ps4_commands(void);
void register_scene_ps4(void);
