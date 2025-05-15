//
// Created by shfdis on 5/15/25.
//

#ifndef SelectRequest_H
#define SelectRequest_H
#include <ranges>
#include <stdexcept>
#include <string>
#include <catch2/generators/catch_generators.hpp>

class RequestValue {
protected:
    bool is_based_ = false;

    bool table_selected_ = false;

    bool is_finalized_ = false;

    std::string request_;

    RequestValue() = default;

public:
    std::string ToString() const;
};

class Request : public RequestValue {
public:
    virtual Request &From(std::string table);

    virtual Request &Where(std::string condition);

    virtual Request &OrderBy(std::string field);

    virtual Request &OrderByDesc(std::string field);

    virtual Request &Limit(int);

    virtual Request &Finalize();
};

class SelectRequest : public Request {
private:
    SelectRequest() = default;

public:
    static SelectRequest Select(std::string columns);
};

class DeleteRequest : public Request {
private:
    DeleteRequest() = default;

public:
    static DeleteRequest Delete();
};

class InsertRequest : public RequestValue {
private:
    bool columns_ = false;
    size_t args = 0;

    InsertRequest() = default;

public:
    static InsertRequest Insert(std::string table);

    template<typename T>
    InsertRequest &Columns(T values) {
        if (columns_) {
            throw std::logic_error("already set");
        }
        columns_ = true;
        args = values.size();
        request_ += " (";
        for (auto i: values) {
            request_ += i + ",";
        }
        if (!values.empty()) {
            request_.pop_back();
        }
        request_ += ")";
        return *this;
    }

    template<typename T>
    InsertRequest &Values(T value) {
        if (!table_selected_ || !is_based_ || args != value.size()) {
            throw std::runtime_error("Request does not have a table select");
        }
        request_ += " VALUES(";
        for (auto i: value) {
            request_ += i + ",";
        }
        if (!value.empty()) {
            request_.pop_back();
        }
        request_ += ")";
        return *this;
    }

    InsertRequest &Finalize();
};
#endif //SelectRequest_H
