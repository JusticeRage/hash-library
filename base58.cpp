// Some code from this page is taken from the Monero code base. Original copyright notice
// reproduced below:
// --------------------------------------------------------------------------------------
// Copyright (c) 2014-2018, The Monero Project
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Parts of this file are originally copyright (c) 2012-2013 The Cryptonote developers

#include "base58.h"

namespace btc {

std::vector<uint8_t> base58_decode(const std::string& enc)
{
    std::string base58 = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
    std::vector<boost::uint8_t> data(25);
    boost::multiprecision::cpp_int big_int = 0;

    // Decode base58 data.
    for (char c : enc) {
        big_int = big_int * 58 + base58.find_first_of(c);
    }
    for (int i = 24; i >= 0; --i)
    {
        // Safe because of the & 0xFF.
        data[24 - i] = boost::multiprecision::cpp_int((big_int >> i * 8) & 0xFF).convert_to<boost::uint8_t>();
    }
    return data;
}

} // !namespace btc

namespace xmr {

const char alphabet[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
const size_t alphabet_size = sizeof(alphabet) - 1;
const size_t encoded_block_sizes[] = {0, 2, 3, 5, 6, 7, 9, 10, 11};
const size_t full_block_size = sizeof(encoded_block_sizes) / sizeof(encoded_block_sizes[0]) - 1;
const size_t full_encoded_block_size = encoded_block_sizes[full_block_size];

struct reverse_alphabet
{
    reverse_alphabet()
    {
        m_data.resize(alphabet[alphabet_size - 1] - alphabet[0] + 1, -1);

        for (size_t i = 0; i < alphabet_size; ++i)
        {
            auto idx = static_cast<size_t>(alphabet[i] - alphabet[0]);
            m_data[idx] = static_cast<int8_t>(i);
        }
    }

    int operator()(char letter) const
    {
        auto idx = static_cast<size_t>(letter - alphabet[0]);
        return idx < m_data.size() ? m_data[idx] : -1;
    }

    static reverse_alphabet instance;

private:
    std::vector<int8_t> m_data;
};

reverse_alphabet reverse_alphabet::instance;

struct decoded_block_sizes
{
    decoded_block_sizes()
    {
        m_data.resize(encoded_block_sizes[full_block_size] + 1, -1);
        for (size_t i = 0; i <= full_block_size; ++i)
        {
            m_data[encoded_block_sizes[i]] = static_cast<int>(i);
        }
    }

    int operator()(size_t encoded_block_size) const
    {
        assert(encoded_block_size <= full_encoded_block_size);
        return m_data[encoded_block_size];
    }

    static decoded_block_sizes instance;

private:
    std::vector<int> m_data;
};

decoded_block_sizes decoded_block_sizes::instance;

uint64_t uint_8be_to_64(const uint8_t* data, size_t size)
{
    assert(1 <= size && size <= sizeof(uint64_t));

    uint64_t res = 0;
    memcpy(reinterpret_cast<uint8_t*>(&res) + sizeof(uint64_t) - size, data, size);
    return SWAP64BE(res);
}

void uint_64_to_8be(uint64_t num, size_t size, uint8_t* data)
{
    assert(1 <= size && size <= sizeof(uint64_t));

    uint64_t num_be = SWAP64BE(num);
    memcpy(data, reinterpret_cast<uint8_t*>(&num_be) + sizeof(uint64_t) - size, size);
}

bool decode_block(const char* block, size_t size, char* res)
{
    assert(1 <= size && size <= full_encoded_block_size);

    int res_size = decoded_block_sizes::instance(size);
    if (res_size <= 0)
        return false; // Invalid block size

    uint64_t res_num = 0;
    uint64_t order = 1;
    for (size_t i = size - 1; i < size; --i)
    {
        int digit = reverse_alphabet::instance(block[i]);
        if (digit < 0)
            return false; // Invalid symbol

        uint64_t product_hi;
        uint64_t tmp = res_num + mul128(order, digit, &product_hi);
        if (tmp < res_num || 0 != product_hi)
            return false; // Overflow

        res_num = tmp;
        order *= alphabet_size; // Never overflows, 58^10 < 2^64
    }

    if (static_cast<size_t>(res_size) < full_block_size && (UINT64_C(1) << (8 * res_size)) <= res_num)
        return false; // Overflow

    uint_64_to_8be(res_num, res_size, reinterpret_cast<uint8_t*>(res));

    return true;
}

std::vector<uint8_t> base58_decode(const std::string& enc)
{
    std::string data;
    if (enc.empty())
    {
        data.clear();
        return std::vector<uint8_t>();
    }

    size_t full_block_count = enc.size() / full_encoded_block_size;
    size_t last_block_size = enc.size() % full_encoded_block_size;
    int last_block_decoded_size = decoded_block_sizes::instance(last_block_size);
    if (last_block_size < 0)
        return std::vector<uint8_t>(); // Invalid enc length
    size_t data_size = full_block_count * full_block_size + last_block_decoded_size;

    data.resize(data_size, 0);
    for (size_t i = 0; i < full_block_count; ++i)
    {
        if (!decode_block(enc.data() + i * full_encoded_block_size, full_encoded_block_size, &data[i * full_block_size]))
            return std::vector<uint8_t>();
    }

    if (0 < last_block_size)
    {
        if (!decode_block(enc.data() + full_block_count * full_encoded_block_size, last_block_size,
                          &data[full_block_count * full_block_size]))
            return std::vector<uint8_t>();
    }

    return std::vector<uint8_t>(data.begin(), data.end());
}

} // !namespace encoding