// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include <any>
#include <fmt/format.h>
#include <map>
#include <string>

namespace core::cc::ring {

class SharedData {
public:
    using Data = std::map<std::string,std::any>;

    // Return a pointer to the data named <id> of the given <size>
    // creating a new T if necessary. Throw an exception if <id>
    // already exists as a different type or size.
    template<class T>
    T *get(const std::string& id, size_t size) {
	if (auto iter = data_.find(id); iter != data_.end()) {
	    auto ptr = std::any_cast<T>(&iter->second);
	    if (ptr == nullptr)
		throw std::runtime_error(fmt::format("mismatched type for shared data: {}", id));
	    return ptr;
	}
	data_[id] = T{size};
	return std::any_cast<T>(&data_[id]);
    }

    // Return a pointer to the data named <id> which must already exist.
    template<class T>
    const T *get(const std::string& id) const {
	if (auto iter = data_.find(id); iter != data_.end()) {
	    auto ptr = std::any_cast<T>(&iter->second);
	    if (ptr == nullptr)
		throw std::runtime_error(fmt::format("mismatched type for shared data: {}", id));
	    return ptr;
	}
	throw std::runtime_error(fmt::format("no data with given name: {}", id));
    }

private:
    Data data_;
};

}; // core::cc::ring
