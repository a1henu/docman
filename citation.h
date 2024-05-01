#pragma once
#ifndef CITATION_H
#define CITATION_H

#include <fstream>
#include <string>

#include "cpp-httplib/httplib.h"
#include "nlohmann/json.hpp"

class Citation;
using CitationPtr = std::shared_ptr<Citation>;

class Citation {
protected:
    std::string id;
    nlohmann::json data;
public:
    virtual ~Citation() = default;

    Citation() = default;
    Citation(const nlohmann::json& data) : id(data["id"].get<std::string>()), data(data) {}

    virtual std::string getResource() const;
    virtual std::string toString() const;
};

class Book : public Citation {
    std::string isbn;
public:
    ~Book() override = default;

    Book() = default;
    Book(const nlohmann::json& data) : Citation(data), isbn(data["isbn"].get<std::string>()) {}

    std::string getResource() const override;
    std::string toString() const override;
};

class WebPage : public Citation {
    std::string url;
public:
    ~WebPage() override = default;

    WebPage() = default;
    WebPage(const nlohmann::json& data) : Citation(data), url(data["url"].get<std::string>()) {}

    std::string getResource() const override;
    std::string toString() const override;
};

class Article : public Citation {
public:
    ~Article() override = default;

    Article() = default;
    Article(const nlohmann::json& data) : Citation(data) {}

    std::string getResource() const override;
    std::string toString() const override;
};

#endif