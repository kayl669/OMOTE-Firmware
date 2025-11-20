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

uint16_t SCENE_PS4         ; //"Scene_ps4"
uint16_t SCENE_PS4_FORCE   ; //"Scene_ps4_force"

std::map<char, repeatModes> key_repeatModes_ps4;
std::map<char, uint16_t> key_commands_short_ps4;
std::map<char, uint16_t> key_commands_long_ps4;

void scene_setKeys_ps4() {
  key_repeatModes_ps4 = {
    {KEY_STOP  , SHORT_REPEATED   },  {KEY_REWI  , SHORT            },  {KEY_PLAY  , SHORT            },  {KEY_FORW  , SHORT_REPEATED   },
    {KEY_CONF  , SHORT            },                                                                      {KEY_INFO  , SHORT            },
                                                        {KEY_UP   , SHORT_REPEATED   },
                       {KEY_LEFT , SHORT_REPEATED   },  {KEY_OK   , SHORT            },  {KEY_RIGHT, SHORT_REPEATED   },
                                                        {KEY_DOWN , SHORT_REPEATED   },
    {KEY_BACK  , SHORT            },                                                                       {KEY_SRC,   SHORT            },
    {KEY_VOLUP , SHORT            },                    {KEY_MUTE   , SHORT          },                    {KEY_CHUP , SHORT            },
    {KEY_VOLDO , SHORT            },                    {KEY_REC    , SHORT          },                    {KEY_CHDOW, SHORT            },
  };

  key_commands_short_ps4 = {
  };
  
  key_commands_long_ps4 = {
  };

}

void scene_start_sequence_ps4(void) {
  executeCommand(SONY_POWER_ON);
  delay(500);
  executeCommand(SONY_INPUT_HDMI_3);
  delay(500);
  executeCommand(BOSE_POWER_ON);
  delay(500);
  executeCommand(BOSE_GAME);
}

void scene_end_sequence_ps4(void) {
  executeCommand(SONY_POWER_OFF);
  delay(500);
  executeCommand(BOSE_POWER_OFF);
}

std::string scene_name_ps4 = "Ps4";

void register_scene_ps4(void) {
  register_command(&SCENE_PS4,       makeCommandData(SCENE, {scene_name_ps4}));
  register_command(&SCENE_PS4_FORCE, makeCommandData(SCENE, {scene_name_ps4, "FORCE"}));

  register_scene(
    scene_name_ps4,
    & scene_setKeys_ps4,
    & scene_start_sequence_ps4,
    & scene_end_sequence_ps4,
    & key_repeatModes_ps4,
    & key_commands_short_ps4,
    & key_commands_long_ps4,
    NULL,
    SCENE_PS4);
}
