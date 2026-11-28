#pragma once

#include <string>
#include <stdint.h>

extern uint16_t SCENE_SWITCH;
// FORCE sends the start sequence again even if scene is already active
extern uint16_t SCENE_SWITCH_FORCE;

extern std::string scene_name_switch;

void register_scene_switch_commands(void);

void register_scene_switch(void);
