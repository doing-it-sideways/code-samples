/*********************************************************************
 * \file   AmmoTypes.cpp
 * \brief  Ammo Effects System
 *
 * \author Evan O'Bryant
 * \date   11/12/21
 Copyright © 2022 DigiPen (USA) Corporation.
 *********************************************************************/

#include "AmmoData.h"
#include "AttackEmitterFactory.h"
#include "DDMath.h"
#include "Debug.h"
#include "EnemyStateComponent.h"
#include <glm/glm.hpp>
#include "Roll.h"
#include "StateComponent.h"
#include "StateSystem.h"
#include <stdexcept>
#include "Transform.h"
#include "TransformComponent.h"
#include <vector>

/////////////////////////////
//     AMMO STATE FUNCS    //
/////////////////////////////

/*!
 * @brief Constructs a new ammo state for an entity.
 * @param parent The entity's state component.
 * @param t The ammo type
*/
AmmoState::AmmoState(StateComponent* parent, AmmoType t) :
	State(parent),
	type(t),
	timer(0),
	cooldownTime(0),
	isPermanent(false),
	oneTimeUse(false),
	stackable(false),
	stackLevel(0),
	effectHandler(StatesSystem::effectList[static_cast<unsigned>(t) - 1]) //minus 1 because array reasons.
																		  //please dont question it...
{}

/*!
 * @brief Initializes the ammo state effect on the enemy, resetting the timers.
*/
void AmmoState::Init() {
	timer = effectHandler->timeActive;
	cooldownTime = effectHandler->cooldownTime;
	isPermanent = effectHandler->isPermanent;
	oneTimeUse = effectHandler->oneUse; //if true: effect only happens upon collision with something
	stackable = effectHandler->stackable;
	if (stackable)
		incrementStack();
	if (oneTimeUse)
	{
		EnemyStateComponent* enemy = static_cast<EnemyStateComponent*>(parent);
		effectHandler->HandleEffect(*enemy, 1);
	}
}

/*!
 * @brief Updates the enemy to be affected by the ammo effect.
 * @param dt
*/
void AmmoState::Update(float dt) {
	EnemyStateComponent* enemy = static_cast<EnemyStateComponent*>(parent);
	assert(enemy);

	if (isPermanent)
		effectHandler->HandleEffect(*enemy, 1);
	else if (oneTimeUse) {
		//effectHandler->HandleEffect(*enemy);
		//enemy->SetAmmoEffect(type, false)
		enemy->SetAmmoEffect(type, false, 0);
		return;
	}
	//effect runs until its timer expires.
	else if (timer > 0) {
		//effect only happens if cooldown is over
		if (cooldownTime <= 0) {
			//if the enemy had a change due to any ammo effect then it broadcasts
			if (effectHandler->HandleEffect(*enemy, stackLevel))
				enemy->needToBroadcast = true;

			//if has cool side effect for a particle or something put that code here

			//reset cooldown timer
			cooldownTime = effectHandler->cooldownTime;
		}
		else
			cooldownTime -= dt;

		timer -= dt;
	}
	else {
		enemy->SetAmmoEffect(type, false, 0); //switches effect off
		stackLevel = 0;
		stackString = "";
	}
}

/*!
 * @brief Resets timers in case effect gets re-enabled.
*/
void AmmoState::Exit() {
	timer = 0;
	cooldownTime = 0;
}

void AmmoState::incrementStack()
{
	stackLevel += std::clamp(static_cast<int>(lastDamageValue), 1, INT32_MAX); timer = effectHandler->timeActive;
	// Note 2025: The buffer being outputted to is not debug information
	char buffer[10];
	sprintf_s(buffer, 10, "x%u", stackLevel);
	stackString = buffer;
}

void AmmoState::decrementStack()
{
	--stackLevel;
	char buffer[10];
	sprintf_s(buffer, 10, "x%u", stackLevel);
	stackString = buffer;
}



/////////////////////////////
//	  AMMO EFFECT FUNCS    //
/////////////////////////////

/*!
 * @brief Constructs a new AmmoEffectHandler
 * @param t The ammo type
*/
AmmoEffectHandler::AmmoEffectHandler(AmmoType t) :
	type(t),
	eHandle(FindEnemyHandler()),
	sHandle(FindSpecialHandler())
{}

/*!
 * @brief Constructs a new AmmoEffectHandler
 * @param t The ammo type
 * @param permanent If the effect is permanent or not.
 * @param oneUse If the effect is a one time use or not.
 * @param _timeActive How long the effect is active for (if persistent)
 * @param _cooldownTime How long the cooldown is (if persistent).
*/
AmmoEffectHandler::AmmoEffectHandler(AmmoType t, bool permanent, bool _oneUse,
									 float _timeActive, float _cooldownTime, bool _stackable) :
	type(t),
	isPermanent(permanent),
	oneUse(_oneUse),
	timeActive(_timeActive),
	cooldownTime(_cooldownTime),
	stackable(_stackable),
	eHandle(FindEnemyHandler()),
	sHandle(FindSpecialHandler())
{}

