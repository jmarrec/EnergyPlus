// EnergyPlus, Copyright (c) 1996-present, The Board of Trustees of the University of Illinois,
// The Regents of the University of California, through Lawrence Berkeley National Laboratory
// (subject to receipt of any required approvals from the U.S. Dept. of Energy), Oak Ridge
// National Laboratory, managed by UT-Battelle, Alliance for Energy Innovation, LLC, and other
// contributors. All rights reserved.
//
// NOTICE: This Software was developed under funding from the U.S. Department of Energy and the
// U.S. Government consequently retains certain rights. As such, the U.S. Government has been
// granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable,
// worldwide license in the Software to reproduce, distribute copies to the public, prepare
// derivative works, and perform publicly and display publicly, and to permit others to do so.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted
// provided that the following conditions are met:
//
// (1) Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//
// (2) Redistributions in binary form must reproduce the above copyright notice, this list of
//     conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//
// (3) Neither the name of the University of California, Lawrence Berkeley National Laboratory,
//     the University of Illinois, U.S. Dept. of Energy nor the names of its contributors may be
//     used to endorse or promote products derived from this software without specific prior
//     written permission.
//
// (4) Use of EnergyPlus(TM) Name. If Licensee (i) distributes the software in stand-alone form
//     without changes from the version obtained under this License, or (ii) Licensee makes a
//     reference solely to the software portion of its product, Licensee must refer to the
//     software as "EnergyPlus version X" software, where "X" is the version number Licensee
//     obtained under this License and may not use a different name for the software. Except as
//     specifically required in this Section (4), Licensee shall not use in a company name, a
//     product name, in advertising, publicity, or other promotional activities any name, trade
//     name, trademark, logo, or other designation of "EnergyPlus", "E+", "e+" or confusingly
//     similar designation, without the U.S. Department of Energy's prior written consent.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

// EnergyPlus::UnitHeater Unit Tests

// Google Test Headers
#include <gtest/gtest.h>

// EnergyPlus Headers
#include <EnergyPlus/Data/EnergyPlusData.hh>
#include <EnergyPlus/DataEnvironment.hh>
#include <EnergyPlus/DataHVACGlobals.hh>
#include <EnergyPlus/DataLoopNode.hh>
#include <EnergyPlus/DataSizing.hh>
#include <EnergyPlus/DataZoneEnergyDemands.hh>
#include <EnergyPlus/DataZoneEquipment.hh>
#include <EnergyPlus/ElectricPowerServiceManager.hh>
#include <EnergyPlus/Fans.hh>
#include <EnergyPlus/FluidProperties.hh>
#include <EnergyPlus/HeatBalanceManager.hh>
#include <EnergyPlus/HeatingCoils.hh>
#include <EnergyPlus/IOFiles.hh>
#include <EnergyPlus/OutputReportPredefined.hh>
#include <EnergyPlus/PlantUtilities.hh>
#include <EnergyPlus/Psychrometrics.hh>
#include <EnergyPlus/ScheduleManager.hh>
#include <EnergyPlus/SimulationManager.hh>
#include <EnergyPlus/SizingManager.hh>
#include <EnergyPlus/SurfaceGeometry.hh>
#include <EnergyPlus/UnitHeater.hh>
#include <EnergyPlus/UtilityRoutines.hh>
#include <EnergyPlus/WaterCoils.hh>
#include <EnergyPlus/ZoneEquipmentManager.hh>

#include "Fixtures/EnergyPlusFixture.hh"

using namespace EnergyPlus;
using namespace EnergyPlus::UnitHeater;
using namespace EnergyPlus::DataEnvironment;
using namespace EnergyPlus::DataHeatBalance;
using namespace EnergyPlus::DataPlant;
using namespace EnergyPlus::DataSizing;
using namespace EnergyPlus::DataSurfaces;
using namespace EnergyPlus::DataZoneEquipment;
using namespace EnergyPlus::DataZoneEnergyDemands;
using namespace EnergyPlus::Fans;
using namespace EnergyPlus::HeatBalanceManager;
using namespace EnergyPlus::OutputProcessor;
using namespace EnergyPlus::OutputReportPredefined;
using namespace EnergyPlus::Psychrometrics;
using namespace EnergyPlus::SimulationManager;
using namespace EnergyPlus::SizingManager;
using namespace EnergyPlus::SurfaceGeometry;
using namespace EnergyPlus::WaterCoils;

