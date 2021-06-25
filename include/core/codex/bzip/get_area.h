// Copyright (C) 2021 by Mark Melton
//

#pragma once
#include "core/common.h"

namespace bzip {

class GetArea {
public:
    GetArea(char *&next, uint& avail, uint capacity);

    bool available() const { return ptr_ < next_; }
    uint capacity() const { return capacity_; }
    char peek() const { return *ptr_; }
    string_view view() const { return {ptr_, (size_t)(next_ - ptr_)}; }
    
    char consume();
    void clear();

private:
    char *&next_;
    uint& avail_;
    uint capacity_;
    std::unique_ptr<char[]> buffer_;
    char *ptr_;
};

}; // bzip
