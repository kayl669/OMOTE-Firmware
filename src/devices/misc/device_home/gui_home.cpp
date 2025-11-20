#include <lvgl.h>
#include "applicationInternal/gui/guiBase.h"
#include "applicationInternal/gui/guiRegistry.h"
#include "applicationInternal/scenes/sceneRegistry.h"
#include "applicationInternal/commandHandler.h"
#include "applicationInternal/keys.h"
#include "devices/misc/device_home/gui_home.h"
#include "devices/misc/device_home/device_home.h"
#include "scenes/scene__default.h"

static lv_obj_t* sliderLivingRoom;
static lv_obj_t* sliderKitchen;
static lv_obj_t* sliderAll;

static int32_t sliderLivingRoomValue = 0;
static int32_t sliderKitchenValue = 0;
static int32_t sliderAllValue = 0;

uint16_t GUI_HOME_ACTIVATE;

std::map<char, repeatModes> key_repeatModes_home = {};
std::map<char, uint16_t> key_commands_short_home = {};
std::map<char, uint16_t> key_commands_long_home = {};

// Smart Home Slider Event handler
static void homeSlider_event_cb(lv_event_t* e){
  lv_obj_t* slider = lv_event_get_target(e);
  std::string payload_str(std::to_string(100 - lv_slider_get_value(slider)));
  // Publish an MQTT message based on the event user data
  #if (ENABLE_WIFI_AND_MQTT == 1)
  int user_data = *((int*)(&(e->user_data)));
  std::string payload = "";
  switch(user_data) {
    case 1: {
      payload +="set_position:" + cover_list_home[0] + ":" + payload_str;
      executeCommand(HOME_MQTT_COMMAND, payload);
      break;
    }
    case 2: {
      payload +="set_position:" + cover_list_home[3] + ":" + payload_str;
      executeCommand(HOME_MQTT_COMMAND, payload);
      break;
    }
    case 3: {
      payload +="set_position:";
      for (int i = 0; i < cover_list_home.size(); i++) {
        payload += cover_list_home[i];
        if (i < cover_list_home.size() - 1) {
          payload +=",";
        }
      }
      payload += ":" + payload_str;
      executeCommand(HOME_MQTT_COMMAND, payload);
      break;
    }
  }
  #endif
}

void createSlider(lv_obj_t *tab, lv_obj_t *&slider, int32_t *slidervalue, std::string text, uint8_t user_data) {
  lv_obj_t *menuLabel = lv_label_create(tab);
  lv_label_set_text(menuLabel, text.c_str());

  lv_obj_set_size(menuLabel, lv_pct(100), 79);
  lv_obj_set_style_bg_color(menuLabel, color_primary, LV_PART_MAIN);
  lv_obj_set_style_border_width(menuLabel, 0, LV_PART_MAIN);

  slider = lv_slider_create(menuLabel);
  lv_slider_set_range(slider, 0, 100);
  lv_obj_set_style_bg_color(slider, lv_color_lighten(lv_color_black(), 30), LV_PART_INDICATOR);
  lv_obj_set_style_bg_color(slider, lv_color_white(), LV_PART_KNOB);
  lv_obj_set_style_bg_opa(slider, 255, LV_PART_MAIN);
  lv_obj_set_style_bg_color(slider, lv_color_lighten(color_primary, 50), LV_PART_MAIN);
  lv_slider_set_value(slider, *slidervalue, LV_ANIM_OFF);
  lv_obj_set_size(slider, lv_pct(90), 10);
  lv_obj_align(slider, LV_ALIGN_TOP_MID, 0, 37);
  lv_obj_add_event_cb(slider, homeSlider_event_cb, LV_EVENT_VALUE_CHANGED, (void *)(intptr_t)user_data);
}

void create_tab_content_home(lv_obj_t* tab) {

  // Add content to the smart home tab
  lv_obj_set_layout(tab, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_scrollbar_mode(tab, LV_SCROLLBAR_MODE_ACTIVE);

  // Add a label, then a box for the light controls
  lv_obj_t *menuLabel;
  createSlider(tab, sliderLivingRoom, &sliderLivingRoomValue, "Salon", 1);
  createSlider(tab, sliderKitchen, &sliderKitchenValue, "Cuisine", 2);
  createSlider(tab, sliderAll, &sliderAllValue, "Tous", 3);
}

void notify_tab_before_delete_home(void) {
  // remember to set all pointers to lvgl objects to NULL if they might be accessed from outside.
  // They must check if object is NULL and must not use it if so
  sliderLivingRoomValue = lv_slider_get_value(sliderLivingRoom);
  sliderKitchenValue = lv_slider_get_value(sliderKitchen);
  sliderAllValue = lv_slider_get_value(sliderAll);
}

void gui_setKeys_home() {
  key_commands_short_home = {
    {KEY_STOP, SCENE_SELECTION},
  };
}

void register_gui_home(void){

  register_gui(
    std::string(tabName_home),
    & create_tab_content_home,
    & notify_tab_before_delete_home,
    & gui_setKeys_home,
    & key_repeatModes_home,
    & key_commands_short_home,
    & key_commands_long_home
    );

  register_command(&GUI_HOME_ACTIVATE, makeCommandData(GUI, {std::to_string(MAIN_GUI_LIST), std::string(tabName_home)}));
}
