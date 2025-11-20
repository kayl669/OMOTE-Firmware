#include <map>
#include "scenes/scene_TV.h"
#include "applicationInternal/keys.h"
#include "applicationInternal/scenes/sceneRegistry.h"
#include "applicationInternal/hardware/hardwarePresenter.h"
// devices
#include "devices/TV/device_sonyTV/device_sonyTV.h"
#include "devices/AVreceiver/device_boseAmp/device_boseAmp.h"
//#include "devices/TV/device_samsungTV/device_samsungTV.h"
//#include "devices/AVreceiver/device_yamahaAmp/device_yamahaAmp.h"
#include "applicationInternal/commandHandler.h"
// guis
//#include "guis/gui_numpad.h"

uint16_t SCENE_TV         ; //"Scene_tv"
uint16_t SCENE_TV_FORCE   ; //"Scene_tv_force"

std::map<char, repeatModes> key_repeatModes_TV;
std::map<char, uint16_t> key_commands_short_TV;
std::map<char, uint16_t> key_commands_long_TV;

void scene_setKeys_TV() {
  key_repeatModes_TV = {
    {KEY_STOP  , SHORT_REPEATED   },  {KEY_REWI  , SHORT            },  {KEY_PLAY  , SHORT            },  {KEY_FORW  , SHORT_REPEATED   },
    {KEY_CONF  , SHORT            },                                                                      {KEY_INFO  , SHORT            },
                                                        {KEY_UP   , SHORT_REPEATED   },
                       {KEY_LEFT , SHORT_REPEATED   },  {KEY_OK   , SHORT            },  {KEY_RIGHT, SHORT_REPEATED   },
                                                        {KEY_DOWN , SHORT_REPEATED   },
    {KEY_BACK  , SHORT            },                                                                       {KEY_SRC,   SHORT            },
    {KEY_VOLUP , SHORT            },                    {KEY_MUTE   , SHORT          },                    {KEY_CHUP , SHORT            },
    {KEY_VOLDO , SHORT            },                    {KEY_REC    , SHORT          },                    {KEY_CHDOW, SHORT            },
  };

  key_commands_short_TV = {
    {KEY_STOP,  SONY_STOP        },  {KEY_REWI,  SONY_REWIND      },  {KEY_PLAY,   SONY_PLAY       },  {KEY_FORW,   SONY_FASTFORWARD },
    {KEY_CONF,  SONY_HOME        },                                                                    {KEY_INFO,  SONY_INFO       },
                                                     {KEY_UP,    SONY_UP           },
                        {KEY_LEFT,  SONY_LEFT    },  {KEY_OK,    SONY_SELECT       },    {KEY_RIGHT, SONY_RIGHT    },
                                                     {KEY_DOWN,  SONY_DOWN         },
    {KEY_BACK,   SONY_RETURN    },                                                                    {KEY_SRC,   SONY_MENU     },
    {KEY_VOLUP,   BOSE_VOL_UP    },                  {KEY_MUTE,    BOSE_MUTE       },                  {KEY_CHUP,  SONY_CHANNEL_UP},
    {KEY_VOLDO,   BOSE_VOL_DOWN  },                  {KEY_REC,     SONY_CONFIGURATION      },                  {KEY_CHDOW, SONY_CHANNEL_DOWN},
  };
  
  key_commands_long_TV = {
    {KEY_STOP,  SONY_STOP        },  {KEY_REWI,  SONY_REWIND      },  {KEY_PLAY,   SONY_PAUSE       },  {KEY_FORW,   SONY_FASTFORWARD },
    {KEY_CONF,  SONY_HOME        },                                                                    {KEY_INFO,  SONY_INFO       },
                                                     {KEY_UP,    SONY_UP           },
                        {KEY_LEFT,  SONY_LEFT    },  {KEY_OK,    SONY_SELECT       },    {KEY_RIGHT, SONY_RIGHT    },
                                                     {KEY_DOWN,  SONY_DOWN         },
    {KEY_BACK,   SONY_RETURN    },                                                                    {KEY_SRC,   SONY_MENU     },
    {KEY_VOLUP,   BOSE_VOL_UP    },                  {KEY_MUTE,    BOSE_MUTE       },                  {KEY_CHUP,  SONY_CHANNEL_UP},
    {KEY_VOLDO,   BOSE_VOL_DOWN  },                  {KEY_REC,     SONY_PAUSE      },                  {KEY_CHDOW, SONY_CHANNEL_DOWN},
  };

}

void scene_start_sequence_TV(void) {
  executeCommand(SONY_POWER_ON);
  delay(500);
  executeCommand(SONY_INPUT_TV);
  delay(500);
  executeCommand(BOSE_POWER_ON);
  delay(500);
  executeCommand(BOSE_INPUT);
}

void scene_end_sequence_TV(void) {
  executeCommand(SONY_POWER_OFF);
  delay(500);
  executeCommand(BOSE_POWER_OFF);
}

std::string scene_name_TV = "TV";

void register_scene_TV(void) {
  register_command(&SCENE_TV,       makeCommandData(SCENE, {scene_name_TV}));
  register_command(&SCENE_TV_FORCE, makeCommandData(SCENE, {scene_name_TV, "FORCE"}));

  register_scene(
    scene_name_TV,
    & scene_setKeys_TV,
    & scene_start_sequence_TV,
    & scene_end_sequence_TV,
    & key_repeatModes_TV,
    & key_commands_short_TV,
    & key_commands_long_TV,
    NULL,
    SCENE_TV);
}
