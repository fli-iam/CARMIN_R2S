
CARMIN_R2S
-----------

Assuming that your server is based on CARMIN rest,
The CARMIN_R2S module is used for translating the rest server to soap server.


Build from Source
-----------------

For the moment, this tutorial is only tested on Ubuntu 14.04 TLS.

GSOAP
=====

Ubuntu Dependencies

$ sudo apt-get install bison flex

This module is based on gsoap. Therefore we first download the source of gsoap_2.7.17.zip.

```
https://sourceforge.net/projects/gsoap2/files/gSOAP/
```

Try to download the version gsoap_2.7.17.zip if it exists.

We don't propose to use the latest version of 2.8 since the license has been changed to GPL.

Extract gsoap_2.7.17.zip into ~/local/sources

$ SOURCES=/root/local/sources
$ INSTALL_PREFIX=/root/local
$ mkdir -p $SOURCES
$ mkdir -p $INSTALL_PREFIX
$ unzip gsoap_2.7.17.zip -d $SOURCES
$ cd $SOURCES/gsoap-2.7

Add `#define WITH_COOKIES` in the beginning of below file

./gsoap/stdsoap2.h

It is to enable the cookie module in gsoap.

$ ./configure --prefix=$INSTALL_PREFIX -DWITH_COOKIES
# you may miss some packages, please install them and re-run `./configure --prefix=$INSTALL_PREFIX -DWITH_COOKIES`.
$ make
$ make install

You can add your installed path bin in your PATH enviroment variable with ~/.bashrc

$ echo "export PATH=$INSTALL_PREFIX/bin:\$PATH" >> ~/.bashrc
$ echo "export PATH=$INSTALL_PREFIX/bin:\$PATH" >> ~/.bashrc


rapidjson 
=========

$ SOURCES=/root/local/sources
$ INSTALL_PREFIX=/root/local

$ git clone https://github.com/miloyip/rapidjson.git ${SOURCES}/rapidjson
$ ln -s ${SOURCES}/rapidjson/include/rapidjson ${INSTALL_PREFIX}/include/



Start with a wsdl file
----------------------


How to generate the .h file
============================

For example, you can start with a wsdl file for example api.wsdl.

The `api.h` file can be producced by the command below: 

```
$ wsdl2h api.wsdl
```

By default, the wsdl2h will generate the function such as 

```
int ns2__xxxxx(xxx xxxx, xxxx xxxx);
```

which has a prefix ns2__. This prefix could be changed by adding the below line in the file which called `typemap.dat`.

```
api = "http://france-life-imaging.fr/api"
```

`typemap.dat` can be found in the source of gSoap.

Put this file `typemap.dat` within the same directory of api.wsdl. You can generate the `api.h` again.

```
$ wsdl2h api.wsdl
```

Therefore all the prefixes will be changed as below:

```
int api_xxxxx(xxx xxxx, xxxx xxxx);
```


How to generate .cpp file
=========================


```
soapcpp2 -I/path/to/share/gsoap/import -S -x api.h
```


Dependencies
------------

gSOAP 2.7
libCurl
rapidjson


---------
