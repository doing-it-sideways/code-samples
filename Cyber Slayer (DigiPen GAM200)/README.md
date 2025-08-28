### A Compilation of Different Major Areas of Code I Worked On
Each subfolder contains a separate system I worked on I think is worth documenting / sharing. Some code contains minor edits from the original to clean up for readability or also to fix bugs that I've noticed since originally writing this code in 2021/2022.

Most formatting and other idiosyncrasies remain as is.

- Messaging:
    - The simple observer pattern implemented for our messaging system.
    - While not nearly my best work on the project or compared to everything else I've done, I feel it gives good context to how much I've improved since. These classes are also used everywhere in the engine.
        - IObserver.hpp: Interface (really an abstract class) defining an object capable of observing a broadcaster.
        - Broadcaster.h/.cpp: Interface (really an abstract class) defining an object capable of broadcasting some set state to a list of observers based on a provided broadcast resolution callback.
- Parser:
    - Simple JSON parsing system using the rapidJSON library. Could handle parsing images and animations, entities (although only the player entity was parsed inside the parser class)
    - Deprecated and unused code removed.
        - Parser.h/.cpp: Defines an interface to load a master list of files in a scene and individual data inside a scene, on top of specialized functions for reading individual pieces of data or special game data like images, animations, arena layouts, etc.
- Ammo Effects:
    - System that modified ammo the player could pick up in the world and applied effects to enemies shot.
    - Note: Picking up the ammo and details about holding it in the player's inventory are not part of this system. Just the effects themselves.
        - AmmoData.h/.cpp: Defines a system to handle ammo effects on an entity or just to proc special effects like lightning or fire.
            - AmmoState: A state the ammo has that effects an entity. Directly tied to an entity. One time uses are fire and forget, stackable effects hold their state until the stack level hits 0 and times out. Contains an AmmoEffectHandler pointer.
            - AmmoEffectHandler: The effect information and handler class for each ammo type. One instance of this class is loaded for each ammo effect. An ammo state will call the handler to either proc an effect (fireball) or affect an enemy (slowing bullet).
                - Each unique handler is a standalone function inside AmmoData.cpp
