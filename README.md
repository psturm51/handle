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

The open function and flags are used to call low level functions depending on the context provided by `type`

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

## Support

If you've found this project helpful, please donate to support the project.

#### Bitcoin

![Bitcoin Wallet](./data/bitcoin_address.png "Bitcoin Wallet Address")

1Hmn14M59F7DWvmyCbdCPhh1kn8tkuKQUr

#### Litecoin

![Litecoin Wallet](./data/litecoin_address.png "Litecoin Wallet Address")

LWxVeCezdD7QJySrnaSwUe2Kv2L7A4bxMa

#### Ethereum

![Ethereum Wallet](./data/ethereum_address.png "Ethereum Wallet Address")

0x497af715daa68dae5be542fbe2171174bf352092