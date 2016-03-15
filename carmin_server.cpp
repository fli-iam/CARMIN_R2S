#include "soapH.h"
#include "CarminSoapBinding.nsmap"
#include "CatiwebAuth.h"
#include <curl/curl.h>
#include <string>
#include <vector>
#include "CookieData.h"
#include "Config.h"
#include "Pipelines.h"
#include "Pipeline.h"
#include "Execution.h"
#include "CatiwebLogout.h"
#include "GlobalProperties.h"

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"



int main(int argc, char **argv) 
{ 
   /*
   rapidjson::Document document;
   document.SetObject();
   rapidjson::Value key1("key1");
   rapidjson::Value value1("value1");
   document.AddMember(key1, value1,
		      document.GetAllocator());
   rapidjson::StringBuffer buffer;
   rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
   document.Accept(writer);
  
   std::cout << buffer.GetString() << std::endl;
   std::cout << buffer.GetSize() << std::endl;
   */
   
   struct soap soap; 
   int m, s; 
   soap_init(&soap); 
   soap.cookie_domain = ".."; 
   soap.cookie_path = "/"; // the path which is used to filter/set cookies with this destination 
   if (argc < 2)
   { 
      soap_getenv_cookies(&soap); // CGI app: grab cookies from 'HTTP_COOKIE' env var 
      soap_serve(&soap); 
   }
   else
   { 
      m = soap_bind(&soap, NULL, atoi(argv[1]), 100); 
      if (m < 0) 
         exit(1); 
      for (int i = 1; ; i++) 
      {
         s = soap_accept(&soap); 
         if (s < 0) 
            exit(1); 
         soap_serve(&soap); 
         soap_end(&soap);	// clean up 
         soap_free_cookies(&soap);	// remove all old cookies from database so no interference occurs with the arrival of new cookies 
      }
   }
   return 0; 
} 

/*
int main( int argc, char **argv )
{ 
	SOAP_SOCKET m, s; // master and slave sockets 
	struct soap soap; // structure soap 

	soap_init(&soap); // initialisation du serveur

	m = soap_bind( &soap, NULL, 8080, 100); // liaison avec le master socket 
	if ( !soap_valid_socket( m ) )
	{ 
		std::cout << "connection error" << std::endl << " click any button to continue";
		std::cin.get();
		return 1;
	}

	// boucle principale
	std::cout << "successful connection = " << m << std::endl;
	for ( ; ; )
	{
		s = soap_accept( &soap );
		std::cout << "client connection ok: slave socket = " << s << std::endl;
		if ( !soap_valid_socket( s ) )
		{ 
			std::cout << "error: socket problem" << std::endl << "click any button to quit";
			return 1;
		} 
		soap_serve( &soap );
		soap_end( &soap );
	}

	return 0;
} */



int __api__authenticateSession(
  struct soap *soap,
  // request parameters:
  api__authenticateSession*           api__authenticateSession_,
  // response parameters:
  api__authenticateSessionResponse*   api__authenticateSessionResponse_
)
{
  /*
  char *name = new char[100];
  char *value = new char[100];
  char *domain = new char[100];
  char *path = new char[100];
 
  strcpy(name, "hello_world_name");
  strcpy(value, "hello_world_value");
  strcpy(domain, "hello_world_domain");
  strcpy(path, "hello_world_path");
  
  struct soap_cookie * ret_cookies = soap_set_cookie(soap, name, value, domain, path);*/

  /*
  int n; 
  const char *s; 

  s = soap_cookie_value(soap, "demo", NULL, NULL); // cookie returned by client? 

  if (!s) 
    s = "init-value"; // no: set initial cookie value 

  soap_set_cookie(soap, "demo", s, NULL, NULL); 
  soap_set_cookie_expire(soap, "demo", 5, NULL, NULL); // cookie may expire at client-side in 5 seconds 
  */

  //std::cout << "username:" << api__authenticateSession_->userName << std::endl;
  //std::cout << "password:" << api__authenticateSession_->password << std::endl;
  
  PCURL_SLIST cookies_data_list = NULL;
  CatiwebAuth catiwebAuth = CatiwebAuth();

  bool is_success = catiwebAuth.Login(
      api__authenticateSession_->userName.c_str(),
      api__authenticateSession_->password.c_str(),
      cookies_data_list
  );

  api__authenticateSessionResponse_->return_ = soap_new_api__Response(soap, 1);

  if (is_success){
    CookieData cookieData = CookieData();
    // convert cookies_data_list to gsoap
    cookieData.set_curl_cookie_to_gsoap_cookie(soap, cookies_data_list);
    curl_slist_free_all(cookies_data_list);
    //std::cout << "auth success" << std::endl;
    //std::cout << "api__authenticateSessionResponse_=" << api__authenticateSessionResponse_ << std::endl;
    api__authenticateSessionResponse_->return_->statusCode = 0;
  }else{
    //std::cout << "auth fail" << std::endl;
    api__authenticateSessionResponse_->return_->statusCode = 1;
    api__authenticateSessionResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    (*api__authenticateSessionResponse_->return_->errorMessage) = catiwebAuth.m_error_message;
  }

  //std::cout << "http_version:" << soap->http_version << std::endl;
  //std::cout << "__api__authenticateSession" << std::endl;

  return SOAP_OK;
}


