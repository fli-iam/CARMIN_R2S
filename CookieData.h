#include <string>
#include <vector>
#include "soapH.h"

#ifndef COOKIEDATA_H
#define COOKIEDATA_H

#include "CatiwebAuth.h"

class CookieData
{
    public:
      CookieData();
      bool parseData(const char * data);
      std::vector<std::string> split_string(const char * c_data, const char * c_delimiter="\t");
   
      void set_curl_cookie_to_gsoap_cookie(
	struct soap *soap,
	PCURL_SLIST cookies_data
      );
   
      bool set_gsoap_cookie_to_curl_cookie(
	struct soap *soap,
	CURL *curl,
	const char * name="auth_tkt"
      );
    public:
      // 3.5 What are all those entries in my cookies.txt file? 
      // http://www.cookiecentral.com/faq/#3.5
      std::string domain;
      bool flag;
      std::string path;
      bool secure;
      unsigned long expiration;
      std::string name;
      std::string value;
};

#endif
