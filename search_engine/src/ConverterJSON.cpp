// src/ConverterJSON.cpp
#include "ConverterJSON.h"

#include <fstream>
#include <iostream>
#include <cmath>

#include "nlohmann/json.hpp"

using json = nlohmann::json;
using namespace std;

ConverterJSON::ConverterJSON() {
    LoadConfig();
}

void ConverterJSON::LoadConfig() {
    ifstream cfg("config.json");
    if (!cfg.is_open()) throw runtime_error("config file is missing");

    json j;
    cfg >> j;

    if (!j.contains("config") || j["config"].is_null())
        throw runtime_error("config file is empty");

    const json& cfg_section = j["config"];

    if (!cfg_section.contains("name") || !cfg_section.contains("version"))
        throw runtime_error("config file is empty");

    engine_name_ = cfg_section["name"].get<string>();
    engine_version_ = cfg_section["version"].get<string>();

    if (engine_version_ != kAppVersion)
        throw runtime_error("config.json has incorrect file version");

    if (cfg_section.contains("max_responses"))
        max_responses_ = cfg_section["max_responses"].get<int>();
    else
        max_responses_ = 5;

    if (!j.contains("files") || !j["files"].is_array() || j["files"].empty())
        throw runtime_error("config file is empty");

    file_paths_.clear();
    for (const auto& item : j["files"]) {
        file_paths_.push_back(item.get<string>());
    }
}

vector<string> ConverterJSON::GetTextDocuments() {
    vector<string> result;
    result.reserve(file_paths_.size());

    for (const string& path : file_paths_) {
        ifstream in(path);
        if (!in.is_open()) {
            cerr << "Error: file '" << path << "' is missing or cannot be opened" << endl;
            continue;
        }

        string line;
        string content;
        bool first = true;
        while (getline(in, line)) {
            if (!first) content += ' ';
            content += line;
            first = false;
        }

        result.push_back(content);
    }

    return result;
}

int ConverterJSON::GetResponsesLimit() const {
    return max_responses_;
}

vector<string> ConverterJSON::GetRequests() {
    vector<string> result;

    ifstream in("requests.json");
    if (!in.is_open()) {
        cerr << "Error: requests.json is missing" << endl;
        return result;
    }

    json j;
    in >> j;

    if (j.contains("requests") && j["requests"].is_array()) {
        for (const auto& item : j["requests"]) {
            result.push_back(item.get<string>());
        }
    }

    return result;
}

void ConverterJSON::putAnswers(const vector<vector<pair<int, float>>>& answers) {
    json root;
    json answers_object = json::object();

    for (size_t i = 0; i < answers.size(); ++i) {
        char buf[32];
        snprintf(buf, sizeof(buf), "request%03zu", i + 1);

        json one_request;
        const auto& single_answer = answers[i];

        if (single_answer.empty()) {
            // No documents found for this request
            one_request["result"] = "false";
        } else if (single_answer.size() == 1) {
            // Exactly one document: separate docid + rank
            one_request["result"] = "true";

            int docid = single_answer[0].first;
            float rank = single_answer[0].second;

            double rounded_rank =
                std::round(static_cast<double>(rank) * 1000.0) / 1000.0;

            one_request["docid"] = docid;
            one_request["rank"]  = rounded_rank;
        } else {
            // Multiple documents
            one_request["result"] = "true";

            json relevance = json::array();

            for (const auto& doc : single_answer) {
                int docid = doc.first;
                float rank = doc.second;

                double rounded_rank =
                    std::round(static_cast<double>(rank) * 1000.0) / 1000.0;

                json item;
                item["docid"] = docid;
                item["rank"] = rounded_rank;
                relevance.push_back(item);
            }

            one_request["relevance"] = relevance;
        }

        answers_object[buf] = one_request;
    }

    root["answers"] = answers_object;

    ofstream out("answers.json", ios::trunc);
    if (!out.is_open()) {
        cerr << "Error: cannot open answers.json for writing" << endl;
        return;
    }

    out << root.dump(4);
}

