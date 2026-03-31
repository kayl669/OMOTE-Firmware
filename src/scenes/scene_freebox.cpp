#include <map>
#include "scenes/scene_BLURAY.h"
#include "applicationInternal/keys.h"
#include "applicationInternal/scenes/sceneRegistry.h"
#include "applicationInternal/hardware/hardwarePresenter.h"
// devices
#include "devices/TV/device_sonyTV/device_sonyTV.h"
#include "devices/AVreceiver/device_boseAmp/device_boseAmp.h"
#include "applicationInternal/commandHandler.h"
// guis
#include "guis/gui_tv.h"

uint16_t SCENE_FREEBOX         ; //"Scene_freebox"
uint16_t SCENE_FREEBOX_FORCE   ; //"Scene_freebox_force"

std::map<char, repeatModes> key_repeatModes_freebox;
std::map<char, uint16_t> key_commands_short_freebox;
std::map<char, uint16_t> key_commands_long_freebox;

void scene_setKeys_freebox() {
  key_repeatModes_freebox = {
    {KEY_STOP  , SHORT_REPEATED   },  {KEY_REWI  , SHORT            },  {KEY_PLAY  , SHORT            },  {KEY_FORW  , SHORT_REPEATED   },
    {KEY_CONF  , SHORT            },                                                                      {KEY_INFO  , SHORT            },
                                                        {KEY_UP   , SHORT_REPEATED   },
                       {KEY_LEFT , SHORT_REPEATED   },  {KEY_OK   , SHORT            },  {KEY_RIGHT, SHORT_REPEATED   },
                                                        {KEY_DOWN , SHORT_REPEATED   },
    {KEY_BACK  , SHORT            },                                                                       {KEY_SRC,   SHORT            },
    {KEY_VOLUP , SHORT            },                    {KEY_MUTE   , SHORT          },                    {KEY_CHUP , SHORT            },
    {KEY_VOLDO , SHORT            },                    {KEY_REC    , SHORT          },                    {KEY_CHDOW, SHORT            },
  };

  key_commands_short_freebox = {
  };
  
  key_commands_long_freebox = {
  };

}

void scene_start_sequence_freebox(void) {
  executeCommand(SONY_POWER_ON);
  delay(500);
  executeCommand(SONY_INPUT_HDMI_3);
  delay(500);
  executeCommand(BOSE_POWER_ON);
  delay(500);
  executeCommand(BOSE_AUX);
  delay(500);
  executeCommand(KEYBOARD_BLE_FREEBOX_HOME);
  currentStartTab = 2;
  currentIndex = 1;
}

void scene_end_sequence_freebox(void) {
  executeCommand(SONY_POWER_OFF);
  delay(500);
  executeCommand(BOSE_POWER_OFF);
}

std::string scene_name_freebox = "Freebox";
t_gui_list scene_freebox_gui_list = {tabName_tv};

void register_scene_freebox(void) {
  register_command(&SCENE_FREEBOX,       makeCommandData(SCENE, {scene_name_freebox}));
  register_command(&SCENE_FREEBOX_FORCE, makeCommandData(SCENE, {scene_name_freebox, "FORCE"}));

  register_scene(
    scene_name_freebox,
    & scene_setKeys_freebox,
    & scene_start_sequence_freebox,
    & scene_end_sequence_freebox,
    & key_repeatModes_freebox,
    & key_commands_short_freebox,
    & key_commands_long_freebox,
    & scene_freebox_gui_list,
    SCENE_FREEBOX);
}
