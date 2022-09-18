// Copyright (C) 2018, 2019, 2022 by Mark Melton
//

#include <fmt/format.h>
#include "core/cc/pipe.h"

namespace core::cc::pipe
{

fp_istreambuf::fp_istreambuf(FILE *fp)
    : m_fp(fp)
    , m_buffer(std::make_unique<char[]>(BufferSize))
{
    this->setg(m_buffer.get(), m_buffer.get(), m_buffer.get());
}

int fp_istreambuf::underflow()
{
    if (this->gptr() == this->egptr())
    {
	auto n = fread(m_buffer.get(), 1, BufferSize, m_fp);
	m_count += n;
	this->setg(m_buffer.get(), m_buffer.get(), m_buffer.get() + n);
    }

    return this->gptr() == this->egptr()
	? std::char_traits<char>::eof()
	: std::char_traits<char>::to_int_type(*this->gptr());
}

namespace detail
{

popen_wrapper::popen_wrapper(std::string_view cmd, std::ptrdiff_t number_retries)
{
    while (number_retries-- >= 0)
    {
	m_fp = popen(cmd.begin(), "r");
	if (m_fp == nullptr)
	    throw std::runtime_error(fmt::format("popen failed for command: {}", cmd));

	auto c = fgetc(m_fp);
	ungetc(c, m_fp);
	if (c >= 0)
	    break;
    }
    
    m_sbuf = std::make_unique<fp_istreambuf>(m_fp);
}
    
popen_wrapper::~popen_wrapper()
{
    if (m_sbuf)
	m_sbuf.reset();

    if (m_fp)
	pclose(m_fp);
}

}; // detail

istream::istream(std::string_view cmd, std::ptrdiff_t number_retries)
    : detail::popen_wrapper(cmd, number_retries)
    , std::istream(this->get_buf())
{ }

}; // end core::cc::pipe
