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
---

## Key Abstractions

### `Player` (struct)
- **File**: `Game/V0.0.0.3/player.h` (lines 16–30)
- **Responsibility**: The *entire* game state. There is no other persistent object.
- **Interface**:
  - `stat[4]` — Kesehatan / Mental / Kekayaan / Reputasi, always clamped `0..100` by `ubahStat()`
  - `karma` — clamped `-50..50` by `ubahKarma()`; the only stat that can go negative
  - `umur` — current age; drives energy tier, action unlocks, boss schedule
  - `energi`, `energiMaks`, `bonusEnergi` — per-day budget; `energiMaks` is *recomputed* every day
  - `ramuan[3]` — potion counts, indexed by `RAMUAN_HEAL/DMG/KARMA`
  - `bahan[3]` — crafting materials, indexed by `EQ_PEDANG/ZIRAH/AMULET`
  - `levelEq[3]` — **`-1` means "not yet crafted"**, `0..3` means owned at that upgrade level
  - `buffDmg` — boolean-ish flag, cleared at the end of every encounter
  - `buffKarma` — numeric bonus, cleared at the start of every day by `resetEnergiHarian()`
- **Lifecycle**: created once on the stack in `jalankanGame()`, lives for the whole program, never written to disk.
- **Used by**: literally everything. `player.h` is included by `game.h`, `action.h`, `combat.h`, `shop.h`, `ending.h`.

### `Aksi` (work-activity descriptor table)
- **File**: `Game/V0.0.0.3/action.c` (lines 10–25)
- **Responsibility**: Data-driven definition of the six activities — name, `minUmur` gate, `biayaEnergi`, which stat it raises, and the health/mental costs to *attempt* it.
- **Interface**: `static const Aksi DAFTAR_AKSI[JUMLAH_AKSI]`. Entries:
  | Activity | min age | energy | raises | HP cost | Mental cost |
  |---|---|---|---|---|---|
  | Bermain & mengamati sekitar | 1 | 1 | Mental | 0 | 0 |
  | Mengambil kayu & ranting | 4 | 1 | Kekayaan | 1 | 0 |
  | Meramu herbal sederhana | 6 | 1 | Kesehatan | 0 | 1 |
  | Bekerja di ladang | 8 | 2 | Kekayaan | 2 | 0 |
  | Bergaul di desa | 8 | 1 | Reputasi | 0 | 1 |
  | Berburu di hutan | 12 | 3 | Kekayaan | 3 | 1 |
- **Lifecycle**: compile-time constant; never mutated.
- **Used by**: `jalankanAksiKerja()` builds a filtered index array `indeksAksi[]` of currently available entries, then maps the player's menu choice back through it.

### `tentukanOutcome()` (the game's single die)
- **File**: `Game/V0.0.0.3/outcome.c` (lines 11–19)
- **Responsibility**: Turn `(karma, bonus)` into one of five `Outcome` values. Every uncertain event in the game — work, free time, negotiation, escape — funnels through this one function.
- **Interface**:
  ```c
  int r = acak(0, 99) - karma / 5 - bonus - 20;
  r < 15 → SANGAT_UNTUNG   (delta +10)
  r < 60 → UNTUNG          (delta  +5)
  r < 80 → NETRAL          (delta   0)
  r < 90 → RUGI            (delta  -4)
  else   → SANGAT_RUGI     (delta  -8)
  ```
- **Design intent**: the `-20` and the wide `+10/+5` bands make the baseline **generous** — at Karma 0 the player has a 35 % chance of a great result and 0 % chance of a loss. Karma is a *tilt*, not a gate.
- **Non-obvious**: `karma / 5` is **integer division truncating toward zero**, so `karma = -4` contributes 0, not −0.8; the luck penalty is slightly asymmetric (positive Karma is worth marginally more than the equivalent negative Karma hurts). Also see "Dead code" below — `SANGAT_RUGI` is in practice unreachable.

