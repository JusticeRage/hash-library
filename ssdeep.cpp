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

#include "ssdeep.h"

#include <array>

namespace ssdeep {

pString hash_file(const std::string& filename)
{
	std::array<char, FUZZY_MAX_RESULT> res;
	FILE* f = fopen(filename.c_str(), "rb");

	if (f == nullptr || fuzzy_hash_file(f, res.data()))
	{
		if (f != nullptr) {
			fclose(f);
		}
		return pString();
	}
	fclose(f);
	return std::make_shared<std::string>(res.data());
}

pString hash_buffer(const std::vector<std::uint8_t>& bytes)
{
	if (bytes.size() == 0) {
		return std::shared_ptr<std::string>();
	}

	std::array<char, FUZZY_MAX_RESULT> res;
	if (fuzzy_hash_buf(&bytes[0], bytes.size(), res.data())) {
		return pString();
	}
	return std::make_shared<std::string>(res.data());
}

}
