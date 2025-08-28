/*********************************************************************
 * \file   Broadcaster.cpp
 * \brief  Implementation of an observable object.
 *
 * \author Evan O'Bryant (evan.o)
 * \date   9/13/21
 Copyright © 2022 DigiPen (USA) Corporation.
 *********************************************************************/

#include "Broadcaster.h"
#include "IObserver.h"
#include "Component.h"

void Broadcaster::AddObserver(IObserver* observer, BroadcastResolutionFunc* func)
{
	observers.push_back(observer);
	observer->AddBroadcaster(this, func);
}


void Broadcaster::RemoveObserver(IObserver* observer) {
	size_t vecLen = observers.size(); //grabs the size of the vector for optimization
	if (!vecLen) return;

	// Note 2025: The following does not clean up memory properly and may have been
	// a source of some of the memory issues we had.

	//erases element at the correct position and then breaks from the loop
	for (size_t i = 0; i < vecLen; i++) {
		if (observers[i] == observer) {
			observers[i] = nullptr;
			break;
		}
	}
}

void Broadcaster::RemoveObserver(UID entityID)
{
	size_t vecLen = observers.size(); //grabs the size of the vector for optimization
	if (!vecLen) return;

	// Note 2025: The following does not clean up memory properly and may have been
	// a source of some of the memory issues we had.

	//erases element at the correct position and then breaks from the loop
	for (size_t i = 0; i < vecLen; i++) {
		if (!observers[i]) continue;
		IComponent* observer = static_cast<IComponent*>(observers[i]);
		if (observer && observer->entity == entityID) {
			observers[i] = nullptr;
		}
	}
}

void Broadcaster::Broadcast() {
	//updates every observer
	for (auto observer : observers) {
		if (!observer) continue;
		observer->ReceiveBroadcast(this);
	}
}

void Broadcaster::ClearObservers() {
	observers.clear();
}
