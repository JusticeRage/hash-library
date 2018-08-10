// SHA-512. Adapted from LibTomCrypt. This code is Public Domain
#pragma once

#include <cstring>
#include <boost/cstdint.hpp>

#include "hash.h"
#include "utils.h"

struct sha512_state
{
    boost::uint64_t length;
    boost::uint64_t state[8];
    boost::uint32_t curlen;
    unsigned char buf[128];
};

void sha_init(sha512_state& md);
void sha_process(sha512_state& md, const void* in, std::uint32_t inlen);
void sha_done(sha512_state& md, void* out);

class SHA512 : public Hash
{
public:
    SHA512();
    void add(const void* data, size_t numBytes) override;
    std::string getHash() override;
    void reset() override;

private:
    sha512_state _state {};
};