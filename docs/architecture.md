# Architecture of the Seed-core

> To easily navigate the project, and above all improve it.

## Table of contents

- [Quick start](#quick-start)
- [Tree](#tree)
- [Headers](#headers)
- [Common](#common)
- [Hierarchy](#hierarchy)
- [Systems](#systems)
- [Tests](#tests)

---

> [!NOTE]
> The architecture is not perfect and will change serveral times.

## Quick start

This is a short document presenting the architecture of the Seed-core.
How the project is conceived, moving form folder names and their contents to a more comprehensive view of the project.

## Tree

A brief overview of the project. 
```C
.
├── docs	// Documentation
├── includes	// Headers
├── src	// The src content
│   ├── common	// Content common to several sections
│   ├── core	// Project core
│   ├── systems	// All project systems
├── tests	// Units tests
```

---

## Headers

All headers are contained in the `includes` folder.

The `seed.h` file is the only file that will be included by user.
If possible, create as few headers as possible, but separation is the watchword.

For example, a complet system may contain internal system, a command system and an entry point, each of which must have its own header.

---

## Common

The common part of the project includes utilities for the entire project.
Whether it be functions managing memory, systems, or other aspects.
If a function is useful to multiple systems, it will be defined here.

---

## Hierarchy

The seed core has a structured hierarchy.
The manager manage all the system, no leak possible if it was correctly used.

- Manager
▼
- Dispatcher
▼
- Any systems

---

## Systems

The core will be contains multiple systems, now it contains only 2 systems.
- The first system is the `writing system` that manage buffers, lines and content with UNICODE / UTF-8.
- The second system is the `filesystem` that manage advanced VFS and os actions.

---

## Tests

The test folder contains all units tests. You can test it with `make test TARGET=<system>`