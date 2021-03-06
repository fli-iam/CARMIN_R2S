CARMIN_R2S
-----------

CARMIN_R2S (CARMIN Rest server to Soap server) <img src="https://travis-ci.org/JinpengLI/CARMIN_R2S.svg?branch=master">

Assuming that your server is based on CARMIN rest (carmin.yaml in https://github.com/fli-iam/CARMIN),
CARMIN_R2S module is used for translating the rest server to the soap server (api.wsdl, https://github.com/fli-iam/CARMIN).


Build from Source
-----------------

For the moment, this tutorial is only tested on Ubuntu 14.04 TLS.

gSOAP
=====

Ubuntu Dependencies

```
$ sudo apt-get install bison flex
$ sudo apt-get install libcurl4-gnutls-dev
```

This module is based on gsoap. Therefore we first download the source of gsoap_2.7.17.zip.

```
https://sourceforge.net/projects/gsoap2/files/gSOAP/
```

Try to download the version gsoap_2.7.17.zip if it exists.

We don't propose to use the latest version of 2.8 since the license has been changed to GPL.

Extract gsoap_2.7.17.zip into ~/local/sources

```
$ SOURCES=/root/local/sources
$ INSTALL_PREFIX=/root/local
$ mkdir -p $SOURCES
$ mkdir -p $INSTALL_PREFIX
$ unzip gsoap_2.7.17.zip -d $SOURCES
$ cd $SOURCES/gsoap-2.7
```

Add the below codes in the beginning of the header file ./gsoap/stdsoap2.h

```
#ifndef WITH_COOKIES
#define WITH_COOKIES
#endif
```

It is used for enabling the cookie module in gsoap.

```
$ ./configure --prefix=$INSTALL_PREFIX
```

you may miss some packages, please install them and re-run `./configure --prefix=$INSTALL_PREFIX`.

```
$ make
$ make install
```


You can add your installed path bin in your PATH enviroment variable with ~/.bashrc

```
$ echo "export PATH=$INSTALL_PREFIX/bin:\$PATH" >> ~/.bashrc
$ echo "export LD_LIBRARY_PATH=$INSTALL_PREFIX/lib:\$LD_LIBRARY_PATH" >> ~/.bashrc
```

rapidjson 
=========

```
$ SOURCES=/root/local/sources
$ INSTALL_PREFIX=/root/local

$ git clone https://github.com/miloyip/rapidjson.git ${SOURCES}/rapidjson
$ ln -s ${SOURCES}/rapidjson/include/rapidjson ${INSTALL_PREFIX}/include/
```


libcurl
===========

On ubuntu, you can install it via apt-get

```
$ apt-get install libcurl4-gnutls-dev
$ apt-get install libcurl4-openssl-dev
```

or built it from sources

```
$ SOURCES=/root/local/sources
$ INSTALL_PREFIX=/root/local

$ cd $SOURCES
$ wget https://curl.haxx.se/snapshots/curl-7.50.0-20160705.tar.gz
$ tar -xvf curl-7.50.0-20160705.tar.gz
$ cd curl-7.50.0-20160705
$ ./configure --prefix=$INSTALL_PREFIX
$ make
$ make install

```

If you build libcurl from source, you have to modify TARGET_LINK_LIBRARIES as below in the CMakeLists.txt from the project CARMIN_R2S.

```
TARGET_LINK_LIBRARIES(carmin_server
${LOCAL_INSTALL_PREFIX}/lib/libgsoap++.a
${LOCAL_INSTALL_PREFIX}/lib/libgsoapssl++.a
${LOCAL_INSTALL_PREFIX}/lib/libcurl.so
)
```


CARMIN_R2S
==========

```
$ SOURCES=/root/local/sources
$ INSTALL_PREFIX=/root/local

$ git clone https://github.com/fli-iam/CARMIN_R2S.git ${SOURCES}/CARMIN_R2S
$ cd ${SOURCES}/CARMIN_R2S
$ cp CMakeLists.txt.origin CMakeLists.txt
```

Modify LOCAL_INSTALL_PREFIX's value as the gsoap and rapidjson location in CMakeLists.txt.

```
set(LOCAL_INSTALL_PREFIX "/root/local")
```

```
$ mkdir builds
$ cd builds
$ cmake ..
$ make
```

Hope there is no error during compilation.

If there is no error, you can start the server as below:

```
$ ${SOURCES}/CARMIN_R2S/builds/carmin_server ${SOURCES}/CARMIN_R2S/config_template/config_demo.ini
```

How to update the .h file from a .wsdl file
===========================================


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


