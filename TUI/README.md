# TUI tool for OpenFOAM class members

Currently this Golang utility talks to the [backend RESTful API](/applications/endpoint)
to fetch OpenFOAM class members (and supported classes).

It is also able to configure the reflection system to fetch skeletons
for concrete types instead of the base RTS ones if needed.

To compile and run:
```bash
../applications/endpoint/endpoint # Run this in another terminal or in bg
go build -o ./tui && ./tui
```
