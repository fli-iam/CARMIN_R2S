#include "CatiwebLogout.h"
#include "Config.h"

CatiwebLogout::CatiwebLogout(){
  
}

bool CatiwebLogout::request(struct soap *soap){
  Config config = Config();
  char routeBuf[config.URL_MAX_LEN];
  snprintf(routeBuf, sizeof(routeBuf), "%s/logout",
	   config.CATIWEB_WEBSERVICE_API);
  if (config.VERBOSE == 1L)
  {
    std::cout << "route:" << routeBuf << std::endl;
  }

  if(!CurlProcess::request(soap, routeBuf))
  {
    return false;
  }
  
  return true;
}