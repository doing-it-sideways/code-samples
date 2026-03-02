/*****************************************************************************
  An action that flips a card for the poker example code.

  Author(s): Evan O'Bryant
  Copyright © 2024 DigiPen (USA) Corporation.    
*****************************************************************************/

#ifndef FLIP_H
#define FLIP_H

//poker specific action

#include "Action.h"
#include "macros.h"

namespace godot {

class Sprite2D;

  //"flip" a Sprite2D to it's last frame
class Flip2D : public Action {
  GDCLASS(Flip2D, Action)

protected:
  static void _bind_methods();

//-----structs and typedefs-----
public:
    //another bit field to see what to process. see Transform.h and Transform.cpp
  enum FlipFlags {
    FLIP_X = 1,
    FLIP_Y = 2
  };

//-----funcs-----
public:
  Flip2D() = default;
  ~Flip2D() = default;

  CUSTOM_CONSTRUCTOR(Flip2D, , Variant node, double delay, double duration, Tween::EaseType ease, Tween::TransitionType trans,
                     BitField<FlipFlags> flags, ID group, IDArr blocks, bool oneshot);
  CUSTOM_CONSTRUCTOR(Flip2D, Copy, Ref<Flip2D> other, Variant node);

  void init_process() override;
  void process(double dt) override;

  void Reverse() override;
  Action* Reversed() override;

  String get_debug_info() const override;

//-----vars-----
private:
  Sprite2D* sprite;

  Vector2 startScale = {1, 1};
  Vector2 endScale = {-1, -1};

  BitField<FlipFlags> flags = 0;

  int flippedTextureIndex;
  bool hasSwappedTexture = false;
};

} // namespace godot

VARIANT_BITFIELD_CAST(Flip2D::FlipFlags);

#endif // FLIP_H
