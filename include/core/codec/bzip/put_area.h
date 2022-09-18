// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include "core/codec/util/buffer.h"

namespace bzip {

// Encapsulate Bzip decompression input by wrapping the Bzip library
// variables in a minimal API.
class PutArea : public core::BufferedArea {
public:
    // Construct an area of the given `capacity` for holding the Bzip
    // input and retain references to the Bzip library variables that
    // describe the buffer.
    PutArea(char *&next, uint& avail, uint capacity);

    // Return true if no more characters are available for the Bzip
    // library to read.
    bool empty() const { return avail_ == 0; }

    // Update the Bzip library variables with newly available data.
    void update(size_t count);
    
private:
    char *&next_;
    uint& avail_;
};

}; // bzip
