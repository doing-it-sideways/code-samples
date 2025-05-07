/*****************************************************************************
  An action that fades between two opacitys for a node.

  Author(s): Evan O'Bryant
  Copyright © 2024 DigiPen (USA) Corporation.    
*****************************************************************************/

#include "Fade.h"
#include <godot_cpp/classes/canvas_item.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Fade::_bind_methods() {
  // enum constants must be specified. in GDScript they work different than c++
  BIND_ENUM_CONSTANT(FADE_IN);
  BIND_ENUM_CONSTANT(FADE_OUT);

  BIND_SMETHOD_ARGS(BIND_CONSTRUCTOR_HELPER(Fade, , "mode", "group", "blocks", "oneshot"),
                                            DEFVAL(FADE_IN), DEFVAL(0), DEFVAL(Array()), DEFVAL(false));
  BIND_SMETHOD_ARGS(BIND_CONSTRUCTOR_HELPER(Fade, Manual, "beginOpacity", "endOpacity", "group", "blocks", "oneshot"),
                                            DEFVAL(0), DEFVAL(1.f), DEFVAL(0), DEFVAL(Array()), DEFVAL(false));
  BIND_SMETHOD_ARGS(BIND_CCONSTRUCTOR_HELPER(Fade, "other", "node"));
}

void Fade::init_process() {
  if (customStart && beginOpacity == endOpacity) {
    UtilityFunctions::printerr("Opacity values provided were the same! Forcing action to end...");
    ForceFinish();
  }
  else
    isDoneFirstUpdate = true;
}

void Fade::process(double dt) {
  Color c = ci->get_modulate();
  
  //godot has built in easing, which is really convenient
  c.a = Tween::interpolate_value(beginOpacity, endOpacity - beginOpacity, TimeElapsed(), duration, trans, ease);
  ci->set_modulate(c);
}

void Fade::Reverse() {
  Action::Reverse();

  float temp = endOpacity;
  endOpacity = beginOpacity;
  beginOpacity = temp;
}

Action* Fade::Reversed() {
  Fade* rev = memnew(Fade(this, node));
  rev->Reverse();
  return rev;
}

String Fade::get_debug_info() const {
  auto id = get_instance_id();
  String statestr = state_to_string(state);
  float timeLeft = duration - TimeElapsed();

  String description = (beginOpacity > endOpacity) ? "Fading out..." : "Fading in...";

  return debug_format->format(Array::make(id, "Fade", statestr, description, timeLeft));
}

// Moved constructors down here as they distract from the core functionality of the Fade action.
// You can see how annoying doing a constructor *and then* a _new function is for each constructor.

Fade::Fade(Variant node, double delay, double duration, Tween::EaseType ease, Tween::TransitionType trans,
           FadeMode mode, ID group, IDArr blocks, bool oneshot)
  : Action(node, delay, duration, ease, trans, group, blocks, oneshot)
  , ci(reinterpret_cast<CanvasItem*>(node_internal)) //all nodes inherit from canvas item, so this works! is it safe? prolly not
  , beginOpacity(mode == FADE_IN ? 0.f : 1.f)
  , endOpacity(mode == FADE_IN ? 1.f : 0.f)
{}

Fade* Fade::_new(Variant node, double delay, double duration, Tween::EaseType ease, Tween::TransitionType trans,
                 FadeMode mode, ID group, IDArr blocks, bool oneshot)
{
  return memnew(Fade(node, delay, duration, ease, trans, mode, group, blocks, oneshot));
}

Fade::Fade(Variant node, double delay, double duration, Tween::EaseType ease, Tween::TransitionType trans,
           float _beginOpacity, float _endOpacity, ID group, IDArr blocks, bool oneshot)
  : Action(node, delay, duration, ease, trans, group, blocks, oneshot)
  , ci(reinterpret_cast<CanvasItem*>(node_internal))
  , beginOpacity(Math::clamp(_beginOpacity, 0.f, 1.f))
  , endOpacity(Math::clamp(_endOpacity, 0.f, 1.f))
  , customStart(true)
{}

Fade* Fade::_new(Variant node, double delay, double duration, Tween::EaseType ease, Tween::TransitionType trans,
                 float _beginOpacity, float _endOpacity, ID group, IDArr blocks, bool oneshot)
{
  return memnew(Fade(node, delay, duration, ease, trans, _beginOpacity, _endOpacity, group, blocks, oneshot));
}

Fade::Fade(Ref<Fade> other, Variant node)
  : Action(other, node)
  , ci(reinterpret_cast<CanvasItem*>(node_internal))
  , beginOpacity(other->beginOpacity)
  , endOpacity(other->endOpacity)
  , customStart(other->customStart)
{}

Fade* Fade::_new(Ref<Fade> other, Variant node) {
  return memnew(Fade(other, node));
}
