/*********************************************************************
 * \file   Broadcaster.h
 * \brief  Defines how an object can be observed.
 *
 * \author Evan O'Bryant (evan.o)
 * \date   9/13/21
 * 
 * Code based on descriptions from Wikipedia
 * (https://en.wikipedia.org/wiki/Observer_pattern)
 * and from an explanation video made by Christopher Okhavri
 * (https://www.youtube.com/watch?v=_BpmfnqjgzQ)
 
 Copyright © 2022 DigiPen (USA) Corporation.
 *********************************************************************/
#pragma once
#include <vector>
#include "FunctionTypedefs.h"
#include "ID.h"
/*!
 * @brief 
 *   Interface for an object that can be observed and will broadcast out messages.
*/
class Broadcaster {
public:
	/*!
	 * @brief 
	 *   Allocates 5 observers per broadcaster to start. will automatically resize if more is needed.
	*/
	//Broadcaster() : observers(5) {};

	// Note 2025: Non virtual destructor, uh oh!
	/*!
	 * @brief OBLITERATES the observable object.
	*/
	~Broadcaster() { };
	
	/*!
	 * @brief
	 *   Attaches an observer to the broadcaster.
	 * @param observer 
	 *   The observer to add.
	 * @param type
	 *   Type of broadcaster for Observer's map.
	*/
	void AddObserver(IObserver* observer, BroadcastResolutionFunc* func);
	
	/*!
	 * @brief 
	 *   Removes a specific observer from the list of observers.
	 * @param observer 
	 *   The specific observer.
	*/
	void RemoveObserver(IObserver* observer);
	void RemoveObserver(UID entityID);
	///*!
	// * @brief 
	// *   Removes a specific observer based on it's position in the vector.
	// * @param index 
	// *   The index of the observer.
	//*/
	//void RemoveObserver(unsigned index);
	//yeah im stupid when would it be viable for the program to know
	//an observer's exact position in the array :/

	/*!
	 * @brief 
	 *   Notifies all observers linked to the broadcaster.
	 *   Can be overriden to allow for special notifications.
	*/
	virtual void Broadcast();

	/*!
	 * @brief 
	 *   Clears the full list of observers for the broadcaster.
	*/
	void ClearObservers();

private:
	std::vector<IObserver*> observers; //!< The list of observers.
};
