# From Value Categories to Reference Binding

> While various resources about value categories can be found online, this article aims to systematically and thoroughly explain the concept and evolution of value categories in C++, helping readers build a comprehensive understanding of C++ value categories.

## 1. Introduction

### 1.1 Background and Significance of Value Categories

In C++, we're accustomed to understanding programs from the perspective of "types":

```cpp
int x = 0;
```

However, knowing only that the type of `x` is `int` **is insufficient to describe the complete semantics of `x` in expressions**.

For example:

```cpp
x;        // an expression
x + 1;    // another expression
```

Both expressions are related to the type `int`, but they have entirely different "properties" at the language level.

C++ introduced **value categories** precisely to answer this question:

> **When an expression is evaluated, how should the compiler interpret "this result"?**

### 1.2 Expressions, Objects, and Results

Before diving into value categories, we must distinguish three often-confused concepts:

- Expression
- Object
- Result of expression evaluation

#### 1.2.1 Expression ≠ Object

```cpp
int x = 1;
x + 2;
```

`x + 2` is a valid expression, but it doesn't **"declare"** or **"name"** any object.

An expression is merely **a set of semantic rules** describing how to compute a result.

#### 1.2.2 Evaluation Result Isn't Necessarily an Object

After evaluation, an expression might yield:

- An existing object
- A temporary object
- A pure computational result (without object identity)

For example:

```cpp
x;        // evaluation result is the existing object x
x + 2;    // evaluation result is a temporary computational result (not an object)
```

Value categories are precisely designed to distinguish these cases.

### 1.3 Object Identity: The Core of Value Categories

In modern C++, the **fundamental criterion** for distinguishing value categories is:

> **Does the result of the expression have object identity?**

We can use an intuitive question to determine this:

> "Is this result a 'recognizable' object that can be identified again?"

#### 1.3.1 Results with Identity

If the result of an expression:

- Can distinguish whether it's the same instance
- Semantically corresponds to a piece of storage

Then it **has identity**.

```cpp
int x = 0;
x;      // has identity
```

#### 1.3.2 Results without Identity

If a result is merely a "computed value" without identifying any object:

```cpp
x + 1;  // doesn't have identity
```

Then semantically it **lacks identity**.

## 2. Evolution and Classification of Value Categories

### 2.1 Development of Value Categories in C++

In early C++ (C++98/03), value categories were primarily divided into two main categories: **lvalues** and **rvalues**.

- **Lvalue**: Originally referred to objects appearing on the left side of an assignment operator that could be assigned to. Later expanded to represent objects with identity that could be accessed and modified through references.
- **Rvalue**: Referred to temporary objects or literals without identity, typically appearing on the right side of assignments and not referenceable.

With C++11's introduction of **rvalue references** and **move semantics**, the definitions of lvalues and rvalues became more complex. C++11 extended value categories, further subdividing lvalues and rvalues into the following categories:

- **Lvalue**: Objects with long lifetimes that have identity. Allocated actual memory addresses.
- **Rvalue**: Temporary objects or literals without identity. Allocated in temporary storage with short lifespans.
- **Xvalue (expiring value)**: A special kind of rvalue with identity that can be moved. Represents objects about to be destroyed whose resources can be transferred and can be polymorphic.
- **Prvalue (pure rvalue)**: Represents temporary computational results without identity, cannot be moved, and cannot be polymorphic.
- **Glvalue (generalized lvalue)**: Includes lvalues and xvalues, representing objects with identity.

Below is a simple diagram showing the relationship between value categories:

![Value Categories Relationship Diagram](/assets/images/value-categories.png)

### 2.2 Why Extend Value Categories?

In the C++98/03 era, with only `lvalue` and `rvalue` classifications, compilers couldn't distinguish between "named objects" and "temporary objects about to be destroyed," forcing even temporary objects to be copied rather than having their resources "transferred."

This directly limited efficient resource management capabilities. For example:

```cpp
std::string s1 = "hello";               // copy constructor
std::string s2 = std::string("world");  // still copy constructor, even for temporary objects
```

By introducing the finer distinctions of `xvalue` and `prvalue`, C++11 completely changed the landscape, enabling compilers to implement:

- Move constructors / move assignment operators
- Perfect forwarding
- Guaranteed copy elision
- Smarter temporary object lifetime management

> If anything is unclear, don't worry—we'll gradually explain these concepts in detail later.

## 3. Deep Understanding of Value Categories

In this chapter, we focus on explaining three main value categories: **lvalue**, **xvalue**, and **prvalue**.

