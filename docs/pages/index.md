---
keywords: Summary
tags: []
sidebar: kratos_for_users
permalink: index.html
title: 
summary: 
---

<p align=center><img height="72.125%" width="72.125%" src="https://raw.githubusercontent.com/KratosMultiphysics/Documentation/master/Wiki_files/Home/kratos.png"></p>

_KRATOS Multiphysics_ ("Kratos") is a framework for building parallel, multi-disciplinary simulation software, aiming at modularity, extensibility, and high performance. Kratos is written in C++, and counts with an extensive Python interface. More in [Overview](https://github.com/KratosMultiphysics/Kratos/wiki/Overview)

**Kratos** is **free** under BSD-4 [license](https://github.com/KratosMultiphysics/Kratos/blob/master/kratos/license.txt) and can be used even in comercial softwares as it is. Many of its main applications are also free and BSD-4 licensed but each derived application can have its own propietary license.

# Main Features
**Kratos** is __multiplatform__ and available for __Windows, Linux__ (several distros) and __macOS__.

**Kratos** is __OpenMP__ and __MPI__ parallel and scalable up to thousands of cores.

**Kratos** provides a core which defines the common framework and several application which work like plug-ins that can be extended in diverse fields.

## Getting Started
{:.no_toc}
- Getting Kratos its easy with the pip packages:

```console
pip install KratosMultiphysics-all
```

- You can also check our Github page and Python Project

<div class=row>
<div class="col-md-6" style="text-align: center"><a href="https://pypi.org/project/KratosMultiphysics-all"><img style="width:96px !important;" src="https://cdn.jsdelivr.net/npm/@programming-languages-logos/python@0.0.0/python_256x256.png"></a></div>
<div class="col-md-6" style="text-align: center"><a href="https://github.com/KratosMultiphysics/Kratos"><img style="width:96px !important;" src="https://raw.githubusercontent.com/rdimascio/icons/master/icons/github.svg"></a></div>
</div>

- Or check our compilation guide for more advanced usages


## Main Applications:
{:.no_toc}
- [DEM](applications/DEMApplication) for cohesive and non cohesive spheric and non spheric particles simulation
- [Fluid Dynamics](applications/FluidDynamicsApplication/README.md) Provides 2D and 3D incompressible fluids formulation
- [Fluid Structure Interaction](applications/FSIApplication/README.md) for solution of different FSI problems
- [Structural Mechanics](applications/StructuralMechanicsApplication/README.md) Providing solution for solid, shell and beam structures with linear and nonlinear, static and dynamic behavior
- [Contact Structural Mechanics](applications/ContactStructuralMechanicsApplication/README.md) For contact problems used along the [Structural Mechanics application](applications/StructuralMechanicsApplication/README.md)

## Main Modules:
{:.no_toc}
- [Linear Solvers](applications/LinearSolversApplication/README.md)
- [Trilinos](applications/TrilinosApplication/README.md)
- [Metis](applications/MetisApplication/README.md)
- [Meshing](applications/MeshingApplication/README.md)

## Tutorials
{:.no_toc}
* [Running an example from GiD](https://kratosmultiphysics.github.io/Kratos/pages/Kratos/For_Users/Tutorials/Running_An_Example_From_GiD.html)
* [Kratos input files and I/O](https://kratosmultiphysics.github.io/Kratos/pages/Kratos/For_Users/Tutorials/Kratos_Input_Files_And_Io.html)
* [Data management](https://kratosmultiphysics.github.io/Kratos/pages/Kratos/For_Users/Tutorials/Data_Management.html)
* [Solving strategies](https://kratosmultiphysics.github.io/Kratos/pages/Kratos/For_Users/Tutorials/Solving_Strategies.html)
* [Manipulating solution values](https://kratosmultiphysics.github.io/Kratos/pages/Kratos/For_Developers/Tutorials/Manipulating-solution-values.html)
* [Multiphysics](https://kratosmultiphysics.github.io/Kratos/pages/Kratos/For_Users/Tutorials/Multiphysics_Example.html)

# Examples of use
Kratos has been used for simulation of many different problems in a wide variety of disciplines ranging from wind over singular building to granular domain dynamics. Some examples and validation benchmarks simulated by Kratos can be found [here](https://kratosmultiphysics.github.io/Examples/)

<span>
<img align="center" src="https://github.com/KratosMultiphysics/Examples/raw/master/fluid_dynamics/use_cases/barcelona_wind/resources/BarcelonaVelocityVector.png" width="288">
</span>
  <p>Barcelona Wind Simulation</p>
<br>

# Contributors
Organizations contributing to Kratos:

<table style="max-width:100%; width:100%">
  <tr>
    <td><img align="left" src="https://github.com/KratosMultiphysics/Documentation/raw/master/Wiki_files/Logos/CIMNE_logo.png" style="width:128px !important;"></td>
    <td style="vertical-align: middle;"><p>International Center for Numerical Methods in Engineering</p></td>
  </tr>
  <tr>
    <td><img align="left" src="https://github.com/KratosMultiphysics/Documentation/raw/master/Wiki_files/Logos/TUM_Logo.png" style="width:128px !important;"></td>
    <td style="vertical-align: middle;"><p>Chair of Structural Analysis<br>Technical University of Munich</p></td>
  </tr>
  <tr>
    <td><img align="left" src="https://github.com/KratosMultiphysics/Documentation/raw/master/Wiki_files/Logos/altair-sponsor-logo.png" style="width:128px !important;"></td>
    <td style="vertical-align: middle;"><p>Altair Engineering</p></td>
  </tr>
  <tr>
    <td><img align="left" src="https://github.com/KratosMultiphysics/Documentation/raw/master/Wiki_files/Logos/Deltares_logo.png" style="width:128px !important;"></td>
    <td style="vertical-align: middle;"><p>Deltares</p></td>
  </tr>
    <tr>
    <td><img align="left" src="https://github.com/KratosMultiphysics/Documentation/raw/master/Wiki_files/Logos/TUBraunschweig_logo.svg" style="width:128px !important;"></td>
    <td style="vertical-align: middle;"><p>Institute of Structural Analysis<br>Technische Universität Braunschweig</p></td>
  </tr>
    <tr>
    <td><img align="left" src="https://github.com/KratosMultiphysics/Documentation/raw/master/Wiki_files/Logos/logo_UNIPD.svg" style="width:128px !important;"></td>
    <td style="vertical-align: middle;"><p>University of Padova, Italy</p></td>
  </tr>
</table> 

# Our Users
Some users of the technologies developed in Kratos are:

<table style="width:100%">
  <tr>
    <td><img align="left" src="https://github.com/KratosMultiphysics/Documentation/raw/master/Wiki_files/Logos/AIRBUS_logo.png" style="width:128px !important;"></td>
    <td style="vertical-align: middle;"><p><p>Airbus Defence and Space<br>Stress Methods & Optimisation Department</p></p></td>
  </tr>
  <tr>
    <td><img align="left" src="https://github.com/KratosMultiphysics/Documentation/raw/master/Wiki_files/Logos/siemens_logo.png" style="width:128px !important;"></td>
    <td style="vertical-align: middle;"><p>Siemens AG<br>Corporate Technology</p></td>
  </tr>
  <tr>
    <td><img align="left" src="https://github.com/KratosMultiphysics/Documentation/raw/master/Wiki_files/Logos/onera_logo.png" style="width:128px !important;"></td>
    <td style="vertical-align: middle;"><p>ONERA, The French Aerospace Lab<br>Applied Aerodynamics Department</p></td>
  </tr>
</table> 

Looking forward to seeing your logo here!

# Special Thanks To
## In Kratos Core:
{:.no_toc}
- [Boost](http://www.boost.org/) for uBLAS
- [pybind11](https://github.com/pybind/pybind11) for exposing C++ to python
- [GidPost](https://www.gidhome.com/gid-plus/tools/476/gidpost/) providing output to [GiD](https://www.gidhome.com/)
- [AMGCL](https://github.com/ddemidov/amgcl) for its highly scalable multigrid solver
- [JSON](https://github.com/nlohmann/json) JSON for Modern C++
- [ZLib](https://zlib.net/) The compression library

## In applications:
{:.no_toc}
- [Eigen](http://eigen.tuxfamily.org) For linear solvers used in the [LinearSolversApplication](applications/LinearSolversApplication)
- [Trilinos](https://trilinos.org/) for MPI linear algebra and solvers used in [TrilinosApplication](applications/TrilinosApplication)
- [METIS](http://glaros.dtc.umn.edu/gkhome/views/metis) for partitioning in [MetisApplication](applications/MetisApplication/README.md)
- [CoSimIO](https://github.com/KratosMultiphysics/CoSimIO) for performing coupled simulations with external solvers within the [CoSimulationApplication](applications/CoSimulationApplication/README.md). The CoSimIO in Kratos uses the following libraries:
  - [Boost](http://www.boost.org/) for the `intrusive_ptr`
  - [filesystem](https://github.com/gulrak/filesystem) Header-only single-file std::filesystem compatible helper library, based on the C++17 specs
  - [asio](https://think-async.com/Asio/) for socket based interprocess communication

# How to cite Kratos?
Please, use the following references when citing Kratos in your work.
- [Dadvand, P., Rossi, R. & Oñate, E. An Object-oriented Environment for Developing Finite Element Codes for Multi-disciplinary Applications. Arch Computat Methods Eng 17, 253–297 (2010). https://doi.org/10.1007/s11831-010-9045-2](https://doi.org/10.1007/s11831-010-9045-2)
- [Dadvand, P., Rossi, R., Gil, M., Martorell, X., Cotela, J., Juanpere, E., Idelsohn, S., Oñate, E. (2013). Migration of a generic multi-physics framework to HPC environments. Computers & Fluids. 80. 301–309. 10.1016/j.compfluid.2012.02.004.](10.1016/j.compfluid.2012.02.004)
- [Vicente Mataix Ferrándiz, Philipp Bucher, Rubén Zorrilla, Suneth Warnakulasuriya, Riccardo Rossi, Alejandro Cornejo, jcotela, Carlos Roig, Josep Maria, tteschemacher, Miguel Masó, Guillermo Casas, Marc Núñez, Pooyan Dadvand, Salva Latorre, Ignasi de Pouplana, Joaquín Irazábal González, AFranci, Ferran Arrufat, riccardotosi, Aditya Ghantasala, Klaus Bernd Sautter, Peter Wilson, dbaumgaertner, Bodhinanda Chandra, Armin Geiser, Inigo Lopez, lluís, jgonzalezusua, Javi Gárate. (2024). KratosMultiphysics/Kratos: Release 10.1 (v10.1). Zenodo. https://doi.org/10.5281/zenodo.3234644](https://zenodo.org/records/6926179)

# Activity

![Git Repository Activity](https://repobeats.axiom.co/api/embed/925787aa734f5f16ad8524017c7feb20d1a0f317.svg)
