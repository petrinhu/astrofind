# AstroFind

**English** | [Português](#português)

---

## English

Modern asteroid detection software for citizen science and educational use.  
Built for Linux, compatible with the IASC / MPC workflow.  
Outputs [ADES 2022](https://minorplanetcenter.net/ades) format reports.

### Features

- FITS image viewer with multi-image blink comparison
- Automated star detection and plate solving (online via [astrometry.net](https://nova.astrometry.net) or offline via [ASTAP](https://www.hnsky.org/astap.htm))
- VizieR star catalog overlay for reference star identification
- Aperture photometry and centroid measurement
- Moving object detection with blink engine
- ADES 2022 / MPC report generation
- Day/Night/Auto UI themes
- Full bilingual interface: English and Brazilian Portuguese
- School workflow: step-by-step guided mode for students

### Requirements

| Dependency | Version |
|------------|---------|
| Qt         | ≥ 6.4   |
| cfitsio    | any     |
| Eigen3     | ≥ 3.3   |
| FFTW3      | any     |
| SEP        | ≥ 1.2   |
| spdlog     | ≥ 1.11  |
| CMake      | ≥ 3.20  |
| C++        | 17      |

### Building

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

Optional: install to system

```bash
sudo cmake --install build
```

### License

[PolyForm Noncommercial License 1.0.0](LICENSE) — free for personal, educational, and non-commercial use.  
Copyright (c) 2024–2026 Petrus Silva Costa.

### Acknowledgments

AstroFind was inspired by and learned from these excellent projects:

| Project | Author(s) | What we learned |
|---------|-----------|-----------------|
| [Astrometrica](http://www.astrometrica.at/) | Herbert Raab | Primary UX inspiration — the standard Windows tool for IASC/MPC work |
| [find_orb](https://github.com/Bill-Gray/find_orb) | Bill Gray (Project Pluto) | ADES output format, MPC submission, orbit determination |
| [umbrella2](https://github.com/mostanes/umbrella2) | CSCF | Moving object detection algorithms, tracklet linking |
| [Siril](https://gitlab.com/free-astro/siril) | Free-Astro team | PSF fitting, background extraction, photometry pipeline |
| [astrometry.net](https://github.com/dstndstn/astrometry.net) | Dustin Lang et al. | Plate-solving REST API used directly |
| [IRAF](https://github.com/iraf-community/iraf) | NOAO / community | Aperture photometry concepts |

Libraries used: [Qt6](https://github.com/qt/qtbase) · [cfitsio](https://github.com/HEASARC/cfitsio) · [Eigen3](https://gitlab.com/libeigen/eigen) · [FFTW3](https://github.com/FFTW/fftw3) · [SEP](https://github.com/kbarbary/sep) · [spdlog](https://github.com/gabime/spdlog) · [Catch2](https://github.com/catchorg/Catch2) · [QuaZip](https://github.com/stachenov/quazip) · [qt-keychain](https://github.com/frankosterfeld/qtkeychain)

---

## Português

Software moderno de detecção de asteroides para ciência cidadã e uso educacional.  
Desenvolvido para Linux, compatível com o fluxo de trabalho IASC / MPC.  
Gera relatórios no formato [ADES 2022](https://minorplanetcenter.net/ades).

### Funcionalidades

- Visualizador de imagens FITS com comparação de múltiplas imagens em blink
- Detecção automática de estrelas e resolução de placa (online via [astrometry.net](https://nova.astrometry.net) ou offline via [ASTAP](https://www.hnsky.org/astap.htm))
- Sobreposição do catálogo de estrelas VizieR para identificação de estrelas de referência
- Fotometria de abertura e medição de centroide
- Detecção de objetos em movimento com motor de blink
- Geração de relatórios ADES 2022 / MPC
- Temas de interface Dia/Noite/Automático
- Interface bilíngue completa: inglês e português brasileiro
- Fluxo escolar: modo guiado passo a passo para alunos

### Requisitos

| Dependência | Versão |
|-------------|--------|
| Qt          | ≥ 6.4  |
| cfitsio     | qualquer |
| Eigen3      | ≥ 3.3  |
| FFTW3       | qualquer |
| SEP         | ≥ 1.2  |
| spdlog      | ≥ 1.11 |
| CMake       | ≥ 3.20 |
| C++         | 17     |

### Compilação

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

Opcional: instalar no sistema

```bash
sudo cmake --install build
```

### Licença

[PolyForm Noncommercial License 1.0.0](LICENSE) — livre para uso pessoal, educacional e não comercial.  
Copyright (c) 2024–2026 Petrus Silva Costa.

### Agradecimentos

O AstroFind foi inspirado e aprendeu com estes excelentes projetos:

| Projeto | Autor(es) | O que aprendemos |
|---------|-----------|------------------|
| [Astrometrica](http://www.astrometrica.at/) | Herbert Raab | Inspiração principal de UX — a ferramenta padrão Windows para IASC/MPC |
| [find_orb](https://github.com/Bill-Gray/find_orb) | Bill Gray (Project Pluto) | Formato de saída ADES, submissão MPC, determinação de órbita |
| [umbrella2](https://github.com/mostanes/umbrella2) | CSCF | Algoritmos de detecção de objetos em movimento, ligação de tracklets |
| [Siril](https://gitlab.com/free-astro/siril) | Free-Astro team | Ajuste de PSF, extração de fundo, pipeline de fotometria |
| [astrometry.net](https://github.com/dstndstn/astrometry.net) | Dustin Lang et al. | API REST de plate-solving usada diretamente |
| [IRAF](https://github.com/iraf-community/iraf) | NOAO / comunidade | Conceitos de fotometria de abertura |

Bibliotecas utilizadas: [Qt6](https://github.com/qt/qtbase) · [cfitsio](https://github.com/HEASARC/cfitsio) · [Eigen3](https://gitlab.com/libeigen/eigen) · [FFTW3](https://github.com/FFTW/fftw3) · [SEP](https://github.com/kbarbary/sep) · [spdlog](https://github.com/gabime/spdlog) · [Catch2](https://github.com/catchorg/Catch2) · [QuaZip](https://github.com/stachenov/quazip) · [qt-keychain](https://github.com/frankosterfeld/qtkeychain)
