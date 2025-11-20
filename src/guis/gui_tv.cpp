#include <lvgl.h>
#include "applicationInternal/gui/guiBase.h"
#include "applicationInternal/gui/guiRegistry.h"
#include "applicationInternal/commandHandler.h"
#include "devices/TV/device_sonyTV/device_sonyTV.h"
#include "devices/AVreceiver/device_boseAmp/device_boseAmp.h"
#include "devices/misc/device_home/device_home.h"
#include "guis/gui_tv.h"

#include "devices/mediaPlayer/device_samsungbluray/device_samsungbluray.h"

#ifndef dimof
    #define dimof(__array)     (sizeof(__array) / sizeof(__array[0]))
#endif

static const char * tab_names[] = {"TV", "Bose", "Maison", "Bluray"};
const int maxTab = 3;
int currentStartTab = 0;
lv_obj_t* tv_tabview = NULL;
lv_obj_t* tabContainer = NULL;

void fillTabTV(lv_obj_t *tab);
void fillTabBose(lv_obj_t *tab);
void fillTabHome(lv_obj_t *tab);
void fillTabBluray(lv_obj_t *tab);

void changed_event_cb(lv_event_t* e) {
    if (tabContainer == NULL) {
      return;
    }

    lv_obj_t * parent = lv_obj_get_parent(tabContainer);

    lv_obj_del(tabContainer);
    tabContainer = NULL;
    tabContainer = lv_obj_create(parent);
    lv_obj_set_size(tabContainer, SCR_WIDTH, tabviewHeight - 35);
    lv_obj_align(tabContainer, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(tabContainer, lv_color_black(), LV_PART_MAIN);

    int index = currentStartTab + lv_tabview_get_tab_act(tv_tabview);
    switch(index) {
      case 0: {
        fillTabTV(tabContainer);
        break;
      }
      case 1: {
        fillTabBose(tabContainer);
        break;
      }
      case 2: {
        fillTabHome(tabContainer);
        break;
      }
      case 3: {
        fillTabBluray(tabContainer);
        break;
      }
    }
}
void button_clicked_event_cb(lv_event_t* e) {
  int user_data = (intptr_t)(e->user_data);

  std::string payload = "";
  switch (user_data) {
    case 0: {
      executeCommand(SONY_POWER_TOGGLE);
      break;
    }
    case 1: {
      executeCommand(SONY_MENU);
      break;
    }
    case 2: {
      executeCommand(SONY_GUIDE);
      break;
    }
    case 3: {
      executeCommand(SONY_INPUT_TV);
      break;
    }
    case 4: {
      executeCommand(SONY_OPTIONS);
      break;
    }
    case 5: {
      executeCommand(SONY_INPUT_HDMI_1);
      break;
    }
    case 6: {
      executeCommand(SONY_INPUT_HDMI_2);
      break;
    }
    case 7: {
      executeCommand(SONY_INPUT_HDMI_3);
      break;
    }
    case 8: {
      executeCommand(SONY_INPUT_HDMI_4);
      break;
    }
    case 10: {
      executeCommand(SONY_NUM_0);
      break;
    }
    case 11: {
      executeCommand(SONY_NUM_1);
      break;
    }
    case 12: {
      executeCommand(SONY_NUM_2);
      break;
    }
    case 13: {
      executeCommand(SONY_NUM_3);
      break;
    }
    case 14: {
      executeCommand(SONY_NUM_4);
      break;
    }
    case 15: {
      executeCommand(SONY_NUM_5);
      break;
    }
    case 16: {
      executeCommand(SONY_NUM_6);
      break;
    }
    case 17: {
      executeCommand(SONY_NUM_7);
      break;
    }
    case 18: {
      executeCommand(SONY_NUM_8);
      break;
    }
    case 19: {
      executeCommand(SONY_NUM_9);
      break;
    }
    case 20: {
      executeCommand(BOSE_POWER_ON);
      break;
    }
    case 21: {
      executeCommand(BOSE_CBL_SAT);
      break;
    }
    case 22: {
      executeCommand(BOSE_BD_DVD);
      break;
    }
    case 23: {
      executeCommand(BOSE_GAME);
      break;
    }
    case 24: {
      executeCommand(BOSE_TV);
      break;
    }
    case 25: {
      executeCommand(BOSE_BLUETOOTH);
      break;
    }
    case 26: {
      executeCommand(BOSE_AUX);
      break;
    }
    case 27: {
      executeCommand(BOSE_INPUT);
      break;
    }
    case 28: {
      executeCommand(BOSE_SOURCE);
      break;
    }
    case 31: {
      payload +="open:" + cover_list_home[0];
      executeCommand(HOME_MQTT_COMMAND, payload);
      break;
    }
    case 32: {
      payload +="stop:" + cover_list_home[0];
      executeCommand(HOME_MQTT_COMMAND, payload);
      break;
    }
    case 33: {
      payload +="close:" + cover_list_home[0];
      executeCommand(HOME_MQTT_COMMAND, payload);
      break;
    }
    case 34: {
      payload +="open:" + cover_list_home[1];
      executeCommand(HOME_MQTT_COMMAND, payload);
      break;
    }
    case 35: {
      payload +="stop:" + cover_list_home[1];
      executeCommand(HOME_MQTT_COMMAND, payload);
      break;
    }
    case 36: {
      payload +="close:" + cover_list_home[1];
      executeCommand(HOME_MQTT_COMMAND, payload);
      break;
    }
    case 37: {
      payload +="open:";
      for (int i = 0; i < cover_list_home.size(); i++) {
        payload += cover_list_home[i];
        if (i < cover_list_home.size() - 1) {
          payload +=",";
        }
      }
      executeCommand(HOME_MQTT_COMMAND, payload);
      break;
    }
    case 38: {
      payload +="stop:";
      for (int i = 0; i < cover_list_home.size(); i++) {
        payload += cover_list_home[i];
        if (i < cover_list_home.size() - 1) {
          payload +=",";
        }
      }
      executeCommand(HOME_MQTT_COMMAND, payload);
      break;
    }
    case 39: {
      payload +="close:";
      for (int i = 0; i < cover_list_home.size(); i++) {
        payload += cover_list_home[i];
        if (i < cover_list_home.size() - 1) {
          payload +=",";
        }
      }
      executeCommand(HOME_MQTT_COMMAND, payload);
      break;
    }
    case 40: {
      payload +="open:" + cover_list_home[2];
      executeCommand(HOME_MQTT_COMMAND, payload);
      break;
    }
    case 41: {
      payload +="stop:" + cover_list_home[2];
      executeCommand(HOME_MQTT_COMMAND, payload);
      break;
    }
    case 42: {
      payload +="close:" + cover_list_home[2];
      executeCommand(HOME_MQTT_COMMAND, payload);
      break;
    }
    case 43: {
      payload +="open:" + cover_list_home[3];
      executeCommand(HOME_MQTT_COMMAND, payload);
      break;
    }
    case 44: {
      payload +="stop:" + cover_list_home[3];
      executeCommand(HOME_MQTT_COMMAND, payload);
      break;
    }
    case 45: {
      payload +="close:" + cover_list_home[3];
      executeCommand(HOME_MQTT_COMMAND, payload);
      break;
    }
    case 46: {
      executeCommand(HOME_HOOD_POWER);
      break;
    }
    case 47: {
      executeCommand(HOME_HOOD_PLUS);
      break;
    }
    case 48: {
      executeCommand(HOME_HOOD_MINUS);
      break;
    }
    case 49: {
      executeCommand(HOME_HOOD_FAN_MAX);
      break;
    }
    case 50: {
      executeCommand(HOME_HOOD_LIGHT);
      break;
    }
    case 51: {
      executeCommand(SAMSUNGBLURAY_POWER);
      break;
    }
    case 52: {
      executeCommand(SAMSUNGBLURAY_OPEN_CLOSE);
      break;
    }
    case 53: {
      executeCommand(SAMSUNGBLURAY_DISC_MENU);
      break;
    }
    case 54: {
      executeCommand(SAMSUNGBLURAY_HOME);
      break;
    }
    case 55: {
      executeCommand(SAMSUNGBLURAY_SUBTITLE);
      break;
    }
    case 56: {
      executeCommand(SAMSUNGBLURAY_TOOLS);
      break;
    }
    case 57: {
      executeCommand(SAMSUNGBLURAY_INFO);
      break;
    }
    case 100: {
      int index = lv_tabview_get_tab_act(tv_tabview);
      index--;
      if (index < 0) {
        index = 0;
        currentStartTab--;
      }
      if (currentStartTab < 0) {
        currentStartTab = 0;
      }
      for (int i = 0; i < maxTab; i++) {
        lv_tabview_rename_tab(tv_tabview, i, tab_names[currentStartTab + i]);
      }
      lv_tabview_set_act(tv_tabview, index, LV_ANIM_ON);
      changed_event_cb(e);
      break;
    }
    case 101: {
      int index = lv_tabview_get_tab_act(tv_tabview);
      index++;
      if (index == maxTab) {
        index = maxTab - 1;
        currentStartTab++;
      }
      if (currentStartTab >= dimof(tab_names) - maxTab) {
        currentStartTab = dimof(tab_names) - maxTab;
      }
      for (int i = 0; i < maxTab; i++) {
        lv_tabview_rename_tab(tv_tabview, i, tab_names[currentStartTab + i]);
      }
      lv_tabview_set_act(tv_tabview, index, LV_ANIM_ON);
      changed_event_cb(e);
      break;
    }
    default: {
      break;
    }
  }
}

void createButton(lv_obj_t *tab, uint8_t col, uint8_t col_span, uint8_t row, uint8_t row_span, std::string text, uint8_t user_data) {
  lv_obj_t* obj = lv_btn_create(tab);
  lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, col, col_span, LV_GRID_ALIGN_STRETCH, row, row_span);
  lv_obj_set_style_radius(obj, 7, LV_PART_MAIN);
  lv_obj_set_style_bg_color(obj, color_primary, LV_PART_MAIN);
  lv_obj_add_event_cb(obj, button_clicked_event_cb, LV_EVENT_CLICKED, (void *)(intptr_t)user_data);
#ifdef drawRedBorderAroundMainWidgets
  lv_obj_add_style(obj, &style_red_border, LV_PART_MAIN);
#endif
  lv_obj_t* label = lv_label_create(obj);
  lv_label_set_text(label, text.c_str());
  lv_obj_center(label);
}

