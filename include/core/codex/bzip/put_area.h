// Copyright (C) 2021 by Mark Melton
//

#pragma once
#include "core/common.h"

namespace bzip {

class PutArea {
public:
    PutArea(char *&next, uint& avail, uint capacity);

    bool available() const { return avail_ > 0; }
    uint capacity() const { return capacity_; }
    string_view view() const { return {next_, avail_}; }
    
    template<class Source>
    bool read(Source& source);

private:
    char *&next_;
    uint& avail_;
    uint capacity_;
    std::unique_ptr<char[]> buffer_;
};

}; // bzip