### 3.1 Lvalue

`lvalue` is the most intuitive and common category of value. It represents objects with **identity** that can be accessed and modified through references.

In C++, the definition of `lvalue` can be simply stated as:

> An `lvalue` is a "named, addressable object that can be reused."

#### 3.1.1 Common `lvalues`

##### I. Named Variables

```cpp
int x = 42;     // x is an lvalue
x = x + 1;      // x can be used to access and modify its value
```

##### II. Dereferenced Pointers

```cpp
int* p = &x;    // p is a pointer to x
*p = 100;       // *p is an lvalue that can modify x's value
```

##### III. Functions Returning Lvalue References

```cpp
int& getRef(int& a) {
    return a;    // returns an lvalue reference
}

int y = 10;
getRef(y) = 20; // y's value can be modified through the returned reference
```

##### IV. Array Elements

```cpp
int arr[3] = {1, 2, 3};
arr[0] = 10;    // arr[0] is an lvalue that can modify array elements
```

##### V. Class Member Variables

```cpp
class MyClass {
public:
    int member;
};

MyClass obj;
obj.member = 5; // obj.member is an lvalue that can modify member variables
```

##### VI. String Literals (C-style strings)

```cpp
char* str = "Hello";
str[0] = 'h'; // str[0] is an lvalue
```

The string literal "Hello" itself is of type `const char[]`, and the array name decays to a pointer `char*`, making `str[0]` an lvalue that can modify the string content (though this practice is unsafe, and modern C++ should use `std::string`).

#### 3.1.2 Key Properties of `lvalue`

| Property | Description | Meaning |
| --- | --- | --- |
| Has identity | Yes (persistent storage, can be referenced multiple times) | Can safely bind to non-const lvalue references |
| Addressable (&)? | Yes (except for function names, bit-fields, etc.) | Commonly use & operator to get address |
| Default behavior | Copy (cannot be moved directly) | Large objects require explicit std::move when passing |
| Can be on left side of assignment? | Yes (non-const lvalue) | Supports modification |
| Lifetime | Determined by declaration (automatic, static, dynamic) | Generally safest, but copy overhead may be significant |

#### 3.1.3 Behavior of `lvalue` in Reference Binding

- Can bind to `T&` and `const T&`
- Cannot directly bind to `T&&` (unless converted to `xvalue` via `std::move`)

```cpp
int x = 42;
int& lr = x;           // OK
const int& clr = x;    // OK

// int&& rr = x;       // Error! lvalue cannot directly bind to rvalue reference
int&& rr = std::move(x);  // OK, std::move converts lvalue to xvalue
```

> For more about std::move, please refer to subsequent sections.

### 3.2 Prvalue (Pure Rvalue)

Prvalue is the "purest temporary" category, lacking identity and representing purely computational results or temporary objects used for initialization.

In summary:

> An expression whose evaluation is used to initialize an object or serve as an operand value, without referring to any existing object, is a prvalue.

#### 3.2.1 Common `prvalues`

##### I. Literal Constants

```cpp
int a = 42;          // 42 is a prvalue
double pi = 3.14;    // 3.14 is a prvalue
```

##### II. Temporary Objects

```cpp
std::string s = std::string("hello"); // std::string("hello") is a prvalue
```

##### III. Function Calls Returning Non-References

```cpp
int getValue() {
    return 100; // returned 100 is a prvalue
}
int val = getValue(); // getValue() is a prvalue, val is lvalue
```

##### IV. Lambda Expressions

```cpp
auto lambda = []() { return 42; }; // lambda is a prvalue
int result = lambda();              // lambda() is a prvalue
```

##### V. Arithmetic and Logical Expressions

```cpp
int sum = 1 + 2;        // (1 + 2) is a prvalue
bool flag = (a == b);   // (a == b) is a prvalue
```

#### 3.2.2 Key Properties of `prvalue`

| Property | Description | Meaning |
| --- | --- | --- |
| Has identity | No | Cannot be referenced multiple times |
| Addressable (&)? | No | &42 is illegal |
| Can be moved? | Yes (by default) | Supports move construction |
| Lifetime | Very short (destroyed at end of expression) | Requires binding to reference to extend lifetime |
| Default behavior | Copy elision (C++17+ guaranteed) or move/copy | Most efficient temporary handling |

#### 3.2.3 Behavior of `prvalue` in Reference Binding

- Can bind to `const T&` (extends lifetime)
- Can bind to `T&&` (triggers move)
- Cannot bind to `T&`

