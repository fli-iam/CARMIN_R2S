#ifndef GLOBALPROPERTIES_H
#define GLOBALPROPERTIES_H

#include "CurlProcess.h"
#include <string>
#include <vector>

#include "soapH.h"

class GlobalProperties: public CurlProcess
{
    public:
      GlobalProperties(Config *pconfig);
      bool request(struct soap *soap);
      api__GlobalProperties * m_api__GlobalProperties;
      Config *m_pconfig;
};

#endif