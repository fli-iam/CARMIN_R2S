#include "Pipeline.h"

#include "Config.h"


Pipeline::Pipeline(){
  
}
bool Pipeline::request(struct soap *soap, const char * pipeline_name_or_uuid)
{
  Config config = Config();
  char routeBuf[config.URL_MAX_LEN];
  snprintf(routeBuf, sizeof(routeBuf), "%s/pipelines/%s",
	   config.CATIWEB_WEBSERVICE_API, pipeline_name_or_uuid);

  if (config.VERBOSE == 1L)
  {
    std::cout << "pipelie route:" << routeBuf << std::endl;
  }

  if(!CurlProcess::request(soap, routeBuf))
  {
    return false;
  }
  
  if (config.VERBOSE == 1L)
  {
    std::cout << this->m_resBuf << std::endl;
  }
  
  if(!CurlProcess::parse_reponse_by_json())
  {
    return false;
  }
  
  m_pPipeline = soap_new_std__vectorTemplateOfPointerTons1__Pipeline(soap, 1);

  ns1__Pipeline * ns1__Pipeline_ = soap_new_ns1__Pipeline(soap, 1);
  m_pPipeline->push_back(ns1__Pipeline_);

  ns1__Pipeline_->name = m_document["name"].GetString();
  ns1__Pipeline_->identifier = m_document["identifier"].GetString();
  
  std::string * pDescription = soap_new_std__string(soap, 1);
  (*pDescription) = m_document["description"].GetString();
  ns1__Pipeline_->description = pDescription;
  ns1__Pipeline_->version = m_document["version"].GetString();
  
  
  return true;
}