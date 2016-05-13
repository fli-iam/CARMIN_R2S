#include "CurlProcess.h"
#include "Config.h"
#include "CookieData.h"
#include <boost/algorithm/string.hpp>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <unistd.h>

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

struct WriteThis {
  const char *readptr;
  int sizeleft;
};

static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{
  struct WriteThis *pooh = (struct WriteThis *)userp;
 
  if(size*nmemb < 1)
    return 0;
 
  if(pooh->sizeleft) {
    *(char *)ptr = pooh->readptr[0]; /* copy one single byte */ 
    pooh->readptr++;                 /* advance pointer */ 
    pooh->sizeleft--;                /* less data left */ 
    return 1;                        /* we return 1 byte at a time! */ 
  }
 
  return 0;                          /* no more data left to deliver */ 
}

CurlProcess::CurlProcess(Config * pconfig)
{
  this->m_pconfig = pconfig;
}

CurlProcess::CurlProcess(const CurlProcess& c){
  this->m_pconfig = c.m_pconfig;
}

bool CurlProcess::parse_reponse_by_json(){
  if(m_document.Parse(this->m_resBuf.c_str()).HasParseError())
  {
    char m_error_message_buf[1028];
    snprintf(m_error_message_buf, sizeof(m_error_message_buf),
	     "cannot parse the json response (parse error) %s", this->m_resBuf.c_str());
    m_error_message = m_error_message_buf;
    return false;
  }
  if(
    (!m_document.IsObject()) &&
    (!m_document.IsArray())
  )
  {
    char m_error_message_buf[1028];
    snprintf(m_error_message_buf, sizeof(m_error_message_buf),
	     "cannot parse the json response (not object and not array) %s", this->m_resBuf.c_str());
    m_error_message = m_error_message_buf;
    return false;
  }
  return true;
}

bool CurlProcess::request(struct soap *soap,
			  const std::string &route,
			  std::string method,
			  const char * post_data,
			  const long post_data_size
 			){
  
  if(m_pconfig->VERBOSE)
  {
    std::cout << route << std::endl;
  }
  this->m_resBuf = "";

  CURL *curl;
  CURLcode res;
  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  
  struct WriteThis pooh;
  
  boost::to_upper(method);

  struct curl_httppost *formpost=NULL;
  struct curl_httppost *lastptr=NULL;
  struct curl_slist *headerlist=NULL;

  bool is_success = true;

  if (curl) {

    std::string readBuffer;

    curl_easy_setopt(curl, CURLOPT_URL, route.c_str());
    curl_easy_setopt(curl, CURLOPT_VERBOSE, m_pconfig->VERBOSE);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcrp/0.1");
    
    if(post_data != NULL)
    {
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
      headerlist = curl_slist_append(headerlist, "Expect:");
      headerlist = curl_slist_append(headerlist, "Content-Type: application/json");
      headerlist = curl_slist_append(headerlist, "charsets: utf-8");
    }

    if(method == "POST")
    {
      if(m_pconfig->VERBOSE)
      {
          std::cout << "POST DATA:" << std::endl;
          std::cout << post_data << std::endl;
      }

      curl_easy_setopt(curl, CURLOPT_POST, 1L);

      pooh.readptr = post_data;
      pooh.sizeleft = strlen(post_data);

      //curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
      //curl_easy_setopt(curl, CURLOPT_READDATA, &pooh);

    }else if(method=="DELETE")
    {
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    }else if(method=="PUT")
    {
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
      headerlist = curl_slist_append(headerlist, "Expect:");
      headerlist = curl_slist_append(headerlist, "Content-Type: application/json");
      headerlist = curl_slist_append(headerlist, "charsets: utf-8");

      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    }

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, ""); /* just to start the cookie engine */

    CookieData cookieData = CookieData(m_pconfig);

    cookieData.set_gsoap_cookie_to_curl_cookie(soap, curl, m_pconfig->COOKIE_NAME_AUTH.c_str());

    res = curl_easy_perform(curl);

    long http_code = 0;

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    if (res != CURLE_OK) {
      is_success = false;
      m_error_message = curl_easy_strerror(res);
      fprintf(stderr, "Curl perform failed: %s\n", m_error_message.c_str());
    }


    if(http_code == 200)
    {
      this->m_resBuf = readBuffer;
      if(m_pconfig->VERBOSE==1L)
      {
	std::cout << "readBuffer=" << readBuffer << std::endl;
      }
      is_success = true;
    }else{
      char errorBuf[1024];
      snprintf(errorBuf, sizeof(errorBuf), "connection error with http code %d", int(http_code));
      m_error_message = errorBuf;
      is_success = false;
    }

    curl_easy_cleanup(curl);

    if(method == "post")
    {
      if(formpost != NULL)
      {
	curl_formfree(formpost);
	formpost = NULL;
      }
      if (headerlist != NULL)
      {
	curl_slist_free_all(headerlist);
	headerlist = NULL;
      }
    }

    curl_global_cleanup();
  }
  return is_success;  
}