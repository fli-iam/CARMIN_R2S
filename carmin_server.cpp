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

int __ns1__authenticateHTTP(
  struct soap *soap,
  // request parameters:
  ns1__authenticateHTTP*              ns1__authenticateHTTP_,
  // response parameters:
  ns1__authenticateHTTPResponse*      ns1__authenticateHTTPResponse_
){

  ns1__authenticateHTTPResponse_->return_ = soap_new_ns1__Response(soap, 1);
  ns1__authenticateHTTPResponse_->return_->statusCode = 1;
  ns1__authenticateHTTPResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
  (*ns1__authenticateHTTPResponse_->return_->errorMessage) = "not supported";

  return SOAP_FATAL_ERROR;
}

int __ns1__authenticateSession(
  struct soap *soap,
  // request parameters:
  ns1__authenticateSession*           ns1__authenticateSession_,
  // response parameters:
  ns1__authenticateSessionResponse*   ns1__authenticateSessionResponse_
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

  //std::cout << "username:" << ns1__authenticateSession_->userName << std::endl;
  //std::cout << "password:" << ns1__authenticateSession_->password << std::endl;
  
  PCURL_SLIST cookies_data_list = NULL;
  CatiwebAuth catiwebAuth = CatiwebAuth();

  bool is_success = catiwebAuth.Login(
      ns1__authenticateSession_->userName.c_str(),
      ns1__authenticateSession_->password.c_str(),
      cookies_data_list
  );

  ns1__authenticateSessionResponse_->return_ = soap_new_ns1__Response(soap, 1);

  if (is_success){
    CookieData cookieData = CookieData();
    // convert cookies_data_list to gsoap
    cookieData.set_curl_cookie_to_gsoap_cookie(soap, cookies_data_list);
    curl_slist_free_all(cookies_data_list);
    //std::cout << "auth success" << std::endl;
    //std::cout << "ns1__authenticateSessionResponse_=" << ns1__authenticateSessionResponse_ << std::endl;
    ns1__authenticateSessionResponse_->return_->statusCode = 0;
  }else{
    //std::cout << "auth fail" << std::endl;
    ns1__authenticateSessionResponse_->return_->statusCode = 1;
    ns1__authenticateSessionResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    (*ns1__authenticateSessionResponse_->return_->errorMessage) = catiwebAuth.m_error_message;
  }

  //std::cout << "http_version:" << soap->http_version << std::endl;
  //std::cout << "__ns1__authenticateSession" << std::endl;

  return SOAP_OK;
}


int __ns1__listPipelines(
  struct soap *soap,
  // request parameters:
  ns1__listPipelines*                 ns1__listPipelines_,
  // response parameters:
  ns1__listPipelinesResponse*         ns1__listPipelinesResponse_
){
  ns1__listPipelinesResponse_->return_ = soap_new_ns1__Response(soap, 1);

  Pipelines pipelines = Pipelines();
  if(pipelines.request(soap, ns1__listPipelines_->studyIdentifier.c_str()))
  {
    ns1__listPipelinesResponse_->return_->statusCode = 0;
    ns1__listPipelinesResponse_->return_->__union_Response = 7;
    ns1__listPipelinesResponse_->return_->union_Response.returnedValueListPipelines = pipelines.m_pPipelines;
  }else{
    ns1__listPipelinesResponse_->return_->statusCode = 1;
    ns1__listPipelinesResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    (*ns1__listPipelinesResponse_->return_->errorMessage) = pipelines.m_error_message;
  }
  return SOAP_OK;
}


int __ns1__getPipeline(
  struct soap *soap,
  // request parameters:
  ns1__getPipeline*                   ns1__getPipeline_,
  // response parameters:
  ns1__getPipelineResponse*           ns1__getPipelineResponse_
)
{
  ns1__getPipelineResponse_->return_ = soap_new_ns1__Response(soap, 1);
  
  std::string pipelineId = ns1__getPipeline_->pipelineId;
  Pipeline pipeline = Pipeline();
  bool is_success = pipeline.request(soap, pipelineId.c_str());
  
  if(is_success)
  {
    ns1__getPipelineResponse_->return_->statusCode = 0;
    ns1__getPipelineResponse_->return_->__union_Response = 1;
    ns1__getPipelineResponse_->return_->union_Response.returnedValuePipeline = pipeline.m_pPipeline;
  }else{
    ns1__getPipelineResponse_->return_->statusCode = 1;
    ns1__getPipelineResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    (*ns1__getPipelineResponse_->return_->errorMessage) = pipeline.m_error_message;
  }

  return SOAP_OK;
}