int __api__listPipelines(
  struct soap *soap,
  // request parameters:
  api__listPipelines*                 api__listPipelines_,
  // response parameters:
  api__listPipelinesResponse*         api__listPipelinesResponse_
){
  api__listPipelinesResponse_->return_ = soap_new_api__Response(soap, 1);

  Pipelines pipelines = Pipelines();
  if(pipelines.request(soap, api__listPipelines_->studyIdentifier.c_str()))
  {
    api__listPipelinesResponse_->return_->statusCode = 0;
    api__listPipelinesResponse_->return_->__union_Response = 7;
    api__listPipelinesResponse_->return_->union_Response.returnedValueListPipelines = pipelines.m_pPipelines;
  }else{
    api__listPipelinesResponse_->return_->statusCode = 1;
    api__listPipelinesResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    (*api__listPipelinesResponse_->return_->errorMessage) = pipelines.m_error_message;
  }
  return SOAP_OK;
}


int __api__getPipeline(
  struct soap *soap,
  // request parameters:
  api__getPipeline*                   api__getPipeline_,
  // response parameters:
  api__getPipelineResponse*           api__getPipelineResponse_
)
{
  api__getPipelineResponse_->return_ = soap_new_api__Response(soap, 1);
  
  std::string pipelineId = api__getPipeline_->pipelineId;
  Pipeline pipeline = Pipeline();
  bool is_success = pipeline.request(soap, pipelineId.c_str());
  
  if(is_success)
  {
    api__getPipelineResponse_->return_->statusCode = 0;
    api__getPipelineResponse_->return_->__union_Response = 1;
    api__getPipelineResponse_->return_->union_Response.returnedValuePipeline = pipeline.m_pPipeline;
  }else{
    api__getPipelineResponse_->return_->statusCode = 1;
    api__getPipelineResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    (*api__getPipelineResponse_->return_->errorMessage) = pipeline.m_error_message;
  }

  return SOAP_OK;
}

int __api__initExecution(
  struct soap *soap,
  // request parameters:
  api__initExecution*                 api__initExecution_,
  // response parameters:
  api__initExecutionResponse*         api__initExecutionResponse_
){
  api__initExecutionResponse_->return_ = soap_new_api__Response(soap, 1);
  
  Execution execution = Execution();
  bool is_success = execution.initExecution(soap, api__initExecution_);

  if(is_success)
  {
    api__initExecutionResponse_->return_->statusCode = 0;
    // if you set __union_Response, please set the real reponse value as well. Otherwise it will produce 
    api__initExecutionResponse_->return_->__union_Response = 2;
    api__initExecutionResponse_->return_->union_Response.returnedValueExecution = execution.m_api__Execution;
  }else{
    api__initExecutionResponse_->return_->statusCode = 1;
    api__initExecutionResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    (*api__initExecutionResponse_->return_->errorMessage) = execution.m_error_message;
  }

  return SOAP_OK;
}

