#include <curl/curl.h>

#ifndef CATIWEBLOGOUT_H
#define CATIWEBLOGOUT_H

#include "CurlProcess.h"

#include "soapH.h"

class CatiwebLogout: public CurlProcess
{
    public:
      CatiwebLogout();
      bool request(struct soap *soap);
};

#endif
