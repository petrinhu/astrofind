# Release packaging scripts

`.github/workflows/release.yml` only calls these scripts, so every package can be
rebuilt locally in the same container image the CI uses. Each build script runs as
root inside its target distribution, starts from the release source tarball
(never from the git checkout) and runs the core unit tests before packaging.

| Script | Container | Produces |
|---|---|---|
| `source-tarball.sh <outdir>` | host (needs git) | `astrofind-X.Y.Z-source.tar.gz` |
| `build-rpm.sh <tarball> <outdir>` | `fedora:44`, `rockylinux:9`, `opensuse/tumbleweed` | `astrofind-X.Y.Z-1.fc44.x86_64.rpm`, `…el9…`, `…opensuse-tumbleweed…` |
| `build-deb.sh <tarball> <outdir>` | `ubuntu:24.04`, `debian:13` | `astrofind_X.Y.Z-1~ubuntu24.04_amd64.deb`, `~debian13` |
| `build-arch.sh <tarball> <outdir>` | `archlinux:latest` | `astrofind-X.Y.Z-1-x86_64.pkg.tar.zst` |
| `build-appimage.sh <tarball> <outdir>` | `debian:12` | `AstroFind-X.Y.Z-x86_64.AppImage` (glibc ≥ 2.36) |
| `smoke-test.sh <file>` | a *fresh* container of the file's distribution | installs it with the native package manager and starts `AstroFind` headless |
| `version.sh` | anywhere | the version from `CMakeLists.txt` |

Example (Debian 13 package):

```bash
packaging/ci/source-tarball.sh dist
docker run --rm -v "$PWD":/src:ro -v "$PWD/dist":/dist debian:13 \
    /src/packaging/ci/build-deb.sh /dist/astrofind-1.2.0-source.tar.gz /dist/out
docker run --rm -v "$PWD":/src:ro -v "$PWD/dist":/dist debian:13 \
    /src/packaging/ci/smoke-test.sh /dist/out/astrofind_1.2.0-1~debian13_amd64.deb
```

## Making a release

1. Set the version in `CMakeLists.txt` (`project(... VERSION X.Y.Z)`) and the same
   number in `packaging/rpm/astrofind.spec` (`Version:` + `%changelog`),
   `packaging/arch/PKGBUILD` (`pkgver`), `packaging/debian/changelog` (new top entry
   `X.Y.Z-1`) and `packaging/install.sh` (`VERSION`). The workflow's first job fails
   if any of them differs.
2. Rename `## [Unreleased]` in `CHANGELOG.md` to `## [X.Y.Z] - date`: that section
   becomes the release notes.
3. Merge to `main`, then push the tag: `git tag vX.Y.Z && git push origin vX.Y.Z`.
   The workflow builds, smoke-tests and publishes the GitHub Release with all files,
   `SHA256SUMS` and a CycloneDX SBOM of the FetchContent dependencies.
