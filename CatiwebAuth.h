#include <curl/curl.h>


#ifndef CATIWEBAUTH_H
#define CATIWEBAUTH_H

#include <string>

typedef curl_slist * PCURL_SLIST;

class CatiwebAuth
{
    public:
      bool Login(const char * username,  const char * password, PCURL_SLIST& cookies_data);
      std::string m_error_message;
};

#endif
