/*
    This file is part of Manalyze.

    Manalyze is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Manalyze is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Manalyze.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "hash.h"

/// compute hash of a memory block
std::string Hash::operator()(const void* data, size_t numBytes)
{
    reset();
    add(data, numBytes);
    return getHash();
};

/// compute hash of a string, excluding final zero
std::string Hash::operator()(const std::string& text)
{
    reset();
    add(text.c_str(), text.size());
    return getHash();
};