void fillTabTV(lv_obj_t *tab) {
  static lv_coord_t col_dsc[] = {
    LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
    LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
    LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
    LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
    LV_GRID_TEMPLATE_LAST
  }; // equal x distribution
  static lv_coord_t row_dsc[] = {5, 30, 30, 30, 30, 30, 30, 30, 16, 30, 30, LV_GRID_TEMPLATE_LAST};
  // manual y distribution to compress the grid a bit
  lv_obj_set_style_pad_all(tab, 0, LV_PART_MAIN);
  lv_obj_set_style_shadow_width(tab, 0, LV_PART_MAIN);
  lv_obj_set_style_border_width(tab, 0, LV_PART_MAIN);
  lv_obj_set_style_grid_column_dsc_array(tab, col_dsc, 0);
  lv_obj_set_style_grid_row_dsc_array(tab, row_dsc, 0);
  lv_obj_set_layout(tab, LV_LAYOUT_GRID);

  lv_obj_t *menuLabel = lv_label_create(tab);
  lv_label_set_text(menuLabel, "");
  lv_obj_set_grid_cell(menuLabel, LV_GRID_ALIGN_STRETCH, 1, 16, LV_GRID_ALIGN_STRETCH, 0, 1);
  createButton(tab, 13, 3, 1, 1,  LV_SYMBOL_POWER, 0);

  createButton(tab, 3,  4, 2, 1,  "1", 11);
  createButton(tab, 7,  4, 2, 1,  "2", 12);
  createButton(tab, 11, 4, 2, 1,  "3", 13);
  createButton(tab, 3,  4, 3, 1,  "4", 14);
  createButton(tab, 7,  4, 3, 1,  "5", 15);
  createButton(tab, 11, 4, 3, 1,  "6", 16);
  createButton(tab, 3,  4, 4, 1,  "7", 17);
  createButton(tab, 7,  4, 4, 1,  "8", 18);
  createButton(tab, 11, 4, 4, 1,  "9", 19);
  createButton(tab, 7,  4, 5, 1,  "0", 10);

  createButton(tab, 1, 8, 6, 1,  "Menu", 1);
  createButton(tab, 9, 8, 6, 1,  "Guide", 2);
  createButton(tab, 1, 8, 7, 1,  "Tv", 3);
  createButton(tab, 9, 8, 7, 1,  "Options", 4);

  menuLabel = lv_label_create(tab);
  lv_label_set_text(menuLabel, "Hdmi");
  lv_obj_set_grid_cell(menuLabel, LV_GRID_ALIGN_STRETCH, 1, 16, LV_GRID_ALIGN_STRETCH, 8, 1);

  createButton(tab, 1,  4, 9, 1, "1", 5);
  createButton(tab, 5,  4, 9, 1, "2", 6);
  createButton(tab, 9,  4, 9, 1, "3", 7);
  createButton(tab, 13, 4, 9, 1, "4", 8);
  menuLabel = lv_label_create(tab);
  lv_label_set_text(menuLabel, "");
  lv_obj_set_grid_cell(menuLabel, LV_GRID_ALIGN_STRETCH, 1, 16, LV_GRID_ALIGN_STRETCH, 10, 1);
}