```cpp
int&& rr = 1024;          // OK, 42 is prvalue and can bind to rvalue reference
const int& cr = 1024;     // OK, 42 is prvalue and can bind to const lvalue reference
// int& lr = 1024;        // Error! prvalue cannot bind to non-const lvalue reference
```

### 3.3 Xvalue (Expiring Value)

Xvalue (expiring value) is a value category introduced in C++11. It is the **only category that simultaneously belongs to glvalue (has identity) and rvalue (movable)**, specifically designed to address the need for "named objects to be movable."

In summary:
> An expression whose evaluation result specifies an **object with identity** that is **about to be destroyed** (whose resources can be reused) is an xvalue.

#### 3.3.1 Common `xvalues`

##### I. Objects Converted with `std::move`

`std::move` can convert an lvalue to an xvalue, enabling move semantics.

> Move semantics simply means "allowing resources to be transferred from one object to another instead of being copied."

```cpp
int x = 42;
int&& rr = std::move(x); // std::move(x) is an xvalue
```

##### II. static_cast<T&&> and reinterpret_cast<T&&>

```cpp
int x = 42;
int&& rr_1 = static_cast<int&&>(x); // static_cast<int&&>(x) is an xvalue
int&& rr_2 = reinterpret_cast<int&&>(x); // reinterpret_cast<int&&>(x) is also an xvalue
```

- `static_cast<T&&>` is a legal and safe way in C++'s reference binding and move semantics system to change an expression's value category.
- `reinterpret_cast<T&&>` is a special conversion method, more like a compiler directive, and is generally not recommended.

##### III. When an Object Itself Is an `xvalue`, Its Member Access Is Also an `xvalue`

```cpp
class MyClass {
public:
    std::string name;
};

MyClass obj;
std::string&& nameRvalue = std::move(obj).name; // std::move(obj).name is an xvalue
```

#### 3.3.2 Key Properties of `xvalue`

| Property | Description | Difference from `lvalue`/`prvalue` | Meaning |
| --- | --- | --- | --- |
| Has identity | Yes (has address, can be referenced multiple times) | Same as `lvalue`, different from `prvalue` | Can be accessed again, though generally not recommended |
| Addressable (&)? | Yes | Same as `lvalue` | &std::move(x) is legal |
| Can be moved? | Yes (designed specifically to allow moving) | Same as `prvalue`, different from `lvalue` | Triggers move constructor/assignment |
| Lifetime | About to end (at full-expression boundary) | Shorter than `lvalue`, slightly longer than `prvalue` | Original object enters "moved-from" state after use |
| Default behavior | Move | — | Bridge for resource transfer |

#### 3.3.3 Behavior of `xvalue` in Reference Binding

- Can bind to `T&&` (rvalue reference) → triggers move
- Can bind to `const T&` (temporarily extends lifetime)
- Cannot directly bind to `T&` (non-const lvalue reference)

```cpp
int x = 42;
int&& rr = std::move(x);          // OK, xvalue can bind to rvalue reference
const int& cr = std::move(x);     // OK, xvalue can bind to const lvalue reference
// int& lr = std::move(x);        // Error! xvalue cannot bind to non-const lvalue reference
```

## 4. Relationships Between Value Categories and Reference Binding Rules

Previously, we thoroughly analyzed the definitions, sources, properties, and behaviors of lvalues, prvalues, and xvalues separately. Now, let's examine their **relationships** and the most critical application rule: **reference binding**.

Understanding reference binding rules is the hallmark of truly "knowing how to use" value categories. Because when deciding whether an expression can bind to a particular reference, the C++ compiler **relies entirely on its value category**.

### 4.1 Comprehensive Comparison Table of Value Categories

| Property / Value Category | `lvalue` | `prvalue` | `xvalue` |
|---------------------------|----------|-----------|----------|
| Has identity | Yes | No | Yes |
| Addressable (&)? | Yes (typically) | No | Yes |
| Can be moved? | No (copies by default) | Yes | Yes |
| Lifetime | Persistent (determined by declaration) | Very short (destroyed at expression end) | About to end (at full-expression boundary) |
| Default behavior | Copy | Copy elision or move/copy | Move |
| Typical sources | Variables, array elements, *p, obj.member | Literals, temporary objects, function return values | std::move(), static_cast<T&&> |
| Engineering keywords | Safe, persistent, modifiable | Temporary, efficient, zero-cost optimization | Bridge, explicit resource transfer |

### 4.2 Reference Binding Rules Overview

