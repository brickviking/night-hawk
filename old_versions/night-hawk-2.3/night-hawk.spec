Summary: Neat X droid-chasing game
Name: night-hawk
Version: 2.3
Release: 1
Copyright: GPL
Group: Amusements/Games
Source: http://www.sourceforge.net/projects/night-hawk/
BuildRoot: /var/tmp/%{name}-buildroot

%description
Great droid-chasing game in the tradition of ParaDroid.
Eliminate all droids from each ship.

%prep
%setup -q

%build
configure --prefix=/usr
make RPM_OPT_FLAGS="$RPM_OPT_FLAGS"

%install
make install

%clean
rm -rf $RPM_BUILD_ROOT

%files

