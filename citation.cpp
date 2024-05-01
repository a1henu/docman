#include "./citation.h"
#include "./utils.hpp"

std::string getFromWeb(const std::string& resource) {
    /*
    This function is used to get some information from the web.
    */
    httplib::Client client{API_ENDPOINT};
    auto res = client.Get(resource);
    if (res && res->status == httplib::OK_200) {
        return res->body;
    } else {
        std::exit(1);
        return std::string();
    } 
}

// Citation class

std::string Citation::getResource() const {
    std::exit(1);
}

std::string Citation::toString() const {
    return std::string("[" + id + "] ");
}

// Article class

std::string Article::getResource() const {
    std::exit(1);
}

std::string Article::toString() const {
    /*
    This function is used to describe an article.
    */
    auto info = data;
    if (data.is_null()) {
        std::exit(1);
    }
    if (info.contains("title") &&
        info.contains("author") &&
        info.contains("journal") &&
        info.contains("year") &&
        info.contains("volume") &&
        info.contains("issue") &&        
        info["title"].is_string() && 
        info["author"].is_string() && 
        info["journal"].is_string() && 
        info["year"].is_number() && 
        info["volume"].is_number() && 
        info["issue"].is_number()) 
    {        
        std::string title       =    info["title"].get<std::string>();
        std::string author      =    info["author"].get<std::string>();
        std::string journal     =    info["journal"].get<std::string>();
        std::string year        =    std::to_string(info["year"].get<int>());
        std::string volume      =    std::to_string(info["volume"].get<int>());
        std::string issue       =    std::to_string(info["issue"].get<int>());
        return std::string("[" + id + "] article: " + author + ", " + title + ", " + journal + ", " + year + ", " + volume + ", " + issue );
    } else {
        std::exit(1);
    }
}

// Book class

std::string Book::getResource() const {
    /*
    This function is used to get book information from the web.
    It sends a GET request to the API with the ISBN of the book.
    If the response is OK (HTTP 200), it processes the response.
    */
    return getFromWeb("/isbn/" + encodeUriComponent(isbn));
}

std::string Book::toString() const {
    /*
    This function is used to describe a book.
    */
    auto info = nlohmann::json::parse(getResource());
    if (data.is_null()) {
        std::exit(1);
    }
    if (info.contains("author") &&
        info.contains("title") &&
        info.contains("publisher") &&
        info.contains("year") &&
        info["author"].is_string() && 
        info["title"].is_string() && 
        info["publisher"].is_string() && 
        info["year"].is_string()) 
    {
        std::string author      =    info["author"].get<std::string>();
        std::string title       =    info["title"].get<std::string>();
        std::string publisher   =    info["publisher"].get<std::string>();
        std::string year        =    info["year"].get<std::string>();

        return std::string("[" + id + "] book: " + author + ", " + title + ", " + publisher + ", " + year);
    } else {
        std::exit(1);
    }
}

// WebPage class

std::string WebPage::getResource() const {
    /*
    This function is used to get website information from the web.
    It sends a GET request to the API with the URL of the website.
    If the response is OK (HTTP 200), it processes the response.
    */
    return getFromWeb("/title/" + encodeUriComponent(url));
}

std::string WebPage::toString() const {
    /*
    This function is used to describe a webpage.
    */
    auto info = nlohmann::json::parse(getResource());
    if (data.is_null()) {
        std::exit(1);
    }
    if (info.contains("title") && info["title"].is_string()) {
        std::string title = info["title"].get<std::string>();
        return std::string("[" + id + "] webpage: " + title + ". Available at " + url);
    } else {
        std::exit(1);
    }
    
}