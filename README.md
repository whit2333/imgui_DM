ImGuiDM
=======

**WORK IN PROGRESS** -- Not working experiment. 

EPICS Display Manager based on [Dear ImGui](https://github.com/ocornut/imgui)

Currently uses the EPICS example app which defines a circle. So if you start 
the ioc that provides $USER:circle:angle, $USER:circle:x, $USER:circle:y, etc. 
you should see those values in the gui. 

Useful: 

- https://github.com/mdavidsaver/cmake4epics


```
git clone ...
cd imguiDM
git submodule update --init --recursive
mkdir build && cd build
cmake ../.
make
make install
imgui_dm
```

The demo will search and timeout while it waits for some PV channels to open.  
If you have the example IOC running it will start up fast because it will find 
them.

## Process Variables 

The handling of PV is very much a work in progress.
Currently the `PVGetList` class  (in `src/epics`) will take a list of PVs and 
try to connect to them. If they are connected, their current values are read 
and cached. There is also a buffer which is filled everytime the memeber 
function `Poll()` is called. This saves a history (for plotting a real time 
strip chart).

TODO: On initialization look up the history from an archiver if it exists, and 
fill the PV buffer.






