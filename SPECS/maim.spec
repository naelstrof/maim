Name:           maim
Version:        5.5.2
Release:        1%{?dist}
Summary:        maim (make image) takes screenshots of your desktop. It has options to take only a region, and relies on slop to query for regions. maim is supposed to be an improved scrot.`

License:        GPLv3
URL:            https://github.com/naelstrof/%{name}
Source0:        https://github.com/naelstrof/%{name}/archive/v%{version}.tar.gz

BuildRequires:  libX11-devel libXfixes-devel libXrandr-devel imlib2-devel libslopy-devel cmake libjpeg-turbo-devel glm-devel libXcomposite-devel mesa-libGLU-devel
BuildRequires:  gcc gcc-c++

%description
maim (Make Image) is a utility that takes screenshots of your
desktop. It's meant to overcome shortcomings of scrot and performs
better in several ways.

%prep
%autosetup -n %{name}-%{version}


%build
%cmake .
%make_build

%install
%make_install

%files
%doc COPYING README.md
%{_bindir}/%{name}
%{_mandir}/man1/%{name}.1.gz

%changelog
* Sun Sep 30 2018 Dom Rodriguez <shymega@shymega.org.uk> - Initial commit.
- Inital version of the RPM spec for maim.
