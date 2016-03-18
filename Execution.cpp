#include "Execution.h"
#include "Config.h"
#include "CookieData.h"

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"


Execution::Execution(Config *pconfig):CurlProcess(pconfig)
{
  this->m_api__Execution = NULL;
  this->m_returnedValueListExecutions = NULL;
  
  this->m_std_out = NULL;
  this->m_std_err = NULL;
  
  m_pconfig = pconfig;
}

bool Execution::deleteExecution(
    struct soap *soap,
    api__deleteExecution*	api__deleteExecution_
      )
{
  /*
   class api__deleteExecution
  { public:
  /// Element executionId of type xs:string.
      std::string                          executionId                    1;	///< Required element.
  /// Element deleteFiles of type xs:boolean.
      bool                                 deleteFiles                    1;	///< Required element.
  /// A handle to the soap struct that manages this instance (automatically set)
      struct soap                         *soap                          ;
  };
  */
  
  std::string executionId = api__deleteExecution_->executionId;
  bool deleteFiles = api__deleteExecution_->deleteFiles;
  
  Config config = Config();
  char routeBuf[m_pconfig->URL_MAX_LEN];
  snprintf(routeBuf, sizeof(routeBuf), "%s/executions/%s",
	    m_pconfig->CATIWEB_WEBSERVICE_API.c_str(), executionId.c_str());
  if(!CurlProcess::request(soap, routeBuf, "delete"))
  {
    return false;
  }
  
  if(!CurlProcess::parse_reponse_by_json())
  {
    return false;
  }
  
  if(!m_document["is_success"].GetBool())
  {
    m_error_message = m_document["error_message"].GetString();
    return false;
  }
  return true;
}

bool Execution::listExecutions(
  struct soap *soap,
  api__listExecutions* api__listExecutions_
){
    if(api__listExecutions_->studyIdentifier=="")
    {
      m_error_message = "studyIdentifier cannot be empty...";
      return false;
    }

    std::string studyIdentifier = api__listExecutions_->studyIdentifier;
    
    Config config = Config();
    char routeBuf[m_pconfig->URL_MAX_LEN];
    snprintf(routeBuf, sizeof(routeBuf), "%s/%s/executions",
	     m_pconfig->CATIWEB_WEBSERVICE_API.c_str(), studyIdentifier.c_str());

    if(!CurlProcess::request(soap, routeBuf, "get"))
    {
      return false;
    }

    if(m_pconfig->VERBOSE)
    {
      std::cout << "route=" << routeBuf << std::endl;
      std::cout << m_resBuf << std::endl;
    }
    
    if(!CurlProcess::parse_reponse_by_json())
    {
      return false;
    }

    if(m_returnedValueListExecutions == NULL)
    {
      m_returnedValueListExecutions = soap_new_std__vectorTemplateOfPointerToapi__ArrayOfExecutions(soap, 1);
    }

    /*
      /// "http://france-life-imaging.fr/api":Execution is a complexType with complexContent extension of "http://france-life-imaging.fr/api":Object.
      class api__Execution : public api__Object
      { public:
      /// Element identifier of type xs:string.
	  std::string                          identifier                     1;	///< Required element.
      /// Element name of type xs:string.
	  std::string                          name                           1;	///< Required element.
      /// Element pipelineIdentifier of type xs:string.
	  std::string                          pipelineIdentifier             1;	///< Required element.
      /// Element timeout of type xs:int.
	  int*                                 timeout                        0;	///< Optional element.
      /// Element status of type "http://france-life-imaging.fr/api":ExecutionStatus.
	  enum api__ExecutionStatus            status                         1;	///< Required element.
      /// Vector of api__StringKeyParameterValuePair* with length 0..unbounded
	  std::vector<api__StringKeyParameterValuePair*> inputValue                     0;
      /// Vector of api__StringKeyParameterValuePair* with length 0..unbounded
	  std::vector<api__StringKeyParameterValuePair*> returnedFile                   0;
      /// Element studyIdentifier of type xs:string.
	  std::string*                         studyIdentifier                0;	///< Optional element.
      /// Element errorCode of type xs:int.
	  int*                                 errorCode                      0;	///< Optional element.
      /// Element startDate of type xs:long.
	  LONG64*                              startDate                      0;	///< Optional element.
      /// Element endDate of type xs:long.
	  LONG64*                              endDate                        0;	///< Optional element.
      };
    */
    api__ArrayOfExecutions * api__ArrayOfExecutions_ = soap_new_api__ArrayOfExecutions(soap, 1);
    m_returnedValueListExecutions->clear();
    m_returnedValueListExecutions->push_back(api__ArrayOfExecutions_);

    for(rapidjson::Value::ConstValueIterator itr = m_document.Begin(); itr != m_document.End(); ++itr)
    {
	api__Execution * api__Execution_ = soap_new_api__Execution(soap, 1);
	char buf_identifier[256];
	snprintf(buf_identifier, sizeof(buf_identifier), "%d", (*itr)["id"].GetInt());

	api__Execution_->identifier = buf_identifier;
	api__Execution_->name = (*itr)["pipeline_name"].GetString();
	api__Execution_->pipelineIdentifier = (*itr)["pipeline_id"].GetString();

	api__Execution_->status = this->convert_soma_workflow_status_to_carmin_status(
	    (*itr)["status"].GetString());

	api__ArrayOfExecutions_->item.push_back(api__Execution_);
    }

    return true;
}


