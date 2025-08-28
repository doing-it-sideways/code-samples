/*********************************************************************
 * \file   AmmoTypes.h
 * \brief  Ammo Effects System
 *
 * \author Evan O'Bryant
 * \date   11/12/21
 Copyright © 2022 DigiPen (USA) Corporation.
 *********************************************************************/

#pragma once

#include "StateComponent.h"
#include <algorithm>

enum class AmmoType : unsigned {
	NONE = 0,

	BASE_TYPE = NONE, //add enum types after the last one and make sure to switch values when needed
	SLOW,
	ACID,
	HEAVY,
	END_BASE = HEAVY,

	SPECIAL_TYPES = END_BASE,
	BOOM,
	FIRE,
	CHAIN,
	END_SPECIAL = CHAIN,

	COUNT = END_SPECIAL
};

class EnemyStateComponent;

using EnemyEffectHandler = bool (*)(EnemyStateComponent&, unsigned);
using SpecialEffectHandler = void (*)(IComponent*, const glm::vec3&, const UID&, float);

class AmmoEffectHandler {
public: //funcs
	explicit AmmoEffectHandler(AmmoType t);
	explicit AmmoEffectHandler(AmmoType t,
							   bool permanent, bool _oneUse,
							   float _timeActive, float _cooldownTime, bool _stackable);
	
	bool HandleEffect(EnemyStateComponent& affectedEnemy, int stackCount) const; // CHANGES ENEMY STATE!!!
	void HandleEffect(IComponent* attackEmmitter, const glm::vec3& pos, const UID& id, float dmg) const;

	AmmoType GetType() const;
	operator unsigned() const; //gets ammo type in u_integer form

	//Deletes default ways of copying -- should only be instantiated once per ammo type
	AmmoEffectHandler(const AmmoEffectHandler&) = delete;
	AmmoEffectHandler& operator=(const AmmoEffectHandler&) = delete;
	AmmoEffectHandler(AmmoEffectHandler&&) = delete;
	AmmoEffectHandler& operator=(AmmoEffectHandler&&) = delete;

public: //data -- all constant, so no need to be private as they'll only be read from
	const bool isPermanent = false;
	const bool oneUse = false;
	//bool isPersistent = true;
	const float timeActive = 4.0f;
	const float cooldownTime = 0;
	const bool stackable = false;

private: //funcs
	EnemyEffectHandler FindEnemyHandler();
	SpecialEffectHandler FindSpecialHandler();

private: //data
	const AmmoType type;
	EnemyEffectHandler eHandle;	  //enemy handler if it exists
	SpecialEffectHandler sHandle; //special type handler if it exists
};

class AmmoState : public State {
public:
	explicit AmmoState(StateComponent* parent, AmmoType t);
	void Init() override;
	void Update(float dt) override;
	void Exit() override;

	AmmoState(const AmmoState&) = delete;
	AmmoState(AmmoState&&) = delete;

	void SetLastDamageVal(float dmg) { lastDamageValue = dmg; }
	float GetLastDamageVal() { return lastDamageValue; }

	// Note 2025: Why the following functions start with a lowercase letter I have no idea.
	inline bool isStackable() { return stackable; }
	void incrementStack();
	void decrementStack();

	const std::string& getStackString() { return stackString; }
private:
	const AmmoType type;
	AmmoEffectHandler* effectHandler;
	bool isPermanent;
	bool oneTimeUse;
	//bool isPersistent = true;
	float timer;
	float cooldownTime;
	float lastDamageValue;
	bool stackable;
	unsigned stackLevel;
	std::string stackString;
};
