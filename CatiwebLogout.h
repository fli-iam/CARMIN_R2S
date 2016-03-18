#include <curl/curl.h>

#ifndef CATIWEBLOGOUT_H
#define CATIWEBLOGOUT_H

#include "CurlProcess.h"

#include "soapH.h"
#include "Config.h"

class CatiwebLogout: public CurlProcess
{
    public:
      CatiwebLogout(Config * config);
      bool request(struct soap *soap);

      Config * m_pconfig;
};

#endif
