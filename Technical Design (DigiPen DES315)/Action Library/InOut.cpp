/*****************************************************************************
  An action that handles an action. It will perform an action, and then perform
  the action again, just reversed.

  Author(s): Evan O'Bryant
  Copyright © 2024 DigiPen (USA) Corporation.    
*****************************************************************************/

#include "InOut.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void InOut::_bind_methods() {
  BIND_SMETHOD_ARGS(get_class_static(), D_METHOD(TEXT(_new), "action", "reverseSpeedMult"), static_cast<cptr_>(_new), DEFVAL(Ref<Action>{}), DEFVAL(1.f));
}

InOut::InOut(Ref<Action> _action, double _reverseSpeedMult)
  : Action(_action->delay, _action->duration + _action->duration * _reverseSpeedMult + _action->delay, _action->group, _action->blockGroups, false)
  , action(_action)
  , reverse(_action->Reversed())
  , swapTime(_action->delay + _action->duration)
  , reverseSpeedMult(_reverseSpeedMult)
{
  reverse->timeElapsed = 0;
}

InOut* InOut::_new(Ref<Action> _action, double _reverseSpeedMult) {
  return memnew(InOut(_action, _reverseSpeedMult));
}

  //partial copy of ActionList::update but doubled
void InOut::process(double dt) {
  if (!UtilityFunctions::is_instance_valid(action->node)) {
    isDead = true;
    return;
  }

  const double timeElapsed = TimeElapsed();

    //process first ("In") action
  if (timeElapsed < swapTime) {
    action->update_time(dt);

    if (action->state != ProcessState::WAITING) {
      if (!action->isDoneFirstUpdate)
        action->init_process();
      
      action->process(dt);

        // vvv NOTE: I never needed to fix this. -- Evan 2025 vvv
        // fix later? probably will not trigger ever because of timing logic
        // although not currently a problem as most actions do not utilize end_process
      if (action->state == ProcessState::DONE)
        action->end_process();
    }
  }
    //process second ("Out") action
  else if (timeElapsed >= swapTime) {
    reverse->update_time(dt * reverseSpeedMult);

      //can skip other checks because InOut is an action itself
    if (reverse->state != ProcessState::WAITING) {
      if (!reverse->isDoneFirstUpdate)
        reverse->init_process();
      
      reverse->process(dt * reverseSpeedMult);
    }
  }
}

void InOut::end_process() {
  reverse->end_process();
  Action::end_process();
}

String InOut::get_debug_info() const {
  auto id = get_instance_id();
  String statestr = state_to_string(state);
  float timeLeft = duration - TimeElapsed();

  String inOutInfo, innerAction;
  if (timeElapsed < swapTime) {
    inOutInfo = "InOut: In";
    innerAction = '{' + action->get_debug_info() + '}' + "\n{" + reverse->get_debug_info() + '}';
  }
  else {
    inOutInfo = "InOut: Out";
    innerAction = '{' + reverse->get_debug_info() + '}';
  }

  return debug_format->format(Array::make(id, inOutInfo, statestr, innerAction, timeLeft));
}
