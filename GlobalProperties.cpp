#include "GlobalProperties.h"
#include "Config.h"


GlobalProperties::GlobalProperties(){
  m_ns1__GlobalProperties = NULL;
}

/*
  
/// "http://france-life-imaging.fr/api":GlobalProperties is a complexType with complexContent extension of "http://france-life-imaging.fr/api":Object.
class ns1__GlobalProperties : public ns1__Object
{ public:
/// Vector of ns1__IntKeyStringValuePair* with length 0..unbounded
    std::vector<ns1__IntKeyStringValuePair*> APIErrorCodesAndMessages       0;
/// Vector of std::string with length 1..unbounded
    std::vector<std::string            > supportedTransferProtocol      1;
/// Vector of enum ns1__Module with length 1..unbounded
    std::vector<enum ns1__Module       > supportedModule                1;
/// Element email of type xs:string.
    std::string*                         email                          0;	///< Optional element.
/// Element platformDescription of type xs:string.
    std::string*                         platformDescription            0;	///< Optional element.
/// Element minAuthorizedExecutionTimeout of type xs:int.
    int*                                 minAuthorizedExecutionTimeout  0;	///< Optional element.
/// Element maxAuthorizedExecutionTimeout of type xs:int.
    int*                                 maxAuthorizedExecutionTimeout  0;	///< Optional element.
/// Element defaultExecutionTimeout of type xs:int.
    int*                                 defaultExecutionTimeout        0;	///< Optional element.
/// Element isKillExecutionSupported of type xs:boolean.
    bool                                 isKillExecutionSupported       1;	///< Required element.
/// Element defaultStudy of type xs:string.
    std::string*                         defaultStudy                   0;	///< Optional element.
/// Element supportedAPIVersion of type xs:string.
    std::string                          supportedAPIVersion            1;	///< Required element.
};

enum ns1__Module
{
	ns1__Module__Processing,	///< xs:string value="Processing"
	ns1__Module__Data,	///< xs:string value="Data"
	ns1__Module__Management,	///< xs:string value="Management"
	ns1__Module__Commercial,	///< xs:string value="Commercial"
};


*/


bool GlobalProperties::request(struct soap *soap){
    if(!m_ns1__GlobalProperties)
    {
      m_ns1__GlobalProperties = soap_new_ns1__GlobalProperties(soap, 1);
    }

    Config config = Config();
    char routeBuf[config.URL_MAX_LEN];
    snprintf(routeBuf, sizeof(routeBuf), "%s/platform",
	    config.CATIWEB_WEBSERVICE_API);

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
    
    rapidjson::Value &val_apiErrorCodesAndMessages = m_document["apiErrorCodesAndMessages"];
    for(int i=0; i< val_apiErrorCodesAndMessages.Size(); i+=1)
    {
      int error_code = val_apiErrorCodesAndMessages[i][0].GetInt();
      std::string error_msg = val_apiErrorCodesAndMessages[i][1].GetString();
      ns1__IntKeyStringValuePair * ns1__IntKeyStringValuePair_ = soap_new_ns1__IntKeyStringValuePair(soap, 1);
      ns1__IntKeyStringValuePair_->name = error_code;
      ns1__IntKeyStringValuePair_->value = error_msg;
      m_ns1__GlobalProperties->APIErrorCodesAndMessages.push_back(ns1__IntKeyStringValuePair_);
    }

    rapidjson::Value &val_supportedTransferProtocols = m_document["supportedTransferProtocols"];
    for(int i=0; i< val_supportedTransferProtocols.Size(); i+=1)
    {
      std::string file_protocol = val_supportedTransferProtocols[i].GetString();
      m_ns1__GlobalProperties->supportedTransferProtocol.push_back(file_protocol);
    }

    rapidjson::Value &val_supportedModules = m_document["supportedModules"];
    for(int i=0; i< val_supportedModules.Size(); i+=1)
    {
      std::string supported_modules = val_supportedModules[i].GetString();
      if(supported_modules=="Processing")
      {
	m_ns1__GlobalProperties->supportedModule.push_back(ns1__Module__Processing);
      }else if(supported_modules=="Data")
      {
	m_ns1__GlobalProperties->supportedModule.push_back(ns1__Module__Data);
      }else if(supported_modules=="Management")
      {
	m_ns1__GlobalProperties->supportedModule.push_back(ns1__Module__Management);
      }else if(supported_modules=="Commercial")
      {
	m_ns1__GlobalProperties->supportedModule.push_back(ns1__Module__Commercial);
      }
    }

    m_ns1__GlobalProperties->email = soap_new_std__string(soap, 1);
    (*m_ns1__GlobalProperties->email) = m_document["email"].GetString();

    m_ns1__GlobalProperties->platformDescription = soap_new_std__string(soap, 1);
    (*m_ns1__GlobalProperties->platformDescription) = m_document["description"].GetString();
    
    m_ns1__GlobalProperties->minAuthorizedExecutionTimeout = (int*)soap_malloc(soap, sizeof(int));
    (*m_ns1__GlobalProperties->minAuthorizedExecutionTimeout) = m_document["minAuthorizedExecutionTimeout"].GetInt();
    
    m_ns1__GlobalProperties->maxAuthorizedExecutionTimeout = (int*)soap_malloc(soap, sizeof(int));
    (*m_ns1__GlobalProperties->maxAuthorizedExecutionTimeout) = m_document["maxAuthorizedExecutionTimeout"].GetInt();
    
    m_ns1__GlobalProperties->defaultExecutionTimeout = (int*)soap_malloc(soap, sizeof(int));
    (*m_ns1__GlobalProperties->defaultExecutionTimeout) = m_document["defaultExecutionTimeout"].GetInt();
    
    m_ns1__GlobalProperties->isKillExecutionSupported = m_document["isKillExecutionSupported"].GetBool();
    
    m_ns1__GlobalProperties->defaultStudy = soap_new_std__string(soap, 1);
    (*m_ns1__GlobalProperties->defaultStudy) = m_document["defaultStudy"].GetString();

    m_ns1__GlobalProperties->supportedAPIVersion = m_document["supportedAPIVersion"].GetString();

    return true;
}
