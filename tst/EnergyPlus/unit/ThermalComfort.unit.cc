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

// EnergyPlus::ThermalComfort Unit Tests

// Google Test Headers
#include <gtest/gtest.h>

// EnergyPlus Headers
#include "Fixtures/EnergyPlusFixture.hh"
#include <EnergyPlus/ConfiguredFunctions.hh>
#include <EnergyPlus/Construction.hh>
#include <EnergyPlus/Data/EnergyPlusData.hh>
#include <EnergyPlus/DataEnvironment.hh>
#include <EnergyPlus/DataGlobals.hh>
#include <EnergyPlus/DataHVACGlobals.hh>
#include <EnergyPlus/DataHeatBalFanSys.hh>
#include <EnergyPlus/DataHeatBalSurface.hh>
#include <EnergyPlus/DataHeatBalance.hh>
#include <EnergyPlus/DataRoomAirModel.hh>
#include <EnergyPlus/DataSurfaces.hh>
#include <EnergyPlus/DataViewFactorInformation.hh>
#include <EnergyPlus/DataZoneEnergyDemands.hh>
#include <EnergyPlus/IOFiles.hh>
#include <EnergyPlus/Psychrometrics.hh>
#include <EnergyPlus/ScheduleManager.hh>
#include <EnergyPlus/SimulationManager.hh>
#include <EnergyPlus/ThermalComfort.hh>
#include <EnergyPlus/ZoneTempPredictorCorrector.hh>

using namespace EnergyPlus;
using namespace EnergyPlus::ThermalComfort;
using namespace EnergyPlus::DataEnvironment;
using namespace EnergyPlus::DataHeatBalance;
using namespace EnergyPlus::RoomAir;
using namespace EnergyPlus::DataHeatBalFanSys;
using namespace EnergyPlus::DataSurfaces;
using namespace EnergyPlus::DataHeatBalSurface;
// using namespace EnergyPlus::ScheduleManager;
using namespace SimulationManager;

TEST_F(EnergyPlusFixture, ThermalComfort_CalcIfSetPointMetTest1)
{
    state->dataGlobal->NumOfZones = 1;
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand.allocate(state->dataGlobal->NumOfZones);
    state->dataThermalComforts->ThermalComfortSetPoint.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBalFanSys->TempControlType.allocate(1);
    state->dataRoomAir->AirModel.allocate(state->dataGlobal->NumOfZones);
    state->dataRoomAir->AirModel(1).AirModel = RoomAir::RoomAirModel::Mixing;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBalFanSys->zoneTstatSetpts.allocate(state->dataGlobal->NumOfZones);
    state->dataGlobal->TimeStepZone = 0.25;
    state->dataThermalComforts->ThermalComfortInASH55.allocate(state->dataGlobal->NumOfZones);
    state->dataThermalComforts->ThermalComfortInASH55(1).ZoneIsOccupied = true;
    state->dataHeatBal->Zone.allocate(state->dataGlobal->NumOfZones);

    // SingleHeatingSetPoint thermostat

    state->dataHeatBalFanSys->TempControlType(1) = HVAC::SetptType::SingleHeat;

    // heating
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(1).ZTAV = 21.1;           // 70F
    state->dataHeatBalFanSys->zoneTstatSetpts(1).setptLo = 22.2;                     // 72F
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).TotalOutputRequired = 500.0; // must be greater than zero
    CalcIfSetPointMet(*state);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeating);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeatingOccupied);
    EXPECT_EQ(0., state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCooling);
    EXPECT_EQ(0., state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCoolingOccupied);

    // cooling
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(1).ZTAV = 25.0;            // 77F
    state->dataHeatBalFanSys->zoneTstatSetpts(1).setptHi = 23.9;                      // 75F
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).TotalOutputRequired = -500.0; // must be less than zero
    CalcIfSetPointMet(*state);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeating);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeatingOccupied);
    EXPECT_EQ(0., state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCooling);
    EXPECT_EQ(0., state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCoolingOccupied);

    // SingleCoolingSetPoint thermostat

    state->dataHeatBalFanSys->TempControlType(1) = HVAC::SetptType::SingleCool;

    // heating
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(1).ZTAV = 21.1;           // 70F
    state->dataHeatBalFanSys->zoneTstatSetpts(1).setptLo = 22.2;                     // 72F
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).TotalOutputRequired = 500.0; // must be greater than zero
    CalcIfSetPointMet(*state);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeating);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeatingOccupied);
    EXPECT_EQ(0., state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCooling);
    EXPECT_EQ(0., state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCoolingOccupied);

    // cooling
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(1).ZTAV = 25.0;            // 77F
    state->dataHeatBalFanSys->zoneTstatSetpts(1).setptHi = 23.9;                      // 75F
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).TotalOutputRequired = -500.0; // must be less than zero
    CalcIfSetPointMet(*state);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeating);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeatingOccupied);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCooling);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCoolingOccupied);

    // SingleHeatCoolSetPoint thermostat

    state->dataHeatBalFanSys->TempControlType(1) = HVAC::SetptType::SingleHeatCool;

    // heating
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(1).ZTAV = 21.1;           // 70F
    state->dataHeatBalFanSys->zoneTstatSetpts(1).setptLo = 22.2;                     // 72F
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).TotalOutputRequired = 500.0; // must be greater than zero
    CalcIfSetPointMet(*state);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeating);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeatingOccupied);
    EXPECT_EQ(0., state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCooling);
    EXPECT_EQ(0., state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCoolingOccupied);

    // cooling
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(1).ZTAV = 25.0;            // 77F
    state->dataHeatBalFanSys->zoneTstatSetpts(1).setptHi = 23.9;                      // 75F
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).TotalOutputRequired = -500.0; // must be less than zero
    CalcIfSetPointMet(*state);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeating);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeatingOccupied);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCooling);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCoolingOccupied);

    // DualHeatCool thermostat

    state->dataHeatBalFanSys->TempControlType(1) = HVAC::SetptType::DualHeatCool;

    // heating
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(1).ZTAV = 21.1;           // 70F
    state->dataHeatBalFanSys->zoneTstatSetpts(1).setptLo = 22.2;                     // 72F
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).TotalOutputRequired = 500.0; // must be greater than zero
    CalcIfSetPointMet(*state);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeating);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeatingOccupied);
    EXPECT_EQ(0., state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCooling);
    EXPECT_EQ(0., state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCoolingOccupied);

    // cooling
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(1).ZTAV = 25.0;            // 77F
    state->dataHeatBalFanSys->zoneTstatSetpts(1).setptHi = 23.9;                      // 75F
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).TotalOutputRequired = -500.0; // must be less than zero
    CalcIfSetPointMet(*state);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeating);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeatingOccupied);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCooling);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCoolingOccupied);
}

