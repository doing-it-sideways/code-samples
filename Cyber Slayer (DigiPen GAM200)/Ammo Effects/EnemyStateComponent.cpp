/**********************************************************************
    An example of how the ammo states were used. My teammate who
    originally created the enemy AI had used a state system and binary
    masks to set and clear states. While a little unwieldy, it got the
    job done and I didn't remake the system.
    Most of the code in this file was removed to just isolate the
    example.
**********************************************************************/

/*********************************************************************
 * \file   EnemyStateComponent.cpp
 * \brief  Represents all possible states of the enemy and is
 *				 responsible for broadcasting data based on the current state
 *
 * \author William Siauw & Evan O'Bryant
 * \date   October 2021
 Copyright © 2022 DigiPen (USA) Corporation.
 *********************************************************************/
#include "EnemyStateComponent.h"
#include "Transform.h"
#include "SteeringSystem.h"
#include "Debug.h"
#include "Roll.h"
#include "PoolSystem.h"
#include "AffineTransformation.h"

#define Parent static_cast<EnemyStateComponent*>(parent)

AmmoState* EnemyStateComponent::GetAmmoState(AmmoType ammoType)
{
  switch (ammoType)
  {
  case AmmoType::SLOW:
    return static_cast<AmmoState*>(statesArray[unsigned(EnemyStates::CRYO_SLOWED)]);
  case AmmoType::ACID:
    return static_cast<AmmoState*>(statesArray[unsigned(EnemyStates::ACID_MELTING)]);
  case AmmoType::FIRE:
    return static_cast<AmmoState*>(statesArray[unsigned(EnemyStates::FIRE_BURNING)]);
  }
  return nullptr;
}

void EnemyStateComponent::SetAmmoEffect(const AmmoType& t, bool value, float dmg) {
    if (t != AmmoType::NONE)
    {
        //start at the beginning of ammo effects
        unsigned mask = 0b1 << static_cast<unsigned>(EnemyStates::AMMO_EFFECTS_START);
        //ammo types start at 1, therefore 1 must be subtracted to line up properly
        mask <<= static_cast<unsigned>(t) - 1;

        if (value == false) { //toggle off
            mask = ~mask;
            dirtyStates &= mask;
        }
        else
            dirtyStates |= mask;
        
            // Note 2025: the team got sick of typing static_cast<type>(var) so we created
            //            a macro for static_cast to type less haha
        unsigned uState = SC(unsigned, EnemyStates::AMMO_EFFECTS_START) + SC(unsigned, t) - 1;
        if (statesArray[uState]) {
            AmmoState* aState = dynamic_cast<AmmoState*>(statesArray[uState]);
            aState->SetLastDamageVal(dmg);
            if (aState) {
                if (aState->isStackable())
                    aState->incrementStack();
            }
        }
        
        dirty = true;
    }
}
