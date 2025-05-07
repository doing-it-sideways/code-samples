/*****************************************************************************
  The structure that manages and manipulates actions. Also provides debug
  functionality.

  Author(s): Evan O'Bryant
  Copyright © 2024 DigiPen (USA) Corporation.    
*****************************************************************************/

#ifndef ACTIONLIST_H
#define ACTIONLIST_H

/*
  Wanted for the groups to block a specific set of groups (as long as it doesn't contain itself)
  rather than blocking subsequent actions inside an action's own group. Feels more natural to me.
*/

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/templates/hash_map.hpp>

namespace godot {

class Action;

class ActionList : public Node {
  GDCLASS(ActionList, Node)

protected:
  static void _bind_methods();

//-----funcs-----
public:
  ActionList() = default;
  ~ActionList() { Clear(); }

    //equivalent of unity's Update
  void _process(double dt) override;

    //add action to the list
  void Append(Ref<Action> action);
    //remove all actions immediately without finishing them
  void Clear();

  inline bool Empty() const { return actions.is_empty(); }
  inline int Size() const   { return actions.size(); }

  void SetPaused(bool pause);
  inline bool GetPaused() const { return isPaused; }

    //mark all actions as finished, set their percentDone to 100%.
    //works similarly to clear() but is safer to prevent weird and rare, but consistent, errors
  void ForceFinishAll();

    //clamp max speed to very high multiplier
  inline void SetSpeed(double speed) { speedMult = CLAMP(speed, 0, (double)(1 << 10)); }
  inline double GetSpeed() const     { return speedMult; }

    //debug mode keeps track of actions, stores their debug strings, and allows the user to print them
  inline void SetDebug(bool on) { debugEnabled = on; }
  inline bool GetDebug() const  { return debugEnabled; }

    //max amount of actions to insert into the debug string
  inline void SetDebugLineAmt(int64_t lines) { debug_lines = lines - 1; }

    //gives the user the debug info of the actions being processed
  String DebugString() const { return debug_info; }

private:
    //remove the groups that the action is blocking
  void remove_blocks(const Action* action);
    //add debug info of the action to the action list's debug string
  void accumulate_debug_info(String actionDebug);

//-----vars-----
public:
  static StringName* debug_header; //= "Total Actions Processing: {0}       Total Blocked: {1}\n";

private:
    //the actual action list
  Vector<Ref<Action>> actions;

    //when the value is 0, the group gets removed from the map, allowing actions in the group matching
    //the key to process
  HashMap<int64_t, int64_t> blockedGroups; //key = Action::ID, value = occurences

    //actionlist dt can be manipulated to speed up / slow down all actions
  double speedMult = 1.f;

    //information relating to outputting the state of the actionlist as a string
  String debug_info = "";
  int64_t debug_lines = 5;
  int64_t debug_linesShown = 0;
  int64_t debug_processedActions;

  bool debugEnabled = false, isPaused = false;
};

} // namespace godot

#endif // ACTIONLIST_H
