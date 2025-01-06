# EventForge

## What it is

EventForge is just a simple event driven Business Application Framework written in C++ with a Focus on:

- Ability to run anywhere (on Linux) from small embedded systems to big cloud instances
- Small Memory Footprint
- As few external Dependencies a possible or rather reasonable
- Modularity
- Performance
- Ability to modify and add new Business Logic after Deployment

## Provided Modules

- Formatted and filtered **Logging** to any destination. Currently implemented are Logging destinations for `stdout`, `stderr`, `file` and `syslog`. See [here](./framework/src/logger/README.md) for Details.

- **Process Pipelining** allows declarative Definition of Process Pipelines. Such Pipelines consist of a series of Business Logic specific Modules, that can be chained together. As Pipelines are defined declarative, they can be changed and extended at runtime, making it quick and easy to react to changing business requirements. Processing Pipelines can be extended by making use of a **lean API**, that allows quick implementation of new Business Logic Modules. For further Details refer to the [Pipeline Implementation Part of the Framework](./framework/src/pipeline/README.md)

- Generic **SQL Database Interface** to enable Processing Pipelines to communicate with any RDBMS. Currently SQLite is implemented as the first one. See [here](./framework/src/dbinterface/README.md) how to use the existing implementation and to extend for the support of further RDBMSs.

- Connectors to external Systems using HTTP and MQTT (coming next)

- Event Receptors for HTTP and MQTT (coming soon)

## How to build it

After cloning the repo cd into the root of the repo and do the following:

* Download the only current dependency `googletest` by running
`./getDependencies.sh`
* Build the project and execute all unit tests by executing
`cmake . && make && ctest -V`

## How to use

Please find one first sample application [here](./sampleApplications/README.md). (Work in Progress ;-) )

