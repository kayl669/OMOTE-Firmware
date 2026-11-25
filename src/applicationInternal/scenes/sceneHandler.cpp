#include <string>
#include "applicationInternal/scenes/sceneHandler.h"
#include "applicationInternal/scenes/sceneRegistry.h"
#include "applicationInternal/hardware/hardwarePresenter.h"
#include "applicationInternal/omote_log.h"

void handleScene(uint16_t command, commandData commandData, std::string additionalPayload) {

  auto current = commandData.commandPayloads.begin();
  std::string scene_name = *current;

  // FORCE can be either as second payload in commandData
  // e.g. register_command(&SCENE_TV_FORCE  , makeCommandData(SCENE, {scene_name_TV, "FORCE"}));
  // or as additionalPayload
  // e.g. executeCommand(activate_scene_command, "FORCE");

  // we can have a second payload
  std::string isForcePayload = "";
  ++current;
  if (current != commandData.commandPayloads.end()) {
    isForcePayload = *current;
  }

  // check if we know the new scene
  if (!sceneExists(scene_name)) {
    omote_log_w("scene: cannot start scene %s, because it is unknown\r\n", scene_name.c_str());
    return;
  } else {
    omote_log_d("scene: will switch from old scene %s to new scene %s\r\n", get_activeScene().c_str(), scene_name.c_str());
  }

  // do not activate the same scene again, only when forced to do so (e.g. when selected by hardware key)
  bool callEndAndStartSequences;
  if ((scene_name == get_activeScene()) && ((isForcePayload != "FORCE") && (additionalPayload != "FORCE"))) {
    omote_log_d("scene: will not start scene again, because it is already active\r\n");
    callEndAndStartSequences = false;
  } else if ((scene_name == get_activeScene()) && ((isForcePayload == "FORCE") || (additionalPayload == "FORCE"))) {
    omote_log_d("scene: scene is already active, but FORCE was set, so start scene again\r\n");
    callEndAndStartSequences = true;
  } else {
    // this is the default when switching to a different scene
    callEndAndStartSequences = true;
  }

  if (callEndAndStartSequences) {
    // end old scene
    if (!sceneExists(get_activeScene()) && (get_activeScene() != "")) {
      omote_log_w("scene: WARNING: cannot end scene %s, because it is unknown\r\n", get_activeScene().c_str());
  
    } else {
      if (get_activeScene() != "") {
        omote_log_d("scene: will call end sequence for scene %s\r\n", get_activeScene().c_str());
        scene_end_sequence_from_registry(get_activeScene());
      }
  
    }

    // start new scene
    omote_log_d("scene: will call start sequence for scene %s\r\n", scene_name.c_str());
    scene_start_sequence_from_registry(scene_name);
  }

  set_activeScene(scene_name);

  omote_log_d("scene: scene handling finished, new scene %s is active\r\n", get_activeScene().c_str());
}