TEST_F(EnergyPlusFixture, ThermalComfort_CalcThermalComfortFanger)
{

    std::string const idf_objects = delimited_string({
        "People,",
        "  Space People,            !- Name",
        "  Space,                   !- Zone or ZoneList or Space or SpaceList Name",
        "  PeopleSchedule,          !- Number of People Schedule Name",
        "  People,                  !- Number of People Calculation Method",
        "  5.0,                     !- Number of People",
        "  ,                        !- People per Floor Area {person/m2}",
        "  ,                        !- Floor Area per Person {m2/person}",
        "  0.3,                     !- Fraction Radiant",
        "  Autocalculate,           !- Sensible Heat Fraction",
        "  Activity Schedule,       !- Activity Level Schedule Name",
        "  ,                        !- Carbon Dioxide Generation Rate {m3/s-W}",
        "  Yes,                     !- Enable ASHRAE 55 Comfort Warnings",
        "  EnclosureAveraged,       !- Mean Radiant Temperature Calculation Type",
        "  ,                        !- Surface Name/Angle Factor List Name",
        "  Work efficiency,         !- Work Efficiency Schedule Name",
        "  ClothingInsulationSchedule,  !- Clothing Insulation Calculation Method",
        "  ,                        !- Clothing Insulation Calculation Method Schedule Name",
        "  Clothing Schedule,       !- Clothing Insulation Schedule Name",
        "  AirVelocitySchedule,     !- Air Velocity Schedule Name",
        "  Fanger;                  !- Thermal Comfort Model 1 Type",

        "Schedule:Compact,",
        "  PeopleSchedule,          !- Name",
        "  Any Number,              !- Schedule Type Limits Name",
        "  Through: 12/30,          !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  1.0,                     !- Field",
        "  Through: 12/31,          !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  0.3;                     !- Field",

        "Schedule:Compact,",
        "  Activity Schedule,       !- Name",
        "  Any Number,              !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  70;                      !- Field",

        "Schedule:Compact,",
        "  Clothing Schedule,       !- Name",
        "  Any Number,              !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  1.0;                     !- Field",

        "Schedule:Compact,",
        "  AirVelocitySchedule,     !- Name",
        "  Any Number,              !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  0.0;                     !- Field",

        "Schedule:Compact,",
        "  Work efficiency,         !- Name",
        "  Any Number,              !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  0.0;                     !- Field",

        "Output:Diagnostics,",
        "  DisplayExtraWarnings;    !- Key",

        "Timestep,4;",

        "Building,",
        "  AirloopHVAC_VentilationRateProcedure,  !- Name",
        "  0.0,                     !- North Axis {deg}",
        "  Suburbs,                 !- Terrain",
        "  0.04,                    !- Loads Convergence Tolerance Value {W}",
        "  0.4,                     !- Temperature Convergence Tolerance Value {deltaC}",
        "  FullExterior,            !- Solar Distribution",
        "  25,                      !- Maximum Number of Warmup Days",
        "  6;                       !- Minimum Number of Warmup Days",

        "SimulationControl,",
        "  No,                      !- Do Zone Sizing Calculation",
        "  No,                      !- Do System Sizing Calculation",
        "  No,                      !- Do Plant Sizing Calculation",
        "  Yes,                     !- Run Simulation for Sizing Periods",
        "  No,                      !- Run Simulation for Weather File Run Periods",
        "  ,                        !- Do HVAC Sizing Simulation for Sizing Periods",
        "  ;                        !- Maximum Number of HVAC Sizing Simulation Passes",

        "ScheduleTypeLimits,",
        "  Any Number;              !- Name",

        "Site:Location,",
        "  Miami Intl Ap FL USA TMY3 WMO=722020E,  !- Name",
        "  25.82,                   !- Latitude {deg}",
        "  -80.3,                   !- Longitude {deg}",
        "  -5.0,                    !- Time Zone {hr}",
        "  11;                      !- Elevation {m}",

        "SizingPeriod:DesignDay,",
        "  Miami Intl Ap Ann Clg .4% Condns DB/MCWB,  !- Name",
        "  7,                       !- Month",
        "  21,                      !- Day of Month",
        "  SummerDesignDay,         !- Day Type",
        "  31.7,                    !- Maximum Dry-Bulb Temperature {C}",
        "  10.0,                    !- Daily Dry-Bulb Temperature Range {deltaC}",
        "  ,                        !- Dry-Bulb Temperature Range Modifier Type",
        "  ,                        !- Dry-Bulb Temperature Range Modifier Day Schedule Name",
        "  WetBulb,                 !- Humidity Condition Type",
        "  22.7,                    !- Wetbulb or DewPoint at Maximum Dry-Bulb {C}",
        "  ,                        !- Humidity Condition Day Schedule Name",
        "  ,                        !- Humidity Ratio at Maximum Dry-Bulb {kgWater/kgDryAir}",
        "  ,                        !- Enthalpy at Maximum Dry-Bulb {J/kg}",
        "  ,                        !- Daily Wet-Bulb Temperature Range {deltaC}",
        "  101217.0,                !- Barometric Pressure {Pa}",
        "  3.8,                     !- Wind Speed {m/s}",
        "  340,                     !- Wind Direction {deg}",
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
        "  Miami Intl Ap Ann Htg 99.6% Condns DB,  !- Name",
        "  1,                       !- Month",
        "  21,                      !- Day of Month",
        "  WinterDesignDay,         !- Day Type",
        "  8.7,                     !- Maximum Dry-Bulb Temperature {C}",
        "  0.0,                     !- Daily Dry-Bulb Temperature Range {deltaC}",
        "  ,                        !- Dry-Bulb Temperature Range Modifier Type",
        "  ,                        !- Dry-Bulb Temperature Range Modifier Day Schedule Name",
        "  WetBulb,                 !- Humidity Condition Type",
        "  8.7,                     !- Wetbulb or DewPoint at Maximum Dry-Bulb {C}",
        "  ,                        !- Humidity Condition Day Schedule Name",
        "  ,                        !- Humidity Ratio at Maximum Dry-Bulb {kgWater/kgDryAir}",
        "  ,                        !- Enthalpy at Maximum Dry-Bulb {J/kg}",
        "  ,                        !- Daily Wet-Bulb Temperature Range {deltaC}",
        "  101217.0,                !- Barometric Pressure {Pa}",
        "  3.8,                     !- Wind Speed {m/s}",
        "  340,                     !- Wind Direction {deg}",
        "  No,                      !- Rain Indicator",
        "  No,                      !- Snow Indicator",
        "  No,                      !- Daylight Saving Time Indicator",
        "  ASHRAEClearSky,          !- Solar Model Indicator",
        "  ,                        !- Beam Solar Day Schedule Name",
        "  ,                        !- Diffuse Solar Day Schedule Name",
        "  ,                        !- ASHRAE Clear Sky Optical Depth for Beam Irradiance (taub) {dimensionless}",
        "  ,                        !- ASHRAE Clear Sky Optical Depth for Diffuse Irradiance (taud) {dimensionless}",
        "  0.0;                     !- Sky Clearness",

        "Zone,",
        "  Space,                   !- Name",
        "  0.0,                     !- Direction of Relative North {deg}",
        "  0.0,                     !- X Origin {m}",
        "  0.0,                     !- Y Origin {m}",
        "  0.0,                     !- Z Origin {m}",
        "  1,                       !- Type",
        "  1,                       !- Multiplier",
        "  2.4,                     !- Ceiling Height {m}",
        "  ,                        !- Volume {m3}",
        "  Autocalculate,           !- Floor Area {m2}",
        "  ,                        !- Zone Inside Convection Algorithm",
        "  ,                        !- Zone Outside Convection Algorithm",
        "  Yes;                     !- Part of Total Floor Area",

        "ZoneGroup,",
        "  Zone Group,              !- Name",
        "  Zone List,               !- Zone List Name",
        "  10;                      !- Zone List Multiplier",

        "ZoneList,",
        "  Zone List,               !- Name",
        "  Spacex10;                !- Zone Name",

        "Zone,",
        "  Spacex10,                !- Name",
        "  0.0,                     !- Direction of Relative North {deg}",
        "  0.0,                     !- X Origin {m}",
        "  0.0,                     !- Y Origin {m}",
        "  0.0,                     !- Z Origin {m}",
        "  1,                       !- Type",
        "  1,                       !- Multiplier",
        "  2.4,                     !- Ceiling Height {m}",
        "  ,                        !- Volume {m3}",
        "  Autocalculate,           !- Floor Area {m2}",
        "  ,                        !- Zone Inside Convection Algorithm",
        "  ,                        !- Zone Outside Convection Algorithm",
        "  Yes;                     !- Part of Total Floor Area",

        "People,",
        "  Spacex10 People,         !- Name",
        "  Spacex10,                !- Zone or ZoneList or Space or SpaceList Name",
        "  OnSched,                 !- Number of People Schedule Name",
        "  People,                  !- Number of People Calculation Method",
        "  11,                      !- Number of People",
        "  ,                        !- People per Floor Area {person/m2}",
        "  ,                        !- Floor Area per Person {m2/person}",
        "  0.3,                     !- Fraction Radiant",
        "  Autocalculate,           !- Sensible Heat Fraction",
        "  Activity Schedule;       !- Activity Level Schedule Name",

        "Lights,",
        "  Space Lights,            !- Name",
        "  Space,                   !- Zone or ZoneList or Space or SpaceList Name",
        "  OnSched,                 !- Schedule Name",
        "  Watts/Area,              !- Design Level Calculation Method",
        "  ,                        !- Lighting Level {W}",
        "  10.0,                    !- Watts per Floor Area {W/m2}",
        "  ,                        !- Watts per Person {W/person}",
        "  0.0,                     !- Return Air Fraction",
        "  0.59,                    !- Fraction Radiant",
        "  0.2,                     !- Fraction Visible",
        "  0,                       !- Fraction Replaceable",
        "  GeneralLights;           !- End-Use Subcategory",

        "Lights,",
        "  Space Lights x10,        !- Name",
        "  Spacex10,                !- Zone or ZoneList or Space or SpaceList Name",
        "  OnSched,                 !- Schedule Name",
        "  Watts/Area,              !- Design Level Calculation Method",
        "  ,                        !- Lighting Level {W}",
        "  10.0,                    !- Watts per Floor Area {W/m2}",
        "  ,                        !- Watts per Person {W/person}",
        "  0.0,                     !- Return Air Fraction",
        "  0.59,                    !- Fraction Radiant",
        "  0.2,                     !- Fraction Visible",
        "  0,                       !- Fraction Replaceable",
        "  GeneralLights;           !- End-Use Subcategory",

        "ElectricEquipment,",
        "  Space ElecEq,            !- Name",
        "  Space,                   !- Zone or ZoneList or Space or SpaceList Name",
        "  OnSched,                 !- Schedule Name",
        "  Watts/Area,              !- Design Level Calculation Method",
        "  ,                        !- Design Level {W}",
        "  20.0,                    !- Watts per Floor Area {W/m2}",
        "  ,                        !- Watts per Person {W/person}",
        "  0.1,                     !- Fraction Latent",
        "  0.3,                     !- Fraction Radiant",
        "  0.1;                     !- Fraction Lost",

        "ElectricEquipment,",
        "  Space ElecEq x10,        !- Name",
        "  Spacex10,                !- Zone or ZoneList or Space or SpaceList Name",
        "  OnSched,                 !- Schedule Name",
        "  Watts/Area,              !- Design Level Calculation Method",
        "  ,                        !- Design Level {W}",
        "  20.0,                    !- Watts per Floor Area {W/m2}",
        "  ,                        !- Watts per Person {W/person}",
        "  0.1,                     !- Fraction Latent",
        "  0.3,                     !- Fraction Radiant",
        "  0.1;                     !- Fraction Lost",

        "Schedule:Compact,",
        "  OnSched,                 !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  1.0;                     !- Field",

        "ScheduleTypeLimits,",
        "  Fraction,                !- Name",
        "  0.0,                     !- Lower Limit Value {BasedOnField A3}",
        "  1.0,                     !- Upper Limit Value {BasedOnField A3}",
        "  Continuous;              !- Numeric Type",

        "Construction,",
        "  INT-WALL-1,              !- Name",
        "  GP02,                    !- Outside Layer",
        "  AL21,                    !- Layer 2",
        "  GP02;                    !- Layer 3",

        "Material,",
        "  GP02,                    !- Name",
        "  MediumSmooth,            !- Roughness",
        "  0.015900001,             !- Thickness {m}",
        "  0.16,                    !- Conductivity {W/m-K}",
        "  801.0,                   !- Density {kg/m3}",
        "  837.0,                   !- Specific Heat {J/kg-K}",
        "  0.9,                     !- Thermal Absorptance",
        "  0.75,                    !- Solar Absorptance",
        "  0.75;                    !- Visible Absorptance",

        "Material:AirGap,",
        "  AL21,                    !- Name",
        "  0.157;                   !- Thermal Resistance {m2-K/W}",

        "Construction,",
        "  FLOOR-SLAB-1,            !- Name",
        "  CC03,                    !- Outside Layer",
        "  CP01;                    !- Layer 2",

        "Material,",
        "  CC03,                    !- Name",
        "  MediumRough,             !- Roughness",
        "  0.1016,                  !- Thickness {m}",
        "  1.31,                    !- Conductivity {W/m-K}",
        "  2243.0,                  !- Density {kg/m3}",
        "  837.0,                   !- Specific Heat {J/kg-K}",
        "  0.9,                     !- Thermal Absorptance",
        "  0.65,                    !- Solar Absorptance",
        "  0.65;                    !- Visible Absorptance",

        "Material:NoMass,",
        "  CP01,                    !- Name",
        "  Rough,                   !- Roughness",
        "  0.367,                   !- Thermal Resistance {m2-K/W}",
        "  0.9,                     !- Thermal Absorptance",
        "  0.75,                    !- Solar Absorptance",
        "  0.75;                    !- Visible Absorptance",

        "Construction,",
        "  CLNG-1,                  !- Name",
        "  MAT-CLNG-1;              !- Outside Layer",

        "Material:NoMass,",
        "  MAT-CLNG-1,              !- Name",
        "  Rough,                   !- Roughness",
        "  0.65225929,              !- Thermal Resistance {m2-K/W}",
        "  0.65,                    !- Thermal Absorptance",
        "  0.65,                    !- Solar Absorptance",
        "  0.65;                    !- Visible Absorptance",

        "BuildingSurface:Detailed,",
        "  FRONT-1,                 !- Name",
        "  Wall,                    !- Surface Type",
        "  INT-WALL-1,              !- Construction Name",
        "  Space,                   !- Zone Name",
        "  ,                        !- Space Name",
        "  Outdoors,                !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  SunExposed,              !- Sun Exposure",
        "  WindExposed,             !- Wind Exposure",
        "  0.5,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  30.5,                    !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  30.5,                    !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  2.4;                     !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  C1-1,                    !- Name",
        "  Ceiling,                 !- Surface Type",
        "  CLNG-1,                  !- Construction Name",
        "  Space,                   !- Zone Name",
        "  ,                        !- Space Name",
        "  Outdoors,                !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  NoSun,                   !- Sun Exposure",
        "  NoWind,                  !- Wind Exposure",
        "  0.0,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  3.7,                     !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  30.5,                    !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  26.8,                    !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  2.4;                     !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  F1-1,                    !- Name",
        "  Floor,                   !- Surface Type",
        "  FLOOR-SLAB-1,            !- Construction Name",
        "  Space,                   !- Zone Name",
        "  ,                        !- Space Name",
        "  Outdoors,                !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  NoSun,                   !- Sun Exposure",
        "  NoWind,                  !- Wind Exposure",
        "  0.0,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  26.8,                    !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  30.5,                    !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  3.7,                     !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  0.0;                     !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  SB12,                    !- Name",
        "  Wall,                    !- Surface Type",
        "  INT-WALL-1,              !- Construction Name",
        "  Space,                   !- Zone Name",
        "  ,                        !- Space Name",
        "  Adiabatic,               !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  NoSun,                   !- Sun Exposure",
        "  NoWind,                  !- Wind Exposure",
        "  0.0,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  30.5,                    !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  30.5,                    !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  26.8,                    !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  26.8,                    !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  2.4;                     !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  SB14,                    !- Name",
        "  Wall,                    !- Surface Type",
        "  INT-WALL-1,              !- Construction Name",
        "  Space,                   !- Zone Name",
        "  ,                        !- Space Name",
        "  Adiabatic,               !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  NoSun,                   !- Sun Exposure",
        "  NoWind,                  !- Wind Exposure",
        "  0.0,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  3.7,                     !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  3.7,                     !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  2.4;                     !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  SB15,                    !- Name",
        "  Wall,                    !- Surface Type",
        "  INT-WALL-1,              !- Construction Name",
        "  Space,                   !- Zone Name",
        "  ,                        !- Space Name",
        "  Adiabatic,               !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  NoSun,                   !- Sun Exposure",
        "  NoWind,                  !- Wind Exposure",
        "  0.0,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  26.8,                    !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  26.8,                    !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  3.7,                     !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  3.7,                     !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  2.4;                     !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  FRONT-1x10,              !- Name",
        "  Wall,                    !- Surface Type",
        "  INT-WALL-1,              !- Construction Name",
        "  Spacex10,                !- Zone Name",
        "  ,                        !- Space Name",
        "  Outdoors,                !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  SunExposed,              !- Sun Exposure",
        "  WindExposed,             !- Wind Exposure",
        "  0.5,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  30.5,                    !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  30.5,                    !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  2.4;                     !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  C1-1x10,                 !- Name",
        "  Ceiling,                 !- Surface Type",
        "  CLNG-1,                  !- Construction Name",
        "  Spacex10,                !- Zone Name",
        "  ,                        !- Space Name",
        "  Outdoors,                !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  NoSun,                   !- Sun Exposure",
        "  NoWind,                  !- Wind Exposure",
        "  0.0,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  3.7,                     !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  30.5,                    !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  26.8,                    !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  2.4;                     !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  F1-1x10,                 !- Name",
        "  Floor,                   !- Surface Type",
        "  FLOOR-SLAB-1,            !- Construction Name",
        "  Spacex10,                !- Zone Name",
        "  ,                        !- Space Name",
        "  Outdoors,                !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  NoSun,                   !- Sun Exposure",
        "  NoWind,                  !- Wind Exposure",
        "  0.0,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  26.8,                    !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  30.5,                    !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  3.7,                     !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  0.0;                     !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  SB12x10,                 !- Name",
        "  Wall,                    !- Surface Type",
        "  INT-WALL-1,              !- Construction Name",
        "  Spacex10,                !- Zone Name",
        "  ,                        !- Space Name",
        "  Adiabatic,               !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  NoSun,                   !- Sun Exposure",
        "  NoWind,                  !- Wind Exposure",
        "  0.0,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  30.5,                    !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  30.5,                    !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  26.8,                    !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  26.8,                    !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  2.4;                     !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  SB14x10,                 !- Name",
        "  Wall,                    !- Surface Type",
        "  INT-WALL-1,              !- Construction Name",
        "  Spacex10,                !- Zone Name",
        "  ,                        !- Space Name",
        "  Adiabatic,               !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  NoSun,                   !- Sun Exposure",
        "  NoWind,                  !- Wind Exposure",
        "  0.0,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  3.7,                     !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  3.7,                     !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  2.4;                     !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  SB15x10,                 !- Name",
        "  Wall,                    !- Surface Type",
        "  INT-WALL-1,              !- Construction Name",
        "  Spacex10,                !- Zone Name",
        "  ,                        !- Space Name",
        "  Adiabatic,               !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  NoSun,                   !- Sun Exposure",
        "  NoWind,                  !- Wind Exposure",
        "  0.0,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  26.8,                    !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  26.8,                    !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  3.7,                     !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  3.7,                     !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  2.4;                     !- Vertex Z-coordinate {m}",

        "Output:Table:SummaryReports,",
        "  AllSummary;              !- Report Name",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    // OutputProcessor::TimeValue.allocate(2);
    state->dataGlobal->DDOnlySimulation = true;

    ManageSimulation(*state);

    //	compare_err_stream( "" );

    auto &zoneHB1 = state->dataZoneTempPredictorCorrector->zoneHeatBalance(1);
    zoneHB1.ZTAVComf = 25.0;
    zoneHB1.MRT = 26.0;
    state->dataViewFactor->EnclRadInfo(1).MRT = 26.0;
    zoneHB1.airHumRatAvgComf = 0.00529; // 0.002 to 0.006

    CalcThermalComfortFanger(*state);

    EXPECT_NEAR(state->dataThermalComforts->ThermalComfortData(1).FangerPMV, -1.262, 0.005);
    EXPECT_NEAR(state->dataThermalComforts->ThermalComfortData(1).FangerPPD, 38.1, 0.1);

    zoneHB1.ZTAVComf = 26.0;
    zoneHB1.MRT = 27.0;
    state->dataViewFactor->EnclRadInfo(1).MRT = 27.0;
    zoneHB1.airHumRatAvgComf = 0.00529; // 0.002 to 0.006

    CalcThermalComfortFanger(*state);

    EXPECT_NEAR(state->dataThermalComforts->ThermalComfortData(1).FangerPMV, -0.860, 0.005);
    EXPECT_NEAR(state->dataThermalComforts->ThermalComfortData(1).FangerPPD, 20.5, 0.1);

    zoneHB1.ZTAVComf = 27.0;
    zoneHB1.MRT = 28.0;
    state->dataViewFactor->EnclRadInfo(1).MRT = 28.0;
    zoneHB1.airHumRatAvgComf = 0.00529; // 0.002 to 0.006

    CalcThermalComfortFanger(*state);

    EXPECT_NEAR(state->dataThermalComforts->ThermalComfortData(1).FangerPMV, -0.460, 0.005);
    EXPECT_NEAR(state->dataThermalComforts->ThermalComfortData(1).FangerPPD, 9.3, 0.1);

    zoneHB1.ZTAVComf = 25.0;
    zoneHB1.MRT = 26.0;
    state->dataViewFactor->EnclRadInfo(1).MRT = 26.0;
    zoneHB1.airHumRatAvgComf = 0.00629; // 0.002 to 0.006

    CalcThermalComfortFanger(*state);

    EXPECT_NEAR(state->dataThermalComforts->ThermalComfortData(1).FangerPMV, -1.201, 0.005);
    EXPECT_NEAR(state->dataThermalComforts->ThermalComfortData(1).FangerPPD, 35.1, 0.1);
}

TEST_F(EnergyPlusFixture, ThermalComfort_CalcSurfaceWeightedMRT)
{

    int SurfNum(1);
    Real64 RadTemp;

    state->dataThermalComforts->AngleFactorList.allocate(1);
    state->dataSurface->TotSurfaces = 3;
    state->dataGlobal->NumOfZones = 1;
    state->dataHeatBalSurf->SurfTempInTmp.allocate(state->dataSurface->TotSurfaces);
    state->dataHeatBalSurf->SurfAbsThermalInt.allocate(state->dataSurface->TotSurfaces);
    state->dataSurface->Surface.allocate(state->dataSurface->TotSurfaces);
    state->dataConstruction->Construct.allocate(state->dataSurface->TotSurfaces);
    state->dataHeatBal->Zone.allocate(1);
    state->dataHeatBal->space.allocate(1);

    state->dataSurface->Surface(1).Area = 20.0;
    state->dataSurface->Surface(2).Area = 15.0;
    state->dataSurface->Surface(3).Area = 10.0;
    state->dataSurface->Surface(1).HeatTransSurf = true;
    state->dataSurface->Surface(2).HeatTransSurf = true;
    state->dataSurface->Surface(3).HeatTransSurf = true;
    state->dataSurface->Surface(1).Construction = 1;
    state->dataSurface->Surface(2).Construction = 2;
    state->dataSurface->Surface(3).Construction = 3;
    state->dataConstruction->Construct(1).InsideAbsorpThermal = 1.0;
    state->dataConstruction->Construct(2).InsideAbsorpThermal = 0.9;
    state->dataConstruction->Construct(3).InsideAbsorpThermal = 0.8;
    state->dataSurface->Surface(1).Zone = 1;
    state->dataSurface->Surface(2).Zone = 1;
    state->dataSurface->Surface(3).Zone = 1;
    state->dataHeatBal->Zone(1).spaceIndexes.emplace_back(1);
    state->dataHeatBal->space(1).HTSurfaceFirst = 1;
    state->dataHeatBal->space(1).HTSurfaceLast = 3;
    state->dataHeatBalSurf->SurfTempInTmp(1) = 20.0;
    state->dataHeatBalSurf->SurfTempInTmp(2) = 15.0;
    state->dataHeatBalSurf->SurfTempInTmp(3) = 10.0;
    state->dataHeatBalSurf->SurfAbsThermalInt(1) = 1.0;
    state->dataHeatBalSurf->SurfAbsThermalInt(2) = 0.9;
    state->dataHeatBalSurf->SurfAbsThermalInt(3) = 0.8;

    state->dataViewFactor->EnclRadInfo.allocate(state->dataGlobal->NumOfZones);
    state->dataViewFactor->EnclRadInfo(1).SurfacePtr.dimension(3);
    state->dataViewFactor->EnclRadInfo(1).SurfacePtr = {1, 2, 3};
    state->dataSurface->Surface(1).RadEnclIndex = 1;
    state->dataSurface->Surface(2).RadEnclIndex = 1;
    state->dataSurface->Surface(3).RadEnclIndex = 1;

    SurfNum = 1;
    state->dataThermalComforts->clear_state();
    RadTemp = CalcSurfaceWeightedMRT(*state, SurfNum);
    EXPECT_NEAR(RadTemp, 16.6, 0.1);

    SurfNum = 2;
    state->dataThermalComforts->clear_state();
    RadTemp = CalcSurfaceWeightedMRT(*state, SurfNum);
    EXPECT_NEAR(RadTemp, 16.1, 0.1);

    SurfNum = 3;
    state->dataThermalComforts->clear_state();
    RadTemp = CalcSurfaceWeightedMRT(*state, SurfNum);
    EXPECT_NEAR(RadTemp, 14.0, 0.1);

    // set AverageWithSurface to false for Kiva surfaces
    SurfNum = 1;
    state->dataThermalComforts->clear_state();
    RadTemp = CalcSurfaceWeightedMRT(*state, SurfNum, false);
    EXPECT_NEAR(RadTemp, 13.1, 0.1);

    SurfNum = 2;
    state->dataThermalComforts->clear_state();
    RadTemp = CalcSurfaceWeightedMRT(*state, SurfNum, false);
    EXPECT_NEAR(RadTemp, 17.1, 0.1);

    SurfNum = 3;
    state->dataThermalComforts->clear_state();
    RadTemp = CalcSurfaceWeightedMRT(*state, SurfNum, false);
    EXPECT_NEAR(RadTemp, 18.0, 0.1);
}

TEST_F(EnergyPlusFixture, ThermalComfort_CalcAngleFactorMRT)
{

    Real64 RadTemp;

    state->dataThermalComforts->AngleFactorList.allocate(1);
    state->dataThermalComforts->AngleFactorList(1).TotAngleFacSurfaces = 3;
    state->dataThermalComforts->AngleFactorList(1).SurfacePtr.allocate(state->dataThermalComforts->AngleFactorList(1).TotAngleFacSurfaces);
    state->dataThermalComforts->AngleFactorList(1).AngleFactor.allocate(state->dataThermalComforts->AngleFactorList(1).TotAngleFacSurfaces);

    state->dataThermalComforts->AngleFactorList(1).SurfacePtr(1) = 1;
    state->dataThermalComforts->AngleFactorList(1).SurfacePtr(2) = 2;
    state->dataThermalComforts->AngleFactorList(1).SurfacePtr(3) = 3;
    state->dataThermalComforts->AngleFactorList(1).AngleFactor(1) = 0.5;
    state->dataThermalComforts->AngleFactorList(1).AngleFactor(2) = 0.3;
    state->dataThermalComforts->AngleFactorList(1).AngleFactor(3) = 0.2;

    state->dataSurface->TotSurfaces = state->dataThermalComforts->AngleFactorList(1).TotAngleFacSurfaces;
    state->dataHeatBalSurf->SurfTempInTmp.allocate(state->dataSurface->TotSurfaces);
    state->dataHeatBalSurf->SurfAbsThermalInt.allocate(state->dataSurface->TotSurfaces);
    state->dataSurface->Surface.deallocate();
    state->dataConstruction->Construct.deallocate();
    state->dataSurface->Surface.allocate(state->dataSurface->TotSurfaces);
    state->dataConstruction->Construct.allocate(state->dataSurface->TotSurfaces);

    state->dataHeatBalSurf->SurfTempInTmp(1) = 20.0;
    state->dataHeatBalSurf->SurfTempInTmp(2) = 15.0;
    state->dataHeatBalSurf->SurfTempInTmp(3) = 10.0;
    state->dataHeatBalSurf->SurfAbsThermalInt(1) = 1.0;
    state->dataHeatBalSurf->SurfAbsThermalInt(2) = 0.9;
    state->dataHeatBalSurf->SurfAbsThermalInt(3) = 0.8;
    state->dataSurface->Surface(1).Construction = 1;
    state->dataSurface->Surface(2).Construction = 2;
    state->dataSurface->Surface(3).Construction = 3;
    state->dataConstruction->Construct(1).InsideAbsorpThermal = 1.0;
    state->dataConstruction->Construct(2).InsideAbsorpThermal = 0.9;
    state->dataConstruction->Construct(3).InsideAbsorpThermal = 0.8;

    RadTemp = CalcAngleFactorMRT(*state, 1);
    EXPECT_NEAR(RadTemp, 16.9, 0.1);
}

TEST_F(EnergyPlusFixture, ThermalComfort_CalcThermalComfortAdaptiveASH55Test)
{
    // 5381
    state->dataThermalComforts->useEpwData = true;

    state->dataThermalComforts->DailyAveOutTemp(1) = 8.704166667;
    state->dataThermalComforts->DailyAveOutTemp(2) = 9.895833333;
    state->dataThermalComforts->DailyAveOutTemp(3) = 12.2;
    state->dataThermalComforts->DailyAveOutTemp(4) = 8.445833333;
    state->dataThermalComforts->DailyAveOutTemp(5) = 7.8;
    state->dataThermalComforts->DailyAveOutTemp(6) = 7.158333333;
    state->dataThermalComforts->DailyAveOutTemp(7) = 8.0125;
    state->dataThermalComforts->DailyAveOutTemp(8) = 8.279166667;
    state->dataThermalComforts->DailyAveOutTemp(9) = 8.166666667;
    state->dataThermalComforts->DailyAveOutTemp(10) = 7.141666667;
    state->dataThermalComforts->DailyAveOutTemp(11) = 7.433333333;
    state->dataThermalComforts->DailyAveOutTemp(12) = 9.0625;
    state->dataThermalComforts->DailyAveOutTemp(13) = 9.741666667;
    state->dataThermalComforts->DailyAveOutTemp(14) = 9.545833333;
    state->dataThermalComforts->DailyAveOutTemp(15) = 11.43333333;
    state->dataThermalComforts->DailyAveOutTemp(16) = 12.375;
    state->dataThermalComforts->DailyAveOutTemp(17) = 12.59583333;
    state->dataThermalComforts->DailyAveOutTemp(18) = 12.6625;
    state->dataThermalComforts->DailyAveOutTemp(19) = 13.50833333;
    state->dataThermalComforts->DailyAveOutTemp(20) = 12.99583333;
    state->dataThermalComforts->DailyAveOutTemp(21) = 11.58333333;
    state->dataThermalComforts->DailyAveOutTemp(22) = 11.72083333;
    state->dataThermalComforts->DailyAveOutTemp(23) = 9.1875;
    state->dataThermalComforts->DailyAveOutTemp(24) = 6.8;
    state->dataThermalComforts->DailyAveOutTemp(25) = 9.391666667;
    state->dataThermalComforts->DailyAveOutTemp(26) = 8.1125;
    state->dataThermalComforts->DailyAveOutTemp(27) = 8.4;
    state->dataThermalComforts->DailyAveOutTemp(28) = 8.475;
    state->dataThermalComforts->DailyAveOutTemp(29) = 7.941666667;
    state->dataThermalComforts->DailyAveOutTemp(30) = 9.316666667;

    state->dataGlobal->BeginDayFlag = true;

    CalcThermalComfortAdaptiveASH55(*state, false);
    EXPECT_NEAR(state->dataThermalComforts->runningAverageASH, 9.29236111, 0.001);
    state->dataThermalComforts->useEpwData = false;
    state->dataGlobal->BeginDayFlag = false;
}

TEST_F(EnergyPlusFixture, ThermalComfort_CalcIfSetPointMetWithCutoutTest)
{
    state->dataGlobal->NumOfZones = 1;
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand.allocate(state->dataGlobal->NumOfZones);
    state->dataThermalComforts->ThermalComfortSetPoint.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBalFanSys->TempControlType.allocate(1);
    state->dataRoomAir->AirModel.allocate(state->dataGlobal->NumOfZones);
    state->dataRoomAir->AirModel(1).AirModel = RoomAir::RoomAirModel::Mixing;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBalFanSys->zoneTstatSetpts.allocate(state->dataGlobal->NumOfZones);
    state->dataThermalComforts->ThermalComfortInASH55.allocate(state->dataGlobal->NumOfZones);
    state->dataThermalComforts->ThermalComfortInASH55(1).ZoneIsOccupied = true;
    state->dataGlobal->TimeStepZone = 0.25;
    state->dataHeatBal->Zone.allocate(state->dataGlobal->NumOfZones);
    state->dataZoneTempPredictorCorrector->NumOnOffCtrZone = 1;

    state->dataHeatBalFanSys->TempControlType(1) = HVAC::SetptType::DualHeatCool;

    // heating
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(1).ZTAV = 21.1;           // 70F
    state->dataHeatBalFanSys->zoneTstatSetpts(1).setptLo = 22.2;                     // 72F
    state->dataHeatBalFanSys->zoneTstatSetpts(1).setptLoAver = 22.2;                 // 72F
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).TotalOutputRequired = 500.0; // must be greater than zero
    CalcIfSetPointMet(*state);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeating);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeatingOccupied);
    EXPECT_EQ(0., state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCooling);
    EXPECT_EQ(0., state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCoolingOccupied);

    // cooling
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(1).ZTAV = 25.0;            // 77F
    state->dataHeatBalFanSys->zoneTstatSetpts(1).setptHi = 23.9;                      // 75F
    state->dataHeatBalFanSys->zoneTstatSetpts(1).setptHiAver = 23.9;                  // 75F
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).TotalOutputRequired = -500.0; // must be less than zero
    CalcIfSetPointMet(*state);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeating);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeatingOccupied);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCooling);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCoolingOccupied);

    // no cooling or heating
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(1).ZTAV = 23.0;         // 73F
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).TotalOutputRequired = 0.0; // must be zero
    CalcIfSetPointMet(*state);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeating);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeatingOccupied);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCooling);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCoolingOccupied);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).totalNotMetHeating);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).totalNotMetHeatingOccupied);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).totalNotMetCooling);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).totalNotMetCoolingOccupied);
}

