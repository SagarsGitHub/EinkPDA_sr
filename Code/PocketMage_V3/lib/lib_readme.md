# Pocketmage Library
This is a basic overview of pocketmage library. Any suggestions/edits to further improve the modularity, readability, and safety of the library are welcome.

### To Do:

1. progressively move globals out of globals.h replace them with variables owned by library. 
2. Add getters for reference from other parts of code.
3. combine setup files into one setup file.
4. refactor pocketmage:: to use only pocketmage::method().

## Essentials:

1. Access each library class methods with:

> CLASS().method()

2. Pocketmage system is a namespace

> pocketmage::typeOfMethod::method()<br />
> current pocketmage method types:
> - file
> - time
> - power
> - debug

## Classes:

- buzzer BZ()
- real time clock CLOCK()
- eink EINK()
- keyboard KB()
- oled OLED()
- sd SD()
- capacitive touch TOUCH()
- system pocketmage()

## Adding to the library

1. Define a class for the object and include h file in pocketmage.h
2. add setup and wire methods as well as an access method to setup/systemSetup.cpp

    // Setup for Class
    void setupFOO() {
    }

    // Wire function for class
    // add any global references here
    void wireFOO() {
    }

    // Access for other apps
    inline PocketmageFOO& FOO() {
      static PocketmageFOO foo();
      return foo;
    }

3. Each class has a singleton object created during setup.
4. Each class references pointers to globals that should exist for the lifetime of the program. These pointers are wired for each class during setup. 

> (class_variable_) && (*class_variable_ == true) checks that variable pointer is not null before checking value
> this will be replaces with variables owned by different parts of the library accessed with getters