# Reflections for OpenFOAM code

This repository hosts my ~~failed~~ attempt at a ~~perfect~~ header-only
reflections system for OpenFOAM code while we wait for the
[proposal](https://www.open-std.org/JTC1/SC22/WG21/docs/papers/2022/p1240r2.pdf)
to make it into the C++ standard. It'll be a while before that happens but
since concepts are now in the language, it's not too far into the future.

Main objectives of this library:
- Provide an automated mechanism to build dictionary schemas for OpenFOAM
  classes (And **JSON** endpoints). A schema represents a "standard" OpenFOAM
  dictionary with required keywords (and sub-dicts) to construct an object
  of the target type.
- Provide an automated mechanism to generate UI widgets for OpenFOAM classes.

Use cases:
- Cheaply generate UIs for OpenFOAM applications (Both in terms of developer
  effort, and performance overhead).
- Natively generate schemas (Both OpenFOAM dictionaries, and in JSON format)
  for use in Unit Testing and Ux generation.

> [!IMPORTANT]
> I discourage the use of reflections for **any other purpose** as they open
> a giant gate for all kinds of bugs. Think of all the bugs in Game Engines.

Reflections are done at compile-time with (near)zero runtime-cost through
the [refl-cpp library](https://github.com/veselink1/refl-cpp).

For a quick overview on how this effort started, the general goals and the current state; see
- [A reflection system for MeshFreeFoam - Part 1](https://foamscience.github.io/MeshFreeFoam-Docs/blog/2023/09/29/a-reflection-system-for-meshfreefoam-part-1/).
- [A reflection system for MeshFreeFoam - Part 2](https://foamscience.github.io/MeshFreeFoam-Docs/blog/2023/10/02/a-reflection-system-for-meshfreefoam-part-2/).
- [A reflection system for MeshFreeFoam - Part 3](https://foamscience.github.io/MeshFreeFoam-Docs/blog/2023/10/05/a-reflection-system-for-meshfreefoam-part-3/).

There is an [interactiveUI](src/interactiveUI) application to **illustrate**
basic usage of the reflection system by providing a JSON endpoint for few dummy OpenFOAM classes.
Also, you need C++20 for this to work
(not a hard requirement but I like concepts).



## Getting started

### Prerequisites

- A recent version of OpenCFD OpenFOAM
- C++20 compiler (For the dummy classes and the JSON endpoint generator)
- NodeJs v18 (For the sample Web UI)
- Golang v1.22+ (For the sample TUI)

All of the requirements are contained in the associated
[Apptainer container](build/reflections.def).

### Compiling the sample application

To compile the sample application:
```bash
git clone https://github.com/FoamScience/openfoam-reflections
cd openfoam-reflections
# Needs OpenFOAM to be sourced
./build.sh # Works on Ubuntu with NodeJs v18 and go installed
./interactiveUI/interactiveUI
# in another terminal, if you want access to the web UI at localhost:3000
npm start --prefix reflect-json-app/
# or, this if you want the TUI instead
./TUI/tui
```

For the mechanism to work for your code, you have to follow specific methods:
- A few macros need to be called for your base and child models
  (See [interactiveUI/baseModel](/interactiveUI/baseModel) as an example)
- Your `::New` function needs to have a dictionary as first argument.
- In general, your types need to satisfy concept constraints in
  [uiConcepts.H](/src/ui/uiConcepts.H) (Or the other way around, your call)
- If you wrap your class members in a `ui::member<...>`, descriptions, bounds 
  and default values will be picked up by the UI generator. Although this is
  not fully implemented.

### Apptainer containers

If you don't want to bloat your system but still would like to give this a try:
```bash
git clone https://github.com/FoamScience/openfoam-apptainer-packaging /tmp/of_tainers
git clone https://github.com/FoamScience/openfoam-reflections
cd openfoam-reflections
ansible-playbook /tmp/of_tainers/build.yaml --extra-vars="original_dir=$PWD" --extra-vars="@build/config.yaml"
# In one terminal (it's fine if there are some errors)
apptainer run containers/projects/reflections.sif "/opt/openfoam-reflections/interactiveUI/interactiveUI"
# In another terminal (and choose one of the options)
apptainer run containers/projects/reflections.sif "npm start --prefix /opt/openfoam-reflections/reflect-json-app"
# Then check localhost:3000 in a web browser
# Or if you prefer a terminal UI
apptainer run containers/projects/reflections.sif "/opt/openfoam-reflections/TUI/tui"
```
