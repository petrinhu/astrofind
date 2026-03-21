#pragma once

#include <QString>
#include <QStringList>
#include <QVector>

// ─── Observatory record ───────────────────────────────────────────────────────

struct Observatory {
    const char* mpcCode;   ///< MPC 3-character code
    const char* name;      ///< Observatory name
    const char* city;      ///< City / location name
    const char* country;   ///< ISO 3166-1 alpha-2 country code
    double      lat;       ///< Latitude  degrees (+N)
    double      lon;       ///< Longitude degrees (+E)
    double      alt;       ///< Altitude metres
};

// ─── Static table ─────────────────────────────────────────────────────────────
// Source: MPC Observatory Codes  https://www.minorplanetcenter.net/iau/lists/ObsCodes.html
// Last reviewed: 2025

namespace {

static const Observatory kObservatories[] = {

    // ── Brazil ────────────────────────────────────────────────────────────────
    { "I11", "Observatório Nacional",                "Rio de Janeiro",        "BR", -22.894583, -43.222639,  33.0 },
    { "874", "INPE / OAB",                           "São José dos Campos",   "BR", -23.201111, -45.861389, 640.0 },
    { "W49", "Observatório do Pico dos Dias (OPD)",  "Itajubá",               "BR", -22.534722, -45.582917, 1864.0 },
    { "F65", "Observatório Astronômico do Sertão de Itaparica (OASI)", "Petrolina", "BR", -9.363611, -40.530278, 317.0 },
    { "I14", "Obs. Astronômico — UFMG",              "Belo Horizonte",        "BR", -19.871944, -43.968056, 858.0 },
    { "T05", "Observatório Dietrich Schiel (OAS/CDCC)", "São Carlos",         "BR", -22.006944, -47.895000, 856.0 },
    { "875", "Obs. do Valongo — UFRJ",               "Rio de Janeiro",        "BR", -22.896944, -43.188056,  23.0 },
    { "876", "Faz. do Capão / IAG-USP",              "São Paulo",             "BR", -23.651389, -46.623056, 800.0 },

    // ── Chile ─────────────────────────────────────────────────────────────────
    { "309", "Cerro Paranal — VLT (ESO)",            "Antofagasta",           "CL", -24.627500, -70.404167, 2635.0 },
    { "809", "Cerro Tololo (CTIO)",                  "La Serena",             "CL", -30.169167, -70.804167, 2215.0 },
    { "807", "Cerro La Silla (ESO)",                 "La Serena",             "CL", -29.256944, -70.729444, 2400.0 },
    { "Y28", "Cerro Pachón — Gemini South (GS)",     "La Serena",             "CL", -30.240750, -70.736667, 2722.0 },
    { "I04", "ALMA (Llano de Chajnantor)",           "San Pedro de Atacama",  "CL", -23.022778, -67.754722, 5058.0 },
    { "W94", "Las Campanas Observatory",             "La Serena",             "CL", -29.003333, -70.701667, 2282.0 },

    // ── Argentina ─────────────────────────────────────────────────────────────
    { "821", "Obs. Astronómico de Córdoba",          "Córdoba",               "AR", -31.434722, -64.193333, 434.0 },
    { "826", "Obs. Astronómico de La Plata",         "La Plata",              "AR", -34.906944, -57.931944,  19.0 },
    { "827", "Obs. Astronómico de San Juan",         "San Juan",              "AR", -31.530000, -68.534167, 2312.0 },

    // ── Mexico ────────────────────────────────────────────────────────────────
    { "679", "Observatorio Astronómico Nacional (OAN)", "Ensenada",           "MX",  31.044167,-115.463889, 2800.0 },
    { "680", "Obs. Astrofísico Guillermo Haro",      "Cananea",               "MX",  31.054444,-110.392500, 2480.0 },
    { "843", "Obs. de Tonantzintla",                 "Puebla",                "MX",  19.032500, -98.313611, 2172.0 },

    // ── United States ─────────────────────────────────────────────────────────
    { "675", "Palomar Observatory",                  "San Diego",             "US",  33.356389,-116.863056, 1706.0 },
    { "568", "Mauna Kea Obs. (Keck/IRTF/CFHT)",     "Mauna Kea",             "US",  19.826667,-155.473333, 4145.0 },
    { "G96", "Mt. Lemmon Survey (MLS)",              "Tucson",                "US",  32.442778,-110.788611, 2791.0 },
    { "703", "Catalina Sky Survey (CSS)",            "Tucson",                "US",  32.416944,-110.732778, 2510.0 },
    { "695", "Kitt Peak National Obs.",              "Tucson",                "US",  31.963056,-111.599722, 1916.0 },
    { "688", "Lowell Observatory",                   "Flagstaff",             "US",  35.096111,-111.535000, 2198.0 },
    { "691", "Spacewatch (Steward Obs.)",            "Tucson",                "US",  32.416667,-111.000000, 2086.0 },
    { "696", "Agassiz Station (CfA)",                "Harvard",               "US",  42.506389, -71.572778,  185.0 },
    { "645", "Goodricke-Pigott Obs.",                "Tucson",                "US",  32.233333,-110.960000, 870.0 },
    { "V06", "Gemini North",                         "Mauna Kea",             "US",  19.823583,-155.469000, 4213.0 },
    { "705", "NEAT (Palomar) — Tri-cam",             "San Diego",             "US",  33.356389,-116.863056, 1706.0 },
    { "644", "McDonald Observatory",                 "Fort Davis",            "US",  30.671667,-104.022222, 2070.0 },
    { "586", "Lick Observatory",                     "San Jose",              "US",  37.341389,-121.642778, 1283.0 },
    { "585", "Mt. Wilson Observatory",               "Pasadena",              "US",  34.221667,-118.057778, 1742.0 },
    { "673", "Table Mountain Obs. (JPL)",            "Wrightwood",            "US",  34.381667,-117.681111, 2286.0 },
    { "711", "McDonald Obs. — HET",                 "Fort Davis",            "US",  30.681667,-104.014722, 2026.0 },

    // ── Portugal ──────────────────────────────────────────────────────────────
    { "985", "Obs. Astronómico de Lisboa",           "Lisboa",                "PT",  38.711111,  -9.183333,  100.0 },
    { "986", "Obs. Astronómico da Universidade do Porto", "Porto",            "PT",  41.156944,  -8.621944,   90.0 },
    { "A61", "Obs. de Santana — OASA",              "Santana",               "PT",  32.802778, -16.891667,  870.0 },
    { "A87", "Obs. Astronómico de Évora",            "Évora",                 "PT",  38.573333,  -7.909722,  309.0 },

    // ── Spain ─────────────────────────────────────────────────────────────────
    { "950", "Obs. del Roque de los Muchachos (ORM)", "La Palma",             "ES",  28.762500, -17.890556, 2396.0 },
    { "954", "Obs. del Teide (OT)",                  "Tenerife",              "ES",  28.299722, -16.512222, 2390.0 },
    { "936", "Isaac Newton Group — INT",             "La Palma",              "ES",  28.761944, -17.877778, 2336.0 },
    { "Z84", "Obs. de Calar Alto",                   "Almería",               "ES",  37.224167,  -2.546389, 2168.0 },
    { "J87", "Obs. Astronómico de Madrid",           "Madrid",                "ES",  40.408333,  -3.688056, 655.0 },

    // ── Germany ───────────────────────────────────────────────────────────────
    { "033", "Hamburger Sternwarte",                 "Hamburg",               "DE",  53.480556,  10.241667,  40.0 },
    { "032", "Heidelberger Landessternwarte",        "Heidelberg",            "DE",  49.397500,   8.721667, 570.0 },
    { "174", "Tautenburg Observatory",               "Tautenburg",            "DE",  50.979167,  11.711111, 341.0 },

    // ── France ────────────────────────────────────────────────────────────────
    { "010", "Obs. de Paris",                        "Paris",                 "FR",  48.836389,   2.336944,  67.0 },
    { "004", "Obs. de Haute-Provence (OHP)",         "Forcalquier",           "FR",  43.931111,   5.713611, 650.0 },
    { "050", "Obs. du Pic du Midi",                  "Tarbes",                "FR",  42.937222,   0.142222, 2877.0 },
    { "612", "Obs. de Bordeaux",                     "Floirac",               "FR",  44.835278,  -0.530556,  73.0 },

    // ── United Kingdom ────────────────────────────────────────────────────────
    { "002", "Royal Observatory Greenwich (hist.)", "London",                "GB",  51.476944,  -0.000278,  46.0 },
    { "I52", "Armagh Observatory",                   "Armagh",                "GB",  54.352500,  -6.649444,  64.0 },

    // ── Italy ─────────────────────────────────────────────────────────────────
    { "105", "Obs. di Brera — Merate",               "Merate",                "IT",  45.703333,   9.425278, 355.0 },
    { "101", "Osservatorio di Torino — Pino Torinese", "Torino",              "IT",  45.043333,   7.783333, 620.0 },
    { "598", "Obs. di Palermo",                      "Palermo",               "IT",  38.111111,  13.356111, 72.0 },
    { "096", "Obs. di Roma (Monte Porzio Catone)",   "Roma",                  "IT",  41.815000,  12.709444, 426.0 },

    // ── Australia ─────────────────────────────────────────────────────────────
    { "413", "Siding Spring Observatory (SSO)",      "Coonabarabran",         "AU", -31.272500, 149.069167, 1149.0 },
    { "414", "Perth Observatory",                    "Perth",                 "AU", -31.961667, 116.135278, 381.0 },
    { "415", "Mount Stromlo Observatory",            "Canberra",              "AU", -35.320278, 149.008056, 768.0 },
    { "431", "Anglo-Australian Observatory (AAT)",   "Coonabarabran",         "AU", -31.276944, 149.066944, 1164.0 },

    // ── Japan ─────────────────────────────────────────────────────────────────
    { "381", "Bisei Spaceguard Center (BSGC)",       "Ibara",                 "JP",  34.570000, 133.540000, 396.0 },
    { "376", "Yamamoto Observatory",                 "Aichi",                 "JP",  34.970833, 136.995833, 100.0 },
    { "345", "Kiso Observatory (U. Tokyo)",          "Nagano",                "JP",  35.795556, 137.627778, 1130.0 },

    // ── China ─────────────────────────────────────────────────────────────────
    { "330", "Purple Mountain Obs. (PMO)",           "Nanjing",               "CN",  32.068056, 118.827222, 267.0 },
    { "327", "Yunnan Obs.",                          "Kunming",               "CN",  25.028611, 102.796944, 1944.0 },
    { "337", "Xinglong Station — NAOC",              "Chengde",               "CN",  40.394444, 117.577500, 960.0 },

    // ── South Africa ──────────────────────────────────────────────────────────
    { "074", "South African Astronomical Obs. (SAAO)", "Sutherland",          "ZA", -32.379722,  20.810556, 1762.0 },
    { "B31", "SALT (SAAO)",                          "Sutherland",            "ZA", -32.375556,  20.810556, 1798.0 },

    // ── India ─────────────────────────────────────────────────────────────────
    { "244", "Vainu Bappu Obs. (IIA)",               "Kavalur",               "IN",  12.576389,  78.826944, 725.0 },
    { "098", "Indian Inst. of Astrophysics (IIA)",   "Bengaluru",             "IN",  12.971111,  77.596389, 920.0 },
    { "N50", "Mount Abu Infrared Obs.",              "Mount Abu",             "IN",  24.653056,  72.779167, 1680.0 },

    // ── Russia ────────────────────────────────────────────────────────────────
    { "095", "Special Astrophysical Obs. (SAO/BTA)", "Zelenchukskaya",        "RU",  43.650000,  41.441667, 2070.0 },
    { "089", "Pulkovo Observatory",                  "Saint Petersburg",      "RU",  59.773056,  30.326944,  75.0 },
    { "187", "Crimean Astrophysical Obs.",            "Nauchniy",              "RU",  44.726389,  34.015278, 550.0 },

    // ── Czech Republic ────────────────────────────────────────────────────────
    { "046", "Ondrejov Observatory",                 "Ondřejov",              "CZ",  49.909444,  14.780000, 533.0 },
    { "047", "Obs. Štefánik (Praha)",               "Praha",                 "CZ",  50.086944,  14.400000, 324.0 },

    // ── Hungary ───────────────────────────────────────────────────────────────
    { "053", "Konkoly Observatory",                  "Budapest",              "HU",  47.499722,  18.965000, 474.0 },
    { "B01", "Piszkéstető Obs. (Konkoly field station)", "Piszkéstető",       "HU",  47.919444,  19.894444, 958.0 },

    // ── Slovakia ──────────────────────────────────────────────────────────────
    { "118", "Astronomical Inst. (Slovak AS) Skalnaté Pleso", "Poprad",      "SK",  49.190000,  20.234167, 1783.0 },
    { "119", "Astronomical Inst. (Slovak AS) Modra",  "Modra",               "SK",  48.374167,  17.274167, 531.0 },

};

static const int kObservatoriesCount =
    static_cast<int>(sizeof(kObservatories) / sizeof(kObservatories[0]));

} // anonymous namespace