The C++ compiler determines binding legality based on the expression's value category + cv qualifiers + reference type:

| Reference Type | `lvalue` | `prvalue` | `xvalue` | Notes and Engineering Implications |
|----------------|----------|-----------|----------|-----------------------------------|
| `T&` | ✅ | ❌ | ❌ | Accepts only modifiable named objects (most restrictive, prevents accidental modification of temporaries) |
| `const T&` | ✅ | ✅ | ✅ | Most versatile read-only parameter, binds to any value category, extends temporary lifetime |
| `T&&` | ❌ | ✅ | ✅ | Non-const rvalue reference, designed specifically for `std::move` / move semantics |
| `const T&&` (const rvalue reference) | ❌ | ✅ (const) | ✅ (const) | Rarely used, typically deliberately used to disable moving or for special constraints |
| `T&&` (forwarding reference in templates) | ✅ | ✅ | ✅ | Forwarding references, core mechanism of perfect forwarding |

**Rule Summary Mnemonic** (for quick memorization):
- `T&`: accepts only lvalues
- `const T&`: accepts everything (most permissive, commonly used for read-only parameters)
- `T&&`: accepts only rvalues
- `T&&` (in templates): accepts both lvalues and rvalues (perfect forwarding)

### 4.3 Typical Binding Scenario Examples

```cpp
int x = 42;                      // x is an lvalue

// Scenario 1: Regular parameter passing
void f1(int& r) {}               // accepts only lvalues
f1(x);                           // OK
// f1(42);                       // Error

// Scenario 2: Const reference (universal)
void f2(const int& cr) {}
f2(x);                           // OK
f2(42);                          // OK, temporary lifetime extended

// Scenario 3: Rvalue reference (move-specific)
void f3(int&& rr) {}
// f3(x);                        // Error
f3(42);                          // OK, prvalue
f3(std::move(x));                // OK, xvalue

// Scenario 4: Perfect forwarding prototype (explained in detail later)
template<typename T>
void forwarder(T&& arg) {
    f3(std::forward<T>(arg));    // preserves original category
}
forwarder(x);                    // passes xvalue to f3
forwarder(42);                   // passes prvalue to f3
```

## 5. Common Misconceptions About Value Categories

We've systematically outlined the C++ value category system and its precise rules for reference binding. This section will analyze several common misconceptions from an engineering practice perspective to help readers avoid pitfalls in actual coding.

### 5.1 A Common Misunderstanding: Value Category ≠ Lvalue/Rvalue Reference

An extremely common and confusing misconception in engineering is:

> "Lvalues are what can bind to `T&`, and rvalues are what can bind to `T&&`"

This description is inaccurate. Consider this example:

```cpp
int&& rr = 1024; // OK
rr = 100;        // OK
```

Here, `1024` is a `prvalue` that can bind to `int&&`, which follows the rules. However, `rr` itself is an `lvalue` because it has a name and identity. Even though `rr` was declared as an rvalue reference, it's still an `lvalue`.

> Once an rvalue is named, it becomes an `lvalue` in expressions.

This is also why:

```cpp
void f(int&& x);

int&& rr = 1024;
// f(rr);         // Error: rr is an lvalue
f(std::move(rr)); // Correct: explicitly converted to xvalue
```

### 5.2 The Semantic Essence of "Moved-From Objects"

Consider this example:

```cpp
std::string s = "hello";
auto t = std::move(s);
```

Here, `std::move(s)` converts `s` to an `xvalue`, allowing the move constructor to be called. **However, after the move, `s` is still an `lvalue`**, though its state has become "moved-from."

A more accurate description would be:

> `std::move` doesn't actually move anything; it only changes the expression's value category.

```cpp
std::move(s); // an xvalue
```

The actual moving happens in:

- Move constructors
- Move assignment operators

The true semantics of a "moved-from object" are:

> The object still has complete identity and lifetime, but its resources are in a "valid but unspecified state."
> For more about `std::move`, please refer to the [move-is-not-moving](/docs/en/move-is-not-moving.md) section.

## 6. Conclusion

In this chapter, we've systematically introduced C++ value categories, including lvalues, prvalues, and xvalues. We've also thoroughly explored their definitions, sources, properties, and behavior in reference binding. Understanding value categories is fundamental to mastering C++ references, move semantics, and resource management. This is why we'll frequently mention value categories in subsequent chapters.

> Readers who wish to gain deeper insights into C++ references and move semantics can consult relevant books such as *Effective Modern C++*, *Effective C++*, and *C++ Primer*, which greatly help in understanding value categories.