cmake_minimum_required (VERSION 3.12)
project (hash-library LANGUAGES CXX)

include(GNUInstallDirs)

add_library(hash-library SHARED hash.cpp crc32.cpp keccak.cpp md5.cpp sha1.cpp sha256.cpp sha3.cpp sha512.cpp fuzzy.cpp edit_dist.cpp ssdeep.cpp hashes.cpp cryptocurrency.cpp utils.cpp base58.cpp)
if (WIN32)
            set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -MTd")
            set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -MTd")
            set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -MT")
            set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -MT")
			set_target_properties(hash-library PROPERTIES COMPILE_DEFINITIONS "HASHLIB_EXPORT")
else()
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
        add_definitions(-fPIC)
    endif()
endif()

if (Tests MATCHES [Oo][Nn])
    add_executable(hash-library-test tests/tests.cpp hash.cpp crc32.cpp keccak.cpp md5.cpp sha1.cpp sha256.cpp sha3.cpp sha512.cpp fuzzy.cpp edit_dist.cpp ssdeep.cpp hashes.cpp cryptocurrency.cpp utils.cpp base58.cpp)
    if (WIN32)
        target_compile_definitions(hash-library-test PRIVATE HASHLIB_EXPORT)
    endif()
endif()


# make install command for linux machines:
if ("${CMAKE_SYSTEM}" MATCHES "Linux")
	# Copy binaries
	install(TARGETS hash-library
			RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
			LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
			ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()