TEST_F(EnergyPlusFixture, UnitHeater_HWHeatingCoilUAAutoSizingTest)
{

    bool ErrorsFound(false);          // function returns true on error
    bool FirstHVACIteration(true);    // TRUE if 1st HVAC simulation of system timestep
    int UnitHeatNum(1);               // unit heat index
    int ZoneNum(1);                   // zone index
    int CoilNum(1);                   // heating coil index
    int PltSizHeatNum(0);             // hot water plant loop index
    Real64 HWMaxVolFlowRate(0.0);     // hot water coil water volume flow rate, m3/s
    Real64 CpHW(0.0);                 // hot water specific heat, J/kg K
    Real64 HWPlantDeltaTDesign(0.0);  // hot water plant loop design temp difference, deltaC
    Real64 HWCoilDesignCapacity(0.0); // hot water heating coil design capacity (W)
    Real64 HWDensity(0.0);            // hot water density, kg/m3

    std::string const idf_objects = delimited_string({
        "Timestep,4;",

        "Building,",
        "  NONE,                    !- Name",
        "  0.0,                     !- North Axis {deg}",
        "  Suburbs,                 !- Terrain",
        "  0.039999999,             !- Loads Convergence Tolerance Value {W}",
        "  0.4,                     !- Temperature Convergence Tolerance Value {deltaC}",
        "  MinimalShadowing,        !- Solar Distribution",
        "  25,                      !- Maximum Number of Warmup Days",
        "  6;                       !- Minimum Number of Warmup Days",

        "HeatBalanceAlgorithm,ConductionTransferFunction;",

        "SurfaceConvectionAlgorithm:Inside,TARP;",

        "SurfaceConvectionAlgorithm:Outside,DOE-2;",

        "RunPeriod,",
        "  WinterDay,               !- Name",
        "  1,                       !- Begin Month",
        "  14,                      !- Begin Day of Month",
        "  ,                        !- Begin Year",
        "  1,                       !- End Month",
        "  14,                      !- End Day of Month",
        "  ,                        !- End Year",
        "  Tuesday,                 !- Day of Week for Start Day",
        "  Yes,                     !- Use Weather File Holidays and Special Days",
        "  Yes,                     !- Use Weather File Daylight Saving Period",
        "  No,                      !- Apply Weekend Holiday Rule",
        "  Yes,                     !- Use Weather File Rain Indicators",
        "  Yes;                     !- Use Weather File Snow Indicators",

        "Site:Location,",
        "  CHICAGO_IL_USA TMY2-94846,  !- Name",
        "  41.78,                   !- Latitude {deg}",
        "  -87.75,                  !- Longitude {deg}",
        "  -6.0,                    !- Time Zone {hr}",
        "  190.0;                   !- Elevation {m}",

        "SimulationControl,",
        "  Yes,                     !- Do Zone Sizing Calculation",
        "  No,                      !- Do System Sizing Calculation",
        "  No,                      !- Do Plant Sizing Calculation",
        "  No,                      !- Run Simulation for Sizing Periods",
        "  No,                      !- Run Simulation for Weather File Run Periods",
        "  ,                        !- Do HVAC Sizing Simulation for Sizing Periods",
        "  ;                        !- Maximum Number of HVAC Sizing Simulation Passes",

        "SizingPeriod:DesignDay,",
        "  CHICAGO_IL_USA Annual Heating 99% Design Conditions DB,  !- Name",
        "  1,                       !- Month",
        "  21,                      !- Day of Month",
        "  WinterDesignDay,         !- Day Type",
        "  -17.3,                   !- Maximum Dry-Bulb Temperature {C}",
        "  0.0,                     !- Daily Dry-Bulb Temperature Range {deltaC}",
        "  ,                        !- Dry-Bulb Temperature Range Modifier Type",
        "  ,                        !- Dry-Bulb Temperature Range Modifier Day Schedule Name",
        "  WetBulb,                 !- Humidity Condition Type",
        "  -17.3,                   !- Wetbulb or DewPoint at Maximum Dry-Bulb {C}",
        "  ,                        !- Humidity Condition Day Schedule Name",
        "  ,                        !- Humidity Ratio at Maximum Dry-Bulb {kgWater/kgDryAir}",
        "  ,                        !- Enthalpy at Maximum Dry-Bulb {J/kg}",
        "  ,                        !- Daily Wet-Bulb Temperature Range {deltaC}",
        "  99063.0,                 !- Barometric Pressure {Pa}",
        "  4.9,                     !- Wind Speed {m/s}",
        "  270.0,                   !- Wind Direction {deg}",
        "  No,                      !- Rain Indicator",
        "  No,                      !- Snow Indicator",
        "  No,                      !- Daylight Saving Time Indicator",
        "  ASHRAEClearSky,          !- Solar Model Indicator",
        "  ,                        !- Beam Solar Day Schedule Name",
        "  ,                        !- Diffuse Solar Day Schedule Name",
        "  ,                        !- ASHRAE Clear Sky Optical Depth for Beam Irradiance (taub) {dimensionless}",
        "  ,                        !- ASHRAE Clear Sky Optical Depth for Diffuse Irradiance (taud) {dimensionless}",
        "  1.0;                     !- Sky Clearness",

        "SizingPeriod:DesignDay,",
        "  CHICAGO_IL_USA Annual Cooling 1% Design Conditions DB/MCWB,  !- Name",
        "  7,                       !- Month",
        "  21,                      !- Day of Month",
        "  SummerDesignDay,         !- Day Type",
        "  31.5,                    !- Maximum Dry-Bulb Temperature {C}",
        "  10.7,                    !- Daily Dry-Bulb Temperature Range {deltaC}",
        "  ,                        !- Dry-Bulb Temperature Range Modifier Type",
        "  ,                        !- Dry-Bulb Temperature Range Modifier Day Schedule Name",
        "  WetBulb,                 !- Humidity Condition Type",
        "  23.0,                    !- Wetbulb or DewPoint at Maximum Dry-Bulb {C}",
        "  ,                        !- Humidity Condition Day Schedule Name",
        "  ,                        !- Humidity Ratio at Maximum Dry-Bulb {kgWater/kgDryAir}",
        "  ,                        !- Enthalpy at Maximum Dry-Bulb {J/kg}",
        "  ,                        !- Daily Wet-Bulb Temperature Range {deltaC}",
        "  99063.0,                 !- Barometric Pressure {Pa}",
        "  5.3,                     !- Wind Speed {m/s}",
        "  230.0,                   !- Wind Direction {deg}",
        "  No,                      !- Rain Indicator",
        "  No,                      !- Snow Indicator",
        "  No,                      !- Daylight Saving Time Indicator",
        "  ASHRAEClearSky,          !- Solar Model Indicator",
        "  ,                        !- Beam Solar Day Schedule Name",
        "  ,                        !- Diffuse Solar Day Schedule Name",
        "  ,                        !- ASHRAE Clear Sky Optical Depth for Beam Irradiance (taub) {dimensionless}",
        "  ,                        !- ASHRAE Clear Sky Optical Depth for Diffuse Irradiance (taud) {dimensionless}",
        "  1.0;                     !- Sky Clearness",

        "Material,",
        "  A1 - 1 IN STUCCO,        !- Name",
        "  Smooth,                  !- Roughness",
        "  0.025389841,             !- Thickness {m}",
        "  0.6918309,               !- Conductivity {W/m-K}",
        "  1858.142,                !- Density {kg/m3}",
        "  836.8,                   !- Specific Heat {J/kg-K}",
        "  0.9,                     !- Thermal Absorptance",
        "  0.92,                    !- Solar Absorptance",
        "  0.92;                    !- Visible Absorptance",

        "Material,",
        "  C4 - 4 IN COMMON BRICK,  !- Name",
        "  Rough,                   !- Roughness",
        "  0.1014984,               !- Thickness {m}",
        "  0.7264224,               !- Conductivity {W/m-K}",
        "  1922.216,                !- Density {kg/m3}",
        "  836.8,                   !- Specific Heat {J/kg-K}",
        "  0.9,                     !- Thermal Absorptance",
        "  0.76,                    !- Solar Absorptance",
        "  0.76;                    !- Visible Absorptance",

        "Material,",
        "  E1 - 3 / 4 IN PLASTER OR GYP BOARD,  !- Name",
        "  Smooth,                  !- Roughness",
        "  0.01905,                 !- Thickness {m}",
        "  0.7264224,               !- Conductivity {W/m-K}",
        "  1601.846,                !- Density {kg/m3}",
        "  836.8,                   !- Specific Heat {J/kg-K}",
        "  0.9,                     !- Thermal Absorptance",
        "  0.92,                    !- Solar Absorptance",
        "  0.92;                    !- Visible Absorptance",

        "Material,",
        "  C6 - 8 IN CLAY TILE,     !- Name",
        "  Smooth,                  !- Roughness",
        "  0.2033016,               !- Thickness {m}",
        "  0.5707605,               !- Conductivity {W/m-K}",
        "  1121.292,                !- Density {kg/m3}",
        "  836.8,                   !- Specific Heat {J/kg-K}",
        "  0.9,                     !- Thermal Absorptance",
        "  0.82,                    !- Solar Absorptance",
        "  0.82;                    !- Visible Absorptance",

        "Material,",
        "  C10 - 8 IN HW CONCRETE,  !- Name",
        "  MediumRough,             !- Roughness",
        "  0.2033016,               !- Thickness {m}",
        "  1.729577,                !- Conductivity {W/m-K}",
        "  2242.585,                !- Density {kg/m3}",
        "  836.8,                   !- Specific Heat {J/kg-K}",
        "  0.9,                     !- Thermal Absorptance",
        "  0.65,                    !- Solar Absorptance",
        "  0.65;                    !- Visible Absorptance",

        "Material,",
        "  E2 - 1 / 2 IN SLAG OR STONE,  !- Name",
        "  Rough,                   !- Roughness",
        "  0.012710161,             !- Thickness {m}",
        "  1.435549,                !- Conductivity {W/m-K}",
        "  881.0155,                !- Density {kg/m3}",
        "  1673.6,                  !- Specific Heat {J/kg-K}",
        "  0.9,                     !- Thermal Absorptance",
        "  0.55,                    !- Solar Absorptance",
        "  0.55;                    !- Visible Absorptance",

        "Material,",
        "  E3 - 3 / 8 IN FELT AND MEMBRANE,  !- Name",
        "  Rough,                   !- Roughness",
        "  0.0095402403,            !- Thickness {m}",
        "  0.1902535,               !- Conductivity {W/m-K}",
        "  1121.292,                !- Density {kg/m3}",
        "  1673.6,                  !- Specific Heat {J/kg-K}",
        "  0.9,                     !- Thermal Absorptance",
        "  0.75,                    !- Solar Absorptance",
        "  0.75;                    !- Visible Absorptance",

        "Material,",
        "  B5 - 1 IN DENSE INSULATION,  !- Name",
        "  VeryRough,               !- Roughness",
        "  0.025389841,             !- Thickness {m}",
        "  0.04323943,              !- Conductivity {W/m-K}",
        "  91.30524,                !- Density {kg/m3}",
        "  836.8,                   !- Specific Heat {J/kg-K}",
        "  0.9,                     !- Thermal Absorptance",
        "  0.5,                     !- Solar Absorptance",
        "  0.5;                     !- Visible Absorptance",

        "Material,",
        "  C12 - 2 IN HW CONCRETE,  !- Name",
        "  MediumRough,             !- Roughness",
        "  0.050901599,             !- Thickness {m}",
        "  1.729577,                !- Conductivity {W/m-K}",
        "  2242.585,                !- Density {kg/m3}",
        "  836.8,                   !- Specific Heat {J/kg-K}",
        "  0.9,                     !- Thermal Absorptance",
        "  0.65,                    !- Solar Absorptance",
        "  0.65;                    !- Visible Absorptance",

        "WindowMaterial:Glazing,",
        "  WIN-LAY-GLASS-LIGHT,     !- Name",
        "  SpectralAverage,         !- Optical Data Type",
        "  ,                        !- Window Glass Spectral Data Set Name",
        "  0.003,                   !- Thickness {m}",
        "  0.9,                     !- Solar Transmittance at Normal Incidence",
        "  0.031,                   !- Front Side Solar Reflectance at Normal Incidence",
        "  0.031,                   !- Back Side Solar Reflectance at Normal Incidence",
        "  0.9,                     !- Visible Transmittance at Normal Incidence",
        "  0.05,                    !- Front Side Visible Reflectance at Normal Incidence",
        "  0.05,                    !- Back Side Visible Reflectance at Normal Incidence",
        "  0.0,                     !- Infrared Transmittance at Normal Incidence",
        "  0.84,                    !- Front Side Infrared Hemispherical Emissivity",
        "  0.84,                    !- Back Side Infrared Hemispherical Emissivity",
        "  0.9;                     !- Conductivity {W/m-K}",

        "Construction,",
        "  EXTWALL80,               !- Name",
        "  A1 - 1 IN STUCCO,        !- Outside Layer",
        "  C4 - 4 IN COMMON BRICK,  !- Layer 2",
        "  E1 - 3 / 4 IN PLASTER OR GYP BOARD;  !- Layer 3",

        "Construction,",
        "  PARTITION06,             !- Name",
        "  E1 - 3 / 4 IN PLASTER OR GYP BOARD,  !- Outside Layer",
        "  C6 - 8 IN CLAY TILE,     !- Layer 2",
        "  E1 - 3 / 4 IN PLASTER OR GYP BOARD;  !- Layer 3",

        "Construction,",
        "  FLOOR SLAB 8 IN,         !- Name",
        "  C10 - 8 IN HW CONCRETE;  !- Outside Layer",

        "Construction,",
        "  ROOF34,                  !- Name",
        "  E2 - 1 / 2 IN SLAG OR STONE,  !- Outside Layer",
        "  E3 - 3 / 8 IN FELT AND MEMBRANE,  !- Layer 2",
        "  B5 - 1 IN DENSE INSULATION,  !- Layer 3",
        "  C12 - 2 IN HW CONCRETE;  !- Layer 4",

        "Construction,",
        "  WIN-CON-LIGHT,           !- Name",
        "  WIN-LAY-GLASS-LIGHT;     !- Outside Layer",

        "ScheduleTypeLimits,",
        "  Any Number;              !- Name",

        "ScheduleTypeLimits,",
        "  Fraction,                !- Name",
        "  0.0,                     !- Lower Limit Value {BasedOnField A3}",
        "  1.0,                     !- Upper Limit Value {BasedOnField A3}",
        "  Continuous;              !- Numeric Type",

        "ScheduleTypeLimits,",
        "  Temperature,             !- Name",
        "  -60.0,                   !- Lower Limit Value {BasedOnField A3}",
        "  200.0,                   !- Upper Limit Value {BasedOnField A3}",
        "  Continuous,              !- Numeric Type",
        "  Temperature;             !- Unit Type",

        "ScheduleTypeLimits,",
        "  Control Type,            !- Name",
        "  0.0,                     !- Lower Limit Value {BasedOnField A3}",
        "  4.0,                     !- Upper Limit Value {BasedOnField A3}",
        "  Discrete;                !- Numeric Type",

        "ScheduleTypeLimits,",
        "  On/Off,                  !- Name",
        "  0.0,                     !- Lower Limit Value {BasedOnField A3}",
        "  1.0,                     !- Upper Limit Value {BasedOnField A3}",
        "  Discrete;                !- Numeric Type",

        "Schedule:Compact,",
        "  Activity Sch,            !- Name",
        "  Any Number,              !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: Alldays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  131.80;                  !- Field",

        "Schedule:Compact,",
        "  Work Eff Sch,            !- Name",
        "  Any Number,              !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: Alldays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  0.00;                    !- Field",

        "Schedule:Compact,",
        "  Clothing Sch,            !- Name",
        "  Any Number,              !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: Alldays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  1.00;                    !- Field",

        "Schedule:Compact,",
        "  Air Velo Sch,            !- Name",
        "  Any Number,              !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: Alldays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  0.137;                   !- Field",

        "Schedule:Compact,",
        "  Office Occupancy,        !- Name",
        "  ANY NUMBER,              !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: Weekdays,           !- Field",
        "  Until: 6:00,             !- Field",
        "  0.00,                    !- Field",
        "  Until: 7:00,             !- Field",
        "  0.10,                    !- Field",
        "  Until: 8:00,             !- Field",
        "  0.50,                    !- Field",
        "  Until: 12:00,            !- Field",
        "  1.00,                    !- Field",
        "  Until: 13:00,            !- Field",
        "  0.50,                    !- Field",
        "  Until: 16:00,            !- Field",
        "  1.00,                    !- Field",
        "  Until: 17:00,            !- Field",
        "  0.50,                    !- Field",
        "  Until: 18:00,            !- Field",
        "  0.10,                    !- Field",
        "  Until: 24:00,            !- Field",
        "  0.00,                    !- Field",
        "  For: Weekends Holidays CustomDay1 CustomDay2,  !- Field",
        "  Until: 24:00,            !- Field",
        "  0.00,                    !- Field",
        "  For: SummerDesignDay,    !- Field",
        "  Until: 24:00,            !- Field",
        "  1.00,                    !- Field",
        "  For: WinterDesignDay,    !- Field",
        "  Until: 24:00,            !- Field",
        "  5.00E-002;               !- Field",

        "Schedule:Compact,",
        "  Intermittent,            !- Name",
        "  ANY NUMBER,              !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: Weekdays,           !- Field",
        "  Until: 8:00,             !- Field",
        "  0.00,                    !- Field",
        "  Until: 18:00,            !- Field",
        "  1.00,                    !- Field",
        "  Until: 24:00,            !- Field",
        "  0.00,                    !- Field",
        "  For: Weekends Holidays CustomDay1 CustomDay2,  !- Field",
        "  Until: 24:00,            !- Field",
        "  0.00,                    !- Field",
        "  For: SummerDesignDay,    !- Field",
        "  Until: 24:00,            !- Field",
        "  1.00,                    !- Field",
        "  For: WinterDesignDay,    !- Field",
        "  Until: 24:00,            !- Field",
        "  0.15;                    !- Field",

        "Schedule:Compact,",
        "  Office Lighting,         !- Name",
        "  ANY NUMBER,              !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: Weekdays,           !- Field",
        "  Until: 6:00,             !- Field",
        "  5.00E-002,               !- Field",
        "  Until: 7:00,             !- Field",
        "  0.20,                    !- Field",
        "  Until: 17:00,            !- Field",
        "  1.00,                    !- Field",
        "  Until: 18:00,            !- Field",
        "  0.50,                    !- Field",
        "  Until: 24:00,            !- Field",
        "  5.00E-002,               !- Field",
        "  For: Weekends Holidays CustomDay1 CustomDay2,  !- Field",
        "  Until: 24:00,            !- Field",
        "  5.00E-002,               !- Field",
        "  For: SummerDesignDay,    !- Field",
        "  Until: 24:00,            !- Field",
        "  1.00,                    !- Field",
        "  For: WinterDesignDay,    !- Field",
        "  Until: 24:00,            !- Field",
        "  0.15;                    !- Field",

        "Schedule:Compact,",
        "  ON,                      !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: Alldays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  1.00;                    !- Field",

        "Schedule:Compact,",
        "  HW LOOP TEMP SCHEDULE,   !- Name",
        "  TEMPERATURE,             !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: Alldays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  82.00;                   !- Field",

        "Schedule:Compact,",
        "  FANANDCOILAVAILSCHED,    !- Name",
        "  FRACTION,                !- Schedule Type Limits Name",
        "  Through: 3/31,           !- Field",
        "  For: Alldays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  1.00,                    !- Field",
        "  Through: 9/30,           !- Field",
        "  For: Weekdays,           !- Field",
        "  Until: 7:00,             !- Field",
        "  0.00,                    !- Field",
        "  Until: 17:00,            !- Field",
        "  1.00,                    !- Field",
        "  Until: 24:00,            !- Field",
        "  0.00,                    !- Field",
        "  For: Weekends Holidays CustomDay1 CustomDay2,  !- Field",
        "  Until: 24:00,            !- Field",
        "  0.00,                    !- Field",
        "  For: SummerDesignDay WinterDesignDay,  !- Field",
        "  Until: 24:00,            !- Field",
        "  1.00,                    !- Field",
        "  Through: 12/31,          !- Field",
        "  For: Alldays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  1.00;                    !- Field",

        "Schedule:Compact,",
        "  HEATING SETPOINTS,       !- Name",
        "  TEMPERATURE,             !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: Weekdays Weekends Holidays CustomDay1 CustomDay2,  !- Field",
        "  Until: 7:00,             !- Field",
        "  15.00,                   !- Field",
        "  Until: 17:00,            !- Field",
        "  20.00,                   !- Field",
        "  Until: 24:00,            !- Field",
        "  15.00,                   !- Field",
        "  For: SummerDesignDay,    !- Field",
        "  Until: 24:00,            !- Field",
        "  15.00,                   !- Field",
        "  For: WinterDesignDay,    !- Field",
        "  Until: 24:00,            !- Field",
        "  20.00;                   !- Field",

        "Schedule:Compact,",
        "  COOLING SETPOINTS,       !- Name",
        "  TEMPERATURE,             !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: Weekdays Weekends Holidays CustomDay1 CustomDay2,  !- Field",
        "  Until: 7:00,             !- Field",
        "  30.00,                   !- Field",
        "  Until: 17:00,            !- Field",
        "  24.00,                   !- Field",
        "  Until: 24:00,            !- Field",
        "  30.00,                   !- Field",
        "  For: SummerDesignDay,    !- Field",
        "  Until: 24:00,            !- Field",
        "  24.00,                   !- Field",
        "  For: WinterDesignDay,    !- Field",
        "  Until: 24:00,            !- Field",
        "  50.00;                   !- Field",

        "Schedule:Compact,",
        "  ZONE CONTROL TYPE SCHED,  !- Name",
        "  CONTROL TYPE,            !- Schedule Type Limits Name",
        "  Through: 3/31,           !- Field",
        "  For: Alldays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  1,                       !- Field",
        "  Through: 9/30,           !- Field",
        "  For: Alldays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  2,                       !- Field",
        "  Through: 12/31,          !- Field",
        "  For: Alldays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  1;                       !- Field",

        "Schedule:Compact,",
        "  UNITHEATAVAILABILITY,    !- Name",
        "  ANY NUMBER,              !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: Alldays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  1.00;                    !- Field",

        "Site:GroundTemperature:BuildingSurface,20.03,20.03,20.13,20.3,20.43,20.52,20.62,20.77,20.78,20.55,20.44,20.2;",

        "Zone,",
        "  EAST ZONE,               !- Name",
        "  0.0,                     !- Direction of Relative North {deg}",
        "  0.0,                     !- X Origin {m}",
        "  0.0,                     !- Y Origin {m}",
        "  0.0,                     !- Z Origin {m}",
        "  1,                       !- Type",
        "  1,                       !- Multiplier",
        "  Autocalculate,           !- Ceiling Height {m}",
        "  Autocalculate;           !- Volume {m3}",

        "People,",
        "  EAST ZONE,               !- Name",
        "  EAST ZONE,               !- Zone or ZoneList or Space or SpaceList Name",
        "  Office Occupancy,        !- Number of People Schedule Name",
        "  People,                  !- Number of People Calculation Method",
        "  3.0,                     !- Number of People",
        "  ,                        !- People per Floor Area {person/m2}",
        "  ,                        !- Floor Area per Person {m2/person}",
        "  0.3,                     !- Fraction Radiant",
        "  ,                        !- Sensible Heat Fraction",
        "  Activity Sch,            !- Activity Level Schedule Name",
        "  3.82e-08,                !- Carbon Dioxide Generation Rate {m3/s-W}",
        "  ,                        !- Enable ASHRAE 55 Comfort Warnings",
        "  EnclosureAveraged,       !- Mean Radiant Temperature Calculation Type",
        "  ,                        !- Surface Name/Angle Factor List Name",
        "  Work Eff Sch,            !- Work Efficiency Schedule Name",
        "  ClothingInsulationSchedule,  !- Clothing Insulation Calculation Method",
        "  ,                        !- Clothing Insulation Calculation Method Schedule Name",
        "  Clothing Sch,            !- Clothing Insulation Schedule Name",
        "  Air Velo Sch,            !- Air Velocity Schedule Name",
        "  Fanger;                  !- Thermal Comfort Model 1 Type",

        "Lights,",
        "  EAST ZONE Lights 1,      !- Name",
        "  EAST ZONE,               !- Zone or ZoneList or Space or SpaceList Name",
        "  Office Lighting,         !- Schedule Name",
        "  LightingLevel,           !- Design Level Calculation Method",
        "  1464.375,                !- Lighting Level {W}",
        "  ,                        !- Watts per Floor Area {W/m2}",
        "  ,                        !- Watts per Person {W/person}",
        "  0.0,                     !- Return Air Fraction",
        "  0.2,                     !- Fraction Radiant",
        "  0.2,                     !- Fraction Visible",
        "  0.0,                     !- Fraction Replaceable",
        "  GeneralLights;           !- End-Use Subcategory",

        "ElectricEquipment,",
        "  EAST ZONE ElecEq 1,      !- Name",
        "  EAST ZONE,               !- Zone or ZoneList or Space or SpaceList Name",
        "  Intermittent,            !- Schedule Name",
        "  EquipmentLevel,          !- Design Level Calculation Method",
        "  1464.375,                !- Design Level {W}",
        "  ,                        !- Watts per Floor Area {W/m2}",
        "  ,                        !- Watts per Person {W/person}",
        "  0.0,                     !- Fraction Latent",
        "  0.3,                     !- Fraction Radiant",
        "  0.0;                     !- Fraction Lost",

        "GlobalGeometryRules,",
        "  UpperLeftCorner,         !- Starting Vertex Position",
        "  Counterclockwise,        !- Vertex Entry Direction",
        "  World;                   !- Coordinate System",

        "BuildingSurface:Detailed,",
        "  Zn002:Wall001,           !- Name",
        "  Wall,                    !- Surface Type",
        "  EXTWALL80,               !- Construction Name",
        "  EAST ZONE,               !- Zone Name",
        "  ,                        !- Space Name",
        "  Outdoors,                !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  SunExposed,              !- Sun Exposure",
        "  WindExposed,             !- Wind Exposure",
        "  0.5,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  12.192,                  !- Vertex X-coordinate {m}",
        "  6.096,                   !- Vertex Y-coordinate {m}",
        "  3.048,                   !- Vertex Z-coordinate {m}",
        "  12.192,                  !- Vertex X-coordinate {m}",
        "  6.096,                   !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  9.144,                   !- Vertex X-coordinate {m}",
        "  6.096,                   !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  9.144,                   !- Vertex X-coordinate {m}",
        "  6.096,                   !- Vertex Y-coordinate {m}",
        "  3.048;                   !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Zn002:Wall002,           !- Name",
        "  Wall,                    !- Surface Type",
        "  EXTWALL80,               !- Construction Name",
        "  EAST ZONE,               !- Zone Name",
        "  ,                        !- Space Name",
        "  Outdoors,                !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  SunExposed,              !- Sun Exposure",
        "  WindExposed,             !- Wind Exposure",
        "  0.5,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  6.096,                   !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  3.048,                   !- Vertex Z-coordinate {m}",
        "  6.096,                   !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  12.192,                  !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  12.192,                  !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  3.048;                   !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Zn002:Wall003,           !- Name",
        "  Wall,                    !- Surface Type",
        "  EXTWALL80,               !- Construction Name",
        "  EAST ZONE,               !- Zone Name",
        "  ,                        !- Space Name",
        "  Outdoors,                !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  SunExposed,              !- Sun Exposure",
        "  WindExposed,             !- Wind Exposure",
        "  0.5,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  12.192,                  !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  3.048,                   !- Vertex Z-coordinate {m}",
        "  12.192,                  !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  12.192,                  !- Vertex X-coordinate {m}",
        "  6.096,                   !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  12.192,                  !- Vertex X-coordinate {m}",
        "  6.096,                   !- Vertex Y-coordinate {m}",
        "  3.048;                   !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Zn002:Wall004,           !- Name",
        "  Wall,                    !- Surface Type",
        "  EXTWALL80,               !- Construction Name",
        "  EAST ZONE,               !- Zone Name",
        "  ,                        !- Space Name",
        "  Outdoors,                !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  SunExposed,              !- Sun Exposure",
        "  WindExposed,             !- Wind Exposure",
        "  0.5,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  6.096,                   !- Vertex X-coordinate {m}",
        "  6.096,                   !- Vertex Y-coordinate {m}",
        "  3.048,                   !- Vertex Z-coordinate {m}",
        "  6.096,                   !- Vertex X-coordinate {m}",
        "  6.096,                   !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  6.096,                   !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  6.096,                   !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  3.048;                   !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Zn002:Wall005,           !- Name",
        "  Wall,                    !- Surface Type",
        "  EXTWALL80,               !- Construction Name",
        "  EAST ZONE,               !- Zone Name",
        "  ,                        !- Space Name",
        "  Outdoors,                !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  SunExposed,              !- Sun Exposure",
        "  WindExposed,             !- Wind Exposure",
        "  0.5,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  9.144,                   !- Vertex X-coordinate {m}",
        "  6.096,                   !- Vertex Y-coordinate {m}",
        "  3.048,                   !- Vertex Z-coordinate {m}",
        "  9.144,                   !- Vertex X-coordinate {m}",
        "  6.096,                   !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  6.096,                   !- Vertex X-coordinate {m}",
        "  6.096,                   !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  6.096,                   !- Vertex X-coordinate {m}",
        "  6.096,                   !- Vertex Y-coordinate {m}",
        "  3.048;                   !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Zn002:Flr001,            !- Name",
        "  Floor,                   !- Surface Type",
        "  FLOOR SLAB 8 IN,         !- Construction Name",
        "  EAST ZONE,               !- Zone Name",
        "  ,                        !- Space Name",
        "  Ground,                  !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  NoSun,                   !- Sun Exposure",
        "  NoWind,                  !- Wind Exposure",
        "  1.0,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  6.096,                   !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  6.096,                   !- Vertex X-coordinate {m}",
        "  6.096,                   !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  12.192,                  !- Vertex X-coordinate {m}",
        "  6.096,                   !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  12.192,                  !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0;                     !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Zn002:Roof001,           !- Name",
        "  Roof,                    !- Surface Type",
        "  ROOF34,                  !- Construction Name",
        "  EAST ZONE,               !- Zone Name",
        "  ,                        !- Space Name",
        "  Outdoors,                !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  SunExposed,              !- Sun Exposure",
        "  WindExposed,             !- Wind Exposure",
        "  0.0,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  6.096,                   !- Vertex X-coordinate {m}",
        "  6.096,                   !- Vertex Y-coordinate {m}",
        "  3.048,                   !- Vertex Z-coordinate {m}",
        "  6.096,                   !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  3.048,                   !- Vertex Z-coordinate {m}",
        "  12.192,                  !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  3.048,                   !- Vertex Z-coordinate {m}",
        "  12.192,                  !- Vertex X-coordinate {m}",
        "  6.096,                   !- Vertex Y-coordinate {m}",
        "  3.048;                   !- Vertex Z-coordinate {m}",

        "Sizing:Parameters,",
        "  1.5,                     !- Heating Sizing Factor",
        "  1.5,                     !- Cooling Sizing Factor",
        "  ;                        !- Timesteps in Averaging Window",

        "ZoneInfiltration:DesignFlowRate,",
        "  EAST ZONE Infiltration,  !- Name",
        "  EAST ZONE,               !- Zone or ZoneList or Space or SpaceList Name",
        "  UNITHEATAVAILABILITY,    !- Schedule Name",
        "  Flow/Zone,               !- Design Flow Rate Calculation Method",
        "  0.00275772902001411,     !- Design Flow Rate {m3/s}",
        "  ,                        !- Flow Rate per Floor Area {m3/s-m2}",
        "  ,                        !- Flow Rate per Exterior Surface Area {m3/s-m2}",
        "  ,                        !- Air Changes per Hour {1/hr}",
        "  ,                        !- Constant Term Coefficient",
        "  ,                        !- Temperature Term Coefficient",
        "  0.224051539,             !- Velocity Term Coefficient",
        "  ;                        !- Velocity Squared Term Coefficient",

        "Sizing:Zone,",
        "  EAST ZONE,               !- Zone or ZoneList Name",
        "  SupplyAirTemperature,    !- Zone Cooling Design Supply Air Temperature Input Method",
        "  12.78,                   !- Zone Cooling Design Supply Air Temperature {C}",
        "  ,                        !- Zone Cooling Design Supply Air Temperature Difference {deltaC}",
        "  SupplyAirTemperature,    !- Zone Heating Design Supply Air Temperature Input Method",
        "  32.22,                   !- Zone Heating Design Supply Air Temperature {C}",
        "  ,                        !- Zone Heating Design Supply Air Temperature Difference {deltaC}",
        "  0.009,                   !- Zone Cooling Design Supply Air Humidity Ratio {kgWater/kgDryAir}",
        "  0.009,                   !- Zone Heating Design Supply Air Humidity Ratio {kgWater/kgDryAir}",
        "  SZ DSOA EAST ZONE,       !- Design Specification Outdoor Air Object Name",
        "  ,                        !- Zone Heating Sizing Factor",
        "  ,                        !- Zone Cooling Sizing Factor",
        "  DesignDay,               !- Cooling Design Air Flow Method",
        "  0.0,                     !- Cooling Design Air Flow Rate {m3/s}",
        "  0.000762,                !- Cooling Minimum Air Flow per Zone Floor Area {m3/s-m2}",
        "  0.0,                     !- Cooling Minimum Air Flow {m3/s}",
        "  0.0,                     !- Cooling Minimum Air Flow Fraction",
        "  DesignDay,               !- Heating Design Air Flow Method",
        "  0.0,                     !- Heating Design Air Flow Rate {m3/s}",
        "  0.002032,                !- Heating Maximum Air Flow per Zone Floor Area {m3/s-m2}",
        "  0.1415762,               !- Heating Maximum Air Flow {m3/s}",
        "  0.3,                     !- Heating Maximum Air Flow Fraction",
        "  SZ DSOA EAST ZONE Design Spec Zone Air Dist,  !- Design Specification Zone Air Distribution Object Name",
        "  ,                        !- Account for Dedicated Outdoor Air System",
        "  ,                        !- Dedicated Outdoor Air System Control Strategy",
        "  ,                        !- Dedicated Outdoor Air Low Setpoint Temperature for Design {C}",
        "  ,                        !- Dedicated Outdoor Air High Setpoint Temperature for Design {C}",
        "  ,                        !- Zone Load Sizing Method",
        "  ,                        !- Zone Latent Cooling Design Supply Air Humidity Ratio Input Method",
        "  ,                        !- Zone Dehumidification Design Supply Air Humidity Ratio {kgWater/kgDryAir}",
        "  ,                        !- Zone Cooling Design Supply Air Humidity Ratio Difference {kgWater/kgDryAir}",
        "  ,                        !- Zone Latent Heating Design Supply Air Humidity Ratio Input Method",
        "  ,                        !- Zone Humidification Design Supply Air Humidity Ratio {kgWater/kgDryAir}",
        "  ;                        !- Zone Humidification Design Supply Air Humidity Ratio Difference {kgWater/kgDryAir}",

        "DesignSpecification:ZoneAirDistribution,",
        "  SZ DSOA EAST ZONE Design Spec Zone Air Dist,  !- Name",
        "  1.0,                     !- Zone Air Distribution Effectiveness in Cooling Mode {dimensionless}",
        "  1.0;                     !- Zone Air Distribution Effectiveness in Heating Mode {dimensionless}",

        "DesignSpecification:OutdoorAir,",
        "  SZ DSOA EAST ZONE,       !- Name",
        "  Sum,                     !- Outdoor Air Method",
        "  0.00780950959206505,     !- Outdoor Air Flow per Person {m3/s-person}",
        "  0.012032,                !- Outdoor Air Flow per Zone Floor Area {m3/s-m2}",
        "  0.0,                     !- Outdoor Air Flow per Zone {m3/s}",
        "  0.0;                     !- Outdoor Air Flow Air Changes per Hour {1/hr}",

        "Sizing:Plant,",
        "  Hot Water Loop,          !- Plant or Condenser Loop Name",
        "  Heating,                 !- Loop Type",
        "  82.2222222222222,        !- Design Loop Exit Temperature {C}",
        "  22.2222222222222;        !- Loop Design Temperature Difference {deltaC}",

        "PlantLoop,",
        "  Hot Water Loop,          !- Name",
        "  Water,                   !- Fluid Type",
        "  ,                        !- User Defined Fluid Type",
        "  Hot Loop Operation,      !- Plant Equipment Operation Scheme Name",
        "  HW Supply Outlet Node,   !- Loop Temperature Setpoint Node Name",
        "  100.0,                   !- Maximum Loop Temperature {C}",
        "  10.0,                    !- Minimum Loop Temperature {C}",
        "  Autosize,                !- Maximum Loop Flow Rate {m3/s}",
        "  0.0,                     !- Minimum Loop Flow Rate {m3/s}",
        "  Autocalculate,           !- Plant Loop Volume {m3}",
        "  HW Supply Inlet Node,    !- Plant Side Inlet Node Name",
        "  HW Supply Outlet Node,   !- Plant Side Outlet Node Name",
        "  Heating Supply Side Branches,  !- Plant Side Branch List Name",
        "  Heating Supply Side Connectors,  !- Plant Side Connector List Name",
        "  HW Demand Inlet Node,    !- Demand Side Inlet Node Name",
        "  HW Demand Outlet Node,   !- Demand Side Outlet Node Name",
        "  Heating Demand Side Branches,  !- Demand Side Branch List Name",
        "  Heating Demand Side Connectors,  !- Demand Side Connector List Name",
        "  Optimal,                 !- Load Distribution Scheme",
        "  ,                        !- Availability Manager List Name",
        "  ,                        !- Plant Loop Demand Calculation Scheme",
        "  ,                        !- Common Pipe Simulation",
        "  ,                        !- Pressure Simulation Type",
        "  2.0;                     !- Loop Circulation Time {minutes}",

        "SetpointManager:Scheduled,",
        "  Hot Water Loop Setpoint Manager,  !- Name",
        "  Temperature,             !- Control Variable",
        "  HW Loop Temp Schedule,   !- Schedule Name",
        "  Hot Water Loop Setpoint Node List;  !- Setpoint Node or NodeList Name",

        "NodeList,",
        "  Hot Water Loop Setpoint Node List,  !- Name",
        "  HW Supply Outlet Node;   !- Node Name",

        "BranchList,",
        "  Heating Supply Side Branches,  !- Name",
        "  Heating Supply Inlet Branch,  !- Branch Name",
        "  Heating Purchased Hot Water Branch,  !- Branch Name",
        "  Heating Supply Bypass Branch,  !- Branch Name",
        "  Heating Supply Outlet Branch;  !- Branch Name",

        "ConnectorList,",
        "  Heating Supply Side Connectors,  !- Name",
        "  Connector:Splitter,      !- Connector 1 Object Type",
        "  Heating Supply Splitter,  !- Connector 1 Name",
        "  Connector:Mixer,         !- Connector 2 Object Type",
        "  Heating Supply Mixer;    !- Connector 2 Name",

        "Connector:Splitter,",
        "  Heating Supply Splitter,  !- Name",
        "  Heating Supply Inlet Branch,  !- Inlet Branch Name",
        "  Heating Purchased Hot Water Branch,  !- Outlet Branch Name",
        "  Heating Supply Bypass Branch;  !- Outlet Branch Name",

        "Connector:Mixer,",
        "  Heating Supply Mixer,    !- Name",
        "  Heating Supply Outlet Branch,  !- Outlet Branch Name",
        "  Heating Purchased Hot Water Branch,  !- Inlet Branch Name",
        "  Heating Supply Bypass Branch;  !- Inlet Branch Name",

        "Branch,",
        "  Heating Supply Inlet Branch,  !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  Pump:VariableSpeed,      !- Component Object Type",
        "  HW Circ Pump,            !- Component Name",
        "  HW Supply Inlet Node,    !- Component Inlet Node Name",
        "  HW Pump Outlet Node;     !- Component Outlet Node Name",

        "Branch,",
        "  Heating Purchased Hot Water Branch,  !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  DistrictHeating:Water,   !- Component Object Type",
        "  Purchased Heating,       !- Component Name",
        "  Purchased Heat Inlet Node,  !- Component Inlet Node Name",
        "  Purchased Heat Outlet Node;  !- Component Outlet Node Name",

        "Branch,",
        "  Heating Supply Bypass Branch,  !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  Pipe:Adiabatic,          !- Component Object Type",
        "  Heating Supply Side Bypass,  !- Component Name",
        "  Heating Supply Bypass Inlet Node,  !- Component Inlet Node Name",
        "  Heating Supply Bypass Outlet Node;  !- Component Outlet Node Name",

        "Pipe:Adiabatic,",
        "  Heating Supply Side Bypass,  !- Name",
        "  Heating Supply Bypass Inlet Node,  !- Inlet Node Name",
        "  Heating Supply Bypass Outlet Node;  !- Outlet Node Name",

        "Branch,",
        "  Heating Supply Outlet Branch,  !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  Pipe:Adiabatic,          !- Component Object Type",
        "  Heating Supply Outlet,   !- Component Name",
        "  Heating Supply Exit Pipe Inlet Node,  !- Component Inlet Node Name",
        "  HW Supply Outlet Node;   !- Component Outlet Node Name",

        "Pipe:Adiabatic,",
        "  Heating Supply Outlet,   !- Name",
        "  Heating Supply Exit Pipe Inlet Node,  !- Inlet Node Name",
        "  HW Supply Outlet Node;   !- Outlet Node Name",

        "Pump:VariableSpeed,",
        "  HW Circ Pump,            !- Name",
        "  HW Supply Inlet Node,    !- Inlet Node Name",
        "  HW Pump Outlet Node,     !- Outlet Node Name",
        "  Autosize,                !- Design Maximum Flow Rate {m3/s}",
        "  179352.0,                !- Design Pump Head {Pa}",
        "  Autosize,                !- Design Power Consumption {W}",
        "  0.9,                     !- Motor Efficiency",
        "  0.0,                     !- Fraction of Motor Inefficiencies to Fluid Stream",
        "  0.0,                     !- Coefficient 1 of the Part Load Performance Curve",
        "  1.0,                     !- Coefficient 2 of the Part Load Performance Curve",
        "  0.0,                     !- Coefficient 3 of the Part Load Performance Curve",
        "  0.0,                     !- Coefficient 4 of the Part Load Performance Curve",
        "  0.0,                     !- Design Minimum Flow Rate {m3/s}",
        "  Intermittent;            !- Pump Control Type",

        "BranchList,",
        "  Heating Demand Side Branches,  !- Name",
        "  ZonesHWInletBranch,      !- Branch Name",
        "  Zone2HWBranch,           !- Branch Name",
        "  ZonesHWBypassBranch,     !- Branch Name",
        "  ZonesHWOutletBranch;     !- Branch Name",

        "ConnectorList,",
        "  Heating Demand Side Connectors,  !- Name",
        "  Connector:Splitter,      !- Connector 1 Object Type",
        "  Zones HW Splitter,       !- Connector 1 Name",
        "  Connector:Mixer,         !- Connector 2 Object Type",
        "  Zones HW Mixer;          !- Connector 2 Name",

        "Connector:Splitter,",
        "  Zones HW Splitter,       !- Name",
        "  ZonesHWInletBranch,      !- Inlet Branch Name",
        "  Zone2HWBranch,           !- Outlet Branch Name",
        "  ZonesHWBypassBranch;     !- Outlet Branch Name",

        "Connector:Mixer,",
        "  Zones HW Mixer,          !- Name",
        "  ZonesHWOutletBranch,     !- Outlet Branch Name",
        "  Zone2HWBranch,           !- Inlet Branch Name",
        "  ZonesHWBypassBranch;     !- Inlet Branch Name",

        "Branch,",
        "  ZonesHWInletBranch,      !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  Pipe:Adiabatic,          !- Component Object Type",
        "  ZonesHWInletPipe,        !- Component Name",
        "  HW Demand Inlet Node,    !- Component Inlet Node Name",
        "  HW Demand Entrance Pipe Outlet Node;  !- Component Outlet Node Name",

        "Pipe:Adiabatic,",
        "  ZonesHWInletPipe,        !- Name",
        "  HW Demand Inlet Node,    !- Inlet Node Name",
        "  HW Demand Entrance Pipe Outlet Node;  !- Outlet Node Name",

        "Branch,",
        "  ZonesHWOutletBranch,     !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  Pipe:Adiabatic,          !- Component Object Type",
        "  ZonesHWOutletPipe,       !- Component Name",
        "  HW Demand Exit Pipe Inlet Node,  !- Component Inlet Node Name",
        "  HW Demand Outlet Node;   !- Component Outlet Node Name",

        "Pipe:Adiabatic,",
        "  ZonesHWOutletPipe,       !- Name",
        "  HW Demand Exit Pipe Inlet Node,  !- Inlet Node Name",
        "  HW Demand Outlet Node;   !- Outlet Node Name",

        "Branch,",
        "  Zone2HWBranch,           !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  Coil:Heating:Water,      !- Component Object Type",
        "  Zone2UnitHeatHeatingCoil,  !- Component Name",
        "  Zone2UnitHeatHWInletNode,  !- Component Inlet Node Name",
        "  Zone2UnitHeatHWOutletNode;  !- Component Outlet Node Name",

        "Branch,",
        "  ZonesHWBypassBranch,     !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  Pipe:Adiabatic,          !- Component Object Type",
        "  ZonesHWBypassPipe,       !- Component Name",
        "  ZonesHWBypassInletNode,  !- Component Inlet Node Name",
        "  ZonesHWBypassOutletNode;  !- Component Outlet Node Name",

        "Pipe:Adiabatic,",
        "  ZonesHWBypassPipe,       !- Name",
        "  ZonesHWBypassInletNode,  !- Inlet Node Name",
        "  ZonesHWBypassOutletNode;  !- Outlet Node Name",

        "PlantEquipmentOperationSchemes,",
        "  Hot Loop Operation,      !- Name",
        "  PlantEquipmentOperation:HeatingLoad,  !- Control Scheme 1 Object Type",
        "  Purchased Heating Only,  !- Control Scheme 1 Name",
        "  ON;                      !- Control Scheme 1 Schedule Name",

        "PlantEquipmentOperation:HeatingLoad,",
        "  Purchased Heating Only,  !- Name",
        "  0.0,                     !- Load Range 1 Lower Limit {W}",
        "  1000000.0,               !- Load Range 1 Upper Limit {W}",
        "  heating plant;           !- Range 1 Equipment List Name",

        "PlantEquipmentList,",
        "  heating plant,           !- Name",
        "  DistrictHeating:Water,   !- Equipment Object Type",
        "  Purchased Heating;       !- Equipment Name",

        "DistrictHeating:Water,",
        "  Purchased Heating,       !- Name",
        "  Purchased Heat Inlet Node,  !- Hot Water Inlet Node Name",
        "  Purchased Heat Outlet Node,  !- Hot Water Outlet Node Name",
        "  1000000.0;               !- Nominal Capacity {W}",

        "ZoneControl:Thermostat,",
        "  Zone 2 Thermostat,       !- Name",
        "  EAST ZONE,               !- Zone or ZoneList Name",
        "  Zone Control Type Sched,  !- Control Type Schedule Name",
        "  ,                        !- Temperature Difference Between Cutout And Setpoint {deltaC}",
        "  ThermostatSetpoint:SingleHeating,  !- Control 1 Object Type",
        "  Heating Setpoint with SB,  !- Control 1 Name",
        "  ThermostatSetpoint:SingleCooling,  !- Control 2 Object Type",
        "  Cooling Setpoint with SB;  !- Control 2 Name",

        "ThermostatSetpoint:SingleHeating,",
        "  Heating Setpoint with SB,  !- Name",
        "  Heating Setpoints;       !- Setpoint Temperature Schedule Name",

        "ThermostatSetpoint:SingleCooling,",
        "  Cooling Setpoint with SB,  !- Name",
        "  Cooling Setpoints;       !- Setpoint Temperature Schedule Name",

        "ZoneHVAC:EquipmentConnections,",
        "  EAST ZONE,               !- Zone Name",
        "  Zone2Equipment,          !- Zone Conditioning Equipment List Name",
        "  Zone2Inlets,             !- Zone Air Inlet Node or NodeList Name",
        "  Zone2Exhausts,           !- Zone Air Exhaust Node or NodeList Name",
        "  Zone 2 Node,             !- Zone Air Node Name",
        "  Zone 2 Outlet Node;      !- Zone Return Air Node or NodeList Name",

        "ZoneHVAC:EquipmentList,",
        "  Zone2Equipment,          !- Name",
        "  SequentialLoad,          !- Load Distribution Scheme",
        "  ZoneHVAC:UnitHeater,     !- Zone Equipment Object Type",
        "  Zone2UnitHeat,           !- Zone Equipment Name",
        "  1,                       !- Zone Equipment Cooling Sequence",
        "  1,                       !- Zone Equipment Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment Sequential Cooling Fraction Schedule Name",
        "  ;                        !- Zone Equipment Sequential Heating Fraction Schedule Name",

        "NodeList,",
        "  Zone2Inlets,             !- Name",
        "  Zone2UnitHeatAirOutletNode;  !- Node Name",

        "NodeList,",
        "  Zone2Exhausts,           !- Name",
        "  Zone2UnitHeatAirInletNode;  !- Node Name",

        "ZoneHVAC:UnitHeater,",
        "  Zone2UnitHeat,           !- Name",
        "  UnitHeatAvailability,    !- Availability Schedule Name",
        "  Zone2UnitHeatAirInletNode,  !- Air Inlet Node Name",
        "  Zone2UnitHeatAirOutletNode,  !- Air Outlet Node Name",
        "  Fan:ConstantVolume,      !- Supply Air Fan Object Type",
        "  Zone2UnitHeatFan,        !- Supply Air Fan Name",
        "  Autosize,                !- Maximum Supply Air Flow Rate {m3/s}",
        "  Coil:Heating:Water,      !- Heating Coil Object Type",
        "  Zone2UnitHeatHeatingCoil,  !- Heating Coil Name",
        "  ,                        !- Supply Air Fan Operating Mode Schedule Name",
        "  No,                      !- Supply Air Fan Operation During No Heating",
        "  Autosize,                !- Maximum Hot Water or Steam Flow Rate {m3/s}",
        "  0.0,                     !- Minimum Hot Water or Steam Flow Rate {m3/s}",
        "  0.001;                   !- Heating Convergence Tolerance",

        "Fan:ConstantVolume,",
        "  Zone2UnitHeatFan,        !- Name",
        "  UnitHeatAvailability,    !- Availability Schedule Name",
        "  0.5,                     !- Fan Total Efficiency",
        "  75.0,                    !- Pressure Rise {Pa}",
        "  Autosize,                !- Maximum Flow Rate {m3/s}",
        "  0.9,                     !- Motor Efficiency",
        "  1.0,                     !- Motor In Airstream Fraction",
        "  Zone2UnitHeatAirInletNode,  !- Air Inlet Node Name",
        "  Zone2UnitHeatFanOutletNode;  !- Air Outlet Node Name",

        "Coil:Heating:Water,",
        "  Zone2UnitHeatHeatingCoil,  !- Name",
        "  FanAndCoilAvailSched,    !- Availability Schedule Name",
        "  Autosize,                !- U-Factor Times Area Value {W/K}",
        "  Autosize,                !- Maximum Water Flow Rate {m3/s}",
        "  Zone2UnitHeatHWInletNode,  !- Water Inlet Node Name",
        "  Zone2UnitHeatHWOutletNode,  !- Water Outlet Node Name",
        "  Zone2UnitHeatFanOutletNode,  !- Air Inlet Node Name",
        "  Zone2UnitHeatAirOutletNode,  !- Air Outlet Node Name",
        "  UFactorTimesAreaAndDesignWaterFlowRate,  !- Performance Input Method",
        "  Autosize,                !- Rated Capacity {W}",
        "  82.2,                    !- Rated Inlet Water Temperature {C}",
        "  16.6,                    !- Rated Inlet Air Temperature {C}",
        "  71.1,                    !- Rated Outlet Water Temperature {C}",
        "  32.2,                    !- Rated Outlet Air Temperature {C}",
        "  ;                        !- Rated Ratio for Air and Water Convection",
    });
    ASSERT_TRUE(process_idf(idf_objects));
    state->dataGlobal->TimeStepsInHour = 4;    // must initialize this to get schedules initialized
    state->dataGlobal->MinutesInTimeStep = 15; // must initialize this to get schedules initialized
    state->init_state(*state);

    ErrorsFound = false;
    HeatBalanceManager::GetProjectControlData(*state, ErrorsFound); // read project control data
    EXPECT_FALSE(ErrorsFound);

    // OutputProcessor::TimeValue.allocate(2);
    state->dataGlobal->DDOnlySimulation = true;

    GetProjectData(*state);
    SetPreConstructionInputParameters(*state); // establish array bounds for constructions early

    state->dataGlobal->BeginSimFlag = true;
    state->dataGlobal->BeginEnvrnFlag = true;
    state->dataGlobal->ZoneSizingCalc = true;
    createFacilityElectricPowerServiceObject(*state);

    state->dataLoopNodes->MoreNodeInfo.allocate(20);
    state->dataHVACGlobal->TimeStepSys = state->dataGlobal->TimeStepZone;
    state->dataHVACGlobal->TimeStepSysSec = state->dataHVACGlobal->TimeStepSys * Constant::rSecsInHour;
    SetupTimePointers(*state, OutputProcessor::TimeStepType::Zone, state->dataGlobal->TimeStepZone);
    SetupTimePointers(*state, OutputProcessor::TimeStepType::System, state->dataHVACGlobal->TimeStepSys);

    UpdateMeterReporting(*state);

    SizingManager::ManageSizing(*state);

    EXPECT_FALSE(ErrorsFound);
    EXPECT_EQ(1, state->dataUnitHeaters->NumOfUnitHeats);
    EXPECT_EQ("ZONE2UNITHEAT", state->dataUnitHeaters->UnitHeat(1).Name);

    ErrorsFound = false;
    state->dataSize->ZoneEqUnitHeater = true;
    state->dataSize->CurZoneEqNum = 1;

    InitUnitHeater(*state, UnitHeatNum, ZoneNum, FirstHVACIteration);
    InitWaterCoil(*state, CoilNum, FirstHVACIteration); // init hot water heating coil

    PltSizHeatNum = PlantUtilities::MyPlantSizingIndex(*state,
                                                       "Coil:Heating:Water",
                                                       state->dataUnitHeaters->UnitHeat(UnitHeatNum).HCoilName,
                                                       state->dataWaterCoils->WaterCoil(CoilNum).WaterInletNodeNum,
                                                       state->dataWaterCoils->WaterCoil(CoilNum).WaterOutletNodeNum,
                                                       ErrorsFound);
    EXPECT_FALSE(ErrorsFound);

    HWMaxVolFlowRate = state->dataWaterCoils->WaterCoil(CoilNum).MaxWaterVolFlowRate;
    HWDensity = state->dataPlnt->PlantLoop(state->dataUnitHeaters->UnitHeat(UnitHeatNum).HWplantLoc.loopNum)
                    .glycol->getDensity(*state, Constant::HWInitConvTemp, "xxx");
    CpHW = state->dataPlnt->PlantLoop(state->dataUnitHeaters->UnitHeat(UnitHeatNum).HWplantLoc.loopNum)
               .glycol->getSpecificHeat(*state, Constant::HWInitConvTemp, "xxx");
    HWPlantDeltaTDesign = state->dataSize->PlantSizData(PltSizHeatNum).DeltaT;
    // calculate hot water coil design capacity
    HWCoilDesignCapacity = HWMaxVolFlowRate * HWDensity * CpHW * HWPlantDeltaTDesign;
    EXPECT_NEAR(HWCoilDesignCapacity, state->dataWaterCoils->WaterCoil(CoilNum).DesWaterHeatingCoilRate, 1.0);
    EXPECT_NEAR(189.06, state->dataWaterCoils->WaterCoil(CoilNum).UACoil, 0.02);
}

