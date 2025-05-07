/*****************************************************************************
  A set of macros to more easily help when exposing class info to the GDExtension API.

  Author(s): Evan O'Bryant
  Copyright © 2024 DigiPen (USA) Corporation.    
*****************************************************************************/

#ifndef ACTIONLIBMACROS_H
#define ACTIONLIBMACROS_H

#define TEXT(Text) #Text
#define METHOD_NAME(Class, Func) D_METHOD(#Func), &Class::Func

	//shortcuts for binding non-static methods
#define BIND_METHOD_NOARG(Class, Func) ClassDB::bind_method(METHOD_NAME(Class, Func))
#define BIND_METHOD_1ARG(Class, Func, Arg) ClassDB::bind_method(D_METHOD(TEXT(Func), Arg), &Class::Func)
#define BIND_METHOD_1ARGD(Class, Func, Arg, DefVal) ClassDB::bind_method(D_METHOD(TEXT(Func), Arg), &Class::Func, DEFVAL(DefVal))
#define BIND_METHOD_ARGS ClassDB::bind_method

	//shortcuts for binding static methods
#define BIND_SMETHOD_NOARG(Class, Func) ClassDB::bind_static_method(METHOD_NAME(Class, Func))
#define BIND_SMETHOD_ARGS ClassDB::bind_static_method

	//allow binding the equivalent of a constructor in godot
	//constructor used internally, static _new exported as callable "constructor" due to create method already being defined in GDCLASS
	//default values passed for arguments that remain the same no matter what
	//in the .cpp files, there's a decent amount of bloat but i didn't want to waste time on working on a macro that can unfold itself for multiple use cases
#define CUSTOM_CONSTRUCTOR(Class, Alias, ...) Class(__VA_ARGS__); static Class* _new(__VA_ARGS__); using cptr_##Alias = Class* (*)(__VA_ARGS__);
#define BIND_CONSTRUCTOR_HELPER(Class, Alias, ...) get_class_static(), D_METHOD(TEXT(_new##Alias), "node", "delay", "duration", "ease", "trans", __VA_ARGS__), static_cast<cptr_##Alias>(&_new), DEFVAL(nullptr), DEFVAL(0.f), DEFVAL(0.f), DEFVAL(Tween::EaseType::EASE_IN), DEFVAL(Tween::TransitionType::TRANS_LINEAR)
#define BIND_CCONSTRUCTOR_HELPER(Class, ...) get_class_static(), D_METHOD(TEXT(_newCopy), __VA_ARGS__), static_cast<cptr_Copy>(&_new)


#endif
