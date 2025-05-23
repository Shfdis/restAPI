//
// Created by shfdis on 5/15/25.
//

#include "request.h"

#include <stdexcept>

bool RequestValue::is_ready() const { return is_finalized_; }

SelectRequest SelectRequest::Select(std::string columns) {
  SelectRequest request;
  request.is_based_ = true;
  request.request_ = "SELECT " + columns;
  return request;
}

Request &Request::From(std::string table) {
  if (!this->is_based_) {
    throw std::logic_error("Request is not based on request type");
  }
  if (table_selected_) {
    throw std::logic_error("Request already exists");
  }
  request_ += " FROM " + table;
  table_selected_ = true;
  return *this;
}

Request &Request::Where(std::string condition) {
  if (!this->is_based_ || !table_selected_) {
    throw std::logic_error(
        "Request is not based on request type or table is not selected");
  }
  request_ += " WHERE " + condition;
  return *this;
}

Request &Request::OrderBy(std::string field) {
  if (!this->is_based_ || !table_selected_) {
    throw std::logic_error(
        "Request is not based on request type or table is not selected");
  }
  request_ += " ORDER BY " + field;
  return *this;
}

Request &Request::Limit(int limit) {
  if (!this->is_based_ || !table_selected_) {
    throw std::logic_error(
        "Request is not based on request type or table is not selected");
  }
  if (limit <= 0) {
    throw std::logic_error("LIMIT must be greater than zero");
  }
  request_ += " LIMIT " + std::to_string(limit);
  return *this;
}

Request &Request::OrderByDesc(std::string field) {
  if (!this->is_based_ || !table_selected_) {
    throw std::logic_error(
        "Request is not based on request type or table is not selected");
  }
  request_ += " ORDER BY " + field + " DESC";
  return *this;
}

Request &Request::Finalize() {
  if (!this->is_based_ || !table_selected_) {
    throw std::logic_error(
        "Request is not based on request type or table is not selected");
  }
  request_ += ";";
  is_finalized_ = true;
  return *this;
}

std::string RequestValue::ToString() const { return request_; }

DeleteRequest DeleteRequest::Delete() {
  DeleteRequest request;
  request.is_based_ = true;
  request.request_ = "DELETE";
  return request;
}

InsertRequest InsertRequest::Insert(std::string table) {
  InsertRequest request;
  request.is_based_ = true;
  request.request_ = "INSERT INTO " + table;
  request.table_selected_ = true;
  return request;
}

InsertRequest &InsertRequest::Finalize() {
  if (!is_based_ || !table_selected_ || !columns_) {
    throw std::logic_error(
        "Request is not based on request type or table is not selected");
  }
  request_ += ";";
  is_finalized_ = true;
  return *this;
}
