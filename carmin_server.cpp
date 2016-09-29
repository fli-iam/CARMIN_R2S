#include "soapH.h"
#include "CarminSoapBinding.nsmap"
#include "CatiwebAuth.h"
#include <sys/types.h>
#include <sys/socket.h>
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


/**********************************************************
 * Global variables
 * ************************************************************/

Config config = Config();

int (* old_freponse)(struct soap *soap, int soap_error_code, size_t count);

static const char soap_padding[4] = "\0\0\0";
#define SOAP_STR_EOS (soap_padding)

static const struct soap_code_map h_http_error_codes[] =
{ { 200, "OK" },
  { 201, "Created" },
  { 202, "Accepted" },
  { 203, "Non-Authoritative Information" },
  { 204, "No Content" },
  { 205, "Reset Content" },
  { 206, "Partial Content" },
  { 300, "Multiple Choices" },
  { 301, "Moved Permanently" },
  { 302, "Found" },
  { 303, "See Other" },
  { 304, "Not Modified" },
  { 305, "Use Proxy" },
  { 307, "Temporary Redirect" },
  { 400, "Bad Request" },
  { 401, "Unauthorized" },
  { 402, "Payment Required" },
  { 403, "Forbidden" },
  { 404, "Not Found" },
  { 405, "Method Not Allowed" },
  { 406, "Not Acceptable" },
  { 407, "Proxy Authentication Required" },
  { 408, "Request Time-out" },
  { 409, "Conflict" },
  { 410, "Gone" },
  { 411, "Length Required" },
  { 412, "Precondition Failed" },
  { 413, "Request Entity Too Large" },
  { 414, "Request-URI Too Large" },
  { 415, "Unsupported Media Type" },
  { 416, "Requested range not satisfiable" },
  { 417, "Expectation Failed" },
  { 500, "Internal Server Error" },
  { 501, "Not Implemented" },
  { 502, "Bad Gateway" },
  { 503, "Service Unavailable" },
  { 504, "Gateway Time-out" },
  { 505, "HTTP Version not supported" },
  {   0, NULL }
};



static const char*
http_error(struct soap *soap, int status)
{ register const char *msg = SOAP_STR_EOS;
#ifndef WITH_LEAN
  msg = soap_code_str(h_http_error_codes, status);
  if (!msg)
    msg = SOAP_STR_EOS;
#endif
  return msg;
}



