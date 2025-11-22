// src/InvertedIndex.cpp
#include "InvertedIndex.h"

#include <sstream>
#include <thread>
#include <unordered_map>

using namespace std;

// Static helper: split text into words
vector<string> InvertedIndex::SplitIntoWords(const string& text) {
    vector<string> result;
    istringstream iss(text);
    string word;
    while (iss >> word) result.push_back(word);
    return result;
}

void InvertedIndex::UpdateDocumentBase(const vector<string>& input_docs) {
    // Copy documents from input
    docs_ = input_docs;
    freq_dictionary_.clear();

    if (docs_.empty()) return;

    // One hash map per document
    vector<unordered_map<string, size_t>> local_counts(docs_.size());
    vector<thread> threads;
    threads.reserve(docs_.size());

    // Start one thread per document to count words
    for (size_t doc_id = 0; doc_id < docs_.size(); ++doc_id) {
        threads.emplace_back([this, &local_counts, doc_id]() {
            unordered_map<string, size_t>& wc = local_counts[doc_id];

            // Use SplitIntoWords for clarity
            vector<string> words = SplitIntoWords(docs_[doc_id]);
            for (const string& word : words) {
                ++wc[word];
            }
        });
    }

    // Wait for all threads
    for (auto& t : threads) t.join();

    // Build freq_dictionary_ in single thread
    for (size_t doc_id = 0; doc_id < local_counts.size(); ++doc_id) {
        for (const auto& pair : local_counts[doc_id]) {
            const string& word = pair.first;
            size_t count = pair.second;
            freq_dictionary_[word].push_back(Entry{doc_id, count});
        }
    }
}

vector<Entry> InvertedIndex::GetWordCount(const string& word) const {
    auto it = freq_dictionary_.find(word);
    if (it == freq_dictionary_.end()) return {};
    return it->second;
}
