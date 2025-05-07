#include "Wait.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Wait::_bind_methods() {
    //i didn't make a macro for a class without a node
  BIND_SMETHOD_ARGS(get_class_static(), D_METHOD(TEXT(_new), "duration", "group", "blocks"),
                    static_cast<cptr_>(_new), DEFVAL(0.f), DEFVAL(-1), DEFVAL(Array()));
  BIND_SMETHOD_ARGS(BIND_CCONSTRUCTOR_HELPER(Wait, "other"));
}

Wait::Wait(double _dur, ID _group, IDArr blocks)
  : Action(0, _dur, _group, blocks, false)
{
  if (blocks.size() == 0)
    UtilityFunctions::printerr("Wait action supplied with no group to block. Actions proceeding as normal...");
}

Wait* Wait::_new(double _dur, ID _group, IDArr blocks) {
  return memnew(Wait(_dur, _group, blocks));
}

Wait::Wait(Ref<Wait> other)
  : Action(other, Variant(nullptr))
{}

Wait* Wait::_new(Ref<Wait> other) {
  return memnew(Wait(other));
}

Action* Wait::Reversed() {
  Wait* rev = memnew(Wait(this));
  rev->Reverse();

  return rev;
}

String Wait::get_debug_info() const {
  auto id = get_instance_id();
  String statestr = state_to_string(state);
  float timeLeft = duration - TimeElapsed();

  return debug_format->format(Array::make(id, "Wait", statestr, "Blocking", timeLeft));
}