TEST_F(EnergyPlusFixture, UnitHeater_SimUnitHeaterTest)
{

    bool ErrorsFound(false);       // function returns true on error
    bool FirstHVACIteration(true); // TRUE if 1st HVAC simulation of system timestep

    int UnitHeatNum(1);       // unit heat index
    int ZoneNum(1);           // zone index
    int CoilNum(1);           // heating coil index
    int WCWaterInletNode(0);  // water coil water inlet node
    int WCWaterOutletNode(0); // water coil water outlet node
    int UHAirInletNode(0);    // unit heater air inlet node
    int UHAirOutletNode(0);   // unit heater air outlet node

    Real64 CpHW(0.0);                  // hot water specific heat, J/kg K
    Real64 SysOutputProvided(0.0);     // heating rate delivered
    Real64 LatOutputProvided(0.0);     // latent heating rate delivered
    Real64 ConvTol(1.0);               // convergence tolerance
    Real64 HWMassFlowRate(0.0);        // hot water coil water mass flow rate, m3/s
    Real64 UHAirMassFlowRate(0.0);     // unit heate air mass flow rate
    Real64 UHHeatingRate(0.0);         // unit heate heating rate
    Real64 UHEnteringAirEnthalpy(0.0); // unit heater entering air enthalpy
    Real64 UHLeavingAirEnthalpy(0.0);  // unit heater leaving air enthalpy
    Real64 HWCoilHeatingRate(0.0);     // hot water heating coil heating rate

    std::string const idf_objects = delimited_string({
        "ScheduleTypeLimits,",
        "  Any Number;              !- Name",

        "Schedule:Compact,",
        "  UNITHEATAVAILABILITY,    !- Name",
        "  ANY NUMBER,              !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: Alldays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  1.00;                    !- Field",

        "Zone,",
        "  EAST ZONE,               !- Name",
        "  0.0,                     !- Direction of Relative North {deg}",
        "  0.0,                     !- X Origin {m}",
        "  0.0,                     !- Y Origin {m}",
        "  0.0,                     !- Z Origin {m}",
        "  1,                       !- Type",
        "  1,                       !- Multiplier",
        "  Autocalculate,           !- Ceiling Height {m}",
        "  Autocalculate;           !- Volume {m3}",

        "ZoneHVAC:EquipmentConnections,",
        "  EAST ZONE,               !- Zone Name",
        "  Zone2Equipment,          !- Zone Conditioning Equipment List Name",
        "  Zone2Inlets,             !- Zone Air Inlet Node or NodeList Name",
        "  Zone2Exhausts,           !- Zone Air Exhaust Node or NodeList Name",
        "  Zone 2 Node,             !- Zone Air Node Name",
        "  Zone 2 Outlet Node;      !- Zone Return Air Node or NodeList Name",

        "ZoneHVAC:EquipmentList,",
        "  Zone2Equipment,          !- Name",
        "  SequentialLoad,          !- Load Distribution Scheme",
        "  ZoneHVAC:UnitHeater,     !- Zone Equipment Object Type",
        "  Zone2UnitHeat,           !- Zone Equipment Name",
        "  1,                       !- Zone Equipment Cooling Sequence",
        "  1,                       !- Zone Equipment Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment Sequential Cooling Fraction Schedule Name",
        "  ;                        !- Zone Equipment Sequential Heating Fraction Schedule Name",

        "NodeList,",
        "  Zone2Inlets,             !- Name",
        "  Zone2UnitHeatAirOutletNode;  !- Node Name",

        "NodeList,",
        "  Zone2Exhausts,           !- Name",
        "  Zone2UnitHeatAirInletNode;  !- Node Name",

        "ZoneHVAC:UnitHeater,",
        "  Zone2UnitHeat,           !- Name",
        "  UnitHeatAvailability,    !- Availability Schedule Name",
        "  Zone2UnitHeatAirInletNode,  !- Air Inlet Node Name",
        "  Zone2UnitHeatAirOutletNode,  !- Air Outlet Node Name",
        "  Fan:ConstantVolume,      !- Supply Air Fan Object Type",
        "  Zone2UnitHeatFan,        !- Supply Air Fan Name",
        "  0.47055,                 !- Maximum Supply Air Flow Rate {m3/s}",
        "  Coil:Heating:Water,      !- Heating Coil Object Type",
        "  Zone2UnitHeatHeatingCoil,  !- Heating Coil Name",
        "  ,                        !- Supply Air Fan Operating Mode Schedule Name",
        "  No,                      !- Supply Air Fan Operation During No Heating",
        "  7.25956e-05,             !- Maximum Hot Water or Steam Flow Rate {m3/s}",
        "  0.0,                     !- Minimum Hot Water or Steam Flow Rate {m3/s}",
        "  0.001;                   !- Heating Convergence Tolerance",

        "Fan:ConstantVolume,",
        "  Zone2UnitHeatFan,        !- Name",
        "  UnitHeatAvailability,    !- Availability Schedule Name",
        "  0.5,                     !- Fan Total Efficiency",
        "  75.0,                    !- Pressure Rise {Pa}",
        "  0.47055,                 !- Maximum Flow Rate {m3/s}",
        "  0.9,                     !- Motor Efficiency",
        "  1.0,                     !- Motor In Airstream Fraction",
        "  Zone2UnitHeatAirInletNode,  !- Air Inlet Node Name",
        "  Zone2UnitHeatFanOutletNode;  !- Air Outlet Node Name",

        "Coil:Heating:Water,",
        "  Zone2UnitHeatHeatingCoil,  !- Name",
        "  UnitHeatAvailability,    !- Availability Schedule Name",
        "  84.93609,                !- U-Factor Times Area Value {W/K}",
        "  7.25956e-05,             !- Maximum Water Flow Rate {m3/s}",
        "  Zone2UnitHeatHWInletNode,  !- Water Inlet Node Name",
        "  Zone2UnitHeatHWOutletNode,  !- Water Outlet Node Name",
        "  Zone2UnitHeatFanOutletNode,  !- Air Inlet Node Name",
        "  Zone2UnitHeatAirOutletNode,  !- Air Outlet Node Name",
        "  UFactorTimesAreaAndDesignWaterFlowRate,  !- Performance Input Method",
        "  6750.70521,              !- Rated Capacity {W}",
        "  82.2,                    !- Rated Inlet Water Temperature {C}",
        "  16.6,                    !- Rated Inlet Air Temperature {C}",
        "  71.1,                    !- Rated Outlet Water Temperature {C}",
        "  32.2,                    !- Rated Outlet Air Temperature {C}",
        "  ;                        !- Rated Ratio for Air and Water Convection",
    });
    ASSERT_TRUE(process_idf(idf_objects));

    state->dataGlobal->TimeStepsInHour = 4;    // must initialize this to get schedules initialized
    state->dataGlobal->MinutesInTimeStep = 15; // must initialize this to get schedules initialized
    state->init_state(*state);

    ErrorsFound = false;
    HeatBalanceManager::GetZoneData(*state, ErrorsFound); // read zone data
    EXPECT_FALSE(ErrorsFound);

    ErrorsFound = false;
    GetZoneEquipmentData(*state);
    EXPECT_FALSE(ErrorsFound);

    ErrorsFound = false;
    GetWaterCoilInput(*state);
    EXPECT_FALSE(ErrorsFound);
    state->dataWaterCoils->GetWaterCoilsInputFlag = false;

    ErrorsFound = false;
    GetFanInput(*state);
    EXPECT_FALSE(ErrorsFound);

    ErrorsFound = false;
    GetUnitHeaterInput(*state); // get unit heaters data
    EXPECT_FALSE(ErrorsFound);
    EXPECT_EQ(1, state->dataUnitHeaters->NumOfUnitHeats);
    EXPECT_EQ("ZONE2UNITHEAT", state->dataUnitHeaters->UnitHeat(1).Name);

    ErrorsFound = false;
    state->dataSize->ZoneEqUnitHeater = true;
    state->dataSize->CurZoneEqNum = 1;

    state->dataPlnt->TotNumLoops = 1;
    state->dataPlnt->PlantLoop.allocate(state->dataPlnt->TotNumLoops);

    for (int l = 1; l <= state->dataPlnt->TotNumLoops; ++l) {
        auto &loopside(state->dataPlnt->PlantLoop(l).LoopSide(DataPlant::LoopSideLocation::Demand));
        loopside.TotalBranches = 1;
        loopside.Branch.allocate(1);
        auto &loopsidebranch(state->dataPlnt->PlantLoop(l).LoopSide(DataPlant::LoopSideLocation::Demand).Branch(1));
        loopsidebranch.TotalComponents = 1;
        loopsidebranch.Comp.allocate(1);
    }

    state->dataPlnt->PlantLoop(1).Name = "HotWaterLoop";
    state->dataPlnt->PlantLoop(1).FluidName = "WATER";
    state->dataPlnt->PlantLoop(1).glycol = Fluid::GetWater(*state);
    state->dataPlnt->PlantLoop(1).LoopSide(DataPlant::LoopSideLocation::Demand).Branch(1).Comp(1).Name = state->dataWaterCoils->WaterCoil(1).Name;
    state->dataPlnt->PlantLoop(1).LoopSide(DataPlant::LoopSideLocation::Demand).Branch(1).Comp(1).Type =
        DataPlant::PlantEquipmentType::CoilWaterSimpleHeating;
    state->dataPlnt->PlantLoop(1).LoopSide(DataPlant::LoopSideLocation::Demand).Branch(1).Comp(1).NodeNumIn =
        state->dataWaterCoils->WaterCoil(1).WaterInletNodeNum;
    state->dataPlnt->PlantLoop(1).LoopSide(DataPlant::LoopSideLocation::Demand).Branch(1).Comp(1).NodeNumOut =
        state->dataWaterCoils->WaterCoil(1).WaterOutletNodeNum;

    state->dataZoneEnergyDemand->ZoneSysEnergyDemand.allocate(1);
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputReqToHeatSP = 2000.0;

    state->dataSize->ZoneSizingRunDone = true;
    state->dataSize->ZoneEqSizing.allocate(1);
    state->dataSize->ZoneEqSizing(state->dataSize->CurZoneEqNum).DesignSizeFromParent = false;
    state->dataGlobal->DoingSizing = true;

    state->dataHVACGlobal->TurnFansOn = true;
    state->dataHVACGlobal->TurnFansOff = false;

    Sched::GetSchedule(*state, "UNITHEATAVAILABILITY")->currentVal = 1;
    state->dataZoneEnergyDemand->CurDeadBandOrSetback.allocate(1);
    state->dataZoneEnergyDemand->CurDeadBandOrSetback(ZoneNum) = false;

    state->dataLoopNodes->Node(state->dataFans->fans(1)->inletNodeNum).Temp = 16.6;
    state->dataLoopNodes->Node(state->dataFans->fans(1)->inletNodeNum).HumRat = PsyWFnTdbRhPb(*state, 16.6, 0.5, 101325.0, "UnitTest");
    state->dataLoopNodes->Node(state->dataFans->fans(1)->inletNodeNum).Enthalpy =
        Psychrometrics::PsyHFnTdbW(state->dataLoopNodes->Node(state->dataFans->fans(1)->inletNodeNum).Temp,
                                   state->dataLoopNodes->Node(state->dataFans->fans(1)->inletNodeNum).HumRat);
    state->dataEnvrn->StdRhoAir = PsyRhoAirFnPbTdbW(*state, 101325.0, 20.0, 0.0);

    WCWaterInletNode = state->dataWaterCoils->WaterCoil(CoilNum).WaterInletNodeNum;
    WCWaterOutletNode = state->dataWaterCoils->WaterCoil(CoilNum).WaterOutletNodeNum;
    state->dataLoopNodes->Node(WCWaterInletNode).Temp = 60.0;

    state->dataGlobal->BeginEnvrnFlag = true;
    state->dataGlobal->SysSizingCalc = true;

    UHAirInletNode = state->dataUnitHeaters->UnitHeat(UnitHeatNum).AirInNode;
    UHAirOutletNode = state->dataUnitHeaters->UnitHeat(UnitHeatNum).AirOutNode;

    SimUnitHeater(*state,
                  state->dataUnitHeaters->UnitHeat(UnitHeatNum).Name,
                  ZoneNum,
                  FirstHVACIteration,
                  SysOutputProvided,
                  LatOutputProvided,
                  state->dataSize->CurZoneEqNum);
    // SimUnitHeater does not converge on the first call: the unit heater delivers more than required heating load. But it meets
    // on the second call (iteration). I suspect it may be an initialization issue related to ControlCompOutput routine
    SimUnitHeater(*state,
                  state->dataUnitHeaters->UnitHeat(UnitHeatNum).Name,
                  ZoneNum,
                  FirstHVACIteration,
                  SysOutputProvided,
                  LatOutputProvided,
                  state->dataSize->CurZoneEqNum);
    // verify the total heat rate deleivered by the unit heater
    UHAirMassFlowRate = state->dataLoopNodes->Node(UHAirInletNode).MassFlowRate;
    UHEnteringAirEnthalpy = PsyHFnTdbW(state->dataLoopNodes->Node(UHAirInletNode).Temp, state->dataLoopNodes->Node(UHAirInletNode).HumRat);
    UHLeavingAirEnthalpy = PsyHFnTdbW(state->dataLoopNodes->Node(UHAirOutletNode).Temp, state->dataLoopNodes->Node(UHAirOutletNode).HumRat);
    UHHeatingRate = UHAirMassFlowRate * (UHLeavingAirEnthalpy - UHEnteringAirEnthalpy);
    EXPECT_NEAR(UHHeatingRate, state->dataUnitHeaters->UnitHeat(UnitHeatNum).HeatPower, ConvTol);
    // verify the heat rate delivered by the hot water heating coil
    HWMassFlowRate = state->dataWaterCoils->WaterCoil(CoilNum).InletWaterMassFlowRate;
    CpHW = state->dataPlnt->PlantLoop(state->dataUnitHeaters->UnitHeat(UnitHeatNum).HWplantLoc.loopNum)
               .glycol->getSpecificHeat(*state, 60.0, "UnitTest");
    HWCoilHeatingRate =
        HWMassFlowRate * CpHW * (state->dataLoopNodes->Node(WCWaterInletNode).Temp - state->dataLoopNodes->Node(WCWaterOutletNode).Temp);
    EXPECT_NEAR(HWCoilHeatingRate, state->dataWaterCoils->WaterCoil(CoilNum).TotWaterHeatingCoilRate, ConvTol);
}

