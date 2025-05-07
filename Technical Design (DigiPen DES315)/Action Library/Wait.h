/*****************************************************************************
  The simplest example of an action. Just adds a group or set of groups to the
  block list of the action list, forcing them to wait _dur amount of time to
  process.

  Author(s): Evan O'Bryant
  Copyright © 2024 DigiPen (USA) Corporation.    
*****************************************************************************/

#ifndef WAIT_H
#define WAIT_H

#include "Action.h"
#include "macros.h"

namespace godot {

  //wait for a set duration and block certain actions in the meantime
class Wait : public Action {
  GDCLASS(Wait, Action)

protected:
  static void _bind_methods();

//-----funcs-----
public:
  Wait() = default;
  ~Wait() = default;

    // the constructor is defined but also an equivalent _new function with the 2nd
    // parameter as the appended name. In GDScript, calling the copy constructor would
    // look like: "Wait._newCopy(...)" while the first one would just be: "Wait._new(...)"
    // Godot cannot use C++ constructors from GDExtensions (as of Godot 4.2.X)
    // Implementations still unfortunately need to be defined manually
  CUSTOM_CONSTRUCTOR(Wait, , double _dur, ID _group, IDArr blocks);
  CUSTOM_CONSTRUCTOR(Wait, Copy, Ref<Wait> other);

  Action* Reversed() override;

  String get_debug_info() const override;

private:

};

} // namespace godot

#endif // WAIT_H
