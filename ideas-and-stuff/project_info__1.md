# Proyek Praktikum KP Akhir — "Kisah di Tanah Perbatasan"

## Summary

This is a **final project for a Programming Concepts practicum** (`Praktikum Konsep Pemrograman`) — a *life-simulation / survival RPG* written in **plain C99, console-only, no external libraries**. The player lives a life from age 0 to age 106: each "year" is compressed into 3 *Action Days* (spend Energy on activities) and 2 *Random Days* (free-time rolls or encounters), with boss fights at ages 16, 46, and 86, and 6 possible endings driven by four stats plus a Karma/luck axis.

The repository is an **iterative snapshot archive**, not a single codebase: `Game/V0.0.0.1`, `V0.0.0.2`, and `V0.0.0.3` are three generations of the same game, each self-contained, each with its own `.o` files and (for 0.0.0.3) a built `.exe`. The design document lives in `ideas-and-stuff/DR's little corner.txt` — an Indonesian-language design journal whose last section, **"LANJUTTTTTT v3"**, is the specification that `V0.0.0.3` implements almost point-for-point.

`V0.0.0.3` is the state of the art and the subject of this report.

---

## What the project is "all about" (design intent, from the ideas folder)

Reading `ideas-and-stuff/DR's little corner.txt` in order tells you the author's actual thesis:

1. **Start deliberately empty.** The character "spawns" with no memory, no name, no possessions — *"Anggap saja kita 'spawn' itu tanpa pengetahuan apapun tentang masa lalu kita"*. This is a deliberate rejection of the usual RPG premise where the hero has a backstory. The narrative is **personal survival first**, community second: *"fokus cerita awalnya hanya fokus pada personal survival, karena diawal tahun kita tidak punya siapa-siapa"*.
2. **Compressed time, meaningful choices.** Years are compressed into a handful of decisions so that a 106-year life is playable in one sitting. The original idea was 7 days per year (5 work + 2 random); v3 settles on **3 Action Days + 2 Random Days**.
3. **Uncertainty with five grades.** Every action resolves into one of five outcomes — *sangat menguntungkan / menguntungkan / netral / tidak menguntungkan / sangat tidak menguntungkan* — and **Karma shifts the roll**. This is the core randomiser of the whole game.
4. **Grounded, not fantasy (for now).** *"Sebenarnya akan lebih baik jika diganti ke yang kental nuansa fantasi, namun.. untuk pendekatan kita, ambil konservatif saja."* Hence the mundane stat set: **Kesehatan, Mental, Kekayaan, Reputasi + Karma**. Fantasy is knowingly deferred — but the *bosses* (Naga Perbatasan) already leak fantasy in.
5. **Undertale-style combat.** Explicitly borrowed: *Serang / Aksi (negosiasi-bicara) / Item*, plus an extra **Assessment** option ("Aku mengamati...") that reveals whether the target is wounded/sick/in-trouble and makes attacking or negotiating easier.
6. **One weapon philosophy.** *"kita tidak perluh mengumpulkan sebanyak-banyaknya equipment... kita hanya perluh pilih satu."* Equipment is a small, upgradeable set — a sword, armour, an amulet — not a loot treadmill.
7. **Failure is death-by-neglect.** Three fail endings, each tied to which stat you let hit zero: madness (Mental), starvation (Kekayaan), illness (Kesehatan). Two "win" endings: **Mandiri** (self-sufficient balance) and **Komunitas** (blend into ordinary village life).

The v3 addendum (the "spec") is fully implemented:

| Spec line from `ideas-and-stuff` | Where it lives in V0.0.0.3 |
|---|---|
| Age 0 tutorial → age 1 start | `prolog()` in `game.c` |
| +5 years until 16, then +10 | `tentukanLompatan()` in `game.c` |
| Mini-boss at 16, second at 46, final at 86 | `UMUR_BOSS[3] = {16,46,86}` in `game.c` |
| Ending at age 106 | `UMUR_MIN_MENANG 106` in `ending.h` |
| Renamed terms: Umur, 3 Hari Aksi, 2 Hari Acak | `HARI_AKSI 3`, `HARI_ACAK 2` |
| Energy 3 for ages 1–16, then 5 | `ENERGI_AWAL 3`, `ENERGI_DEWASA 5` in `player.h` |
| Membership training: +3 energy | `BONUS_MEMBERSHIP 3`, `bukaToko()` |
| 3 potion types: heal / damage-buff / karma-buff | `RAMUAN_HEAL/DMG/KARMA`, `pakaiRamuan()` |
| 3 equipment, 3 materials each, crafted at blacksmith, 3 upgrade levels (total Lv0–3) | `JUMLAH_EQ`, `BAHAN_PER_EQ 3`, `HARGA_UPGRADE[3]`, `bukaPandaiBesi()` |
| "Special interaction event" — *not yet implemented* | — (still missing, as noted in the ideas file) |

---

## Architecture

**Pattern: layered module-per-concern, classic C translation units with one header each.** There is no framework, no build system beyond a Dev-C++ generated `Makefile.win`, no dynamic allocation, no file I/O, no save system. The entire game state is a single `Player` struct declared on the stack inside `jalankanGame()` in `game.c` and passed by pointer into every subsystem.

The layering is strictly one-directional:

