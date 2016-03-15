#include "Pipelines.h"
#include "Config.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"


Pipelines::Pipelines(){
  this->m_pPipelines = NULL;
}

bool Pipelines::request(struct soap *soap, const char * study_name){

  Config config = Config();
  char routeBuf[config.URL_MAX_LEN];
  snprintf(routeBuf, sizeof(routeBuf), "%s/%s/pipelines",
	   config.CATIWEB_WEBSERVICE_API, study_name);

  std::cout << "pipelie route:" << routeBuf << std::endl;

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

  if (this->m_pPipelines==NULL)
  {
    this->m_pPipelines = soap_new_std__vectorTemplateOfPointerToapi__ArrayOfPipelines(soap, 1);
  }else{
    this->m_pPipelines->clear();
  }

  api__ArrayOfPipelines* api__ArrayOfPipelines_ = soap_new_api__ArrayOfPipelines(soap, 1);
  this->m_pPipelines->push_back(api__ArrayOfPipelines_);

  for(rapidjson::SizeType i = 0; i < m_document.Size(); i++)
  {
    api__Pipeline * api__Pipeline_ = soap_new_api__Pipeline(soap, 1);
    const rapidjson::Value & pipeline_value = m_document[i];
    api__Pipeline_->identifier = pipeline_value["identifier"].GetString();
    api__Pipeline_->name = pipeline_value["name"].GetString();
    (*this->m_pPipelines)[0]->item.push_back(api__Pipeline_);
  }  

  return true;
}