TEST_F(EnergyPlusFixture, ThermalComfort_CalcThermalComfortASH55)
{
    state->init_state(*state);

    // Set the data for the test
    state->dataHeatBal->TotPeople = 1;
    state->dataHeatBal->People.allocate(state->dataHeatBal->TotPeople);
    state->dataThermalComforts->ThermalComfortData.allocate(state->dataHeatBal->TotPeople);
    state->dataGlobal->NumOfZones = 1;
    state->dataHeatBal->Zone.allocate(state->dataGlobal->NumOfZones);
    state->dataRoomAir->IsZoneDispVent3Node.allocate(state->dataGlobal->NumOfZones);
    state->dataRoomAir->IsZoneUFAD.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBalFanSys->ZoneQdotRadHVACToPerson.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBalFanSys->ZoneQHTRadSysToPerson.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBalFanSys->ZoneQCoolingPanelToPerson.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBalFanSys->ZoneQHWBaseboardToPerson.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBalFanSys->ZoneQSteamBaseboardToPerson.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBalFanSys->ZoneQElecBaseboardToPerson.allocate(state->dataGlobal->NumOfZones);

    state->dataHeatBal->People(1).ZonePtr = 1;
    state->dataHeatBal->People(1).sched = Sched::GetScheduleAlwaysOn(*state);
    state->dataHeatBal->People(1).NumberOfPeople = 5.0;
    state->dataHeatBal->People(1).NomMinNumberPeople = 5.0;
    state->dataHeatBal->People(1).NomMaxNumberPeople = 5.0;
    state->dataHeatBal->Zone(state->dataHeatBal->People(1).ZonePtr).TotOccupants = state->dataHeatBal->People(1).NumberOfPeople;
    state->dataHeatBal->People(1).FractionRadiant = 0.3;
    state->dataHeatBal->People(1).FractionConvected = 1.0 - state->dataHeatBal->People(1).FractionRadiant;
    state->dataHeatBal->People(1).UserSpecSensFrac = Constant::AutoCalculate;
    state->dataHeatBal->People(1).CO2RateFactor = 3.82e-8;
    state->dataHeatBal->People(1).Show55Warning = true;
    state->dataHeatBal->People(1).Pierce = true;
    state->dataHeatBal->People(1).MRTCalcType = DataHeatBalance::CalcMRT::EnclosureAveraged;
    state->dataHeatBal->People(1).spaceIndex = 1;
    state->dataHeatBal->space.allocate(1);
    state->dataHeatBal->space(1).radiantEnclosureNum = 1;
    state->dataViewFactor->EnclRadInfo.allocate(1);
    state->dataHeatBal->People(1).workEffSched = Sched::GetScheduleAlwaysOff(*state);
    state->dataHeatBal->People(1).clothingType = ClothingType::InsulationSchedule;

    state->dataRoomAir->IsZoneDispVent3Node(1) = state->dataRoomAir->IsZoneUFAD(1) = false;
    state->dataHeatBalFanSys->ZoneQHTRadSysToPerson(1) = 0.0;
    state->dataHeatBalFanSys->ZoneQCoolingPanelToPerson(1) = 0.0;
    state->dataHeatBalFanSys->ZoneQHWBaseboardToPerson(1) = 0.0;
    state->dataHeatBalFanSys->ZoneQSteamBaseboardToPerson(1) = 0.0;
    state->dataHeatBalFanSys->ZoneQElecBaseboardToPerson(1) = 0.0;
    Real64 BodySurfaceArea = 1.8258;
    state->dataEnvrn->OutBaroPress = 101325.;
    Real64 WorkEff = 0.0;
    auto *activitySched = state->dataHeatBal->People(1).activityLevelSched = Sched::AddScheduleConstant(*state, "ACTIVITY");
    auto *clothingSched = state->dataHeatBal->People(1).clothingSched = Sched::AddScheduleConstant(*state, "CLOTHING");
    auto *airVeloSched = state->dataHeatBal->People(1).airVelocitySched = Sched::AddScheduleConstant(*state, "AIR VELO");
    auto *ankleAirVeloSched = state->dataHeatBal->People(1).ankleAirVelocitySched = Sched::AddScheduleConstant(*state, "ANKLE AIR VELO");

    // Part 1: Test SET calculations.
    // Reference: ANSI/ASHRAE Standard 55-2017 Appendix D - Table D3 Validation Table for SET Computer Model
    const std::vector<double> TAir = {25, 0, 40, 25, 25, 25, 25, 25};
    const std::vector<double> RH = {0.5, 0.5, 0.5, 0.9, 0.5, 0.5, 0.5, 0.5};
    const std::vector<double> Vel = {0.15, 0.15, 0.15, 0.15, 3, 0.15, 0.15, 0.15};
    const std::vector<double> TRad = {25, 25, 25, 25, 25, 40, 25, 25};
    const std::vector<double> MET = {1, 1, 1, 1, 1, 1, 2, 1};
    const std::vector<double> Clo = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 2};
    const std::vector<double> SET = {23.8, 12.3, 34.3, 24.9, 18.8, 31.8, 29.7, 32.5};

    for (unsigned i = 0; i < TAir.size(); i++) {
        Real64 SETResult = CalcStandardEffectiveTemp(*state, TAir[i], TRad[i], RH[i], Vel[i], MET[i], Clo[i], WorkEff);
        EXPECT_NEAR(SETResult, SET[i], 0.1);
    }

    // Part 2: Test cooling effect
    state->dataHeatBal->People(1).CoolingEffectASH55 = true;
    Real64 AirTemp = 25.0;
    Real64 RadTemp = 25.0;
    Real64 RelHum = 0.5;
    Real64 ActMet = 1;
    Real64 CloUnit = 0.5;

    state->dataZoneTempPredictorCorrector->zoneHeatBalance.allocate(state->dataGlobal->NumOfZones);
    auto &zoneHB1 = state->dataZoneTempPredictorCorrector->zoneHeatBalance(1);
    zoneHB1.ZTAVComf = AirTemp;
    zoneHB1.MRT = RadTemp;
    state->dataViewFactor->EnclRadInfo(1).MRT = RadTemp;
    zoneHB1.airHumRatAvgComf = Psychrometrics::PsyWFnTdbRhPb(*state, zoneHB1.ZTAVComf, RelHum, state->dataEnvrn->OutBaroPress);
    activitySched->currentVal = ActMet * BodySurfaceArea * ThermalComfort::ActLevelConv;
    clothingSched->currentVal = CloUnit;

    // Test 1 - Air velocity = 0.15 m/s.
    Real64 AirVel = 0.15;
    airVeloSched->currentVal = AirVel;
    CalcThermalComfortCoolingEffectASH(*state);
    Real64 CoolingEffect = state->dataThermalComforts->ThermalComfortData(1).CoolingEffectASH55;
    Real64 StillAirVel = 0.1;
    Real64 RelAirVel = CalcRelativeAirVelocity(AirVel, ActMet);
    Real64 InitialSET = CalcStandardEffectiveTemp(*state, AirTemp, RadTemp, RelHum, RelAirVel, ActMet, CloUnit, WorkEff);
    Real64 CoolingEffectSET =
        CalcStandardEffectiveTemp(*state, AirTemp - CoolingEffect, RadTemp - CoolingEffect, RelHum, StillAirVel, ActMet, CloUnit, WorkEff);
    // TODO: This is being calculated about 0.2 degrees different on x86* and ARM, so I loosened up the tolerance, but why??
    EXPECT_NEAR(CoolingEffectSET, InitialSET, 0.25);

    // Test 2 - Air velocity = 1 m/s.
    AirVel = 1;
    airVeloSched->currentVal = AirVel;
    CalcThermalComfortCoolingEffectASH(*state);
    CoolingEffect = state->dataThermalComforts->ThermalComfortData(1).CoolingEffectASH55;
    CoolingEffectSET =
        CalcStandardEffectiveTemp(*state, AirTemp - CoolingEffect, RadTemp - CoolingEffect, RelHum, StillAirVel, ActMet, CloUnit, WorkEff);

    RelAirVel = CalcRelativeAirVelocity(AirVel, ActMet);
    InitialSET = CalcStandardEffectiveTemp(*state, AirTemp, RadTemp, RelHum, RelAirVel, ActMet, CloUnit, WorkEff);
    // TODO: This is being calculated about 0.2 degrees different on x86* and ARM, so I loosened up the tolerance, but why??
    EXPECT_NEAR(CoolingEffectSET, InitialSET, 0.25);

    // Part 3: Test ankle draft PPD.
    state->dataHeatBal->People(1).AnkleDraftASH55 = true;
    AirVel = 0.15;
    Real64 AnkleAirVel = 0.3;
    airVeloSched->currentVal = AirVel;
    ankleAirVeloSched->currentVal = AnkleAirVel;
    CalcThermalComfortAnkleDraftASH(*state);
    // TODO: This is being calculated about 0.2 degrees different on x86* and ARM, so I loosened up the tolerance, but why??
    EXPECT_NEAR(state->dataThermalComforts->ThermalComfortData(1).AnkleDraftPPDASH55, 25.0, 0.25);
}