```
main.c            — srand + jalankanGame + pause
  └ game.c        — the year loop, the only orchestrator
      ├ action.c  — 1 Action Day (menu of work activities)
      ├ combat.c  — turn-based encounters & boss fights
      ├ shop.c    — Toko + Pandai Besi
      ├ ending.c  — fail/win conditions & credits text
      ├ outcome.c — the 5-grade luck roll (the game's dice)
      └ player.c  — state, clamping, potions, equipment effects
           └ util.c — rand helper, input reading, screen clear
```

`player.h` is the hub: every other header includes it. `outcome.c` is the only module that *both* `action.c` and `combat.c` depend on, which is why "everything is rolled the same way" — a single function, `tentukanOutcome()`.

**Technology stack**

- Language: **C** (compiled as C, `IsCpp=0` in the `.dev` project file), targeted at **C99-ish** but with no `-std=` flag set in the Makefile.
- Toolchain: **Dev-C++ 5.11 / MinGW64 GCC 4.9.2** (`Game/V0.0.0.3/DEV/Makefile.win` hard-codes `C:/Users/DENA/OneDrive/Desktop/Dev-Cpp/MinGW64/...` include and lib paths — this Makefile is *not portable*; it will only build on the author's machine or with path edits).
- Libraries: `<stdio.h>`, `<stdlib.h>`, `<string.h>`, `<time.h>` — standard only.
- Artifacts: `.o` files are committed alongside sources in every version folder; `Game/V0.0.0.3/DEV/V0.0.0.3.exe` is the built binary.

**Execution flow (the primary runtime loop)**

```
main()                                   main.c
  srand(time(NULL))
  jalankanGame()                         game.c
    inisialisasiPlayer(&p)               player.c  (70/60/30/20 stats, 3 energy, levelEq=-1)
    prolog()                             game.c    (title + tutorial, waits 1 keypress)
    p.umur = 1
    while (e == ENDING_NONE) {
        e = jalankanTahun(&p)            game.c
        if (ok) p.umur += tentukanLompatan(&p)   // +5 below 16, +10 from 16 up
        if (ok) e = cekMenang(&p)                // only fires at umur >= 106
    }
    tampilStatus(&p); tampilEnding(e)
  tungguEnter()
```

`jalankanTahun()` is the heartbeat and reads like a schedule:

```
for hari = 1..3   → resetEnergiHarian; while (energi > 0) jalankanAksiKerja(); cekGagal each action
for hari = 1..2   → resetEnergiHarian; jalankanWaktuLuang();                    cekGagal
if (umur == 16|46|86) → mulaiBoss(p, index);                                    cekGagal
end-of-year tax → Kekayaan -3 (BIAYA_HIDUP), Kesehatan +10 (PEMULIHAN_TAHUNAN)
return cekGagal(p)
```

---

## Directory Structure

```
working-directory/
├── README.md                      — two lines: the project title, in Indonesian
├── ideas-and-stuff/
│   └── DR's little corner.txt     — THE design journal; ends with the v3 spec
└── Game/
    ├── V0.0.0.1/                  — prototype: "Syntax Odyssey" (event-choice game)
    │   ├── main.c                 — main loop, age stepping, history log, ending
    │   ├── chara.c/.h             — Character struct + stat clamping + death reason
    │   ├── events.c/.h            — EVENT_DB[] array of {age range, title, A, B}
    │   ├── ui.c/.h                — title screen, name input, status render, menu prompt
    │   ├── *.o                    — compiled objects (committed)
    │   └── project/               — Dev-C++ project files for a separate tiny main.c
    ├── V0.0.0.2/                  — rewrite into a stats/day simulation
    │   ├── Makefile.win, Project1.dev/.layout, Project1.exe
    │   └── files/                 — the seven-module rewrite
    │       ├── game.c/.h          — year loop (3 action days + 1 free-time phase)
    │       ├── player.c/.h        — stats, single potion counter, KARMA_MIN/MAX
    │       ├── action.c/.h        — work actions (energy-costed) + waktu luang
    │       ├── ending.c/.h        — fail/win checks (UMUR_MIN_MENANG was 5 — a test stub)
    │       ├── outcome.c/.h       — 5-grade roll (identical to v3)
    │       ├── util.c/.h          — acak / bacaAngka / tungguEnter / bersihkanLayar
    │       └── main.c             — srand + jalankanGame
    └── V0.0.0.3/                  — CURRENT. Nine modules, flat layout
        ├── main.c                 ── entry point
        ├── game.c/.h              ── year schedule, age leap, prologue, bosses
        ├── player.c/.h            ── Player struct, stats, potions, equipment effects
        ├── action.c/.h            ── activity table, sub-event, random-day roll
        ├── combat.c/.h            ── Undertale-style encounter + 3 bosses
        ├── shop.c/.h              ── Toko (potions/materials/membership) + Pandai Besi
        ├── ending.c/.h            ── fail conditions, 106-year win tiers
        ├── outcome.c/.h           ── the single 5-grade probability function
        ├── util.c/.h              ── rand/input/clear helpers
        ├── *.o                    ── compiled objects
        └── DEV/                   ── Dev-C++ project (Makefile.win, .dev, .exe, .layout)
```

Note the **flattening trend**: v1 grouped by `project/`, v2 used `files/`, v3 is flat with only the IDE folder separated. The `.o` files sitting next to sources in every version means the author builds in-place with Dev-C++ and commits the results; `make clean` was never run before committing.
