#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "citation.h"
#include "utils.hpp"

// use CitationPtr to represent a shared pointer to Citation

std::unordered_map<std::string, CitationPtr> loadCitations(const std::string& filename) {
    // FIXME: load citations from file
    std::ifstream file{filename};
    nlohmann::json citationJson;

    try {
        file >> citationJson;
    } catch (...) {
        std::exit(1);
    }
    
    nlohmann::json data = citationJson["citations"];
    std::unordered_map<std::string, CitationPtr> citations;

    if (data.empty()) {
        std::exit(1);
    }

    for (auto& item : data) {
        std::string type = item["type"].get<std::string>();
        std::string id = item["id"].get<std::string>();
        if (type == "book") {
            citations[id] = std::make_shared<Book>(item);
        } else if (type == "webpage") {
            citations[id] = std::make_shared<WebPage>(item);
        } else if (type == "article") {
            citations[id] = std::make_shared<Article>(item);
        } else {
            std::exit(1);
        }
    }

    return citations;
}

int main(int argc, char** argv) {
    std::string citationFile, inputFile, outputFile;
    std::stringstream outputBuf;

    if (argc == 4) {
        // "docman", "-c", "citations.json", "input.txt"
        if (std::string(argv[1]) != "-c") {
            std::exit(1);
        }
        citationFile = argv[2];
        inputFile = argv[3];

    } else if (argc == 6) {
        // "docman", "-c", "citations.json", "-o", "output.txt", "input.txt"
        if (std::string(argv[1]) != "-c" || std::string(argv[3]) != "-o") {
            std::exit(1);
        }
        citationFile = argv[2];
        outputFile = argv[4];
        inputFile = argv[5];

    } else {

        std::exit(1);

    }
    
    // std::vector<Citation*> printedCitations{};

    // FIXME: read all input to the string, and process citations in the input text
    // auto input = readFromFile(argv[3]);
    // ...
    // print the paragraph first
    std::unordered_map<std::string, CitationPtr> citations;
    try {
        citations = loadCitations(citationFile);
    } catch(...) {
        std::exit(1);
    }

    std::istream* input;
    if (inputFile == "-") {
        input = &std::cin;
    } else {
        input = new std::ifstream(inputFile);
        if (!input->good()) {
            std::cerr << "Failed to open input file\n";
            std::exit(1);
        }
    }

    std::set<std::string> citationIDs;
    std::string line;
    std::regex citationRegex{"\\[(.*?)\\]"};

    while(std::getline(*input, line)) {
        outputBuf << line << std::endl;

        std::smatch matches;
        std::string::const_iterator searchStart(line.cbegin());
        while (std::regex_search(searchStart, line.cend(), matches, citationRegex)) {
            citationIDs.insert(matches[1].str());
            searchStart = matches.suffix().first;
        }
    }

    if (inputFile != "-") {
        delete input;
    }

    if (citationIDs.empty()) {
        std::exit(1);
    }
    outputBuf << "\nReferences:\n";
    for (auto id : citationIDs) {
        try{
            outputBuf << citations[id]->toString() << std::endl;
        } catch(...) {
            std::exit(1);
        }
    }

    if (argc == 4) {
        std::cout << outputBuf.str();
    } else if (argc == 6) {
        std::ofstream outputFileStream{outputFile};
        outputFileStream << outputBuf.str();
    }

}
