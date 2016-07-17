# libvot Coding Style

## Contribute to libvot
Thanks for your interests in contributing code to *libvot*! Before send your code, we'd like to bring your attention about the coding style of *libvot*.

There are two important rules to follow:
* When making changes, try to conform with the style and conventions of the surrounding code.

* Strive for clarity, even if it occasionally breaks the guidelines. After all, coding styles are applied to facilitate the development process, rather than against it.

Specific guidelines are detailed in the following sections.

###Language and Encoding
* American Spelling for all doc-strings variable names and comments.

* Use ASCII where possible, avoid special unicode characters such as '÷', '¶' or 'λ'.

* Use UTF8 encoding for all source files where unicode characters are required.

###Naming
* We largely follow the variable naming style of [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).

* Use descriptive names for global variables and functions.

* All file names are in lowercase letters, with hyphen ('-') to connect words.

###Operators and Statements
* Spaces should be used around assign operators (=, +=, /= and so), one space on each side of it:

```cpp
/* Don't: */
foo= bar;
/* Do: */
foo = bar;
```

* Space should be used between statement and brace:

```cpp
/* Don't: */
for(ob = bmain->object.first; ob; ob = ob->id.next) {}
for( ob = bmain->object.first; ob; ob = ob->id.next ) {}
/* Do: */
for (ob = bmain->object.first; ob; ob = ob->id.next) {}
```

* The "pointer qualifier, '*', should be with the variable name rather than with the type.

```cpp
/* Don't: */
char* s;
/* Do: */
char *s;
```

###Indentation
* In C/C++ sources use TABs for indentation. Spaces are only allowed to keep wrapped part of line aligned with first part of line.

####Preprocessor Indentation
* Preprocessor directives should never be indented, the hash/pound (# character) should always be the first character on the line, however indentation after the hash can be used.

```cpp
/* DON'T */
int test(void)
{
    #ifdef WIN32
    int a;
    #else
    size_t a;
    #endif
}
 
 
/* DO */
int test(void)
{
#ifdef WIN32
    int a;
#else
    size_t a;
#endif
}
```

###Braces

####Function Braces
* Functions should always have braces on new lines.

```cpp
/* Don't: */
void func(void) {
    return 1;
}
/* Do: */
void func(void)
{
    return 1;
}
```

####Switch Statement Braces
* The body of switch statement should be indented, so case keyword has next indentation level relative to switch, body of case statement has got next indentation level relative to case:

```cpp
/* Don't: */
switch(value){
case TEST_A: {
    int a = func();
    result = a + 10;
}
break;
case TEST_B:
    func_b();
case TEST_C:
case TEST_D: {
    func_c();
}
break;
}
 
/* Do: */
switch (value) {
    case TEST_A:
    {
        int a = func();
        result = a + 10;
        break;
    }
    case TEST_B:
        func_b();
        /* fall-through */
    case TEST_C:
    case TEST_D:
    {
        func_c();
        break;
    }
}
```

####Other Statements (if, while, for, ...)
* For everything else, braces should be placed on the same line.

```cpp
/* Don't: */
    if (a == b)
    {
        d = 1;
    } else {
        c = 2;
    }
/* Do: */
    if (a == b) {
        d = 1;
    }
    else {
        c = 2;
    }
/* Don't: */
    for (i = 0; i < 10; i++)
        {
        func();
        }
/* Do: */
    for (i = 0; i < 10; i++) {
        func();
    }
```

####Exceptions to Braces on the Same Line
* When flow control (if, for, while) is broken up into multiple lines, it reads better to have the brace on its own lines.

```cpp
/* Don't: */
    if ((very_long_function_check(that, has, many, arguments)) &&
        (another_very_long_function(some, more, arguments)) &&
        some_more_checks_this_gets_tedious(but, its, readable)) {
        some_other_check_this_could_be_confusing ? func_a() : func_b();
    }
 
/* Do: */
    if ((very_long_function_check(that, has, many, arguments)) &&
        (another_very_long_function(some, more, arguments)) &&
        some_more_checks_this_gets_tedious(but, its, readable))
    {
        (some_other_check_this_could_be_confusing && n) ? func_a() : func_b();
    }
/* Don't: */
    for (a = long_function_name();
         a < some_long_check;
         a = some_long_iterator()) {
        *a_other = a;
    }
 
/* Do: */
    for (a = long_function_name();
         a < some_long_check;
         a = some_long_iterator())
    {
        *a_other = a;
    }
```

###Documentation
* Use [Doxygen comment style](http://www.stack.nl/~dimitri/doxygen/manual/docblocks.html) to comment your code.

###Git Usage
* Each commit message should start with '+' for bringing in new features, '*' for modification of existing code, and '-' for removal of a feature/module.
* Use `git pull --rebase` to synchronize with upstream, because we are aiming for a linear commit history.
