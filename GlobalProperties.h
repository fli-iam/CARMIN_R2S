#ifndef GLOBALPROPERTIES_H
#define GLOBALPROPERTIES_H

#include "CurlProcess.h"
#include <string>
#include <vector>

#include "soapH.h"

class GlobalProperties: public CurlProcess
{
    public:
      GlobalProperties();
      bool request(struct soap *soap);
      ns1__GlobalProperties * m_ns1__GlobalProperties;
};

#endif