api__ExecutionStatus Execution::convert_soma_workflow_status_to_carmin_status(std::string soma_workflow_status)
{
      api__ExecutionStatus execution_status ; 
      if(soma_workflow_status=="workflow_done")
      {
	execution_status = api__ExecutionStatus__Finished;
      }else if(soma_workflow_status=="worklflow_not_started")
      {
	execution_status = api__ExecutionStatus__Ready;
      }
      else if(soma_workflow_status=="workflow_in_progress")
      {
	execution_status = api__ExecutionStatus__Running;
      }
      else if(soma_workflow_status=="delete_pending")
      {
	execution_status = api__ExecutionStatus__Killed;
      }
      else if(soma_workflow_status=="warning")
      {
	execution_status = api__ExecutionStatus__ExecutionFailed;
      }
      else{
	execution_status = api__ExecutionStatus__Unknown;
      }
      return execution_status;
}

bool Execution::updateExecution(struct soap *soap,
	std::string executionId,
	std::string name,
	std::string value
      )
{
  Config config = Config();

  char routeBuf[m_pconfig->URL_MAX_LEN];
    snprintf(routeBuf, sizeof(routeBuf), "%s/executions/%s",
	     m_pconfig->CATIWEB_WEBSERVICE_API.c_str(), executionId.c_str());

  rapidjson::Document document;
  document.SetObject();

  rapidjson::Value execution_op_name;
  rapidjson::Value execution_op_value;

  execution_op_name.SetString(
    name.c_str(),
    name.length(),
    document.GetAllocator()
  );

  execution_op_value.SetString(
    value.c_str(),
    value.length(),
    document.GetAllocator()
  );

  document.AddMember(execution_op_name,
		     execution_op_value,
		     document.GetAllocator());


  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  document.Accept(writer);

  if(m_pconfig->VERBOSE)
  {
    std::cout << buffer.GetString() << std::endl;
    std::cout << buffer.GetSize() << std::endl;
  }

  //char json_buffer[] = "{\"pipeline_name\": \"ProcessDemo\", \"params\":{\"ff\": 14.0}}";
  //std::cout << json_buffer << std::endl;

  if(!CurlProcess::request(soap, routeBuf, "PUT",
			    "UpdateExecution",
			    buffer.GetString(),
			    buffer.GetSize()))
  {
    return false;
  }

  if(!CurlProcess::parse_reponse_by_json())
  {
    return false;
  }

  if(m_pconfig->VERBOSE)
  {
    std::cout << "success parse_reponse_by_json" << std::endl;
  }
  
  if(!m_document["is_success"].GetBool())
  {
    m_error_message = m_document["error_message"].GetString();
    return false;
  }

  return true;
}
bool Execution::killExecution(struct soap *soap,
	std::string executionId
      ){
  
    std::string name = "name";
    std::string op = "kill";
    return this->updateExecution(
      soap,
      executionId,
      name, op
    );

    /*
    Config config = Config();

    char routeBuf[m_pconfig->URL_MAX_LEN];
    snprintf(routeBuf, sizeof(routeBuf), "%s/executions/%s",
	     m_pconfig->CATIWEB_WEBSERVICE_API, executionId.c_str());

    rapidjson::Document document;
    document.SetObject();

    rapidjson::Value execution_op_name("name");
    rapidjson::Value execution_op_value("kill");

    document.AddMember(execution_op_name,
		       execution_op_value,
		       document.GetAllocator());


    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    if(m_pconfig->VERBOSE)
    {
      std::cout << buffer.GetString() << std::endl;
      std::cout << buffer.GetSize() << std::endl;
    }

    //char json_buffer[] = "{\"pipeline_name\": \"ProcessDemo\", \"params\":{\"ff\": 14.0}}";
    //std::cout << json_buffer << std::endl;

    if(!CurlProcess::request(soap, routeBuf, "PUT",
			     "UpdateExecution",
			     buffer.GetString(),
			     buffer.GetSize()))
    {
      return false;
    }

    if(!CurlProcess::parse_reponse_by_json())
    {
      return false;
    }

    if(m_pconfig->VERBOSE)
    {
      std::cout << "success parse_reponse_by_json" << std::endl;
    }
    
    if(!m_document["is_success"].GetBool())
    {
      m_error_message = m_document["error_message"].GetString();
      return false;
    }

    return true;*/
}