int __ns1__initExecution(
  struct soap *soap,
  // request parameters:
  ns1__initExecution*                 ns1__initExecution_,
  // response parameters:
  ns1__initExecutionResponse*         ns1__initExecutionResponse_
){
  ns1__initExecutionResponse_->return_ = soap_new_ns1__Response(soap, 1);
  
  Execution execution = Execution();
  bool is_success = execution.initExecution(soap, ns1__initExecution_);

  if(is_success)
  {
    ns1__initExecutionResponse_->return_->statusCode = 0;
    // if you set __union_Response, please set the real reponse value as well. Otherwise it will produce 
    ns1__initExecutionResponse_->return_->__union_Response = 2;
    ns1__initExecutionResponse_->return_->union_Response.returnedValueExecution = execution.m_ns1__Execution;
  }else{
    ns1__initExecutionResponse_->return_->statusCode = 1;
    ns1__initExecutionResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    (*ns1__initExecutionResponse_->return_->errorMessage) = execution.m_error_message;
  }

  return SOAP_OK;
}

int __ns1__deleteExecution(
  struct soap *soap,
  // request parameters:
  ns1__deleteExecution*               ns1__deleteExecution_,
  // response parameters:
  ns1__deleteExecutionResponse*       ns1__deleteExecutionResponse_
)
{
  ns1__deleteExecutionResponse_->return_ = soap_new_ns1__Response(soap, 1);

  Execution execution = Execution();
  bool is_success = execution.deleteExecution(soap, ns1__deleteExecution_);

  if(is_success)
  {
    ns1__deleteExecutionResponse_->return_->statusCode = 0;
    // if you set __union_Response, please set the real reponse value as well. Otherwise it will produce 
    //ns1__deleteExecutionResponse_->return_->__union_Response = 2;
    //ns1__deleteExecutionResponse_->return_->union_Response.returnedValueExecution = execution.m_ns1__Execution;
  }else{
    ns1__deleteExecutionResponse_->return_->statusCode = 1;
    ns1__deleteExecutionResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    (*ns1__deleteExecutionResponse_->return_->errorMessage) = execution.m_error_message;
  }

  return SOAP_OK;
}

int __ns1__listExecutions(
  struct soap *soap,
  // request parameters:
  ns1__listExecutions*                ns1__listExecutions_,
  // response parameters:
  ns1__listExecutionsResponse*        ns1__listExecutionsResponse_
){
  ns1__listExecutionsResponse_->return_ = soap_new_ns1__Response(soap, 1);
 
  Execution execution = Execution();
  bool is_success = execution.listExecutions(soap, ns1__listExecutions_);

  if(is_success)
  {
    ns1__listExecutionsResponse_->return_->statusCode = 0;
    // if you set __union_Response, please set the real reponse value as well. Otherwise it will produce 
    ns1__listExecutionsResponse_->return_->__union_Response = 6;
    ns1__listExecutionsResponse_->return_->union_Response.returnedValueListExecutions = execution.m_returnedValueListExecutions;
  }else{
    ns1__listExecutionsResponse_->return_->statusCode = 1;
    ns1__listExecutionsResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    (*ns1__listExecutionsResponse_->return_->errorMessage) = execution.m_error_message;
  }

  return SOAP_OK;
}

