/*
Pok�mon Emerald 2 Version
------------------------------

file        : mapWarps.cpp
author      : Philip Wellnitz
description : Map warps.

Copyright (C) 2012 - 2015
Philip Wellnitz

This file is part of Pok�mon Emerald 2 Version.

Pok�mon Emerald 2 Version is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pok�mon Emerald 2 Version is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pok�mon Emerald 2 Version.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mapWarps.h"

namespace MAP {
    std::map<std::pair<u8, position>, std::pair<u8, position>> warpList = { };
}