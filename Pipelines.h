#ifndef PIPELINES_H
#define PIPELINES_H

#include "CurlProcess.h"
#include <string>
#include <vector>

#include "soapH.h"
#include "Config.h"

class Pipelines: public CurlProcess
{
    public:
      Pipelines(Config *pconfig);
      Pipelines(Pipelines& pipelines, Config *pconfig);
      Config *m_pconfig;
      bool request(struct soap *soap, const char * study_name);
    public:
      std::vector<api__ArrayOfPipelines*> * m_pPipelines;
};

#endif