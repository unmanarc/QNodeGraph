%define bname QNodeGraph
%define name lib%{bname}-qt5
%define version 1.0.0
%define build_timestamp %{lua: print(os.date("%Y%m%d"))}

Name:           %{name}
Version:        %{version}
Release:        %{build_timestamp}.git%{?dist}
Summary:        Qt5 Node Graph Widget
License:        LGPLv3
URL:            https://github.com/unmanarc/QNodeGraph
Source0:        https://github.com/unmanarc/QNodeGraph/archive/main.tar.gz#/%{bname}-%{version}-%{build_timestamp}.tar.gz
Group:          Development/Libraries

%if 0%{?rhel} == 8
%define debug_package %{nil}
%endif

%if 0%{?rhel} == 9
%define debug_package %{nil}
%endif

%if 0%{?fedora} >= 33
%define debug_package %{nil}
%endif

BuildRequires:  libqt5-qtbase-common-devel libQt5Widgets-devel libQt5Xml-devel gcc-c++
Requires: libQt5Widgets5 libQt5Xml5

%description
Library for node graphs display widget for Qt

%package devel
Summary:        Library for node graphs display widget for Qt development files
Group:          Development/Libraries
Provides:       %{name}-devel
Requires:       %{name}

%description devel
This package contains necessary header files for %{name} development.

%prep
%autosetup -n %{bname}-main
gcc --version
g++ --version
qmake-qt5 --version
qmake-qt5 . CONFIG+=release PREFIX=%{_prefix}

%build
make %{?_smp_mflags}

%clean
rm -rf $RPM_BUILD_ROOT

%install
rm -rf $RPM_BUILD_ROOT
export INSTALL_ROOT="%{buildroot}"
make install

%files
%doc
%{_libdir}/libqnodegraph-qt5*

%files devel
/usr/include/qnodegraph-qt5/*
/usr/lib64/pkgconfig/qnodegraph-qt5.pc

%changelog
