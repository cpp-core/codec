// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include <regex>
#include "core/codex/filter.h"

namespace core {

inline auto filter_comments(std::istream& is) {
    std::regex comment("^[ \t]*//");
    return core::filter_istream(is, [=](std::string_view s) {
	return not std::regex_search(s.data(), comment);
    });
}

}; // ns core
