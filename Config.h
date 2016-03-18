
#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <iostream>
#include <map>


// http://www.cplusplus.com/forum/general/21115/
//---------------------------------------------------------------------------
// The configuration::data is a simple map string (key, value) pairs.
// The file is stored as a simple listing of those pairs, one per line.
// The key is separated from the value by an equal sign '='.
// Commentary begins with the first non-space character on the line a hash or
// semi-colon ('#' or ';').
//
// Example:
//   # This is an example
//   source.directory = C:\Documents and Settings\Jennifer\My Documents\
//   file.types = *.jpg;*.gif;*.png;*.pix;*.tif;*.bmp
//
// Notice that the configuration file format does not permit values to span
// more than one line, commentary at the end of a line, or [section]s.
// 


class ConfigData: public std::map <std::string, std::string>
{
  public:
    // Here is a little convenience method...
    bool iskey(const std::string& s) const;
};


//---------------------------------------------------------------------------
// The extraction operator reads configuration::data until EOF.
// Invalid data is ignored.
//
std::istream & operator >> ( std::istream& ins, ConfigData& d );


//---------------------------------------------------------------------------
// The insertion operator writes all configuration::data to stream.
//
std::ostream& operator << ( std::ostream& outs, const ConfigData& d );



class Config: ConfigData
{
  public:

    Config(std::string path_config);

    Config();

    bool LoadFromFile(std::string path_config,
		      bool is_need_exit=false);

    int GSOAP_SERVER_PORT;
    std::string GSOAP_SERVER_ROOT;
    std::string GSOAP_COOKIE_DOMAIN;
    std::string GSOAP_COOKIE_PATH;

    std::string CATIWEB_WEBSERVICE_API;
    std::string CATIWEB_COOKIE_DOMAIN;
    std::string CATIWEB_COOKIE_PATH;
    std::string CATIWEB_PIPELINES;
    
    std::string COOKIE_NAME_AUTH;

    std::string get_route_catiweb_pipelines();

    static const int COOKIE_MAX_LEN=512;
    static const int URL_MAX_LEN=2083;

    long VERBOSE;

};

#endif