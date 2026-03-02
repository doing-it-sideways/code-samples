/*****************************************************************************
  An action that transforms a Node2D node or Control node over time.

  Author(s): Evan O'Bryant
  Copyright © 2024 DigiPen (USA) Corporation.    
*****************************************************************************/

#include "Transform.h"
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/math.hpp>

using namespace godot;

void Transform::_bind_methods() {
    //register the bitfield flags so they can be used inside godot properly
  BIND_BITFIELD_FLAG(PROCESS_TRANSLATE);
  BIND_BITFIELD_FLAG(PROCESS_ROTATE);
  BIND_BITFIELD_FLAG(PROCESS_SCALE);
  BIND_BITFIELD_FLAG(PROCESS_ALL);

  BIND_SMETHOD_ARGS(BIND_CONSTRUCTOR_HELPER(Transform, , "flags", "posRotScaleArr", "group", "blocks", "oneShot"),
                                            DEFVAL(PROCESS_ALL), DEFVAL(Array()), DEFVAL(0), DEFVAL(Array()), DEFVAL(false));
  BIND_SMETHOD_ARGS(BIND_CCONSTRUCTOR_HELPER(Transform, "other", "node"));
}

Transform::Transform(Variant node, double delay, double duration, Tween::EaseType ease, Tween::TransitionType trans,
                     BitField<ProcessFlags> _flags, Array posRotScale,
                     ID group, IDArr blocks, bool oneshot)
  : Action(node, delay, duration, ease, trans, group, blocks, oneshot)
  , flags(_flags)
{
  int arrPos = 0;

  if (_flags.has_flag(PROCESS_TRANSLATE))
    endPos = posRotScale[arrPos++];

  if (_flags.has_flag(PROCESS_ROTATE)) {
    endRot = posRotScale[arrPos++];
    endRot = Math::deg_to_rad(endRot);
  }

  if (_flags.has_flag(PROCESS_SCALE))
    endScale = posRotScale[arrPos];
}

Transform* Transform::_new(Variant node, double delay, double duration, Tween::EaseType ease, Tween::TransitionType trans,
             BitField<ProcessFlags> flags, Array posRotScale,
             ID group, IDArr blocks, bool oneshot)
{
  return memnew(Transform(node, delay, duration, ease, trans, flags, posRotScale, group, blocks, oneshot));
}

Transform::Transform(Ref<Transform> other, Variant node)
  : Action(other, node)
  , endPos(other->endPos)
  , endScale(other->endScale)
  , endRot(other->endRot)
  , flags(other->flags)
{}

Transform* Transform::_new(Ref<Transform> other, Variant node) {
  return memnew(Transform(other, node));
}

void Transform::init_process() {
    //cast node to right type and then get values
    //the parent of both Control and Node, CanvasItem, doesn't have any sort of transform attached to it
    //so values must be grabbed separately
  if (type == NodeType::NODE2D) {
    auto node = reinterpret_cast<Node2D*>(node_internal);

    beginPos = node->get_position();
    beginRot = node->get_rotation();
    beginScale = node->get_scale();
  }
  else if (type == NodeType::CONTROL) {
    auto node = reinterpret_cast<Control*>(node_internal);

    beginPos = node->get_position();
    beginRot = node->get_rotation();
    beginScale = node->get_scale();
  }

  isDoneFirstUpdate = true;
}

void Transform::process(double dt) {
  const double timeElapsed = TimeElapsed();

    //the instructions for the project didn't say we had to make our own easing stuff afaik, so just use godot's manual interpolation function
  Vector2 pos = Tween::interpolate_value(beginPos, endPos - beginPos, timeElapsed, duration, trans, ease);
  float rot = Tween::interpolate_value(beginRot, endRot - beginRot, timeElapsed, duration, trans, ease);
  Vector2 scale = Tween::interpolate_value(beginScale, endScale - beginScale, timeElapsed, duration, trans, ease);

  if (type == NodeType::NODE2D) {
    auto node = reinterpret_cast<Node2D*>(node_internal);

    if (flags.has_flag(PROCESS_TRANSLATE))
      node->set_position(pos);
    
    if (flags.has_flag(PROCESS_ROTATE))
      node->set_rotation(rot);
    
    if (flags.has_flag(PROCESS_SCALE))
      node->set_scale(scale);
  }
  else if (type == NodeType::CONTROL) {
    auto node = reinterpret_cast<Control*>(node_internal);

    if (flags.has_flag(PROCESS_TRANSLATE))
      node->set_position(pos);
    
    if (flags.has_flag(PROCESS_ROTATE))
      node->set_rotation(rot);
    
    if (flags.has_flag(PROCESS_SCALE))
      node->set_scale(scale);
  }
}

void Transform::Reverse() {
  Action::Reverse();

  endPos = beginPos;
  endRot = beginRot;
  endScale = beginScale;
}

Action* Transform::Reversed() {
  Transform* rev = memnew(Transform(this, node));
  rev->Reverse();

  return rev;
}

String Transform::get_debug_info() const {
  String description = "";
  if (flags.has_flag(PROCESS_TRANSLATE))
      description += "|Translating|";
    
  if (flags.has_flag(PROCESS_ROTATE))
      description += "|Rotating|";
    
  if (flags.has_flag(PROCESS_SCALE))
      description += "|Scaling|";

  auto id = get_instance_id();
  String statestr = state_to_string(state);
  float timeLeft = duration - TimeElapsed();

  return debug_format->format(Array::make(id, "Transform", statestr, description, timeLeft));
}
