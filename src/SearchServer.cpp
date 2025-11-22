// src/SearchServer.cpp
#include "SearchServer.h"

#include <algorithm>
#include <unordered_map>
#include <cctype>

using namespace std;

SearchServer::SearchServer(InvertedIndex& idx, size_t responses_limit)
    : index_(idx), responses_limit_(responses_limit) {
}

vector<string> SearchServer::SplitIntoWords(const string& text) {
    vector<string> words;
    string current;

    for (char ch : text) {
        if (isspace(static_cast<unsigned char>(ch))) {
            if (!current.empty()) {
                words.push_back(current);
                current.clear();
            }
        } else {
            char lower = static_cast<char>(tolower(static_cast<unsigned char>(ch)));
            current.push_back(lower);
        }
    }
    if (!current.empty()) words.push_back(current);
    return words;
}

vector<vector<RelativeIndex>> SearchServer::search(const vector<string>& queries_input) {
    vector<vector<RelativeIndex>> results;
    results.reserve(queries_input.size());

    for (const string& query : queries_input) {
        vector<string> words = SplitIntoWords(query);

        // make unique
        sort(words.begin(), words.end());
        words.erase(unique(words.begin(), words.end()), words.end());

        unordered_map<size_t, size_t> doc_to_abs;
        size_t max_abs = 0;

        for (const string& word : words) {
            vector<Entry> entries = index_.GetWordCount(word);
            for (const Entry& e : entries) {
                size_t& value = doc_to_abs[e.doc_id];
                value += e.count;
                if (value > max_abs) max_abs = value;
            }
        }

        vector<RelativeIndex> one_query_result;

        if (!doc_to_abs.empty() && max_abs > 0) {
            one_query_result.reserve(doc_to_abs.size());
            for (const auto& [doc_id, abs_value] : doc_to_abs) {
                float rank = static_cast<float>(abs_value) / static_cast<float>(max_abs);
                one_query_result.push_back(RelativeIndex{doc_id, rank});
            }

            sort(one_query_result.begin(), one_query_result.end(),
                 [](const RelativeIndex& a, const RelativeIndex& b) {
                     if (a.rank == b.rank) return a.doc_id < b.doc_id;
                     return a.rank > b.rank;
                 });

            if (one_query_result.size() > responses_limit_)
                one_query_result.resize(responses_limit_);
        }

        results.push_back(one_query_result);
    }

    return results;
}
