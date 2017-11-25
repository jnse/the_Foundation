# c_Lite

Simplicity of C11, power of object-orientation! API usability is priority #1. 

## Conventions

c_Lite is built on many conventions.

### General

- Global symbols like type names and macros use the `i` prefix (e.g., `iMin`).
- Method names and variables use camelCase.
- Type names and classes start with a capital letter.
- Preprocessor macros and constants use naming similar to classes (e.g., 
  `iDeclareType`).

### Classes

All class members use the class name as a suffix, e.g., `length_String`. This
improves readability and associates the first argument (the `d` object,
equivalent to `this` in C++) with the type of the class.

A static/private member of a class additionally adds an extra underscore to the
suffix, e.g., `element_Array_`.

Type names are declared with the `iDeclareType` macro. The implementation 
struct is always called `struct Impl_{Name}`.

Macros are used to define member functions with default values for parameters.

### Construction and destruction

- `new` allocates memory but does not initialize it. 
- `delete` deinitializes and deletes the object.
- `init` initializes the object's memory (e.g., zeroing it).
- `deinit` releases any memory/resource allocations, but not delete the object
   itself nor is any of the object's state reset to zero.

### Iterators

- The member `value` is a pointer to the current element. If NULL, the 
  iteration will stop.
- Iterators may have additional members depending on the type of the data
  and the requirements for internal state.
