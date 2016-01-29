#ifndef PIPELINES_H
#define PIPELINES_H

#include "CurlProcess.h"
#include <string>
#include <vector>

#include "soapH.h"

class Pipelines: public CurlProcess
{
    public:
      Pipelines();
      bool request(struct soap *soap, const char * study_name);
    public:
      std::vector<ns1__ArrayOfPipelines*> * m_pPipelines;
};

#endif