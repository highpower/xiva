Summary:	Xiva (acronym for HTTP Extended EVent Automata) is a simple HTTP server.
Name:		xiva
Version:	0.6.3
Release:	1%{?dist}

License:	Yandex License
Group:		System Environment/Libraries
Source:		%{name}-%{version}.tar.bz2
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:	boost-devel
BuildRequires:	automake, autoconf, libtool
BuildRequires:	pkgconfig
BuildRequires:  asio-devel
BuildRequires:  python-devel
BuildRequires:  libxml2-devel


%description
Core package


%package	devel
Summary:	Development environment for %{name}
Group:		System Environment/Libraries
Requires:	%{name} = %{version}-%{release}

%description    devel
Devel for %{name}

%package        daemon
Summary:        Daemon for %{name}
Group:          System Environment/Libraries
Requires:       %{name} = %{version}-%{release}

%description	daemon
Daemon for %{name}


%package        utils
Summary:        Utilities for %{name}
Group:          System Environment/Libraries
Requires:       %{name} = %{version}-%{release}

%description    utils
Utilities for %{name}


%package        syslog
Summary:        Syslog for %{name}
Group:          System Environment/Libraries
Requires:       %{name} = %{version}-%{release}

%description    syslog
Syslog for %{name}


%package        stdin-reader
Summary:        Stdin-reader for %{name}
Group:          System Environment/Libraries
Requires:       %{name} = %{version}-%{release}

%description    stdin-reader
Stdin-reader for %{name}


%prep
%setup -q


%build
./autogen.sh
%configure --enable-maintainer-mode --sysconfdir=/etc/xiva
make release=1


%check
make check


%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}
install -dD %{buildroot}%{_sysconfdir}/%{name}
install -m 755 daemon/xiva.conf.example %{buildroot}%{_sysconfdir}/%{name}


%clean
rm -rf %{buildroot}


%files
%defattr(-,root,root)
%{_libdir}/*.so.*


%files utils
%defattr(-,root,root)
%{_bindir}/xiva-util


%files devel
%defattr(-,root,root)
%{_includedir}/*
%{_libdir}/python2.4/site-packages/*
%{_libdir}/*.so
%{_libdir}/*.a
%{_libdir}/*.la
%{_libdir}/%{name}/*.so
%{_libdir}/%{name}/*.a
%{_libdir}/%{name}/*.la


%files daemon
%defattr(-,root,root)
%{_sbindir}/xiva-daemon
%{_sysconfdir}/%{name}/xiva.conf.example


%files syslog
%defattr(-,root,root)
%{_libdir}/%{name}/xiva-syslog*so.*


%files stdin-reader
%defattr(-,root,root)
%{_libdir}/%{name}/xiva-stdin-reader*.so.*


%changelog
* Thu Oct 29 2009 Arkady L. Shane <ashejn@yandex-team.ru> 
- initial yandex's rpm build
