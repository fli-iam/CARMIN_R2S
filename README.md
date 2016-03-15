
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

