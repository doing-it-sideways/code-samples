/*****************************************************************************
  The interface defining what an action is and what it contains. The ActionList
  class handles the manipulation of the action, which is why it's a friend class.

  Author(s): Evan O'Bryant
  Copyright © 2024 DigiPen (USA) Corporation.    
*****************************************************************************/

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "macros.h"
#include "Action.h"

using namespace godot;

StringName* Action::debug_format = nullptr;

  // Registers the methods with Godot. Required for all GDClasses
void Action::_bind_methods() {
  BIND_METHOD_NOARG(Action, GetNode);
  BIND_METHOD_NOARG(Action, TimeElapsed);
  BIND_METHOD_NOARG(Action, PercentDone);
  BIND_METHOD_NOARG(Action, IsDone);
  BIND_METHOD_NOARG(Action, ForceFinish);
  
  BIND_VIRTUAL_METHOD(Action, Reverse);
  BIND_VIRTUAL_METHOD(Action, Reversed);

  BIND_METHOD_1ARGD(Action, SetPause, "IsPaused", true);
  BIND_METHOD_NOARG(Action, GetPause);
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "paused"), TEXT(SetPause), TEXT(GetPause));

    //used a ton: provides a callback for when this action finishes
  ADD_SIGNAL(MethodInfo("action_finished"));
}

  // No node constructor
Action::Action(double _delay, double _dur, ID _group, IDArr blocks, bool oneshot)
  : duration(_dur)
  , delay(_delay)
  , isOneShot(oneshot)
  , blockGroups(blocks)
  , group(_group)
  , curDelay(_delay)
{}

  // Node constructor
Action::Action(Variant _node, double _delay, double _dur, Tween::EaseType _ease, Tween::TransitionType _trans,
               ID _group, IDArr blocks, bool oneshot)
  : node(_node)
  , type(bind_node(_node))
  , ease(_ease)
  , trans(_trans)
  , duration(_dur)
  , delay(_delay)
  , isOneShot(oneshot)
  , blockGroups(blocks)
  , group(_group)
  , curDelay(_delay)
{}

  // Copy constructor
Action::Action(Ref<Action> other, Variant _node)
  : node(_node)
  , type(bind_node(_node))
  , ease(other->ease)
  , trans(other->trans)
  , duration(other->duration)
  , delay(other->delay)
  , isOneShot(other->isOneShot)
  , blockGroups(other->blockGroups)
  , group(other->group)
  , curDelay(other->delay)
{}

  // Sets the internal node
Action::NodeType Action::bind_node(Variant _node) {
  Object* nodeObj = static_cast<Object*>(_node);

  Node2D* node2d = Object::cast_to<Node2D>(nodeObj);
  if (node2d) {
    node_internal = node2d;
    return NodeType::NODE2D;
  }

  Control* control = Object::cast_to<Control>(nodeObj);
  if (control) {
    node_internal = control;
    return NodeType::CONTROL;
  }

    //some actions do not need a node to be bound
  if (_node.get_type() != Variant::NIL)
    UtilityFunctions::printerr("Invalid node type supplied to an action.");
  
  node_internal = nullptr;
  return NodeType::NONE;
}

  // Updates time information for the action. dt passed in since it can be manipulated.
void Action::update_time(double dt) {
    //handle beginning delay
  if (curDelay > 0) {
    curDelay -= dt;

    if (curDelay > 0) {
      state = ProcessState::WAITING;
      return;
    }

      //compensate for delay overshooting after delay ends
    timeElapsed += -curDelay;
  }
  else //action is currently happening
    timeElapsed += dt;

  if (isOneShot) {
    state = ProcessState::DONE;
    return;
  }

    //action is finished
  if (timeElapsed >= duration) {
    timeElapsed = duration;
    state = ProcessState::DONE;
  }
  else
    state = ProcessState::DOING;
}

void Action::ForceFinish() {
  isDoneFirstUpdate = true;
  timeElapsed = duration;
}

void Action::Reverse() {
    //reverse current duration
  timeElapsed = duration - timeElapsed;

    //if still waiting, reverse waiting period
  if (curDelay > 0) {
    curDelay = delay - curDelay;
    state = ProcessState::WAITING;
  }
  else
    state = ProcessState::DOING;

    //make sure the init_process function gets called so easing looks normal
  isDoneFirstUpdate = false;
}

StringName Action::state_to_string(ProcessState state) {
  switch (state) {
    case ProcessState::BLOCKED: return "Blocked";
    case ProcessState::WAITING: return "Waiting";
    case ProcessState::DOING: return "Doing";
    case ProcessState::DONE: return "Done";
    default: return "!!Error!!";
  }
}