bool Execution::playExecution(struct soap *soap,
	std::string executionId
      )
{

    Config config = Config();

    char routeBuf[m_pconfig->URL_MAX_LEN];
    snprintf(routeBuf, sizeof(routeBuf), "%s/executions/%s",
	     m_pconfig->CATIWEB_WEBSERVICE_API.c_str(), executionId.c_str());

    rapidjson::Document document;
    document.SetObject();

    rapidjson::Value execution_op_name("name");
    rapidjson::Value execution_op_value("start");

    document.AddMember(execution_op_name,
		       execution_op_value,
		       document.GetAllocator());


    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    if(m_pconfig->VERBOSE)
    {
      std::cout << buffer.GetString() << std::endl;
      std::cout << buffer.GetSize() << std::endl;
    }

    //char json_buffer[] = "{\"pipeline_name\": \"ProcessDemo\", \"params\":{\"ff\": 14.0}}";
    //std::cout << json_buffer << std::endl;

    if(!CurlProcess::request(soap, routeBuf, "PUT",
			     "UpdateExecution",
			     buffer.GetString(),
			     buffer.GetSize()))
    {
      return false;
    }

    if(!CurlProcess::parse_reponse_by_json())
    {
      return false;
    }

    if(m_pconfig->VERBOSE)
    {
      std::cout << "success parse_reponse_by_json" << std::endl;
    }
    
    if(!m_document["is_success"].GetBool())
    {
      m_error_message = m_document["error_message"].GetString();
      return false;
    }

    return true;
}

bool Execution::getStdOutErr(struct soap *soap,
	std::string executionId
      )
{
    Config config = Config();

    char routeBuf[m_pconfig->URL_MAX_LEN];
    snprintf(routeBuf, sizeof(routeBuf), "%s/executions/%s/std_out_err",
	     m_pconfig->CATIWEB_WEBSERVICE_API.c_str(), executionId.c_str());

    if(!CurlProcess::request(soap, routeBuf))
    {
      return false;
    }

    if(!CurlProcess::parse_reponse_by_json())
    {
      return false;
    }

    if(m_pconfig->VERBOSE)
    {
      std::cout << "success parse_reponse_by_json" << std::endl;
    }
    
    if(!m_document["is_success"].GetBool())
    {
      m_error_message = m_document["error_message"].GetString();
      return false;
    }

    if(this->m_std_out == NULL)
    {
      this->m_std_out = soap_new_std__vectorTemplateOfstd__string(soap, 1);
    }
    if(this->m_std_err == NULL)
    {
      this->m_std_err = soap_new_std__vectorTemplateOfstd__string(soap, 1);
    }

    this->m_std_out->clear();
    this->m_std_err->clear();

    (*this->m_std_out).push_back(m_document["return"]["stdout_summary"].GetString());
    (*this->m_std_err).push_back(m_document["return"]["stderr_summary"].GetString());

    return true;
}

