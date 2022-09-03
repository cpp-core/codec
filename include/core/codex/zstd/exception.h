// Copyright (C) 2018, 2019, 2022 by Mark Melton
//

#pragma once
#include <zstd.h>
#include <fmt/printf.h>
#include "core/utility/common.h"

namespace zstd
{

class error : public std::exception
{
public:
    using Base = std::runtime_error;
    
    template<typename ...Ts>
    error(const string& fmt, Ts... args)
	: m_what("zstd:" + fmt::sprintf(fmt, args...))
    { }
    
    virtual const char *what() const noexcept override
    { return m_what.c_str(); }
    
private:
    string m_what;
};

}; // zstd

