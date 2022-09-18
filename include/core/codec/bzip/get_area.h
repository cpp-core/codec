// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include "core/codec/util/get_area.h"

namespace bzip {

// Encapsulate Bzip (de)compression output by wrapping the Bzip
// library variables in a minimal API that allows structured access to
// the data (set core::GetArea for the interface).
//
class GetArea : public core::GetArea {
public:
    // Construct an area of the given `capacity` for managing the Bzip
    // output.
    GetArea(char *&next, unsigned int& avail, unsigned int capacity);

    // Prepare to recieve output from Bzip.
    void clear();

    // Update the get area with new output from Bzip.
    void update();
	  
private:
    char *&next_;
    unsigned int& avail_;
};

}; // bzip
