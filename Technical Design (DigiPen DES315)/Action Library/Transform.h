/*****************************************************************************
  An action that transforms a Node2D node or Control node over time.

  Author(s): Evan O'Bryant
  Copyright © 2024 DigiPen (USA) Corporation.    
*****************************************************************************/

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Action.h"
#include "macros.h"

namespace godot {

  //modify a node's transform over time
class Transform : public Action {
  GDCLASS(Transform, Action)

protected:
  static void _bind_methods();

//-----structs and typedefs-----
public:
    //a bitfield for what parts of the transform should be processed
  enum ProcessFlags {
    PROCESS_TRANSLATE = 1,
    PROCESS_ROTATE = 2,
    PROCESS_SCALE = 4,
    PROCESS_ALL = PROCESS_SCALE | PROCESS_ROTATE | PROCESS_TRANSLATE
  };

//-----funcs-----
public:
  Transform() = default;
  ~Transform() = default;

  CUSTOM_CONSTRUCTOR(Transform, , Variant node, double delay, double duration, Tween::EaseType ease, Tween::TransitionType trans,
                     BitField<ProcessFlags> flags, Array posRotScale,
                     ID group, IDArr blocks, bool oneshot);
  CUSTOM_CONSTRUCTOR(Transform, Copy, Ref<Transform> other, Variant node);

  void init_process() override;
  void process(double dt) override;

  void Reverse() override;
  Action* Reversed() override;

  String get_debug_info() const override;

//-----vars-----
private:
  Vector2 beginPos = {0, 0};
  Vector2 endPos = {0, 0};

  Vector2 beginScale = {1, 1};
  Vector2 endScale = {1, 1};

  float beginRot = 0;
  float endRot = 0;

  BitField<ProcessFlags> flags = PROCESS_ALL;
};

} // namespace godot

  //declare outside of namespace to avoid doing godot::godot::
VARIANT_BITFIELD_CAST(Transform::ProcessFlags);

#endif // TRANSFORM_H