/*!
 * @brief Handles the correct ammo effect given a certain enemy.
 * @param affectedEnemy The affected enemy.
 * @return If the enemy state was changed or not.
*/
bool AmmoEffectHandler::HandleEffect(EnemyStateComponent& affectedEnemy, int stackCount) const {
	if (eHandle != nullptr)
		return eHandle(affectedEnemy, stackCount);
	else
		return false;
}

/*!
 * @brief Handles the correct ammo effect given that it's a special ammo type.
 * @param attackEmmitter the emmitter to use to create the effect
 * @param pos the position the effect will show up
*/
void AmmoEffectHandler::HandleEffect(IComponent* attackEmmitter, const glm::vec3& pos, const UID& id, float dmg) const {
	if (sHandle != nullptr)
		sHandle(attackEmmitter, pos, id, dmg);
}

/*!
 * @brief Gets the ammo type that the effect handler handles.
 * @return The type of the ammo.
*/
AmmoType AmmoEffectHandler::GetType() const {
	return type;
}

/*!
 * @brief Returns the ammo type managed by the effect handler as an unsigned integer.
 *        This may provide ease of instead of getting the type then casting it.
*/
AmmoEffectHandler::operator unsigned() const {
	return static_cast<unsigned>(type);
}

/////////////////////////////
//	  HANDLER FUNCTIONS    //
/////////////////////////////

/*!
 * @brief Handles a slow bullet by slowing the enemy in question.
 * @param enemy The enemy being slowed.
 * @return If the enemy's data was modified at all. True means the component should broadcast.
*/
bool HandleSlow(EnemyStateComponent& enemy, unsigned stackCount) {
	constexpr float slowMultiplier = .8f;
	float currentMultiplier = 1.0f;
	for (unsigned i = 0; i < stackCount; ++i)
	{
		currentMultiplier *= slowMultiplier;
	}
	enemy.cryoMultiplier = currentMultiplier;
	if (enemy.CheckState(unsigned(EnemyStates::AVOIDING_OBSTACLES)))
	{
		float newSpeed = enemy.maxSpeed * enemy.cryoMultiplier;
		if (enemy.currentSpeed != newSpeed) { //only changes if enemy is actively moving
			enemy.currentSpeed = newSpeed;
			return true;
		}
	}
	else
	{
		glm::vec3 newVel = enemy.enemyVelocity * enemy.cryoMultiplier;
		if (enemy.enemyVelocity != newVel) { //only changes if enemy is actively moving
			enemy.enemyVelocity = newVel;
			return true;
		}

	}

	return false;
}

/*!
 * @brief Handles an acid bullet by slowly damaging and potentially killing the enemy.
 * @param enemy The enemy being afflicted.
 * @return true
*/
bool HandleAcid(EnemyStateComponent& enemy, unsigned stackCount) {
	//data from playtest said acid was a bit too strong, especially on the shotgun, so it's slightly nerfed
	constexpr float acidDamage = 0.8f;

	enemy.health -= acidDamage * stackCount;

	if (enemy.health <= 0) {
		++(static_cast<StatesSystem*>(enemy.parent)->dirtyEnemiesKilledThisFrame);
		enemy.SetState(EnemyStates::DEAD, true);
	}
	
	return true;
}

/*!
 * @brief Handles a heavy bullet knocking back an enemy and stunning it.
 * @param enemy The enemy being afflicted
 * @return if the enemy position was updated (if enemy wasn't moving, it still isn't moving).
*/
bool HandleHeavy(EnemyStateComponent& enemy, unsigned stackCount) {
	UNREFERENCED_PARAMETER(stackCount);
		//playtest data showed heavy as being unfavorable, players didn't want to use it, upped from 250 to 400
	const float knockbackForceMagnitude = 400.0f * enemy.GetHeavyLastBulletDamage();

	enemy.dirtyForceAppliedThisFrame += enemy.lastBulletHitDirection * knockbackForceMagnitude;
	enemy.dirty = true;

	return true;
}

bool HandleBurn(EnemyStateComponent& enemy, unsigned stackCount) {
	constexpr float burnDamage = 1.15f;

	enemy.health -= burnDamage * stackCount;

	if (enemy.health <= 0) {
		++(static_cast<StatesSystem*>(enemy.parent)->dirtyEnemiesKilledThisFrame);
		enemy.SetState(EnemyStates::DEAD, true);
	}

	return true;
}

/*!
 * @brief Creates an explosion on the location the bullet hit
 * @param attackEmmitter The emmitter factory that will create the explosion
 * @param pos The position to create the explosion at
 * @param unused.
*/
void HandleBoom(IComponent* attackEmitter, const glm::vec3& pos, const UID&, float dmg) {
	AttackEmitterFactory* factory = static_cast<AttackEmitterFactory*>(attackEmitter);
	
	if (!factory->hasBeenCreated) { //only create one
		factory->damage = static_cast<int>(5 * dmg);
		factory->setSpawnPos(pos);
		factory->Create();
	}
}

