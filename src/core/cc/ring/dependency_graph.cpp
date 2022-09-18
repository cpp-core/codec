// Copyright (C) 2021, 2022 by Mark Melton
//

#include "core/cc/ring/dependency_graph.h"
#include "core/util/exception.h"
#include "core/util/json.h"

namespace core::cc::ring {

void DependencyGraph::configure(const json& j) {
    for (const auto& [key, jval] : j.items()) {
	auto bs = get_or_else<std::vector<std::string>>(jval, "rw", std::vector<std::string>{});
	auto rbs = get_or_else<std::vector<std::string>>(jval, "read", std::vector<std::string>{});
	auto wbs = get_or_else<std::vector<std::string>>(jval, "write",std::vector<std::string>{});

	for (const auto& b : bs) {
	    read_depends_[key].push_back(b);
	    write_depends_[b].push_back(key);
	}
	
	for (const auto& r : rbs)
	    read_depends_[key].push_back(r);
	
	for (const auto& w : wbs)
	    write_depends_[key].push_back(w);
	
	unregistered_.insert(key);

	if (jval.contains("size"))
	    sizes_[key] = jval.at("size").get<size_t>();
    }
}

size_t DependencyGraph::size(const std::string& id) const {
    if (auto iter = sizes_.find(id); iter != sizes_.end())
	return iter->second;
    throw runtime_error("size for id {} not found", id);
}

size_t DependencyGraph::size(const std::string& id, int i) const {
    auto source_id = source(id, i);
    if (auto iter = sizes_.find(source_id); iter != sizes_.end())
	return iter->second;
    throw runtime_error("size for {}'th source of id {} not found: {}", id, i, source_id);
}

std::vector<std::string> DependencyGraph::sources(const std::string& id) const {
    if (auto iter = read_depends_.find(id); iter != read_depends_.end())
	return iter->second;
    return std::vector<std::string>{};
}
    
std::string DependencyGraph::source(const std::string& id, int i) const {
    if (auto iter = read_depends_.find(id); iter != read_depends_.end()) {
	if (i < iter->second.size())
	    return iter->second[i];
	throw core::runtime_error("only {} read dependencies for id {}: {}",
				  iter->second.size(), id, i);
    }
    throw core::runtime_error("no read dependencies found for id {}", id);
}
    
void DependencyGraph::register_cursor(Cursor *cursor, const std::string& id) {
    if (cursors_.find(id) != cursors_.end())
	throw runtime_error("Attempt to register duplicate cursor id in depdency graph: {}", id);
    cursors_[id] = cursor;
    unregistered_.erase(id);
    cond_.notify_all();
}

void DependencyGraph::wait_for_all_registrations() {
    std::mutex reg_mutex;
    std::unique_lock reg_lock(reg_mutex);
    cond_.wait(reg_lock, [this]{ return unregistered_.empty(); });
}

Cursor *DependencyGraph::get_cursor(const std::string& id) {
    if (auto iter = cursors_.find(id); iter != cursors_.end())
	return iter->second;
    throw runtime_error("cursor for id {} not found");
}

}; // core::cc::ring
