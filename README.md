# Reflections for OpenFOAM code

This repository hosts my ~~failed~~ attempt at a ~~perfect~~ header-only
reflections system for OpenFOAM code while we wait for this
[proposal](https://www.open-std.org/JTC1/SC22/WG21/docs/papers/2022/p1240r2.pdf)
to make it into the C++ standard. It'll be a while before that happens but
since concepts are now in the language, it's not too far into the future.

The **main** objectives of this library are:
- Provide an _automated_ mechanism to build dictionary skeletons for OpenFOAM
  types. A skeleton represents a "standard" OpenFOAM dictionary with required
  with required keywords (and sub-dicts) to construct an object of the target type
  in its "standard" state, i.e. with default values for its members.
- Provide an _automated_ mechanism to generate UI widgets for OpenFOAM types, preferably
  supporting hints on the type, a description and default value of class members.

Use cases:
- Cheaply generate UIs for OpenFOAM applications (Both in terms of developer
  effort, and performance overhead).
- Natively generate skeletons (Both OpenFOAM dictionaries, and in JSON format)
  for use in Unit Testing and UX generation as well as class diagramming.

> [!IMPORTANT]
> I discourage the use of reflections for **any other purpose** as they open
> a giant gate for all kinds of bugs. Think of all the bugs in Game Engines.

Reflections are done at compile-time with (near)zero runtime-cost through
the [refl-cpp library](https://github.com/veselink1/refl-cpp).

For a quick overview on how this effort started, the general goals and the current state; see
- [A reflection system for MeshFreeFoam - Part 1](https://foamscience.github.io/MeshFreeFoam-Docs/blog/2023/09/29/a-reflection-system-for-meshfreefoam-part-1/).
- [A reflection system for MeshFreeFoam - Part 2](https://foamscience.github.io/MeshFreeFoam-Docs/blog/2023/10/02/a-reflection-system-for-meshfreefoam-part-2/).
- [A reflection system for MeshFreeFoam - Part 3](https://foamscience.github.io/MeshFreeFoam-Docs/blog/2023/10/05/a-reflection-system-for-meshfreefoam-part-3/).

There is an [endpoint](src/endpoint) application to **illustrate**
basic usage of the reflection system by providing a JSON endpoint for few dummy OpenFOAM classes.
Also, you need C++20 for this to work (not a hard requirement but I like concepts).

## Getting started

### Prerequisites

- A recent version of OpenCFD OpenFOAM
- C++20 compilers (For the dummy classes and the JSON endpoint generator)
- Golang v1.22+ (For the sample TUI)

All of the requirements are contained in the associated
[Apptainer container](build/reflections.def).

### Compiling the sample application

To compile the sample application:
```bash
git clone https://github.com/FoamScience/openfoam-reflections
cd openfoam-reflections
# Needs OpenFOAM to be sourced
./build.sh # Works on Ubuntu with go installed
./applications/endpoint/endpoint
# in another terminal, 
./TUI/tui
```

There is a minimal set of requirements your code needs to satisfy so you can
use the reflection system:
- A few macros need to be called for your base and child model classes.
  [src/testlib](src/testlib) illustrates what macros need to be called for
  each level of reflection support:
  1. `lvl0Support`: these are "vanilla" OpenFOAM classes, and they are minimally
     supported through wrappers. You can get protected members and their types,
     but with compiler-defaults for members. The provided example showcases an RTS-enabled
     templated class system.
  1. `lvl1Support`: these are types with some support for the reflection mechanism, but
     the outcome is the same as the `lvl1Support` ones.
  1. `lvl2Support`: these are types with full support for the reflection mechanism so
     you can get member types, descriptions, min/max if set, and even "default" values
     which would be used in a default construction of the type!
- Your static `::New` method from base classes needs to take a dictionary as first
  argument (and never to be overloaded).
- In general, your types need to satisfy concept constraints in
  [reflectConcepts.H](src/reflector/reflectConcepts.H) (Or the other way around, your call)

### Apptainer containers

If you don't want to bloat your system but still would like to give this a try:
```bash
git clone https://github.com/FoamScience/openfoam-apptainer-packaging /tmp/of_tainers
git clone https://github.com/FoamScience/openfoam-reflections
cd openfoam-reflections
ansible-playbook /tmp/of_tainers/build.yaml --extra-vars="original_dir=$PWD" --extra-vars="@build/config.yaml"
# In one terminal (it's fine if there are some errors)
apptainer run containers/projects/reflections.sif "/opt/openfoam-reflections/endpoint/endpoint"
# In another terminal
apptainer run containers/projects/reflections.sif "/opt/openfoam-reflections/TUI/tui"
```
