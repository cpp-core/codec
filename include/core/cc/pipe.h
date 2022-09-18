// Copyright (C) 2018, 2019, 2022 by Mark Melton
//

#pragma once
#include <thread>
#include <streambuf>
#include <istream>

namespace core::cc::pipe
{

class fp_istreambuf : public std::streambuf
{
public:
    static constexpr size_t BufferSize = 1024;
    
    fp_istreambuf(FILE *fp);
    ~fp_istreambuf() {}
    int underflow() override;
    
private:
    size_t m_count{0};
    FILE *m_fp{nullptr};
    std::unique_ptr<char[]> m_buffer;
};

namespace detail
{

class popen_wrapper
{
public:
    popen_wrapper(std::string_view cmd, std::ptrdiff_t number_retries = 0);
    ~popen_wrapper();
    fp_istreambuf *get_buf() const { return m_sbuf.get(); }
    
private:
    FILE *m_fp{nullptr};
    std::unique_ptr<fp_istreambuf> m_sbuf;
};

}; // detail

class istream : private detail::popen_wrapper, public std::istream
{
public:
    istream(std::string_view cmd, std::ptrdiff_t number_retries = 0);
};

}; // end core::cc::pipe
