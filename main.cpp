#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "citation.h"
#include "utils.hpp"

// use CitationPtr to represent a shared pointer to Citation

std::unordered_map<std::string, CitationPtr> loadCitations(const std::string& filename) {
    // FIXME: load citations from file

    /*
    Load citations from a JSON file.
    
    This function reads a JSON file specified by `filename`, parses it into a JSON object,
    and then constructs a map of `CitationPtr` objects. The keys in the map are the citation
    IDs, and the values are the corresponding `CitationPtr` objects.
    
    Each citation object in the "citations" **array** should have a "type" and an "id" field.
    Each error in the JSON file should be handled by calling `std::exit(1)`.

    Args:
        filename: A string representing the path to the JSON file.
    
    Returns:
        A map of citation IDs to `CitationPtr` objects.
    */

    std::ifstream file{filename};
    nlohmann::json citationJson;

    citationJson = nlohmann::json::parse(file);

    if (!citationJson.contains("citations") || citationJson.empty()) {
        std::exit(1);
    }
    nlohmann::json data = citationJson["citations"];
    std::unordered_map<std::string, CitationPtr> citations;

    if (data.empty() || !data.is_array()) {
        std::exit(1);
    }

    for (auto& item : data) {
        if (!item.contains("type") || !item.contains("id") || !item["type"].is_string() || !item["id"].is_string()) {
            std::exit(1);
        }
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

std::tuple<std::string, std::string, std::string> parseArgs(int argc, char** argv) {
    /*
    Parse command line arguments.

    This function parses the command line arguments passed to the program and extracts the
    citation file name, input file name, and output file name. The program expects one of
    the following sets of arguments:
    
    - "docman", "-c", "citations.json", "input.txt"/"-"
    - "docman", "-c", "citations.json", "-o", "output.txt", "input.txt"/"-"

    If the arguments do not match one of these patterns, the function will call `std::exit(1)`.

    Args:
        argc: An integer representing the number of command line arguments.
        argv: A pointer to an array of C-style strings representing the command line arguments.
    
    Returns:
        A tuple containing the citation file name, input file name, and output file name.
    */


    std::string citationFile, inputFile, outputFile;
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

    return std::make_tuple(citationFile, inputFile, outputFile);
}

void outputCitations(std::istream& input, 
                    std::stringstream& outputBuf, 
                    const std::unordered_map<std::string, CitationPtr>& citations)
{
    /*
    Process citations in the input text and output them.

    This function reads lines from the `input` stream expected to contain citations in the 
    format "[citationID]", checks if the brackets in each line are balanced, and extracts 
    citation IDs from the lines. It then outputs the lines and the corresponding citations
    to the `outputBuf` stream.

    Any errors in the input text should be handled by calling `std::exit(1)`.

    Args:
        input: A reference to an input stream.
        outputBuf: A reference to a stringstream to store the output.
        citations: A map of citation IDs to `CitationPtr` objects.
    */

    std::set<std::string> citationIDs;
    std::string line;
    std::regex citationRegex{"\\[(.*?)\\]"};
    int bracketCount = 0;

    while(std::getline(input, line)) {
        outputBuf << line << std::endl;

        // check if brackets are balanced
        for (char c : line) {
            if (c == '[') {
                ++bracketCount;
            } else if (c == ']') {
                --bracketCount;
            }

            if (bracketCount < 0) {
                std::exit(1);
            }
        }

        std::smatch matches;
        std::string::const_iterator searchStart(line.cbegin());
        while (std::regex_search(searchStart, line.cend(), matches, citationRegex)) {
            citationIDs.insert(matches[1].str());
            searchStart = matches.suffix().first;
        }
    }

    // if bracketCount is not 0, it means the number of left brackets is not equal to the number of right brackets
    if (bracketCount != 0) {
        std::exit(1);
    }

    if (citationIDs.empty()) {
        std::exit(1);
    }
    outputBuf << "\nReferences:\n";
    for (auto& id : citationIDs) {
        try{
            outputBuf << citations.at(id)->toString() << std::endl;
        } catch(...) {
            std::exit(1);
        }
    }
}

int main(int argc, char** argv) {
    
    std::stringstream outputBuf;

    // FIXME: read all input to the string, and process citations in the input text
    // auto input = readFromFile(argv[3]);
    // ...
    // print the paragraph first

    // parse command line arguments
    auto [citationFile, inputFile, outputFile] = parseArgs(argc, argv);

    // load citations from file
    std::unordered_map<std::string, CitationPtr> citations;
    try {
        citations = loadCitations(citationFile);
    } catch(...) {
        std::exit(1);
    }

    // check if the input file is stdin
    std::istream* input;
    if (inputFile == "-") {
        input = &std::cin;
    } else {
        input = new std::ifstream(inputFile);
        if (!input->good()) {
            std::exit(1);
        }
    }

    // output the citations to buffer
    outputCitations(*input, outputBuf, citations);

    // if the input file is not stdin, close the file
    if (inputFile != "-") {
        delete input;
    }

    // output the result
    if (argc == 4) {
        std::cout << outputBuf.str();
    } else if (argc == 6) {
        std::ofstream outputFileStream;
        outputFileStream.open(outputFile);
        outputFileStream << outputBuf.str();
    }

}
