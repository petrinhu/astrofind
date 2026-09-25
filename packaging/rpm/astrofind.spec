Name:           astrofind
Version:        1.2.2
Release:        1%{?dist}
Summary:        Asteroid detection and astrometry for citizen science (IASC/MPC)
License:        AGPL-3.0-or-later
URL:            https://github.com/petrinhu/astrofind

# The release asset astrofind-%%{version}-source.tar.gz (top directory
# astrofind-%%{version}/), also produced by packaging/ci/source-tarball.sh.
Source0:        %{name}-%{version}-source.tar.gz

# One spec for Fedora, RHEL/Rocky/Alma 9 (EPEL + CRB) and openSUSE Tumbleweed;
# built by packaging/ci/build-rpm.sh in .github/workflows/release.yml.
# The build fetches spdlog, nlohmann/json, SEP and Catch2 with CMake
# FetchContent (pinned SHAs); CCfits comes from originals/ in the tarball.
# Library Requires are generated automatically from the binary's sonames.

BuildRequires:  cmake >= 3.22
BuildRequires:  git
BuildRequires:  desktop-file-utils
%if 0%{?suse_version}
BuildRequires:  ninja
BuildRequires:  gcc-c++
BuildRequires:  pkgconfig
BuildRequires:  qt6-base-devel >= 6.4
BuildRequires:  qt6-charts-devel
BuildRequires:  qt6-opengl-devel
BuildRequires:  qt6-openglwidgets-devel
BuildRequires:  qt6-tools-devel
BuildRequires:  qt6-linguist-devel
BuildRequires:  qtkeychain-qt6-devel
BuildRequires:  cfitsio-devel
BuildRequires:  fftw3-devel
BuildRequires:  libarchive-devel
BuildRequires:  libraw-devel
BuildRequires:  Mesa-libGL-devel
BuildRequires:  libxkbcommon-devel
# SQLite driver for the catalog cache (QSQLITE) and the X11 platform plugin
# are loaded at run time, so the soname scan cannot see them.
Requires:       qt6-sql-sqlite
Recommends:     qt6-imageformats
%else
BuildRequires:  ninja-build
%if 0%{?rhel} && 0%{?rhel} < 10
# C++23 (std::expected) needs GCC >= 12; EL9's system compiler is GCC 11.
BuildRequires:  gcc-toolset-13-gcc-c++
# The distribution flags load the annobin plugin, packaged separately for the toolset.
BuildRequires:  gcc-toolset-13-annobin-plugin-gcc
%else
BuildRequires:  gcc-c++
%endif
BuildRequires:  pkgconf-pkg-config
BuildRequires:  qt6-qtbase-devel >= 6.4
BuildRequires:  qt6-qtcharts-devel
BuildRequires:  qt6-qttools-devel
%if ! (0%{?rhel} && 0%{?rhel} < 10)
# Not in EPEL 9: on EL9 the API key is kept in the settings file (mode 0600)
# instead of the system keychain (CMake finds Qt6Keychain as optional).
BuildRequires:  qtkeychain-qt6-devel
%endif
BuildRequires:  cfitsio-devel
BuildRequires:  fftw-devel
BuildRequires:  libarchive-devel
BuildRequires:  LibRaw-devel
BuildRequires:  mesa-libGL-devel
BuildRequires:  libxkbcommon-devel
Recommends:     qt6-qtimageformats
%endif
# ZIP image sets are extracted with unzip(1).
Recommends:     unzip

%description
AstroFind detects and measures asteroids and other moving objects in
astronomical images (FITS, SER, XISF, TIFF/PNG, DSLR RAW, NASA PDS) and
writes ADES 2022 reports (XML and PSV) for the Minor Planet Center,
following the IASC / MPC workflow. Plate solving via astrometry.net or
ASTAP, star detection (SEP), aperture photometry, blink comparison,
moving-object detection and dark/flat/bias calibration. Bilingual
interface (English / Brazilian Portuguese).

%prep
%autosetup -n %{name}-%{version}

%build
%if 0%{?rhel} && 0%{?rhel} < 10
. /opt/rh/gcc-toolset-13/enable
%endif
# Plain cmake instead of the %%cmake macros, which differ between Fedora and
# openSUSE; the distribution's compiler and linker flags are passed through.
cmake -S . -B _build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=%{_prefix} \
    -DCMAKE_C_FLAGS="%{optflags}" \
    -DCMAKE_CXX_FLAGS="%{optflags}" \
    -DCMAKE_EXE_LINKER_FLAGS="%{?build_ldflags}" \
    -DCMAKE_SHARED_LINKER_FLAGS="%{?build_ldflags}"
cmake --build _build --parallel %{?_smp_build_ncpus}

%install
DESTDIR=%{buildroot} cmake --install _build

%check
desktop-file-validate %{buildroot}%{_datadir}/applications/astrofind.desktop
# Core unit tests (no display needed). The UI suite runs in the QA workflows.
_build/bin/astrofind_tests --reporter compact

%files
%license LICENSE NOTICE LICENSES
%doc README.md CHANGELOG.md
%{_bindir}/AstroFind
%{_datadir}/applications/astrofind.desktop
%{_datadir}/icons/hicolor/*/apps/astrofind.png

%changelog
* Fri Sep 25 2026 Petrus Silva Costa <petrinhu@yahoo.com.br> - 1.2.2-1
- No change for RPM distributions; the CachyOS package is built for the x86-64 baseline
* Fri Sep 25 2026 Petrus Silva Costa <petrinhu@yahoo.com.br> - 1.2.1-1
- Release file names of the .deb packages use "." instead of "~"
* Fri Sep 25 2026 Petrus Silva Costa <petrinhu@yahoo.com.br> - 1.2.0-1
- Audit waves E1-E4: hostile-input hardening, WCS/refraction/ephemeris/time
  fixes, NOTICE/LICENSES, SBOM
- One spec for Fedora, EL9 and openSUSE; library Requires generated
  automatically; AstroFind --version / --help
* Thu Sep 24 2026 Petrus Silva Costa <petrinhu@yahoo.com.br> - 1.1.0-1
- DSLR RAW loading via LibRaw (CR2/CR3/NEF/ARW/DNG/...) and NASA PDS3/PDS4 images
- Audit wave E1: spectrum/background/FFT memory-safety fixes, SEP leak patch applied
- 1.0.0 was the maturity level of 0.9.0 and was never tagged separately
* Sat Jul 11 2026 Petrus Silva Costa <petrinhu@yahoo.com.br> - 0.9.0-1
- Stable release: full internal security/robustness/correctness audit passed
- Fixed License field (was stale LicenseRef-PolyForm-NC-1.0; repo migrated to AGPL-3.0-or-later)
- Added missing BuildRequires/Requires: qtkeychain-qt6 (linked by binary, previously undeclared)
* Sun Mar 22 2026 Petrus Silva Costa <petrinhu@yahoo.com.br> - 0.9.0-0.beta1
- Beta release: all features complete, pending real-world FITS validation
- Version 0.9.0 bumped from 0.5.0
- Fixed: invert/flip in blink mode, theme CSS consistency, marking colors
- Added: DPI-proportional sizing (Theme::dp), unified marking color constants
- Fixed: 10 truncated PT-BR translations