void HandleFire(IComponent* attackEmitter, const glm::vec3& pos, const UID&, float dmg) {
	AttackEmitterFactory* factory = static_cast<AttackEmitterFactory*>(attackEmitter);

	if (!factory->hasBeenCreated) { //only create one
		factory->radius = 2 * dmg;
		factory->setSpawnPos(pos);
		factory->Create();
	}
}

/*!
 * @brief Creates a chain lightning effect starting from the first enemy going into others.
 * @param attackEmmitter The emitter factory that will create the small damaging radius.
 * @param pos The position of the first enemy (avoids hitting the same enemy).
 * @param firstEnemy id of the first enemy so that chain doesn't affect originally hit enemy
*/
void HandleChain(IComponent* attackEmmitter, const glm::vec3& pos, const UID& firstEnemy, float dmg) {
	constexpr unsigned chainLength = 5; //how many enemies should hit in the chains
	constexpr float maxDist = 60.0625f;	//the max distance one chain can be from another -- 7.75^2
	
	AttackEmitterFactory* factory = static_cast<AttackEmitterFactory*>(attackEmmitter);
	FactorySystem* facSys = static_cast<FactorySystem*>(factory->parent); //for access to list of enemies
	ISystem* transformSys = facSys->engine->GetSystem(Transform_SYS);


	std::vector<TransformComponent*> enemiesInRange;
	std::vector<UID> enemiesHit{ firstEnemy };
	std::vector<glm::vec2> enemyHitPositions{ {pos.x, pos.y} };
	glm::vec3 lastPos = pos;
	float distLeft = maxDist;

	if (!factory->hasBeenCreated) { //do not do the effect multiple times
		//chain 3 enemies together
		for (unsigned i = 0; i < chainLength; ++i) {

				//find possible enemy/enemies
			for (auto& id : facSys->activeEnemies) {
					//do not hit the same enemy or first enemy that was in contact with
				const auto lambda = [&](const UID& enemyID) { return enemyID == id; };
				if (std::find_if(enemiesHit.cbegin(), enemiesHit.cend(), lambda) != enemiesHit.cend())
					continue;

				TransformComponent* eTransform = static_cast<TransformComponent*>(transformSys->GetEntity(id));
				
				//if (eTransform->getPos() == pos) //skip enemy initially hit
				//	continue;
				
					//if in suitible range
				float currDist = DDMath::SquareDistance(lastPos, eTransform->getPos());
				if (currDist <= distLeft) {
					distLeft -= currDist;
#if 1
					//un-if 0 this if you want random values instead of first found
					enemiesInRange.push_back(eTransform);
#elif
					enemiesInRange.push_back(eTransform);
					break; //stop searching after first found
#endif
				}
			}

			if (enemiesInRange.empty()) //nothing to do -- end chaining effect
				break;

			//found enemy/enemies, chain
#if 1
			//un-if 0 this if you want to use the random values instead of first found
			int index = enemiesInRange.size() > 1 ? RollInt(0, static_cast<int>(enemiesInRange.size()) - 1) : 0;
			const glm::vec3& ePos = enemiesInRange[index]->getPos();

			factory->setSpawnPos(ePos);
			enemyHitPositions.push_back(glm::vec2(ePos));
			factory->Create();

			lastPos = ePos;
			enemiesHit.push_back(enemiesInRange[index]->entity);
#elif
			const glm::vec3& ePos = enemiesInRange[0]->getPos();

			factory->setSpawnPos(ePos);
			factory->Create();

			lastPos = ePos;
			enemiesHit.push_back(enemiesInRange[0]->entity);
#endif
			enemiesInRange.clear();
		} //repeat up to "chainLength" amount

		FactorySystem::SetChainEffectPositionsList(enemyHitPositions);
		FactorySystem::GetChainEffectFactory()->Create();
	}
}

/////////////////////////////
//	   GETTING HANDLER     //
/////////////////////////////

/*!
 * @brief Grabs the correct function for the ammo type
 * @return A pointer to the correct handler.
*/
EnemyEffectHandler AmmoEffectHandler::FindEnemyHandler() {
	//dont need break statements since the only line is a return
	switch (type) {
	case AmmoType::SLOW:
		return HandleSlow;
	case AmmoType::ACID:
		return HandleAcid;
	case AmmoType::HEAVY:
		return HandleHeavy;
	case AmmoType::FIRE:
		return HandleBurn;
	default:
		return nullptr;
	}
}

SpecialEffectHandler AmmoEffectHandler::FindSpecialHandler() {
	switch (type) {
	case AmmoType::BOOM:
		return HandleBoom;
	case AmmoType::FIRE:
		return HandleFire;
	case AmmoType::CHAIN:
		return HandleChain;
	default:
		return nullptr;
	}
}
