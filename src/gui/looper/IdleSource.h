//
// IdleSource.h - This file is part of the nglui library
//
// Copyright (c) 2015 Matthew Brush <mbrush@codebrainz.ca>
// All rights reserved.
//

#ifndef LOOPIDLESOURCE_H
#define LOOPIDLESOURCE_H

#include <looper/EventSource.h>

namespace nglui
{

class IdleSource : public EventSource
{
public:
    bool prepare(int&) override final
    {
        return true;
    }
    bool check() override final
    {
        return true;
    }
    bool dispatch(EventHandler &func) override
    {
        return func(*this);
    }
    bool is_idle_source() const override final
    {
        return true;
    }
};

} // namespace nglui

#endif // LOOPER_IDLESOURCE_H
