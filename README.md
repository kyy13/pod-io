# PODserializer
c++ (and c# interface) endian-independent binary POD serializer with optional compression and CRC

> key
```
[ ] identifies a number of bytes
( ) identifies a number of file-endianness dependent bytes
{ } identifies a 0s padded block with a multiple of 8 bytes
```
> file format
```
> start crc32 (crc=0)
[4] signature "PODS"
[4] file-endianness
    "LEND" (little endian)
    "BEND" (big endian)
[4] checksum
    "NONE" (none)
    "AD32" (adler32)
    "CR32" (crc32)
[4] reserved (0s)
{
  [N] compressed data (DEFLATE)
}
> end crc32 (crc=x)
(0,4) optional checksum (if checksum is not NONE)
```
> decompressed data format
```
{
  (4) key size (characters)
  (4) data type
      0x02000001 (Char8)
      0x00000001 (UInt8)
      0x00000002 (UInt16)
      0x00000004 (UInt32)
      0x00000008 (UInt64)
      0x00010001 (SInt8)
      0x00010002 (SInt16)
      0x00010004 (SInt32)
      0x00010008 (SInt64)
      0x01010004 (Float32)
      0x01010008 (Float64)
  (4) data size (numbers)
  [N] key
}
{
  (N) data
}
```
