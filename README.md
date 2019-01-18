# NeSTiNg - Network Simulator for Time-sensitive Networking

**Updated to Inet 4.0.0, will be merged into master branch soon!**

NeSTiNg is a simulation model for Time-sensitive Networking (TSN) using the OMNeT++ discrete event simulation framework.
Our model uses the [INET framework](https://inet.omnetpp.org/) and enhances it by TSN-capable components.
The model was initially developed by a group of students during a curricular project and is continously extended at the [Distributed Systems group of IPVS, University of Stuttgart](https://www.ipvs.uni-stuttgart.de/abteilungen/vs/)

## Compatibility

The current version of the NeSTiNg simulation framework has been tested with OMNeT++ version 5.4 and INET version 4.0.0 under Linux.

## Getting Started

+ Follow the instructions at [https://omnetpp.org/](https://omnetpp.org/) to download and install OMNeT++ version 5.4.
+ Make sure OMNeT++ is in your `PATH` by changing into the OMNeT++ directory and sourcing the `setenv` script.
+ `cd` into an arbitrarily named, preferably empty `<workspace>` directory.
+ Clone this repository, i.e.

```
  $ git clone https://gitlab.com/ipvs/nesting.git
```

+ Download and unpack INET version 4.0.0 or clone the INET repository and checkout tag v4.0.0, i.e.

```
  $ git clone https://github.com/inet-framework/inet.git
  $ git checkout -b v4.0.0 v4.0.0
```
  Alternatively, as space-saving one-liner
```
  $ git clone --branch v4.0.0 --depth 1 https://github.com/inet-framework/inet.git
```

+ Your directory should now look like this:

```
  <workspace>
  ├── nesting
  └── inet
```

### Importing and building from the OMNeT++ IDE

+ Start the OMNeT++ IDE, either from your desktop environment or by calling `omnetpp` from the terminal.
+ Set your previously chosen `<workspace>` directory as the workspace and launch the IDE.
+ Import both `nesting` and `inet` into your workspace:
	- Select `File -> Import...` from the menu.
	- Select `General -> Existing Projects into Workspace` and click `Next >`.
	- Under `Select root directory:`, choose the `<workspace>` directory.
	- Both project folders should now appear and be checked under `Projects`.
	- Click `Finish` and wait for the indexer to complete.
+ Build both INET and NeSTiNg by right-clicking on the corresponding project folder and selecting `Build Project`.
  (You can switch between the `release` and `debug` configuration in the same context menu under `Build Configurations -> Set Active`)
+ You can now run the supplied example simulation:
	- Navigate into `nesting > simulations` in the project explorer.
	- Right-click `example.ini` and choose `Run As` (or `Debug As` depending on the build configuration) `OMNeT++ Simulation`.
	- After a possible build step you will be presented with a graphical interface showing the simulation model.

### Building from the terminal

If you want to use NeSTiNg without the IDE or using OMNeT++ core, you can also build INET and NeSTiNg and run simulations from the terminal. (By default, the release versions of both projects will be built. To build the debug version, call `MODE=debug make` instead of `make` below. NeSTiNg requires the INET library to be built with the same configuration.)

+ To build INET:

```
  $ cd inet
  $ make makefiles
  $ make
```

+ To build NeSTiNg (both the library and the simulation executable):

```
  $ cd ../nesting
  $ make makefiles
  $ make
```

+ To run the example simulation, change to the `nesting/simulations` directory and call one of the following

```
  $ ./runsim example.ini                # run simulation without graphical interface (release)
  $ ./runsim-qt example.ini             # run simulation with the Qt interface (release)
  $ MODE=debug ./runsim example.ini     # run simulation without graphical interface (debug)
  $ MODE=debug ./runsim-qt example.ini  # run simulation with the Qt interface (debug)
```


## Contribution
Institute of Parallel and Distributed Systems  
University of Stuttgart  
[https://www.ipvs.uni-stuttgart.de/abteilungen/vs/](https://www.ipvs.uni-stuttgart.de/abteilungen/vs/)

