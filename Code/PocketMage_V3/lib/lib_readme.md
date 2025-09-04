# Pocketmage Library
This is a basic overview of pocketmage library

## Essentials:

1. Access each library class methods with:

> CLASS().method()

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