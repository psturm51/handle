# handle

This library is a simple header-only library to make file handling easier in Linux. It exports the handle class which is the only class in the header file

## Properties

| Property      | Type          |
| ------------- |:-------------:|
| path          | std::string   |
| type          | fileHandleType|
| descriptor    | int           |
| filepointer   | FILE*         |

## Methods

`int open(u32 flags=0)`

The open function and flags are used to call low level funcions depending on the context provided by

`void close()`

`int read(void *buffer, int size)`

`int write(const void *buffer, int size)`

`void rewind()`

`s64 size()`

`s64 position()`

`s64 available()`

`int allocate(s64 s)`

`int setBlocking(bool b)`

`int setTime(s64 alarm,s64 frequency,u32 flags=0)`

`handle* add(handle *h,u32 flags)`

`void remove(handle *h)`

`handle* accept()`

`int transfer(u8 command,u8 address,u8 offset, u8 *buffer,u16 length,u32 flags)`

`int setAddress(short address)`