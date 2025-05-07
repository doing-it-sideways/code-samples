/*****************************************************************************
  The initialization and shutdown handling of the library. Registers
  classes with Godot and allocates some debug data used in the ActionList and
  Action classes due to not being able to use constexpr and weird hiccups
  with static variables.

  Author(s): Evan O'Bryant
  Copyright © 2024 DigiPen (USA) Corporation.    
*****************************************************************************/

#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "register_types.h"

#include "ActionList.h"
#include "Action.h"

#include "actions/TimedDestroy.h"
#include "actions/Fade.h"
#include "actions/Flip2D.h"
#include "actions/Transform.h"
#include "actions/Wait.h"
#include "actions/InOut.h"
#include "actions/Infinite.h"

using namespace godot;

  //initialize when godot boots up the scene, we dont need to boot up earlier
void init_action_module(ModuleInitializationLevel level) {
  if (level != MODULE_INITIALIZATION_LEVEL_SCENE)
    return;
  
  GDREGISTER_CLASS(ActionList);
    //allocate static data here so that we don't have to worry about it later
    //weird rules in gdextension prevent some values from being statically initialized non-dynamically
  ActionList::debug_header = memnew(StringName);
  *ActionList::debug_header = "Total Actions Processing: {0}       Total Blocked: {1}\n";

  GDREGISTER_ABSTRACT_CLASS(Action);
  Action::debug_format = memnew(StringName);
  //({id}) {action}: {status}, {description}. {time}s left.
  *Action::debug_format = "({0}) {1}: {2}, {3}. {4}s left.";

  GDREGISTER_CLASS(TimedDestroy);
  GDREGISTER_CLASS(Fade);
  GDREGISTER_CLASS(Flip2D);
  GDREGISTER_CLASS(Transform);
  GDREGISTER_CLASS(Wait);
  GDREGISTER_CLASS(InOut);
  GDREGISTER_CLASS(Infinite);
}

void uninit_action_module(ModuleInitializationLevel level) {
  if (level != MODULE_INITIALIZATION_LEVEL_SCENE)
    return;

  memdelete(ActionList::debug_header);
  memdelete(Action::debug_format);
}

  //avoid name mangling due to the .gdextension file needing to have access to this function's name
  //this is basically a copy paste from the docs
extern "C" {
GDExtensionBool GDE_EXPORT actionlib_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                                          const GDExtensionClassLibraryPtr p_lib,
                                          GDExtensionInitialization* r_initialization)
{
  godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_lib, r_initialization);

  init_obj.register_initializer(init_action_module);
  init_obj.register_terminator(uninit_action_module);
  init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

  return init_obj.init();
}
} //extern "C"