int __api__deleteExecution(
  struct soap *soap,
  // request parameters:
  api__deleteExecution*               api__deleteExecution_,
  // response parameters:
  api__deleteExecutionResponse*       api__deleteExecutionResponse_
)
{
  api__deleteExecutionResponse_->return_ = soap_new_api__Response(soap, 1);

  Execution execution = Execution();
  bool is_success = execution.deleteExecution(soap, api__deleteExecution_);

  if(is_success)
  {
    api__deleteExecutionResponse_->return_->statusCode = 0;
    // if you set __union_Response, please set the real reponse value as well. Otherwise it will produce 
    //api__deleteExecutionResponse_->return_->__union_Response = 2;
    //api__deleteExecutionResponse_->return_->union_Response.returnedValueExecution = execution.m_api__Execution;
  }else{
    api__deleteExecutionResponse_->return_->statusCode = 1;
    api__deleteExecutionResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    (*api__deleteExecutionResponse_->return_->errorMessage) = execution.m_error_message;
  }

  return SOAP_OK;
}

int __api__listExecutions(
  struct soap *soap,
  // request parameters:
  api__listExecutions*                api__listExecutions_,
  // response parameters:
  api__listExecutionsResponse*        api__listExecutionsResponse_
){
  api__listExecutionsResponse_->return_ = soap_new_api__Response(soap, 1);
 
  Execution execution = Execution();
  bool is_success = execution.listExecutions(soap, api__listExecutions_);

  if(is_success)
  {
    api__listExecutionsResponse_->return_->statusCode = 0;
    // if you set __union_Response, please set the real reponse value as well. Otherwise it will produce 
    api__listExecutionsResponse_->return_->__union_Response = 6;
    api__listExecutionsResponse_->return_->union_Response.returnedValueListExecutions = execution.m_returnedValueListExecutions;
  }else{
    api__listExecutionsResponse_->return_->statusCode = 1;
    api__listExecutionsResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    (*api__listExecutionsResponse_->return_->errorMessage) = execution.m_error_message;
  }

  return SOAP_OK;
}

int __api__getExecution(
  struct soap *soap,
  // request parameters:
  api__getExecution*                  api__getExecution_,
  // response parameters:
  api__getExecutionResponse*          api__getExecutionResponse_
){
  api__getExecutionResponse_->return_ = soap_new_api__Response(soap, 1);
 
  Execution execution = Execution();
  bool is_success = execution.getExecution(soap, api__getExecution_->executionId);

  if(is_success)
  {
    api__getExecutionResponse_->return_->statusCode = 0;
    // if you set __union_Response, please set the real reponse value as well. Otherwise it will produce 
    api__getExecutionResponse_->return_->__union_Response = 2;
    api__getExecutionResponse_->return_->union_Response.returnedValueExecution = execution.m_api__Execution;
  }else{
    api__getExecutionResponse_->return_->statusCode = 1;
    api__getExecutionResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    (*api__getExecutionResponse_->return_->errorMessage) = execution.m_error_message;
  }

  return SOAP_OK;
}

int __api__getGlobalProperties(
  struct soap *soap,
  // request parameters:
  api__getGlobalProperties*           api__getGlobalProperties_,
  // response parameters:
  api__getGlobalPropertiesResponse*   api__getGlobalPropertiesResponse_
){
  api__getGlobalPropertiesResponse_->return_ = soap_new_api__Response(soap, 1);
 
  GlobalProperties globalProperties = GlobalProperties();
  bool is_success = globalProperties.request(soap);

  if(is_success)
  {
    api__getGlobalPropertiesResponse_->return_->statusCode = 0;
    // if you set __union_Response, please set the real reponse value as well. Otherwise it will produce 
    api__getGlobalPropertiesResponse_->return_->__union_Response = 3;
    api__getGlobalPropertiesResponse_->return_->union_Response.returnedValueGlobalProp = globalProperties.m_api__GlobalProperties;
  }else{
    api__getGlobalPropertiesResponse_->return_->statusCode = 1;
    api__getGlobalPropertiesResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    (*api__getGlobalPropertiesResponse_->return_->errorMessage) = globalProperties.m_error_message;
  }

  return SOAP_OK;
}