void fillTabBose(lv_obj_t *tab) {
  static lv_coord_t col_dsc[] = {
    0, LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
    LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), 0, LV_GRID_TEMPLATE_LAST
  }; // equal x distribution
  static lv_coord_t row_dsc[] = {5, 30, 30, 30, 30, 30, LV_GRID_TEMPLATE_LAST};
  // manual y distribution to compress the grid a bit
  lv_obj_set_align(tab, LV_ALIGN_TOP_MID);
  lv_obj_set_style_pad_all(tab, 0, LV_PART_MAIN);
  lv_obj_set_style_shadow_width(tab, 0, LV_PART_MAIN);
  lv_obj_set_style_border_width(tab, 0, LV_PART_MAIN);
  lv_obj_set_style_grid_column_dsc_array(tab, col_dsc, 0);
  lv_obj_set_style_grid_row_dsc_array(tab, row_dsc, 0);
  lv_obj_set_layout(tab, LV_LAYOUT_GRID);

  lv_obj_t *menuLabel = lv_label_create(tab);
  lv_label_set_text(menuLabel, "");
  lv_obj_set_grid_cell(menuLabel, LV_GRID_ALIGN_STRETCH, 1, 6, LV_GRID_ALIGN_STRETCH, 0, 1);

  createButton(tab, 5, 1, 1, 1, LV_SYMBOL_POWER, 20);
  createButton(tab, 1, 2, 2, 1, "Cable", 21);
  createButton(tab, 3, 2, 2, 1, "Bluray", 22);
  createButton(tab, 5, 2, 2, 1, "Jeux", 23);
  createButton(tab, 1, 2, 3, 1, "TV", 24);
  createButton(tab, 3, 2, 3, 1, LV_SYMBOL_BLUETOOTH, 34);
  createButton(tab, 5, 2, 3, 1, "Aux", 26);
  createButton(tab, 2, 2, 4, 1, "Entree", 27);
  createButton(tab, 4, 2, 4, 1, "Source", 28);
}

