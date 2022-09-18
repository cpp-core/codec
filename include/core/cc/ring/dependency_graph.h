// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include <set>
#include "core/cc/ring/processor.h"
#include "core/util/json.h"

namespace core::cc::ring {

class DependencyGraph {
public:
    using Node = std::pair<std::string,Cursor*>;
    using Nodes = std::vector<Node>;
    using NodeMap = std::map<std::string,Cursor*>;
    using AdjancencyMap = std::map<std::string,std::vector<std::string>>;
    using SizeMap = std::map<std::string,size_t>;

    // Configure the dependency graph based on <graph>.
    void configure(const json& graph);

    // Return the size for the cursor <id>.
    size_t size(const std::string& id) const;

    // Return the size for the i'th source for <id>.
    size_t size(const std::string& id, int i) const;

    // Return the source <id>'s for the given <id>.
    std::vector<std::string> sources(const std::string& id) const;

    // Return the i'th source <id> for the given <id>.
    std::string source(const std::string& id, int i) const;

    // Register <cursor> with the given <id>.
    void register_cursor(Cursor *cursor, const std::string& id);

    // Blocks until all cursors have been registered.
    void wait_for_all_registrations();

    // Apply the configured read and write barriers to <processor>
    template<class P>
    void apply_barriers(P& processor, const std::string& id) {
	wait_for_all_registrations();
	if (auto iter = read_depends_.find(id); iter != read_depends_.end())
	    for (auto c : iter->second)
		processor.add_read_barrier(get_cursor(c));
	if (auto iter = write_depends_.find(id); iter != write_depends_.end())
	    for (auto c : iter->second)
		processor.add_write_barrier(get_cursor(c));
    }

private:
    // Return the cursor for the given <id> or throw an exception.
    Cursor *get_cursor(const std::string& id);

    std::mutex mutex_;
    std::condition_variable cond_;
    std::set<std::string> unregistered_;
    NodeMap cursors_;
    AdjancencyMap write_depends_, read_depends_;
    SizeMap sizes_;
};

}; // core::cc::ring
