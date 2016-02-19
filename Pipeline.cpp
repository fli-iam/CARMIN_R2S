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
  
  // process parameters
  const rapidjson::Value & parameters = m_document["parameters"];
  for (rapidjson::SizeType i = 0; i < parameters.Size(); i++){
    const rapidjson::Value& parameter = parameters[i];
    ns1__PipelineParameter * ns1__PipelineParameter_ = soap_new_ns1__PipelineParameter(soap, 1);
    ns1__PipelineParameter_->name = parameter["name"].GetString();
    ns1__PipelineParameter_->isOptional = parameter["isOptional"].GetBool();
    ns1__PipelineParameter_->isReturnedValue = parameter["isReturnedValue"].GetBool();

    if(parameter.HasMember("description"))
    {
      ns1__PipelineParameter_->description = soap_new_std__string(soap, 1);
    }

    std::string type_str = parameter["type"].GetString();

    if(type_str == "path")
    {
      ns1__PipelineParameter_->type = ns1__ParameterType__File;
      
      ns1__PipelineParameter_->defaultValue = soap_new_ns1__ParameterTypedValue(soap, 1);
      (*ns1__PipelineParameter_->defaultValue).type = ns1__PipelineParameter_->type;
      (*ns1__PipelineParameter_->defaultValue).__union_ParameterTypedValue = 1;
      (*ns1__PipelineParameter_->defaultValue).union_ParameterTypedValue.valueStr = soap_new_std__string(soap, 1);
      (*(*ns1__PipelineParameter_->defaultValue).union_ParameterTypedValue.valueStr) = parameter["defaultValue"].GetString();
    }
    else if(type_str == "float")
    {
      ns1__PipelineParameter_->type = ns1__ParameterType__Double;
      
      ns1__PipelineParameter_->defaultValue = soap_new_ns1__ParameterTypedValue(soap, 1);
      (*ns1__PipelineParameter_->defaultValue).type = ns1__PipelineParameter_->type;
      (*ns1__PipelineParameter_->defaultValue).__union_ParameterTypedValue = 4;
      
      (*ns1__PipelineParameter_->defaultValue).union_ParameterTypedValue.valueDouble = parameter["defaultValue"].GetDouble();
    }
    else if(type_str == "string")
    {
      ns1__PipelineParameter_->type = ns1__ParameterType__String;
      
      ns1__PipelineParameter_->defaultValue = soap_new_ns1__ParameterTypedValue(soap, 1);
      (*ns1__PipelineParameter_->defaultValue).type = ns1__PipelineParameter_->type;
      (*ns1__PipelineParameter_->defaultValue).__union_ParameterTypedValue = 1;
      (*ns1__PipelineParameter_->defaultValue).union_ParameterTypedValue.valueStr = soap_new_std__string(soap, 1);
      (*(*ns1__PipelineParameter_->defaultValue).union_ParameterTypedValue.valueStr) = parameter["defaultValue"].GetString();
    }
    else if(type_str == "integer")
    {
      ns1__PipelineParameter_->type = ns1__ParameterType__Int64;
      
      ns1__PipelineParameter_->defaultValue = soap_new_ns1__ParameterTypedValue(soap, 1);
      (*ns1__PipelineParameter_->defaultValue).type = ns1__PipelineParameter_->type;
      (*ns1__PipelineParameter_->defaultValue).__union_ParameterTypedValue = 3;
      (*ns1__PipelineParameter_->defaultValue).union_ParameterTypedValue.valueInt = parameter["defaultValue"].GetInt();
    }
    else if(type_str == "boolean")
    {
      ns1__PipelineParameter_->type = ns1__ParameterType__Boolean;

      ns1__PipelineParameter_->defaultValue = soap_new_ns1__ParameterTypedValue(soap, 1);
      (*ns1__PipelineParameter_->defaultValue).type = ns1__PipelineParameter_->type;
      (*ns1__PipelineParameter_->defaultValue).__union_ParameterTypedValue = 2;
      (*ns1__PipelineParameter_->defaultValue).union_ParameterTypedValue.valueBool = parameter["defaultValue"].GetBool();
    }
    else if(memcmp(type_str.c_str(), "list_", 5) == 0)
    {
      ns1__PipelineParameter_->type = ns1__ParameterType__List;
    }

    // TODO: default Value
    //ns1__PipelineParameter_->defaultValue = parameter["defaultValue"].GetBool();

    ns1__Pipeline_->parameters.push_back(ns1__PipelineParameter_);
  }
  
  //ns1__Pipeline_->errorCodesAndMessages = soap_new_std__vectorTemplateOfPointerTons1__IntKeyStringValuePair(soap, 1);
  
  const rapidjson::Value & json_errorCodesAndMessages = m_document["errorCodesAndMessages"];
  for (rapidjson::SizeType i = 0; i < json_errorCodesAndMessages.Size(); i++){
    const rapidjson::Value & code_msg_pair = json_errorCodesAndMessages[i];
    const rapidjson::Value & code = code_msg_pair[0];
    const rapidjson::Value & msg = code_msg_pair[1];
    
    ns1__IntKeyStringValuePair* ns1__IntKeyStringValuePair_ = soap_new_ns1__IntKeyStringValuePair(soap, 1);
    
    ns1__IntKeyStringValuePair_->name = code.GetInt();
    ns1__IntKeyStringValuePair_->value = msg.GetString();
    
    ns1__Pipeline_->errorCodesAndMessages.push_back(ns1__IntKeyStringValuePair_);
  }
  
  
  
  return true;
}