#ifndef PIPELINE_H
#define PIPELINE_H

#include "CurlProcess.h"
#include <string>
#include <vector>

#include "soapH.h"

class Pipeline: public CurlProcess
{
    public:
      Pipeline(Config *pconfig);
      Config * m_pconfig;
      bool request(struct soap *soap, const char * pipeline_name_or_uuid);
      std::vector<api__Pipeline*>  * m_pPipeline;
};

#endif