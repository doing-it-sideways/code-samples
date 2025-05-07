/*****************************************************************************
  The interface defining what an action is and what it contains. The ActionList
  class handles the manipulation of the action, which is why it's a friend class.
  See Wait.h/cpp for the simplest example of an action.

  Author(s): Evan O'Bryant
  Copyright © 2024 DigiPen (USA) Corporation.    
*****************************************************************************/

#ifndef ACTION_H
#define ACTION_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/tween.hpp>
#include <godot_cpp/variant/packed_int64_array.hpp>

namespace godot {

/*
  Since there can't be pure virtual functions, many things have a default implementation.
*/

class Action : public RefCounted {
  // This macro is required for all GD classes. It expands to hundreds of lines of boilerplate code.
  GDCLASS(Action, RefCounted)

protected:
  static void _bind_methods();

//-----structs and typedefs-----
public:
    //objects that can manage an action
  friend class ActionList; //to easily read without get-sets
  friend class InOut;      //although a child, needs to be treated like a mini action list
  friend class Infinite;   //although a child, needs to be treated like a mini action list

  using ID = int64_t;
  using IDArr = PackedInt64Array;

  // Node2D and Control have different functions to manipulate data, so they must be handled separately.
  enum NodeType {
    NONE,
    NODE2D,
    CONTROL
  };

  enum ProcessState {
    BLOCKED = 0,
    WAITING = 1,
    DOING = 2,
    DONE = 3
  };

//-----funcs-----
public:
  Action() = default;
  virtual ~Action() = default;

  void update_time(double dt);

    //done before the first process call
  virtual void init_process()     { isDoneFirstUpdate = true; }
  virtual void process(double dt) {}
    //done once the process state is done -- emits signal for external code to be notified when action finishes
  virtual void end_process()      { emit_signal("action_finished"); }

    //converts the process state enum to a string
  static StringName state_to_string(ProcessState state);

    //node can either be a Node2D or a Control node. invalid otherwise
    //mainly here so that external code can get the underlying node, the internal library
    //uses a void* along with the enum to get the node faster
  Variant GetNode() const { return node; }

  inline double TimeElapsed() const { return timeElapsed; }
  inline double PercentDone() const { return timeElapsed / duration; }
  inline bool IsDone() const        { return PercentDone() >= 1.0; }

    //force the time elapsed to equal duration
  void ForceFinish();

  inline void SetPause(bool pause) { isPaused = pause; }
  inline bool GetPause() const     { return isPaused; }

    //reverses an action in place
  virtual void Reverse();
    //Pure virtual: returns an action identical to the current one, just reversed
  virtual Action* Reversed() { return this; }

    //Pure virtual: returns the current info about an action.
    //returns a formatted version of the debug_format string. (see line 100)
    //ex: (-9422434343435) Action: Waiting, hi! im an action!. 4.243434s left.
  virtual String get_debug_info() const { return ""; }

protected:
    //each action needs to provide a delay, a duration, a group, block groups, and if it's a oneshot action
    //for actual actions, every single parameter has a default value inside of godot, but not in the library
    //see each action's .h and bind_methods function along with macros.h to see how a constructor gets bound
  Action(double _delay, double _dur, ID _group, IDArr blocks, bool oneshot);
  Action(Variant _node, double _delay, double _dur, Tween::EaseType _ease, Tween::TransitionType _trans,
         ID _group, IDArr blocks, bool oneshot);
  Action(Ref<Action> other, Variant _node);

  inline NodeType get_type() const { return type; }

private:
    //binds the internal node pointer along with the type variable to avoid frequent dynamic casting
  NodeType bind_node(Variant node);

//-----vars-----
public:
  Variant node;
  static StringName* debug_format; //({id}) {action}: {status}, {description}. {time}s left.

protected:
    //the node as a void pointer so it can be reinterpret casted to the right type quickly
  void* node_internal;
  const NodeType type = NodeType::NONE;

  const double duration{}; //default-init due to GDCLASS requiring a default constructor
  const double delay{};

  ProcessState state = ProcessState::WAITING;
  Tween::EaseType ease;
  Tween::TransitionType trans;
  
  bool isOneShot, isDoneFirstUpdate = false, isPaused = false;
  bool isDead = false; //node no longer exists -> can't update

private:
  IDArr blockGroups = {}; //the groups the action will block once placed into an actionlist
  ID group; //the group this action belongs to. this group should not be contained in blockGroups

    //how much time has passed and how much time is left in the delay.
  double curDelay, timeElapsed = 0;

    //if the action has added it's blocks to an actionlist.
    //future goal: allow action to be added to multiple actionlists without blocking issues
  bool hasAddedBlocks = false;
};

} // namespace godot

#endif // ACTION_H