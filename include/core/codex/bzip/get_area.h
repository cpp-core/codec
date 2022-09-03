// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include "core/util/common.h"
#include "core/codex/util/get_area.h"

namespace bzip {

// Encapsulate Bzip (de)compression output by wrapping the Bzip
// library variables in a minimal API that allows structured access to
// the data (set core::GetArea for the interface).
//
class GetArea : public core::GetArea {
public:
    // Construct an area of the given `capacity` for managing the Bzip
    // output.
    GetArea(char *&next, uint& avail, uint capacity);

    // Prepare to recieve output from Bzip.
    void clear();

    // Update the get area with new output from Bzip.
    void update();
	  
private:
    char *&next_;
    uint& avail_;
};

}; // bzip
