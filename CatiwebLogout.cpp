#include "CatiwebLogout.h"
#include "Config.h"

CatiwebLogout::CatiwebLogout(Config * config): CurlProcess(config){
  m_pconfig = config;
}

bool CatiwebLogout::request(struct soap *soap){
  char routeBuf[m_pconfig->URL_MAX_LEN];
  snprintf(routeBuf, sizeof(routeBuf), "%s/logout",
	   m_pconfig->CATIWEB_WEBSERVICE_API.c_str());
  if (m_pconfig->VERBOSE == 1L)
  {
    std::cout << "route:" << routeBuf << std::endl;
  }

  if(!CurlProcess::request(soap, routeBuf))
  {
    return false;
  }
  
  return true;
}