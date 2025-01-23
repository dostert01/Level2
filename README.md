# LaForge
- [LaForge](#laforge)
  - [Preface](#preface)
  - [What it is](#what-it-is)
    - [The main building blocks in a nutshell](#the-main-building-blocks-in-a-nutshell)
      - [Runtime](#runtime)
      - [Business Objects](#business-objects)
      - [Business Logic](#business-logic)
  - [Technical Implementation](#technical-implementation)
    - [Provided Modules](#provided-modules)
  - [How to build it](#how-to-build-it)
    - [Optional External Dependencies](#optional-external-dependencies)
      - [MQTT](#mqtt)
  - [How to use](#how-to-use)
  - [Current bucket list](#current-bucket-list)

## Preface

This software is still in a very early state. This means, that not all features, that are described in the current documentation, are already (fully) implemented and available. In particular, being a framework, this project requires real life use cases, which are also not implemented yet. However, it is even more important to mention them already now, to be able to describe the big picture and the purpose of this project. This way, I'd like to share the idea of what is already possible and what might be missing.

In case you are interested in participating and/or contributing, both would be very welcome and appreciated. But of course I am also happy if you simply use the project for your own purposes :-).

## What it is

LaForge is an event driven business application framework written in C++. Its main idea is to keep **Business Objects**, **Business Logic** and **Runtime** separated from each other to allow

- the runtime to be scalable
- business objects to be defined independent of the runtime and the business logic
- the business logic to be defined as part of the configuration allowing declarative description of the logic
- deployment of business logic to be independent of the software deployment process
- to build any kind of message oriented middleware and dialog based business (web) applications on top of it.

### The main building blocks in a nutshell

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

The smallest building blocks of business logic inside the framework are business logic modules. These modules themselves implement the IPO pattern. They are based on a simple API, what allows modular extensibility of the system's capabilities in a way, that is independent of development and release cycle of the framework itself.

Business logic modules can be chained together into pipelines. This is, where the state (respectively the meta information) of a payload and its business object comes into play. Each pipeline can have a set to matching patterns attached. These patterns are matched against the state of any incoming payload. Once a pipeline's matching patters match a payload, this particular payload will be processed by the corresponding pipeline. During the processing of a payload, a pipeline or more precise the contained business logic modules can apply any kind of changes to the payload and its state information. This can either cause the payload to be processed by subsequent matching pipelines or being handed over to external systems.

## Technical Implementation

### Provided Modules

- **Process Pipelining** allows declarative definition of processing pipelines. Such pipelines consist of a series of business logic specific modules, that can be chained together. As pipelines are defined declarative, they can be changed and extended at runtime, making it quick and easy to react to changing business requirements.
  
- **Connectors** to external systems using local filesystem, MQTT and HTTP (coming soon).Any further protocol is possible and can be implemented as business logic module even outside of the framework.

- Event **listeners** for local filesystem, MQTT and HTTP (coming soon). Any further protocol is possible but currently must be implemented as part of the framework.

- **API** for developing business logic modules and connectors. Processing pipelines can be extended by making use of a **lean API**, that allows quick implementation of new business logic modules. For further details refer to the [Pipeline Implementation Part of the Framework](./framework/src/pipeline/README.md)

- Generic **SQL Database Interface** to enable processing pipelines to communicate with any RDBMS. Currently SQLite is implemented as the first one. See [here](./framework/src/dbinterface/README.md) how to use the existing implementation and to extend for the support of further RDBMSs.

- Formatted and filtered **Logging** to any destination. Currently implemented are logging destinations for `stdout`, `stderr`, `file` and `syslog`. See [here](./framework/src/logger/README.md) for details.

- **Application Context** covers thread save access to environmental information about the application and the main configuration of the application as such. Modules of the application (currently the listeners) can be bootstrapped using the `ApplicationContext`.

## How to build it

After cloning the repo cd into the root of the repo and do the following:

- Build the project and execute all unit tests by executing
  
    `cmake . && make && ctest -V`

    This will automatically execute `getDependencies.sh` and download  `googletest` to `_deps/googletest/` from where `cmake`  will trigger its build.

    Further on, [CMakeLists.txt](./CMakeLists.txt) will download [nlohmann/json](https://github.com/nlohmann/json) and make it available to the project build.


### Optional External Dependencies

#### MQTT

To build with support for MQTT, it is required to have [Eclipse Mosquitto](https://mosquitto.org/) and its development packages installed. [CMakeLists.txt](./framework/CMakeLists.txt) uses `find_package(Mosquito REQUIRED)` to make it available for the build process. However, LaForge can still be built without this dependency. MQTT support will be excluded from the build if Mosquitto can not be found.

For successful execution of MQTT related unit tests in [test_mqttconnector.cpp](./framework/tests/test_mqttconnector.cpp) and [test_mqttlistener.cpp](./framework/tests/test_mqttlistener.cpp) it is required to have an MQTT broker listening on port 1883 on the loopback interface.

## How to use

Please find one first sample application [here](./sampleApplications/README.md). (Work in Progress ;-) )

## Current bucket list

- A first **sample application** as a proof of concept and showcase
- **Monitoring** or at least the opportunity to provide data for monitoring the internal state of pipeline is required. A starting point for this has already been made by enabling the payload in form of instances of `PipelineProcessingData` and friends to be serialized to Json. at least at the end of each pipeline, it could make sense to automatically persist the corresponding payload object.
- **HTTP server and client**. Especially the server should be implemented without the use of further external dependencies. Currently is appears desirable to have a home grown implementation for that. After providing more documentation and implementing the first sample application, this will be definitely one of the upcoming tasks to do. Short term goal is, to have REST on board.

