/*****************************************************************************
  An action that handles an action. It will perform an action, and then perform
  the action again, just reversed.

  Author(s): Evan O'Bryant
  Copyright © 2024 DigiPen (USA) Corporation.    
*****************************************************************************/

#ifndef INOUT_H
#define INOUT_H

#include "Action.h"
#include "macros.h"

namespace godot {

  //do an action once, then do the reverse of that action once
class InOut : public Action {
  GDCLASS(InOut, Action)

protected:
  static void _bind_methods();

//-----funcs-----
public:
    //Infinite uses InOut internally -- process/end_process, action refs
  friend class Infinite;

  InOut() = default;
  ~InOut() = default;

  CUSTOM_CONSTRUCTOR(InOut, , Ref<Action> _action, double _reverseSpeedMult);

private:
  void process(double dt) override;
  void end_process() override;
  String get_debug_info() const override;

//-----vars-----
private:
  Ref<Action> action, reverse;
  
  const double swapTime{};
  const double reverseSpeedMult{};
};

} // namespace godot

#endif // INOUT_H
