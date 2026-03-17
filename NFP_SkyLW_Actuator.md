# EMS Actuator for CondFD Exterior Sky Longwave Radiation Override

**Brian, NatLabRockies**

- March 2026 - Initial Draft

## Overview

Building energy simulation researchers coupling EnergyPlus with external radiation solvers (COMSOL, RadTherm, custom view factor codes) need to override the linearized sky longwave radiation calculation in the CondFD heat balance. Currently, EnergyPlus computes sky LW as `hsky*(Tsky - Tsurf)` using a linearized h-coefficient approach. This is adequate for standard simulations but insufficient when:

- Users have measured net sky radiation data (pyrgeometer)
- External tools compute view-factor-accurate LW exchange with complex sky models
- Research requires decoupling the sky radiation model from the EnergyPlus surface solver

## Justification

The CondFD algorithm already supports EMS actuators for material conductivity, specific heat, and internal heat flux (added FY2021-2022). This proposal extends that pattern to the exterior sky radiation boundary condition — the last remaining piece needed for full external coupling of the CondFD exterior heat balance.

## Implementation

Add an EMS actuator `"CondFD Surface" / "Sky Longwave Radiation Override"` (W/m2) that replaces the `hsky*(Tsky - Tsurf)` term in the CondFD exterior boundary equation (`ExteriorBCEqns` in `HeatBalFiniteDiffManager.cc`). When not actuated, existing behavior is unchanged.

### Sign Convention

Positive = heat INTO the surface (consistent with all existing EnergyPlus reporting):
- `Enet > 0` — net sky LW heats the surface
- `Enet < 0` — net sky LW cools the surface (typical)
- `Enet = 0` — no sky LW exchange

### Scope

- CondFD algorithm only (CTF/EMPD not affected)
- Replaces only the sky component; ground, surrounding surface, and air LW terms unchanged
- All 4 solver paths patched: R-layer, Crank-Nicolson, fully implicit, movable insulation
- Reporting (QNetSurfFromOutside, SurfQdotRadOutRepPerArea) updated consistently

### Actuator Details

| Field | Value |
|-------|-------|
| Component Type | `CondFD Surface` |
| Unique ID | Surface name (e.g., `Zn001:Roof001`) |
| Control Type | `Sky Longwave Radiation Override` |
| Units | `[W/m2]` |

One actuator per exterior CondFD surface, controllable via IDF EMS programs or Python API.

### New Output Variable

`CondFD EMS Sky Longwave Radiation Override Heat Flux [W/m2]` — reports the actuated value (0.0 when not actuated).

### Files Modified

| File | Change |
|------|--------|
| `HeatBalFiniteDiffManager.hh` | Add `enetActuator` field to `SurfaceDataFD` |
| `HeatBalFiniteDiffManager.cc` | Actuator registration + 6 solver location overrides |

### Physics

In the linearized CondFD exterior heat balance, `hsky*Tsky` appears in the **numerator** and `hsky` alone in the **denominator** of implicit solver equations. When actuated:
- **Numerator:** `hsky * Tsky` replaced by `Enet`
- **Denominator:** `hsky` removed (Enet is a fixed flux, not temperature-dependent)

This is applied consistently across all 6 code locations in `ExteriorBCEqns`.

## Testing

- Unit test: `HeatBalFiniteDiffManager_EnetActuatorOverride` — exercises ExteriorBCEqns with actuator OFF, Enet=0, Enet=-200, Enet=+200; verifies correct temperature ordering
- Integration tests: `1ZoneCondFD_Enet_Test.idf` (baseline) and `1ZoneCondFD_Enet_EMS.idf` (EMS with Enet=-200)
- Validated against Phase 2 hardcoded results (roof temps match within 0.1C)

### Phase 2 Validation Results

| Run | Enet (W/m2) | Roof Temp (C) | LW Report (W/m2) |
|-----|-------------|---------------|-------------------|
| Baseline | n/a | -23.8 | -32.0 |
| Enet=0 | 0 | -16.0 | 0.0 |
| Enet=-200 | -200 | -55.8 | -200.0 |
| Enet=+200 | +200 | +12.6 | +200.0 |

## Documentation

EMS Application Guide will be updated to describe the new actuator, including:
- Usage with IDF EMS programs
- Usage with Python plugin API
- Usage with standalone Python API (Jupyter notebook example provided)

## IDD Changes and Transition

None required. The actuator is registered programmatically via `SetupEMSActuator`.

## Example Files

- `testfiles/1ZoneCondFD_Enet_Test.idf` — baseline CondFD test
- `testfiles/1ZoneCondFD_Enet_EMS.idf` — EMS actuator with Enet=-200
- `scripts/CondFD_Enet_Override.ipynb` — Jupyter notebook for interactive use with any IDF/EPW
