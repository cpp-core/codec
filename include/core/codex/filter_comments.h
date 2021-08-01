// Copyright (C) 2021 by Mark Melton
//

#pragma once
#include <regex>
#include "core/codex/filter.h"

namespace core {

auto filter_comments(std::istream& is) {
    std::regex comment("^[ \t]*//");
    return core::filter_istream(is, [=](string_view s) {
	return not std::regex_search(s.data(), comment);
    });
}

}; // ns core
