#include "Config.h"

#include <stdlib.h>
#include <stdio.h>

const char *Config::GSOAP_SERVER_ROOT = "http://localhost:8080";
const char *Config::GSOAP_COOKIE_DOMAIN = "localhost";
const char *Config::GSOAP_COOKIE_PATH = "/";


//const char *Config::CATIWEB_WEBSERVICE_API = "https://cati.cea.fr/catidb3/api/1";
const char *Config::CATIWEB_WEBSERVICE_API = "http://0.0.0.0:6547/api/1";
const char *Config::CATIWEB_COOKIE_DOMAIN = "0.0.0.0";
const char *Config::CATIWEB_COOKIE_PATH = "/";

const char * Config::CATIWEB_PIPELINES = "/pipelines";

const char * Config::COOKIE_NAME_AUTH = "auth_tkt";

std::string Config::get_route_catiweb_pipelines(){
  std::string url;
  char url_buf[URL_MAX_LEN];
  snprintf(url_buf, URL_MAX_LEN, "%s%s", CATIWEB_WEBSERVICE_API, CATIWEB_PIPELINES);
  url = url_buf;
  return url;
}