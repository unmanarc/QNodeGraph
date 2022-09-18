# QNodeGraph 

Qt C++ Widget library for Node Graph
  
Author: Aaron Mizrachi (unmanarc) <aaron@unmanarc.com>   
Main License: LGPLv3   


***
## Functionality

This library provides a widget for directed and undirected 2D node graphics

***
## Installing packages (HOWTO)

- COPR (Fedora/OpenSUSE/etc):  
[![Copr build status](https://copr.fedorainfracloud.org/coprs/amizrachi/unmanarc/package/QNodeGraph/status_image/last_build.png)](https://copr.fedorainfracloud.org/coprs/amizrachi/unmanarc/package/QNodeGraph/)


### Simple installation guide for Fedora/RHEL to develop software:

To activate our repo's and download/install the software (for development):

In RHEL8/Fedora:
```bash
# Install EPEL Repo
dnf -y install 'dnf-command(config-manager)'
dnf config-manager --set-enabled powertools
dnf -y install epel-release

# Install unmanarc's copr
dnf copr enable amizrachi/unmanarc -y
# Install the required software:
dnf -y install libQNodeGraph-qt5-devel
```


***
## Compatibility

This library was tested so far in:

* OpenSUSE Tumbleweed
* Fedora Linux 35

### Overall Pre-requisites:

* C++17 Compatible Compiler
* Qt5 ot Qt6 (Widget+XML)

