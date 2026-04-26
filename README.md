# custom_dampers_lib — AKN k-epsilon Custom Damping Functions for STAR-CCM+

Custom STAR-CCM+ user library implementing the **f2** and **fmu** damping
functions for the AKN low-Reynolds-number k-epsilon turbulence model.

The formulas currently match the standard AKN model. The TODO comments in
`src/custom_dampers.cpp` mark where to insert a PySR / symbolic regression equation
once it has been derived from DNS data.

---

## Project structure

```
custom_dampers_lib/
├── Makefile
├── README.md
├── include/
│   └── uclib.h
└── src/
    └── custom_dampers.cpp
```

---



## Step 1 — Verify paths before building

```bash
make info
```

Expected output (all lines should say OK):

```
----------------------------------------------
STARCCM_INSTALL : /opt/Siemens/20.06.010/STAR-CCM+20.06.010
STARCCM_LIB_DIR : .../clang17.0/lib
STARCCM_SYS_DIR : .../system/clang17.0-64
CXX             : clang++
----------------------------------------------
  [OK] STARCCM_LIB_DIR found
  [OK] libUserFunctions.so found
  [OK] STARCCM_SYS_DIR found
  [OK] libstdc++.so.6 found
  [OK] clang++ found at ...
----------------------------------------------
```

If `clang++` is not found:
```bash
which clang++
# or if your cluster uses modules:
module avail clang
module load clang
```

---

## Step 2 — Build

```bash
make
```

Successful output ends with:

```
  Built: libcustom_dampers.so
  Load in STAR-CCM+: Tools → User Code → Add User Library
```

---

## Step 3 — Load in STAR-CCM+

```
Tools → User Code → Add User Library → select libcustom_dampers.so
```

After loading, two new field functions appear in the field function list:

| Field Function Name  | Description                                       |
|----------------------|---------------------------------------------------|
| `custom_dampers_custom`      | Damps destruction term in ε-equation near wall    |
| `fmu_AKN_custom`     | Damps turbulent viscosity νt = Cμ·fmu·k²/ε       |

---

## Step 4 — Use the field functions in STAR-CCM+

**Option A — Visualise and validate (recommended first step):**

1. Create a new Scalar Scene
2. Set scalar to `custom_dampers_custom`
3. Create an XY Plot: Wall Distance on Y-axis, `custom_dampers_custom` on X-axis
4. Compare the profile against DNS data

**Option B — Apply as source term to the ε-equation:**

```
Physics Continuum → Models → K-Epsilon → Source Terms → Add User Source
```

Use `custom_dampers_custom` to correct the C_eps2 destruction term:
```
($$custom_dampers_custom - f2_builtin) * C_eps2
* $TurbulentDissipationRate^2 / $TurbulentKineticEnergy * $Volume
```

---

## Step 5 — Replace with your PySR equation

Once PySR has been run on the DNS data (Re=5200), find the TODO comment
in `src/custom_dampers.cpp` and replace the standard AKN formula:

```cpp
// Current standard AKN formula — REPLACE THIS:
double f2 = pow((1.0 - exp(-ystar / 3.1)), 2)
          * (1.0 - 0.3 * exp(-pow(Rt / 6.5, 2.0)));

// With your PySR output, for example:
double f2 = <your_pysr_expression_using_ystar_and_Rt>;
```

Available variables inside the loop:

| Variable | Meaning                            | Units  |
|----------|------------------------------------|--------|
| `ystar`  | wall distance in dissipation units | —      |
| `Rt`     | turbulent Reynolds number k²/(ν·ε) | —      |
| `nu`     | kinematic viscosity                | m²/s   |
| `u_eps`  | dissipation velocity (ε·ν)^0.25   | m/s    |

After editing, recompile and reload:
```bash
make clean && make
# then reload libcustom_dampers.so in STAR-CCM+
```

---

## Troubleshooting

**`libUserFunctions.so` not found at runtime:**
```bash
ldd libcustom_dampers.so    # check what it links to
```

**GLIBCXX version error on load:**
The Makefile handles this via `-Wl,-rpath` pointing to
`system/clang17.0-64/libstdc++.so.6` bundled with STAR-CCM+.
If it still fails, double-check `STARCCM_SYS_DIR` in the Makefile.

**Compiler not found:**
```bash
module load clang
```

**Clean rebuild:**
```bash
make clean && make
```