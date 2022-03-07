# PODstore
by Kyle J Burgess

## Summary

`c#` and `c++` endian-independent binary POD (plain old data) file reader/writer with compression and crc options.

## Features

#### Data Types
* Store any number of arrays of POD types indexed by keys of 8-bit ASCII characters
    * 8-bit ASCII characters
    * 8-bit, 16-bit, 32-bit, or 64-bit unsigned integers
    * 8-bit, 16-bit, 32-bit, or 64-bit twos-complement signed integers
    * 32-bit, or 64-bit IEEE floating point numbers
* Individual array size is limited to 2^32 bytes

#### Data Validation
* Data types and sizes are validated on the host by providing the intended size and type of data when both storing and retrieving data.
* Corrupt files are reported on load. See **Checksum** for further data validation options.

#### Endian Independence
* Files keep track of the endianness they were saved in--allowing for optimal performance when writing and reading from a host with the same endianness.
* When a file is loaded into memory, the POD values are converted into the correct endianness for the host.
* Reading a file saved for an endianness that differs from the host will incur a small performance overhead.

#### Checksum
* Choose between no checksum, `adler32` checksum, or `crc32` checksum when saving a file.
* PODstore will automatically read and validate checksums on load.

#### Compression Level
* Choose betweening varying levels of compression based on `zlib`'s DEFLATE comoression levels.

## File Format
All character types are stored in ASCII where the smallest byte corresponds to the left-most character.

#### HEADER
| byte(s) | value(s)
| --- | --- |
| `0...3` | *signature*<br>`PODS` |
| `4...7` | *endianness*<br>`LITE` little endian<br>`BIGE` big endian |
| `8...11` | *checksum*<br>`NONE` no checksum<br>`AD32` adler32 <br>`CR32` crc32 |
| `12...15` | *reserved* |

#### BODY
| byte(s) | value(s)
| --- | --- |
| `16...N` | DEFLATE compressed bytes of a contiguous array of data blocks.<br>See **BLOCK** |

#### TRAILER
| byte(s) | value(s)
| --- | --- |
| `None` or<br>`N+1...N+4` | If *checksum* is `NONE`, then 0 bytes.<br>If *checksum* is `AD32` or `CR32`, then 4 bytes of 32-bit unsigned integer checksum stored in the endian order specified by *endianness*. |

#### BLOCK
| byte(s) | value(s)
| --- | --- |
| `0...3` | *key size*<br>32-bit unsigned integer stored in the endian order specified by *endianness*.<br>Represents the number of characters in the *key*. |
| `4...7` | *data size*<br>32-bit unsigned integer stored in the endian order specified by *endianness*.<br>Represents the number of values in *data*.<br>NOTE: This represents the number of values not the number of bytes.
| `8...11` | *data type*<br>32-bit unsigned integer stored in the endian order specified by *endianness*<br>`0x02000001` 8-bit ASCII character<br>`0x00000001` 8-bit unsigned integer<br>`0x00000002` 16-bit unsigned integer<br>`0x00000004` 32-bit unsigned integer<br>`0x00000008` 64-bit unsigned integer<br>`0x00010001` 8-bit twos-complement signed integer<br>`0x00010002` 16-bit twos-complement signed integer<br>`0x00010004` 32-bit twos-complement signed integer<br>`0x00010008` 64-bit twos-complement signed integer<br>`0x01010004` 32-bit IEEE floating point number<br>`0x01010008` 64-bit IEEE floating point number |
| `12...X` | *key*<br>encoded as *key size* number of 8-bit ASCII characters.
| `X+1...Y` | *data*<br>encoded as *data size* number of values stored contiguously in an array where each value is stored in the endian order specified by *endianness*.
