#include "CatiwebAuth.h"
#include "Config.h"
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include "Config.h"


CatiwebAuth::CatiwebAuth(Config *config){
  m_pconfig = config;
}

static void
print_cookies(CURL *curl)
{
  CURLcode res;
  struct curl_slist *cookies;
  struct curl_slist *nc;
  int i;

  printf("Cookies, curl knows:\n");
  res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
  if (res != CURLE_OK) {
    fprintf(stderr, "Curl curl_easy_getinfo failed: %s\n", curl_easy_strerror(res));
    exit(1);
  }
  nc = cookies, i = 1;
  while (nc) {
    printf("[%d]: %s\n", i, nc->data);
    nc = nc->next;
    i++;
  }
  if (i == 1) {
    printf("(none)\n");
  }
  curl_slist_free_all(cookies);
}


bool CatiwebAuth::Login(const char * username,  const char * password, PCURL_SLIST& cookies_data)
{
    /*
     * 	cookies_data should be free by curl_slist_free_all.
     */
    m_error_message = "";

    if(m_pconfig->VERBOSE)
    {
      std::cout << "CatiwebAuth::Login login:" << username << " password:" << password <<std::endl;
    }

    CURL *curl;
    CURLcode res;
    
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    
    if (curl) {

      char auth_url[m_pconfig->URL_MAX_LEN];
      snprintf(auth_url, sizeof(auth_url), "%s/login?username=%s&password=%s", m_pconfig->CATIWEB_WEBSERVICE_API.c_str(), username, password);

      if(m_pconfig->VERBOSE){
      std::cout << "auth_url=" << auth_url << std::endl;
      }

      curl_easy_setopt(curl, CURLOPT_URL, auth_url);
      curl_easy_setopt(curl, CURLOPT_VERBOSE, m_pconfig->VERBOSE);
      curl_easy_setopt(curl, CURLOPT_COOKIEFILE, ""); /* just to start the cookie engine */
      
      res = curl_easy_perform(curl);

      long http_code = 0;
      
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);      

      if (res != CURLE_OK) {
	m_error_message = curl_easy_strerror(res);
	fprintf(stderr, "Curl perform failed: %s\n", m_error_message.c_str());
	return false;
      }

      if(http_code == 200)
      {
	print_cookies(curl);
	res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies_data);
	if (res != CURLE_OK) {
	  m_error_message = curl_easy_strerror(res);
	  fprintf(stderr, "Curl curl_easy_getinfo failed: %s\n", m_error_message.c_str());
	  return false;
	}
	return true;
      }else{
	return false;
      }
    }
    
    return false;
}