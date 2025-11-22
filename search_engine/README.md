# Search Engine (C++ Final Project)

This project is an educational implementation of a simplified search engine for local text files.
It indexes text documents, processes search requests, calculates relevance, and writes results to a JSON file.

The program can:

read and index multiple .txt documents;

process search queries from requests.json;

calculate document relevance (rank);

save results into answers.json;

run unit tests via GoogleTest.


---
How the Program Works

The program works with three JSON/text files located in the project folder:
1. config.json — program settings (what documents to search)
You must:
create your documents (any plain .txt files), place them into a folder (for example, resources/),
list them inside the "files" array (file config.json).
The program will read all files and build a search index from their content.

2. requests.json — search queries
This file contains the user’s search requests.
For every request, the program will:
break the request into words,
find documents where all the words appear,
calculate relevance,
record the results in answers.json

3. answers.json — search results (program output)
After running the program, this file is automatically created or overwritten.
---


## Project structure

search_engine/
│   CMakeLists.txt        # top-level CMake (root project)
│   README.md
│   config.json           # configuration
│   requests.json         # input search requests
│   answers.json          # output with search results
│
├── src/                  # main application sources
│   │   CMakeLists.txt
│   │   main.cpp
│   │   ConverterJSON.cpp
│   │   ConverterJSON.h
│   │   InvertedIndex.cpp
│   │   InvertedIndex.h
│   │   SearchServer.cpp
│   │   SearchServer.h
│   ...
│
├── tests/                # unit tests
│   │   CMakeLists.txt
│   │   tests.cpp
│
├── include/
│   └── nlohmann/
│       └── json.hpp      # single-header JSON library
│
└── resources/            # text documents to be indexed
    │   file001.txt
    │   file002.txt
    │   file003.txt


---

## Requirements

- Windows 10/11
- MinGW-w64 (GCC)
- CMake 3.16+
- PowerShell (for build and run)
- No additional libraries required (JSON and GoogleTest included)

---

## How to build (MinGW + PowerShell)

Open PowerShell in the project directory:

mkdir build
cd build

If CMake fails due to SSL certificates:
$env:CMAKE_TLS_VERIFY = "0"

Configure the project:
cmake .. -G "MinGW Makefiles"

Build:
mingw32-make

How to run (PowerShell)
From the project root:
.\build\src\search_engine_app.exe

The program will:
read config.json
index all documents from resources/
read queries from requests.json
write results to answers.json

How to run tests (PowerShell):
.\build\tests\search_engine_tests.exe
