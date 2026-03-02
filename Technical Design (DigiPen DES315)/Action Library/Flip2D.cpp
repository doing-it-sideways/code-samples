/*****************************************************************************
  An action that flips a card for the poker example code.

  Author(s): Evan O'Bryant
  Copyright © 2024 DigiPen (USA) Corporation.    
*****************************************************************************/

#include "Flip2D.h"
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Flip2D::_bind_methods() {
  BIND_BITFIELD_FLAG(FLIP_X);
  BIND_BITFIELD_FLAG(FLIP_Y);

  BIND_SMETHOD_ARGS(BIND_CONSTRUCTOR_HELPER(Flip2D, , "flags", "group", "blocks", "oneshot"),
                                            DEFVAL(FLIP_X | FLIP_Y), DEFVAL(0), DEFVAL(Array()), DEFVAL(false));
  BIND_SMETHOD_ARGS(BIND_CCONSTRUCTOR_HELPER(Flip2D, "other", "node"));
}

Flip2D::Flip2D(Variant node, double delay, double duration, Tween::EaseType ease, Tween::TransitionType trans,
               BitField<FlipFlags> _flags, ID group, IDArr blocks, bool oneshot)
  : Action(node, delay, duration, ease, trans, group, blocks, oneshot)
  , sprite(Object::cast_to<Sprite2D>(node))
  , flags(_flags)
  , flippedTextureIndex(sprite->get_frame() == 0 ? sprite->get_hframes() - 1 : 0)
{
  if (!UtilityFunctions::is_instance_valid(sprite))
    UtilityFunctions::printerr("Invalid Sprite2D");
}

Flip2D* Flip2D::_new(Variant node, double delay, double duration, Tween::EaseType ease, Tween::TransitionType trans,
                     BitField<FlipFlags> flags, ID group, IDArr blocks, bool oneshot)
{
  return memnew(Flip2D(node, delay, duration, ease, trans, flags, group, blocks, oneshot));
}

Flip2D::Flip2D(Ref<Flip2D> other, Variant node)
  : Action(other, node)
  , sprite(Object::cast_to<Sprite2D>(node))
  , flags(other->flags)
  , flippedTextureIndex(sprite->get_frame() == 0 ? sprite->get_hframes() - 1 : 0)
{
  if (!UtilityFunctions::is_instance_valid(sprite))
    UtilityFunctions::printerr("Invalid Sprite2D");
}

Flip2D* Flip2D::_new(Ref<Flip2D> other, Variant node) {
  return memnew(Flip2D(other, node));
}

void Flip2D::init_process() {
  startScale = sprite->get_scale();
  endScale = -startScale;

  isDoneFirstUpdate = true;
}

void Flip2D::process(double dt) {
    //modify the scale
  Vector2 curScale = sprite->get_scale();
  Vector2 scale = Tween::interpolate_value(startScale, endScale - startScale, TimeElapsed(), duration, trans, ease);

  if (flags.has_flag(FLIP_X)) {
    sprite->set_scale({ scale.x, curScale.y });
    curScale.x = scale.x;
  }
  
  if (flags.has_flag(FLIP_Y))
    sprite->set_scale({ curScale.x, scale.y });
  
    //when the object's scale is essentially 0, swap the textures so the user doesn't see anything
  if (!hasSwappedTexture && PercentDone() >= 0.5) {
    sprite->set_frame(flippedTextureIndex);
    hasSwappedTexture = true;
  }
}

void Flip2D::Reverse() {
  Action::Reverse();

  endScale = startScale;
  flippedTextureIndex = sprite->get_frame() == 0 ? sprite->get_hframes() - 1 : 0;
  hasSwappedTexture = false;
}

Action* Flip2D::Reversed() {
  Flip2D* rev = memnew(Flip2D(this, node));
  rev->Reverse();

  return rev;
}

String Flip2D::get_debug_info() const {
  String description = hasSwappedTexture ? "Flipped" : "Not Flipped";

  auto id = get_instance_id();
  String statestr = state_to_string(state);
  float timeLeft = duration - TimeElapsed();

  return debug_format->format(Array::make(id, "Flip2D", statestr, description, timeLeft));
}
