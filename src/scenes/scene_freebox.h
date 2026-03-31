#pragma once

#include <string>
#include <stdint.h>

extern uint16_t SCENE_FREEBOX;
// FORCE sends the start sequence again even if scene is already active
extern uint16_t SCENE_FREEBOX_FORCE;

extern std::string scene_name_freebox;
void register_scene_freebox_commands(void);
void register_scene_freebox(void);
