Gairq
===============================================================================

Gairq is a GObject based RESTful API library written in C for gathering Air Quality information.

It provides a set of APIs to request JSON-based air quality information to a server. 
Not only just a request, but also the response, JSON data, is automatically deserialized to 
``GairqAirObject`` in which is the basic data structure used for holding the information.

It is for C developers who want to gather the information in Linux Desktop Environment, IoT and so forth.

This library is yet in an early stage of development, use it just for test purposes at your own risk. 
See ``tests/city-main.c`` to find out the use of the currently implemented APIs.

For more information, see:

 * [GLib and GObject][glib]
 * [aqicn][aqicn]
 * [aqicn json-api][json-api]

Features
----------------------------------------------

 * Support GObject based class construction/finalization
 * Support Sync/Async functionalities
 * Request [City Feed][city-feed] based information
 
Todo
----------------------------------------------

 * [ ] Implement Geolocalized Feed
 * [ ] Implement Map Queries
 * [ ] Implement Search
 * [ ] Implement GObject Introspection
 * [ ] Documentation

Requirements
----------------------------------------------

 * GLib, GIO >= 2.50
 * JSON-GLib >= 1.4.0
 * librest   >= 0.7.93

Build and installation
----------------------------------------------

```sh
 $ meson _build .
 $ ninja -C _build
 $ meson test -C _build
 $ sudo ninja -C _build install
```

Test
----------------------------------------------
The following command is required to test all the cases.

```sh
 $ export GAIRQ_API_TOKEN=put-your-token-here
```

Debug
----------------------------------------------

```sh
 $ G_MESSAGES_DEBUG="Gairq" ./_build/tests/city-main
```

Copyright and licensing
----------------------------------------------

Gairq is fully under LGPL-3.0

However, you must be aware of this that the "data", it refers to the information 
from the API server, is fully under the [Term of Service][term-of-service]. 
We have no warranty on it.



[city-feed]: https://aqicn.org/json-api/doc/#api-City_Feed
[glib]: https://gitlab.gnome.org/GNOME/glib
[aqicn]: http://aqicn.org
[json-api]: https://aqicn.org/json-api/doc/#api-_
[term-of-service]: https://aqicn.org/api/
