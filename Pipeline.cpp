#include "Pipeline.h"
#include "Config.h"


Pipeline::Pipeline(Config *pconfig): CurlProcess(pconfig){
  m_pconfig = pconfig;
}

bool Pipeline::request(struct soap *soap, const char * pipeline_name_or_uuid)
{
  char routeBuf[m_pconfig->URL_MAX_LEN];
  snprintf(routeBuf, sizeof(routeBuf), "%s/pipelines/%s",
	   m_pconfig->CATIWEB_WEBSERVICE_API.c_str(),
	   pipeline_name_or_uuid);

  if (m_pconfig->VERBOSE == 1L)
  {
    std::cout << "pipelie route:" << routeBuf << std::endl;
  }

  if(!CurlProcess::request(soap, routeBuf))
  {
    return false;
  }
  
  if (m_pconfig->VERBOSE == 1L)
  {
    std::cout << this->m_resBuf << std::endl;
  }
  
  if(!CurlProcess::parse_reponse_by_json())
  {
    return false;
  }
  
  m_pPipeline = soap_new_std__vectorTemplateOfPointerToapi__Pipeline(soap, 1);

  api__Pipeline * api__Pipeline_ = soap_new_api__Pipeline(soap, 1);
  m_pPipeline->push_back(api__Pipeline_);

  api__Pipeline_->name = m_document["name"].GetString();

  api__Pipeline_->identifier = m_document["identifier"].GetString();

  std::string * pDescription = soap_new_std__string(soap, 1);

  (*pDescription) = m_document["description"].GetString();
  api__Pipeline_->description = pDescription;
  api__Pipeline_->version = m_document["version"].GetString();
  
  // process parameters
  const rapidjson::Value & parameters = m_document["parameters"];
  for (rapidjson::SizeType i = 0; i < parameters.Size(); i++){
    const rapidjson::Value& parameter = parameters[i];
    api__PipelineParameter * api__PipelineParameter_ = soap_new_api__PipelineParameter(soap, 1);
    api__PipelineParameter_->name = parameter["name"].GetString();
    api__PipelineParameter_->isOptional = parameter["isOptional"].GetBool();
    api__PipelineParameter_->isReturnedValue = parameter["isReturnedValue"].GetBool();

    if(parameter.HasMember("description"))
    {
      api__PipelineParameter_->description = soap_new_std__string(soap, 1);
    }

    std::string type_str = parameter["type"].GetString();

    if(type_str == "path")
    {
      api__PipelineParameter_->type = api__ParameterType__File;
      
      api__PipelineParameter_->defaultValue = soap_new_api__ParameterTypedValue(soap, 1);
      (*api__PipelineParameter_->defaultValue).type = api__PipelineParameter_->type;
      (*api__PipelineParameter_->defaultValue).__union_ParameterTypedValue = 1;
      (*api__PipelineParameter_->defaultValue).union_ParameterTypedValue.valueStr = soap_new_std__string(soap, 1);
      (*(*api__PipelineParameter_->defaultValue).union_ParameterTypedValue.valueStr) = parameter["defaultValue"].GetString();
    }
    else if(type_str == "float")
    {
      api__PipelineParameter_->type = api__ParameterType__Double;
      
      api__PipelineParameter_->defaultValue = soap_new_api__ParameterTypedValue(soap, 1);
      (*api__PipelineParameter_->defaultValue).type = api__PipelineParameter_->type;
      (*api__PipelineParameter_->defaultValue).__union_ParameterTypedValue = 4;
      
      (*api__PipelineParameter_->defaultValue).union_ParameterTypedValue.valueDouble = parameter["defaultValue"].GetDouble();
    }
    else if(type_str == "string")
    {
      api__PipelineParameter_->type = api__ParameterType__String;
      
      api__PipelineParameter_->defaultValue = soap_new_api__ParameterTypedValue(soap, 1);
      (*api__PipelineParameter_->defaultValue).type = api__PipelineParameter_->type;
      (*api__PipelineParameter_->defaultValue).__union_ParameterTypedValue = 1;
      (*api__PipelineParameter_->defaultValue).union_ParameterTypedValue.valueStr = soap_new_std__string(soap, 1);
      (*(*api__PipelineParameter_->defaultValue).union_ParameterTypedValue.valueStr) = parameter["defaultValue"].GetString();
    }
    else if(type_str == "integer")
    {
      api__PipelineParameter_->type = api__ParameterType__Int64;
      
      api__PipelineParameter_->defaultValue = soap_new_api__ParameterTypedValue(soap, 1);
      (*api__PipelineParameter_->defaultValue).type = api__PipelineParameter_->type;
      (*api__PipelineParameter_->defaultValue).__union_ParameterTypedValue = 3;
      (*api__PipelineParameter_->defaultValue).union_ParameterTypedValue.valueInt = parameter["defaultValue"].GetInt();
    }
    else if(type_str == "boolean")
    {
      api__PipelineParameter_->type = api__ParameterType__Boolean;

      api__PipelineParameter_->defaultValue = soap_new_api__ParameterTypedValue(soap, 1);
      (*api__PipelineParameter_->defaultValue).type = api__PipelineParameter_->type;
      (*api__PipelineParameter_->defaultValue).__union_ParameterTypedValue = 2;
      (*api__PipelineParameter_->defaultValue).union_ParameterTypedValue.valueBool = parameter["defaultValue"].GetBool();
    }
    else if(memcmp(type_str.c_str(), "list_", 5) == 0)
    {
      api__PipelineParameter_->type = api__ParameterType__List;
    }

    // TODO: default Value
    //api__PipelineParameter_->defaultValue = parameter["defaultValue"].GetBool();

    api__Pipeline_->parameters.push_back(api__PipelineParameter_);
  }
  
  //api__Pipeline_->errorCodesAndMessages = soap_new_std__vectorTemplateOfPointerToapi__IntKeyStringValuePair(soap, 1);
  
  const rapidjson::Value & json_errorCodesAndMessages = m_document["errorCodesAndMessages"];
  for (rapidjson::SizeType i = 0; i < json_errorCodesAndMessages.Size(); i++){
    const rapidjson::Value & code_msg_pair = json_errorCodesAndMessages[i];
    const rapidjson::Value & code = code_msg_pair[0];
    const rapidjson::Value & msg = code_msg_pair[1];
    
    api__IntKeyStringValuePair* api__IntKeyStringValuePair_ = soap_new_api__IntKeyStringValuePair(soap, 1);
    
    api__IntKeyStringValuePair_->name = code.GetInt();
    api__IntKeyStringValuePair_->value = msg.GetString();
    
    api__Pipeline_->errorCodesAndMessages.push_back(api__IntKeyStringValuePair_);
  }
  
  
  
  return true;
}