int __api__killExecution(
  struct soap *soap,
  // request parameters:
  api__killExecution*                 api__killExecution_,
  // response parameters:
  api__killExecutionResponse*         api__killExecutionResponse_
){
  api__killExecutionResponse_->return_ = soap_new_api__Response(soap, 1);
 
  Execution execution = Execution();
  bool is_success = execution.killExecution(soap, api__killExecution_->executionId);

  if(is_success)
  {
    api__killExecutionResponse_->return_->statusCode = 0;
    // if you set __union_Response, please set the real reponse value as well. Otherwise it will produce 
    //api__killExecutionResponse_->return_->__union_Response = 2;
    //api__killExecutionResponse_->return_->union_Response.returnedValueExecution = execution.m_api__Execution;
  }else{
    api__killExecutionResponse_->return_->statusCode = 1;
    api__killExecutionResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    (*api__killExecutionResponse_->return_->errorMessage) = execution.m_error_message;
  }

  return SOAP_OK;
}


int __api__playExecution(
  struct soap *soap,
  // request parameters:
  api__playExecution*                 api__playExecution_,
  // response parameters:
  api__playExecutionResponse*         api__playExecutionResponse_
){
  api__playExecutionResponse_->return_ = soap_new_api__Response(soap, 1);
  Execution execution = Execution();
  bool is_success = execution.playExecution(soap, api__playExecution_->executionId);

  if(is_success)
  {
    api__playExecutionResponse_->return_->statusCode = 0;
    // if you set __union_Response, please set the real reponse value as well. Otherwise it will produce 
    //api__killExecutionResponse_->return_->__union_Response = 2;
    //api__killExecutionResponse_->return_->union_Response.returnedValueExecution = execution.m_api__Execution;
  }else{
    api__playExecutionResponse_->return_->statusCode = 1;
    api__playExecutionResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    (*api__playExecutionResponse_->return_->errorMessage) = execution.m_error_message;
  }

  return SOAP_OK;
}


int __api__getStdOut(
  struct soap *soap,
  // request parameters:
  api__getStdOut*                     api__getStdOut_,
  // response parameters:
  api__getStdOutResponse*             api__getStdOutResponse_
){
  api__getStdOutResponse_->return_ = soap_new_api__Response(soap, 1);
 
  Execution execution = Execution();
  bool is_success = execution.getStdOutErr(soap, api__getStdOut_->executionId);

  if(is_success)
  {
    api__getStdOutResponse_->return_->statusCode = 0;
    // if you set __union_Response, please set the real reponse value as well. Otherwise it will produce 
    api__getStdOutResponse_->return_->__union_Response = 4;
    api__getStdOutResponse_->return_->union_Response.returnedValueStr = execution.m_std_out;
  }else{
    api__getStdOutResponse_->return_->statusCode = 1;
    api__getStdOutResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    (*api__getStdOutResponse_->return_->errorMessage) = execution.m_error_message;
  }

  return SOAP_OK;
}

int __api__getStdErr(
  struct soap *soap,
  // request parameters:
  api__getStdErr*                     api__getStdErr_,
  // response parameters:
  api__getStdErrResponse*             api__getStdErrResponse_
){
  api__getStdErrResponse_->return_ = soap_new_api__Response(soap, 1);
 
  Execution execution = Execution();
  bool is_success = execution.getStdOutErr(soap, api__getStdErr_->executionId);

  if(is_success)
  {
    api__getStdErrResponse_->return_->statusCode = 0;
    // if you set __union_Response, please set the real reponse value as well. Otherwise it will produce 
    api__getStdErrResponse_->return_->__union_Response = 4;
    api__getStdErrResponse_->return_->union_Response.returnedValueStr = execution.m_std_err;
  }else{
    api__getStdErrResponse_->return_->statusCode = 1;
    api__getStdErrResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    (*api__getStdErrResponse_->return_->errorMessage) = execution.m_error_message;
  }

  return SOAP_OK;
}





