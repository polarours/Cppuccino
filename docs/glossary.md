# Glossary

Quick reference for C++ terms used in this repository.

## A

**ABI (Application Binary Interface)**
Defines how functions are called at the binary level, including calling conventions, name mangling, and data layout. Changes to ABI can break compatibility between compiled code.

**Allocator**
An object that manages memory allocation and deallocation. STL containers use allocators to allocate memory for their elements.

**Atom**
The smallest unit of memory that can be accessed atomically without data races. Atomic operations on atoms are guaranteed to be thread-safe.

## B

**Backtracking**
A algorithm technique where the solver tries a choice, then backtracks if it leads to a dead end. Used in regex matching and parsing.

**Base Class**
A class from which other classes inherit. Provides common interface and implementation for derived classes.

**Behavior, Undefined (UB)**
Behavior that the C++ standard does not specify. Common causes include null pointer dereference, buffer overflow, and use-after-free.

**Bit Field**
A data structure that specifies the number of bits to use for a member variable, allowing compact storage of data.

**Buffer Overflow**
Writing data beyond the boundaries of a buffer, leading to memory corruption and potential security vulnerabilities.

## C

**CRTP (Curiously Recurring Template Pattern)**
A technique where a class derives from a template instantiation of itself. Used for static polymorphism and compile-time interface enforcement.

**Const Correctness**
Using `const` to indicate that a function or variable does not modify state. Helps prevent bugs and enables compiler optimizations.

**Copy Elision**
Compiler optimization that eliminates intermediate copies when returning objects from functions. Guaranteed in C++17 for certain cases (guaranteed copy elision).

**Coroutine**
A function that can be suspended and resumed. Enables asynchronous programming without callbacks.

**Concept (C++20)**
A named set of template requirements that constrains template parameters. Enables better error messages and more readable code.

**Consteval (C++20)**
Specifier that forces a function to be evaluated at compile time. Cannot be called at runtime.

**Constinit (C++20)**
Specifier that ensures a variable is initialized at compile time, preventing dynamic initialization order issues.

**constexpr**
Specifier that allows functions and variables to be evaluated at compile time when possible.

**Condition Variable**
A synchronization primitive that allows threads to wait for a condition to become true.

## D

**Dangling Pointer**
A pointer that points to memory that has been freed or gone out of scope. Dereferencing it is undefined behavior.

**Deadlock**
A situation where two or more threads are blocked forever, each waiting for the other to release a resource.

**Destructor**
A special member function called when an object goes out of scope or is deleted. Used for cleanup and resource release.

**Dynamic Dispatch**
Runtime mechanism for selecting which function to call based on the actual type of an object. Implemented via vtables.

## E

**Exception Safety**
Guarantees about what happens when exceptions are thrown. Levels: basic, strong, and no-throw.

**Executor**
An object that manages thread execution, often used with thread pools to dispatch tasks to worker threads.

## F

**Forwarding Reference**
A reference that can bind to both lvalues and rvalues. Formed as `T&&` where `T` is a template parameter. Used for perfect forwarding.

**Future**
An object that represents the result of an asynchronous operation. Can be used to retrieve the result or wait for completion.

## G

**Garbage Collection**
Automatic memory management that reclaims memory no longer reachable by the program. Not used in C++ (manual or RAII-based management is preferred).

**Glvalue (Generalized Lvalue)**
An expression whose identity (address) can be taken. Includes both lvalues and xvalues.

**Global Variable**
A variable declared outside any function or class, with global scope and static storage duration.

**Grouping (Regex)**
Parentheses in regular expressions that group patterns together for quantifiers or alternation.

## H

**Handle**
A value that refers to a resource (like a file descriptor or socket). Must be properly closed/released.

**Header File**
A file containing declarations (functions, classes, templates) that can be included in multiple source files.

**Heap**
Memory area for dynamic allocation. Objects allocated with `new` or `malloc` live on the heap.

**Homogeneous Container**
A container that stores elements of the same type.

## I

**Iterator**
An object that points to elements in a container and supports traversal. Categories: input, output, forward, bidirectional, random access.

**Implementation (PImpl)**
Technique to hide implementation details by storing them behind a pointer. Reduces compilation dependencies.

**Inline Function**
A function defined in a header file that can be expanded at each call site, avoiding function call overhead.

**Integer Promotion**
Automatic conversion of smaller integer types to `int` or `unsigned int` in expressions.

## J

**Job Control**
Shell feature for managing background and foreground processes. Includes signals (SIGTSTP, SIGCONT) and process groups.

**Join (Thread)**
Blocking operation that waits for a thread to finish execution before continuing.

## K

**Key-Value Store**
A data structure that stores associations between keys and values. Supports operations like get, set, and delete.

## L

**Lambda**
An anonymous function object. Can capture variables from the enclosing scope. Defined with `[]` syntax.

**Lifetime**
The period during which an object exists in memory. From construction to destruction.

**Lvalue**
An expression that refers to an object with identifiable memory location. Can appear on the left side of assignment.

**Lock**
A synchronization mechanism that prevents multiple threads from accessing shared data simultaneously.

**Literal**
A constant value directly written in code, like `42`, `3.14`, or `"hello"`.

## M

