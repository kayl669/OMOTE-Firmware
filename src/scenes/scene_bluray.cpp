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
#include "devices/mediaPlayer/device_samsungbluray/device_samsungbluray.h"

uint16_t SCENE_BLURAY         ; //"Scene_bluray"
uint16_t SCENE_BLURAY_FORCE   ; //"Scene_bluray_force"

std::map<char, repeatModes> key_repeatModes_bluray;
std::map<char, uint16_t> key_commands_short_bluray;
std::map<char, uint16_t> key_commands_long_bluray;

void scene_setKeys_bluray() {
  key_repeatModes_bluray = {
    {KEY_STOP  , SHORT_REPEATED   },  {KEY_REWI  , SHORT            },  {KEY_PLAY  , SHORT            },  {KEY_FORW  , SHORT_REPEATED   },
    {KEY_CONF  , SHORT            },                                                                      {KEY_INFO  , SHORT            },
                                                        {KEY_UP   , SHORT_REPEATED   },
                       {KEY_LEFT , SHORT_REPEATED   },  {KEY_OK   , SHORT            },  {KEY_RIGHT, SHORT_REPEATED   },
                                                        {KEY_DOWN , SHORT_REPEATED   },
    {KEY_BACK  , SHORT            },                                                                       {KEY_SRC,   SHORT            },
    {KEY_VOLUP , SHORT            },                    {KEY_MUTE   , SHORT          },                    {KEY_CHUP , SHORT            },
    {KEY_VOLDO , SHORT            },                    {KEY_REC    , SHORT          },                    {KEY_CHDOW, SHORT            },
  };

  key_commands_short_bluray = {
    {KEY_STOP,  SAMSUNGBLURAY_STOP        },  {KEY_REWI,  SAMSUNGBLURAY_REW      },  {KEY_PLAY,   SAMSUNGBLURAY_PLAY       },  {KEY_FORW,   SAMSUNGBLURAY_FF },
    {KEY_CONF,  SAMSUNGBLURAY_DISC_MENU        },                                                                    {KEY_INFO,  SAMSUNGBLURAY_INFO       },
                                                     {KEY_UP,    SAMSUNGBLURAY_CURSOR_UP           },
                        {KEY_LEFT,  SAMSUNGBLURAY_LEFT    },  {KEY_OK,    SAMSUNGBLURAY_ENTER       },    {KEY_RIGHT, SAMSUNGBLURAY_RIGHT    },
                                                     {KEY_DOWN,  SAMSUNGBLURAY_CURSOR_DOWN         },
    {KEY_BACK,   SAMSUNGBLURAY_BACK    },                                                                    {KEY_SRC,   SAMSUNGBLURAY_SUBTITLE     },
    {KEY_VOLUP,   BOSE_VOL_UP    },                  {KEY_MUTE,    BOSE_MUTE       },                  {KEY_CHUP,  SAMSUNGBLURAY_SKIP},
    {KEY_VOLDO,   BOSE_VOL_DOWN  },                  {KEY_REC,     SAMSUNGBLURAY_OPEN_CLOSE      },                  {KEY_CHDOW, SAMSUNGBLURAY_PREVIOUS},
  };
  
  key_commands_long_bluray = {
  
  
  };

}

void scene_start_sequence_bluray(void) {
  executeCommand(SAMSUNGBLURAY_POWER);
  delay(500);
  executeCommand(SONY_POWER_ON);
  delay(500);
  executeCommand(SONY_INPUT_HDMI_3);
  delay(500);
  executeCommand(BOSE_POWER_ON);
  delay(500);
  executeCommand(BOSE_BD_DVD);
}

void scene_end_sequence_bluray(void) {
  executeCommand(SAMSUNGBLURAY_PLAY);
  delay(500);
  executeCommand(SAMSUNGBLURAY_PLAY);
  delay(500);
  executeCommand(SAMSUNGBLURAY_POWER);
  delay(500);
  executeCommand(SONY_POWER_OFF);
  delay(500);
  executeCommand(BOSE_POWER_OFF);
}

std::string scene_name_bluray = "Regarder un film";

void register_scene_bluray(void) {
  register_command(&SCENE_BLURAY,       makeCommandData(SCENE, {scene_name_bluray}));
  register_command(&SCENE_BLURAY_FORCE, makeCommandData(SCENE, {scene_name_bluray, "FORCE"}));

  register_scene(
    scene_name_bluray,
    & scene_setKeys_bluray,
    & scene_start_sequence_bluray,
    & scene_end_sequence_bluray,
    & key_repeatModes_bluray,
    & key_commands_short_bluray,
    & key_commands_long_bluray,
    NULL,
    SCENE_BLURAY);
}
