// src/SearchServer.h
#pragma once

#include <string>
#include <vector>

#include "InvertedIndex.h"

struct RelativeIndex {
    size_t doc_id;
    float rank;

    bool operator==(const RelativeIndex& other) const {
        return doc_id == other.doc_id && rank == other.rank;
    }
};

class SearchServer {
public:
    explicit SearchServer(InvertedIndex& idx, size_t responses_limit = 5);

    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& queries_input);

private:
    InvertedIndex index_;
    size_t responses_limit_;

    static std::vector<std::string> SplitIntoWords(const std::string& text);
};
