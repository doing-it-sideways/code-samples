/*****************************************************************************
  The implementation of the actual action list that manages and manipulates
  actions.

  Author(s): Evan O'Bryant
  Copyright © 2024 DigiPen (USA) Corporation.    
*****************************************************************************/

#include "macros.h"
#include "ActionList.h"
#include "Action.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

StringName* ActionList::debug_header = nullptr;

  // Exposes all methods to Godot
void ActionList::_bind_methods() {
  BIND_METHOD_1ARG(ActionList, Append, "action");
  BIND_METHOD_NOARG(ActionList, Clear);
  BIND_METHOD_NOARG(ActionList, Empty);
  BIND_METHOD_NOARG(ActionList, Size);
  BIND_METHOD_NOARG(ActionList, ForceFinishAll);
  BIND_METHOD_NOARG(ActionList, DebugString);
  
    //bind properties with setters and getters
  BIND_METHOD_1ARGD(ActionList, SetPaused, "set_paused", true);
  BIND_METHOD_NOARG(ActionList, GetPaused);
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "paused", PropertyHint::PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), TEXT(SetPaused), TEXT(GetPaused));
  
  BIND_METHOD_1ARG(ActionList, SetSpeed, "set_speed");
  BIND_METHOD_NOARG(ActionList, GetSpeed);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed"), TEXT(SetSpeed), TEXT(GetSpeed));

  BIND_METHOD_1ARG(ActionList, SetDebug, "set_enabled");
  BIND_METHOD_NOARG(ActionList, GetDebug);
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug_mode"), TEXT(SetDebug), TEXT(GetDebug));

  BIND_METHOD_1ARG(ActionList, SetDebugLineAmt, "lines");

    //add a custom signal to process when the entire list has finished
  ADD_SIGNAL(MethodInfo("actions_finished"));
}

void ActionList::_process(double dt) {
  if (isPaused)
    return;

  dt *= speedMult;

    //reset accumulators for debug
  if (debugEnabled) {
    debug_processedActions = 0;
    debug_info = "";
    debug_linesShown = 0;
  }

    // For each action: remove action if it's finished or dead, skip if currently blocked,
    // process the next state of the action, then get some debug info.
  for (int i = 0; i < actions.size(); ++i) {
    auto action = actions[i];

      //if the action is attached to a node and the node is dead
    if (action->node.get_type() != Variant::NIL && !UtilityFunctions::is_instance_valid(action->node))
      action->isDead = true;

      //remove if non-existent or somehow finished early
    if (action->isDead || action->IsDone()) {
        //attempt to do cleanup
      if (!action->isDead)
        action->end_process();
      
      if (action->hasAddedBlocks)
        remove_blocks(*action);
      
      actions.remove_at(i--);
      
      if (actions.size() == 0)
        emit_signal("actions_finished");

      continue;
    }

      //skip if blocked including debug info
    if (blockedGroups.has(action->group))
      continue;
    
      //add block groups immediately
    if (!action->hasAddedBlocks) {
        //assume block groups doesn't have action's group
      for (auto group : action->blockGroups) {
        ++blockedGroups[group];
      }
      action->hasAddedBlocks = true;
    }

    action->update_time(dt);

      //make sure to always handle the last update
    if (action->state == Action::ProcessState::DOING || action->state == Action::ProcessState::DONE) {
      if (!action->isDoneFirstUpdate)
        action->init_process(); //assumes action sets the isDoneFirstUpdate flag

      action->process(dt);
    }

      //handle cleanup when finished
    if (action->state == Action::ProcessState::DONE) {
      action->end_process();
      remove_blocks(*action);
      actions.remove_at(i--);

      if (actions.size() == 0)
        emit_signal("actions_finished");
    }

      //add action's debug info to the debug string
    if (debugEnabled)
      accumulate_debug_info(action->get_debug_info());
  }

    //format debug string by adding header to the beginning
  if (debugEnabled && debug_processedActions != 0)
    debug_info = debug_header->format(Array::make(actions.size(), actions.size() - debug_processedActions)) + debug_info + "\n";
}

void ActionList::Append(Ref<Action> action) {
  actions.append(action);
}

  // Pause all actions in the list
void ActionList::SetPaused(bool pause) {
    //dont do repetitive work
  if (isPaused == pause)
    return;

  for (auto action : actions) {
    action->SetPause(pause);
  }

  isPaused = pause;
}

  // Clear all actions in the list
void ActionList::Clear() {
    //handle any necessary cleanup
  for (auto action : actions) {
    action->end_process();

    if (action->hasAddedBlocks)
      remove_blocks(*action);
  }

  actions.clear();
}

void ActionList::ForceFinishAll() {
  for (auto action : actions) {    
    action->ForceFinish();
  }
}

  // Remove the group block that the action had on the list
void ActionList::remove_blocks(const Action* action) {
  if (!action->hasAddedBlocks)
    return;

  for (auto group : action->blockGroups) {
    Action::ID& blockCount = blockedGroups[group];
    --blockCount;

    if (blockCount == 0)
      blockedGroups.erase(group);
  }
}

  // Set a string with information on the actions processed during the last update iteration.
void ActionList::accumulate_debug_info(String actionDebug) {
  ++debug_processedActions;
  
    //print full action info
  if (debug_linesShown < debug_lines) {
    debug_info += actionDebug + '\n';
    ++debug_linesShown;
  }
    //broken currently
    //print info regarding the amount of actions not being shown due to the debug_lines limit
  else if (debug_processedActions == debug_lines - 1) {
      //string op+ only handles 32bit
    int hiddenActions = int(actions.size() - debug_lines);

    if (hiddenActions != 0) {
      debug_info += "... (";
      debug_info += hiddenActions;
      debug_info += " actions hidden) ...\n";
    }

    debug_info += actionDebug + '\n';
  }
}
