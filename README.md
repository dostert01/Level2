# EventForge

## What it is

EventForge is just a simple event driven Business Application Framework written in C++ with a focus on:

- Ability to run anywhere (on Linux) from small embedded systems to big cloud instances
- Small memory footprint
- As few external dependencies as possible or rather reasonable
- Modularity
- Performance
- Ability to modify and add new business logic after deployment

## Provided Modules

- Formatted and filtered **Logging** to any destination. Currently implemented are logging destinations for `stdout`, `stderr`, `file` and `syslog`. See [here](./framework/src/logger/README.md) for details.

- **Process Pipelining** allows declarative definition of processing pipelines. Such pipelines consist of a series of business logic specific modules, that can be chained together. As pipelines are defined declarative, they can be changed and extended at runtime, making it quick and easy to react to changing business requirements. Processing pipelines can be extended by making use of a **lean API**, that allows quick implementation of new business logic modules. For further details refer to the [Pipeline Implementation Part of the Framework](./framework/src/pipeline/README.md)

- Generic **SQL Database Interface** to enable processing pipelines to communicate with any RDBMS. Currently SQLite is implemented as the first one. See [here](./framework/src/dbinterface/README.md) how to use the existing implementation and to extend for the support of further RDBMSs.

- Connectors to external Systems using HTTP and MQTT (coming next)

- Event Receptors for HTTP and MQTT (coming soon)

- **Application Context** covers thread save access to environmental information about the application

## How to build it

After cloning the repo cd into the root of the repo and do the following:

* Download the only current dependency `googletest` by running
`./getDependencies.sh`
* Build the project and execute all unit tests by executing
`cmake . && make && ctest -V`

## How to use

Please find one first sample application [here](./sampleApplications/README.md). (Work in Progress ;-) )

