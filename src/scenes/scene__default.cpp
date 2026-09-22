#include <map>
#include "applicationInternal/keys.h"
#include "applicationInternal/scenes/sceneRegistry.h"
#include "applicationInternal/commandHandler.h"
// devices
#include "devices/TV/device_samsungTV/device_samsungTV.h"
// scenes
#include "scene__default.h"
#include "scenes/scene_allOff.h"

uint16_t SCENE_SELECTION;
std::string scene_name_selection = "sceneSelection";
uint16_t SCENE_BACK_TO_PREVIOUS_GUI_LIST;
std::string scene_back_to_previous_gui_list = "backToPreviousList";
uint16_t GUI_PREV;
std::string scene_gui_prev = "GUI_prev";
uint16_t GUI_NEXT;
std::string scene_gui_next = "GUI_next";

std::map<char, repeatModes> key_repeatModes_default;
std::map<char, uint16_t> key_commands_short_default;
std::map<char, uint16_t> key_commands_long_default;

// This is the main list of guis we want to be shown when swiping. Need not to be all the guis that have been registered, can be only a subset.
// You can swipe through these guis. Will be in the order you place them here in the vector.
// By default, it is a list of the guis that have been registered in main.cpp
// If a scene defines a scene specific gui list, this will be used instead as long as the scene is active and we don't explicitely navigate back to main_gui_list
t_gui_list main_gui_list;

void register_scene_defaultKeys(void) {
  key_repeatModes_default = {
                                                                                                             {KEY_OFF,   SHORT            },
    {KEY_STOP,  SHORT            },    {KEY_REWI,  SHORTorLONG      },    {KEY_PLAY,  SHORT            },    {KEY_FORW,  SHORTorLONG      },
    {KEY_CONF,  SHORT            },                                                                          {KEY_INFO,  SHORT            },
                                                         {KEY_UP,    SHORT            },
                      {KEY_LEFT,  SHORT            },    {KEY_OK,    SHORT            },    {KEY_RIGHT, SHORT            },
                                                         {KEY_DOWN,  SHORT            },
    {KEY_BACK,  SHORT            },                                                                          {KEY_SRC,   SHORT            },
    {KEY_VOLUP, SHORT_REPEATED   },                      {KEY_MUTE,  SHORT            },                     {KEY_CHUP,  SHORT            },
    {KEY_VOLDO, SHORT_REPEATED   },                      {KEY_REC,   SHORT            },                     {KEY_CHDOW, SHORT            },
    {KEY_RED,   SHORT            },    {KEY_GREEN, SHORT            },    {KEY_YELLO, SHORT            },    {KEY_BLUE,  SHORT            },
  };
  
  key_commands_short_default = {
                                                                                                             {KEY_OFF,   SAMSUNG_POWER_TOGGLE},
    {KEY_STOP,  SAMSUNG_STOP        },  {KEY_REWI,  SAMSUNG_REWIND      },  {KEY_PLAY,   SAMSUNG_PAUSE       },  {KEY_FORW,   SAMSUNG_FASTFORWARD },
    {KEY_CONF,  SAMSUNG_MENU        },                                                                    {KEY_INFO,  SAMSUNG_INFO       },
                                                     {KEY_UP,    SAMSUNG_UP           },
                        {KEY_LEFT,  SAMSUNG_LEFT    },  {KEY_OK,    SAMSUNG_SELECT       },    {KEY_RIGHT, SAMSUNG_RIGHT    },
                                                     {KEY_DOWN,  SAMSUNG_DOWN         },
    {KEY_BACK,   SAMSUNG_RETURN    },                                                                    {KEY_SRC,   SAMSUNG_MENU     },
    {KEY_VOLUP, SAMSUNG_VOL_PLUS  },                      {KEY_MUTE,  SAMSUNG_MUTE_TOGGLE},                  {KEY_CHUP,  SAMSUNG_CHANNEL_UP  },
    {KEY_VOLDO, SAMSUNG_VOL_MINUS },                      {KEY_REC,   SCENE_BACK_TO_PREVIOUS_GUI_LIST  },   {KEY_CHDOW, SAMSUNG_CHANNEL_DOWN  },
    {KEY_RED,   SAMSUNG_KEY_A   },      {KEY_GREEN, SAMSUNG_KEY_B},         {KEY_YELLO, SAMSUNG_KEY_C},          {KEY_BLUE,  SAMSUNG_KEY_D},
  };
  
  key_commands_long_default = {
  
  
  };

  register_command(&SCENE_SELECTION                , makeCommandData(SCENE, {scene_name_selection}));
  register_command(&SCENE_BACK_TO_PREVIOUS_GUI_LIST, makeCommandData(SCENE, {scene_back_to_previous_gui_list}));
  register_command(&GUI_PREV                       , makeCommandData(SCENE, {scene_gui_prev}));
  register_command(&GUI_NEXT                       , makeCommandData(SCENE, {scene_gui_next}));

}
