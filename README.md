# the_Foundation: a C11 library

Simplicity of C11 combined with the power of object-orientation! API usability
is priority #1.

## Introduction

C++ is an awesome language. It is also extremely complex and each of its modern
updates adds even more complexity. It attempts to be low-level while supporting
high-level programming styles; this conflict prevents it from being truly
high-level while compromising the benefits of low-level C. The complexity also
makes compiling C++ programs much slower than C.

**the\_Foundation** is a C11 library (and a coding convention) for
object-oriented programming that has been designed from the point of view of
someone who appreciates the user-friendliness of Qt and the philosophy behind
C++ STL. The preprocessor is used heavily to provide flexibility and
convenience for the programmer.

### Wait, what about GLib?

GLib is a solid library that serves an important function in GTK. It also has
its own coding style and naming conventions that come with a set of assumptions
how things are expected to work. However, the\_Foundation aims to be more
light-weight and bolder in its conventions to achieve specific design goals.

## Conventions

### General

- Global symbols like type names and macros use the `i` prefix (e.g., `iMin`).

- Method names and variables use camelCase.

- Type names and classes start with a capital letter (following the `i` 
  prefix).

- Preprocessor macros and constants use naming similar to classes (e.g.,
  `iDeclareType`). They begin with a verb.

- The general base class `iObject` implements reference counting. The class of
  an object determines how the object is deinitialized.

- In functions where an object is passed as an argument, the reference count
  must be incremented if the function holds a pointer to the object. Otherwise,
  the reference count should not be modified.

### Types and classes

All class members use the class name as a _suffix_, e.g., `length_String`. This
improves readability and associates the first argument (the `d` object,
equivalent to `this` in C++) with the type of the class.

A static/private member of a class additionally adds an extra underscore to the
suffix, e.g., `element_Array_`.

Type names are declared with the `iDeclareType(Name)` macro. The implementation
struct is always called `struct Impl_Name` that has a typedef alias called
`iName`. The `Impl_Name` struct should be kept opaque (only declared in the
header) by default to hide the implementation details.

`static inline` functions (or macros) are used to define member functions with
default values for parameters.

### Construction and destruction

For a given type `Type`:

- `new_Type` allocates memory for a Type instance from the heap and initializes it by calling the init method `init_Type`. The macro `iMalloc(Type)` is provided for convenience.
- `delete_Type` deletes the object after deinitializing with `deinit_Type`.
- `init_Type` initializes an object's memory (e.g., zeroing it) inside a memory buffer with uninitialized contents. The memory can be located anywhere (heap or stack).
- `deinit_Type` releases any memory/resource allocations, but not delete the object itself nor is any of the object's state reset to zero. The memory contents are considered undefined afterwards.

### Iterators

- The member `value` is a pointer to the current element. If NULL, the iteration will stop.
- The `value` pointer must be the first member in an iterator struct. Derived iterators should use an anonymous union to alias the value pointer to an appropriate type, while remaining compatible with the base class's iterator implementation.
- Iterators may have additional members depending on the type of the data and the requirements for internal state.
- Non-const iterators have a method called `remove` if the currently iterated element can be removed during iteration. Using this ensures that memory owned by the container itself will be released when the element is deleted.

## License

[BSD-2-Clause](https://opensource.org/licenses/BSD-2-Clause)

## Author

the\_Foundation has been written by Jaakko Keränen <jaakko.keranen@iki.fi>.
