ImGuiDM
=======

**WORK IN PROGRESS**

EPICS Display Manager based on [Dear ImGui](https://github.com/ocornut/imgui)

Currently uses the EPICS example app which defines a circle. So if you start 
the ioc that provides $USER:circle:angle, $USER:circle:x, $USER:circle:y, etc. 
you should see those values in the gui. 

Useful: 

- https://github.com/mdavidsaver/cmake4epics


```
git clone ...
cd imguiDM
mkdir build && cd build
cmake ../.
make
```
