# Benchmarking CSPong

> [!WARNING]
> Archived and no longer maintained; kept for reference. Built in 2016 for my master's thesis against ChilliSource v1.5/1.6 and Visual Studio 2013/2015. The `automation` branch's engine submodule points at a Bitbucket repo that no longer exists, so that branch won't fully clone or build as-is.

## Overview

A fork of CSPong, the sample Pong game from [ChilliWorks/CSSamples](https://github.com/ChilliWorks/CSSamples), that I turned into a benchmark harness for my master's thesis at the University of Montana. It stresses ChilliSource's particle and UI systems so I could measure where they bottleneck.

There are two branches:

- **`master`** still looks and feels like a game, with a UI. Its engine submodule points at my [engine fork](https://github.com/angiebrr/um-thesis-chillisource-engine), which has the Shiny profiling and metrics system.
- **`automation`** plays the game on its own, stepping through particle counts and emission settings, and writes metrics to CSV. This is the branch that produced the thesis data, run on Windows, iOS, and Android.

**Tech:** C++, ChilliSource, Shiny, Visual Studio, Android, iOS

Related repos:

- [um-thesis-particle-optimization](https://github.com/angiebrr/um-thesis-particle-optimization): the thesis, results, and dataset
- [um-thesis-chillisource-engine](https://github.com/angiebrr/um-thesis-chillisource-engine): the engine fork this game runs on
- [um-thesis-hdf5-data-packer](https://github.com/angiebrr/um-thesis-hdf5-data-packer): turns the CSVs this game writes into the HDF5 dataset

## Links

- ChilliWorks [website](http://chilli-works.com/)
- ChilliSource [repository](https://github.com/ChilliWorks/ChilliSource)
