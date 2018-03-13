# Threading


## Threading

* Goals
	* Detach rendering from logic system
	* Ex: physics calculation does not block rendering
	* Async tasks
	* Ex: AI path calculation, map generation, etc


## Threads model

---   main  ---
---   game  ---
---  audio  ---
--- network ---

--- worker #1 ---
--- worker #2 ---
--- ......... ---
--- worker #n ---


## Main thread

* Rendering (cannot be done on another thread on certain OSes)
* Window update/Input management
* Possess all render game data needed by the renderer
	* 3D models
	* Textures
	* Shaders
* Works on a copy of the mutable state
	* Tree of moveable objects (shared with the Physics subsystem)
* Works on read-only immutable data
	* Tree of static objects (ex map)


## Game thread

* Executed on each tick, then sleep and wait for next tick
* On each tick
	* Update Physics
	* Update AI
* Possess game data
	* 2 versions of mutable state: last and new
		* Rendering on last
		* Physics & update on new
	* Immutable data
		* Static objects (ex map)
* Queue of commands to edit game data
	* CREATE(id, type, info)
	* MODIFY(id, changes)
	* DESTROY(id)


## Audio thread

* Low computing stress
* Only an event pool of instructions to destack
* Possess all audio game data files
	* Musics
	* Sounds


## Network thread

* Can sleep if nothing has to be done
* Receive & send buffer watching


## Worker threads

* Thread pool, waiting for new tasks to be executed
* Minimum 2 workers seems good
* Adapt the number of workers according to the number of CPU cores
* Any thread can insert task, useful for computing intensive tasks

## ThreadManager

* Create all threads on the program start


## JobManager

* Contains the Thread pool and wait for jobs