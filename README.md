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

`s32 open(u32 flags=0)`

The open function and flags are used to call low level funcions depending on the context provided by

`void close()`

`s32 read(void *buffer, s32 size)`

`s32 write(const void *buffer, s32 size)`

`void rewind()`

`s64 size()`

`s64 position()`

`s64 available()`

`s32 allocate(s64 s)`

`s32 setBlocking(bool b)`

`s32 setTime(s64 alarm,s64 frequency,u32 flags=0)`

`handle* add(handle *h,u32 flags)`

`void remove(handle *h)`

`handle* accept()`

`s32 transfer(u8 command,u8 address,u8 offset, u8 *buffer,u16 length,u32 flags)`

`s32 setAddress(short address)`