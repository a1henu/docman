#include <fstream>
#include <iostream>
#include <regex>
#include <unordered_map>
#include <vector>

#include "citation.h"
#include "utils.hpp"

// use CitationPtr to represent a shared pointer to Citation
std::unordered_map<std::string, CitationPtr> loadCitations(const std::string& filename) {
    // FIXME: load citations from file
    std::ifstream file{filename};
    nlohmann::json data = nlohmann::json::parse(file)["citations"];
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
    std::unordered_map<std::string, CitationPtr> citations;
    std::string citationFile, inputFile, outputFile;
    std::ofstream outputFileStream;
    std::ostream* output;

    if (argc == 4) {
        // "docman", "-c", "citations.json", "input.txt"
        if (std::string(argv[1]) != "-c") {
            std::exit(1);
        }
        citationFile = argv[2];
        citations = loadCitations(citationFile);
        output = &std::cout;
        inputFile = argv[3];

    } else if (argc == 6) {
        // "docman", "-c", "citations.json", "-o", "output.txt", "input.txt"
        if (std::string(argv[1]) != "-c" || std::string(argv[3]) != "-o") {
            std::exit(1);
        }
        citationFile = argv[2];
        citations = loadCitations(citationFile);
        outputFile = argv[4];
        inputFile = argv[5];

    } else {

        std::exit(1);

    }
    
    // std::vector<Citation*> printedCitations{};

    // FIXME: read all input to the string, and process citations in the input text
    // auto input = readFromFile(argv[3]);
    // ...

    if (argc == 6) {
        outputFileStream.open(outputFile);
        output = &outputFileStream;
    }

    // print the paragraph first
    std::set<std::string> citationIDs;
    std::ifstream inputFileStream{inputFile};
    std::string line;
    std::regex citationRegex{"\\[(.*?)\\]"};

    while(std::getline(inputFileStream, line)) {
        *output << line << std::endl;

        std::smatch matches;
        std::string::const_iterator searchStart(line.cbegin());
        while (std::regex_search(searchStart, line.cend(), matches, citationRegex)) {
            citationIDs.insert(matches[1].str());
            searchStart = matches.suffix().first;
        }
    }

    *output << "\nReferences:\n";
    for (auto id : citationIDs) {
        *output << citations[id]->toString() << std::endl;
    }

}
