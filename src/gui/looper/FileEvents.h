//
// FileEvents.h - This file is part of the nglui library
//
// Copyright (c) 2015 Matthew Brush <mbrush@codebrainz.ca>
// All rights reserved.
//

#ifndef LOOP_FILEEVENTS_H
#define LOOP_FILEEVENTS_H

#include <poll.h>

namespace nglui
{

enum class FileEvents {
    NONE     = 0,
    INPUT    = POLLIN,
    OUTPUT   = POLLOUT,
    PRIORITY = POLLPRI,
    ERROR    = POLLERR,
    HANGUP   = POLLHUP,
};

} // namespace nglui

static inline int operator&(nglui::FileEvents ev1, nglui::FileEvents ev2)
{
    return static_cast<int>(ev1) & static_cast<int>(ev2);
}

static inline int operator&(int ev1, nglui::FileEvents ev2)
{
    return ev1 & static_cast<int>(ev2);
}

static inline nglui::FileEvents operator|(nglui::FileEvents ev1, nglui::FileEvents ev2)
{
    return static_cast<nglui::FileEvents>(static_cast<int>(ev1) | static_cast<int>(ev2));
}

#endif // LOOP_FILEEVENTS_H

