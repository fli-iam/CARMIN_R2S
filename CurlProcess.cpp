#include "CurlProcess.h"
#include "Config.h"
#include "CookieData.h"

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

CurlProcess::CurlProcess()
{
  
}

CurlProcess::CurlProcess(const CurlProcess& c){
  
}
bool CurlProcess::parse_reponse_by_json(){
  if(m_document.Parse(this->m_resBuf.c_str()).HasParseError())
  {
    char m_error_message_buf[1028];
    snprintf(m_error_message_buf, sizeof(m_error_message_buf),
	     "cannot parse the json respone %s", this->m_resBuf.c_str());
    m_error_message = m_error_message_buf;
    return false;
  }
  return true;
}
bool CurlProcess::request(struct soap *soap,
			  const std::string &route){

  this->m_resBuf = "";

  Config config = Config();
  CURL *curl;
  CURLcode res;
  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();

  if (curl) {

    std::string readBuffer;

    curl_easy_setopt(curl, CURLOPT_URL, route.c_str());
    curl_easy_setopt(curl, CURLOPT_VERBOSE, config.VERBOSE);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, ""); /* just to start the cookie engine */

    CookieData cookieData = CookieData();
    
    cookieData.set_gsoap_cookie_to_curl_cookie(soap, curl, config.COOKIE_NAME_AUTH);

    res = curl_easy_perform(curl);

    long http_code = 0;
 
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK) {
      m_error_message = curl_easy_strerror(res);
      fprintf(stderr, "Curl perform failed: %s\n", m_error_message.c_str());
      return false;
    }
    if(http_code == 200)
    {
      this->m_resBuf = readBuffer;
      return true;
    }else{
      char errorBuf[1024];
      snprintf(errorBuf, sizeof(errorBuf), "connection error with http code %d", int(http_code));
      m_error_message = errorBuf;
      return false;
    }
  }
  return false;  
}