int __ns1__getExecution(
  struct soap *soap,
  // request parameters:
  ns1__getExecution*                  ns1__getExecution_,
  // response parameters:
  ns1__getExecutionResponse*          ns1__getExecutionResponse_
){
  ns1__getExecutionResponse_->return_ = soap_new_ns1__Response(soap, 1);
 
  Execution execution = Execution();
  bool is_success = execution.getExecution(soap, ns1__getExecution_->executionId);

  if(is_success)
  {
    ns1__getExecutionResponse_->return_->statusCode = 0;
    // if you set __union_Response, please set the real reponse value as well. Otherwise it will produce 
    ns1__getExecutionResponse_->return_->__union_Response = 2;
    ns1__getExecutionResponse_->return_->union_Response.returnedValueExecution = execution.m_ns1__Execution;
  }else{
    ns1__getExecutionResponse_->return_->statusCode = 1;
    ns1__getExecutionResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    (*ns1__getExecutionResponse_->return_->errorMessage) = execution.m_error_message;
  }

  return SOAP_OK;
}

int __ns1__getGlobalProperties(
  struct soap *soap,
  // request parameters:
  ns1__getGlobalProperties*           ns1__getGlobalProperties_,
  // response parameters:
  ns1__getGlobalPropertiesResponse*   ns1__getGlobalPropertiesResponse_
){
  ns1__getGlobalPropertiesResponse_->return_ = soap_new_ns1__Response(soap, 1);
 
  GlobalProperties globalProperties = GlobalProperties();
  bool is_success = globalProperties.request(soap);

  if(is_success)
  {
    ns1__getGlobalPropertiesResponse_->return_->statusCode = 0;
    // if you set __union_Response, please set the real reponse value as well. Otherwise it will produce 
    ns1__getGlobalPropertiesResponse_->return_->__union_Response = 3;
    ns1__getGlobalPropertiesResponse_->return_->union_Response.returnedValueGlobalProp = globalProperties.m_ns1__GlobalProperties;
  }else{
    ns1__getGlobalPropertiesResponse_->return_->statusCode = 1;
    ns1__getGlobalPropertiesResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    (*ns1__getGlobalPropertiesResponse_->return_->errorMessage) = globalProperties.m_error_message;
  }

  return SOAP_OK;
}

int __ns1__killExecution(
  struct soap *soap,
  // request parameters:
  ns1__killExecution*                 ns1__killExecution_,
  // response parameters:
  ns1__killExecutionResponse*         ns1__killExecutionResponse_
){
  ns1__killExecutionResponse_->return_ = soap_new_ns1__Response(soap, 1);
 
  Execution execution = Execution();
  bool is_success = execution.killExecution(soap, ns1__killExecution_->executionId);

  if(is_success)
  {
    ns1__killExecutionResponse_->return_->statusCode = 0;
    // if you set __union_Response, please set the real reponse value as well. Otherwise it will produce 
    //ns1__killExecutionResponse_->return_->__union_Response = 2;
    //ns1__killExecutionResponse_->return_->union_Response.returnedValueExecution = execution.m_ns1__Execution;
  }else{
    ns1__killExecutionResponse_->return_->statusCode = 1;
    ns1__killExecutionResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    (*ns1__killExecutionResponse_->return_->errorMessage) = execution.m_error_message;
  }

  return SOAP_OK;
}


int __ns1__playExecution(
  struct soap *soap,
  // request parameters:
  ns1__playExecution*                 ns1__playExecution_,
  // response parameters:
  ns1__playExecutionResponse*         ns1__playExecutionResponse_
){
  ns1__playExecutionResponse_->return_ = soap_new_ns1__Response(soap, 1);
 
  Execution execution = Execution();
  bool is_success = execution.playExecution(soap, ns1__playExecution_->executionId);

  if(is_success)
  {
    ns1__playExecutionResponse_->return_->statusCode = 0;
    // if you set __union_Response, please set the real reponse value as well. Otherwise it will produce 
    //ns1__killExecutionResponse_->return_->__union_Response = 2;
    //ns1__killExecutionResponse_->return_->union_Response.returnedValueExecution = execution.m_ns1__Execution;
  }else{
    ns1__playExecutionResponse_->return_->statusCode = 1;
    ns1__playExecutionResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    (*ns1__playExecutionResponse_->return_->errorMessage) = execution.m_error_message;
  }

  return SOAP_OK;
}


