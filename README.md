# Reflections for OpenFOAM code

This repository hosts my ~~failed~~ attempt at a ~~perfect~~ header-only reflections system for OpenFOAM code while we wait for the [proposal](https://www.open-std.org/JTC1/SC22/WG21/docs/papers/2022/p1240r2.pdf) to make it into the C++ standard. It'll be a while before that happens but since concepts are now in the language, it's not too far into the future.

Main objectives of this library:
- Provide an automated mechanism to build dictionary schemas for OpenFOAM classes. A schema represents a "standard" OpenFOAM dictionary with required keywords (and sub-dicts) to construct an object of the target type.
- Provide an automated mechanism to generate UI widgets for OpenFOAM classes.

> I discourage the use of reflections for **any other purpose** as they open a giant gate for all kinds of bugs. Think of all the bugs in Game Engines.

Reflections are done at compile-time with zero runtime-cost through the [refl-cpp library](https://github.com/veselink1/refl-cpp).

For a quick overview on how this effort started, the general goals and the current state; see
- [A reflection system for MeshFreeFoam - Part 1](https://foamscience.github.io/MeshFreeFoam-Docs/blog/2023/09/29/a-reflection-system-for-meshfreefoam-part-1/).
- [A reflection system for MeshFreeFoam - Part 2](https://foamscience.github.io/MeshFreeFoam-Docs/blog/2023/10/02/a-reflection-system-for-meshfreefoam-part-2/).
- [A reflection system for MeshFreeFoam - Part 3](https://foamscience.github.io/MeshFreeFoam-Docs/blog/2023/10/05/a-reflection-system-for-meshfreefoam-part-3/).

There is an [interactiveUI](src/interactiveUI) application to **illustrate** basic usage of the reflection system.
Also, you need C++20 for this to work (not a hard requirement but I like concepts).

## Getting started

To compile the sample application:
```bash
git clone https://github.com/FoamScience/openfoam-reflections
cd openfoam-reflections
# Needs OpenFOAM to be sourced
./build.sh # Works on Ubuntu with NodeJs v18 installed
npm start --prefix reflect-json-app/
# in another terminal, you need to fire the interactiveUI
./interactiveUI/interactiveUI
```

For the mechanism to work for you, you have to follow specific methods:
- A few macros need to be called for your base and child models (See [interactiveUI/baseModel](/interactiveUI/baseModel) as an example)
- Your `::New` function needs to have a dictionary as first argument.
- In general, your types need to satisfy concept constraints in [uiConcepts.H](/src/ui/uiConcepts.H) (Or the other way around, your call)

## Apptainer containers

If you don't want to bloat your system but still would like to give this a try:
```bash
git clone https://github.com/FoamScience/openfoam-apptainer-packaging /tmp/of_tainers

```
