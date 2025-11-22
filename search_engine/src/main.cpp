#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>

#include "ConverterJSON.h"
#include "InvertedIndex.h"

using namespace std;

// Helper
static vector<string> SplitRequestIntoWords(const string& text) {
    vector<string> result;
    istringstream iss(text);
    string word;
    while (iss >> word) result.push_back(word);
    return result;
}

int main() {
    try {
        cout << "Starting search engine v2" << endl;

        // 1. Load config and documents
        ConverterJSON converter;
        vector<string> documents = converter.GetTextDocuments();

        // 2. Build inverted index
        InvertedIndex index;
        index.UpdateDocumentBase(documents);

        // 3. Load requests
        vector<string> requests = converter.GetRequests();
        int max_responses = converter.GetResponsesLimit();

        // 4. For each request, compute doc ranks
        vector<vector<pair<int, float>>> all_answers;
        all_answers.reserve(requests.size());

        for (const string& request : requests) {
            vector<string> words = SplitRequestIntoWords(request);

            // doc_id -> total word count
            map<int, int> doc_counts;

            for (const string& w : words) {
                vector<Entry> entries = index.GetWordCount(w);
                for (const Entry& e : entries) {
                    int doc_id = static_cast<int>(e.doc_id);
                    int count  = static_cast<int>(e.count);
                    doc_counts[doc_id] += count;
                }
            }

            // No docs found
            if (doc_counts.empty()) {
                all_answers.push_back({});
                continue;
            }

            // Find max count
            int max_count = 0;
            for (const auto& p : doc_counts) {
                if (p.second > max_count) max_count = p.second;
            }

            // Build list
            vector<pair<int, float>> one_request;
            one_request.reserve(doc_counts.size());

            for (const auto& p : doc_counts) {
                int doc_id = p.first;
                int cnt    = p.second;
                float rank = static_cast<float>(cnt) / static_cast<float>(max_count);
                one_request.push_back({doc_id, rank});
            }

            // Sort: by rank desc, docid asc
            sort(one_request.begin(), one_request.end(),
                 [](const pair<int, float>& a, const pair<int, float>& b) {
                     if (a.second != b.second) return a.second > b.second;
                     return a.first < b.first;
                 });

            // Limit number of responses
            if (static_cast<int>(one_request.size()) > max_responses)
                one_request.resize(static_cast<size_t>(max_responses));

            all_answers.push_back(one_request);
        }

        // 5. Write answers.json
        converter.putAnswers(all_answers);

    } catch (const exception& ex) {
        cerr << "Error: " << ex.what() << endl;
        return 1;
    }

    return 0;
}