int soap_fresponse(struct soap *soap, int status, size_t count)
{
  //int err;
  //std::cout << "DEBUG soap_fresponse" << std::endl;

  //if ((err = soap->fposthdr(soap, "Access-Control-Allow-Origin", "*")))
  //    return err;
  //if ((err = soap->fposthdr(soap, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, CONNECT")))
  //    return err;
  //if ((err = soap->fposthdr(soap, "Access-Control-Allow-Headers", "X-Requested-With, Content-Type")))
  //    return err;
  //if ((err = soap->fposthdr(soap, "Access-Control-Allow-Credentials", "true")))
  //    return err;
   
  //int ret = old_freponse(soap, soap_error_code, count);

  //return ret;
  std::cout << "debug status=" << status << std::endl;
  status = 200;

register int err;
#ifdef WMW_RPM_IO
  if (soap->rpmreqid)
    httpOutputEnable(soap->rpmreqid);
#endif
  if (strlen(soap->http_version) > 4)
    return soap->error = SOAP_EOM;
  if (!status || status == SOAP_HTML || status == SOAP_FILE)
  { const char *s;
    if (count || ((soap->omode & SOAP_IO) == SOAP_IO_CHUNK))
      s = "200 OK";
    else
      s = "202 ACCEPTED";
    DBGLOG(TEST, SOAP_MESSAGE(fdebug, "Status = %s\n", s));
#ifdef WMW_RPM_IO
    if (soap->rpmreqid || soap_valid_socket(soap->master) || soap_valid_socket(soap->socket)) /* RPM behaves as if standalone */
#else
    if (soap_valid_socket(soap->master) || soap_valid_socket(soap->socket)) /* standalone application (socket) or CGI (stdin/out)? */
#endif
    { sprintf(soap->tmpbuf, "HTTP/%s %s", soap->http_version, s);
      if ((err = soap->fposthdr(soap, soap->tmpbuf, NULL)))
        return err;
    }
    else if ((err = soap->fposthdr(soap, "Status", s))) /* CGI header */
      return err;
  }
  else if (status >= 200 && status < 600)
  { sprintf(soap->tmpbuf, "HTTP/%s %d %s", soap->http_version, status, http_error(soap, status));
    if ((err = soap->fposthdr(soap, soap->tmpbuf, NULL)))
      return err;
#ifndef WITH_LEAN 
    if (status == 401)
    { sprintf(soap->tmpbuf, "Basic realm=\"%s\"", (soap->authrealm && strlen(soap->authrealm) < sizeof(soap->tmpbuf) - 14) ? soap->authrealm : "gSOAP Web Service");
      if ((err = soap->fposthdr(soap, "WWW-Authenticate", soap->tmpbuf)))
        return err;
    }
    else if ((status >= 301 && status <= 303) || status == 307)
    { if ((err = soap->fposthdr(soap, "Location", soap->endpoint)))
        return err;
    }
#endif
  }
else
  { const char *s = *soap_faultcode(soap);
    //std::cout << "DEBUG status=" << status << std::endl;
    //std::cout << "DEBUG SOAP_GET_METHOD=" << SOAP_GET_METHOD << std::endl;
    //std::cout << "DEBUG SOAP_HTTP_METHOD=" << SOAP_HTTP_METHOD << std::endl;
    if (status >= SOAP_GET_METHOD && status <= SOAP_HTTP_METHOD)
      s = "405 Method Not Allowed";
    else if (soap->version == 2 && (!s || !strcmp(s, "SOAP-ENV:Sender")))
      s = "400 Bad Request";
    else
      s = "500 Internal Server Error";
    DBGLOG(TEST, SOAP_MESSAGE(fdebug, "Error %s (status=%d)\n", s, status));
#ifdef WMW_RPM_IO
    if (soap->rpmreqid || soap_valid_socket(soap->master) || soap_valid_socket(soap->socket)) /* RPM behaves as if standalone */
#else
    if (soap_valid_socket(soap->master) || soap_valid_socket(soap->socket)) /* standalone application */
#endif
    { sprintf(soap->tmpbuf, "HTTP/%s %s", soap->http_version, s);
      if ((err = soap->fposthdr(soap, soap->tmpbuf, NULL)))
        return err;
    }
    else if ((err = soap->fposthdr(soap, "Status", s))) /* CGI */
      return err;
  }
  if ((err = soap->fposthdr(soap, "Server", "gSOAP/2.7"))
   || (err = soap_puthttphdr(soap, status, count)))
    return err;
#ifdef WITH_COOKIES
  if (soap_putsetcookies(soap))
    return soap->error;
#endif

  //if ((err = soap->fposthdr(soap, "Access-Control-Allow-Origin", "*")))
  //    return err;

  //if ((err = soap->fposthdr(soap, "Access-Control-Allow-Credentials", "true")))
  //    return err;


  //if ((err = soap->fposthdr(soap, "Access-Control-Allow-Headers", "origin,x-requested-with,access-control-request-headers,content-type,access-control-request-method,accept")))
  //    return err;

  //if ((err = soap->fposthdr(soap, "Access-Control-Allow-Methods", "POST,HEAD,GET,PUT,DELETE,OPTIONS")))
  //    return err;

  //if ((err = soap->fposthdr(soap, "Allow", "HEAD,GET,PUT,DELETE,OPTIONS")))
  //    return err;

  return soap->fposthdr(soap, NULL, NULL);


}

int soap_fhead(struct soap *soap)
{
   //std::cout << "DEBUG soap_fhead" << std::endl;
   return SOAP_OK;
}

int soap_foptions(struct soap *soap)
{ 
   int err=0; 
   //std::cout << "DEBUG soap_foptions" << std::endl;
   //if ((err = soap->fposthdr(soap, "Allow", "CONVERT")))
   //    return err;
   soap_response(soap, SOAP_OK); 
   soap_end_send(soap);
   return SOAP_OK;
}

int http_post(struct soap *soap, const char *endpoint, const char *host, int port, const char *path, const char *action, size_t count)
{
    //std::cout << "hi, i am here post" << std::endl;
    //soap->fposthdr(soap, "Access-Control-Allow-Origin", "*"); 
    return SOAP_OK;
}

int http_get(struct soap *soap) 
{
   //std::cout << "DEBUG i am here get" << std::endl;
   //soap->fposthdr(soap, "Access-Control-Allow-Origin", "*");

   FILE *fd = NULL;
   char *s = strchr(soap->path, '?'); 
   if (!s || strcmp(s, "?wsdl")) 
      return SOAP_GET_METHOD; 
   fd = fopen(config.WSDL_FILE_PATH.c_str(), "rb"); // open WSDL file to copy 
   if (!fd) 
      return 404; // return HTTP not found error 
   soap->http_content = "text/xml"; // HTTP header with text/xml content 
   soap_response(soap, SOAP_FILE); 
   for (;;) 
   { 
      size_t r = fread(soap->tmpbuf, 1, sizeof(soap->tmpbuf), fd); 
      if (!r) 
         break; 
      if (soap_send_raw(soap, soap->tmpbuf, r)) 
         break; // can't send, but little we can do about that 
   } 
   fclose(fd); 
   soap_end_send(soap); 
   return SOAP_OK; 
}

