#include <map>
#include "scenes/scene_switch.h"
#include "applicationInternal/keys.h"
#include "applicationInternal/scenes/sceneRegistry.h"
#include "applicationInternal/hardware/hardwarePresenter.h"
// devices
#include "devices/TV/device_sonyTV/device_sonyTV.h"
#include "devices/AVreceiver/device_boseAmp/device_boseAmp.h"
#include "applicationInternal/commandHandler.h"
// guis

uint16_t SCENE_SWITCH; //"Scene_switch"
uint16_t SCENE_SWITCH_FORCE; //"Scene_switch_force"

std::map<char, repeatModes> key_repeatModes_switch;
std::map<char, uint16_t> key_commands_short_switch;
std::map<char, uint16_t> key_commands_long_switch;

void scene_setKeys_switch() {
    key_repeatModes_switch = {
        {KEY_STOP, SHORT_REPEATED}, {KEY_REWI, SHORT}, {KEY_PLAY, SHORT}, {KEY_FORW, SHORT_REPEATED},
        {KEY_CONF, SHORT}, {KEY_INFO, SHORT},
        {KEY_UP, SHORT_REPEATED},
        {KEY_LEFT, SHORT_REPEATED}, {KEY_OK, SHORT}, {KEY_RIGHT, SHORT_REPEATED},
        {KEY_DOWN, SHORT_REPEATED},
        {KEY_BACK, SHORT}, {KEY_SRC, SHORT},
        {KEY_VOLUP, SHORT}, {KEY_MUTE, SHORT}, {KEY_CHUP, SHORT},
        {KEY_VOLDO, SHORT}, {KEY_REC, SHORT}, {KEY_CHDOW, SHORT},
    };

    key_commands_short_switch = {
    };

    key_commands_long_switch = {
    };
}

void scene_start_sequence_switch(void) {
    executeCommand(SONY_POWER_ON);
    delay(500);
    executeCommand(BOSE_POWER_ON);
    delay(500);
    executeCommand(BOSE_CBL_SAT);
    delay(500);
    executeCommand(SONY_INPUT_HDMI_3);
}

void scene_end_sequence_switch(void) {
    executeCommand(SONY_POWER_OFF);
    delay(500);
    executeCommand(BOSE_POWER_OFF);
}

std::string scene_name_switch = "Switch";

void register_scene_switch(void) {
    register_command(&SCENE_SWITCH, makeCommandData(SCENE, {scene_name_switch}));
    register_command(&SCENE_SWITCH_FORCE, makeCommandData(SCENE, {scene_name_switch, "FORCE"}));

    register_scene(
        scene_name_switch,
        &scene_setKeys_switch,
        &scene_start_sequence_switch,
        &scene_end_sequence_switch,
        &key_repeatModes_switch,
        &key_commands_short_switch,
        &key_commands_long_switch,
        NULL,
        SCENE_SWITCH);
}
