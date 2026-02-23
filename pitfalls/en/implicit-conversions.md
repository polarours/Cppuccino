# Implicit Conversions

## 1. Introduction

In C++, type conversion is a common operation that transforms a value of one type into another. **Implicit conversions** are an important mechanism: the compiler performs them automatically, without an explicit cast. They can improve readability and brevity, but when used carelessly they can cause surprising behavior, precision loss, or even security issues.

### 1.1 What Are Implicit Conversions?

An implicit conversion is when the compiler automatically converts a value to another type in certain contexts, without the programmer writing an explicit cast (such as `static_cast` or `type`). These conversions typically occur when types are not identical but are compatible, for example:
- Variable assignment
- Function argument passing
- Arithmetic operands
- Return values or expression evaluation

The goal is to make code more concise—e.g., allowing integers and floating-point numbers to be added directly. But implicit conversions must follow rules defined by the C++ standard to ensure they are safe (or at least predictable).

Implicit conversions are the opposite of **explicit conversions**, which require the programmer to specify them, such as `static_cast<double>(int_number)`. Implicit conversions are “invisible” to the reader; the compiler does them behind the scenes, and if conversion is not possible it will emit an error.

### 1.2 When Do Implicit Conversions Happen?

Implicit conversions occur in many common contexts where the compiler considers a conversion reasonable or safe:
- Arithmetic operations (`+ - * / %`): e.g., `int + double`, where the `int` is implicitly converted to `double`.
- Assignment: e.g., `double d = 3;`, where `3` is implicitly converted to `3.0`.
- Function arguments: e.g., a function expects `double` and you pass an `int`.
- Boolean contexts: e.g., in `if` conditions, nonzero integers convert to `true` and zero to `false`.
- Pointer conversions: e.g., a derived-class pointer implicitly converts to a base-class pointer.
- Enum conversions: enum values can implicitly convert to their underlying integer type.

These are all governed by the standard’s conversion rules to maintain type compatibility and correctness.

## 2. How Implicit Conversions Work

Implicit conversions are not “random.” C++ defines strict and layered conversion sequences, one of the most complex parts of the type system. This section explains how the compiler decides what to do.

### 2.1 Standard Conversion Sequences

When an implicit conversion is required, the compiler follows the **standard conversion sequence**. It is a predictable, ordered set of rules defined by the standard.

A standard conversion sequence usually consists of up to three phases (not all always apply):
- Zero or one lvalue transformation
- Zero or one value conversion
- Zero or one qualification conversion

If the conversion can be fully handled by these steps, it is a standard conversion sequence and has the highest priority among implicit conversions.

#### 2.1.1 Lvalue Transformations

These deal primarily with expression value categories, including:
- Array-to-pointer
- Function-to-pointer
- Reference removal

##### Example 1: Array to pointer conversion
```cpp
void foo(int* p);

int arr[10];
foo(arr);  // arr implicitly converts to int*
```

`arr` has type `int[10]` but decays to `int*` when passed to a function. This is a typical lvalue transformation.

##### Example 2: Reference removal
```cpp
void foo(int x);

int y = 10;
foo(y);
```

`y` is an lvalue, but when passed to `foo` the reference is removed and a pure rvalue is passed.

#### 2.1.2 Value Conversions

This is the category most likely to cause surprises. It includes:
- Integral promotions
- Arithmetic conversions
- Floating-point conversions
- Pointer conversions
- Boolean conversions

##### I. Integral Promotion

Smaller integer types are promoted to `int` or `unsigned int` before arithmetic:

```cpp
char c = 100;
int x = c;   // char → int
```

Notes:
- `char`, `bool`, and `short` are almost always promoted to `int`.
- Promotion happens **before** the operation, not just at assignment.

> Mixed signed/unsigned arithmetic can produce unexpected results—discussed later.

##### II. Arithmetic Conversions

When two arithmetic values of different types participate in an operation, the compiler chooses a common type and implicitly converts both sides to it.

Typical rule (simplified):
> long double > double > float > unsigned long long > long long > unsigned long > long > unsigned int > int > ...

Example:
```cpp
int i = 10;
double d = 5.5;
auto result = i + d;  // i converts to double, result is double
```

##### III. Boolean Conversions

In conditions, almost any type can be implicitly converted to `bool`:
```cpp
int x = 42;
if (x) {  // x converts to bool: nonzero is true
	// ...
}

int* p = nullptr;
if (p) {  // p converts to bool: non-null is true
	// ...
}
```

##### IV. Pointer Conversions

Pointer conversions follow specific rules, for example:
- A derived-class pointer can convert to a base-class pointer.
- `void*` can accept any object pointer.
- A null pointer constant (like `nullptr`) can convert to any pointer type.

```cpp
class Base {};
class Derived : public Base {};

Derived d;
Base* b = &d;  // Derived* converts to Base*
```

#### 2.1.3 Qualification Conversions

These adjust `const`/`volatile` qualifiers:
```cpp
void foo(const int* p);

int x = 10;
foo(&x);  // int* → const int*
```

This conversion is one-way safe:
- You can go from non-`const` to `const`
- You cannot go from `const` to non-`const`

### 2.2 User-Defined Conversions

If a standard conversion sequence cannot complete the conversion, the compiler considers **user-defined conversions**, including:

- Converting constructors
- Conversion operators

#### 2.2.1 Converting Constructors

