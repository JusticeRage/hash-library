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

#include "utils.h"

std::string hexlify(const boost::uint8_t* data, size_t len)
{
    // convert to hex string
    std::string result;
    for (int i = 0; i < len; i++)
    {
        static const char dec2hex[16+1] = "0123456789abcdef";
        result += dec2hex[(data[i] >> 4) & 15];
        result += dec2hex[ data[i]       & 15];
    }
    return result;
}