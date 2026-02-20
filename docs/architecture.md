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

## Quick start

This is a short document presenting the architecture of the Seed-core.
How the project is conceived, moving form folder names and their contents to a more comprehensive view of the project.

> [!NOTE]
> The architecture is not perfect and will change serveral times.

## Tree

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

## Common

The common part of the project includes utilities for the entire project.
Whether it be functions managing memory, systems, or other aspects.
If a function is useful to multiple systems, it will be defined here.

## Hierarchy