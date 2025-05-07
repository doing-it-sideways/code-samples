### ActionList Library
- C++17
- GDExtension (Godot 4.2.X)
- A library adding the concept of an "action list" to Godot. I don't know if there's any official
term for this; this is just the name my technical design professor gave for this structure.
- An action list is sort of an "animation" feature, similar to how the AnimationPlayer node works.
The main difference is that that instead of having statically defined variables for things like speed, start parameters, end parameters, and being relatively unflexible since it's supposed to be
an animation, an action list is created programmatically and can change during runtime.
- Although students from future classes may find this, I'm not worried as much about copying since
everyone's implementations of the "action list" concept should differ to some degree. Also I don't
think many people are insane enough to make a GDExtension.

### Poker Project
- The ActionList library was originally created for this project, so some example code is provided
to show its use cases.
- Additionally, this library was used for my DES365 HUD and Menu projects. Examples of those projects
can be found in the DES365 folder.


### Quick Notes about the ActionList library and Godot:
- GDExtension (Godot 4.1.X/4.2.X) does not allow pure virtual functions in any GDCLASS.
    - The action class has redundant function definitions because of this.
- Static variables sometimes cannot be initialized properly before library startup,
    so they are initialized in register_types and are allocated as pointers.
    - See https://github.com/godotengine/godot/issues/59688
- Custom constructors cannot be defined *in GDExtension* currently due to "create" being defined to use the default constructor.
    - static functions are bound instead and mimic the constructors when interacting from GDScript.
    - all classes need to have a default constructor
- Nodes in godot aren't a catch-all like Unity's GameObject/MonoBehaviour.
    - A basic "Node" doesn't store position, only types that require it do such as Node2D, Control (UI focused nodes), and Node3D.
    - As such, to make a generic action list, some casting and storing of type data are used to work with different node types.
    - This results in some duplicated code that looks very similar.
Library style:
- Functions with upper-camelcase names are registered and bound to the godot runtime.
- Functions with lowercase underscored names are used inside the library only.
    - Exception: _new##Alias for constructors