```cpp
class MyClass {
public:
	MyClass(int x) { }
};

void foo(MyClass a);

foo(10);        // int → MyClass (implicit constructor call)
MyClass obj = 20; // int → MyClass (implicit constructor call)
```

Key points:
- Single-parameter constructors (or with defaults after the first) are eligible.
- They are implicitly allowed by default.
- Common in smart pointers, string classes, numeric wrappers, and unit types.

You can disable implicit conversion with `explicit`:
```cpp
class MyClass {
public:
	explicit MyClass(int x) { }
};

void foo(MyClass a);
foo(10);          // error: no implicit conversion
MyClass obj_1 = 20; // error
MyClass obj_2(30);  // OK: explicit construction
```

#### 2.2.2 Conversion Operators

Typical syntax:
```cpp
operator TargetType() const;    // usually const
```

Example:
```cpp
class MyClass {
public:
	operator int() const { return 42; }
};

MyClass obj;
int x = obj;  // MyClass → int (implicit conversion operator)
```

This should generally be avoided unless absolutely necessary, because it can reduce readability and introduce surprising behavior.

### 2.3 Conversion Ranking and Overload Resolution

When multiple overloads “seem callable,” the compiler compares conversion sequences. Priority is:
1. Exact match
2. Standard conversion
3. User-defined conversion
4. Ellipsis (`...`) match

## 3. Potential Risks of Implicit Conversions

Implicit conversions are powerful, but power comes with risk. Many “convenient” implicit conversions become sources of subtle logic bugs, performance traps, or maintenance disasters.

The modern C++ community (including the C++ Core Guidelines) generally recommends:
By default, mark all single-argument constructors as `explicit` unless you have a clear, well-justified reason to allow implicit conversion.

Below are common risk categories:

### 3.1 Precision Loss and Semantic Drift

The most frequent issue is numeric conversion, especially between integers and floating-point types, which can lead to loss of precision or semantic drift.

```cpp
// Integer -> floating-point
int i = 42;
double d = i;  // usually safe

// Floating-point -> integer
double pi = 3.14159;
int n = pi;  // precision loss, n == 3
```

Summary:
- Integer to floating-point is usually safe (unless the integer is very large).
- Floating-point to integer truncates the fractional part (no rounding).

### 3.2 Signed / Unsigned Mixing

This is subtle and common, especially in comparisons.

```cpp
int a = -1;
unsigned int b = 1;

if (a < b) {
	// always false: a is converted to a large unsigned value
}
```

`a` is converted to `unsigned int`, often becoming $2^{32} - 1$ on 32-bit `unsigned`. The comparison result is not the intuitive “-1 < 1.”

### 3.3 Ambiguity in `bool` Conversion

Here we focus on **assignment-as-condition** cases. Consider:
```cpp
if (x = y) {
	// legal, compilable, extremely dangerous
}
```

Semantics:
1. Execute assignment `x = y`.
2. Convert the assignment result (the new value of `x`) to `bool` for the `if` condition.

> This is why many coding guidelines require `if ((x = y) != 0)` or compiler warnings.

### 3.4 Problems from User-Defined Implicit Conversions

User-defined implicit conversions are more dangerous than built-in ones because they directly affect API semantics, overload resolution, and readability.

Example:
```cpp
class FilePath {
public:
	FilePath(const char* path) : path_(path) {}
private:
	std::string path_;
};

void openFile(FilePath path);

openFile("config.json");  // OK: const char* → FilePath
```

It looks convenient, but:
- “config.json” is silently converted into `FilePath`.
- The call site hides a semantic type change.
- The real API intent is obscured.

As projects grow, such implicit construction often leads to:
- The wrong overload being called
- Surprising overload resolution results
- Unintended construction cost (performance issues)

## 4. Practical Guidance

Implicit conversions are not inherently evil, but trusting them by default is dangerous. The following rules are proven in real-world engineering practice.

### 4.1 Use `explicit` by Default

> Rule: unless you are very sure, make all single-argument constructors `explicit`.

```cpp
class UserId {
public:
	explicit UserId(int id) : id_(id) {}
private:
	int id_;
};
```

This forces the call site to state intent explicitly:
```cpp
UserId uid(42);   // OK
UserId uid = 42;  // error
```

### 4.2 Express Semantics with the Type System, Not Conversions

If two concepts are semantically different:
- Do not rely on “same underlying value.”
- Do not “cheat” with implicit conversions.

For example:
```cpp
struct Width  { int v; };
struct Height { int v; };

void resize(Width w, Height h);
```

`Width` and `Height` both contain `int`, but they express different semantic roles. If you wrote:

```cpp
void resize(int w, int h);
```

then any two integers can be passed, and the compiler can’t distinguish their roles. The type system’s job is not to reduce keystrokes—it is to prevent semantic misuse.

If you allow implicit conversion back to `int`:
```cpp
operator int() const;
```

then `Width` and `Height` lose their distinction, and the semantic boundary you created is silently erased.

## 5. Summary

Implicit conversions are among the most powerful—and dangerous—features in C++’s type system.

In this chapter we saw that:
- Implicit conversions follow strict standard conversion sequences, not arbitrary behavior.
- User-defined implicit conversions can significantly affect API semantics and overload resolution.
- Many real-world bugs are not “logic mistakes” but cases where type boundaries are silently erased.

The final takeaway:

> If a conversion is critical to correctness, it should not be implicit.