TEST_F(EnergyPlusFixture, ThermalComfort_CalcThermalComfortFanger_Correct_TimeStep)
{

    std::string const idf_objects = delimited_string({
        "People,",
        "  Space People,            !- Name",
        "  Space,                   !- Zone or ZoneList or Space or SpaceList Name",
        "  PeopleSchedule,          !- Number of People Schedule Name",
        "  People,                  !- Number of People Calculation Method",
        "  5.0,                     !- Number of People",
        "  ,                        !- People per Floor Area {person/m2}",
        "  ,                        !- Floor Area per Person {m2/person}",
        "  0.3,                     !- Fraction Radiant",
        "  Autocalculate,           !- Sensible Heat Fraction",
        "  Activity Schedule,       !- Activity Level Schedule Name",
        "  ,                        !- Carbon Dioxide Generation Rate {m3/s-W}",
        "  Yes,                     !- Enable ASHRAE 55 Comfort Warnings",
        "  EnclosureAveraged,       !- Mean Radiant Temperature Calculation Type",
        "  ,                        !- Surface Name/Angle Factor List Name",
        "  Work efficiency,         !- Work Efficiency Schedule Name",
        "  ClothingInsulationSchedule,  !- Clothing Insulation Calculation Method",
        "  ,                        !- Clothing Insulation Calculation Method Schedule Name",
        "  Clothing Schedule,       !- Clothing Insulation Schedule Name",
        "  AirVelocitySchedule,     !- Air Velocity Schedule Name",
        "  Fanger;                  !- Thermal Comfort Model 1 Type",

        "Schedule:Compact,",
        "  PeopleSchedule,          !- Name",
        "  Any Number,              !- Schedule Type Limits Name",
        "  Through: 12/30,          !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  1.0,                     !- Field",
        "  Through: 12/31,          !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  0.3;                     !- Field",

        "Schedule:Compact,",
        "  Activity Schedule,       !- Name",
        "  Any Number,              !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  70;                      !- Field",

        "Schedule:Compact,",
        "  Clothing Schedule,       !- Name",
        "  Any Number,              !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  1.0;                     !- Field",

        "Schedule:Compact,",
        "  AirVelocitySchedule,     !- Name",
        "  Any Number,              !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  0.0;                     !- Field",

        "Schedule:Compact,",
        "  Work efficiency,         !- Name",
        "  Any Number,              !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  0.0;                     !- Field",

        "Output:Diagnostics,",
        "  DisplayExtraWarnings;    !- Key",

        "Timestep,4;",

        "Building,",
        "  AirloopHVAC_VentilationRateProcedure,  !- Name",
        "  0.0,                     !- North Axis {deg}",
        "  Suburbs,                 !- Terrain",
        "  0.04,                    !- Loads Convergence Tolerance Value {W}",
        "  0.4,                     !- Temperature Convergence Tolerance Value {deltaC}",
        "  FullExterior,            !- Solar Distribution",
        "  25,                      !- Maximum Number of Warmup Days",
        "  6;                       !- Minimum Number of Warmup Days",

        "SimulationControl,",
        "  No,                      !- Do Zone Sizing Calculation",
        "  No,                      !- Do System Sizing Calculation",
        "  No,                      !- Do Plant Sizing Calculation",
        "  Yes,                     !- Run Simulation for Sizing Periods",
        "  No,                      !- Run Simulation for Weather File Run Periods",
        "  ,                        !- Do HVAC Sizing Simulation for Sizing Periods",
        "  ;                        !- Maximum Number of HVAC Sizing Simulation Passes",

        "ScheduleTypeLimits,",
        "  Any Number;              !- Name",

        "Site:Location,",
        "  Miami Intl Ap FL USA TMY3 WMO=722020E,  !- Name",
        "  25.82,                   !- Latitude {deg}",
        "  -80.3,                   !- Longitude {deg}",
        "  -5.0,                    !- Time Zone {hr}",
        "  11;                      !- Elevation {m}",

        "SizingPeriod:DesignDay,",
        "  Miami Intl Ap Ann Clg .4% Condns DB/MCWB,  !- Name",
        "  7,                       !- Month",
        "  21,                      !- Day of Month",
        "  SummerDesignDay,         !- Day Type",
        "  31.7,                    !- Maximum Dry-Bulb Temperature {C}",
        "  10.0,                    !- Daily Dry-Bulb Temperature Range {deltaC}",
        "  ,                        !- Dry-Bulb Temperature Range Modifier Type",
        "  ,                        !- Dry-Bulb Temperature Range Modifier Day Schedule Name",
        "  WetBulb,                 !- Humidity Condition Type",
        "  22.7,                    !- Wetbulb or DewPoint at Maximum Dry-Bulb {C}",
        "  ,                        !- Humidity Condition Day Schedule Name",
        "  ,                        !- Humidity Ratio at Maximum Dry-Bulb {kgWater/kgDryAir}",
        "  ,                        !- Enthalpy at Maximum Dry-Bulb {J/kg}",
        "  ,                        !- Daily Wet-Bulb Temperature Range {deltaC}",
        "  101217.0,                !- Barometric Pressure {Pa}",
        "  3.8,                     !- Wind Speed {m/s}",
        "  340,                     !- Wind Direction {deg}",
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
        "  Miami Intl Ap Ann Htg 99.6% Condns DB,  !- Name",
        "  1,                       !- Month",
        "  21,                      !- Day of Month",
        "  WinterDesignDay,         !- Day Type",
        "  8.7,                     !- Maximum Dry-Bulb Temperature {C}",
        "  0.0,                     !- Daily Dry-Bulb Temperature Range {deltaC}",
        "  ,                        !- Dry-Bulb Temperature Range Modifier Type",
        "  ,                        !- Dry-Bulb Temperature Range Modifier Day Schedule Name",
        "  WetBulb,                 !- Humidity Condition Type",
        "  8.7,                     !- Wetbulb or DewPoint at Maximum Dry-Bulb {C}",
        "  ,                        !- Humidity Condition Day Schedule Name",
        "  ,                        !- Humidity Ratio at Maximum Dry-Bulb {kgWater/kgDryAir}",
        "  ,                        !- Enthalpy at Maximum Dry-Bulb {J/kg}",
        "  ,                        !- Daily Wet-Bulb Temperature Range {deltaC}",
        "  101217.0,                !- Barometric Pressure {Pa}",
        "  3.8,                     !- Wind Speed {m/s}",
        "  340,                     !- Wind Direction {deg}",
        "  No,                      !- Rain Indicator",
        "  No,                      !- Snow Indicator",
        "  No,                      !- Daylight Saving Time Indicator",
        "  ASHRAEClearSky,          !- Solar Model Indicator",
        "  ,                        !- Beam Solar Day Schedule Name",
        "  ,                        !- Diffuse Solar Day Schedule Name",
        "  ,                        !- ASHRAE Clear Sky Optical Depth for Beam Irradiance (taub) {dimensionless}",
        "  ,                        !- ASHRAE Clear Sky Optical Depth for Diffuse Irradiance (taud) {dimensionless}",
        "  0.0;                     !- Sky Clearness",

        "Zone,",
        "  Space,                   !- Name",
        "  0.0,                     !- Direction of Relative North {deg}",
        "  0.0,                     !- X Origin {m}",
        "  0.0,                     !- Y Origin {m}",
        "  0.0,                     !- Z Origin {m}",
        "  1,                       !- Type",
        "  1,                       !- Multiplier",
        "  2.4,                     !- Ceiling Height {m}",
        "  ,                        !- Volume {m3}",
        "  Autocalculate,           !- Floor Area {m2}",
        "  ,                        !- Zone Inside Convection Algorithm",
        "  ,                        !- Zone Outside Convection Algorithm",
        "  Yes;                     !- Part of Total Floor Area",

        "ZoneGroup,",
        "  Zone Group,              !- Name",
        "  Zone List,               !- Zone List Name",
        "  10;                      !- Zone List Multiplier",

        "ZoneList,",
        "  Zone List,               !- Name",
        "  Spacex10;                !- Zone Name",

        "Zone,",
        "  Spacex10,                !- Name",
        "  0.0,                     !- Direction of Relative North {deg}",
        "  0.0,                     !- X Origin {m}",
        "  0.0,                     !- Y Origin {m}",
        "  0.0,                     !- Z Origin {m}",
        "  1,                       !- Type",
        "  1,                       !- Multiplier",
        "  2.4,                     !- Ceiling Height {m}",
        "  ,                        !- Volume {m3}",
        "  Autocalculate,           !- Floor Area {m2}",
        "  ,                        !- Zone Inside Convection Algorithm",
        "  ,                        !- Zone Outside Convection Algorithm",
        "  Yes;                     !- Part of Total Floor Area",

        "People,",
        "  Spacex10 People,         !- Name",
        "  Spacex10,                !- Zone or ZoneList or Space or SpaceList Name",
        "  OnSched,                 !- Number of People Schedule Name",
        "  People,                  !- Number of People Calculation Method",
        "  11,                      !- Number of People",
        "  ,                        !- People per Floor Area {person/m2}",
        "  ,                        !- Floor Area per Person {m2/person}",
        "  0.3,                     !- Fraction Radiant",
        "  Autocalculate,           !- Sensible Heat Fraction",
        "  Activity Schedule;       !- Activity Level Schedule Name",

        "Lights,",
        "  Space Lights,            !- Name",
        "  Space,                   !- Zone or ZoneList or Space or SpaceList Name",
        "  OnSched,                 !- Schedule Name",
        "  Watts/Area,              !- Design Level Calculation Method",
        "  ,                        !- Lighting Level {W}",
        "  10.0,                    !- Watts per Floor Area {W/m2}",
        "  ,                        !- Watts per Person {W/person}",
        "  0.0,                     !- Return Air Fraction",
        "  0.59,                    !- Fraction Radiant",
        "  0.2,                     !- Fraction Visible",
        "  0,                       !- Fraction Replaceable",
        "  GeneralLights;           !- End-Use Subcategory",

        "Lights,",
        "  Space Lights x10,        !- Name",
        "  Spacex10,                !- Zone or ZoneList or Space or SpaceList Name",
        "  OnSched,                 !- Schedule Name",
        "  Watts/Area,              !- Design Level Calculation Method",
        "  ,                        !- Lighting Level {W}",
        "  10.0,                    !- Watts per Floor Area {W/m2}",
        "  ,                        !- Watts per Person {W/person}",
        "  0.0,                     !- Return Air Fraction",
        "  0.59,                    !- Fraction Radiant",
        "  0.2,                     !- Fraction Visible",
        "  0,                       !- Fraction Replaceable",
        "  GeneralLights;           !- End-Use Subcategory",

        "ElectricEquipment,",
        "  Space ElecEq,            !- Name",
        "  Space,                   !- Zone or ZoneList or Space or SpaceList Name",
        "  OnSched,                 !- Schedule Name",
        "  Watts/Area,              !- Design Level Calculation Method",
        "  ,                        !- Design Level {W}",
        "  20.0,                    !- Watts per Floor Area {W/m2}",
        "  ,                        !- Watts per Person {W/person}",
        "  0.1,                     !- Fraction Latent",
        "  0.3,                     !- Fraction Radiant",
        "  0.1;                     !- Fraction Lost",

        "ElectricEquipment,",
        "  Space ElecEq x10,        !- Name",
        "  Spacex10,                !- Zone or ZoneList or Space or SpaceList Name",
        "  OnSched,                 !- Schedule Name",
        "  Watts/Area,              !- Design Level Calculation Method",
        "  ,                        !- Design Level {W}",
        "  20.0,                    !- Watts per Floor Area {W/m2}",
        "  ,                        !- Watts per Person {W/person}",
        "  0.1,                     !- Fraction Latent",
        "  0.3,                     !- Fraction Radiant",
        "  0.1;                     !- Fraction Lost",

        "Schedule:Compact,",
        "  OnSched,                 !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  1.0;                     !- Field",

        "ScheduleTypeLimits,",
        "  Fraction,                !- Name",
        "  0.0,                     !- Lower Limit Value {BasedOnField A3}",
        "  1.0,                     !- Upper Limit Value {BasedOnField A3}",
        "  Continuous;              !- Numeric Type",

        "Construction,",
        "  INT-WALL-1,              !- Name",
        "  GP02,                    !- Outside Layer",
        "  AL21,                    !- Layer 2",
        "  GP02;                    !- Layer 3",

        "Material,",
        "  GP02,                    !- Name",
        "  MediumSmooth,            !- Roughness",
        "  0.015900001,             !- Thickness {m}",
        "  0.16,                    !- Conductivity {W/m-K}",
        "  801.0,                   !- Density {kg/m3}",
        "  837.0,                   !- Specific Heat {J/kg-K}",
        "  0.9,                     !- Thermal Absorptance",
        "  0.75,                    !- Solar Absorptance",
        "  0.75;                    !- Visible Absorptance",

        "Material:AirGap,",
        "  AL21,                    !- Name",
        "  0.157;                   !- Thermal Resistance {m2-K/W}",

        "Construction,",
        "  FLOOR-SLAB-1,            !- Name",
        "  CC03,                    !- Outside Layer",
        "  CP01;                    !- Layer 2",

        "Material,",
        "  CC03,                    !- Name",
        "  MediumRough,             !- Roughness",
        "  0.1016,                  !- Thickness {m}",
        "  1.31,                    !- Conductivity {W/m-K}",
        "  2243.0,                  !- Density {kg/m3}",
        "  837.0,                   !- Specific Heat {J/kg-K}",
        "  0.9,                     !- Thermal Absorptance",
        "  0.65,                    !- Solar Absorptance",
        "  0.65;                    !- Visible Absorptance",

        "Material:NoMass,",
        "  CP01,                    !- Name",
        "  Rough,                   !- Roughness",
        "  0.367,                   !- Thermal Resistance {m2-K/W}",
        "  0.9,                     !- Thermal Absorptance",
        "  0.75,                    !- Solar Absorptance",
        "  0.75;                    !- Visible Absorptance",

        "Construction,",
        "  CLNG-1,                  !- Name",
        "  MAT-CLNG-1;              !- Outside Layer",

        "Material:NoMass,",
        "  MAT-CLNG-1,              !- Name",
        "  Rough,                   !- Roughness",
        "  0.65225929,              !- Thermal Resistance {m2-K/W}",
        "  0.65,                    !- Thermal Absorptance",
        "  0.65,                    !- Solar Absorptance",
        "  0.65;                    !- Visible Absorptance",

        "BuildingSurface:Detailed,",
        "  FRONT-1,                 !- Name",
        "  Wall,                    !- Surface Type",
        "  INT-WALL-1,              !- Construction Name",
        "  Space,                   !- Zone Name",
        "  ,                        !- Space Name",
        "  Outdoors,                !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  SunExposed,              !- Sun Exposure",
        "  WindExposed,             !- Wind Exposure",
        "  0.5,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  30.5,                    !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  30.5,                    !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  2.4;                     !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  C1-1,                    !- Name",
        "  Ceiling,                 !- Surface Type",
        "  CLNG-1,                  !- Construction Name",
        "  Space,                   !- Zone Name",
        "  ,                        !- Space Name",
        "  Outdoors,                !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  NoSun,                   !- Sun Exposure",
        "  NoWind,                  !- Wind Exposure",
        "  0.0,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  3.7,                     !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  30.5,                    !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  26.8,                    !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  2.4;                     !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  F1-1,                    !- Name",
        "  Floor,                   !- Surface Type",
        "  FLOOR-SLAB-1,            !- Construction Name",
        "  Space,                   !- Zone Name",
        "  ,                        !- Space Name",
        "  Outdoors,                !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  NoSun,                   !- Sun Exposure",
        "  NoWind,                  !- Wind Exposure",
        "  0.0,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  26.8,                    !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  30.5,                    !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  3.7,                     !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  0.0;                     !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  SB12,                    !- Name",
        "  Wall,                    !- Surface Type",
        "  INT-WALL-1,              !- Construction Name",
        "  Space,                   !- Zone Name",
        "  ,                        !- Space Name",
        "  Adiabatic,               !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  NoSun,                   !- Sun Exposure",
        "  NoWind,                  !- Wind Exposure",
        "  0.0,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  30.5,                    !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  30.5,                    !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  26.8,                    !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  26.8,                    !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  2.4;                     !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  SB14,                    !- Name",
        "  Wall,                    !- Surface Type",
        "  INT-WALL-1,              !- Construction Name",
        "  Space,                   !- Zone Name",
        "  ,                        !- Space Name",
        "  Adiabatic,               !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  NoSun,                   !- Sun Exposure",
        "  NoWind,                  !- Wind Exposure",
        "  0.0,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  3.7,                     !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  3.7,                     !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  2.4;                     !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  SB15,                    !- Name",
        "  Wall,                    !- Surface Type",
        "  INT-WALL-1,              !- Construction Name",
        "  Space,                   !- Zone Name",
        "  ,                        !- Space Name",
        "  Adiabatic,               !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  NoSun,                   !- Sun Exposure",
        "  NoWind,                  !- Wind Exposure",
        "  0.0,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  26.8,                    !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  26.8,                    !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  3.7,                     !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  3.7,                     !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  2.4;                     !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  FRONT-1x10,              !- Name",
        "  Wall,                    !- Surface Type",
        "  INT-WALL-1,              !- Construction Name",
        "  Spacex10,                !- Zone Name",
        "  ,                        !- Space Name",
        "  Outdoors,                !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  SunExposed,              !- Sun Exposure",
        "  WindExposed,             !- Wind Exposure",
        "  0.5,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  30.5,                    !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  30.5,                    !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  2.4;                     !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  C1-1x10,                 !- Name",
        "  Ceiling,                 !- Surface Type",
        "  CLNG-1,                  !- Construction Name",
        "  Spacex10,                !- Zone Name",
        "  ,                        !- Space Name",
        "  Outdoors,                !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  NoSun,                   !- Sun Exposure",
        "  NoWind,                  !- Wind Exposure",
        "  0.0,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  3.7,                     !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  30.5,                    !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  26.8,                    !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  2.4;                     !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  F1-1x10,                 !- Name",
        "  Floor,                   !- Surface Type",
        "  FLOOR-SLAB-1,            !- Construction Name",
        "  Spacex10,                !- Zone Name",
        "  ,                        !- Space Name",
        "  Outdoors,                !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  NoSun,                   !- Sun Exposure",
        "  NoWind,                  !- Wind Exposure",
        "  0.0,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  26.8,                    !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  30.5,                    !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  3.7,                     !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  0.0;                     !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  SB12x10,                 !- Name",
        "  Wall,                    !- Surface Type",
        "  INT-WALL-1,              !- Construction Name",
        "  Spacex10,                !- Zone Name",
        "  ,                        !- Space Name",
        "  Adiabatic,               !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  NoSun,                   !- Sun Exposure",
        "  NoWind,                  !- Wind Exposure",
        "  0.0,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  30.5,                    !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  30.5,                    !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  26.8,                    !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  26.8,                    !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  2.4;                     !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  SB14x10,                 !- Name",
        "  Wall,                    !- Surface Type",
        "  INT-WALL-1,              !- Construction Name",
        "  Spacex10,                !- Zone Name",
        "  ,                        !- Space Name",
        "  Adiabatic,               !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  NoSun,                   !- Sun Exposure",
        "  NoWind,                  !- Wind Exposure",
        "  0.0,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  3.7,                     !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  3.7,                     !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  0.0,                     !- Vertex X-coordinate {m}",
        "  0.0,                     !- Vertex Y-coordinate {m}",
        "  2.4;                     !- Vertex Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  SB15x10,                 !- Name",
        "  Wall,                    !- Surface Type",
        "  INT-WALL-1,              !- Construction Name",
        "  Spacex10,                !- Zone Name",
        "  ,                        !- Space Name",
        "  Adiabatic,               !- Outside Boundary Condition",
        "  ,                        !- Outside Boundary Condition Object",
        "  NoSun,                   !- Sun Exposure",
        "  NoWind,                  !- Wind Exposure",
        "  0.0,                     !- View Factor to Ground",
        "  4,                       !- Number of Vertices",
        "  26.8,                    !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  2.4,                     !- Vertex Z-coordinate {m}",
        "  26.8,                    !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  3.7,                     !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  0.0,                     !- Vertex Z-coordinate {m}",
        "  3.7,                     !- Vertex X-coordinate {m}",
        "  3.7,                     !- Vertex Y-coordinate {m}",
        "  2.4;                     !- Vertex Z-coordinate {m}",

        "Output:Table:SummaryReports,",
        "  AllSummary;              !- Report Name",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    state->dataGlobal->DDOnlySimulation = true;

    ManageSimulation(*state);

    CalcThermalComfortFanger(*state);

    EXPECT_NEAR(state->dataZoneTempPredictorCorrector->zoneHeatBalance(1).ZTAVComf, 14.863733439268286, 0.001);

    EXPECT_NEAR(state->dataZoneTempPredictorCorrector->zoneHeatBalance(1).airHumRatAvgComf, 0.010564839505489259, 0.0001);

    EXPECT_NEAR(state->dataThermalComforts->ThermalComfortData(1).FangerPMV, -5.5851444408083006, 0.001);

    EXPECT_NEAR(state->dataThermalComforts->ThermalComfortData(1).FangerPPD, 100.0, 0.001);
}

TEST_F(EnergyPlusFixture, ThermalComfort_CalcThermalComfortAdaptiveCEN15251)
{
    state->files.inputWeatherFilePath.filePath = configured_source_directory() / "tst/EnergyPlus/unit/Resources/ThermalComfortCEN15251Test.epw";

    // test the initialisation
    state->dataEnvrn->DayOfYear = 1;
    state->dataEnvrn->CurrentYearIsLeapYear = false;
    CalcThermalComfortAdaptiveCEN15251(*state, true, true, 0.0);
    EXPECT_NEAR(state->dataThermalComforts->runningAverageCEN, -1.3671408, 0.01);

    // skip the first day
    Array1D<Real64> HourlyDryBulbTemp = {
        -4.8, -5.4, -5.7, -5.9, -6.1, -6.2, -7.2, -6.7, -6.7, -5.6, -5., -4.4, -5., -3.9, -5., -5., -6.7, -7.8, -9.4, -9.4, -9.4, -9.4, -7.8, -7.2,
    };
    state->dataGlobal->BeginDayFlag = true;
    state->dataGlobal->BeginHourFlag = true;
    for (int i = 1; i <= 24; i++) {
        state->dataEnvrn->OutDryBulbTemp = HourlyDryBulbTemp(i);
        CalcThermalComfortAdaptiveCEN15251(*state, false);
        if (i == 1) {
            state->dataGlobal->BeginDayFlag = false;
        }
    }

    // test the second day
    state->dataEnvrn->DayOfYear += 1;
    state->dataGlobal->BeginDayFlag = true;
    CalcThermalComfortAdaptiveCEN15251(*state, false);
    EXPECT_NEAR(state->dataThermalComforts->runningAverageCEN, -2.3912126, 0.01);
    state->dataGlobal->BeginDayFlag = false;
}

TEST_F(EnergyPlusFixture, ThermalComfort_GetAngleFactorListTest)
{

    std::string const idf_objects = delimited_string({
        "ComfortViewFactorAngles,",
        "  Angle Factor Lost,       !- Name",
        "  InTheZone:Wall1,         !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall2,         !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall3,         !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall4,         !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall5,         !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall6,         !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall7,         !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall8,         !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall9,         !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall10,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall11,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall12,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall13,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall14,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall15,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall16,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall17,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall18,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall19,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall20,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall21,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall22,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall23,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall24,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall25,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall26,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall27,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall28,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall29,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall30,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall31,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall32,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall33,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall34,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall35,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall36,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall37,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall38,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall39,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall40,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall41,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall42,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall43,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall44,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall45,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall46,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall47,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall48,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall49,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall50,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall51,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall52,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall53,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall54,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall55,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall56,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall57,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall58,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall59,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall60,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall61,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall62,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall63,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall64,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall65,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall66,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall67,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall68,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall69,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall70,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall71,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall72,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall73,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall74,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall75,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall76,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall77,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall78,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall79,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall80,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall81,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall82,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall83,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall84,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall85,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall86,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall87,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall88,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall89,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall90,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall91,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall92,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall93,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall94,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall95,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall96,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall97,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall98,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall99,        !- Surface Name",
        "  0.01,                    !- Angle Factor",
        "  InTheZone:Wall100,       !- Surface Name",
        "  0.01;                    !- Angle Factor",
    });

    state->dataSurface->Surface.allocate(100);
    state->dataHeatBal->Zone.allocate(1);
    state->dataHeatBal->Zone(1).Name = "INTHEZONE";
    for (int i = 1; i <= 100; ++i) {
        state->dataSurface->Surface(i).Name = "INTHEZONE:WALL" + std::to_string(i);
        state->dataSurface->Surface(i).Zone = 1;
    }

    ASSERT_TRUE(process_idf(idf_objects));

    GetAngleFactorList(*state);

    ASSERT_EQ(state->dataThermalComforts->AngleFactorList(1).TotAngleFacSurfaces, 100);
}

TEST_F(EnergyPlusFixture, ThermalComfort_CalcSurfaceWeightedMRT_Enclosure_Based)
{
    // Test Issue 9377 fix via PR 9628: Enclosure based MRT calculation

    int SurfNum(1);
    Real64 RadTemp;

    state->dataThermalComforts->AngleFactorList.allocate(1);
    state->dataSurface->TotSurfaces = 12;
    state->dataGlobal->NumOfZones = 2;
    state->dataGlobal->numSpaces = 2;

    state->dataHeatBalSurf->SurfTempInTmp.allocate(state->dataSurface->TotSurfaces);
    state->dataHeatBalSurf->SurfAbsThermalInt.dimension(state->dataSurface->TotSurfaces, 0.9);
    state->dataSurface->Surface.allocate(state->dataSurface->TotSurfaces);
    state->dataConstruction->Construct.allocate(state->dataSurface->TotSurfaces);
    state->dataHeatBal->Zone.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBal->space.allocate(state->dataGlobal->numSpaces);

    state->dataSurface->Surface(1).Area = 10.0;
    state->dataSurface->Surface(2).Area = 10.0;
    state->dataSurface->Surface(3).Area = 10.0;
    state->dataSurface->Surface(4).Area = 10.0;
    state->dataSurface->Surface(5).Area = 10.0;
    state->dataSurface->Surface(6).Area = 10.0;

    state->dataSurface->Surface(7).Area = 10.0;
    state->dataSurface->Surface(8).Area = 10.0;
    state->dataSurface->Surface(9).Area = 10.0;
    state->dataSurface->Surface(10).Area = 10.0;
    state->dataSurface->Surface(11).Area = 10.0;
    state->dataSurface->Surface(12).Area = 10.0;

    state->dataSurface->Surface(1).HeatTransSurf = true;
    state->dataSurface->Surface(2).HeatTransSurf = true;
    state->dataSurface->Surface(3).HeatTransSurf = true;
    state->dataSurface->Surface(4).HeatTransSurf = true;
    state->dataSurface->Surface(5).HeatTransSurf = true;
    state->dataSurface->Surface(6).HeatTransSurf = false;

    state->dataSurface->Surface(7).HeatTransSurf = false;
    state->dataSurface->Surface(8).HeatTransSurf = true;
    state->dataSurface->Surface(9).HeatTransSurf = true;
    state->dataSurface->Surface(10).HeatTransSurf = true;
    state->dataSurface->Surface(11).HeatTransSurf = true;
    state->dataSurface->Surface(12).HeatTransSurf = true;

    state->dataSurface->Surface(1).Construction = 1; // floor
    state->dataSurface->Surface(2).Construction = 2; // roof
    state->dataSurface->Surface(3).Construction = 3; // wall
    state->dataSurface->Surface(4).Construction = 3; // wall
    state->dataSurface->Surface(5).Construction = 3; // wall
    state->dataSurface->Surface(6).Construction = 4; // air boundary

    state->dataSurface->Surface(7).Construction = 4;  // air boundary
    state->dataSurface->Surface(8).Construction = 3;  // wall
    state->dataSurface->Surface(9).Construction = 3;  // wall
    state->dataSurface->Surface(10).Construction = 3; // wall
    state->dataSurface->Surface(11).Construction = 2; // roof
    state->dataSurface->Surface(12).Construction = 1; // floor

    state->dataConstruction->Construct(1).InsideAbsorpThermal = 0.9;
    state->dataConstruction->Construct(2).InsideAbsorpThermal = 0.9;
    state->dataConstruction->Construct(3).InsideAbsorpThermal = 0.9;
    state->dataConstruction->Construct(4).InsideAbsorpThermal = 0.9;

    state->dataSurface->Surface(1).Zone = 1;
    state->dataSurface->Surface(2).Zone = 1;
    state->dataSurface->Surface(3).Zone = 1;
    state->dataSurface->Surface(4).Zone = 1;
    state->dataSurface->Surface(5).Zone = 1;
    state->dataSurface->Surface(6).Zone = 1;
    state->dataSurface->Surface(1).spaceNum = 1;
    state->dataSurface->Surface(2).spaceNum = 1;
    state->dataSurface->Surface(3).spaceNum = 1;
    state->dataSurface->Surface(4).spaceNum = 1;
    state->dataSurface->Surface(5).spaceNum = 1;
    state->dataSurface->Surface(6).spaceNum = 1;

    state->dataSurface->Surface(7).Zone = 2;
    state->dataSurface->Surface(8).Zone = 2;
    state->dataSurface->Surface(9).Zone = 2;
    state->dataSurface->Surface(10).Zone = 2;
    state->dataSurface->Surface(11).Zone = 2;
    state->dataSurface->Surface(12).Zone = 2;
    state->dataSurface->Surface(7).spaceNum = 2;
    state->dataSurface->Surface(8).spaceNum = 2;
    state->dataSurface->Surface(9).spaceNum = 2;
    state->dataSurface->Surface(10).spaceNum = 2;
    state->dataSurface->Surface(11).spaceNum = 2;
    state->dataSurface->Surface(12).spaceNum = 2;

    state->dataHeatBal->Zone(1).spaceIndexes.allocate(1);
    state->dataHeatBal->Zone(1).spaceIndexes[0] = 1;
    state->dataHeatBal->space.allocate(1);
    state->dataHeatBal->space(1).HTSurfaceFirst = 1;
    state->dataHeatBal->space(1).HTSurfaceLast = 6;
    for (int surfNum = 1; surfNum <= 6; ++surfNum) {
        state->dataHeatBalSurf->SurfTempInTmp(surfNum) = 10.0;
    }
    for (int surfNum = 7; surfNum <= 12; ++surfNum) {
        state->dataHeatBalSurf->SurfTempInTmp(surfNum) = 20.0;
    }

    state->dataViewFactor->EnclRadInfo.allocate(1); // (state->dataGlobal->NumOfZones);
    state->dataViewFactor->EnclRadInfo(1).SurfacePtr.dimension(12);
    state->dataViewFactor->EnclRadInfo(1).SurfacePtr = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    state->dataSurface->Surface(1).RadEnclIndex = 1;
    state->dataSurface->Surface(2).RadEnclIndex = 1;
    state->dataSurface->Surface(3).RadEnclIndex = 1;
    state->dataSurface->Surface(4).RadEnclIndex = 1;
    state->dataSurface->Surface(5).RadEnclIndex = 1;
    state->dataSurface->Surface(6).RadEnclIndex = 1;

    state->dataSurface->Surface(7).RadEnclIndex = 1;
    state->dataSurface->Surface(8).RadEnclIndex = 1;
    state->dataSurface->Surface(9).RadEnclIndex = 1;
    state->dataSurface->Surface(10).RadEnclIndex = 1;
    state->dataSurface->Surface(11).RadEnclIndex = 1;
    state->dataSurface->Surface(12).RadEnclIndex = 1;

    SurfNum = 1;
    state->dataThermalComforts->clear_state();
    RadTemp = CalcSurfaceWeightedMRT(*state, SurfNum);
    EXPECT_NEAR(RadTemp, 12.727, 0.1);

    SurfNum = 3;
    state->dataThermalComforts->clear_state();
    RadTemp = CalcSurfaceWeightedMRT(*state, SurfNum);
    EXPECT_NEAR(RadTemp, 12.727, 0.1);

    SurfNum = 5;
    state->dataThermalComforts->clear_state();
    RadTemp = CalcSurfaceWeightedMRT(*state, SurfNum);
    EXPECT_NEAR(RadTemp, 12.727, 0.1);

    SurfNum = 8;
    state->dataThermalComforts->clear_state();
    RadTemp = CalcSurfaceWeightedMRT(*state, SurfNum);
    EXPECT_NEAR(RadTemp, 17.273, 0.1);

    SurfNum = 10;
    state->dataThermalComforts->clear_state();
    RadTemp = CalcSurfaceWeightedMRT(*state, SurfNum);
    EXPECT_NEAR(RadTemp, 17.273, 0.1);

    SurfNum = 12;
    state->dataThermalComforts->clear_state();
    RadTemp = CalcSurfaceWeightedMRT(*state, SurfNum);
    EXPECT_NEAR(RadTemp, 17.273, 0.1);

    // set AverageWithSurface to false for Kiva surfaces
    SurfNum = 1;
    state->dataThermalComforts->clear_state();
    RadTemp = CalcSurfaceWeightedMRT(*state, SurfNum, false);
    EXPECT_NEAR(RadTemp, 15.455, 0.1);

    SurfNum = 2;
    state->dataThermalComforts->clear_state();
    RadTemp = CalcSurfaceWeightedMRT(*state, SurfNum, false);
    EXPECT_NEAR(RadTemp, 15.455, 0.1);

    SurfNum = 3;
    state->dataThermalComforts->clear_state();
    RadTemp = CalcSurfaceWeightedMRT(*state, SurfNum, false);
    EXPECT_NEAR(RadTemp, 15.455, 0.1);

    SurfNum = 8;
    state->dataThermalComforts->clear_state();
    RadTemp = CalcSurfaceWeightedMRT(*state, SurfNum, false);
    EXPECT_NEAR(RadTemp, 14.545, 0.1);

    SurfNum = 10;
    state->dataThermalComforts->clear_state();
    RadTemp = CalcSurfaceWeightedMRT(*state, SurfNum, false);
    EXPECT_NEAR(RadTemp, 14.545, 0.1);

    SurfNum = 12;
    state->dataThermalComforts->clear_state();
    RadTemp = CalcSurfaceWeightedMRT(*state, SurfNum, false);
    EXPECT_NEAR(RadTemp, 14.545, 0.1);
}

TEST_F(EnergyPlusFixture, CalcFangerPMVTest)
{
    // Test of Fix for Defect #11112 (update PMV calculation to match current ASHRAE 55/ISO 7730
    Real64 airTemp;
    Real64 radTemp;
    Real64 relHum;
    Real64 airVel;
    Real64 actLevel;
    Real64 cloUnit;
    Real64 workEff;
    Real64 expectedAnswer;
    Real64 actualAnswer;
    Real64 constexpr closeEnough = 0.00001;

    // Test 1: representative winter-ish conditions
    airTemp = 20.0;
    radTemp = 18.0;
    relHum = 0.50;
    airVel = 1.0;
    actLevel = 58.2;
    cloUnit = 1.5;
    workEff = 0.0;
    expectedAnswer = -0.715102;
    actualAnswer = CalcFangerPMV(*state, airTemp, radTemp, relHum, airVel, actLevel, cloUnit, workEff);
    EXPECT_NEAR(actualAnswer, expectedAnswer, closeEnough);

    // Test 2: representative summer-ish conditions
    airTemp = 25.0;
    radTemp = 28.0;
    relHum = 0.75;
    airVel = 1.0;
    actLevel = 80.0;
    cloUnit = 0.8;
    workEff = 0.0;
    expectedAnswer = 0.644378;
    actualAnswer = CalcFangerPMV(*state, airTemp, radTemp, relHum, airVel, actLevel, cloUnit, workEff);
    EXPECT_NEAR(actualAnswer, expectedAnswer, closeEnough);
}

TEST_F(EnergyPlusFixture, ThermalComfortClothingValueReportVariableTest)
{
    // Tests for Defect #11120 Fix
    // Variable declarations
    Real64 tSet;
    Real64 resultingPMV;
    Real64 constexpr closeEnough = 0.000000001;

    // Allocations/Set-up Data for Tests
    state->dataHeatBal->TotPeople = 1;
    state->dataHeatBal->People.allocate(state->dataHeatBal->TotPeople);
    state->dataThermalComforts->ThermalComfortData.allocate(state->dataHeatBal->TotPeople);
    state->dataZoneTempPredictorCorrector->zoneHeatBalance.allocate(1);
    state->dataThermalComforts->ZoneNum = 1;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance.allocate(1);
    state->dataRoomAir->anyNonMixingRoomAirModel = false;
    state->dataThermalComforts->PeopleNum = 1;
    state->dataHeatBal->People(1).ZonePtr = 1;
    state->dataHeatBal->People(1).spaceIndex = 1;
    state->dataHeatBal->People(1).MRTCalcType = DataHeatBalance::CalcMRT::EnclosureAveraged;
    state->dataViewFactor->EnclRadInfo.allocate(1);
    state->dataHeatBal->People(1).clothingType = DataHeatBalance::ClothingType::InsulationSchedule;
    state->dataHeatBal->People(1).clothingSched = Sched::GetScheduleAlwaysOn(*state);
    state->dataHeatBal->People(1).activityLevelSched = Sched::GetScheduleAlwaysOn(*state);
    state->dataHeatBal->People(1).workEffSched = Sched::GetScheduleAlwaysOn(*state);
    state->dataHeatBal->People(1).airVelocitySched = Sched::GetScheduleAlwaysOn(*state);

    state->dataRoomAir->IsZoneCrossVent.allocate(1);
    state->dataRoomAir->IsZoneCrossVent(1) = false;
    state->dataHeatBalFanSys->ZoneQdotRadHVACToPerson.allocate(1);
    state->dataHeatBalFanSys->ZoneQdotRadHVACToPerson(1) = 0.0;
    state->dataHeatBalFanSys->ZoneQHTRadSysToPerson.allocate(1);
    state->dataHeatBalFanSys->ZoneQHTRadSysToPerson(1) = 0.0;
    state->dataHeatBalFanSys->ZoneQCoolingPanelToPerson.allocate(1);
    state->dataHeatBalFanSys->ZoneQCoolingPanelToPerson(1) = 0.0;
    state->dataHeatBalFanSys->ZoneQHWBaseboardToPerson.allocate(1);
    state->dataHeatBalFanSys->ZoneQHWBaseboardToPerson(1) = 0.0;
    state->dataHeatBalFanSys->ZoneQSteamBaseboardToPerson.allocate(1);
    state->dataHeatBalFanSys->ZoneQSteamBaseboardToPerson(1) = 0.0;
    state->dataHeatBalFanSys->ZoneQElecBaseboardToPerson.allocate(1);
    state->dataHeatBalFanSys->ZoneQElecBaseboardToPerson(1) = 0.0;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(1).airHumRatAvgComf = 0.008;
    state->dataEnvrn->OutBaroPress = 101400.0;
    state->dataHeatBal->space.allocate(1);
    state->dataHeatBal->space(1).radiantEnclosureNum = 1;
    state->dataViewFactor->EnclRadInfo.allocate(1);
    state->dataViewFactor->EnclRadInfo(1).MRT = 23.0;

    // Test 1: Fanger Model (this one was NOT working previously)
    state->dataHeatBal->People(1).Fanger = true;
    state->dataHeatBal->People(1).KSU = false;
    state->dataHeatBal->People(1).Pierce = false;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(1).MAT = 23.0;
    tSet = 20.0;
    resultingPMV = 0.0;
    state->dataThermalComforts->ThermalComfortData(1).ClothingValue = 0.0;

    CalcThermalComfortFanger(*state, 1, tSet, resultingPMV);
    EXPECT_NEAR(1.0, state->dataThermalComforts->ThermalComfortData(1).ClothingValue, closeEnough);

    // Test 2: Pierce Model (this one was working previously)
    state->dataHeatBal->People(1).Fanger = false;
    state->dataHeatBal->People(1).KSU = false;
    state->dataHeatBal->People(1).Pierce = true;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(1).MAT = 23.0;
    state->dataThermalComforts->ThermalComfortData(1).ClothingValue = 0.0;
    state->dataThermalComforts->AirTemp = 23.0;
    state->dataThermalComforts->RadTemp = 24.0;
    state->dataThermalComforts->RelHum = 0.50;
    state->dataThermalComforts->AirVel = 1.0;
    state->dataThermalComforts->ActMet = 58.4;
    state->dataThermalComforts->CloUnit = 1.23;
    state->dataThermalComforts->WorkEff = 0.0;

    CalcThermalComfortPierceASHRAE(*state);
    EXPECT_NEAR(1.0, state->dataThermalComforts->ThermalComfortData(1).ClothingValue, closeEnough);

    // Test 3: KSU Model (this one was also working previously)
    state->dataHeatBal->People(1).Fanger = false;
    state->dataHeatBal->People(1).KSU = true;
    state->dataHeatBal->People(1).Pierce = false;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(1).ZTAVComf = 23.0;
    state->dataThermalComforts->ThermalComfortData(1).ClothingValue = 0.0;

    CalcThermalComfortKSU(*state);
    EXPECT_NEAR(1.0, state->dataThermalComforts->ThermalComfortData(1).ClothingValue, closeEnough);
}
