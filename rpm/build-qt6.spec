%define bname QNodeGraph
%define name lib%{bname}-qt6
%define version 1.0.0
%define build_timestamp %{lua: print(os.date("%Y%m%d"))}

Name:           %{name}
Version:        %{version}
Release:        %{build_timestamp}.git%{?dist}
Summary:        Qt6 Node Graph Widget
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

BuildRequires:  qt6-base-common-devel qt6-widgets-devel qt6-xml-devel gcc-c++
Requires: libQt6Widgets6 libQt6Xml6

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
qmake6 --version
qmake6 . CONFIG+=release PREFIX=%{_prefix} QMAKE_LFLAGS_RPATH=

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
%{_libdir}/libqnodegraph-qt6*

%files devel
/usr/include/qnodegraph-qt6/*
/usr/lib64/pkgconfig/qnodegraph-qt6.pc

%changelog
