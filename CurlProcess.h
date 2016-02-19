#ifndef CURPROCESS_H
#define CURPROCESS_H

#include <string>
#include "rapidjson/document.h"


class CurlProcess{
  public:
    CurlProcess();
    CurlProcess(const CurlProcess&);
    std::string m_resBuf;
    std::string m_error_message;

    bool request(struct soap *soap,
		 const std::string &route,
		 std::string method = "get",
		 const char * post_field_name = NULL,
		 const char * post_data = NULL,
		 const long post_data_size = 0);

    bool parse_reponse_by_json();

    rapidjson::Document m_document;

};

#endif