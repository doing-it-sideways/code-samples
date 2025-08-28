/*********************************************************************
 * \file   IObserver.h
 * \brief  Defines how an object observes another one.
 *
 * \author Evan O'Bryant (evan.o)
 * \date   9/13/21
 * 
 * 
 * Code based on descriptions from Wikipedia
 * (https://en.wikipedia.org/wiki/Observer_pattern)
 * and from an explanation video made by Christopher Okhavri
 * (https://www.youtube.com/watch?v=_BpmfnqjgzQ)
 
 Copyright © 2022 DigiPen (USA) Corporation.
 *********************************************************************/

#pragma once
#include <list> // Note 2025: Not sure what the original reason for using lists instead of vectors was.

// Note 2025: This file contained a couple things, most notably function signatures for our
// messaging system including "BroadcastResolutionFunc".
#include "FunctionTypedefs.h"


/*!
 * @brief 
 *   Interface for an object that can observe another.
*/
class IObserver {
public:

	IObserver(){}
	
	/*!
	 * @brief 
	 *   Instantiates the observer with at least one observable object to listen to.
	 *   Starts the list with just one observer. Most objects won't have more than one.
	 * @param object 
	 *   The object to observe.
	*/
	IObserver(Broadcaster* broadcaster, BroadcastResolutionFunc* func) { resolutionFuncs.emplace_back(std::pair<Broadcaster*, BroadcastResolutionFunc*>(broadcaster, func)); }

	// Note 2025: Non virtual destructor, uh oh.
	/*!
	 * @brief OBLITERATES the Observer object.
	*/
	~IObserver() {}

	/*!
	 * @brief 
	 *   An update function that is called when an observable object is updated.
	 *   Must be object specific, hence it must be overriden and implemented on a
	 *   case-by-case basis.
	*/
	virtual void ReceiveBroadcast(Broadcaster* broadcaster)
	{
		for (auto it = resolutionFuncs.begin(); it != resolutionFuncs.end(); ++it)
		{
			if (it->first == broadcaster && it->second)
			{
				it->second(it->first, this);
			}
		}

	}


	void AddBroadcaster(Broadcaster* broadcaster, BroadcastResolutionFunc* func) 
	{ 
		//std::pair<Broadcaster*, BroadcastResolutionFunc*> temp = std::pair<Broadcaster*, BroadcastResolutionFunc*>(broadcaster, func);
		resolutionFuncs.emplace_back(std::pair<Broadcaster*, BroadcastResolutionFunc*>(broadcaster, func)); 
	}

	std::list<std::pair<Broadcaster*, BroadcastResolutionFunc*>> resolutionFuncs; //!< A list of objects the observer can listen to.
};