void *process_request(void *soap) 
{ 
   pthread_detach(pthread_self());
   ((struct soap*)soap)->recv_timeout = 300; // Timeout after 5 minutes stall on recv 
   ((struct soap*)soap)->send_timeout = 60; // Timeout after 1 minute stall on send 

   ((struct soap*)soap)->fget = http_get;
   ((struct soap*)soap)->fpost = http_post;
   ((struct soap*)soap)->foptions = soap_foptions;
   old_freponse = ((struct soap*)soap)->fresponse;

   ((struct soap*)soap)->fresponse = soap_fresponse;
   ((struct soap*)soap)->fhead = soap_fhead;

   //std::cout << "DEBUG process_request pt1" << std::endl;
   soap_serve((struct soap*)soap);
 
   soap_destroy((struct soap*)soap); 
   soap_end((struct soap*)soap); 
   soap_free((struct soap*)soap); 
   
   return NULL;
}


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

   struct soap soap, *tsoap;
   pthread_t tid;
   int m, s;
   //soap_init2(&soap, SOAP_IO_KEEPALIVE, SOAP_IO_KEEPALIVE);
   //soap.max_keep_alive = 10;
   //soap.accept_timeout = 15;
   soap.cookie_domain = ".."; 
   soap.cookie_path = "/"; // the path which is used to filter/set cookies with this destination 
   int count = 0; 
   if (argc < 2)
   { 

      std::cout << "It should contain config file. Please checkout the config examples in config_template" << std::endl;
      exit(1);

      soap_getenv_cookies(&soap); // CGI app: grab cookies from 'HTTP_COOKIE' env var 
      soap_serve(&soap);

   }
   else
   {  
      std::string path_config = argv[1];
      if(!config.LoadFromFile(path_config))
      {
	exit(1);
      }


      m = soap_bind(&soap, NULL, config.GSOAP_SERVER_PORT, 100); 
      if (m < 0)
         exit(1);

      for (count = 0; count >= 0; count++) 
      { 
         soap.socket_flags = MSG_NOSIGNAL; // use this 
         //soap.accept_flags = SO_NOSIGPIPE; // or this to prevent sigpipe 
         s = soap_accept(&soap); 
         if (s < 0) 
         { 
            if (soap.errnum) 
               soap_print_fault(&soap, stderr); 
            else
               fprintf(stderr, "Server timed out\n"); // Assume timeout is long enough for threads to complete serving requests 
            break; 
         } 
         fprintf(stderr, "Accepts socket %d connection from IP %d.%d.%d.%d\n", s, (int)(soap.ip >> 24)&0xFF, (int)(soap.ip >> 16)&0xFF, (int)(soap.ip >> 8)&0xFF, (int)soap.ip&0xFF); 
         tsoap = soap_copy(&soap); 
         pthread_create(&tid, NULL, (void*(*)(void*))process_request, (void*)tsoap); 
      } 

   }
   return 0; 
} 


int __api__authenticateSession(
  struct soap *soap,
  // request parameters:
  api__authenticateSession*           api__authenticateSession_,
  // response parameters:
  api__authenticateSessionResponse*   api__authenticateSessionResponse_
)
{
  //std::cout << "username:" << api__authenticateSession_->userName << std::endl;
  //std::cout << "password:" << api__authenticateSession_->password << std::endl;
  
  PCURL_SLIST cookies_data_list = NULL;
  CatiwebAuth catiwebAuth = CatiwebAuth(&config);

  bool is_success = catiwebAuth.Login(
      api__authenticateSession_->userName.c_str(),
      api__authenticateSession_->password.c_str(),
      cookies_data_list
  );

  api__authenticateSessionResponse_->return_ = soap_new_api__Response(soap, 1);

  if (is_success){
    CookieData cookieData = CookieData(&config);
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

  Pipelines pipelines = Pipelines(&config);
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
  Pipeline pipeline = Pipeline(&config);
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
  
  Execution execution = Execution(&config);
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

  Execution execution = Execution(&config);
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
 
  Execution execution = Execution(&config);
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
 
  Execution execution = Execution(&config);
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
 
  GlobalProperties globalProperties = GlobalProperties(&config);
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
 
  Execution execution = Execution(&config);
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
  if(config.VERBOSE)
  {
    std::cout << "__api__playExecution" << std::endl;
  }
  api__playExecutionResponse_->return_ = soap_new_api__Response(soap, 1);
  Execution execution = Execution(&config);
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
 
  Execution execution = Execution(&config);
  bool is_success = execution.getStdOut(soap, api__getStdOut_->executionId);

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
 
  Execution execution = Execution(&config);
  bool is_success = execution.getStdErr(soap, api__getStdErr_->executionId);

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

  Execution execution = Execution(&config);

  bool is_success = false;

  bool type_error = false;
  
  std::string type_error_message = "For the moment, only string value can be accepted.";
  
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

