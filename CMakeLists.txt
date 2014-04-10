cmake_minimum_required (VERSION 2.6)
project (hash-library)

add_library(hash-library crc32.cpp keccak.cpp md5.cpp sha1.cpp sha256.cpp)

