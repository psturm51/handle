# handle

This library is a simple header-only library to make file handling easier in Linux. It exports the handle class which is the only class in the header file

## fileHandleType

The `fileHandleType` determines what methods are usable to the developer for the specific `handle` object. The possible values are:

- FHT_NULL
- FHT_FILE
- FHT_PIPE
- FHT_EXEC
- FHT_DIR
- FHT_SOCKET
- FHT_SERIAL
- FHT_SPI
- FHT_I2C
- FHT_BT
- FHT_KEYBOARD
- FHT_INOTIFY
- FHT_MOUSE
- FHT_TIMER
- FHT_CAMERA
- FHT_MICROPHONE
- FHT_SPEAKER

## Properties

| Property      | Type          |
| ------------- |:-------------:|
| path          | std::string   |
| type          | fileHandleType|
| descriptor    | int           |
| filepointer   | FILE*         |

## Methods

`s32 open(u32 flags=0)`

The open function and flags are used to call low level functions depending on the context provided by `type`.

- File: uses `open` with O_LARGEFILE flag set and passes `flags` to the `open` call
- Exec: uses `popen` in read-only mode with the command being given by `path`
- Camera: uses `open` in read-only mode with the pathname being given by `path`
- Serial: uses `open` on the given `path` with the serial speed given by `flags`. Valid flags include:
    - FLAG_SERIAL_LOW_SPEED (9600 baudrate)
    - FLAG_SERIAL_MED_SPEED (115200 baudrate)
    - FLAG_SERIAL_HI_SPEED (1000000 baudrate)
    - FLAG_SERIAL_ULTRA_SPEED (2000000 baudrate)
- Keyboard: uses `open` in read-only mode and runs the `assertKeyboard` method to ensure it is a keyboard
- INotify: uses `inotify_init` to return the descriptor.
- Timer: uses `timerfd_create` to return the descriptor with clock id CLOCK_MONOTONIC and TFD_NONBLOCK for the flags
- Socket: If `flags` is set to FLAG_SOCKET_CLIENT, `connect` is called. Otherwise, `bind` is called.
- I2C: `open` is called with O_RDWR flag set
- Default: `open` is called with O_RDWR flag set

`void close()`

`s32 read(void *buffer, s32 size)`

`s32 write(const void *buffer, s32 size)`

`void rewind()`

`s64 size()`

`s64 position(s64 pos=0)`

`s64 available()`

`s32 allocate(s64 s)`

`s32 setBlocking(bool b)`

`s32 setTime(s64 alarm,s64 frequency,u32 flags=0)`

`handle* add(handle *h,u32 flags)`

`void remove(handle *h)`

`handle* accept()`

`s32 transfer(u8 command,u8 address,u8 offset, u8 *buffer,u16 length,u32 flags)`

`s32 setAddress(short address)`

`s32 getCameraCapabilities(struct v4l2_capability *cap);`

## Support

If you've found this project helpful, please donate to support the project.

### Bitcoin

![Bitcoin Wallet](./data/bitcoin_address.png "Bitcoin Wallet Address")

1Hmn14M59F7DWvmyCbdCPhh1kn8tkuKQUr

### Litecoin

![Litecoin Wallet](./data/litecoin_address.png "Litecoin Wallet Address")

LWxVeCezdD7QJySrnaSwUe2Kv2L7A4bxMa

### Ethereum

![Ethereum Wallet](./data/ethereum_address.png "Ethereum Wallet Address")

0x497af715daa68dae5be542fbe2171174bf352092