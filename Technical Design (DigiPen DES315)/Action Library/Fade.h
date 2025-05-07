/*****************************************************************************
  An action that fades between two opacitys for a node.

  Author(s): Evan O'Bryant
  Copyright © 2024 DigiPen (USA) Corporation.    
*****************************************************************************/

#ifndef FADE_H
#define FADE_H

#include "Action.h"
#include "macros.h"

namespace godot {

class CanvasItem;

  //fade a node either in or out
class Fade : public Action {
  GDCLASS(Fade, Action)

protected:
  static void _bind_methods();

//-----structs and typedefs-----
public:
    //just a regular enum telling whether the node should fade out or in
  enum FadeMode {
    FADE_IN,
    FADE_OUT
  };

//-----funcs-----
public:
  Fade() = default;
  ~Fade() = default;

    //auto fading either in or out
  CUSTOM_CONSTRUCTOR(Fade, , Variant node, double delay, double duration, Tween::EaseType ease, Tween::TransitionType trans,
                     FadeMode mode, ID group, IDArr blocks, bool oneshot);
    //manual fading from a specified begin opacity to an end opacity
  CUSTOM_CONSTRUCTOR(Fade, Manual, Variant node, double delay, double duration, Tween::EaseType ease, Tween::TransitionType trans,
                     float _beginOpacity, float _endOpacity,
                     ID group, IDArr blocks, bool oneshot);
    //copy constructor
  CUSTOM_CONSTRUCTOR(Fade, Copy, Ref<Fade> other, Variant node);

  void init_process() override;
  void process(double dt) override;
  
  void Reverse() override;
  Action* Reversed() override;

  String get_debug_info() const override;

//-----vars-----
private:
  CanvasItem* ci;

  float beginOpacity = 0.f;
  float endOpacity = 1.f;

  bool customStart = false;
};

} // namespace godot

// has to be declared outside of the godot namespace, otherwise it would do godot::godot::...
VARIANT_ENUM_CAST(Fade::FadeMode);

#endif // FADE_H