### `Target` (combat entity)
- **File**: `Game/V0.0.0.3/combat.c` (lines 8–15) — **private to `combat.c`**, not in the header
- **Responsibility**: A single encounter's opponent: `nama[24]`, `jenis` (`MUSUH` / `PEDAGANG` / `BOSS`), hidden `kondisi` (`NORMAL` / `TERLUKA` / `SAKIT` / `KESULITAN`), `hp`, `kekuatan`.
- **Lifecycle**: stack-local inside `mulaiEncounter()` / `mulaiBoss()`, destroyed when the fight ends. Nothing persists.
- **Notable**: the *condition* is generated at spawn but is **invisible to the player until they spend a turn on Assessment** (`assessed` flag). That flag also enables a **+6 damage bonus** on wounded/sick enemies and a **+20 outcome bonus** / **+5 wealth** when negotiating with a struggling merchant.

### `Ending` (enum)
- **File**: `Game/V0.0.0.3/ending.h` (lines 7–17)
- **Responsibility**: Single source of truth for "is the run over and how". `ENDING_NONE` is the loop sentinel, so fail/win checks return a `Ending` that *is* the control signal.
- **Interface**:
  - `cekGagal(p)` — called **every day**; returns a fail ending if any of HP / Mental / Kekayaan `<= 0`
  - `cekMenang(p)` — called **once per year**; no-op below age 106, otherwise tiers: Komunitas (Rep ≥ 65, others ≥ 40) → Mandiri (all stats ≥ 55) → Bertahan (fallback)
  - `tampilEnding(e)` — prints the epilogue prose
- **Used by**: `game.c`, which uses `e != ENDING_NONE` as the loop exit condition.

### The `bayar()` invariant
- **File**: `Game/V0.0.0.3/shop.c` (lines 17–21)
- **Responsibility**: A deliberate guard — `if (p->stat[STAT_KEKAYAAN] <= harga)` refuses the purchase.
- **Why**: because `Kekayaan == 0` is an **instant game-over** (`ENDING_GAGAL_MISKIN`), spending your last coin would silently kill the player from inside a shop menu. `bayar()` therefore makes it *impossible* to buy something that would leave you at 0 wealth. Pay attention to the strictness: even buying at *exactly* your remaining wealth is refused.

---

## Data Flow

### A. App start → first playable year
1. `main()` seeds `rand()` with `time(NULL)` and calls `jalankanGame()`.
2. `inisialisasiPlayer()` zeroes the struct, sets `levelEq[i] = -1`, stats `70/60/30/20`, energy `3/3`.
3. `prolog()` clears the screen, prints the tutorial (stat ranges, day structure, age leaps, boss ages, the five outcome grades, fail conditions), waits for a single keypress.
4. `umur = 1`; the `while (e == ENDING_NONE)` loop begins.

### B. One Action Day (repeated 3× per year)
1. `resetEnergiHarian()` → `energiMaks = (umur <= 16 ? 3 : 5) + bonusEnergi`, refills `energi`, zeroes `buffKarma`.
2. Screen banner `######## UMUR n | Hari Aksi i/3 ########`, then `while (p->energi > 0)`:
3. `tampilStatus()` prints the stat block and potion counts.
4. `jalankanAksiKerja()` prints the filtered activity menu plus **Toko / Pandai Besi / Pakai Ramuan / Akhiri hari**. Only activities you can afford are selectable; an unaffordable pick loops back with a message.
5. On confirmation, the cost is paid (`pakaiEnergi`, HP cost, Mental cost), then `tentukanOutcome(karmaEfektif(p), 0)` is rolled and applied to the activity's main stat.
6. Post-action hooks, in order:
   - `indeksAsli == 1` (gather wood) and `acak(1,100) <= 45` → **`subEventHewanTerperangkap()`**: a trapped-animal choice. Helping costs 1 energy, grants **+5 Karma**, then rolls with **`bonus = +10`** for loot (1 potion + 4 Mental) / consolation (+3 Mental) / bite (−4 HP).
   - `indeksAsli == 2` (herbalism) and a favourable outcome → `+1 Ramuan Penyembuh`.
   - outcome `== SANGAT_RUGI` and `umur >= 8` → **forced encounter** (`mulaiEncounter`) — *see the dead-code note: this cannot currently fire.*
7. `cekGagal()` after every action; a fail ending aborts the whole game immediately, mid-day.
8. "Akhiri hari" returns 1 and breaks the energy loop.