// ─── Namespace functions ──────────────────────────────────────────────────────

namespace ObservatoryDatabase {

/// Pointer to the static array of all observatory entries.
inline const Observatory* all() { return kObservatories; }

/// Total number of entries (including possible MPC-code duplicates).
inline int count() { return kObservatoriesCount; }

/// Unique country codes present in the database, sorted alphabetically.
inline QStringList countries()
{
    QStringList result;
    for (int i = 0; i < kObservatoriesCount; ++i) {
        const QString cc = QString::fromLatin1(kObservatories[i].country);
        if (!result.contains(cc))
            result.append(cc);
    }
    result.sort();
    return result;
}

/// All observatories for the given two-letter country code.
inline QVector<const Observatory*> forCountry(const char* cc)
{
    QVector<const Observatory*> result;
    for (int i = 0; i < kObservatoriesCount; ++i) {
        if (qstrcmp(kObservatories[i].country, cc) == 0) {
            // Skip entries with duplicate MPC code + same name combination
            bool alreadyIn = false;
            for (const Observatory* p : result) {
                if (qstrcmp(p->mpcCode, kObservatories[i].mpcCode) == 0 &&
                    qstrcmp(p->name,    kObservatories[i].name)    == 0) {
                    alreadyIn = true;
                    break;
                }
            }
            if (!alreadyIn)
                result.append(&kObservatories[i]);
        }
    }
    return result;
}

/// Look up an observatory by MPC code. Returns nullptr if not found.
inline const Observatory* byCode(const char* code)
{
    for (int i = 0; i < kObservatoriesCount; ++i) {
        if (qstrcmp(kObservatories[i].mpcCode, code) == 0)
            return &kObservatories[i];
    }
    return nullptr;
}

/// Country code → full country name in Portuguese.
inline QString countryName(const char* cc)
{
    struct Pair { const char* code; const char* name; };
    static const Pair kNames[] = {
        { "AR", "Argentina" },
        { "AU", "Austrália" },
        { "BR", "Brasil" },
        { "CL", "Chile" },
        { "CN", "China" },
        { "CZ", "República Tcheca" },
        { "DE", "Alemanha" },
        { "ES", "Espanha" },
        { "FR", "França" },
        { "GB", "Reino Unido" },
        { "HU", "Hungria" },
        { "IN", "Índia" },
        { "IT", "Itália" },
        { "JP", "Japão" },
        { "MX", "México" },
        { "PT", "Portugal" },
        { "RU", "Rússia" },
        { "SK", "Eslováquia" },
        { "US", "Estados Unidos" },
        { "ZA", "África do Sul" },
    };
    for (const auto& p : kNames) {
        if (qstrcmp(p.code, cc) == 0)
            return QString::fromUtf8(p.name);
    }
    return QString::fromLatin1(cc);
}

} // namespace ObservatoryDatabase
