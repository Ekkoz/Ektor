# Memory


## Allocator

* External libraries use 'new'
	* Overload the default new to a custom allocator ? Which one ?
	* External libraries don't specify which custom allocator to use
	* Problem: if the Frame custom allocator became the default new, what is the overhead ? Worst or better than the default 'new' ?
* Using explicit custom allocators (no 'new', no 'delete', no 'malloc')
* Each custom allocator contain 2 methods
	* 'allocate' base on type template
	* 'malloc' to only allocate raw data (no constructor has to be called)
* Can fill memory with value (ex 0x66) after deallocation to detect usage after free and uninitialize memory


## Custom allocator example

```
void *p = a.allocate(sizeof(T));
T *obj = new (p) T(arg1, arg2);
```

Also useable with macro


## Stack allocator

* Big chunks of memory
* Each chunk has to be saved
* The top pointer is increased
	* On each allocation
* The top pointer is decreased
	* On a free
	* Only if the freed block is on the top
	* If it is, it frees all underlying ready to free blocks
* Each allocated block has a header
	* Contains if the block is freed
* Useful to allocate data for one frame duration (Single frame memory)
* In the future: add a defragmenting method for low memory systems
* => Perfect for many low-size allocations (in a loop for example)


## Pool allocator

* Big chunks of memory
* Each big chunk has to be saved
* Divided in small chunks of specific size
* Linked list of freed chunks (headers directly in available small chunks)
* On a allocation, an available small chunk is chosen and deleted from list
* On a free, the small chunk is pushed on the list
* Useful to build an Object Pools factory
* => Perfect for large allocations because of the low waste and small lists


## Proxy allocator

* Not a real allocator
* Only wrap another one
* Keep track of all allocation/desallocation
* Has a name
* Can track memory consumption
* Can add static data before and after memory alloc (ex canary) and test if static data has been written on deallocation
* Destructor can assert if there is memory leaks
* The proxy allocator can be completly shut down during release compilations


## Overflow detector allocator

* Allocate memory through pages
* Add readonly blocks before and after the allocated memory
	* mprotect() on Unix
	* VirtualProtect() on Windows


## Startup

* Allocators are Singleton
	* They are allocated on the stack on program startup


## Future work

* Thread-safe allocator
    * Wrap an instance of each allocator inside a thread instance class
    * Ex: currentThread()->getAllocator();
* Temp allocator with static stack buffer
	* Very low quantity of memory
	* Very small life duration