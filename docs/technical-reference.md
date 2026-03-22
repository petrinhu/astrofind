# AstroFind — Technical Reference

**Algorithms, Coordinate Systems, and Numerical Methods**

> **Audience:** Contributors and advanced users who need to understand the mathematical
> underpinning of AstroFind's reduction pipeline.
> All equations use SI or astronomical units as noted.

---

## Table of Contents

1. [FITS File Structure](#1-fits-file-structure)
2. [Supported Image Formats](#2-supported-image-formats)
3. [WCS Pipeline — Pixel to Sky](#3-wcs-pipeline--pixel-to-sky)
4. [WCS Projections](#4-wcs-projections)
5. [Atmospheric Refraction](#5-atmospheric-refraction)
6. [Coordinate Chain: ICRS → CIRS → Topocentric](#6-coordinate-chain-icrs--cirs--topocentric)
7. [Annual Aberration](#7-annual-aberration)
8. [Precession and Nutation](#8-precession-and-nutation)
9. [Elliptical PSF Fitting (Levenberg-Marquardt)](#9-elliptical-psf-fitting-levenbergmarquardt)
10. [Aperture Photometry and Zero-Point](#10-aperture-photometry-and-zero-point)
11. [Differential Photometry](#11-differential-photometry)
12. [Airmass](#12-airmass)
13. [Julian Date](#13-julian-date)
14. [Ecliptic and Galactic Coordinates](#14-ecliptic-and-galactic-coordinates)
15. [FFT Image Registration (Phase Correlation)](#15-fft-image-registration-phase-correlation)
16. [Bad Pixel Correction](#16-bad-pixel-correction)
17. [Streak Detection by Blob Elongation](#17-streak-detection-by-blob-elongation)
18. [ClumpFind — Blended Source Separation](#18-clumpfind--blended-source-separation)
19. [Sliding-Median Background Estimator](#19-sliding-median-background-estimator)
20. [2-D Power Spectrum](#20-2-d-power-spectrum)
21. [ADES / MPC Report Format](#21-ades--mpc-report-format)

---

## 1. FITS File Structure

A FITS file consists of one or more **Header/Data Units (HDUs)**. The primary HDU is at offset 0;
additional extensions begin on 2880-byte block boundaries.

### Header

Keywords are 80-character records of the form `KEY     = value / comment`. AstroFind reads:

| Keyword | Purpose |
|---------|---------|
| `NAXIS`, `NAXIS1`, `NAXIS2`, `NAXIS3` | Image dimensions |
| `BITPIX` | Pixel data type (8, 16, 32, −32, −64) |
| `BSCALE`, `BZERO` | Physical value: $p = \text{BSCALE} \times \text{raw} + \text{BZERO}$ |
| `DATE-OBS` | ISO 8601 observation start (UTC) |
| `EXPTIME` / `EXPOSURE` | Integration time in seconds |
| `CRPIX1/2` | Reference pixel (1-based) |
| `CRVAL1/2` | Sky coordinate at reference pixel (degrees) |
| `CD1_1`, `CD1_2`, `CD2_1`, `CD2_2` | CD matrix (degrees/pixel) |
| `CTYPE1/2` | Projection type (e.g., `RA---TAN`, `DEC--TAN`) |
| `GAIN` | Electrons per ADU |
| `RDNOISE` | Read noise in electrons |

### Cube Handling

When `NAXIS3 > 3`, each plane $k$ is read with cfitsio `fpixel[3] = {1, 1, k+1}`. A
`cubeDepth` field stores $\text{NAXIS3}$. RGB images have `NAXIS3 = 3`.

---

## 2. Supported Image Formats

### SER

Binary video format. Header is 178 bytes:

```
Offset 14: ColorID (uint32) — 0=mono, 8=Bayer, 100=RGB
Offset 26: Width  (uint32)
Offset 30: Height (uint32)
Offset 34: PixelDepthPerPlane (uint32) — 8 or 16
Offset 38: FrameCount (uint32)
```

Frame data starts at offset 178. Each frame is `Width × Height × (PixelDepth/8)` bytes.
16-bit frames are little-endian.

### XISF

XML-based format (PixInsight). The file begins with an 8-byte magic `XISF0100`, followed by a
4-byte little-endian XML header length, then the XML block. The `<Image>` element carries:

```xml
<Image geometry="W:H:channels" sampleFormat="Float32" location="attachment:offset:size"/>
```

Pixel data at `offset` bytes from file start. AstroFind reads Float32 and UInt16.

---

## 3. WCS Pipeline — Pixel to Sky

The three-step Calabretta & Greisen (2002) pipeline:

**Step 1 — Pixel → Intermediate World Coordinates (IWC)**

$$
\begin{pmatrix} x \\ y \end{pmatrix} =
\begin{pmatrix} CD_{1,1} & CD_{1,2} \\ CD_{2,1} & CD_{2,2} \end{pmatrix}
\begin{pmatrix} p_1 - CRPIX_1 \\ p_2 - CRPIX_2 \end{pmatrix}
$$

where $(p_1, p_2)$ are 1-based pixel coordinates and $(x, y)$ are in degrees.

**Step 2 — IWC → Native Spherical Coordinates $(\phi, \theta)$**

Projection-specific (see §4). Result is native longitude $\phi$ and latitude $\theta$ in degrees.

**Step 3 — Native → Celestial (RA, Dec)**

Spherical rotation using the reference point $(\alpha_0, \delta_0) = (CRVAL_1, CRVAL_2)$
and the native latitude of the fiducial point $\theta_0$ (typically 90° for zenithal
projections):

$$
\alpha = \alpha_0 + \arctan\!\left(
  \frac{-\cos\theta\,\sin(\phi - \phi_p)}
       {\sin\theta\,\cos\delta_0 - \cos\theta\,\cos(\phi - \phi_p)\,\sin\delta_0}
\right)
$$

$$
\delta = \arcsin\!\left(
  \sin\theta\,\sin\delta_0 + \cos\theta\,\cos(\phi - \phi_p)\,\cos\delta_0
\right)
$$

where $\phi_p = 180°$ for $\delta_0 \geq \theta_0$, else $0°$.

---

## 4. WCS Projections

### TAN — Gnomonic (default)

$$
\phi = \arctan\!\left(\frac{x}{-y}\right), \quad
\theta = \arctan\!\left(\frac{180°}{\pi\,\sqrt{x^2 + y^2}}\right)
$$

Valid for small fields of view. Straight lines in the image correspond to great circles.

### SIN — Slant Orthographic

$$
\phi = \arctan\!\left(\frac{x}{-y}\right), \quad
\theta = \arccos\!\left(\frac{\pi\,\sqrt{x^2+y^2}}{180°}\right)
$$

Used by radio interferometry (VLBI images).

### ARC — Zenithal Equidistant

$$
R_\theta = \frac{180°}{\pi}\,(90° - \theta), \quad
\phi = \arctan\!\left(\frac{x}{-y}\right)
$$

Preserves great-circle distances from the projection centre.

### STG — Stereographic

$$
R_\theta = \frac{360°}{\pi}\,\tan\!\left(\frac{90° - \theta}{2}\right)
$$

Conformal; circles on the sphere map to circles in the plane.

### MER — Mercator

$$
x = \phi - \phi_0, \quad y = \frac{180°}{\pi}\,\ln\!\left(\tan\!\left(\frac{90°+\delta}{2}\right)\right)
$$

Cylindrical conformal. Used for full-sky survey maps.

### GLS / CAR — Global Sinusoidal / Plate Carrée

For GLS: $x = (\alpha - \alpha_0)\cos\delta$, $y = \delta - \delta_0$.
For CAR (special case $\delta_0 = 0$): equirectangular.

### AIT — Hammer-Aitoff

$$
z = \sqrt{1 + \cos\delta\,\cos(\alpha/2)}, \quad
x = \frac{2\cos\delta\,\sin(\alpha/2)}{z}, \quad
y = \frac{\sin\delta}{z}
$$

Equal-area full-sky projection. Used for CMB and survey coverage maps.

---

## 5. Atmospheric Refraction

The observed zenith angle $z'$ differs from the true zenith angle $z$ by:

$$
R = R_0 \cdot P/P_0 \cdot T_0/T \cdot \tan z
$$

where:
- $R_0 = 60.4''$ (standard refraction constant)
- $P$ = atmospheric pressure (default 1013.25 hPa)
- $T$ = temperature in Kelvin (default 283 K)
- $P_0 = 1013.25\,\text{hPa}$, $T_0 = 283\,\text{K}$

For elevations below 15°, the Bennett (1982) formula is used:

$$
R = \frac{1.02}{\tan\!\left(a + \frac{10.3}{a + 5.11}\right)}
$$

where $a$ is the true altitude in degrees, giving $R$ in arcminutes.

The correction is applied to each measured RA/Dec before writing to the ADES report.
The log panel shows the applied correction as `Refraction correction: X" (R=Y')`.

---

## 6. Coordinate Chain: ICRS → CIRS → Topocentric

AstroFind computes the apparent topocentric position of each observed object following
the IAU 2006/2000A precession-nutation model:

1. **ICRS** (catalog/WCS coordinates, epoch-independent)
2. **Apply precession** from J2000.0 to the date of observation (see §8)
3. **Apply nutation** to get the true equator and equinox of date
4. **CIRS** — add annual aberration (see §7)
5. **Topocentric** — parallax correction for the observer's geodetic position
6. **Apply atmospheric refraction** (see §5)

The ADES XML output tags coordinates with `<sys>ICRF</sys>` because AstroFind reports
the aberration-corrected ICRF-aligned position, consistent with the ADES 2017 standard.

---

## 7. Annual Aberration

The stellar aberration due to Earth's orbital velocity $\mathbf{v}_\oplus$:

$$
\Delta\alpha = \frac{1}{c}\left(-v_x\sin\alpha + v_y\cos\alpha\right) / \cos\delta
$$

$$
\Delta\delta = \frac{1}{c}\left(-v_x\cos\alpha\sin\delta - v_y\sin\alpha\sin\delta + v_z\cos\delta\right)
$$

where $v_x, v_y, v_z$ are the components of Earth's velocity in the equatorial frame,
derived from the Sun's ecliptic longitude $\lambda_\odot$:

$$
v_x = -\kappa\,\sin\lambda_\odot, \quad
v_y =  \kappa\,\cos\lambda_\odot\,\cos\varepsilon, \quad
v_z =  \kappa\,\cos\lambda_\odot\,\sin\varepsilon
$$

The constant of aberration $\kappa = 20.49552''$; $\varepsilon$ is the obliquity of the ecliptic.

The Sun's ecliptic longitude is computed from the Julian date via `sunEclipticPosition()`.

---

## 8. Precession and Nutation

**Precession from J2000.0 to epoch $t$** (IAU 1976, Lieske):

$$
\zeta_A = 2306.2181'' t + 0.30188'' t^2 + 0.017998'' t^3
$$
$$
z_A    = 2306.2181'' t + 1.09468'' t^2 + 0.018203'' t^3
$$
$$
\theta_A = 2004.3109'' t - 0.42665'' t^2 - 0.041775'' t^3
$$

where $t$ = Julian centuries from J2000.0. The rotation matrix is:

$$
P = R_z(-z_A)\,R_y(\theta_A)\,R_z(-\zeta_A)
$$

**Nutation** corrections $\Delta\psi$ (longitude) and $\Delta\varepsilon$ (obliquity) are
computed from the 1980 IAU series (106 terms). The mean obliquity:

$$
\varepsilon_0 = 84381.448'' - 46.8150'' t - 0.00059'' t^2 + 0.001813'' t^3
$$

---

## 9. Elliptical PSF Fitting (Levenberg-Marquardt)

The 2-D elliptical Gaussian model with 6 free parameters:

$$
I(x, y) = A\,\exp\!\left(-\frac{1}{2}\left[
  \left(\frac{x'(x,y)}{\sigma_x}\right)^2 +
  \left(\frac{y'(x,y)}{\sigma_y}\right)^2
\right]\right) + B
$$

where the rotated coordinates are:

$$
x'(x,y) = (x - x_0)\cos\theta + (y - y_0)\sin\theta
$$
$$
y'(x,y) = -(x - x_0)\sin\theta + (y - y_0)\cos\theta
$$

Parameters: amplitude $A$, centroid $(x_0, y_0)$, widths $(\sigma_x, \sigma_y)$,
rotation $\theta$, and sky background $B$.

The **Levenberg-Marquardt** iteration minimises:

$$
\chi^2 = \sum_{i,j}\frac{\left(I_\text{obs}(x_i, y_j) - I_\text{model}(x_i, y_j)\right)^2}{\sigma_{ij}^2}
$$

The update step blends gradient descent and Gauss-Newton:

$$
\left(J^T W J + \lambda\,\text{diag}(J^T W J)\right)\Delta\mathbf{p} = J^T W \mathbf{r}
$$

Convergence: $\|\Delta\mathbf{p}\| < 10^{-6}$ or 200 iterations. The FWHM values are:

$$
\text{FWHM}_{a,b} = 2\sqrt{2\ln 2}\,\sigma_{x,y}
$$

---

## 10. Aperture Photometry and Zero-Point

**Source flux** in an aperture of radius $r$:

$$
F_\text{src} = \sum_{(x,y): r_{xy} \leq r} I(x,y) - N_\text{ap}\,\bar{S}
$$

where $\bar{S}$ is the median sky per pixel estimated from an annulus
$(r_\text{in}, r_\text{out})$ and $N_\text{ap} = \pi r^2$ is the aperture area.

**Instrumental magnitude:**

$$
m_\text{inst} = -2.5\,\log_{10}(F_\text{src}\,/\,t_\text{exp})
$$

**Calibrated magnitude:**

$$
m_\text{cal} = m_\text{inst} + Z
$$

where the zero-point $Z$ is determined from reference stars in the field:

$$
Z = \langle m_\text{catalog,i} - m_\text{inst,i} \rangle
$$

The standard error of the zero-point:

$$
\sigma_Z = \frac{\text{MAD}(Z_i)}{0.6745\sqrt{N_\text{ref}}}
$$

---

## 11. Differential Photometry

For each science frame, a differential zero-point is computed from the stable reference
stars (those not flagged as variable, blended, or near the detection limit):

$$
Z_\text{diff} = \text{median}\!\left(m_\text{catalog,i} - m_\text{inst,i}\right)
$$

The uncertainty of the measured object's magnitude:

$$
\sigma_m = \sqrt{\sigma_Z^2 + \left(\frac{2.5}{\ln 10}\right)^2 \frac{F_\text{src} + N_\text{ap}\,\bar{S} + N_\text{ap}\,\sigma_\text{rn}^2/g^2}{F_\text{src}^2}}
$$

where $g$ = gain (e⁻/ADU) and $\sigma_\text{rn}$ = read noise (e⁻).

---

## 12. Airmass

For altitudes above 10°, the plane-parallel formula:

$$
X = \sec z = \frac{1}{\sin a}
$$

For lower altitudes, the Young & Irvine (1967) formula:

$$
X = \sec z \left(1 - 0.0012\,(\sec^2 z - 1)\right)
$$

where $z$ is the true zenith angle and $a$ is the true altitude.

The altitude is computed from the hour angle $H$, declination $\delta$, and
observer latitude $\varphi$:

$$
\sin a = \sin\varphi\,\sin\delta + \cos\varphi\,\cos\delta\,\cos H
$$

---

## 13. Julian Date

The Julian Date of a given UTC time $(Y, M, D, h, m, s)$:

$$
JD = 367Y - \lfloor 7(Y + \lfloor(M+9)/12\rfloor)/4 \rfloor
   + \lfloor 275M/9 \rfloor + D + 1721013.5
   + \frac{h + m/60 + s/3600}{24}
$$

Valid for dates after 1582-10-15 (Gregorian calendar).

The Modified Julian Date: $\text{MJD} = JD - 2400000.5$.

The Julian century from J2000.0: $t = (JD - 2451545.0) / 36525$.

---

## 14. Ecliptic and Galactic Coordinates

**Equatorial → Ecliptic** (mean ecliptic of date):

$$
\lambda = \arctan\!\left(\frac{\sin\alpha\,\cos\varepsilon + \tan\delta\,\sin\varepsilon}{\cos\alpha}\right)
$$
$$
\beta = \arcsin\!\left(\sin\delta\,\cos\varepsilon - \cos\delta\,\sin\varepsilon\,\sin\alpha\right)
$$

**Equatorial (J2000) → Galactic:**

The IAU 1958 galactic pole at $(\alpha_G, \delta_G) = (192.25°, 27.40°)$,
ascending node $l_\Omega = 33°$:

$$
b = \arcsin\!\left(\cos\delta\,\cos\delta_G\,\cos(\alpha - \alpha_G) + \sin\delta\,\sin\delta_G\right)
$$
$$
l = \arctan\!\left(\frac{\sin\delta - \sin b\,\sin\delta_G}{\cos\delta\,\sin(\alpha - \alpha_G)\,\cos\delta_G}\right) + l_\Omega
$$

AstroFind warns when $|b| < 10°$ because dense stellar fields near the Galactic plane
increase false-positive detection rates.

---

## 15. FFT Image Registration (Phase Correlation)

Sub-pixel translation between frames $f_1$ and $f_2$:

$$
G = \frac{F_1 \cdot \overline{F_2}}{|F_1 \cdot \overline{F_2}|}
$$

The cross-power spectrum is normalised (i.e., only phase information is retained).
The inverse FFT of $G$ gives a sharp delta-like peak at the shift vector $(\Delta x, \Delta y)$.

Sub-pixel accuracy is achieved by fitting a 2-D Gaussian to the peak:

$$
(\Delta x_\text{sub}, \Delta y_\text{sub}) = \arg\max \text{Gaussian fit to peak neighbourhood}
$$

AstroFind uses FFTW3's real-to-complex `r2c` plan for efficiency. Frames are
zero-padded to the next power-of-two size to avoid circular aliasing.

---

## 16. Bad Pixel Correction

For each pixel $(i, j)$ across a stack of $N$ calibrated frames:

1. Compute the local median $\tilde{p}$ and MAD $= \text{median}(|p_k - \tilde{p}|)$.
2. The normalised deviation is $s = |p_k - \tilde{p}| / (1.4826 \cdot \text{MAD})$.
3. Pixels with $s > \sigma_\text{threshold}$ (default $\sigma = 5$) are flagged.
4. Flagged pixels are replaced by bilinear interpolation from the 4 nearest valid neighbours:

$$
p_{i,j} = \frac{w_{+x}\,p_{i+1,j} + w_{-x}\,p_{i-1,j} + w_{+y}\,p_{i,j+1} + w_{-y}\,p_{i,j-1}}
               {w_{+x} + w_{-x} + w_{+y} + w_{-y}}
$$

where weights are 1 for valid pixels, 0 for flagged ones.

---

## 17. Streak Detection by Blob Elongation

After SEP source extraction, each detected source has semi-major and semi-minor
axes $a$ and $b$ (from the second moments of the intensity distribution):

$$
e = a / b
$$

Sources with $e > e_\text{threshold}$ (default 4.0) are classified as streaks/trails
and shown with a dashed-line overlay. Their position angle $\theta_\text{PA}$ is stored
in the measurement record for archival purposes.

The second moments from SEP:

$$
\bar{x^2} = \frac{\sum I(x,y)(x - \bar{x})^2}{\sum I(x,y)}, \quad
\bar{y^2} = \frac{\sum I(x,y)(y - \bar{y})^2}{\sum I(x,y)}, \quad
\bar{xy}  = \frac{\sum I(x,y)(x - \bar{x})(y - \bar{y})}{\sum I(x,y)}
$$

$$
a = \sqrt{\frac{\bar{x^2}+\bar{y^2}}{2} + \sqrt{\left(\frac{\bar{x^2}-\bar{y^2}}{2}\right)^2 + \bar{xy}^2}}
$$

---

## 18. ClumpFind — Blended Source Separation

When SEP marks a source island with the blended flag, AstroFind runs a multi-peak scan:

1. Identify the bounding box of the blended island.
2. Compute the local maximum $I_\text{max}$.
3. Scan isophotal contours from $I_\text{max}$ down to $I_\text{threshold}$ in steps of
   $\Delta I = 0.1\,(I_\text{max} - I_\text{threshold})$.
4. At each level, count disconnected regions; a new region appearing for the first time
   is a new sub-peak.
5. Each sub-peak is assigned the pixels it uniquely captures as the level descends.

Blended detections are drawn with a magenta double-circle overlay (inner circle = first
sub-peak, outer = second). Photometry of blended sources carries an uncertainty flag.

---

## 19. Sliding-Median Background Estimator

The image is divided into a grid of $N_x \times N_y$ tiles of size $w \times w$ pixels.
For each tile:

1. Compute the 3σ-clipped median $\tilde{S}$ (iteratively remove values more than
   $3\sigma$ from the median until convergence).
2. Smooth the grid using bilinear interpolation to produce a continuous sky map $S(x,y)$.

The background-subtracted image: $I'(x,y) = I(x,y) - S(x,y)$.

Default tile size: 64 pixels. Smaller tiles track finer gradients but may confuse
extended nebulosity with background.

---

## 20. 2-D Power Spectrum

The discrete 2-D Hann-windowed power spectrum:

**Hann window:**

$$
W(x, y) = \sin^2\!\left(\frac{\pi x}{N_x}\right)\sin^2\!\left(\frac{\pi y}{N_y}\right)
$$

**Real-to-complex FFT:** $F = \text{FFT2D}(I \cdot W)$

**Power spectrum (log scale, quadrant-shifted):**

$$
P(u, v) = \log_e\!\left(1 + |F(u, v)|^2\right)
$$

The quadrant shift places $u = v = 0$ at the image centre.

**Spatial frequency to period conversion:**

$$
\nu = \sqrt{u^2 + v^2}\,/\,N_x \quad [\text{cycles/pixel}]
$$
$$
T_\text{arcsec} = \frac{\text{plate\_scale}}{\nu} \quad [\text{arcsec}]
$$

---

## 21. ADES / MPC Report Format

### ADES 2017 XML Structure

```xml
<ades version="2017">
  <obsBlock>
    <obsContext>
      <observatory>
        <mpcCode>T05</mpcCode>
      </observatory>
      <submitter><name>Smith, J.</name></submitter>
      <observers><name>Smith, J.</name></observers>
      <measurers><name>Smith, J.</name></measurers>
      <telescope>
        <design>Reflector</design>
        <aperture>0.40</aperture>
        <fRatio>8</fRatio>
        <detector>CMOS</detector>
      </telescope>
    </obsContext>
    <obsData>
      <optical>
        <trkSub>2024ABC</trkSub>
        <mode>CCD</mode>
        <stn>T05</stn>
        <obsTime>2024-03-15T22:14:37.50Z</obsTime>
        <ra>185.4321</ra>
        <dec>+12.3456</dec>
        <rmsRA>0.15</rmsRA>
        <rmsDec>0.15</rmsDec>
        <mag>18.4</mag>
        <band>V</band>
        <sys>ICRF</sys>
        <ctr>399</ctr>
      </optical>
    </obsData>
  </obsBlock>
</ades>
```

The `<sys>ICRF</sys>` tag indicates the reported position is in the ICRF frame (i.e.,
corrected for annual aberration, precession, and nutation from J2000.0, and for atmospheric
refraction). The `<ctr>399</ctr>` tag is the NAIF body code for Earth.

### PSV Format

Pipe-Separated Values, one observation per line:

```
permID |provID |trkSub|mode|stn|obsTime                |ra        |dec      |rmsRA|rmsDec|mag|rmsMag|band|sys |ctr
       |       |A001  |CCD |T05|2024-03-15T22:14:37.50Z|185.432100|+12.34560|0.15 |0.15  |18.4|0.3  |V   |ICRF|399
```

---

*Last updated: 2026-03-21 — AstroFind 0.4.0*
*Author: Petrus Silva Costa*