int __ns1__getStdOut(
  struct soap *soap,
  // request parameters:
  ns1__getStdOut*                     ns1__getStdOut_,
  // response parameters:
  ns1__getStdOutResponse*             ns1__getStdOutResponse_
){
  ns1__getStdOutResponse_->return_ = soap_new_ns1__Response(soap, 1);
 
  Execution execution = Execution();
  bool is_success = execution.getStdOutErr(soap, ns1__getStdOut_->executionId);

  if(is_success)
  {
    ns1__getStdOutResponse_->return_->statusCode = 0;
    // if you set __union_Response, please set the real reponse value as well. Otherwise it will produce 
    ns1__getStdOutResponse_->return_->__union_Response = 4;
    ns1__getStdOutResponse_->return_->union_Response.returnedValueStr = execution.m_std_out;
  }else{
    ns1__getStdOutResponse_->return_->statusCode = 1;
    ns1__getStdOutResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    (*ns1__getStdOutResponse_->return_->errorMessage) = execution.m_error_message;
  }

  return SOAP_OK;
}

int __ns1__getStdErr(
  struct soap *soap,
  // request parameters:
  ns1__getStdErr*                     ns1__getStdErr_,
  // response parameters:
  ns1__getStdErrResponse*             ns1__getStdErrResponse_
){
  ns1__getStdErrResponse_->return_ = soap_new_ns1__Response(soap, 1);
 
  Execution execution = Execution();
  bool is_success = execution.getStdOutErr(soap, ns1__getStdErr_->executionId);

  if(is_success)
  {
    ns1__getStdErrResponse_->return_->statusCode = 0;
    // if you set __union_Response, please set the real reponse value as well. Otherwise it will produce 
    ns1__getStdErrResponse_->return_->__union_Response = 4;
    ns1__getStdErrResponse_->return_->union_Response.returnedValueStr = execution.m_std_err;
  }else{
    ns1__getStdErrResponse_->return_->statusCode = 1;
    ns1__getStdErrResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    (*ns1__getStdErrResponse_->return_->errorMessage) = execution.m_error_message;
  }

  return SOAP_OK;
}





/* not supported web service.........
 
 How to get pipeline results from a workflow of soma-workflow...
 
 */
int __ns1__getExecutionResults(
  struct soap *soap,
  // request parameters:
  ns1__getExecutionResults*           ns1__getExecutionResults_,
  // response parameters:
  ns1__getExecutionResultsResponse*   ns1__getExecutionResultsResponse_
){
  std::cout << "__ns1__getExecutionResults" << std::endl;
  return SOAP_FATAL_ERROR;
}


/* not supported web service.........*/
int __ns1__logout(
  struct soap *soap,
  // request parameters:
  ns1__logout*                        ns1__logout_,
  // response parameters:
  ns1__logoutResponse*                ns1__logoutResponse_
){

  std::cout << "__ns1__logout cannot be implemented with catidb" << std::endl;
  return SOAP_FATAL_ERROR;

  /*
  ns1__logoutResponse_->return_ = soap_new_ns1__Response(soap, 1);
 
  CatiwebLogout catiwebLogout = CatiwebLogout();
  bool is_success = catiwebLogout.request(soap);

  if(is_success)
  {
    ns1__logoutResponse_->return_->statusCode = 0;
    // if you set __union_Response, please set the real reponse value as well. Otherwise it will produce 
    //ns1__logoutResponse_->return_->__union_Response = 6;
    //ns1__logoutResponse_->return_->union_Response.returnedValueListExecutions = execution.m_returnedValueListExecutions;
  }else{
    ns1__logoutResponse_->return_->statusCode = 1;
    ns1__logoutResponse_->return_->errorMessage = (soap_new_std__string(soap, 1));
    (*ns1__logoutResponse_->return_->errorMessage) = catiwebLogout.m_error_message;
  }

  return SOAP_OK;*/
}

/*not supported web service.........*/
int __ns1__updateExecution(
  struct soap *soap,
  // request parameters:
  ns1__updateExecution*               ns1__updateExecution_,
  // response parameters:
  ns1__updateExecutionResponse*       ns1__updateExecutionResponse_
){
  std::cout << "__ns1__updateExecution" << std::endl;
  return SOAP_FATAL_ERROR;
}

