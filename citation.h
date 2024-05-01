#ifndef CITATION_H
#define CITATION_H

#include <fstream>
#include <string>

#include "cpp-httplib/httplib.h"
#include "nlohmann/json.hpp"

class Citation;
using CitationPtr = std::shared_ptr<Citation>;

class Citation {
/*
Base class for all citations.

This class stores the ID and data of a citation. The data is stored as a JSON object.
Derived classes should override the `getResource` and `toString` methods to provide
behavior to fetch the citation resource and to convert the citation to a string, respectively.
*/

protected:
    std::string id;
    nlohmann::json data;
public:
    virtual ~Citation() = default;

    Citation() = default;
    Citation(const nlohmann::json& data);

    virtual std::string getResource() const;
    virtual std::string toString() const;
};

class Book : public Citation {
/*
This class represents a book citation.

In addition to the base class fields, this class also stores the ISBN of the book.
The `getResource` method returns the ISBN, and the `toString` method returns a string
representation of the citation in the format expected for book citations.
*/

private:
    std::string isbn;
public:
    ~Book() override = default;

    Book() = default;
    Book(const nlohmann::json& data);

    std::string getResource() const override;
    std::string toString() const override;
};

class WebPage : public Citation {
/*
This class represents a webpage citation.

In addition to the base class fields, this class also stores the URL of the webpage.
The `getResource` method returns the URL, and the `toString` method returns a string
representation of the citation in the format expected for webpage citations.
*/

private:
    std::string url;
public:
    ~WebPage() override = default;

    WebPage() = default;
    WebPage(const nlohmann::json& data);

    std::string getResource() const override;
    std::string toString() const override;
};

class Article : public Citation {
/*
This class represents an article citation.

This class does not add any new fields to the base class. The `getResource` method
returns an empty string, and the `toString` method returns a string representation
of the citation in the format expected for article citations.
*/

public:
    ~Article() override = default;

    Article() = default;
    Article(const nlohmann::json& data);

    std::string getResource() const override;
    std::string toString() const override;
};

#endif