void fillTabHome(lv_obj_t *tab) {
  static lv_coord_t col_dsc[] = {0, LV_GRID_FR(1), 30, 30, 30, 30, 30, 0, LV_GRID_TEMPLATE_LAST};
  // equal x distribution
  static lv_coord_t row_dsc[] = {5, 30, 30, 30, 30, 30, 16, 30, 30, 30, LV_GRID_TEMPLATE_LAST};
  // manual y distribution to compress the grid a bit

  lv_obj_set_align(tab, LV_ALIGN_TOP_MID);
  lv_obj_set_style_pad_all(tab, 0, LV_PART_MAIN);
  lv_obj_set_style_shadow_width(tab, 0, LV_PART_MAIN);
  lv_obj_set_style_border_width(tab, 0, LV_PART_MAIN);
  lv_obj_set_style_grid_column_dsc_array(tab, col_dsc, 0);
  lv_obj_set_style_grid_row_dsc_array(tab, row_dsc, 0);
  lv_obj_set_layout(tab, LV_LAYOUT_GRID);

  lv_obj_t *menuLabel = lv_label_create(tab);
  lv_label_set_text(menuLabel, "");
  lv_obj_set_grid_cell(menuLabel, LV_GRID_ALIGN_STRETCH, 1, 7, LV_GRID_ALIGN_STRETCH, 0, 1);

  menuLabel = lv_label_create(tab);
  lv_label_set_text(menuLabel, "Salon");
  lv_obj_set_grid_cell(menuLabel, LV_GRID_ALIGN_STRETCH, 1, 2, LV_GRID_ALIGN_STRETCH, 1, 1);
  createButton(tab, 4, 1, 1, 1, LV_SYMBOL_UP, 31);
  createButton(tab, 5, 1, 1, 1, LV_SYMBOL_STOP, 32);
  createButton(tab, 6, 1, 1, 1, LV_SYMBOL_DOWN, 33);

  menuLabel = lv_label_create(tab);
  lv_label_set_text(menuLabel, "Salle");
  lv_obj_set_grid_cell(menuLabel, LV_GRID_ALIGN_STRETCH, 1, 2, LV_GRID_ALIGN_STRETCH, 2, 1);
  createButton(tab, 4, 1, 2, 1, LV_SYMBOL_UP, 34);
  createButton(tab, 5, 1, 2, 1, LV_SYMBOL_STOP, 35);
  createButton(tab, 6, 1, 2, 1, LV_SYMBOL_DOWN, 36);

  menuLabel = lv_label_create(tab);
  lv_label_set_text(menuLabel, "Cuisine");
  lv_obj_set_grid_cell(menuLabel, LV_GRID_ALIGN_STRETCH, 1, 2, LV_GRID_ALIGN_STRETCH, 3, 1);
  createButton(tab, 4, 1, 3, 1, LV_SYMBOL_UP, 40);
  createButton(tab, 5, 1, 3, 1, LV_SYMBOL_STOP, 41);
  createButton(tab, 6, 1, 3, 1, LV_SYMBOL_DOWN, 42);

  menuLabel = lv_label_create(tab);
  lv_label_set_text(menuLabel, "Store Cuisine");
  lv_obj_set_grid_cell(menuLabel, LV_GRID_ALIGN_STRETCH, 1, 3, LV_GRID_ALIGN_STRETCH, 4, 1);
  createButton(tab, 4, 1, 4, 1, LV_SYMBOL_UP, 43);
  createButton(tab, 5, 1, 4, 1, LV_SYMBOL_STOP, 44);
  createButton(tab, 6, 1, 4, 1, LV_SYMBOL_DOWN, 45);

  menuLabel = lv_label_create(tab);
  lv_label_set_text(menuLabel, "Tous");
  lv_obj_set_grid_cell(menuLabel, LV_GRID_ALIGN_STRETCH, 1, 2, LV_GRID_ALIGN_STRETCH, 5, 1);
  createButton(tab, 4, 1, 5, 1, LV_SYMBOL_UP, 37);
  createButton(tab, 5, 1, 5, 1, LV_SYMBOL_STOP, 38);
  createButton(tab, 6, 1, 5, 1, LV_SYMBOL_DOWN, 39);

  menuLabel = lv_label_create(tab);
  lv_label_set_text(menuLabel, "Hotte");
  lv_obj_set_grid_cell(menuLabel, LV_GRID_ALIGN_STRETCH, 1, 7, LV_GRID_ALIGN_STRETCH, 6, 1);

  createButton(tab, 2, 1, 7, 1, LV_SYMBOL_POWER, 46);
  createButton(tab, 3, 1, 7, 1, LV_SYMBOL_PLUS, 47);
  createButton(tab, 4, 1, 7, 1, LV_SYMBOL_MINUS, 48);
  createButton(tab, 5, 2, 7, 1, "Max", 49);
  createButton(tab, 3, 3, 8, 1, "Lumiere", 50);
  menuLabel = lv_label_create(tab);
  lv_label_set_text(menuLabel, "");
  lv_obj_set_grid_cell(menuLabel, LV_GRID_ALIGN_STRETCH, 1, 7, LV_GRID_ALIGN_STRETCH, 9, 1);
}

