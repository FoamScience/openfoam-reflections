# Reflections for OpenFOAM code

> Not suitable for production use; Well, this does nothing useful yet.

This repository hosts the (failed-attempt) at a ~~perfect~~ header-only reflections system for OpenFOAM code while we wait for the [proposal](https://www.open-std.org/JTC1/SC22/WG21/docs/papers/2022/p1240r2.pdf) to make it into the C++ standard. It'll be a while before that happens but since concepts are now in the language, it's not too far into the future.

Reflections are done compile-time with zero-runtime-cost through the [refl-cpp library](https://github.com/veselink1/refl-cpp).

For a quick overview on how this effort started and the general goals; see [this blog post](https://foamscience.github.io/MeshFreeFoam-Docs/blog/2023/09/29/a-reflection-system-for-meshfreefoam-part-1/).

For a recap of what's possible and what's not, see [this blog post](https://foamscience.github.io/MeshFreeFoam-Docs/blog/2023/10/02/a-reflection-system-for-meshfreefoam-part-2/).

There is an [interactiveUI](src/interactiveUI) application to illustrate basic usage of the reflection system. Also, you need C++20 (not a hard requirement but I like concepts).
