Name:           astrofind
# VERSION substituted by CI via sed
Version:        0.9.0
Release:        1%{?dist}
Summary:        Modern asteroid detection software for citizen science (IASC/MPC workflow)
License:        AGPL-3.0-or-later
URL:            https://codeberg.org/petrinhu/astrofind
BuildArch:      x86_64
# Disable auto-dependency detection — binary is pre-built and deps are explicit
AutoReqProv:    no

Requires:       qt6-qtbase >= 6.4
Requires:       qt6-qtcharts >= 6.4
Requires:       qt6-qt5compat >= 6.4
Requires:       qtkeychain-qt6
Requires:       cfitsio
Requires:       fftw >= 3.3
Requires:       libarchive >= 3.5
Requires:       mesa-libGL
Requires:       libxkbcommon

%description
AstroFind is modern asteroid detection software for citizen science and
educational use. Compatible with the IASC / MPC workflow. Outputs ADES 2022
reports (XML and PSV) for submission to the Minor Planet Center.

Stable release, first release after a full internal security/robustness/
correctness audit (29 issues found and fixed — see AUDIT_FIND.md upstream).

%prep
# Binary packaging only — no source extraction needed.

%build
# Binary packaging only — compiled by CI before this step.

%install
# staging is passed via: rpmbuild --define "staging /path/to/staged/files"
cp -a %{staging}/. %{buildroot}/

%files
%{_bindir}/AstroFind
%{_datadir}/applications/astrofind.desktop
%{_datadir}/icons/hicolor/16x16/apps/astrofind.png
%{_datadir}/icons/hicolor/32x32/apps/astrofind.png
%{_datadir}/icons/hicolor/48x48/apps/astrofind.png
%{_datadir}/icons/hicolor/64x64/apps/astrofind.png
%{_datadir}/icons/hicolor/128x128/apps/astrofind.png
%{_datadir}/icons/hicolor/256x256/apps/astrofind.png
%{_datadir}/icons/hicolor/512x512/apps/astrofind.png

%changelog
* Sat Jul 11 2026 Petrus Silva Costa <petrinhu@yahoo.com.br> - 0.9.0-1
- Stable release: full internal security/robustness/correctness audit passed
- Fixed License field (was stale LicenseRef-PolyForm-NC-1.0; repo migrated to AGPL-3.0-or-later)
- Added missing Requires: qtkeychain-qt6 (linked by binary, previously undeclared)
* Sun Mar 22 2026 Petrus Silva Costa <petrinhu@yahoo.com.br> - 0.9.0-0.beta1
- Beta release: all features complete, pending real-world FITS validation
- Version 0.9.0 bumped from 0.5.0
