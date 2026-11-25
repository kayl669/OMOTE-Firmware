#pragma once

#include <lvgl.h>
#include "applicationInternal/gui/guiMemoryOptimizer.h"

// used by memoryUsage.cpp
extern lv_obj_t* MemoryUsageLabel;
// used by guiStatusUpdate.cpp
extern lv_obj_t* BluetoothLabel;
extern lv_obj_t* BattPercentageLabel;
extern lv_obj_t* BattIconLabel;
// used by sceneHandler.cpp
extern lv_obj_t* SceneLabel;
// used by guiMemoryOptimizer.cpp
extern lv_style_t panel_style;
extern int tabviewTop;
extern int tabviewHeight;
extern int panelHeight;
// used by almost all gui_*.cpp
extern lv_color_t color_primary;
// button colors, dark enough to keep a white label readable
constexpr uint32_t BUTTON_RED    = 0xD32F2F;
constexpr uint32_t BUTTON_ORANGE = 0xE65100;
constexpr uint32_t BUTTON_AMBER  = 0xB7791F;
constexpr uint32_t BUTTON_GREEN  = 0x2E7D32;
constexpr uint32_t BUTTON_TEAL   = 0x00897B;
constexpr uint32_t BUTTON_CYAN   = 0x0088A3;
constexpr uint32_t BUTTON_BLUE   = 0x1976D2;
constexpr uint32_t BUTTON_INDIGO = 0x3949AB;
constexpr uint32_t BUTTON_PURPLE = 0x7B1FA2;
constexpr uint32_t BUTTON_PINK   = 0xC2185B;
constexpr uint32_t BUTTON_SLATE  = 0x546E7A;
// apply a color with a vertical gradient to a button
void setButtonColor(lv_obj_t* button, uint32_t color);

//#define drawRedBorderAroundMainWidgets
#ifdef drawRedBorderAroundMainWidgets
extern lv_style_t style_red_border;
#endif

// used by main.cpp
void init_gui(void);
// used by main.cpp and sceneHandler.cpp
void gui_loop(void);
// used by guiMemoryOptimizer.cpp
void tabview_content_is_scrolling_event_cb(lv_event_t* e);
void tabview_tab_changed_event_cb(lv_event_t* e);
void sceneLabel_or_pageIndicator_event_cb(lv_event_t* e);
void pageIndicator_navigate_event_cb(lv_event_t* e);
// used by sceneHandler.cpp
void guis_doTabCreationAfterGUIlistChanged(GUIlists newGUIlist);
void guis_doTabCreationForSpecificGUI(GUIlists GUIlist, int gui_list_index);
void guis_doTabCreationForNavigateToLastActiveGUIofPreviousGUIlist();
// used by guiMemoryOptimizer.cpp and sceneHandler.cpp
void setActiveTab(uint32_t index, lv_anim_enable_t anim_en, bool send_tab_changed_event = false);
// used by memoryUsage.cpp
void showMemoryUsageBar(bool showBar);
// used by commandHandler to show WiFi status
void showWiFiConnected(bool connected);