bool Execution::getExecution(struct soap *soap,
	std::string executionId
      )
{

    Config config = Config();

    char routeBuf[m_pconfig->URL_MAX_LEN];
    snprintf(routeBuf, sizeof(routeBuf), "%s/executions/%s",
	     m_pconfig->CATIWEB_WEBSERVICE_API.c_str(), executionId.c_str());

    if(!CurlProcess::request(soap, routeBuf))
    {
      return false;
    }

    if(!CurlProcess::parse_reponse_by_json())
    {
      return false;
    }

    if(m_pconfig->VERBOSE)
    {
      std::cout << "success parse_reponse_by_json" << std::endl;
    }
    
    if(!m_document["is_success"].GetBool())
    {
      m_error_message = m_document["error_message"].GetString();
      return false;
    }

    if(this->m_api__Execution==NULL)
    {
      this->m_api__Execution = soap_new_api__Execution(soap, 1);
    }

    char buf_identifier[256];
    snprintf(buf_identifier, sizeof(buf_identifier), "%d", m_document["return"]["id"].GetInt());

    this->m_api__Execution->identifier = buf_identifier;
    this->m_api__Execution->name = m_document["return"]["pipeline_name"].GetString();
    this->m_api__Execution->pipelineIdentifier = m_document["return"]["pipeline_id"].GetString();

    this->m_api__Execution->status = 
      this->convert_soma_workflow_status_to_carmin_status(m_document["return"]["status"].GetString());

    return true;
}