TEST_F(EnergyPlusFixture, UnitHeater_SecondPriorityZoneEquipment)
{

    std::string const idf_objects = delimited_string({
        "Timestep,1;",

        "Building,",
        "  Bldg,                    !- Name",
        "  0.0,                     !- North Axis {deg}",
        "  Suburbs,                 !- Terrain",
        "  0.05,                    !- Loads Convergence Tolerance Value {W}",
        "  0.05,                    !- Temperature Convergence Tolerance Value {deltaC}",
        "  MinimalShadowing,        !- Solar Distribution",
        "  30,                      !- Maximum Number of Warmup Days",
        "  6;                       !- Minimum Number of Warmup Days",

        "SurfaceConvectionAlgorithm:Inside,Simple;",

        "SurfaceConvectionAlgorithm:Outside,SimpleCombined;",

        "HeatBalanceAlgorithm,ConductionTransferFunction;",

        "SimulationControl,",
        "  No,                      !- Do Zone Sizing Calculation",
        "  No,                      !- Do System Sizing Calculation",
        "  No,                      !- Do Plant Sizing Calculation",
        "  Yes,                     !- Run Simulation for Sizing Periods",
        "  No,                      !- Run Simulation for Weather File Run Periods",
        "  ,                        !- Do HVAC Sizing Simulation for Sizing Periods",
        "  ;                        !- Maximum Number of HVAC Sizing Simulation Passes",

        "Site:Location,",
        "  CHICAGO_IL_USA TMY2-94846,  !- Name",
        "  41.78,                   !- Latitude {deg}",
        "  -87.75,                  !- Longitude {deg}",
        "  -6.0,                    !- Time Zone {hr}",
        "  190.0;                   !- Elevation {m}",

        "SizingPeriod:DesignDay,",
        "  CHICAGO_IL_USA Annual Heating 99% Design Conditions DB,  !- Name",
        "  1,                       !- Month",
        "  21,                      !- Day of Month",
        "  WinterDesignDay,         !- Day Type",
        "  -12.3,                   !- Maximum Dry-Bulb Temperature {C}",
        "  0.0,                     !- Daily Dry-Bulb Temperature Range {deltaC}",
        "  ,                        !- Dry-Bulb Temperature Range Modifier Type",
        "  ,                        !- Dry-Bulb Temperature Range Modifier Day Schedule Name",
        "  WetBulb,                 !- Humidity Condition Type",
        "  -17.3,                   !- Wetbulb or DewPoint at Maximum Dry-Bulb {C}",
        "  ,                        !- Humidity Condition Day Schedule Name",
        "  ,                        !- Humidity Ratio at Maximum Dry-Bulb {kgWater/kgDryAir}",
        "  ,                        !- Enthalpy at Maximum Dry-Bulb {J/kg}",
        "  ,                        !- Daily Wet-Bulb Temperature Range {deltaC}",
        "  99063.0,                 !- Barometric Pressure {Pa}",
        "  4.9,                     !- Wind Speed {m/s}",
        "  270.0,                   !- Wind Direction {deg}",
        "  No,                      !- Rain Indicator",
        "  No,                      !- Snow Indicator",
        "  No,                      !- Daylight Saving Time Indicator",
        "  ASHRAEClearSky,          !- Solar Model Indicator",
        "  ,                        !- Beam Solar Day Schedule Name",
        "  ,                        !- Diffuse Solar Day Schedule Name",
        "  ,                        !- ASHRAE Clear Sky Optical Depth for Beam Irradiance (taub) {dimensionless}",
        "  ,                        !- ASHRAE Clear Sky Optical Depth for Diffuse Irradiance (taud) {dimensionless}",
        "  0.0;                     !- Sky Clearness",

        "Site:GroundTemperature:BuildingSurface,21.5,21.4,21.5,21.5,22.0,22.9,23.0,23.1,23.1,22.2,21.7,21.6;",

        "Material:NoMass,",
        "  R13LAYER,                !- Name",
        "  Rough,                   !- Roughness",
        "  2.290965,                !- Thermal Resistance {m2-K/W}",
        "  0.9,                     !- Thermal Absorptance",
        "  0.75,                    !- Solar Absorptance",
        "  0.75;                    !- Visible Absorptance",

        "Material:NoMass,",
        "  R31LAYER,                !- Name",
        "  Rough,                   !- Roughness",
        "  5.456,                   !- Thermal Resistance {m2-K/W}",
        "  0.9,                     !- Thermal Absorptance",
        "  0.75,                    !- Solar Absorptance",
        "  0.75;                    !- Visible Absorptance",

        "Material,",
        "  C5 - 4 IN HW CONCRETE,   !- Name",
        "  MediumRough,             !- Roughness",
        "  0.1014984,               !- Thickness {m}",
        "  1.729577,                !- Conductivity {W/m-K}",
        "  2242.585,                !- Density {kg/m3}",
        "  836.8,                   !- Specific Heat {J/kg-K}",
        "  0.9,                     !- Thermal Absorptance",
        "  0.65,                    !- Solar Absorptance",
        "  0.65;                    !- Visible Absorptance",

        "Construction,",
        "  R13WALL,                 !- Name",
        "  R13LAYER;                !- Outside Layer",

        "Construction,",
        "  FLOOR,                   !- Name",
        "  C5 - 4 IN HW CONCRETE;   !- Outside Layer",

        "Construction,",
        "  ROOF31,                  !- Name",
        "  R31LAYER;                !- Outside Layer",

        "Zone,",
        "  Main Zone,               !- Name",
        "  0.0,                     !- Direction of Relative North {deg}",
        "  0.0,                     !- X Origin {m}",
        "  0.0,                     !- Y Origin {m}",
        "  0.0,                     !- Z Origin {m}",
        "  1,                       !- Type",
        "  1,                       !- Multiplier",
        "  Autocalculate,           !- Ceiling Height {m}",
        "  Autocalculate;           !- Volume {m3}",

        "GlobalGeometryRules,",
        "  UpperLeftCorner,         !- Starting Vertex Position",
        "  Counterclockwise,        !- Vertex Entry Direction",
        "  Relative;                !- Coordinate System",

        "BuildingSurface:Detailed,",
        "  Zn001:Wall001,           !- Name",
        "  Wall,                    !- Surface Type",
        "  R13WALL,                 !- Construction Name",
        "  Main Zone,               !- Zone Name",
        "  ,                        !- Space Name",
        "  Outdoors,                !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  SunExposed,              !- Sun Exposure",
        "  WindExposed,             !- Wind Exposure",
        "  0.5,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  4.572,                   !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  15.24,                   !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  15.24,                   !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  4.572;                   !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Zn001:Wall002,           !- Name",
        "  Wall,                    !- Surface Type",
        "  R13WALL,                 !- Construction Name",
        "  Main Zone,               !- Zone Name",
        "  ,                        !- Space Name",
        "  Outdoors,                !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  SunExposed,              !- Sun Exposure",
        "  WindExposed,             !- Wind Exposure",
        "  0.5,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  15.24,                   !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  4.572,                   !- Vertex Z-coordinate {m}",
        "  15.24,                   !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  15.24,                   !- Vertex X-coordinate {m}",
        "  15.24,                   !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  15.24,                   !- Vertex X-coordinate {m}",
        "  15.24,                   !- Vertex Y-coordinate {m}",
        "  4.572;                   !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Zn001:Wall003,           !- Name",
        "  Wall,                    !- Surface Type",
        "  R13WALL,                 !- Construction Name",
        "  Main Zone,               !- Zone Name",
        "  ,                        !- Space Name",
        "  Outdoors,                !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  SunExposed,              !- Sun Exposure",
        "  WindExposed,             !- Wind Exposure",
        "  0.5,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  15.24,                   !- Vertex X-coordinate {m}",
        "  15.24,                   !- Vertex Y-coordinate {m}",
        "  4.572,                   !- Vertex Z-coordinate {m}",
        "  15.24,                   !- Vertex X-coordinate {m}",
        "  15.24,                   !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  15.24,                   !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  15.24,                   !- Vertex Y-coordinate {m}",
        "  4.572;                   !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Zn001:Wall004,           !- Name",
        "  Wall,                    !- Surface Type",
        "  R13WALL,                 !- Construction Name",
        "  Main Zone,               !- Zone Name",
        "  ,                        !- Space Name",
        "  Outdoors,                !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  SunExposed,              !- Sun Exposure",
        "  WindExposed,             !- Wind Exposure",
        "  0.5,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  15.24,                   !- Vertex Y-coordinate {m}",
        "  4.572,                   !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  15.24,                   !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  4.572;                   !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Zn001:Flr001,            !- Name",
        "  Floor,                   !- Surface Type",
        "  FLOOR,                   !- Construction Name",
        "  Main Zone,               !- Zone Name",
        "  ,                        !- Space Name",
        "  Surface,                 !- Outside Boundary Condition",
        "  Zn001:Flr001,            !- Outside Boundary Condition Object",
        "  NoSun,                   !- Sun Exposure",
        "  NoWind,                  !- Wind Exposure",
        "  1.0,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  15.24,                   !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  15.24,                   !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  15.24,                   !- Vertex X-coordinate {m}",
        "  15.24,                   !- Vertex Y-coordinate {m}",
        "  0.0;                     !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Zn001:Roof001,           !- Name",
        "  Roof,                    !- Surface Type",
        "  ROOF31,                  !- Construction Name",
        "  Main Zone,               !- Zone Name",
        "  ,                        !- Space Name",
        "  Outdoors,                !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  SunExposed,              !- Sun Exposure",
        "  WindExposed,             !- Wind Exposure",
        "  0.0,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  15.24,                   !- Vertex Y-coordinate {m}",
        "  4.572,                   !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  4.572,                   !- Vertex Z-coordinate {m}",
        "  15.24,                   !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  4.572,                   !- Vertex Z-coordinate {m}",
        "  15.24,                   !- Vertex X-coordinate {m}",
        "  15.24,                   !- Vertex Y-coordinate {m}",
        "  4.572;                   !- Vertex Z-coordinate {m}",

        "ScheduleTypeLimits,",
        "  Any Number;              !- Name",

        "ScheduleTypeLimits,",
        "  Fraction,                !- Name",
        "  0.0,                     !- Lower Limit Value {BasedOnField A3}",
        "  1.0,                     !- Upper Limit Value {BasedOnField A3}",
        "  Continuous;              !- Numeric Type",

        "ScheduleTypeLimits,",
        "  Temperature,             !- Name",
        "  -60.0,                   !- Lower Limit Value {BasedOnField A3}",
        "  200.0,                   !- Upper Limit Value {BasedOnField A3}",
        "  Continuous,              !- Numeric Type",
        "  Temperature;             !- Unit Type",

        "ScheduleTypeLimits,",
        "  Control Type,            !- Name",
        "  0.0,                     !- Lower Limit Value {BasedOnField A3}",
        "  4.0,                     !- Upper Limit Value {BasedOnField A3}",
        "  Discrete;                !- Numeric Type",

        "ScheduleTypeLimits,",
        "  On/Off,                  !- Name",
        "  0.0,                     !- Lower Limit Value {BasedOnField A3}",
        "  1.0,                     !- Upper Limit Value {BasedOnField A3}",
        "  Discrete;                !- Numeric Type",

        "ScheduleTypeLimits,",
        "  FlowRate,                !- Name",
        "  0.0,                     !- Lower Limit Value {BasedOnField A3}",
        "  10.0,                    !- Upper Limit Value {BasedOnField A3}",
        "  Continuous;              !- Numeric Type",

        "ScheduleTypeLimits,",
        "  Humidity,                !- Name",
        "  0.0,                     !- Lower Limit Value {BasedOnField A3}",
        "  100.0,                   !- Upper Limit Value {BasedOnField A3}",
        "  Continuous;              !- Numeric Type",

        "Schedule:Compact,",
        "  OCCUPY-1,                !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: WeekDays SummerDesignDay CustomDay1 CustomDay2,  !- Field",
        "  Until: 8:00,             !- Field",
        "  0.0,                     !- Field",
        "  Until: 11:00,            !- Field",
        "  1.00,                    !- Field",
        "  Until: 12:00,            !- Field",
        "  0.80,                    !- Field",
        "  Until: 13:00,            !- Field",
        "  0.40,                    !- Field",
        "  Until: 14:00,            !- Field",
        "  0.80,                    !- Field",
        "  Until: 18:00,            !- Field",
        "  1.00,                    !- Field",
        "  Until: 19:00,            !- Field",
        "  0.50,                    !- Field",
        "  Until: 21:00,            !- Field",
        "  0.10,                    !- Field",
        "  Until: 24:00,            !- Field",
        "  0.0,                     !- Field",
        "  For: Weekends WinterDesignDay Holiday,  !- Field",
        "  Until: 24:00,            !- Field",
        "  0.0;                     !- Field",

        "Schedule:Compact,",
        "  LIGHTS-1,                !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: WeekDays SummerDesignDay CustomDay1 CustomDay2,  !- Field",
        "  Until: 8:00,             !- Field",
        "  0.05,                    !- Field",
        "  Until: 9:00,             !- Field",
        "  0.9,                     !- Field",
        "  Until: 10:00,            !- Field",
        "  0.95,                    !- Field",
        "  Until: 11:00,            !- Field",
        "  1.00,                    !- Field",
        "  Until: 12:00,            !- Field",
        "  0.95,                    !- Field",
        "  Until: 13:00,            !- Field",
        "  0.8,                     !- Field",
        "  Until: 14:00,            !- Field",
        "  0.9,                     !- Field",
        "  Until: 18:00,            !- Field",
        "  1.00,                    !- Field",
        "  Until: 19:00,            !- Field",
        "  0.60,                    !- Field",
        "  Until: 21:00,            !- Field",
        "  0.20,                    !- Field",
        "  Until: 24:00,            !- Field",
        "  0.05,                    !- Field",
        "  For: Weekends WinterDesignDay Holiday,  !- Field",
        "  Until: 24:00,            !- Field",
        "  0.05;                    !- Field",

        "Schedule:Compact,",
        "  EQUIP-1,                 !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: WeekDays SummerDesignDay CustomDay1 CustomDay2,  !- Field",
        "  Until: 8:00,             !- Field",
        "  0.02,                    !- Field",
        "  Until: 9:00,             !- Field",
        "  0.4,                     !- Field",
        "  Until: 14:00,            !- Field",
        "  0.9,                     !- Field",
        "  Until: 15:00,            !- Field",
        "  0.8,                     !- Field",
        "  Until: 16:00,            !- Field",
        "  0.7,                     !- Field",
        "  Until: 18:00,            !- Field",
        "  0.5,                     !- Field",
        "  Until: 20:00,            !- Field",
        "  0.3,                     !- Field",
        "  Until: 24:00,            !- Field",
        "  0.02,                    !- Field",
        "  For: Weekends WinterDesignDay Holiday,  !- Field",
        "  Until: 24:00,            !- Field",
        "  0.2;                     !- Field",

        "Schedule:Compact,",
        "  INFIL-SCH,               !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 3/31,           !- Field",
        "  For: WeekDays SummerDesignDay CustomDay1 CustomDay2,  !- Field",
        "  Until: 24:00,            !- Field",
        "  1.0,                     !- Field",
        "  For: AllOtherDays,       !- Field",
        "  Until: 24:00,            !- Field",
        "  1.0,                     !- Field",
        "  Through: 9/30,           !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  0.7,                     !- Field",
        "  Through: 12/31,          !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  1.0,                     !- Field",
        "  For: AllOtherDays,       !- Field",
        "  Until: 24:00,            !- Field",
        "  0.2;                     !- Field",

        "Schedule:Compact,",
        "  ActSchd,                 !- Name",
        "  Any Number,              !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  117.239997864;           !- Field",

        "Schedule:Compact,",
        "  Htg-SetP-Sch,            !- Name",
        "  Temperature,             !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  21.1;                    !- Field",

        "Schedule:Compact,",
        "  Clg-SetP-Sch,            !- Name",
        "  Temperature,             !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  23.9;                    !- Field",

        "Schedule:Compact,",
        "  Min OA Sched,            !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  0.5;                     !- Field",

        "Schedule:Compact,",
        "  FanAvailSched,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 3/31,           !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  1.0,                     !- Field",
        "  Through: 9/30,           !- Field",
        "  For: WeekDays,           !- Field",
        "  Until: 7:00,             !- Field",
        "  0.0,                     !- Field",
        "  Until: 17:00,            !- Field",
        "  1.0,                     !- Field",
        "  Until: 24:00,            !- Field",
        "  0.0,                     !- Field",
        "  For: SummerDesignDay WinterDesignDay,  !- Field",
        "  Until: 24:00,            !- Field",
        "  1.0,                     !- Field",
        "  For: AllOtherDays,       !- Field",
        "  Until: 24:00,            !- Field",
        "  0.0,                     !- Field",
        "  Through: 12/31,          !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  1.0;                     !- Field",

        "Schedule:Compact,",
        "  AlwaysOn,                !- Name",
        "  Any Number,              !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  1.0;                     !- Field",

        "People,",
        "  Main Zone People 1,      !- Name",
        "  Main Zone,               !- Zone or ZoneList or Space or SpaceList Name",
        "  OCCUPY-1,                !- Number of People Schedule Name",
        "  People,                  !- Number of People Calculation Method",
        "  20.0,                    !- Number of People",
        "  ,                        !- People per Floor Area {person/m2}",
        "  ,                        !- Floor Area per Person {m2/person}",
        "  0.3,                     !- Fraction Radiant",
        "  Autocalculate,           !- Sensible Heat Fraction",
        "  ActSchd;                 !- Activity Level Schedule Name",

        "Lights,",
        "  Main Zone Lights 1,      !- Name",
        "  Main Zone,               !- Zone or ZoneList or Space or SpaceList Name",
        "  LIGHTS-1,                !- Schedule Name",
        "  LightingLevel,           !- Design Level Calculation Method",
        "  2964.0,                  !- Lighting Level {W}",
        "  ,                        !- Watts per Floor Area {W/m2}",
        "  ,                        !- Watts per Person {W/person}",
        "  0.2,                     !- Return Air Fraction",
        "  0.59,                    !- Fraction Radiant",
        "  0.2,                     !- Fraction Visible",
        "  0.0,                     !- Fraction Replaceable",
        "  GeneralLights;           !- End-Use Subcategory",

        "ElectricEquipment,",
        "  Main Zone ElecEq 1,      !- Name",
        "  Main Zone,               !- Zone or ZoneList or Space or SpaceList Name",
        "  EQUIP-1,                 !- Schedule Name",
        "  EquipmentLevel,          !- Design Level Calculation Method",
        "  19760.0,                 !- Design Level {W}",
        "  ,                        !- Watts per Floor Area {W/m2}",
        "  ,                        !- Watts per Person {W/person}",
        "  0.5,                     !- Fraction Latent",
        "  0.3,                     !- Fraction Radiant",
        "  0.0;                     !- Fraction Lost",

        "ZoneInfiltration:DesignFlowRate,",
        "  Main Zone Infil 1,       !- Name",
        "  Main Zone,               !- Zone or ZoneList or Space or SpaceList Name",
        "  INFIL-SCH,               !- Schedule Name",
        "  Flow/Zone,               !- Design Flow Rate Calculation Method",
        "  0.031089,                !- Design Flow Rate {m3/s}",
        "  ,                        !- Flow Rate per Floor Area {m3/s-m2}",
        "  ,                        !- Flow Rate per Exterior Surface Area {m3/s-m2}",
        "  ,                        !- Air Changes per Hour {1/hr}",
        "  0.0,                     !- Constant Term Coefficient",
        "  0.0,                     !- Temperature Term Coefficient",
        "  0.2237,                  !- Velocity Term Coefficient",
        "  0.0;                     !- Velocity Squared Term Coefficient",

        "Sizing:Parameters,",
        "  1.2,                     !- Heating Sizing Factor",
        "  1.2;                     !- Cooling Sizing Factor",

        "ThermostatSetpoint:DualSetpoint,",
        "  All Zones Dual SP Control,  !- Name",
        "  Htg-SetP-Sch,            !- Heating Setpoint Temperature Schedule Name",
        "  Clg-SetP-Sch;            !- Cooling Setpoint Temperature Schedule Name",

        "Schedule:Compact,",
        "  ZONE CONTROL TYPE SCHED,  !- Name",
        "  Any Number,              !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  4;                       !- Field",

        "ZoneControl:Thermostat,",
        "  Main Zone Thermostat,    !- Name",
        "  Main Zone,               !- Zone or ZoneList Name",
        "  ZONE CONTROL TYPE SCHED,  !- Control Type Schedule Name",
        "  ,                        !- Temperature Difference Between Cutout And Setpoint {deltaC}",
        "  ThermostatSetpoint:DualSetpoint,  !- Control 1 Object Type",
        "  All Zones Dual SP Control;  !- Control 1 Name",

        "ZoneHVAC:EquipmentConnections,",
        "  Main Zone,               !- Zone Name",
        "  Main Zone Equipment,     !- Zone Conditioning Equipment List Name",
        "  Main Zone Inlet Nodes,   !- Zone Air Inlet Node or NodeList Name",
        "  Main Zone Exh Node List,  !- Zone Air Exhaust Node or NodeList Name",
        "  Main Zone Zone Air Node,  !- Zone Air Node Name",
        "  Main Zone Return Outlet;  !- Zone Return Air Node or NodeList Name",

        "NodeList,",
        "  Main Zone Inlet Nodes,   !- Name",
        "  Main Zone Supply Inlet,  !- Node Name",
        "  Node 33;                 !- Node Name",

        "NodeList,",
        "  Main Zone Exh Node List,  !- Name",
        "  Node 32;                 !- Node Name",

        "ZoneHVAC:EquipmentList,",
        "  Main Zone Equipment,     !- Name",
        "  SequentialLoad,          !- Load Distribution Scheme",
        "  ZoneHVAC:AirDistributionUnit,  !- Zone Equipment Object Type",
        "  Main Zone ATU,           !- Zone Equipment Name",
        "  1,                       !- Zone Equipment Cooling Sequence",
        "  1,                       !- Zone Equipment Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment Sequential Cooling Fraction Schedule Name",
        "  ,                        !- Zone Equipment Sequential Heating Fraction Schedule Name",
        "  ZoneHVAC:UnitHeater,     !- Zone Equipment Object Type",
        "  UnitHeater,              !- Zone Equipment Name",
        "  2,                       !- Zone Equipment Cooling Sequence",
        "  2,                       !- Zone Equipment Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment Sequential Cooling Fraction Schedule Name",
        "  ;                        !- Zone Equipment Sequential Heating Fraction Schedule Name",

        "ZoneHVAC:AirDistributionUnit,",
        "  Main Zone ATU,           !- Name",
        "  Main Zone Supply Inlet,  !- Air Distribution Unit Outlet Node Name",
        "  AirTerminal:SingleDuct:VAV:Reheat,  !- Air Terminal Object Type",
        "  Main Zone VAV Reheat;    !- Air Terminal Name",

        "AirTerminal:SingleDuct:VAV:Reheat,",
        "  Main Zone VAV Reheat,    !- Name",
        "  ,                        !- Availability Schedule Name",
        "  Main Zone Damper Outlet,  !- Damper Air Outlet Node Name",
        "  Main Zone Zone Equip Inlet,  !- Air Inlet Node Name",
        "  1.13941,                 !- Maximum Air Flow Rate {m3/s}",
        "  Constant,                !- Zone Minimum Air Flow Input Method",
        "  0.3,                     !- Constant Minimum Air Flow Fraction",
        "  ,                        !- Fixed Minimum Air Flow Rate {m3/s}",
        "  ,                        !- Minimum Air Flow Fraction Schedule Name",
        "  Coil:Heating:Electric,   !- Reheat Coil Object Type",
        "  Main Zone Reheat Coil,   !- Reheat Coil Name",
        "  ,                        !- Maximum Hot Water or Steam Flow Rate {m3/s}",
        "  ,                        !- Minimum Hot Water or Steam Flow Rate {m3/s}",
        "  Main Zone Supply Inlet,  !- Air Outlet Node Name",
        "  0.001,                   !- Convergence Tolerance",
        "  Reverse,                 !- Damper Heating Action",
        "  ,                        !- Maximum Flow per Zone Floor Area During Reheat {m3/s-m2}",
        "  ;                        !- Maximum Flow Fraction During Reheat",

        "Coil:Heating:Electric,",
        "  Main Zone Reheat Coil,   !- Name",
        "  ,                        !- Availability Schedule Name",
        "  1.0,                     !- Efficiency",
        "  17542.25595,             !- Nominal Capacity {W}",
        "  Main Zone Damper Outlet,  !- Air Inlet Node Name",
        "  Main Zone Supply Inlet;  !- Air Outlet Node Name",

        "ZoneHVAC:UnitHeater,",
        "  UnitHeater,              !- Name",
        "  Alwayson,                !- Availability Schedule Name",
        "  Node 32,                 !- Air Inlet Node Name",
        "  Node 33,                 !- Air Outlet Node Name",
        "  Fan:OnOff,               !- Supply Air Fan Object Type",
        "  UnitHeater_SupplyFan,    !- Supply Air Fan Name",
        "  0.1888,                  !- Maximum Supply Air Flow Rate {m3/s}",
        "  Coil:Heating:Electric,   !- Heating Coil Object Type",
        "  UnitHeater_ElectricHeater,  !- Heating Coil Name",
        "  ,                        !- Supply Air Fan Operating Mode Schedule Name",
        "  No,                      !- Supply Air Fan Operation During No Heating",
        "  ,                        !- Maximum Hot Water or Steam Flow Rate {m3/s}",
        "  0.0,                     !- Minimum Hot Water or Steam Flow Rate {m3/s}",
        "  0.001;                   !- Heating Convergence Tolerance",

        "Fan:OnOff,",
        "  UnitHeater_SupplyFan,    !- Name",
        "  Alwayson,                !- Availability Schedule Name",
        "  0.5,                     !- Fan Total Efficiency",
        "  250.0,                   !- Pressure Rise {Pa}",
        "  0.1888,                  !- Maximum Flow Rate {m3/s}",
        "  0.9,                     !- Motor Efficiency",
        "  1.0,                     !- Motor In Airstream Fraction",
        "  Node 32,                 !- Air Inlet Node Name",
        "  UnitHeater_FanOutletNode,  !- Air Outlet Node Name",
        "  ,                        !- Fan Power Ratio Function of Speed Ratio Curve Name",
        "  ,                        !- Fan Efficiency Ratio Function of Speed Ratio Curve Name",
        "  General;                 !- End-Use Subcategory",

        "Coil:Heating:Electric,",
        "  UnitHeater_ElectricHeater,  !- Name",
        "  Alwayson,                !- Availability Schedule Name",
        "  1.0,                     !- Efficiency",
        "  15235.58538,             !- Nominal Capacity {W}",
        "  UnitHeater_FanOutletNode,  !- Air Inlet Node Name",
        "  Node 33;                 !- Air Outlet Node Name",

        "AirLoopHVAC,",
        "  VAV System,              !- Name",
        "  VAV System Controllers,  !- Controller List Name",
        "  VAV System Availability Managers,  !- Availability Manager List Name",
        "  1.13941,                 !- Design Supply Air Flow Rate {m3/s}",
        "  VAV System Branches,     !- Branch List Name",
        "  ,                        !- Connector List Name",
        "  VAV System Air Loop Inlet,  !- Supply Side Inlet Node Name",
        "  VAV System Return Air Outlet,  !- Demand Side Outlet Node Name",
        "  VAV System Supply Path Inlet,  !- Demand Side Inlet Node Names",
        "  VAV System Supply Fan Outlet;  !- Supply Side Outlet Node Names",

        "AirLoopHVAC:ControllerList,",
        "  VAV System Controllers,  !- Name",
        "  Controller:WaterCoil,    !- Controller 1 Object Type",
        "  VAV System Cooling Coil Controller;  !- Controller 1 Name",

        "BranchList,",
        "  VAV System Branches,     !- Name",
        "  VAV System Main Branch;  !- Branch Name",

        "Branch,",
        "  VAV System Main Branch,  !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  AirLoopHVAC:OutdoorAirSystem,  !- Component Object Type",
        "  VAV System OA System,    !- Component Name",
        "  VAV System Air Loop Inlet,  !- Component Inlet Node Name",
        "  VAV System Mixed Air Outlet,  !- Component Outlet Node Name",
        "  Coil:Cooling:Water,      !- Component Object Type",
        "  VAV System Cooling Coil,  !- Component Name",
        "  VAV System Mixed Air Outlet,  !- Component Inlet Node Name",
        "  VAV System Cooling Coil Outlet,  !- Component Outlet Node Name",
        "  Fan:VariableVolume,      !- Component Object Type",
        "  VAV System Supply Fan,   !- Component Name",
        "  VAV System Cooling Coil Outlet,  !- Component Inlet Node Name",
        "  VAV System Supply Fan Outlet;  !- Component Outlet Node Name",

        "AirLoopHVAC:SupplyPath,",
        "  VAV System Supply Path,  !- Name",
        "  VAV System Supply Path Inlet,  !- Supply Air Path Inlet Node Name",
        "  AirLoopHVAC:ZoneSplitter,  !- Component Object Type",
        "  VAV System Zone Splitter;  !- Component Name",

        "AirLoopHVAC:ZoneSplitter,",
        "  VAV System Zone Splitter,  !- Name",
        "  VAV System Supply Path Inlet,  !- Inlet Node Name",
        "  Main Zone Zone Equip Inlet;  !- Outlet Node Name",

        "AirLoopHVAC:ReturnPath,",
        "  VAV System Return Path,  !- Name",
        "  VAV System Return Air Outlet,  !- Return Air Path Outlet Node Name",
        "  AirLoopHVAC:ZoneMixer,   !- Component Object Type",
        "  VAV System Zone Mixer;   !- Component Name",

        "AirLoopHVAC:ZoneMixer,",
        "  VAV System Zone Mixer,   !- Name",
        "  VAV System Return Air Outlet,  !- Outlet Node Name",
        "  Main Zone Return Outlet;  !- Inlet Node Name",

        "AvailabilityManagerAssignmentList,",
        "  VAV System Availability Managers,  !- Name",
        "  AvailabilityManager:NightCycle,  !- Availability Manager Object Type",
        "  VAV System Availability;  !- Availability Manager Name",

        "AvailabilityManager:NightCycle,",
        "  VAV System Availability,  !- Name",
        "  Alwayson,                !- Applicability Schedule Name",
        "  FanAvailSched,           !- Fan Schedule Name",
        "  CycleOnAny,              !- Control Type",
        "  0.2,                     !- Thermostat Tolerance {deltaC}",
        "  FixedRunTime,            !- Cycling Run Time Control Type",
        "  3600.0;                  !- Cycling Run Time {s}",

        "SetpointManager:Warmest,",
        "  Setpoint Manager Warmest,  !- Name",
        "  Temperature,             !- Control Variable",
        "  VAV System,              !- HVAC Air Loop Name",
        "  12.78,                   !- Minimum Setpoint Temperature {C}",
        "  18.3,                    !- Maximum Setpoint Temperature {C}",
        "  MaximumTemperature,      !- Strategy",
        "  VAV System Supply Fan Outlet;  !- Setpoint Node or NodeList Name",

        "Coil:Cooling:Water,",
        "  VAV System Cooling Coil,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  0.00112294,              !- Design Water Flow Rate {m3/s}",
        "  1.13941,                 !- Design Air Flow Rate {m3/s}",
        "  7.22,                    !- Design Inlet Water Temperature {C}",
        "  25.56582,                !- Design Inlet Air Temperature {C}",
        "  12.8,                    !- Design Outlet Air Temperature {C}",
        "  0.0118139,               !- Design Inlet Air Humidity Ratio {kgWater/kgDryAir}",
        "  0.00847862,              !- Design Outlet Air Humidity Ratio {kgWater/kgDryAir}",
        "  VAV System Cooling Coil ChW Inlet,  !- Water Inlet Node Name",
        "  VAV System Cooling Coil ChW Outlet,  !- Water Outlet Node Name",
        "  VAV System Mixed Air Outlet,  !- Air Inlet Node Name",
        "  VAV System Cooling Coil Outlet,  !- Air Outlet Node Name",
        "  DetailedAnalysis,        !- Type of Analysis",
        "  CrossFlow;               !- Heat Exchanger Configuration",

        "Controller:WaterCoil,",
        "  VAV System Cooling Coil Controller,  !- Name",
        "  Temperature,             !- Control Variable",
        "  Reverse,                 !- Action",
        "  Flow,                    !- Actuator Variable",
        "  VAV System Cooling Coil Outlet,  !- Sensor Node Name",
        "  VAV System Cooling Coil ChW Inlet,  !- Actuator Node Name",
        "  0.000424056,             !- Controller Convergence Tolerance {deltaC}",
        "  0.00112294,              !- Maximum Actuated Flow {m3/s}",
        "  0.0;                     !- Minimum Actuated Flow {m3/s}",

        "SetpointManager:MixedAir,",
        "  VAV System Cooling Coil Air Temp Manager,  !- Name",
        "  Temperature,             !- Control Variable",
        "  VAV System Supply Fan Outlet,  !- Reference Setpoint Node Name",
        "  VAV System Cooling Coil Outlet,  !- Fan Inlet Node Name",
        "  VAV System Supply Fan Outlet,  !- Fan Outlet Node Name",
        "  VAV System Mixed Air Nodes;  !- Setpoint Node or NodeList Name",

        "NodeList,",
        "  VAV System Mixed Air Nodes,  !- Name",
        "  VAV System Cooling Coil Outlet,  !- Node Name",
        "  VAV System Mixed Air Outlet;  !- Node Name",

        "Branch,",
        "  VAV System Cooling Coil ChW Branch,  !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  Coil:Cooling:Water,      !- Component Object Type",
        "  VAV System Cooling Coil,  !- Component Name",
        "  VAV System Cooling Coil ChW Inlet,  !- Component Inlet Node Name",
        "  VAV System Cooling Coil ChW Outlet;  !- Component Outlet Node Name",

        "Fan:VariableVolume,",
        "  VAV System Supply Fan,   !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  0.7,                     !- Fan Total Efficiency",
        "  600.0,                   !- Pressure Rise {Pa}",
        "  1.13941,                 !- Maximum Flow Rate {m3/s}",
        "  Fraction,                !- Fan Power Minimum Flow Rate Input Method",
        "  0.25,                    !- Fan Power Minimum Flow Fraction",
        "  ,                        !- Fan Power Minimum Air Flow Rate {m3/s}",
        "  0.9,                     !- Motor Efficiency",
        "  1.0,                     !- Motor In Airstream Fraction",
        "  0.35071223,              !- Fan Power Coefficient 1",
        "  0.30850535,              !- Fan Power Coefficient 2",
        "  -0.54137364,             !- Fan Power Coefficient 3",
        "  0.87198823,              !- Fan Power Coefficient 4",
        "  0.0,                     !- Fan Power Coefficient 5",
        "  VAV System Cooling Coil Outlet,  !- Air Inlet Node Name",
        "  VAV System Supply Fan Outlet;  !- Air Outlet Node Name",

        "OutdoorAir:NodeList,",
        "  VAV System Outside Air Inlet;  !- Node or NodeList Name",

        "AirLoopHVAC:OutdoorAirSystem,",
        "  VAV System OA System,    !- Name",
        "  VAV System OA System Controllers,  !- Controller List Name",
        "  VAV System OA System Equipment;  !- Outdoor Air Equipment List Name",

        "AirLoopHVAC:ControllerList,",
        "  VAV System OA System Controllers,  !- Name",
        "  Controller:OutdoorAir,   !- Controller 1 Object Type",
        "  VAV System OA Controller;  !- Controller 1 Name",

        "AirLoopHVAC:OutdoorAirSystem:EquipmentList,",
        "  VAV System OA System Equipment,  !- Name",
        "  OutdoorAir:Mixer,        !- Component 1 Object Type",
        "  VAV System OA Mixing Box;  !- Component 1 Name",

        "OutdoorAir:Mixer,",
        "  VAV System OA Mixing Box,  !- Name",
        "  VAV System Mixed Air Outlet,  !- Mixed Air Node Name",
        "  VAV System Outside Air Inlet,  !- Outdoor Air Stream Node Name",
        "  VAV System Relief Air Outlet,  !- Relief Air Stream Node Name",
        "  VAV System Air Loop Inlet;  !- Return Air Stream Node Name",

        "Controller:OutdoorAir,",
        "  VAV System OA Controller,  !- Name",
        "  VAV System Relief Air Outlet,  !- Relief Air Outlet Node Name",
        "  VAV System Air Loop Inlet,  !- Return Air Node Name",
        "  VAV System Mixed Air Outlet,  !- Mixed Air Node Name",
        "  VAV System Outside Air Inlet,  !- Actuator Node Name",
        "  0.1888,                  !- Minimum Outdoor Air Flow Rate {m3/s}",
        "  1.13941,                 !- Maximum Outdoor Air Flow Rate {m3/s}",
        "  DifferentialDryBulb,     !- Economizer Control Type",
        "  ModulateFlow,            !- Economizer Control Action Type",
        "  19.0,                    !- Economizer Maximum Limit Dry-Bulb Temperature {C}",
        "  ,                        !- Economizer Maximum Limit Enthalpy {J/kg}",
        "  ,                        !- Economizer Maximum Limit Dewpoint Temperature {C}",
        "  ,                        !- Electronic Enthalpy Limit Curve Name",
        "  4.0,                     !- Economizer Minimum Limit Dry-Bulb Temperature {C}",
        "  NoLockout,               !- Lockout Type",
        "  FixedMinimum,            !- Minimum Limit Type",
        "  Min OA Sched;            !- Minimum Outdoor Air Schedule Name",

        "Chiller:Electric:EIR,",
        "  Main Chiller,            !- Name",
        "  Autosize,                !- Reference Capacity {W}",
        "  3.2,                     !- Reference COP {W/W}",
        "  6.67,                    !- Reference Leaving Chilled Water Temperature {C}",
        "  29.4,                    !- Reference Entering Condenser Fluid Temperature {C}",
        "  Autosize,                !- Reference Chilled Water Flow Rate {m3/s}",
        "  Autosize,                !- Reference Condenser Fluid Flow Rate {m3/s}",
        "  Main Chiller RecipCapFT,  !- Cooling Capacity Function of Temperature Curve Name",
        "  Main Chiller RecipEIRFT,  !- Electric Input to Cooling Output Ratio Function of Temperature Curve Name",
        "  Main Chiller RecipEIRFPLR,  !- Electric Input to Cooling Output Ratio Function of Part Load Ratio Curve Name",
        "  0.0,                     !- Minimum Part Load Ratio",
        "  1.0,                     !- Maximum Part Load Ratio",
        "  1.0,                     !- Optimum Part Load Ratio",
        "  0.25,                    !- Minimum Unloading Ratio",
        "  Main Chiller ChW Inlet,  !- Chilled Water Inlet Node Name",
        "  Main Chiller ChW Outlet,  !- Chilled Water Outlet Node Name",
        "  Main Chiller Cnd Inlet,  !- Condenser Inlet Node Name",
        "  Main Chiller Cnd Outlet,  !- Condenser Outlet Node Name",
        "  AirCooled,               !- Condenser Type",
        "  0.035,                   !- Condenser Fan Power Ratio {W/W}",
        "  1.0,                     !- Fraction of Compressor Electric Consumption Rejected by Condenser",
        "  5.0,                     !- Leaving Chilled Water Lower Temperature Limit {C}",
        "  ConstantFlow,            !- Chiller Flow Mode",
        "  0.0,                     !- Design Heat Recovery Water Flow Rate {m3/s}",
        "  ,                        !- Heat Recovery Inlet Node Name",
        "  ,                        !- Heat Recovery Outlet Node Name",
        "  1.0;                     !- Sizing Factor",

        "Branch,",
        "  Main Chiller ChW Branch,  !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  Chiller:Electric:EIR,    !- Component Object Type",
        "  Main Chiller,            !- Component Name",
        "  Main Chiller ChW Inlet,  !- Component Inlet Node Name",
        "  Main Chiller ChW Outlet;  !- Component Outlet Node Name",

        "Curve:Biquadratic,",
        "  Main Chiller RecipCapFT,  !- Name",
        "  0.507883,                !- Coefficient1 Constant",
        "  0.145228,                !- Coefficient2 x",
        "  -0.00625644,             !- Coefficient3 x**2",
        "  -0.0011178,              !- Coefficient4 y",
        "  -0.0001296,              !- Coefficient5 y**2",
        "  -0.00028188,             !- Coefficient6 x*y",
        "  5.0,                     !- Minimum Value of x {BasedOnField A2}",
        "  10.0,                    !- Maximum Value of x {BasedOnField A2}",
        "  24.0,                    !- Minimum Value of y {BasedOnField A3}",
        "  35.0;                    !- Maximum Value of y {BasedOnField A3}",

        "Curve:Biquadratic,",
        "  Main Chiller RecipEIRFT,  !- Name",
        "  1.03076,                 !- Coefficient1 Constant",
        "  -0.103536,               !- Coefficient2 x",
        "  0.00710208,              !- Coefficient3 x**2",
        "  0.0093186,               !- Coefficient4 y",
        "  0.00031752,              !- Coefficient5 y**2",
        "  -0.00104328,             !- Coefficient6 x*y",
        "  5.0,                     !- Minimum Value of x {BasedOnField A2}",
        "  10.0,                    !- Maximum Value of x {BasedOnField A2}",
        "  24.0,                    !- Minimum Value of y {BasedOnField A3}",
        "  35.0;                    !- Maximum Value of y {BasedOnField A3}",

        "Curve:Quadratic,",
        "  Main Chiller RecipEIRFPLR,  !- Name",
        "  0.088065,                !- Coefficient1 Constant",
        "  1.137742,                !- Coefficient2 x",
        "  -0.225806,               !- Coefficient3 x**2",
        "  0.0,                     !- Minimum Value of x {BasedOnField A2}",
        "  1.0;                     !- Maximum Value of x {BasedOnField A2}",

        "OutdoorAir:Node,",
        "  Main Chiller Cnd Inlet,  !- Name",
        "  -1.0;                    !- Height Above Ground {m}",

        "Sizing:Plant,",
        "  Chilled Water Loop Chilled Water Loop,  !- Plant or Condenser Loop Name",
        "  Cooling,                 !- Loop Type",
        "  7.22,                    !- Design Loop Exit Temperature {C}",
        "  6.67;                    !- Loop Design Temperature Difference {deltaC}",

        "PlantLoop,",
        "  Chilled Water Loop Chilled Water Loop,  !- Name",
        "  Water,                   !- Fluid Type",
        "  ,                        !- User Defined Fluid Type",
        "  Chilled Water Loop Chiller Operation,  !- Plant Equipment Operation Scheme Name",
        "  Chilled Water Loop ChW Supply Outlet,  !- Loop Temperature Setpoint Node Name",
        "  98.0,                    !- Maximum Loop Temperature {C}",
        "  1.0,                     !- Minimum Loop Temperature {C}",
        "  Autosize,                !- Maximum Loop Flow Rate {m3/s}",
        "  0.0,                     !- Minimum Loop Flow Rate {m3/s}",
        "  Autocalculate,           !- Plant Loop Volume {m3}",
        "  Chilled Water Loop ChW Supply Inlet,  !- Plant Side Inlet Node Name",
        "  Chilled Water Loop ChW Supply Outlet,  !- Plant Side Outlet Node Name",
        "  Chilled Water Loop ChW Supply Side Branches,  !- Plant Side Branch List Name",
        "  Chilled Water Loop ChW Supply Side Connectors,  !- Plant Side Connector List Name",
        "  Chilled Water Loop ChW Demand Inlet,  !- Demand Side Inlet Node Name",
        "  Chilled Water Loop ChW Demand Outlet,  !- Demand Side Outlet Node Name",
        "  Chilled Water Loop ChW Demand Side Branches,  !- Demand Side Branch List Name",
        "  Chilled Water Loop ChW Demand Side Connectors,  !- Demand Side Connector List Name",
        "  SequentialLoad,          !- Load Distribution Scheme",
        "  Chilled Water Loop Availability List,  !- Availability Manager List Name",
        "  SingleSetpoint,          !- Plant Loop Demand Calculation Scheme",
        "  None;                    !- Common Pipe Simulation",

        "PlantEquipmentOperationSchemes,",
        "  Chilled Water Loop Chiller Operation,  !- Name",
        "  PlantEquipmentOperation:CoolingLoad,  !- Control Scheme 1 Object Type",
        "  Chilled Water Loop Chiller Operation All Hours,  !- Control Scheme 1 Name",
        "  Alwayson;                !- Control Scheme 1 Schedule Name",

        "PlantEquipmentOperation:CoolingLoad,",
        "  Chilled Water Loop Chiller Operation All Hours,  !- Name",
        "  0.0,                     !- Load Range 1 Lower Limit {W}",
        "  1000000000000000.0,      !- Load Range 1 Upper Limit {W}",
        "  Chilled Water Loop All Chillers;  !- Range 1 Equipment List Name",

        "PlantEquipmentList,",
        "  Chilled Water Loop All Chillers,  !- Name",
        "  Chiller:Electric:EIR,    !- Equipment Object Type",
        "  Main Chiller;            !- Equipment Name",

        "NodeList,",
        "  Chilled Water Loop ChW Supply Setpoint Nodes,  !- Name",
        "  Main Chiller ChW Outlet,  !- Node Name",
        "  Chilled Water Loop ChW Supply Outlet;  !- Node Name",

        "Schedule:Compact,",
        "  HVACTemplate-Always 7.22,  !- Name",
        "  ANY NUMBER,              !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  7.22;                    !- Field",

        "SetpointManager:Scheduled,",
        "  Chilled Water Loop ChW Temp Manager,  !- Name",
        "  Temperature,             !- Control Variable",
        "  HVACTemplate-Always 7.22,  !- Schedule Name",
        "  Chilled Water Loop ChW Supply Setpoint Nodes;  !- Setpoint Node or NodeList Name",

        "BranchList,",
        "  Chilled Water Loop ChW Supply Side Branches,  !- Name",
        "  Chilled Water Loop ChW Supply Inlet Branch,  !- Branch Name",
        "  Main Chiller ChW Branch,  !- Branch Name",
        "  Chilled Water Loop ChW Supply Bypass Branch,  !- Branch Name",
        "  Chilled Water Loop ChW Supply Outlet Branch;  !- Branch Name",

        "ConnectorList,",
        "  Chilled Water Loop ChW Supply Side Connectors,  !- Name",
        "  Connector:Splitter,      !- Connector 1 Object Type",
        "  Chilled Water Loop ChW Supply Splitter,  !- Connector 1 Name",
        "  Connector:Mixer,         !- Connector 2 Object Type",
        "  Chilled Water Loop ChW Supply Mixer;  !- Connector 2 Name",

        "Connector:Splitter,",
        "  Chilled Water Loop ChW Supply Splitter,  !- Name",
        "  Chilled Water Loop ChW Supply Inlet Branch,  !- Inlet Branch Name",
        "  Chilled Water Loop ChW Supply Bypass Branch,  !- Outlet Branch Name",
        "  Main Chiller ChW Branch;  !- Outlet Branch Name",

        "Connector:Mixer,",
        "  Chilled Water Loop ChW Supply Mixer,  !- Name",
        "  Chilled Water Loop ChW Supply Outlet Branch,  !- Outlet Branch Name",
        "  Chilled Water Loop ChW Supply Bypass Branch,  !- Inlet Branch Name",
        "  Main Chiller ChW Branch;  !- Inlet Branch Name",

        "Branch,",
        "  Chilled Water Loop ChW Supply Bypass Branch,  !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  Pipe:Adiabatic,          !- Component Object Type",
        "  Chilled Water Loop ChW Supply Side Bypass Pipe,  !- Component Name",
        "  Chilled Water Loop ChW Supply Bypass Inlet,  !- Component Inlet Node Name",
        "  Chilled Water Loop ChW Supply Bypass Outlet;  !- Component Outlet Node Name",

        "Pipe:Adiabatic,",
        "  Chilled Water Loop ChW Supply Side Bypass Pipe,  !- Name",
        "  Chilled Water Loop ChW Supply Bypass Inlet,  !- Inlet Node Name",
        "  Chilled Water Loop ChW Supply Bypass Outlet;  !- Outlet Node Name",

        "Pump:ConstantSpeed,",
        "  Chilled Water Loop ChW Supply Pump,  !- Name",
        "  Chilled Water Loop ChW Supply Inlet,  !- Inlet Node Name",
        "  Chilled Water Loop ChW Pump Outlet,  !- Outlet Node Name",
        "  Autosize,                !- Design Flow Rate {m3/s}",
        "  179352.0,                !- Design Pump Head {Pa}",
        "  Autosize,                !- Design Power Consumption {W}",
        "  0.9,                     !- Motor Efficiency",
        "  0.0,                     !- Fraction of Motor Inefficiencies to Fluid Stream",
        "  Intermittent;            !- Pump Control Type",

        "Branch,",
        "  Chilled Water Loop ChW Supply Inlet Branch,  !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  Pump:ConstantSpeed,      !- Component Object Type",
        "  Chilled Water Loop ChW Supply Pump,  !- Component Name",
        "  Chilled Water Loop ChW Supply Inlet,  !- Component Inlet Node Name",
        "  Chilled Water Loop ChW Pump Outlet;  !- Component Outlet Node Name",

        "Branch,",
        "  Chilled Water Loop ChW Supply Outlet Branch,  !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  Pipe:Adiabatic,          !- Component Object Type",
        "  Chilled Water Loop ChW Supply Outlet Pipe,  !- Component Name",
        "  Chilled Water Loop ChW Supply Outlet Pipe Inlet,  !- Component Inlet Node Name",
        "  Chilled Water Loop ChW Supply Outlet;  !- Component Outlet Node Name",

        "Pipe:Adiabatic,",
        "  Chilled Water Loop ChW Supply Outlet Pipe,  !- Name",
        "  Chilled Water Loop ChW Supply Outlet Pipe Inlet,  !- Inlet Node Name",
        "  Chilled Water Loop ChW Supply Outlet;  !- Outlet Node Name",

        "BranchList,",
        "  Chilled Water Loop ChW Demand Side Branches,  !- Name",
        "  Chilled Water Loop ChW Demand Inlet Branch,  !- Branch Name",
        "  VAV System Cooling Coil ChW Branch,  !- Branch Name",
        "  Chilled Water Loop ChW Demand Bypass Branch,  !- Branch Name",
        "  Chilled Water Loop ChW Demand Outlet Branch;  !- Branch Name",

        "ConnectorList,",
        "  Chilled Water Loop ChW Demand Side Connectors,  !- Name",
        "  Connector:Splitter,      !- Connector 1 Object Type",
        "  Chilled Water Loop ChW Demand Splitter,  !- Connector 1 Name",
        "  Connector:Mixer,         !- Connector 2 Object Type",
        "  Chilled Water Loop ChW Demand Mixer;  !- Connector 2 Name",

        "Connector:Splitter,",
        "  Chilled Water Loop ChW Demand Splitter,  !- Name",
        "  Chilled Water Loop ChW Demand Inlet Branch,  !- Inlet Branch Name",
        "  Chilled Water Loop ChW Demand Bypass Branch,  !- Outlet Branch Name",
        "  VAV System Cooling Coil ChW Branch;  !- Outlet Branch Name",

        "Connector:Mixer,",
        "  Chilled Water Loop ChW Demand Mixer,  !- Name",
        "  Chilled Water Loop ChW Demand Outlet Branch,  !- Outlet Branch Name",
        "  Chilled Water Loop ChW Demand Bypass Branch,  !- Inlet Branch Name",
        "  VAV System Cooling Coil ChW Branch;  !- Inlet Branch Name",

        "Branch,",
        "  Chilled Water Loop ChW Demand Inlet Branch,  !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  Pipe:Adiabatic,          !- Component Object Type",
        "  Chilled Water Loop ChW Demand Inlet Pipe,  !- Component Name",
        "  Chilled Water Loop ChW Demand Inlet,  !- Component Inlet Node Name",
        "  Chilled Water Loop ChW Demand Inlet Pipe Outlet;  !- Component Outlet Node Name",

        "Pipe:Adiabatic,",
        "  Chilled Water Loop ChW Demand Inlet Pipe,  !- Name",
        "  Chilled Water Loop ChW Demand Inlet,  !- Inlet Node Name",
        "  Chilled Water Loop ChW Demand Inlet Pipe Outlet;  !- Outlet Node Name",

        "Branch,",
        "  Chilled Water Loop ChW Demand Bypass Branch,  !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  Pipe:Adiabatic,          !- Component Object Type",
        "  Chilled Water Loop ChW Demand Side Bypass Pipe,  !- Component Name",
        "  Chilled Water Loop ChW Demand Bypass Inlet,  !- Component Inlet Node Name",
        "  Chilled Water Loop ChW Demand Bypass Outlet;  !- Component Outlet Node Name",

        "Pipe:Adiabatic,",
        "  Chilled Water Loop ChW Demand Side Bypass Pipe,  !- Name",
        "  Chilled Water Loop ChW Demand Bypass Inlet,  !- Inlet Node Name",
        "  Chilled Water Loop ChW Demand Bypass Outlet;  !- Outlet Node Name",

        "Branch,",
        "  Chilled Water Loop ChW Demand Outlet Branch,  !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  Pipe:Adiabatic,          !- Component Object Type",
        "  Chilled Water Loop ChW Demand Outlet Pipe,  !- Component Name",
        "  Chilled Water Loop ChW Demand Outlet Pipe Inlet,  !- Component Inlet Node Name",
        "  Chilled Water Loop ChW Demand Outlet;  !- Component Outlet Node Name",

        "Pipe:Adiabatic,",
        "  Chilled Water Loop ChW Demand Outlet Pipe,  !- Name",
        "  Chilled Water Loop ChW Demand Outlet Pipe Inlet,  !- Inlet Node Name",
        "  Chilled Water Loop ChW Demand Outlet;  !- Outlet Node Name",

        "AvailabilityManagerAssignmentList,",
        "  Chilled Water Loop Availability List,  !- Name",
        "  AvailabilityManager:LowTemperatureTurnOff,  !- Availability Manager Object Type",
        "  Chilled Water Loop Availability Low Temp Off;  !- Availability Manager Name",

        "AvailabilityManager:LowTemperatureTurnOff,",
        "  Chilled Water Loop Availability Low Temp Off,  !- Name",
        "  Chilled Water Loop Outside Air Sensor,  !- Sensor Node Name",
        "  7.22;                    !- Temperature {C}",

        "OutdoorAir:Node,",
        "  Chilled Water Loop Outside Air Sensor,  !- Name",
        "  -1.0;                    !- Height Above Ground {m}",
    });
    ASSERT_TRUE(process_idf(idf_objects));

    // OutputProcessor::TimeValue.allocate(2);
    state->dataGlobal->DDOnlySimulation = true;

    ManageSimulation(*state);

    EXPECT_EQ(state->dataZoneEquip->ZoneEquipList(1).NumOfEquipTypes, 2);
    // first priority zone equipment is zone ADU
    EXPECT_EQ(state->dataZoneEquipmentManager->PrioritySimOrder(1).EquipTypeName, "ZONEHVAC:AIRDISTRIBUTIONUNIT");
    EXPECT_EQ(state->dataZoneEquipmentManager->PrioritySimOrder(1).EquipName, "MAIN ZONE ATU");
    EXPECT_EQ(state->dataHeatingCoils->HeatingCoil(1).Name, "MAIN ZONE REHEAT COIL");
    // second priority zone equipment is unit heater
    EXPECT_EQ(state->dataZoneEquipmentManager->PrioritySimOrder(2).EquipTypeName, "ZONEHVAC:UNITHEATER");
    EXPECT_EQ(state->dataZoneEquipmentManager->PrioritySimOrder(2).EquipName, "UNITHEATER");
    EXPECT_EQ(state->dataHeatingCoils->HeatingCoil(2).Name, "UNITHEATER_ELECTRICHEATER");
    // check the reheat coil output
    EXPECT_NEAR(state->dataHeatingCoils->HeatingCoil(1).HeatingCoilRate, 7028.9, 1.0);
    // check the unit heater heating coil output
    EXPECT_EQ(state->dataHeatingCoils->HeatingCoil(2).HeatingCoilRate, 0.0);

    // re-set the hour of the day
    state->dataGlobal->TimeStep = 1;
    state->dataGlobal->HourOfDay = 24;
    state->dataGlobal->CurrentTime = 24.0;
    // set zone air node condition
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp = 20.0;
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat = 0.005;
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Enthalpy =
        Psychrometrics::PsyHFnTdbW(state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp,
                                   state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat);
    // set the zone loads
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).TotalOutputRequired = 0.0;
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).OutputRequiredToHeatingSP = 15000.0;
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).OutputRequiredToCoolingSP = 20000.0;
    // local variables
    bool SimZoneEquipment = true;
    bool SimAirLoops = true;
    bool FirstHVACIteration = false;
    // re-simulate the zone HVAC equipment per the priority order
    ZoneEquipmentManager::ManageZoneEquipment(*state, FirstHVACIteration, SimZoneEquipment, SimAirLoops);
    // check the reheat coil nominal capacity
    EXPECT_NEAR(state->dataHeatingCoils->HeatingCoil(1).NominalCapacity, 17542.3, 1.0);
    // check the reheat coil outputis the full capacity
    EXPECT_NEAR(state->dataHeatingCoils->HeatingCoil(1).HeatingCoilRate, 17542.3, 1.0);
    // check the unit heater heating coil is handling the remaining load
    EXPECT_NEAR(state->dataHeatingCoils->HeatingCoil(2).HeatingCoilRate, 213.9, 1.0);
    // finaly check that RemaingingOutputRequired is zero
    EXPECT_EQ(state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputRequired, 0.0);
}
