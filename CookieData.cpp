#include "CookieData.h"
#include <curl/curl.h>
#include <boost/algorithm/string.hpp>
#include "Config.h"

std::vector<std::string> CookieData::split_string(const char * c_data, const char * c_delimiter){
  std::vector<std::string> ret;
  std::string s = c_data;
  std::string delimiter = c_delimiter;
  size_t pos = 0;
  std::string token;
  while ((pos = s.find(delimiter)) != std::string::npos) {
	token = s.substr(0, pos);
	std::cout << token << std::endl;
	ret.push_back(token);
	s.erase(0, pos + delimiter.length());
  }
  std::cout << s << std::endl;
  ret.push_back(s);
  return ret;
}


CookieData::CookieData()
{
    // 3.5 What are all those entries in my cookies.txt file? 
    // http://www.cookiecentral.com/faq/#3.5
    domain = "";
    flag = false;
    path = "";
    secure = false;
    expiration = 0;
    name = "";
    value = "";
}

bool CookieData::parseData(const char * data)
{
  char delimiter[] = "\t";
  std::vector<std::string> fields = this->split_string(data, delimiter);
 
  /*
   * cati.cea.fr
   * FALSE
   * /
   * FALSE
   * 0
   * auth_tkt
   * 341916aefd9e9718b1d2b0ca6e6fb9b8513a0710aee27309b87db08fcd259aa70590d05774b13fc2c67519b97bb40c7429ec36cd106c4babc7f0d06fd535840d56a22cebamlucGVuZw%3D%3D!userid_type:b64unicode
   */
  
  this->domain = fields[0];

  if(boost::iequals(fields[1], "true"))
  {
    this->flag = true;
  }else{
    this->flag = false;
  }

  this->path = fields[2];

  if(boost::iequals(fields[3], "true"))
  {
    this->secure = true;
  }else{
    this->secure = false;
  }

  this->expiration = atoi(fields[4].c_str());
  this->name = fields[5];
  this->value = fields[6];

  return false;
}


bool CookieData::set_gsoap_cookie_to_curl_cookie(
      struct soap *soap,
      CURL *curl,
      const char * name
)
{    
    Config config = Config();

    char curl_cookie_data[config.COOKIE_MAX_LEN];

    const char * cur_cookie_val = soap_cookie_value(
	soap, name,
	config.GSOAP_COOKIE_DOMAIN,
	config.GSOAP_COOKIE_PATH);

    /* Netscape format cookie */
    snprintf(
      curl_cookie_data, sizeof(curl_cookie_data),
      "%s\t%s\t%s\t%s\t%lu\t%s\t%s",
      config.CATIWEB_COOKIE_DOMAIN, "FALSE", "/", "FALSE", 0L, name, cur_cookie_val);

    CURLcode res;
    res = curl_easy_setopt(curl, CURLOPT_COOKIELIST, curl_cookie_data);
    if (res != CURLE_OK) {
      fprintf(stderr, "Curl curl_easy_setopt failed: %s\n", curl_easy_strerror(res));
      return false;
    }
    return true;
}

void CookieData::set_curl_cookie_to_gsoap_cookie(
  struct soap *soap,
  PCURL_SLIST cookies_data
)
{
  Config config = Config();
  // http://www.cookiecentral.com/faq/#3.5
  // 3.5 What are all those entries in my cookies.txt file? 
  printf("set_curl_cookie_to_gsoap_cookie\n");
  std::string delimiter = "\t";
  struct curl_slist *nc;
  int i = 0;
  nc = cookies_data, i = 1;
  while (nc) {
    printf("[%d]: %s\n", i, nc->data);
    CookieData cookieData = CookieData();
    cookieData.parseData(nc->data);
    soap_set_cookie(soap, cookieData.name.c_str(),
		    cookieData.value.c_str(),
		    config.GSOAP_COOKIE_DOMAIN,
		    config.GSOAP_COOKIE_PATH
 		   );
    soap_set_cookie_expire(soap, cookieData.name.c_str(),
		    cookieData.expiration,
		    config.GSOAP_COOKIE_DOMAIN,
		    config.GSOAP_COOKIE_PATH
 		   );
    nc = nc->next;
    i++;
  }
}