void fillTabBluray(lv_obj_t *tab) {
  static lv_coord_t col_dsc[] = {
    0, LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), 0, LV_GRID_TEMPLATE_LAST
  }; // equal x distribution
  static lv_coord_t row_dsc[] = {5, 30, 30, 30, 30, 30, LV_GRID_TEMPLATE_LAST};
  // manual y distribution to compress the grid a bit
  lv_obj_set_align(tab, LV_ALIGN_TOP_MID);
  lv_obj_set_style_pad_all(tab, 0, LV_PART_MAIN);
  lv_obj_set_style_shadow_width(tab, 0, LV_PART_MAIN);
  lv_obj_set_style_border_width(tab, 0, LV_PART_MAIN);
  lv_obj_set_style_grid_column_dsc_array(tab, col_dsc, 0);
  lv_obj_set_style_grid_row_dsc_array(tab, row_dsc, 0);
  lv_obj_set_layout(tab, LV_LAYOUT_GRID);

  lv_obj_t *menuLabel = lv_label_create(tab);
  lv_label_set_text(menuLabel, "");
  lv_obj_set_grid_cell(menuLabel, LV_GRID_ALIGN_STRETCH, 1, 4, LV_GRID_ALIGN_STRETCH, 0, 1);

  createButton(tab, 4, 1, 1, 1, LV_SYMBOL_POWER, 51);
  createButton(tab, 1, 2, 2, 1, LV_SYMBOL_EJECT, 52);
  createButton(tab, 3, 2, 2, 1, "Disque", 53);
  createButton(tab, 1, 2, 3, 1, LV_SYMBOL_HOME, 54);
  createButton(tab, 3, 2, 3, 1, "Sous-titre", 55);
  createButton(tab, 1, 2, 4, 1, "Outils", 56);
  createButton(tab, 3, 2, 4, 1, "Info", 57);
}