/* not supported web service.........
 
 How to get pipeline results from a workflow of soma-workflow...
 
 */
int __api__getExecutionResults(
  struct soap *soap,
  // request parameters:
  api__getExecutionResults*           api__getExecutionResults_,
  // response parameters:
  api__getExecutionResultsResponse*   api__getExecutionResultsResponse_
){
  std::cout << "__api__getExecutionResults" << std::endl;
  return SOAP_FATAL_ERROR;
}

/* not supported web service.........*/
int __api__authenticateHTTP(
  struct soap *soap,
  // request parameters:
  api__authenticateHTTP*              api__authenticateHTTP_,
  // response parameters:
  api__authenticateHTTPResponse*      api__authenticateHTTPResponse_
){

  api__authenticateHTTPResponse_->return_ = soap_new_api__Response(soap, 1);
  api__authenticateHTTPResponse_->return_->statusCode = 1;
  api__authenticateHTTPResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
  (*api__authenticateHTTPResponse_->return_->errorMessage) = "not supported";

  return SOAP_FATAL_ERROR;
}


/* not supported web service.........*/
int __api__logout(
  struct soap *soap,
  // request parameters:
  api__logout*                        api__logout_,
  // response parameters:
  api__logoutResponse*                api__logoutResponse_
){

  std::cout << "__api__logout cannot be implemented with catidb" << std::endl;
  return SOAP_FATAL_ERROR;

  /*
  api__logoutResponse_->return_ = soap_new_api__Response(soap, 1);
 
  CatiwebLogout catiwebLogout = CatiwebLogout();
  bool is_success = catiwebLogout.request(soap);

  if(is_success)
  {
    api__logoutResponse_->return_->statusCode = 0;
    // if you set __union_Response, please set the real reponse value as well. Otherwise it will produce 
    //api__logoutResponse_->return_->__union_Response = 6;
    //api__logoutResponse_->return_->union_Response.returnedValueListExecutions = execution.m_returnedValueListExecutions;
  }else{
    api__logoutResponse_->return_->statusCode = 1;
    api__logoutResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    (*api__logoutResponse_->return_->errorMessage) = catiwebLogout.m_error_message;
  }

  return SOAP_OK;*/
}

/*not supported web service.........*/
int __api__updateExecution(
  struct soap *soap,
  // request parameters:
  api__updateExecution*               api__updateExecution_,
  // response parameters:
  api__updateExecutionResponse*       api__updateExecutionResponse_
){
  api__updateExecutionResponse_->return_ = soap_new_api__Response(soap, 1);
 
  Execution execution = Execution();
  
  bool is_success = false;
  
  bool type_error = false;
  std::string type_error_message = "For the moment, only strint value can be accepted.";
  
  if(api__updateExecution_->keyValuePair[0]->__union_StringKeyValuePair != 1)
  {
    type_error = true;
  }
  
  is_success = execution.updateExecution(soap, api__updateExecution_->executionId,
    api__updateExecution_->keyValuePair[0]->name ,
    *(api__updateExecution_->keyValuePair[0]->union_StringKeyValuePair.valueStr)
  );

  if(is_success and (!type_error))
  {
    api__updateExecutionResponse_->return_->statusCode = 0;
    // if you set __union_Response, please set the real reponse value as well. Otherwise it will produce 
    //api__killExecutionResponse_->return_->__union_Response = 2;
    //api__killExecutionResponse_->return_->union_Response.returnedValueExecution = execution.m_api__Execution;
  }else{
    api__updateExecutionResponse_->return_->statusCode = 1;
    
    api__updateExecutionResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    
    if(type_error)
    {
      (*api__updateExecutionResponse_->return_->errorMessage) = type_error_message;
    }else{
      
      (*api__updateExecutionResponse_->return_->errorMessage) = execution.m_error_message;
    }
  }

  return SOAP_OK;
  
  std::cout << "__api__updateExecution" << std::endl;
  return SOAP_FATAL_ERROR;
}

