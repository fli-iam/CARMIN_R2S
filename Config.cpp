#include "Config.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream> 
#include <stdlib.h>
#include <stdio.h>

bool ConfigData::iskey(const std::string& s) const
{
  return count(s) != 0;
}

Config::Config():ConfigData(){
  VERBOSE=0L;
}



//---------------------------------------------------------------------------
// The extraction operator reads configuration::data until EOF.
// Invalid data is ignored.
//
std::istream & operator >> ( std::istream& ins, ConfigData& d )
{
  std::string s, key, value;

  // For each (key, value) pair in the file
  while(std::getline(ins, s))
  {
    std::string::size_type begin = s.find_first_not_of( " \f\t\v" );

    // Skip blank lines
    if (begin == std::string::npos) continue;

    // Skip commentary
    if (std::string( "#;" ).find( s[ begin ] ) != std::string::npos) continue;

    // Extract the key value
    std::string::size_type end = s.find( '=', begin );
    key = s.substr( begin, end - begin );

    // (No leading or trailing whitespace allowed)
    key.erase( key.find_last_not_of( " \f\t\v" ) + 1 );

    // No blank keys allowed
    if (key.empty()) continue;

    // Extract the value (no leading or trailing whitespace allowed)
    begin = s.find_first_not_of( " \f\n\r\t\v", end + 1 );
    end   = s.find_last_not_of(  " \f\n\r\t\v" ) + 1;

    value = s.substr(begin, end - begin);

    // Insert the properly extracted (key, value) pair into the map
    d[key] = value;
  }

  return ins;
}



//---------------------------------------------------------------------------
// The insertion operator writes all configuration::data to stream.
//
std::ostream& operator << ( std::ostream& outs, const ConfigData& d )
{
  ConfigData::const_iterator iter;
  for (iter = d.begin(); iter != d.end(); iter++)
    outs << iter->first << " = " << iter->second << std::endl;
  return outs;
}


Config::Config(std::string path_config): ConfigData(){
  LoadFromFile(path_config, true);
}

bool Config::LoadFromFile(std::string path_config, bool is_need_exit){
    std::ifstream f(path_config.c_str());
    f >> (*this);
    f.close();

    this->GSOAP_SERVER_PORT = 8080; // default port

    std::vector<std::string> required_fields;
    std::vector<std::string> missing_fields;

    required_fields.push_back(std::string("GSOAP_SERVER_PORT"));
    required_fields.push_back(std::string("GSOAP_SERVER_ROOT"));
    required_fields.push_back(std::string("GSOAP_COOKIE_DOMAIN"));
    required_fields.push_back(std::string("GSOAP_COOKIE_PATH"));

    required_fields.push_back(std::string("CATIWEB_WEBSERVICE_API"));
    required_fields.push_back(std::string("CATIWEB_COOKIE_DOMAIN"));
    required_fields.push_back(std::string("CATIWEB_COOKIE_PATH"));

    required_fields.push_back(std::string("CATIWEB_PIPELINES"));
    required_fields.push_back(std::string("COOKIE_NAME_AUTH"));
    required_fields.push_back(std::string("VERBOSE"));

    for(int i=0; i<required_fields.size(); i++)
    {
      std::string required_field = required_fields[i];
      if(!this->iskey(required_field))
      {
	missing_fields.push_back(required_field);
      }
    }

    if(missing_fields.size() > 0)
    {
	std::cout << "Config file misses a parameter....." << std::endl;

	for(int i=0;i<missing_fields.size();i++)
	{
	  std::cout << "missing :" << missing_fields[i] << std::endl;
	}

	if(is_need_exit)
	{
	  exit(EXIT_FAILURE);
	}

	return false;
    }

    this->GSOAP_SERVER_PORT = atoi((*this)["GSOAP_SERVER_PORT"].c_str());
    this->GSOAP_SERVER_ROOT = (*this)["GSOAP_SERVER_ROOT"];
    this->GSOAP_COOKIE_DOMAIN = (*this)["GSOAP_COOKIE_DOMAIN"];
    this->GSOAP_COOKIE_PATH = (*this)["GSOAP_COOKIE_PATH"];

    this->CATIWEB_WEBSERVICE_API = (*this)["CATIWEB_WEBSERVICE_API"];
    this->CATIWEB_COOKIE_DOMAIN = (*this)["CATIWEB_COOKIE_DOMAIN"];
    this->CATIWEB_COOKIE_PATH = (*this)["CATIWEB_COOKIE_PATH"];

    this->CATIWEB_PIPELINES = (*this)["CATIWEB_PIPELINES"];
    this->COOKIE_NAME_AUTH = (*this)["COOKIE_NAME_AUTH"];

    this->VERBOSE = atoi((*this)["VERBOSE"].c_str());

    if(this->VERBOSE)
    {
      std::cout << "Configuration loaded:"<< std::endl;
      Config::const_iterator iter;
      for(iter = this->begin(); iter != this->end(); iter++)
      {
	  std::cout << iter->first << " = " << iter->second << std::endl;
      }
    }

    return true;
}

std::string Config::get_route_catiweb_pipelines(){
  std::string url;
  char url_buf[URL_MAX_LEN];
  snprintf(url_buf, URL_MAX_LEN, "%s%s", CATIWEB_WEBSERVICE_API.c_str(), CATIWEB_PIPELINES.c_str());
  url = url_buf;
  return url;
}