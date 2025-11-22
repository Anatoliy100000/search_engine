// src/ConverterJSON.h
#pragma once

#include <string>
#include <vector>

class ConverterJSON {
public:
    ConverterJSON();

    std::vector<std::string> GetTextDocuments();
    int GetResponsesLimit() const;
    std::vector<std::string> GetRequests();
    void putAnswers(const std::vector<std::vector<std::pair<int, float>>>& answers);

private:
    void LoadConfig();

    std::string engine_name_;
    std::string engine_version_;
    int max_responses_ = 5;
    std::vector<std::string> file_paths_;

    static constexpr const char* kAppVersion = "0.1";
};