### C. One Random Day (repeated 2× per year)
1. `resetEnergiHarian()` again (yes — energy refills, but Random Days don't consume it unless a fight begins).
2. `jalankanWaktuLuang()`: if `umur >= 8` and a 50 % roll hits, the day becomes an **encounter** and returns immediately. Otherwise a random stat (any of the four) is nudged by the outcome delta of a single roll.

### D. A combat encounter
1. `mulaiEncounter()` → `buatTarget()`: 60 % monster (3 species, HP 25–40, str 3–6, 1/3 chance of being *terluka* which **halves HP**, 1/3 *sakit*), 40 % merchant (HP 20, str 0, 2/3 *kesulitan*).
2. Banner: `Kamu bertemu <nama> di <hutan|jalan desa|tepi sungai|gerbang kota>, apa yang kamu mau lakukan?`
3. `jalankanEncounter()` menu loop: **1 Serang, 2 Aksi (Negosiasi/Bicara), 3 Item (Ramuan), 4 Assessment, 5 Kabur**.
   - *Serang* on a merchant = robbery: `Kekayaan +8`, `Reputasi −10`, `Karma −10`, fight ends instantly.
   - *Serang* on a monster = `acak(8,14) + efekEq(Pedang) + (buffDmg ? 6 : 0)`, **+6** if `assessed` and wounded/sick. Kill grants `Kekayaan +5`, `Reputasi +3`.
   - *Negosiasi* on a merchant always resolves the fight; wealth changes by `deltaOutcome ± 5`. On a monster, a favourable roll makes it leave (`Reputasi +2`, `Karma +2`) but a bad roll **wastes the turn**.
   - *Item* → `pakaiRamuan(p, 1)`; a cancelled potion does **not** consume the turn.
   - *Assessment* → sets `assessed`, prints the hidden condition; re-assessing is refused.
   - *Kabur* → merchants and (implicitly) nothing else let you walk away; success on monsters needs `<= NETRAL`; always fails on bosses.
4. If the fight is not over and the target isn't a merchant, `giliranMusuh()` hits for `kekuatan + acak(0,3) − efekEq(Zirah)`, clamped to a minimum of 1.
5. On exit, `p->buffDmg = 0`.

### E. Boss fight (ages 16, 46, 86)
`mulaiBoss(p, tingkat)` builds a `TARGET_BOSS` with fixed stats — **Alfa Serigala 70 HP / 6 str**, **Kepala Bandit 130 HP / 9 str**, **Naga Perbatasan 200 HP / 13 str** — prints `Tidak ada jalan kabur`, and runs the same encounter loop. Escape and negotiation are functionally disabled, so the loop only terminates on boss death or player death. Victory awards `Reputasi +10`, `Kekayaan +10`.

### F. End of year → next year
1. Living costs: `Kekayaan −3`, then `Kesehatan +10` (recovery, clamped at 100).
2. `cekGagal()` — so the living-cost deduction can *itself* be the thing that kills you.
3. Back in `jalankanGame()`: `umur += tentukanLompatan()` (`+5` while `umur < 16`, else `+10`), then `cekMenang()` (no-op until 106).

Resulting age sequence: **1 → 6 → 11 → 16 → 26 → 36 → 46 → 56 → 66 → 76 → 86 → 96 → 106** — thirteen playable years, each a full 3+2 day cycle. A complete run is roughly 13 × 7 ≈ 90 menu resolutions plus fights.

---

## Non-Obvious Behaviors & Design Decisions

- **`OUT_SANGAT_RUGI` is unreachable — and it gates a feature.** From `outcome.c`, `r = acak(0,99) − karma/5 − bonus − 20`. The maximum possible `r` occurs at `acak = 99`, `karma = −50` (its clamp floor), `bonus = 0`: `99 + 10 − 20 = 89`, which maps to `OUT_RUGI`, not `OUT_SANGAT_RUGI` (needs `r ≥ 90`). Consequence: the branch in `action.c` — `if (o == OUT_SANGAT_RUGI && p->umur >= 8) mulaiEncounter(p);` — **can never execute**. The "something disturbs your work" ambush is dead code. The only live encounter path is the 50 % free-time roll in `jalankanWaktuLuang()`. This is the single most likely thing to confuse someone reading `action.c`: the code reads as though failure spawns fights, but it doesn't.
- **The dice are biased in the player's favour by design, then swung by Karma.** The `−20` shift plus bands `[0,15)/[15,60)/[60,80)/[80,90)/[90,100)` mean: Karma 0 → 35 % great / 45 % good / 20 % neutral / **0 % bad**. Karma +50 → 45 % great. Karma −50 → still only 10 % bad. A developer "fixing" the `−20` would destroy the game's difficulty curve entirely.
- **`levelEq = -1` is a sentinel, not a level.** `efekEq()` returns `0` for negative values; `bukaPandaiBesi()` treats `lv < 0` as "craft", `0..2` as "upgrade", `3` as "maxed". Every new equipment array init must respect this or the "Rakit" menu silently becomes an "upgrade Lv-1" bug.
- **Two different buff lifetimes.** `buffKarma` is cleared by `resetEnergiHarian()`, i.e. it expires at **dawn of the next day** — so a Karma potion bought at the end of an Action Day is nearly wasted. `buffDmg` is cleared at the **end of the encounter**, so it lasts the whole fight. `pakaiRamuan()`'s `combat` parameter is what stops the damage potion being drunk outside fights.
- **`energiMaks` is derived, never authoritative.** `resetEnergiHarian()` recomputes it from `umur` and `bonusEnergi` every day. Any code that mutates `energiMaks` directly is overwritten at the next day boundary. The membership bonus therefore only "appears" the following day — the shop text says exactly that ("mulai hari berikutnya").
- **Three independent kill switches.** Each day, `cekGagal()` checks HP ≤ 0, Mental ≤ 0, and Kekayaan ≤ 0. But Kekayaan also drains by 3 per year automatically, so the economic fail-state is a slow clock rather than an event: at starting wealth 30 you have ~10 years of runway unless you work. Meanwhile `bayar()` prevents *self-inflicted* bankruptcy in shops.
- **Boss victory is inferred from the player's health, not the boss's.** `mulaiBoss()` prints `Kamu mengalahkan %s!` when `p->stat[STAT_KESEHATAN] > 0` after the loop. This is correct only because the encounter loop cannot exit otherwise (no escape, no negotiation, boss keeps fighting until dead), but it is fragile: adding a non-lethal exit would silently hand out boss rewards.
- **Health is both a resource and a clock.** Actions cost HP on *attempt* (`biayaKes`) regardless of outcome, combat costs HP, but every year grants `+10` HP back (capped at 100). So maximising long-term survival means ending each year with low HP so the +10 isn't wasted — an emergent optimisation the tutorial never mentions.
- **Final-boss maths is extremely tight.** Naga: 200 HP, 13 str (+0..3). With Amulet/Zirah maxed you reduce incoming to `13+d3−5` ≈ 8–11 per turn. Player output is `acak(8,14) + 12` (Pedang Lv3) ≈ 20–26. That's ~9 turns to kill vs ~11 turns to die from 100 HP — i.e. **the age‑86 fight is effectively gated on having both Pedang and Zirah crafted and near-maxed**, which costs hundreds of Kekayaan. The game's real difficulty is therefore economic, not tactical.
- **`system("cls")` is the only platform-specific code**, behind `#ifdef _WIN32`. Everything else is portable standard C.
- **Input hardening is minimal but real**: `bacaAngka()` reads a line with `fgets` + `sscanf`, re-prompts on garbage or out-of-range, and `exit(0)` on EOF — so piping the program's stdin causes a silent clean exit rather than an infinite loop.
- **No save/load, no seeds, no replay.** `rand()` is seeded from wall-clock time; runs are unreproducible. This is a real limitation for a graded practicum project: there is no way to demonstrate the same run twice.
- **`(void)p;` in v2's `tentukanLompatan`** is a marker of the evolution — v2 advanced one year at a time (a very long game), v3 changed one line into the `+5/+10` leap and turned the same program into a ~15-minute session. That single function is the whole pacing design.
- **v2's `UMUR_MIN_MENANG 5`** versus v3's `106` shows the author used the win condition as a **debug shortcut** during development, then changed it back. Anyone running v2 will "win" almost instantly — that is not a bug in v2, it's a test stub left in.
- **The ideas file is a spec, not documentation.** Where the implementation differs from it (e.g. the spec says Random Days are where stats rise *and* encounters happen; the code makes single random days mutually exclusive — either an encounter *or* a stat nudge), the code is the source of truth, but the ideas file is what tells you *why* each system exists.