bool Execution::initExecution(
      struct soap *soap,
      api__initExecution* api__initExecution_){

    if(api__initExecution_->studyId==NULL)
    {
      m_error_message = "studyId cannot be empty...";
      return false;
    }

    if(api__initExecution_->pipelineId=="")
    {
      m_error_message = "pipelineId cannot be empty...";
      return false;
    }

    std::string studyId = *api__initExecution_-> studyId;
    std::string pipelineId = api__initExecution_-> pipelineId;

    Config config = Config();
    char routeBuf[m_pconfig->URL_MAX_LEN];
    snprintf(routeBuf, sizeof(routeBuf), "%s/%s/executions",
	     m_pconfig->CATIWEB_WEBSERVICE_API.c_str(), studyId.c_str());

    std::vector<api__StringKeyParameterValuePair*> &inputValue = api__initExecution_->inputValue;

    int * timeout = api__initExecution_-> timeout;
    std::string * executionName = api__initExecution_-> executionName;
    bool * playExecution = api__initExecution_-> playExecution;

    rapidjson::Document document;
    document.SetObject();

    rapidjson::Value pipeline_name("pipeline_name");
    rapidjson::Value pipeline_value;
    pipeline_value.SetString(pipelineId.c_str(), pipelineId.length());

    document.AddMember(pipeline_name,
		       pipeline_value,
		       document.GetAllocator());

    rapidjson::Value params_str("params");
    rapidjson::Value params;
    params.SetObject();

    if(m_pconfig->VERBOSE)
    {
      std::cout << "params:" << std::endl;
    }
    for(int i=0; i < inputValue.size(); i += 1)
    {
      api__StringKeyParameterValuePair * api__StringKeyParameterValuePair = inputValue[i];
      if(m_pconfig->VERBOSE)
      {
	std::cout << api__StringKeyParameterValuePair->name << std::endl;
	std::cout << api__StringKeyParameterValuePair->value << std::endl;
	std::cout << "type:" << api__StringKeyParameterValuePair->value->type << std::endl;
      }
      rapidjson::Value param_key;
      std::string std_param_name = api__StringKeyParameterValuePair->name;
      param_key.SetString(std_param_name.c_str(),
			  std_param_name.length(),
			  document.GetAllocator()
 			);
      rapidjson::Value param_val;

      /*
       * /// "http://france-life-imaging.fr/api":ParameterTypedValue is a complexType.
	  class api__ParameterTypedValue
	  { public:
	  /// Element type of type "http://france-life-imaging.fr/api":ParameterType.
	      enum api__ParameterType              type                           1;	///< Required element.
	  /// CHOICE OF ELEMENTS <xs:choice>
	    $int                                  __union_ParameterTypedValue   ;	///< Union _api__union_ParameterTypedValue selector: set to SOAP_UNION__api__union_ParameterTypedValue_<fieldname>
	      union _api__union_ParameterTypedValue
	      {
	  /// Element valueStr of type xs:string.
	      std::string*                         valueStr                       1;	///< Required element.
	  /// Element valueBool of type xs:boolean.
	      bool                                 valueBool                      1;	///< Required element.
	  /// Element valueInt of type xs:int.
	      int                                  valueInt                       1;	///< Required element.
	  /// Element valueDouble of type xs:double.
	      double                               valueDouble                    1;	///< Required element.
	      }                                    union_ParameterTypedValue     ;
	  //  END OF CHOICE
	  /// A handle to the soap struct that manages this instance (automatically set)
	      struct soap                         *soap                          ;
	  };
       */
      
      if(api__StringKeyParameterValuePair->value->__union_ParameterTypedValue==1)
      {
	std::string std_param_val = *api__StringKeyParameterValuePair->value->union_ParameterTypedValue.valueStr;
	param_val.SetString(std_param_val.c_str(), std_param_val.length(), document.GetAllocator());
	if(m_pconfig->VERBOSE)
	{
	  std::cout << param_val.GetString() << std::endl;
	}
      }else if(api__StringKeyParameterValuePair->value->__union_ParameterTypedValue==2)
      {
	param_val.SetBool(api__StringKeyParameterValuePair->value->union_ParameterTypedValue.valueBool);
	if(m_pconfig->VERBOSE)
	{
	  std::cout << param_val.GetBool() << std::endl;
	}
      }else if(api__StringKeyParameterValuePair->value->__union_ParameterTypedValue==3)
      {
	param_val.SetInt(api__StringKeyParameterValuePair->value->union_ParameterTypedValue.valueInt);
	if(m_pconfig->VERBOSE)
	{
	  std::cout << param_val.GetInt() << std::endl;
	}
      }else if(api__StringKeyParameterValuePair->value->__union_ParameterTypedValue==4)
      { // string
	param_val.SetDouble(api__StringKeyParameterValuePair->value->union_ParameterTypedValue.valueDouble);
	if(m_pconfig->VERBOSE)
	{
	  std::cout << param_val.GetDouble() << std::endl;
	}
      }else{
	char buf_error_message[1024];
	snprintf(buf_error_message,
		 sizeof(buf_error_message),
		 "parameter %s api__StringKeyParameterValuePair->value->__union_ParameterTypedValue==%d is not supported.",
		 std_param_name.c_str(),
		 api__StringKeyParameterValuePair->value->__union_ParameterTypedValue
		);
	m_error_message = buf_error_message;
	return false;
      }

      params.AddMember(
	param_key,
	param_val,
	document.GetAllocator()
      );
    }

    document.AddMember(params_str,
		       params,
		       document.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    if(m_pconfig->VERBOSE)
    {
      std::cout << buffer.GetString() << std::endl;
      std::cout << buffer.GetSize() << std::endl;
    }

    //char json_buffer[] = "{\"pipeline_name\": \"ProcessDemo\", \"params\":{\"ff\": 14.0}}";
    //std::cout << json_buffer << std::endl;

    if(!CurlProcess::request(soap, routeBuf, "post",
			     "pipeline",
			     buffer.GetString(),
			     buffer.GetSize()))
    {
      return false;
    }

    if(!CurlProcess::parse_reponse_by_json())
    {
      return false;
    }

    if(m_pconfig->VERBOSE)
    {
      std::cout << "success parse_reponse_by_json" << std::endl;
    }

    if(this->m_api__Execution==NULL)
    {
      this->m_api__Execution = soap_new_api__Execution(soap, 1);
    }
    
    char buf_identifier[256];
    snprintf(buf_identifier, sizeof(buf_identifier), "%d", m_document["id"].GetInt());
    
    this->m_api__Execution->identifier = buf_identifier;
    this->m_api__Execution->name = pipelineId;
    this->m_api__Execution->pipelineIdentifier = pipelineId;
    
    //this->m_api__Execution->status = soap_new_std__string(soap, 1);
    //(*this->m_api__Execution->status) = api__ExecutionStatus__Running;
    this->m_api__Execution->status = api__ExecutionStatus__Running;

    return true;
}