void notify_tab_before_delete_tv(void) {
}

void create_tab_content_tv(lv_obj_t* tab) {
  lv_obj_set_style_pad_all(tab, 0, LV_PART_MAIN);
  lv_obj_set_style_shadow_width(tab, 0, LV_PART_MAIN);
  lv_obj_set_style_border_width(tab, 0, LV_PART_MAIN);
  lv_obj_clear_flag(tab, LV_OBJ_FLAG_SCROLLABLE);

  tv_tabview = lv_tabview_create(tab, LV_DIR_TOP, 35);
  lv_obj_add_event_cb(tv_tabview, changed_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

  lv_obj_align(tv_tabview, LV_ALIGN_TOP_LEFT, 35, tabviewHeight - 35);
  lv_obj_set_size(tv_tabview, SCR_WIDTH - 35 - 35, tabviewHeight - 35);
  lv_obj_align(tab, LV_ALIGN_TOP_LEFT, 0, 0);

  {
    lv_obj_t* prev = lv_btn_create(tab);
    lv_obj_set_style_bg_color(prev, color_primary, LV_PART_MAIN);
    lv_obj_set_style_radius(prev, 0, LV_PART_MAIN);
    lv_obj_add_event_cb(prev, button_clicked_event_cb, LV_EVENT_CLICKED, (void *)(intptr_t)100);
    lv_obj_set_size(prev, 35, 35);
    lv_obj_align(prev, LV_ALIGN_TOP_LEFT, 0, tabviewHeight - 35);
#ifdef drawRedBorderAroundMainWidgets
    lv_obj_add_style(prev, &style_red_border, LV_PART_MAIN);
#endif
    lv_obj_t* label = lv_label_create(prev);
    lv_label_set_text(label, LV_SYMBOL_LEFT);
    lv_obj_center(label);
  }

  {
    lv_obj_t* next = lv_btn_create(tab);
    lv_obj_set_style_bg_color(next, color_primary, LV_PART_MAIN);
    lv_obj_set_style_radius(next, 0, LV_PART_MAIN);
    lv_obj_add_event_cb(next, button_clicked_event_cb, LV_EVENT_CLICKED, (void *)(intptr_t)101);
    lv_obj_set_size(next, 35, 35);
    lv_obj_align(next, LV_ALIGN_TOP_LEFT, SCR_WIDTH - 35, tabviewHeight - 35);
#ifdef drawRedBorderAroundMainWidgets
    lv_obj_add_style(next, &style_red_border, LV_PART_MAIN);
#endif
    lv_obj_t* label = lv_label_create(next);
    lv_label_set_text(label, LV_SYMBOL_RIGHT);
    lv_obj_center(label);
  }

  for (int i = currentStartTab; i < dimof(tab_names) && i < currentStartTab + maxTab; i++) {
    lv_tabview_add_tab(tv_tabview, tab_names[i]);
  }

  lv_obj_t * btns = lv_tabview_get_tab_btns(tv_tabview);

  lv_obj_set_style_text_color(btns, lv_color_white(), LV_PART_ITEMS | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(btns, lv_color_white(), LV_PART_ITEMS | LV_STATE_FOCUSED);
  lv_obj_set_style_text_color(btns, lv_color_white(), LV_PART_ITEMS | LV_STATE_CHECKED);

  lv_obj_set_style_bg_color(btns, color_primary, LV_PART_ITEMS | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(btns, lv_color_white(), LV_PART_ITEMS | LV_STATE_FOCUSED);
  lv_obj_set_style_bg_color(btns, lv_color_white(), LV_PART_ITEMS | LV_STATE_CHECKED);

  lv_obj_set_style_bg_color(btns, lv_color_black(), 0);
  lv_obj_set_style_text_color(btns, color_primary, 0);
  lv_obj_set_style_border_side(btns, LV_BORDER_SIDE_NONE, LV_PART_ITEMS | LV_STATE_CHECKED);

  tabContainer = lv_obj_create(tab);
  lv_obj_set_size(tabContainer, SCR_WIDTH, tabviewHeight - 35);
  lv_obj_align(tabContainer, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_style_bg_color(tabContainer, lv_color_black(), LV_PART_MAIN);
  currentStartTab = 0;
  fillTabTV(tabContainer);
  lv_tabview_set_act(tv_tabview, 0, LV_ANIM_ON);
}

void register_gui_tv(void){
  register_gui(std::string(tabName_tv), & create_tab_content_tv, & notify_tab_before_delete_tv);
}
