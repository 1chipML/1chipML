<html>
<p align="center">
  <img src="https://github.com/1chipML/artwork/blob/main/1chipML%2BLFAI%26Data.png" alt="1chipML logo and LF AI & Data Logo" width="300">
</p>

<h1 align="center">
  1chipML
</h1>

⚠️ **Archived Project**  
Due to inactivity, the 1ChipML project was archived in May 2025. The contents remain available for historical purposes.

![Lint, Build and Tests](https://github.com/1chipML/1chipML/actions/workflows/make.yml/badge.svg?style=flat)

[![CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/5704/badge)](https://bestpractices.coreinfrastructure.org/projects/5704)

## Introduction

1chipML is an open source library for basic numerical crunching and machine learning for microcontrollers.
As the Internet of Things and Edge Computing are becoming a ubiquitous reality, we need to a
reliable and open framework to use on limited and low power demanding hardware.
The expected benefits are endless and we expect 1chipML will play an essential role in enabling
this new computing revolution!

This library is implemented by team of developers who are refered to as maintainers or contributors,
depending on their actual role.
  
The 1chipML project is hosted as a Sandbox project at the [LF AI & Data Foundation](https://lfaidata.foundation).

## Installation and Usage

This package does not necessitate any installation, it is essentially a collection of independent
numerical and machine learning methods which target microcontrollers. Therefore, the installation
is going to depend on the hardware itself along with the software tools used (e.g., the compiler).

In any case, we do our best to ensure that the library can be compiled by the main compilers
provided by AVR and ARM microcontrollers (avr-gcc and Cortex-M compilers) by writing code that is
entirely in the ANSI C language.

Therefore, in order to use this library in practical cases, you need to select one or more of the
source files which can be found in the [src](./src) forlder. A very efficient way to start is by studying
the source files stored in the [tests](./tests) folder.

## Tests

Although the target of this library is represented by microcontrollers, it is possible to run
it on a common personal computer. This is twofold, on one hand it allows the user to have a
feeling about what this library can perform, on the other hand it allows the debug of codes
on a computer before deployment on microcontrollers.

To run the tests on computer, simply type the following command in a shell:

```bash
make test
```

## Prototypes

To illustrate the usage of each algorithm, some prototypes have been developed. Each prototype located in the [prototype](./prototype) folder brings to light how a specific algorithm can be used. 
We recommend you look at these prototypes to have a better understanding of how each algorithm can be used for development. Most of them have been implemented to work with the platform Arduino IDE and all have a unique README file to better understand their implementation. 
