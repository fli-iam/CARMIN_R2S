#include <curl/curl.h>


#ifndef CATIWEBAUTH_H
#define CATIWEBAUTH_H

#include <string>
#include "Config.h"

typedef curl_slist * PCURL_SLIST;

class CatiwebAuth
{
    public:
      
      CatiwebAuth(Config *config);
      bool Login(const char * username,  const char * password, PCURL_SLIST& cookies_data);
      
      Config * m_pconfig;
      std::string m_error_message;
};

#endif
