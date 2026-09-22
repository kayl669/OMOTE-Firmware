#include <map>
#include "applicationInternal/keys.h"
#include "applicationInternal/scenes/sceneRegistry.h"
#include "applicationInternal/commandHandler.h"
// devices
#include "devices/TV/device_samsungTV/device_samsungTV.h"
#include "devices/misc/device_home/device_home.h"
// scenes
#include "scene__default.h"
#include "scenes/scene_allOff.h"

std::map<char, repeatModes> key_repeatModes_default;
std::map<char, uint16_t> key_commands_short_default;
std::map<char, uint16_t> key_commands_long_default;

void register_scene_defaultKeys(void) {
  key_repeatModes_default = {
                                                                                                             {KEY_OFF,   SHORTorLONG      },
    {KEY_STOP,  SHORT_REPEATED   },    {KEY_REWI,  SHORT_REPEATED   },    {KEY_PLAY,  SHORT_REPEATED   },    {KEY_FORW,  SHORT_REPEATED   },
    {KEY_CONF,  SHORT            },                                                                          {KEY_INFO,  SHORT            },
                                                         {KEY_UP,    SHORT            },
                      {KEY_LEFT,  SHORT            },    {KEY_OK,    SHORT            },    {KEY_RIGHT, SHORT            },
                                                         {KEY_DOWN,  SHORT            },
    {KEY_BACK,  SHORT            },                                                                          {KEY_SRC,   SHORT            },
    {KEY_VOLUP, SHORT_REPEATED   },                      {KEY_MUTE,  SHORT            },                     {KEY_CHUP,  SHORT            },
    {KEY_VOLDO, SHORT_REPEATED   },                      {KEY_REC,   SHORT            },                     {KEY_CHDOW, SHORT            },
    {KEY_RED,   SHORT_REPEATED   },    {KEY_GREEN, SHORT_REPEATED   },    {KEY_YELLO, SHORT_REPEATED   },    {KEY_BLUE,  SHORT_REPEATED   },
  };
  
  key_commands_short_default = {
                                                                                                             {KEY_OFF,   SAMSUNG_POWER_TOGGLE},
    {KEY_STOP,  BED_BLE_OKIN_BED_MEMORY_1     },  {KEY_REWI,  BED_BLE_OKIN_BED_LIGHT     },  {KEY_PLAY,   HOME_MQTT_COMMAND_OPEN       },  {KEY_FORW,   HOME_MQTT_COMMAND_CLOSE },
    {KEY_CONF,  SAMSUNG_TOOLS        },                                                                    {KEY_INFO,  SAMSUNG_INFO       },
                                                     {KEY_UP,    SAMSUNG_UP           },
                        {KEY_LEFT,  SAMSUNG_LEFT    },  {KEY_OK,    SAMSUNG_SELECT       },    {KEY_RIGHT, SAMSUNG_RIGHT    },
                                                     {KEY_DOWN,  SAMSUNG_DOWN         },
    {KEY_BACK,   SAMSUNG_RETURN    },                                                                    {KEY_SRC,   SAMSUNG_EXIT     },
    {KEY_VOLUP, SAMSUNG_VOL_PLUS  },                      {KEY_MUTE,  SAMSUNG_MUTE_TOGGLE},                  {KEY_CHUP,  SAMSUNG_CHANNEL_UP  },
    {KEY_VOLDO, SAMSUNG_VOL_MINUS },                      {KEY_REC,   SAMSUNG_GUIDE  },   {KEY_CHDOW, SAMSUNG_CHANNEL_DOWN  },
    {KEY_RED,   BED_BLE_OKIN_BED_HEAD_UP   },      {KEY_GREEN, BED_BLE_OKIN_BED_HEAD_DOWN},         {KEY_YELLO, BED_BLE_OKIN_BED_FEET_UP},          {KEY_BLUE,  BED_BLE_OKIN_BED_FEET_DOWN},
  };
  
  key_commands_long_default = {
                                                                                                             {KEY_OFF,   SCENE_ALLOFF_FORCE},
  };

}
