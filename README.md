# Level2

- [Level2](#level2)
  - [What it is](#what-it-is)
  - [Preface](#preface)
    - [Why this silly name "Level2"](#why-this-silly-name-level2)
    - [The main building blocks in a nutshell](#the-main-building-blocks-in-a-nutshell)
      - [Schematic overview](#schematic-overview)
      - [Runtime](#runtime)
      - [Business Objects](#business-objects)
      - [Business Logic](#business-logic)
  - [Technical Implementation](#technical-implementation)
    - [Provided Modules](#provided-modules)
  - [How to build it](#how-to-build-it)
    - [External Dependencies](#external-dependencies)
      - [GoogleTest](#googletest)
      - [nlohmann/json](#nlohmannjson)
      - [SQLite](#sqlite)
      - [MQTT / Mosquitto](#mqtt--mosquitto)
  - [How to use](#how-to-use)
  - [Current bucket list](#current-bucket-list)

## What it is

Level2 is an event driven business application framework written in C++. Its main idea is to keep **Business Objects**, **Business Logic** and **Runtime** separated from each other to allow

- the runtime to be scalable
- business objects to be defined independent of the runtime and the business logic
- the business logic to be defined as part of the configuration allowing declarative description of the logic
- deployment of business logic to be independent of the software deployment process
- to build any kind of message oriented middleware and dialog based business (web) applications on top of it.

## Preface

This software is still in a very early state. This means, that it is not production ready yet and not all features, that are described in the current documentation or that are required for production readiness, are already (fully) implemented and available. In particular, being a framework, this project requires real life use cases as proof of concept, which are also not implemented yet.

Even though, there might still be numerous gaps to be filled, Level2 already is capable of processing any kind of structured (text based) data using configurable and modularized workflows. Hence, the development has reached a point, where it makes sense to start spending further development effort with real business requirements in mind. This is mainly the case, because the core concept and core functionalities are already in place and working.

So, if you are looking for a small, performant, scalable open source framework to base your business applications on, Level2 might be worth a look for you. In case you are interested in participating and/or contributing, both would be very welcome and appreciated. But of course I am also happy if you simply use the project for your own purposes.

### Why this silly name "Level2"

Because Level2 is my second implementation of an event driven modular business application framework. This time completely from scratch as free software and with the knowledge of certain important dos and don'ts in mind.

This will help to leap applications, that are running on this framework to the next level (level2) in terms of scalability, performance and minimization of TCO. Or as Sledge Hammer would say: "Trust me, I know what I'm doing" ;-)

### The main building blocks in a nutshell

#### Schematic overview

![schematic overview of the dataflow](./img/level2DataFlow.png)

#### Runtime

The runtime implements the input-processing-output (IPO) pattern. While doing so,it provides

- Interfaces to external systems in the form of listeners (for receiving of incoming data), connectors (for sending data to external systems)
- Capability to choose between parallel and strict fifo processing of business logic
- Synchronous and asynchronous processing
- Scalability from having very small footprint in the hosting system to high performance and high data throughput on more capable systems

#### Business Objects

Business objects can either be represented in form of structured text data such as common EDI formats, XML, Json and so on or as instances of binary data objects, that might have been serialized from databases or external incoming data of any kind.

When reaching the system, incoming data is being enriched with meta information, that describes its state and (ideally) its type. By receiving this enrichment, it becomes the payload for a transaction, that will be carried out by a certain type of business logic.

#### Business Logic

The smallest building blocks of business logic inside the framework are business logic modules. These modules themselves implement the IPO pattern. They are based on a simple API, which allows modular extensibility of the system's capabilities in a way, that is independent of development and release cycle of the framework itself.

Business logic modules can be chained together into pipelines. This is, where the state (respectively the meta information) of a payload and its business object comes into play. Each pipeline can have a set to matching patterns attached. These patterns are matched against the state of any incoming payload. Once a pipeline's matching patters match a payload, this particular payload will be processed by the corresponding pipeline. During the processing of a payload, a pipeline or more precise the contained business logic modules can apply any kind of changes to the payload and its state information. This can either cause the payload to be processed by subsequent matching pipelines or being handed over to external systems.

## Technical Implementation

### Provided Modules

- **Process Pipelining** allows declarative definition of processing pipelines. Such pipelines consist of a series of business logic specific modules, that can be chained together. As pipelines are defined declarative, they can be changed and extended at runtime, making it quick and easy to react to changing business requirements.
  
- **Connectors** to external systems using local filesystem, MQTT and HTTP (coming soon).Any further protocol is possible and can be implemented as business logic module even outside of the framework.

- Event **listeners** for local filesystem, MQTT and HTTP (coming soon). Any further protocol is possible but currently must be implemented as part of the framework.

- **API** for developing business logic modules and connectors. Processing pipelines can be extended by making use of a **lean API**, that allows quick implementation of new business logic modules. For further details refer to the [Pipeline Implementation Part of the Framework](./framework/src/pipeline/README.md)

- Generic **SQL Database Interface** to enable processing pipelines to communicate with any RDBMS. Currently SQLite is implemented as the first one. See [here](./framework/src/dbinterface/README.md) how to use the existing implementation and to extend for the support of further RDBMSs.

- Formatted and filtered **Logging** to any destination. Currently implemented are logging destinations for `stdout`, `stderr`, `file` and `syslog`. See [here](./framework/src/logger/README.md) for details.

- **Application Context** covers thread safe access to environmental information about the application and the main configuration of the application as such. Modules of the application (currently the listeners) can be bootstrapped using the `ApplicationContext`.

## How to build it

After cloning the repo cd into the root of the repo and do the following:

- Build the project and execute all unit tests by executing
  
    `cmake . && make && ctest -V`

    This will automatically execute `getDependencies.sh` and download  `googletest` to `_deps/googletest/` from where `cmake`  will trigger its build.

    Further on, [CMakeLists.txt](./CMakeLists.txt) will download [nlohmann/json](https://github.com/nlohmann/json) and make it available to the project build.

### External Dependencies

#### GoogleTest

GoogleTest is heavily used for unit, component and integration tests in [framework/tests/](./framework/tests/).

It is automatically downloaded and built inside the project by `execute_process(COMMAND ./getDependencies.sh)` in [CMakeLists.txt](./CMakeLists.txt).

#### nlohmann/json

[nlohmann/json](https://github.com/nlohmann/json) is used throughout the project for serializing objects to and from json. This dependency is satisfied and made available to the whole project by use of `FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)` and `FetchContent_MakeAvailable(json)` in [CMakeCache.txt](./CMakeCache.txt)

#### SQLite

For building and using the DB interface classes in [dbinterface](./framework/src/dbinterface/) it is required to have SQLite development packages installed.

CMake makes use of `find_package(SQLite3 REQUIRED)` to make SQLite available for the build.

#### MQTT / Mosquitto

To build with support for MQTT, it is required to have [Eclipse Mosquitto](https://mosquitto.org/) and its development packages installed. [CMakeLists.txt](./framework/CMakeLists.txt) uses `find_package(Mosquito REQUIRED)` to make it available for the build process. However, Level2 can still be built without this dependency. MQTT support will be excluded from the build if Mosquitto can not be found.

For successful execution of MQTT related unit tests in [test_mqttconnector.cpp](./framework/tests/test_mqttconnector.cpp) and [test_mqttlistener.cpp](./framework/tests/test_mqttlistener.cpp) it is required to have an MQTT broker listening on port 1883 on the loopback interface.

## How to use

First real roundtrip, that makes use of HttpListener to

- trigger synchronous processing
- selecting the processing pipeline by parameters from the incoming HTTP request
- returning processed data to the issuer of the request

can be found in an integration test in [test_httplistener.cpp](./framework/tests/test_httplistener.cpp). Please refer to test function `TEST(HTTPListener, sendsDataBackOnGetRequest)` to see for more details on the source code part of the implementation as well as the configuration of the required (simple) processing pipeline.

Please find one first sample application [here](./sampleApplications/README.md). (Work in Progress ;-) )

## Current bucket list

- Documentation, documentation, documentation
- A first **sample application** as a proof of concept and showcase
- Authentication API and module for the HTTP server (HttpListener)
- Request filtering API and module for the HTTP server (HttpListener)
- **HTTP client** / HttpConnector.
- Blueprinting a setup to run HttpListener behind a reverse proxy
- Sample implementation of a weberver for static content using HttpListener
- Proof of concept for passing [db connections represented by instances of class `Database`](./framework/src/dbinterface/dbinterface.h) from one processing module to another through the processing pipeline.
- Thread safe database connection pooling
- **Monitoring** capabilities should be extended beyond whats already possible by dumping the `PipelineProcessingData` and friends to Json. At least at the end of each pipeline, it could make sense to automatically persist the corresponding payload object. How the json serialization works can be seen for instance in [test_httplistener.cpp](./framework/tests/test_httplistener.cpp) in `TEST(HTTPListener, sendsDataBackOnGetRequest)`.
- Make dependency to SQLite optional
