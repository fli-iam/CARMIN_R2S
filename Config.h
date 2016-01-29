
#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class Config
{
  public:
    static const char * GSOAP_SERVER_ROOT;
    static const char * GSOAP_COOKIE_DOMAIN;
    static const char * GSOAP_COOKIE_PATH;

    static const char * CATIWEB_WEBSERVICE_API;
    static const char * CATIWEB_COOKIE_DOMAIN;
    static const char * CATIWEB_COOKIE_PATH;
    static const char * CATIWEB_PIPELINES;
    
    static const char * COOKIE_NAME_AUTH;
    
    static std::string get_route_catiweb_pipelines();

    static const int COOKIE_MAX_LEN=512;
    static const int URL_MAX_LEN=2083;
    
    static const long VERBOSE=1L;

};

#endif