**Memory Model**
Defines the rules for concurrent memory access in multithreaded programs. Determines when operations are visible to other threads.

**Move Constructor**
A constructor that transfers resources from a temporary object to a new object. Enables efficient transfer of ownership.

**Move Semantics**
Feature that allows transferring resources instead of copying. Enabled by rvalue references and move constructors/assignment operators.

**Mutex**
A synchronization primitive that protects shared data from concurrent access. Only one thread can hold the mutex at a time.

**Multithreading**
Execution of multiple threads concurrently, allowing parallel processing and improved performance.

**Member Variable**
A variable declared inside a class, storing the state of an object.

## N

**NFA (Nondeterministic Finite Automaton)**
A theoretical model used in regex matching. Can be in multiple states simultaneously.

**Noexcept**
Specifier indicating a function will not throw exceptions. Important for move constructors and swap functions.

**Narrowing Conversion**
Implicit conversion that may lose data, like converting `double` to `int`. Generally discouraged in modern C++.

## O

**Object Slicing**
Problem that occurs when a derived class object is assigned to a base class variable by value, losing the derived class data.

**Overload Resolution**
Process by which the compiler selects the best function to call when multiple functions have the same name.

**Operator Overloading**
Defining custom behavior for operators (like `+`, `-`, `==`) for user-defined types.

## P

**PImpl (Pointer to Implementation)**
Technique to hide implementation details by storing them behind a pointer. Reduces compilation dependencies.

**Pipeline**
A sequence of processes where the output of one is the input of the next. Used in shell commands and data processing.

**Prvalue (Pure Rvalue)**
An expression that initializes an object or computes a value. Does not have identity (address).

**Promise**
An object that sets the value for a future. Used to communicate between threads.

**Pointer**
A variable that stores the memory address of another variable.

**Predicate**
A function that returns a boolean value, used for filtering or condition checking.

## Q

**RAII (Resource Acquisition Is Initialization)**
Idiom where resource lifetime is bound to object lifetime. Resources are acquired in constructor, released in destructor.

**Rule of Zero**
Design principle: if a class manages no resources, don't declare any special member functions.

**Rule of Three**
If a class needs a custom destructor, copy constructor, or copy assignment operator, it likely needs all three.

**Rule of Five**
Extension of Rule of Three: if you define any of destructor, copy constructor, copy assignment, move constructor, or move assignment, consider defining all five.

**Range (C++20)**
A concept representing a sequence of elements that can be iterated over. Used with range-based for loops and range adaptors.

**Range Adaptor (C++20)**
A function object that transforms a range into another range, like `filter` or `transform`.

## S

**SFINAE (Substitution Failure Is Not An Error)**
Template metaprogramming technique where invalid template argument substitutions are not errors but simply remove the function from overload set.

**Shared Pointer**
Smart pointer that shares ownership of an object. Uses reference counting to track owners.

**Smart Pointer**
RAII wrapper around raw pointers that manages lifetime automatically. Examples: unique_ptr, shared_ptr, weak_ptr.

**Stack**
LIFO data structure used for function call frames, local variables, and recursion.

**Static Polymorphism**
Compile-time polymorphism using templates or CRTP. No virtual dispatch overhead.

**Synchronization**
Coordination of concurrent access to shared resources. Includes mutexes, condition variables, and atomics.

**Span (C++20)**
A non-owning view over a contiguous sequence of objects. Provides safe access without ownership.

**Structured Binding (C++17)**
Syntax that unpacks aggregate types into named variables: `auto [x, y] = pair;`

**Statement**
A single line of executable code, like an assignment, function call, or loop.

**Storage Class**
Specifies the lifetime and visibility of a variable: auto, static, extern, register.

## T

**Template**
A blueprint for generating functions or classes. Parameters can be types, values, or templates themselves.

**Thread**
A sequence of instructions that can be executed concurrently with other threads. Managed by the OS or runtime.

**Thread Pool**
A pattern where a fixed number of threads execute tasks from a queue. Avoids overhead of creating threads per task.

**Thread Safety**
Property of code that functions correctly when called from multiple threads simultaneously.

**Type Deduction**
Compiler's ability to determine the type of a variable or expression automatically.

**Type Erasure**
Technique that hides the concrete type behind a generic interface, like `std::function` or `std::any`.

**Tuple**
A fixed-size collection of heterogeneous values, like `std::tuple<int, std::string, double>`.

## U

**Unique Pointer**
Smart pointer that has exclusive ownership of an object. Cannot be copied, only moved.

**Undefined Behavior**
See Behavior, Undefined.

**Universal Reference**
See Forwarding Reference.

## V

**Value Category**
Classification of expressions based on whether they have identity and whether they can be moved from. Categories: lvalue, prvalue, xvalue, glvalue, rvalue.

**Virtual Function**
A function that can be overridden in derived classes. Called through virtual dispatch at runtime.

**Vtable (Virtual Table)**
Table of function pointers used for virtual dispatch. Each class with virtual functions has one.

## W

**Weak Pointer**
Smart pointer that observes an object managed by shared_ptr without extending its lifetime. Used to break circular references.

**Worker**
A thread that executes tasks from a queue, typically part of a thread pool.

## X

**Xvalue (Expiring Value)**
An expression that has identity but can be moved from. Created by rvalue casts and move operations.
