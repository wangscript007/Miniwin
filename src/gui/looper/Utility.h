//
// Utility.h - This file is part of the Grinder library
//
// Copyright (c) 2015 Matthew Brush <mbrush@codebrainz.ca>
// All rights reserved.
//

#ifndef LOOPER_UTILITY_H
#define LOOPER_UTILITY_H

#include <chrono>
#include <cstdint>

namespace nglui
{

static inline std::uint64_t time_now()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch()).count();
}

} // namespace nglui

#endif // LOOPER_UTILITY_H

