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

// Google Test Headers
#include <gtest/gtest.h>

// EnergyPlus Headers
#include "Fixtures/EnergyPlusFixture.hh"
#include <EnergyPlus/DataZoneEquipment.hh>

// I should not have to import these, but I do, the headers don't declare them
#include <EnergyPlus/Data/EnergyPlusData.hh>
#include <EnergyPlus/DataEnvironment.hh>
#include <EnergyPlus/DataHeatBalance.hh>
#include <EnergyPlus/HeatBalanceInternalHeatGains.hh>
#include <EnergyPlus/HeatBalanceManager.hh>
#include <EnergyPlus/Plant/DataPlant.hh>
#include <EnergyPlus/ScheduleManager.hh>

#include <EnergyPlus/InternalHeatGains.hh>
#include <EnergyPlus/RefrigeratedCase.hh>

// Standard headers
#include <string>
#include <string_view>

using namespace EnergyPlus;

static std::string_view constexpr oneZoneBuildingWithIdealLoads = R"IDF(

ScheduleTypeLimits,
  Any Number;              !- Name

Schedule:Constant,Always On,Any Number,1;

Schedule:Constant,Always Off,Any Number,0;

Zone,
  Zone_1,                  !- Name
  ,                        !- Direction of Relative North {deg}
  0,                       !- X Origin {m}
  0,                       !- Y Origin {m}
  0,                       !- Z Origin {m}
  ,                        !- Type
  1,                       !- Multiplier
  ,                        !- Ceiling Height {m}
  ,                        !- Volume {m3}
  ,                        !- Floor Area {m2}
  ,                        !- Zone Inside Convection Algorithm
  ,                        !- Zone Outside Convection Algorithm
  Yes;                     !- Part of Total Floor Area

Space,
  Space_1,                 !- Name
  Zone_1,                  !- Zone Name
  ,                        !- Ceiling Height {m}
  ,                        !- Volume {m3}
  ,                        !- Floor Area {m2}
  ;                        !- Space Type

BuildingSurface:Detailed,
  Space_1 Floor,           !- Name
  Floor,                   !- Surface Type
  Construction 1,          !- Construction Name
  Zone_1,                  !- Zone Name
  Space_1,                 !- Space Name
  Ground,                  !- Outside Boundary Condition
  ,                        !- Outside Boundary Condition Object
  NoSun,                   !- Sun Exposure
  NoWind,                  !- Wind Exposure
  ,                        !- View Factor to Ground
  ,                        !- Number of Vertices
  10,5,0,  !- X,Y,Z ==> Vertex 1 {m}
  10,-5,0,  !- X,Y,Z ==> Vertex 2 {m}
  -10,-5,0,  !- X,Y,Z ==> Vertex 3 {m}
  -10,5,0;  !- X,Y,Z ==> Vertex 4 {m}

BuildingSurface:Detailed,
  Space_1 RoofCeiling,     !- Name
  Roof,                    !- Surface Type
  Construction 1,          !- Construction Name
  Zone_1,                  !- Zone Name
  Space_1,                 !- Space Name
  Outdoors,                !- Outside Boundary Condition
  ,                        !- Outside Boundary Condition Object
  SunExposed,              !- Sun Exposure
  WindExposed,             !- Wind Exposure
  ,                        !- View Factor to Ground
  ,                        !- Number of Vertices
  -10,5,3,  !- X,Y,Z ==> Vertex 1 {m}
  -10,-5,3,  !- X,Y,Z ==> Vertex 2 {m}
  10,-5,3,  !- X,Y,Z ==> Vertex 3 {m}
  10,5,3;  !- X,Y,Z ==> Vertex 4 {m}

BuildingSurface:Detailed,
  Space_1 Wall 1,          !- Name
  Wall,                    !- Surface Type
  Construction 1,          !- Construction Name
  Zone_1,                  !- Zone Name
  Space_1,                 !- Space Name
  Outdoors,                !- Outside Boundary Condition
  ,                        !- Outside Boundary Condition Object
  SunExposed,              !- Sun Exposure
  WindExposed,             !- Wind Exposure
  ,                        !- View Factor to Ground
  ,                        !- Number of Vertices
  10,5,3,  !- X,Y,Z ==> Vertex 1 {m}
  10,-5,3,  !- X,Y,Z ==> Vertex 2 {m}
  10,-5,0,  !- X,Y,Z ==> Vertex 3 {m}
  10,5,0;  !- X,Y,Z ==> Vertex 4 {m}

BuildingSurface:Detailed,
  Space_1 Wall 2,          !- Name
  Wall,                    !- Surface Type
  Construction 1,          !- Construction Name
  Zone_1,                  !- Zone Name
  Space_1,                 !- Space Name
  Outdoors,                !- Outside Boundary Condition
  ,                        !- Outside Boundary Condition Object
  SunExposed,              !- Sun Exposure
  WindExposed,             !- Wind Exposure
  ,                        !- View Factor to Ground
  ,                        !- Number of Vertices
  10,-5,3,  !- X,Y,Z ==> Vertex 1 {m}
  -10,-5,3,  !- X,Y,Z ==> Vertex 2 {m}
  -10,-5,0,  !- X,Y,Z ==> Vertex 3 {m}
  10,-5,0;  !- X,Y,Z ==> Vertex 4 {m}

BuildingSurface:Detailed,
  Space_1 Wall 3,          !- Name
  Wall,                    !- Surface Type
  Construction 1,          !- Construction Name
  Zone_1,                  !- Zone Name
  Space_1,                 !- Space Name
  Outdoors,                !- Outside Boundary Condition
  ,                        !- Outside Boundary Condition Object
  SunExposed,              !- Sun Exposure
  WindExposed,             !- Wind Exposure
  ,                        !- View Factor to Ground
  ,                        !- Number of Vertices
  -10,-5,3,  !- X,Y,Z ==> Vertex 1 {m}
  -10,5,3,  !- X,Y,Z ==> Vertex 2 {m}
  -10,5,0,  !- X,Y,Z ==> Vertex 3 {m}
  -10,-5,0;  !- X,Y,Z ==> Vertex 4 {m}

BuildingSurface:Detailed,
  Space_1 Wall 4,          !- Name
  Wall,                    !- Surface Type
  Construction 1,          !- Construction Name
  Zone_1,                  !- Zone Name
  Space_1,                 !- Space Name
  Outdoors,                !- Outside Boundary Condition
  ,                        !- Outside Boundary Condition Object
  SunExposed,              !- Sun Exposure
  WindExposed,             !- Wind Exposure
  ,                        !- View Factor to Ground
  ,                        !- Number of Vertices
  -10,5,3,  !- X,Y,Z ==> Vertex 1 {m}
  10,5,3,  !- X,Y,Z ==> Vertex 2 {m}
  10,5,0,  !- X,Y,Z ==> Vertex 3 {m}
  -10,5,0;  !- X,Y,Z ==> Vertex 4 {m}

Material,
  Material 1,              !- Name
  Smooth,                  !- Roughness
  0.1,                     !- Thickness {m}
  0.1,                     !- Conductivity {W/m-K}
  0.1,                     !- Density {kg/m3}
  1400,                    !- Specific Heat {J/kg-K}
  0.9,                     !- Thermal Absorptance
  0.7,                     !- Solar Absorptance
  0.7;                     !- Visible Absorptance

Construction,
  Construction 1,          !- Name
  Material 1;              !- Outside Layer

ZoneHVAC:EquipmentConnections,
  Zone_1,                  !- Zone Name
  Zone_1 Equipment List,   !- Zone Conditioning Equipment List Name
  Zone_1 Inlet Node List,  !- Zone Air Inlet Node or NodeList Name
  Zone_1 Exhaust Node List,  !- Zone Air Exhaust Node or NodeList Name
  Node 1;                  !- Zone Air Node Name

NodeList,
  Zone_1 Inlet Node List,  !- Name
  Node 3;                  !- Node 1 Name

NodeList,
  Zone_1 Exhaust Node List,  !- Name
  Node 2;                  !- Node 1 Name

ZoneHVAC:IdealLoadsAirSystem,
  Zone HVAC Ideal Loads Air System 1,  !- Name
  ,                        !- Availability Schedule Name
  Node 3,                  !- Zone Supply Air Node Name
  Node 2,                  !- Zone Exhaust Air Node Name
  ,                        !- System Inlet Air Node Name
  ,                        !- Maximum Heating Supply Air Temperature {C}
  ,                        !- Minimum Cooling Supply Air Temperature {C}
  ,                        !- Maximum Heating Supply Air Humidity Ratio {kgWater/kgDryAir}
  ,                        !- Minimum Cooling Supply Air Humidity Ratio {kgWater/kgDryAir}
  ,                        !- Heating Limit
  ,                        !- Maximum Heating Air Flow Rate {m3/s}
  ,                        !- Maximum Sensible Heating Capacity {W}
  ,                        !- Cooling Limit
  ,                        !- Maximum Cooling Air Flow Rate {m3/s}
  ,                        !- Maximum Total Cooling Capacity {W}
  ,                        !- Heating Availability Schedule Name
  ,                        !- Cooling Availability Schedule Name
  ,                        !- Dehumidification Control Type
  ,                        !- Cooling Sensible Heat Ratio {dimensionless}
  ,                        !- Humidification Control Type
  ,                        !- Design Specification Outdoor Air Object Name
  ,                        !- Outdoor Air Inlet Node Name
  ,                        !- Demand Controlled Ventilation Type
  ,                        !- Outdoor Air Economizer Type
  ,                        !- Heat Recovery Type
  ,                        !- Sensible Heat Recovery Effectiveness {dimensionless}
  ;                        !- Latent Heat Recovery Effectiveness {dimensionless}

ZoneHVAC:EquipmentList,
  Zone_1 Equipment List,   !- Name
  SequentialLoad,          !- Load Distribution Scheme
  ZoneHVAC:IdealLoadsAirSystem,  !- Zone Equipment 1 Object Type
  Zone HVAC Ideal Loads Air System 1,  !- Zone Equipment 1 Name
  1,                       !- Zone Equipment 1 Cooling Sequence
  1,                       !- Zone Equipment 1 Heating or No-Load Sequence
  ,                        !- Zone Equipment 1 Sequential Cooling Fraction Schedule Name
  ;                        !- Zone Equipment 1 Sequential Heating Fraction Schedule Name

)IDF";

TEST_F(EnergyPlusFixture, RefrigeratedRackWithCaseInZone)
{
    std::string_view constexpr idf_objects = R"IDF(

Refrigeration:CompressorRack,
  SelfContainedDisplay,    !- Name
  Zone,                    !- Heat Rejection Location
  4.0,                     !- Design Compressor Rack COP {W/W}
  RackCOPfTCurve2,         !- Compressor Rack COP Function of Temperature Curve Name
  175.0,                   !- Design Condenser Fan Power {W}
  ,                        !- Condenser Fan Power Function of Temperature Curve Name
  AirCooled,               !- Condenser Type
  ,                        !- Water-Cooled Condenser Inlet Node Name
  ,                        !- Water-Cooled Condenser Outlet Node Name
  ,                        !- Water-Cooled Loop Flow Type
  ,                        !- Water-Cooled Condenser Outlet Temperature Schedule Name
  ,                        !- Water-Cooled Condenser Design Flow Rate {m3/s}
  ,                        !- Water-Cooled Condenser Maximum Flow Rate {m3/s}
  ,                        !- Water-Cooled Condenser Maximum Water Outlet Temperature {C}
  ,                        !- Water-Cooled Condenser Minimum Water Inlet Temperature {C}
  ,                        !- Evaporative Condenser Availability Schedule Name
  ,                        !- Evaporative Condenser Effectiveness {dimensionless}
  ,                        !- Evaporative Condenser Air Flow Rate {m3/s}
  ,                        !- Basin Heater Capacity {W/K}
  ,                        !- Basin Heater Setpoint Temperature {C}
  ,                        !- Design Evaporative Condenser Water Pump Power {W}
  ,                        !- Evaporative Water Supply Tank Name
  ,                        !- Condenser Air Inlet Node Name
  ,                        !- End-Use Subcategory
  SelfContainedDisplayCase,  !- Refrigeration Case Name or WalkIn Name or CaseAndWalkInList Name
  ZONE_1;                  !- Heat Rejection Zone Name

Curve:Quadratic,
  RackCOPfTCurve2,         !- Name
  1.0,                     !- Coefficient1 Constant
  0.0,                     !- Coefficient2 x
  0.0,                     !- Coefficient3 x**2
  0.0,                     !- Minimum Value of x
  50.0,                    !- Maximum Value of x
  ,                        !- Minimum Curve Output
  ,                        !- Maximum Curve Output
  Temperature,             !- Input Unit Type for X
  Dimensionless;           !- Output Unit Type

Refrigeration:Case,
  SelfContainedDisplayCase,  !- Name
  ,                        !- Availability Schedule Name
  ZONE_1,                  !- Zone Name
  23.88,                   !- Rated Ambient Temperature {C}
  55.0,                    !- Rated Ambient Relative Humidity {percent}
  1000.0,                  !- Rated Total Cooling Capacity per Unit Length {W/m}
  0.08,                    !- Rated Latent Heat Ratio
  0.85,                    !- Rated Runtime Fraction
  10.0,                    !- Case Length {m}
  13.0,                    !- Case Operating Temperature {C}
  CaseTemperatureMethod,   !- Latent Case Credit Curve Type
  MultiShelfVertical_LatentEnergyMult,  !- Latent Case Credit Curve Name
  40.0,                    !- Standard Case Fan Power per Unit Length {W/m}
  40.0,                    !- Operating Case Fan Power per Unit Length {W/m}
  75.0,                    !- Standard Case Lighting Power per Unit Length {W/m}
  ,                        !- Installed Case Lighting Power per Unit Length {W/m}
  Always On,               !- Case Lighting Schedule Name
  0.9,                     !- Fraction of Lighting Energy to Case
  0.0,                     !- Case Anti-Sweat Heater Power per Unit Length {W/m}
  ,                        !- Minimum Anti-Sweat Heater Power per Unit Length {W/m}
  None,                    !- Anti-Sweat Heater Control Type
  0.0,                     !- Humidity at Zero Anti-Sweat Heater Energy {percent}
  0.0,                     !- Case Height {m}
  0.0,                     !- Fraction of Anti-Sweat Heater Energy to Case
  0.0,                     !- Case Defrost Power per Unit Length {W/m}
  None,                    !- Case Defrost Type
  ,                        !- Case Defrost Schedule Name
  ,                        !- Case Defrost Drip-Down Schedule Name
  ,                        !- Defrost Energy Correction Curve Type
  ,                        !- Defrost Energy Correction Curve Name
  0.0,                     !- Under Case HVAC Return Air Fraction
  SelfContainedCaseStockingSched;  !- Refrigerated Case Restocking Schedule Name

Curve:Cubic,
  MultiShelfVertical_LatentEnergyMult,  !- Name
  0.026526281,             !- Coefficient1 Constant
  0.001078032,             !- Coefficient2 x
  -6.02558e-05,            !- Coefficient3 x**2
  1.23732e-06,             !- Coefficient4 x**3
  -35.0,                   !- Minimum Value of x
  20.0;                    !- Maximum Value of x

Schedule:Compact,
  SelfContainedCaseStockingSched,  !- Name
  AnyNumber,               !- Schedule Type Limits Name
  Through: 12/31,          !- Field 1
  For: AllDays,            !- Field 2
  Until: 6:00,0.0,         !- Field 3
  Until: 7:00,50.0,        !- Field 5
  Until: 9:00,70.0,        !- Field 7
  Until: 10:00,80.0,       !- Field 9
  Until: 11:00,70.0,       !- Field 11
  Until: 13:00,50.0,       !- Field 13
  Until: 14:00,80.0,       !- Field 15
  Until: 15:00,90.0,       !- Field 17
  Until: 16:00,80.0,       !- Field 19
  Until: 24:00,0.0;        !- Field 21

)IDF";

    ASSERT_TRUE(process_idf(std::format("{}\n{}", oneZoneBuildingWithIdealLoads, idf_objects))); // read idf objects
    state->init_state(*state);

    state->dataZoneEquip->ZoneEquipInputsFilled = true;
    state->dataEnvrn->OutBaroPress = 101325.0;
    bool ErrorsFound = false;
    HeatBalanceManager::GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);
    DataZoneEquipment::GetZoneEquipmentData(*state);

    // ZoneAirLoopEquipmentManager::GetZoneAirLoopEquipment(*state);
    InternalHeatGains::ManageInternalHeatGains(*state, true);
    RefrigeratedCase::ManageRefrigeratedCaseRacks(*state);
}

TEST_F(EnergyPlusFixture, RefrigeratedRackWithWalkInInZone)
{

    std::string_view constexpr idf_objects = R"IDF(

Refrigeration:CompressorRack,
  SelfContainedDisplay,    !- Name
  Zone,                    !- Heat Rejection Location
  4.0,                     !- Design Compressor Rack COP {W/W}
  RackCOPfTCurve2,         !- Compressor Rack COP Function of Temperature Curve Name
  175.0,                   !- Design Condenser Fan Power {W}
  ,                        !- Condenser Fan Power Function of Temperature Curve Name
  AirCooled,               !- Condenser Type
  ,                        !- Water-Cooled Condenser Inlet Node Name
  ,                        !- Water-Cooled Condenser Outlet Node Name
  ,                        !- Water-Cooled Loop Flow Type
  ,                        !- Water-Cooled Condenser Outlet Temperature Schedule Name
  ,                        !- Water-Cooled Condenser Design Flow Rate {m3/s}
  ,                        !- Water-Cooled Condenser Maximum Flow Rate {m3/s}
  ,                        !- Water-Cooled Condenser Maximum Water Outlet Temperature {C}
  ,                        !- Water-Cooled Condenser Minimum Water Inlet Temperature {C}
  ,                        !- Evaporative Condenser Availability Schedule Name
  ,                        !- Evaporative Condenser Effectiveness {dimensionless}
  ,                        !- Evaporative Condenser Air Flow Rate {m3/s}
  ,                        !- Basin Heater Capacity {W/K}
  ,                        !- Basin Heater Setpoint Temperature {C}
  ,                        !- Design Evaporative Condenser Water Pump Power {W}
  ,                        !- Evaporative Water Supply Tank Name
  ,                        !- Condenser Air Inlet Node Name
  ,                        !- End-Use Subcategory
  RefrigerationWalkIn,     !- Refrigeration Case Name or WalkIn Name or CaseAndWalkInList Name
  ZONE_1;                  !- Heat Rejection Zone Name

Curve:Quadratic,
  RackCOPfTCurve2,         !- Name
  1.0,                     !- Coefficient1 Constant
  0.0,                     !- Coefficient2 x
  0.0,                     !- Coefficient3 x**2
  0.0,                     !- Minimum Value of x
  50.0,                    !- Maximum Value of x
  ,                        !- Minimum Curve Output
  ,                        !- Maximum Curve Output
  Temperature,             !- Input Unit Type for X
  Dimensionless;           !- Output Unit Type

Refrigeration:WalkIn,
  RefrigerationWalkIn,     !- Name
  Always On,               !- Availability Schedule Name
  5,                       !- Rated Coil Cooling Capacity {W}
  1.66666666666667,        !- Operating Temperature {C}
  -6.11111111111111,       !- Rated Cooling Source Temperature {C}
  0.0,                     !- Rated Total Heating Power {W}
  Always On,               !- Heating Power Schedule Name
  5,                       !- Rated Cooling Coil Fan Power {W}
  0.0,                     !- Rated Circulation Fan Power {W}
  5,                       !- Rated Total Lighting Power {W}
  Always On,               !- Lighting Schedule Name
  Electric,                !- Defrost Type
  TimeSchedule,            !- Defrost Control Type
  Always Off,              !- Defrost Schedule Name
  ,                        !- Defrost Drip-Down Schedule Name
  0.001,                   !- Defrost Power {W}
  ,                        !- Temperature Termination Defrost Fraction to Ice {dimensionless}
  ,                        !- Restocking Schedule Name
  ,                        !- Average Refrigerant Charge Inventory {kg}
  1,                       !- Insulated Floor Surface Area {m2}
  0.17744571875,           !- Insulated Floor U-Value {W/m2-K}
  ZONE_1,                  !- Zone 1 Name
  6.504,                   !- Total Insulated Surface Area Facing Zone 1 {m2}
  0.17744571875,           !- Insulated Surface U-Value Facing Zone 1 {W/m2-K}
  ,                        !- Area of Glass Reach In Doors Facing Zone 1 {m2}
  ,                        !- Height of Glass Reach In Doors Facing Zone 1 {m}
  ,                        !- Glass Reach In Door U Value Facing Zone 1 {W/m2-K}
  ,                        !- Glass Reach In Door Opening Schedule Name Facing Zone 1
  1.216,                   !- Area of Stocking Doors Facing Zone 1 {m2}
  1.651,                   !- Height of Stocking Doors Facing Zone 1 {m}
  ,                        !- Stocking Door U Value Facing Zone 1 {W/m2-K}
  ,                        !- Stocking Door Opening Schedule Name Facing Zone 1
  None;                    !- Stocking Door Opening Protection Type Facing Zone 1

)IDF";

    ASSERT_TRUE(process_idf(std::format("{}\n{}", oneZoneBuildingWithIdealLoads, idf_objects))); // read idf objects
    state->init_state(*state);

    state->dataZoneEquip->ZoneEquipInputsFilled = true;
    state->dataEnvrn->OutBaroPress = 101325.0;
    bool ErrorsFound = false;
    HeatBalanceManager::GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);
    DataZoneEquipment::GetZoneEquipmentData(*state);
    // ZoneAirLoopEquipmentManager::GetZoneAirLoopEquipment(*state);
    InternalHeatGains::ManageInternalHeatGains(*state, true);
    RefrigeratedCase::ManageRefrigeratedCaseRacks(*state);
}

TEST_F(EnergyPlusFixture, RefrigeratedRackWithWalkInInZone_CaseAndWalkinList)
{

    std::string_view constexpr idf_objects = R"IDF(

Refrigeration:CompressorRack,
  SelfContainedDisplay,    !- Name
  Zone,                    !- Heat Rejection Location
  4.0,                     !- Design Compressor Rack COP {W/W}
  RackCOPfTCurve2,         !- Compressor Rack COP Function of Temperature Curve Name
  175.0,                   !- Design Condenser Fan Power {W}
  ,                        !- Condenser Fan Power Function of Temperature Curve Name
  AirCooled,               !- Condenser Type
  ,                        !- Water-Cooled Condenser Inlet Node Name
  ,                        !- Water-Cooled Condenser Outlet Node Name
  ,                        !- Water-Cooled Loop Flow Type
  ,                        !- Water-Cooled Condenser Outlet Temperature Schedule Name
  ,                        !- Water-Cooled Condenser Design Flow Rate {m3/s}
  ,                        !- Water-Cooled Condenser Maximum Flow Rate {m3/s}
  ,                        !- Water-Cooled Condenser Maximum Water Outlet Temperature {C}
  ,                        !- Water-Cooled Condenser Minimum Water Inlet Temperature {C}
  ,                        !- Evaporative Condenser Availability Schedule Name
  ,                        !- Evaporative Condenser Effectiveness {dimensionless}
  ,                        !- Evaporative Condenser Air Flow Rate {m3/s}
  ,                        !- Basin Heater Capacity {W/K}
  ,                        !- Basin Heater Setpoint Temperature {C}
  ,                        !- Design Evaporative Condenser Water Pump Power {W}
  ,                        !- Evaporative Water Supply Tank Name
  ,                        !- Condenser Air Inlet Node Name
  ,                        !- End-Use Subcategory
  CompressorRack with Case and Walkin List,  !- Refrigeration Case Name or WalkIn Name or CaseAndWalkInList Name
  ZONE_1;                  !- Heat Rejection Zone Name

Curve:Quadratic,
  RackCOPfTCurve2,         !- Name
  1.0,                     !- Coefficient1 Constant
  0.0,                     !- Coefficient2 x
  0.0,                     !- Coefficient3 x**2
  0.0,                     !- Minimum Value of x
  50.0,                    !- Maximum Value of x
  ,                        !- Minimum Curve Output
  ,                        !- Maximum Curve Output
  Temperature,             !- Input Unit Type for X
  Dimensionless;           !- Output Unit Type

Refrigeration:CaseAndWalkInList,
  CompressorRack with Case and Walkin List,  !- Name
  RefrigerationWalkIn;     !- Case or WalkIn 1 Name

Refrigeration:WalkIn,
  RefrigerationWalkIn,     !- Name
  Always On,               !- Availability Schedule Name
  5,                       !- Rated Coil Cooling Capacity {W}
  1.66666666666667,        !- Operating Temperature {C}
  -6.11111111111111,       !- Rated Cooling Source Temperature {C}
  0.0,                     !- Rated Total Heating Power {W}
  Always On,               !- Heating Power Schedule Name
  5,                       !- Rated Cooling Coil Fan Power {W}
  0.0,                     !- Rated Circulation Fan Power {W}
  5,                       !- Rated Total Lighting Power {W}
  Always On,               !- Lighting Schedule Name
  Electric,                !- Defrost Type
  TimeSchedule,            !- Defrost Control Type
  Always Off,              !- Defrost Schedule Name
  ,                        !- Defrost Drip-Down Schedule Name
  0.001,                   !- Defrost Power {W}
  ,                        !- Temperature Termination Defrost Fraction to Ice {dimensionless}
  ,                        !- Restocking Schedule Name
  ,                        !- Average Refrigerant Charge Inventory {kg}
  1,                       !- Insulated Floor Surface Area {m2}
  0.17744571875,           !- Insulated Floor U-Value {W/m2-K}
  ZONE_1,                  !- Zone 1 Name
  6.504,                   !- Total Insulated Surface Area Facing Zone 1 {m2}
  0.17744571875,           !- Insulated Surface U-Value Facing Zone 1 {W/m2-K}
  ,                        !- Area of Glass Reach In Doors Facing Zone 1 {m2}
  ,                        !- Height of Glass Reach In Doors Facing Zone 1 {m}
  ,                        !- Glass Reach In Door U Value Facing Zone 1 {W/m2-K}
  ,                        !- Glass Reach In Door Opening Schedule Name Facing Zone 1
  1.216,                   !- Area of Stocking Doors Facing Zone 1 {m2}
  1.651,                   !- Height of Stocking Doors Facing Zone 1 {m}
  ,                        !- Stocking Door U Value Facing Zone 1 {W/m2-K}
  ,                        !- Stocking Door Opening Schedule Name Facing Zone 1
  None;                    !- Stocking Door Opening Protection Type Facing Zone 1

)IDF";

    ASSERT_TRUE(process_idf(std::format("{}\n{}", oneZoneBuildingWithIdealLoads, idf_objects))); // read idf objects
    state->init_state(*state);

    state->dataZoneEquip->ZoneEquipInputsFilled = true;
    state->dataEnvrn->OutBaroPress = 101325.0;
    bool ErrorsFound = false;
    HeatBalanceManager::GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);
    DataZoneEquipment::GetZoneEquipmentData(*state);
    // ZoneAirLoopEquipmentManager::GetZoneAirLoopEquipment(*state);
    InternalHeatGains::ManageInternalHeatGains(*state, true);
    RefrigeratedCase::ManageRefrigeratedCaseRacks(*state);
}

TEST_F(EnergyPlusFixture, RefrigeratedRackWithBothInZone_CaseAndWalkinList)
{

    std::string_view constexpr idf_objects = R"IDF(

Refrigeration:CompressorRack,
  SelfContainedDisplay,    !- Name
  Zone,                    !- Heat Rejection Location
  4.0,                     !- Design Compressor Rack COP {W/W}
  RackCOPfTCurve2,         !- Compressor Rack COP Function of Temperature Curve Name
  175.0,                   !- Design Condenser Fan Power {W}
  ,                        !- Condenser Fan Power Function of Temperature Curve Name
  AirCooled,               !- Condenser Type
  ,                        !- Water-Cooled Condenser Inlet Node Name
  ,                        !- Water-Cooled Condenser Outlet Node Name
  ,                        !- Water-Cooled Loop Flow Type
  ,                        !- Water-Cooled Condenser Outlet Temperature Schedule Name
  ,                        !- Water-Cooled Condenser Design Flow Rate {m3/s}
  ,                        !- Water-Cooled Condenser Maximum Flow Rate {m3/s}
  ,                        !- Water-Cooled Condenser Maximum Water Outlet Temperature {C}
  ,                        !- Water-Cooled Condenser Minimum Water Inlet Temperature {C}
  ,                        !- Evaporative Condenser Availability Schedule Name
  ,                        !- Evaporative Condenser Effectiveness {dimensionless}
  ,                        !- Evaporative Condenser Air Flow Rate {m3/s}
  ,                        !- Basin Heater Capacity {W/K}
  ,                        !- Basin Heater Setpoint Temperature {C}
  ,                        !- Design Evaporative Condenser Water Pump Power {W}
  ,                        !- Evaporative Water Supply Tank Name
  ,                        !- Condenser Air Inlet Node Name
  ,                        !- End-Use Subcategory
  CompressorRack with Case and Walkin List,  !- Refrigeration Case Name or WalkIn Name or CaseAndWalkInList Name
  ZONE_1;                  !- Heat Rejection Zone Name

Curve:Quadratic,
  RackCOPfTCurve2,         !- Name
  1.0,                     !- Coefficient1 Constant
  0.0,                     !- Coefficient2 x
  0.0,                     !- Coefficient3 x**2
  0.0,                     !- Minimum Value of x
  50.0,                    !- Maximum Value of x
  ,                        !- Minimum Curve Output
  ,                        !- Maximum Curve Output
  Temperature,             !- Input Unit Type for X
  Dimensionless;           !- Output Unit Type

Refrigeration:CaseAndWalkInList,
  CompressorRack with Case and Walkin List,  !- Name
  RefrigerationWalkIn,     !- Case or WalkIn 1 Name
  SelfContainedDisplayCase;  !- Case or WalkIn 2 Name

Refrigeration:WalkIn,
  RefrigerationWalkIn,     !- Name
  Always On,               !- Availability Schedule Name
  5,                       !- Rated Coil Cooling Capacity {W}
  1.66666666666667,        !- Operating Temperature {C}
  -6.11111111111111,       !- Rated Cooling Source Temperature {C}
  0.0,                     !- Rated Total Heating Power {W}
  Always On,               !- Heating Power Schedule Name
  5,                       !- Rated Cooling Coil Fan Power {W}
  0.0,                     !- Rated Circulation Fan Power {W}
  5,                       !- Rated Total Lighting Power {W}
  Always On,               !- Lighting Schedule Name
  Electric,                !- Defrost Type
  TimeSchedule,            !- Defrost Control Type
  Always Off,              !- Defrost Schedule Name
  ,                        !- Defrost Drip-Down Schedule Name
  0.001,                   !- Defrost Power {W}
  ,                        !- Temperature Termination Defrost Fraction to Ice {dimensionless}
  ,                        !- Restocking Schedule Name
  ,                        !- Average Refrigerant Charge Inventory {kg}
  1,                       !- Insulated Floor Surface Area {m2}
  0.17744571875,           !- Insulated Floor U-Value {W/m2-K}
  ZONE_1,                  !- Zone 1 Name
  6.504,                   !- Total Insulated Surface Area Facing Zone 1 {m2}
  0.17744571875,           !- Insulated Surface U-Value Facing Zone 1 {W/m2-K}
  ,                        !- Area of Glass Reach In Doors Facing Zone 1 {m2}
  ,                        !- Height of Glass Reach In Doors Facing Zone 1 {m}
  ,                        !- Glass Reach In Door U Value Facing Zone 1 {W/m2-K}
  ,                        !- Glass Reach In Door Opening Schedule Name Facing Zone 1
  1.216,                   !- Area of Stocking Doors Facing Zone 1 {m2}
  1.651,                   !- Height of Stocking Doors Facing Zone 1 {m}
  ,                        !- Stocking Door U Value Facing Zone 1 {W/m2-K}
  ,                        !- Stocking Door Opening Schedule Name Facing Zone 1
  None;                    !- Stocking Door Opening Protection Type Facing Zone 1

Refrigeration:Case,
  SelfContainedDisplayCase,  !- Name
  ,                        !- Availability Schedule Name
  ZONE_1,                  !- Zone Name
  23.88,                   !- Rated Ambient Temperature {C}
  55.0,                    !- Rated Ambient Relative Humidity {percent}
  1000.0,                  !- Rated Total Cooling Capacity per Unit Length {W/m}
  0.08,                    !- Rated Latent Heat Ratio
  0.85,                    !- Rated Runtime Fraction
  10.0,                    !- Case Length {m}
  13.0,                    !- Case Operating Temperature {C}
  CaseTemperatureMethod,   !- Latent Case Credit Curve Type
  MultiShelfVertical_LatentEnergyMult,  !- Latent Case Credit Curve Name
  40.0,                    !- Standard Case Fan Power per Unit Length {W/m}
  40.0,                    !- Operating Case Fan Power per Unit Length {W/m}
  75.0,                    !- Standard Case Lighting Power per Unit Length {W/m}
  ,                        !- Installed Case Lighting Power per Unit Length {W/m}
  Always On,               !- Case Lighting Schedule Name
  0.9,                     !- Fraction of Lighting Energy to Case
  0.0,                     !- Case Anti-Sweat Heater Power per Unit Length {W/m}
  ,                        !- Minimum Anti-Sweat Heater Power per Unit Length {W/m}
  None,                    !- Anti-Sweat Heater Control Type
  0.0,                     !- Humidity at Zero Anti-Sweat Heater Energy {percent}
  0.0,                     !- Case Height {m}
  0.0,                     !- Fraction of Anti-Sweat Heater Energy to Case
  0.0,                     !- Case Defrost Power per Unit Length {W/m}
  None,                    !- Case Defrost Type
  ,                        !- Case Defrost Schedule Name
  ,                        !- Case Defrost Drip-Down Schedule Name
  ,                        !- Defrost Energy Correction Curve Type
  ,                        !- Defrost Energy Correction Curve Name
  0.0,                     !- Under Case HVAC Return Air Fraction
  SelfContainedCaseStockingSched;  !- Refrigerated Case Restocking Schedule Name

Curve:Cubic,
  MultiShelfVertical_LatentEnergyMult,  !- Name
  0.026526281,             !- Coefficient1 Constant
  0.001078032,             !- Coefficient2 x
  -6.02558e-05,            !- Coefficient3 x**2
  1.23732e-06,             !- Coefficient4 x**3
  -35.0,                   !- Minimum Value of x
  20.0;                    !- Maximum Value of x

Schedule:Compact,
  SelfContainedCaseStockingSched,  !- Name
  AnyNumber,               !- Schedule Type Limits Name
  Through: 12/31,          !- Field 1
  For: AllDays,            !- Field 2
  Until: 6:00,0.0,         !- Field 3
  Until: 7:00,50.0,        !- Field 5
  Until: 9:00,70.0,        !- Field 7
  Until: 10:00,80.0,       !- Field 9
  Until: 11:00,70.0,       !- Field 11
  Until: 13:00,50.0,       !- Field 13
  Until: 14:00,80.0,       !- Field 15
  Until: 15:00,90.0,       !- Field 17
  Until: 16:00,80.0,       !- Field 19
  Until: 24:00,0.0;        !- Field 21

)IDF";

    ASSERT_TRUE(process_idf(std::format("{}\n{}", oneZoneBuildingWithIdealLoads, idf_objects))); // read idf objects
    state->init_state(*state);

    state->dataZoneEquip->ZoneEquipInputsFilled = true;
    state->dataEnvrn->OutBaroPress = 101325.0;
    bool ErrorsFound = false;
    HeatBalanceManager::GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);
    DataZoneEquipment::GetZoneEquipmentData(*state);
    // ZoneAirLoopEquipmentManager::GetZoneAirLoopEquipment(*state);
    InternalHeatGains::ManageInternalHeatGains(*state, true);
    RefrigeratedCase::ManageRefrigeratedCaseRacks(*state);
}

TEST_F(EnergyPlusFixture, DesuperheaterRefrigeration)
{
    std::string const idf_objects = delimited_string({
        "Zone,",
        "  Basic Zone,              !- Name",
        "  ,                        !- Direction of Relative North {deg}",
        "  0,                       !- X Origin {m}",
        "  0,                       !- Y Origin {m}",
        "  0,                       !- Z Origin {m}",
        "  ,                        !- Type",
        "  1,                       !- Multiplier",
        "  ,                        !- Ceiling Height {m}",
        "  ,                        !- Volume {m3}",
        "  ,                        !- Floor Area {m2}",
        "  ,                        !- Zone Inside Convection Algorithm",
        "  ,                        !- Zone Outside Convection Algorithm",
        "  Yes;                     !- Part of Total Floor Area",

        "ZoneHVAC:EquipmentConnections,",
        "  Basic Zone,              !- Zone Name",
        "  Basic Zone Equipment List,  !- Zone Conditioning Equipment List Name",
        "  Basic Zone Inlet Node,   !- Zone Air Inlet Node or NodeList Name",
        "  Basic Zone Exhaust Node,  !- Zone Air Exhaust Node or NodeList Name",
        "  Basic Zone Air Node;     !- Zone Air Node Name",

        "ZoneHVAC:EquipmentList,",
        "  Basic Zone Equipment List,  !- Name",
        "  SequentialLoad,          !- Load Distribution Scheme",
        "  ZoneHVAC:PackagedTerminalAirConditioner,  !- Zone Equipment 1 Object Type",
        "  Basic Zone PTAC,         !- Zone Equipment 1 Name",
        "  1,                       !- Zone Equipment 1 Cooling Sequence",
        "  1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 1 Sequential Cooling Fraction Schedule Name",
        "  ;                        !- Zone Equipment 1 Sequential Heating Fraction Schedule Name",

        "ZoneHVAC:PackagedTerminalAirConditioner,",
        "  Basic Zone PTAC,         !- Name",
        "  ,                        !- Availability Schedule Name",
        "  Basic Zone PTAC Inlet Node,  !- Air Inlet Node Name",
        "  Basic Zone Inlet Node,   !- Air Outlet Node Name",
        "  OutdoorAir:Mixer,        !- Outdoor Air Mixer Object Type",
        "  Basic Zone PTAC OA Mixer,  !- Outdoor Air Mixer Name",
        "  Autosize,                !- Cooling Supply Air Flow Rate {m3/s}",
        "  Autosize,                !- Heating Supply Air Flow Rate {m3/s}",
        "  Autosize,                !- No Load Supply Air Flow Rate {m3/s}",
        "  No,                      !- No Load Supply Air Flow Rate Control Set To Low Speed",
        "  Autosize,                !- Cooling Outdoor Air Flow Rate {m3/s}",
        "  Autosize,                !- Heating Outdoor Air Flow Rate {m3/s}",
        "  Autosize,                !- No Load Outdoor Air Flow Rate {m3/s}",
        "  Fan:OnOff,               !- Supply Air Fan Object Type",
        "  Basic Zone PTAC Fan,     !- Supply Air Fan Name",
        "  Coil:Heating:Fuel,       !- Heating Coil Object Type",
        "  Basic Zone PTAC Heating Coil,  !- Heating Coil Name",
        "  Coil:Cooling:DX:SingleSpeed,  !- Cooling Coil Object Type",
        "  Basic Zone PTAC Cooling Coil,  !- Cooling Coil Name",
        "  DrawThrough,             !- Fan Placement",
        "  ;                        !- Supply Air Fan Operating Mode Schedule Name",

        "OutdoorAir:Mixer,",
        "  Basic Zone PTAC OA Mixer,  !- Name",
        "  Basic Zone PTAC Mixed Air Node,  !- Mixed Air Node Name",
        "  Basic Zone PTAC OA Node,  !- Outdoor Air Stream Node Name",
        "  Basic Zone PTAC Relief Air Node,  !- Relief Air Stream Node Name",
        "  Basic Zone PTAC Inlet Node;  !- Return Air Stream Node Name",

        "OutdoorAir:NodeList,",
        "  Basic Zone PTAC OA Node;  !- Node or NodeList Name 1",

        "Fan:OnOff,",
        "  Basic Zone PTAC Fan,     !- Name",
        "  ,                        !- Availability Schedule Name",
        "  0.6,                     !- Fan Total Efficiency",
        "  250,                     !- Pressure Rise {Pa}",
        "  Autosize,                !- Maximum Flow Rate {m3/s}",
        "  0.8,                     !- Motor Efficiency",
        "  1,                       !- Motor In Airstream Fraction",
        "  Basic Zone PTAC Heating Coil Outlet Node,  !- Air Inlet Node Name",
        "  Basic Zone Inlet Node,   !- Air Outlet Node Name",
        "  Generic Curve,           !- Fan Power Ratio Function of Speed Ratio Curve Name",
        "  Generic Curve,           !- Fan Efficiency Ratio Function of Speed Ratio Curve Name",
        "  General;                 !- End-Use Subcategory",

        "Coil:Heating:Fuel,",
        "  Basic Zone PTAC Heating Coil,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  NaturalGas,              !- Fuel Type",
        "  0.8,                     !- Burner Efficiency",
        "  1,                       !- Nominal Capacity {W}",
        "  Basic Zone PTAC Cooling Coil Outlet Node,  !- Air Inlet Node Name",
        "  Basic Zone PTAC Heating Coil Outlet Node,  !- Air Outlet Node Name",
        "  ,                        !- Temperature Setpoint Node Name",
        "  0,                       !- On Cycle Parasitic Electric Load {W}",
        "  ,                        !- Part Load Fraction Correlation Curve Name",
        "  0;                       !- Off Cycle Parasitic Fuel Load {W}",

        "Coil:Cooling:DX:SingleSpeed,",
        "  Basic Zone PTAC Cooling Coil,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  Autosize,                !- Gross Rated Total Cooling Capacity {W}",
        "  Autosize,                !- Gross Rated Sensible Heat Ratio",
        "  3,                       !- Gross Rated Cooling COP {W/W}",
        "  Autosize,                !- Rated Air Flow Rate {m3/s}",
        "  773.3,                   !- 2017 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  934.4,                   !- 2023 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  Basic Zone PTAC Mixed Air Node,  !- Air Inlet Node Name",
        "  Basic Zone PTAC Cooling Coil Outlet Node,  !- Air Outlet Node Name",
        "  Generic Curve,           !- Total Cooling Capacity Function of Temperature Curve Name",
        "  Generic Curve,           !- Total Cooling Capacity Function of Flow Fraction Curve Name",
        "  Generic Curve,           !- Energy Input Ratio Function of Temperature Curve Name",
        "  Generic Curve,           !- Energy Input Ratio Function of Flow Fraction Curve Name",
        "  Generic Curve,           !- Part Load Fraction Correlation Curve Name",
        "  -25,                     !- Minimum Outdoor Dry-Bulb Temperature for Compressor Operation {C}",
        "  0,                       !- Nominal Time for Condensate Removal to Begin {s}",
        "  0,                       !- Ratio of Initial Moisture Evaporation Rate and Steady State Latent Capacity {dimensionless}",
        "  0,                       !- Maximum Cycling Rate {cycles/hr}",
        "  0,                       !- Latent Capacity Time Constant {s}",
        "  ,                        !- Condenser Air Inlet Node Name",
        "  AirCooled,               !- Condenser Type",
        "  0,                       !- Evaporative Condenser Effectiveness {dimensionless}",
        "  Autosize,                !- Evaporative Condenser Air Flow Rate {m3/s}",
        "  Autosize,                !- Evaporative Condenser Pump Rated Power Consumption {W}",
        "  0,                       !- Crankcase Heater Capacity {W}",
        "  ,                        !- Crankcase Heater Capacity Function of Temperature Curve Name",
        "  0,                       !- Maximum Outdoor Dry-Bulb Temperature for Crankcase Heater Operation {C}",
        "  ,                        !- Supply Water Storage Tank Name",
        "  ,                        !- Condensate Collection Water Storage Tank Name",
        "  0,                       !- Basin Heater Capacity {W/K}",
        "  10;                      !- Basin Heater Setpoint Temperature {C}",

        "Zone,",
        "  Refrig Cond AC Zone,     !- Name",
        "  ,                        !- Direction of Relative North {deg}",
        "  0,                       !- X Origin {m}",
        "  0,                       !- Y Origin {m}",
        "  0,                       !- Z Origin {m}",
        "  ,                        !- Type",
        "  1,                       !- Multiplier",
        "  ,                        !- Ceiling Height {m}",
        "  ,                        !- Volume {m3}",
        "  ,                        !- Floor Area {m2}",
        "  ,                        !- Zone Inside Convection Algorithm",
        "  ,                        !- Zone Outside Convection Algorithm",
        "  Yes;                     !- Part of Total Floor Area",

        "ZoneHVAC:EquipmentConnections,",
        "  Refrig Cond AC Zone,     !- Zone Name",
        "  Refrig Cond AC Zone Equipment List,  !- Zone Conditioning Equipment List Name",
        "  Refrig Cond AC Zone Inlet Node,  !- Zone Air Inlet Node or NodeList Name",
        "  Refrig Cond AC Zone Exhaust Node,  !- Zone Air Exhaust Node or NodeList Name",
        "  Refrig Cond AC Zone Air Node,  !- Zone Air Node Name",
        "  Refrig Cond AC Zone Return Node;  !- Zone Return Air Node or NodeList Name",

        "ZoneHVAC:EquipmentList,",
        "  Refrig Cond AC Zone Equipment List,  !- Name",
        "  SequentialLoad,          !- Load Distribution Scheme",
        "  ZoneHVAC:AirDistributionUnit,  !- Zone Equipment 1 Object Type",
        "  Refrig Cond AC ADU,      !- Zone Equipment 1 Name",
        "  1,                       !- Zone Equipment 1 Cooling Sequence",
        "  1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 1 Sequential Cooling Fraction Schedule Name",
        "  ;                        !- Zone Equipment 1 Sequential Heating Fraction Schedule Name",

        "ZoneHVAC:AirDistributionUnit,",
        "  Refrig Cond AC ADU,      !- Name",
        "  Refrig Cond AC Zone Inlet Node,  !- Air Distribution Unit Outlet Node Name",
        "  AirTerminal:SingleDuct:ConstantVolume:NoReheat,  !- Air Terminal Object Type",
        "  Refrig Cond AC Air Terminal;  !- Air Terminal Name",

        "AirTerminal:SingleDuct:ConstantVolume:NoReheat,",
        "  Refrig Cond AC Air Terminal,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  Refrig Cond AC Air Terminal Inlet Node,  !- Air Inlet Node Name",
        "  Refrig Cond AC Zone Inlet Node,  !- Air Outlet Node Name",
        "  Autosize;                !- Maximum Air Flow Rate {m3/s}",

        "AirLoopHVAC,",
        "  Refrig Cond AC PTAC,     !- Name",
        "  ,                        !- Controller List Name",
        "  ,                        !- Availability Manager List Name",
        "  Autosize,                !- Design Supply Air Flow Rate {m3/s}",
        "  Refrig Cond AC PTAC Supply Branch List,  !- Branch List Name",
        "  ,                        !- Connector List Name",
        "  Refrig Cond AC PTAC Supply Side Inlet Node,  !- Supply Side Inlet Node Name",
        "  Refrig Cond AC PTAC Demand Side Outlet Node,  !- Demand Side Outlet Node Name",
        "  Refrig Cond AC PTAC Demand Side Inlet Node,  !- Demand Side Inlet Node Names",
        "  Refrig Cond AC PTAC Supply Side Outlet Node,  !- Supply Side Outlet Node Names",
        "  1;                       !- Design Return Air Flow Fraction of Supply Air Flow",

        "BranchList,",
        "  Refrig Cond AC PTAC Supply Branch List,  !- Name",
        "  Refrig Cond AC PTAC Branch;  !- Branch 1 Name",

        "Branch,",
        "  Refrig Cond AC PTAC Branch,  !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  AirLoopHVAC:OutdoorAirSystem,  !- Component 1 Object Type",
        "  Refrig Cond AC PTAC OA System,  !- Component 1 Name",
        "  Refrig Cond AC PTAC Supply Side Inlet Node,  !- Component 1 Inlet Node Name",
        "  Refrig Cond AC PTAC Mixed Air Node,  !- Component 1 Outlet Node Name",
        "  Coil:Cooling:DX:SingleSpeed,  !- Component 2 Object Type",
        "  Refrig Cond AC PTAC Cooling Coil,  !- Component 2 Name",
        "  Refrig Cond AC PTAC Mixed Air Node,  !- Component 2 Inlet Node Name",
        "  Refrig Cond AC PTAC Desuperheater Inlet Node,  !- Component 2 Outlet Node Name",
        "  Coil:Heating:Desuperheater,  !- Component 3 Object Type",
        "  Refrig Cond AC PTAC Desuperheater,  !- Component 3 Name",
        "  Refrig Cond AC PTAC Desuperheater Inlet Node,  !- Component 3 Inlet Node Name",
        "  Refrig Cond AC PTAC Desuperheater Outlet Node,  !- Component 3 Outlet Node Name",
        "  Coil:Heating:Fuel,       !- Component 4 Object Type",
        "  Refrig Cond AC PTAC Heating Coil,  !- Component 4 Name",
        "  Refrig Cond AC PTAC Desuperheater Outlet Node,  !- Component 4 Inlet Node Name",
        "  Refrig Cond AC PTAC Heating Coil Outlet Node,  !- Component 4 Outlet Node Name",
        "  Fan:ConstantVolume,      !- Component 5 Object Type",
        "  Refrig Cond AC PTAC Fan,  !- Component 5 Name",
        "  Refrig Cond AC PTAC Heating Coil Outlet Node,  !- Component 5 Inlet Node Name",
        "  Refrig Cond AC PTAC Supply Side Outlet Node;  !- Component 5 Outlet Node Name",

        "AirLoopHVAC:OutdoorAirSystem,",
        "  Refrig Cond AC PTAC OA System,  !- Name",
        "  Refrig Cond AC PTAC OA System Controller List,  !- Controller List Name",
        "  Refrig Cond AC PTAC OA System Equipment List;  !- Outdoor Air Equipment List Name",

        "AirLoopHVAC:ControllerList,",
        "  Refrig Cond AC PTAC OA System Controller List,  !- Name",
        "  Controller:OutdoorAir,   !- Controller 1 Object Type",
        "  Refrig Cond AC PTAC OA Controller;  !- Controller 1 Name",

        "Controller:OutdoorAir,",
        "  Refrig Cond AC PTAC OA Controller,  !- Name",
        "  Refrig Cond AC PTAC Relief Node,  !- Relief Air Outlet Node Name",
        "  Refrig Cond AC PTAC Supply Side Inlet Node,  !- Return Air Node Name",
        "  Refrig Cond AC PTAC Mixed Air Node,  !- Mixed Air Node Name",
        "  Refrig Cond AC PTAC OA Node,  !- Actuator Node Name",
        "  0,                       !- Minimum Outdoor Air Flow Rate {m3/s}",
        "  Autosize,                !- Maximum Outdoor Air Flow Rate {m3/s}",
        "  NoEconomizer,            !- Economizer Control Type",
        "  ModulateFlow,            !- Economizer Control Action Type",
        "  28,                      !- Economizer Maximum Limit Dry-Bulb Temperature {C}",
        "  64000,                   !- Economizer Maximum Limit Enthalpy {J/kg}",
        "  ,                        !- Economizer Maximum Limit Dewpoint Temperature {C}",
        "  ,                        !- Electronic Enthalpy Limit Curve Name",
        "  -100,                    !- Economizer Minimum Limit Dry-Bulb Temperature {C}",
        "  NoLockout,               !- Lockout Type",
        "  FixedMinimum,            !- Minimum Limit Type",
        "  ,                        !- Minimum Outdoor Air Schedule Name",
        "  ,                        !- Minimum Fraction of Outdoor Air Schedule Name",
        "  ,                        !- Maximum Fraction of Outdoor Air Schedule Name",
        "  Refrig Cond AC PTAC MV Controller,  !- Mechanical Ventilation Controller Name",
        "  ,                        !- Time of Day Economizer Control Schedule Name",
        "  No,                      !- High Humidity Control",
        "  ,                        !- Humidistat Control Zone Name",
        "  ,                        !- High Humidity Outdoor Air Flow Ratio",
        "  Yes,                     !- Control High Indoor Humidity Based on Outdoor Humidity Ratio",
        "  BypassWhenWithinEconomizerLimits,  !- Heat Recovery Bypass Control Type",
        "  InterlockedWithMechanicalCooling;  !- Economizer Operation Staging",

        "OutdoorAir:NodeList,",
        "  Refrig Cond AC PTAC OA Node;  !- Node or NodeList Name 1",

        "Controller:MechanicalVentilation,",
        "  Refrig Cond AC PTAC MV Controller,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  No,                      !- Demand Controlled Ventilation",
        "  ZoneSum,                 !- System Outdoor Air Method",
        "  ,                        !- Zone Maximum Outdoor Air Fraction {dimensionless}",
        "  Refrig Cond AC Zone,     !- Zone or ZoneList 1 Name",
        "  ,                        !- Design Specification Outdoor Air Object Name 1",
        "  ;                        !- Design Specification Zone Air Distribution Object Name 1",

        "AirLoopHVAC:OutdoorAirSystem:EquipmentList,",
        "  Refrig Cond AC PTAC OA System Equipment List,  !- Name",
        "  OutdoorAir:Mixer,        !- Component 1 Object Type",
        "  Refrig Cond AC PTAC OA System Outdoor Air Mixer;  !- Component 1 Name",

        "OutdoorAir:Mixer,",
        "  Refrig Cond AC PTAC OA System Outdoor Air Mixer,  !- Name",
        "  Refrig Cond AC PTAC Mixed Air Node,  !- Mixed Air Node Name",
        "  Refrig Cond AC PTAC OA Node,  !- Outdoor Air Stream Node Name",
        "  Refrig Cond AC PTAC Relief Node,  !- Relief Air Stream Node Name",
        "  Refrig Cond AC PTAC Supply Side Inlet Node;  !- Return Air Stream Node Name",

        "Coil:Cooling:DX:SingleSpeed,",
        "  Refrig Cond AC PTAC Cooling Coil,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  Autosize,                !- Gross Rated Total Cooling Capacity {W}",
        "  Autosize,                !- Gross Rated Sensible Heat Ratio",
        "  3,                       !- Gross Rated Cooling COP {W/W}",
        "  Autosize,                !- Rated Air Flow Rate {m3/s}",
        "  773.3,                   !- 2017 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  934.4,                   !- 2023 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  Refrig Cond AC PTAC Mixed Air Node,  !- Air Inlet Node Name",
        "  Refrig Cond AC PTAC Desuperheater Inlet Node,  !- Air Outlet Node Name",
        "  Generic Curve,           !- Total Cooling Capacity Function of Temperature Curve Name",
        "  Generic Curve,           !- Total Cooling Capacity Function of Flow Fraction Curve Name",
        "  Generic Curve,           !- Energy Input Ratio Function of Temperature Curve Name",
        "  Generic Curve,           !- Energy Input Ratio Function of Flow Fraction Curve Name",
        "  Generic Curve,           !- Part Load Fraction Correlation Curve Name",
        "  -25,                     !- Minimum Outdoor Dry-Bulb Temperature for Compressor Operation {C}",
        "  0,                       !- Nominal Time for Condensate Removal to Begin {s}",
        "  0,                       !- Ratio of Initial Moisture Evaporation Rate and Steady State Latent Capacity {dimensionless}",
        "  0,                       !- Maximum Cycling Rate {cycles/hr}",
        "  0,                       !- Latent Capacity Time Constant {s}",
        "  ,                        !- Condenser Air Inlet Node Name",
        "  AirCooled,               !- Condenser Type",
        "  0.9,                     !- Evaporative Condenser Effectiveness {dimensionless}",
        "  Autosize,                !- Evaporative Condenser Air Flow Rate {m3/s}",
        "  Autosize,                !- Evaporative Condenser Pump Rated Power Consumption {W}",
        "  0,                       !- Crankcase Heater Capacity {W}",
        "  ,                        !- Crankcase Heater Capacity Function of Temperature Curve Name",
        "  10,                      !- Maximum Outdoor Dry-Bulb Temperature for Crankcase Heater Operation {C}",
        "  ,                        !- Supply Water Storage Tank Name",
        "  ,                        !- Condensate Collection Water Storage Tank Name",
        "  0,                       !- Basin Heater Capacity {W/K}",
        "  2;                       !- Basin Heater Setpoint Temperature {C}",

        "Coil:Heating:Desuperheater,",
        "  Refrig Cond AC PTAC Desuperheater,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  0.25,                    !- Heat Reclaim Recovery Efficiency",
        "  Refrig Cond AC PTAC Desuperheater Inlet Node,  !- Air Inlet Node Name",
        "  Refrig Cond AC PTAC Desuperheater Outlet Node,  !- Air Outlet Node Name",
        "  Refrigeration:Condenser:AirCooled,  !- Heating Source Object Type",
        "  Refrig Cond AC Refrig Condenser,  !- Heating Source Name",
        "  Refrig Cond AC PTAC Desuperheater Outlet Node;  !- Temperature Setpoint Node Name",

        "Coil:Heating:Fuel,",
        "  Refrig Cond AC PTAC Heating Coil,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  NaturalGas,              !- Fuel Type",
        "  0.9,                     !- Burner Efficiency",
        "  Autosize,                !- Nominal Capacity {W}",
        "  Refrig Cond AC PTAC Desuperheater Outlet Node,  !- Air Inlet Node Name",
        "  Refrig Cond AC PTAC Heating Coil Outlet Node,  !- Air Outlet Node Name",
        "  Refrig Cond AC PTAC Heating Coil Outlet Node,  !- Temperature Setpoint Node Name",
        "  0,                       !- On Cycle Parasitic Electric Load {W}",
        "  ,                        !- Part Load Fraction Correlation Curve Name",
        "  0;                       !- Off Cycle Parasitic Fuel Load {W}",

        "Fan:ConstantVolume,",
        "  Refrig Cond AC PTAC Fan,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  0.7,                     !- Fan Total Efficiency",
        "  500,                     !- Pressure Rise {Pa}",
        "  Autosize,                !- Maximum Flow Rate {m3/s}",
        "  0.9,                     !- Motor Efficiency",
        "  1,                       !- Motor In Airstream Fraction",
        "  Refrig Cond AC PTAC Heating Coil Outlet Node,  !- Air Inlet Node Name",
        "  Refrig Cond AC PTAC Supply Side Outlet Node;  !- Air Outlet Node Name",

        "Refrigeration:System,",
        "  Refrig Cond AC Refrig System,  !- Name",
        "  Refrig Cond AC CaseAndWalkInList,  !- Refrigerated Case or Walkin or CaseAndWalkInList Name",
        "  ,                        !- Refrigeration Transfer Load or TransferLoad List Name",
        "  Refrig Cond AC Refrig Condenser,  !- Refrigeration Condenser Name",
        "  Refrig Cond AC Refrig Compressor,  !- Compressor or CompressorList Name",
        "  20,                      !- Minimum Condensing Temperature {C}",
        "  R407a,                   !- Refrigeration System Working Fluid Type",
        "  ConstantSuctionTemperature,  !- Suction Temperature Control Type",
        "  ,                        !- Mechanical Subcooler Name",
        "  ,                        !- Liquid Suction Heat Exchanger Subcooler Name",
        "  0,                       !- Sum UA Suction Piping {W/K}",
        "  Refrig Cond AC Zone,     !- Suction Piping Zone Name",
        "  General,                 !- End-Use Subcategory",
        "  1,                       !- Number of Compressor Stages",
        "  None,                    !- Intercooler Type",
        "  0.8;                     !- Shell-and-Coil Intercooler Effectiveness",

        "Refrigeration:CaseAndWalkInList,",
        "  Refrig Cond AC CaseAndWalkInList,  !- Name",
        "  Refrig Cond AC Case,     !- Case or WalkIn 1 Name",
        "  Refrig Cond AC WalkIn;   !- Case or WalkIn 2 Name",

        "Refrigeration:Case,",
        "  Refrig Cond AC Case,     !- Name",
        "  ,                        !- Availability Schedule Name",
        "  Refrig Cond AC Zone,     !- Zone Name",
        "  24,                      !- Rated Ambient Temperature {C}",
        "  55,                      !- Rated Ambient Relative Humidity {percent}",
        "  1406,                    !- Rated Total Cooling Capacity per Unit Length {W/m}",
        "  0.3,                     !- Rated Latent Heat Ratio",
        "  0.85,                    !- Rated Runtime Fraction",
        "  2.19,                    !- Case Length {m}",
        "  4,                       !- Case Operating Temperature {C}",
        "  CaseTemperatureMethod,   !- Latent Case Credit Curve Type",
        "  Generic Curve,           !- Latent Case Credit Curve Name",
        "  30,                      !- Standard Case Fan Power per Unit Length {W/m}",
        "  30,                      !- Operating Case Fan Power per Unit Length {W/m}",
        "  20,                      !- Standard Case Lighting Power per Unit Length {W/m}",
        "  20,                      !- Installed Case Lighting Power per Unit Length {W/m}",
        "  ,                        !- Case Lighting Schedule Name",
        "  1,                       !- Fraction of Lighting Energy to Case",
        "  0,                       !- Case Anti-Sweat Heater Power per Unit Length {W/m}",
        "  0,                       !- Minimum Anti-Sweat Heater Power per Unit Length {W/m}",
        "  None,                    !- Anti-Sweat Heater Control Type",
        "  -10,                     !- Humidity at Zero Anti-Sweat Heater Energy {percent}",
        "  1.5,                     !- Case Height {m}",
        "  1,                       !- Fraction of Anti-Sweat Heater Energy to Case",
        "  0,                       !- Case Defrost Power per Unit Length {W/m}",
        "  OffCycle,                !- Case Defrost Type",
        "  AlwaysOn,                !- Case Defrost Schedule Name",
        "  ,                        !- Case Defrost Drip-Down Schedule Name",
        "  None,                    !- Defrost Energy Correction Curve Type",
        "  ,                        !- Defrost Energy Correction Curve Name",
        "  0,                       !- Under Case HVAC Return Air Fraction",
        "  ,                        !- Refrigerated Case Restocking Schedule Name",
        "  ,                        !- Case Credit Fraction Schedule Name",
        "  -7,                      !- Design Evaporator Temperature or Brine Inlet Temperature {C}",
        "  0;                       !- Average Refrigerant Charge Inventory {kg/m}",

        "Refrigeration:WalkIn,",
        "  Refrig Cond AC WalkIn,   !- Name",
        "  ,                        !- Availability Schedule Name",
        "  4744,                    !- Rated Coil Cooling Capacity {W}",
        "  4,                       !- Operating Temperature {C}",
        "  -6,                      !- Rated Cooling Source Temperature {C}",
        "  0,                       !- Rated Total Heating Power {W}",
        "  ,                        !- Heating Power Schedule Name",
        "  308,                     !- Rated Cooling Coil Fan Power {W}",
        "  0,                       !- Rated Circulation Fan Power {W}",
        "  100,                     !- Rated Total Lighting Power {W}",
        "  ,                        !- Lighting Schedule Name",
        "  Electric,                !- Defrost Type",
        "  TimeSchedule,            !- Defrost Control Type",
        "  AlwaysOn,                !- Defrost Schedule Name",
        "  ,                        !- Defrost Drip-Down Schedule Name",
        "  2400,                    !- Defrost Power {W}",
        "  ,                        !- Temperature Termination Defrost Fraction to Ice {dimensionless}",
        "  ,                        !- Restocking Schedule Name",
        "  0,                       !- Average Refrigerant Charge Inventory {kg}",
        "  13,                      !- Insulated Floor Surface Area {m2}",
        "  0.25,                    !- Insulated Floor U-Value {W/m2-K}",
        "  Refrig Cond AC Zone,     !- Zone 1 Name",
        "  55,                      !- Total Insulated Surface Area Facing Zone 1 {m2}",
        "  0.3,                     !- Insulated Surface U-Value Facing Zone 1 {W/m2-K}",
        "  0,                       !- Area of Glass Reach In Doors Facing Zone 1 {m2}",
        "  1.5,                     !- Height of Glass Reach In Doors Facing Zone 1 {m}",
        "  1.136,                   !- Glass Reach In Door U Value Facing Zone 1 {W/m2-K}",
        "  ,                        !- Glass Reach In Door Opening Schedule Name Facing Zone 1",
        "  2,                       !- Area of Stocking Doors Facing Zone 1 {m2}",
        "  2,                       !- Height of Stocking Doors Facing Zone 1 {m}",
        "  0.3785,                  !- Stocking Door U Value Facing Zone 1 {W/m2-K}",
        "  ,                        !- Stocking Door Opening Schedule Name Facing Zone 1",
        "  None;                    !- Stocking Door Opening Protection Type Facing Zone 1",

        "Refrigeration:Condenser:AirCooled,",
        "  Refrig Cond AC Refrig Condenser,  !- Name",
        "  Condenser Curve,         !- Rated Effective Total Heat Rejection Rate Curve Name",
        "  0,                       !- Rated Subcooling Temperature Difference {deltaC}",
        "  VariableSpeed,           !- Condenser Fan Speed Control Type",
        "  2000,                    !- Rated Fan Power {W}",
        "  ,                        !- Minimum Fan Air Flow Ratio {dimensionless}",
        "  Refrig Cond AC Outdoor Air Node,  !- Air Inlet Node Name or Zone Name",
        "  General,                 !- End-Use Subcategory",
        "  0,                       !- Condenser Refrigerant Operating Charge Inventory {kg}",
        "  0,                       !- Condensate Receiver Refrigerant Inventory {kg}",
        "  0;                       !- Condensate Piping Refrigerant Inventory {kg}",

        "OutdoorAir:Node,",
        "  Refrig Cond AC Outdoor Air Node;  !- Name",

        "Refrigeration:Compressor,",
        "  Refrig Cond AC Refrig Compressor,  !- Name",
        "  Generic Curve,           !- Refrigeration Compressor Power Curve Name",
        "  Capacity Curve,          !- Refrigeration Compressor Capacity Curve Name",
        "  ,                        !- Rated Superheat {deltaC}",
        "  18.3,                    !- Rated Return Gas Temperature {C}",
        "  ,                        !- Rated Liquid Temperature {C}",
        "  0,                       !- Rated Subcooling {deltaC}",
        "  General,                 !- End-Use Subcategory",
        "  Subcritical;             !- Mode of Operation",

        "Zone,",
        "  Refrig Cond WC Zone,     !- Name",
        "  ,                        !- Direction of Relative North {deg}",
        "  0,                       !- X Origin {m}",
        "  0,                       !- Y Origin {m}",
        "  0,                       !- Z Origin {m}",
        "  ,                        !- Type",
        "  1,                       !- Multiplier",
        "  ,                        !- Ceiling Height {m}",
        "  ,                        !- Volume {m3}",
        "  ,                        !- Floor Area {m2}",
        "  ,                        !- Zone Inside Convection Algorithm",
        "  ,                        !- Zone Outside Convection Algorithm",
        "  Yes;                     !- Part of Total Floor Area",

        "ZoneHVAC:EquipmentConnections,",
        "  Refrig Cond WC Zone,     !- Zone Name",
        "  Refrig Cond WC Zone Equipment List,  !- Zone Conditioning Equipment List Name",
        "  Refrig Cond WC Zone Inlet Node,  !- Zone Air Inlet Node or NodeList Name",
        "  Refrig Cond WC Zone Exhaust Node,  !- Zone Air Exhaust Node or NodeList Name",
        "  Refrig Cond WC Zone Air Node,  !- Zone Air Node Name",
        "  Refrig Cond WC Zone Return Node;  !- Zone Return Air Node or NodeList Name",

        "ZoneHVAC:EquipmentList,",
        "  Refrig Cond WC Zone Equipment List,  !- Name",
        "  SequentialLoad,          !- Load Distribution Scheme",
        "  ZoneHVAC:AirDistributionUnit,  !- Zone Equipment 1 Object Type",
        "  Refrig Cond WC ADU,      !- Zone Equipment 1 Name",
        "  1,                       !- Zone Equipment 1 Cooling Sequence",
        "  1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 1 Sequential Cooling Fraction Schedule Name",
        "  ;                        !- Zone Equipment 1 Sequential Heating Fraction Schedule Name",

        "ZoneHVAC:AirDistributionUnit,",
        "  Refrig Cond WC ADU,      !- Name",
        "  Refrig Cond WC Zone Inlet Node,  !- Air Distribution Unit Outlet Node Name",
        "  AirTerminal:SingleDuct:ConstantVolume:NoReheat,  !- Air Terminal Object Type",
        "  Refrig Cond WC Air Terminal;  !- Air Terminal Name",

        "AirTerminal:SingleDuct:ConstantVolume:NoReheat,",
        "  Refrig Cond WC Air Terminal,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  Refrig Cond WC Air Terminal Inlet Node,  !- Air Inlet Node Name",
        "  Refrig Cond WC Zone Inlet Node,  !- Air Outlet Node Name",
        "  Autosize;                !- Maximum Air Flow Rate {m3/s}",

        "AirLoopHVAC,",
        "  Refrig Cond WC PTAC,     !- Name",
        "  ,                        !- Controller List Name",
        "  ,                        !- Availability Manager List Name",
        "  Autosize,                !- Design Supply Air Flow Rate {m3/s}",
        "  Refrig Cond WC PTAC Supply Branch List,  !- Branch List Name",
        "  ,                        !- Connector List Name",
        "  Refrig Cond WC PTAC Supply Side Inlet Node,  !- Supply Side Inlet Node Name",
        "  Refrig Cond WC PTAC Demand Side Outlet Node,  !- Demand Side Outlet Node Name",
        "  Refrig Cond WC PTAC Demand Side Inlet Node,  !- Demand Side Inlet Node Names",
        "  Refrig Cond WC PTAC Supply Side Outlet Node,  !- Supply Side Outlet Node Names",
        "  1;                       !- Design Return Air Flow Fraction of Supply Air Flow",

        "BranchList,",
        "  Refrig Cond WC PTAC Supply Branch List,  !- Name",
        "  Refrig Cond WC PTAC Branch;  !- Branch 1 Name",

        "Branch,",
        "  Refrig Cond WC PTAC Branch,  !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  AirLoopHVAC:OutdoorAirSystem,  !- Component 1 Object Type",
        "  Refrig Cond WC PTAC OA System,  !- Component 1 Name",
        "  Refrig Cond WC PTAC Supply Side Inlet Node,  !- Component 1 Inlet Node Name",
        "  Refrig Cond WC PTAC Mixed Air Node,  !- Component 1 Outlet Node Name",
        "  Coil:Cooling:DX:SingleSpeed,  !- Component 2 Object Type",
        "  Refrig Cond WC PTAC Cooling Coil,  !- Component 2 Name",
        "  Refrig Cond WC PTAC Mixed Air Node,  !- Component 2 Inlet Node Name",
        "  Refrig Cond WC PTAC Desuperheater Inlet Node,  !- Component 2 Outlet Node Name",
        "  Coil:Heating:Desuperheater,  !- Component 3 Object Type",
        "  Refrig Cond WC PTAC Desuperheater,  !- Component 3 Name",
        "  Refrig Cond WC PTAC Desuperheater Inlet Node,  !- Component 3 Inlet Node Name",
        "  Refrig Cond WC PTAC Desuperheater Outlet Node,  !- Component 3 Outlet Node Name",
        "  Coil:Heating:Fuel,       !- Component 4 Object Type",
        "  Refrig Cond WC PTAC Heating Coil,  !- Component 4 Name",
        "  Refrig Cond WC PTAC Desuperheater Outlet Node,  !- Component 4 Inlet Node Name",
        "  Refrig Cond WC PTAC Heating Coil Outlet Node,  !- Component 4 Outlet Node Name",
        "  Fan:ConstantVolume,      !- Component 5 Object Type",
        "  Refrig Cond WC PTAC Fan,  !- Component 5 Name",
        "  Refrig Cond WC PTAC Heating Coil Outlet Node,  !- Component 5 Inlet Node Name",
        "  Refrig Cond WC PTAC Supply Side Outlet Node;  !- Component 5 Outlet Node Name",

        "AirLoopHVAC:OutdoorAirSystem,",
        "  Refrig Cond WC PTAC OA System,  !- Name",
        "  Refrig Cond WC PTAC OA System Controller List,  !- Controller List Name",
        "  Refrig Cond WC PTAC OA System Equipment List;  !- Outdoor Air Equipment List Name",

        "AirLoopHVAC:ControllerList,",
        "  Refrig Cond WC PTAC OA System Controller List,  !- Name",
        "  Controller:OutdoorAir,   !- Controller 1 Object Type",
        "  Refrig Cond WC PTAC OA Controller;  !- Controller 1 Name",

        "Controller:OutdoorAir,",
        "  Refrig Cond WC PTAC OA Controller,  !- Name",
        "  Refrig Cond WC PTAC Relief Node,  !- Relief Air Outlet Node Name",
        "  Refrig Cond WC PTAC Supply Side Inlet Node,  !- Return Air Node Name",
        "  Refrig Cond WC PTAC Mixed Air Node,  !- Mixed Air Node Name",
        "  Refrig Cond WC PTAC OA Node,  !- Actuator Node Name",
        "  0,                       !- Minimum Outdoor Air Flow Rate {m3/s}",
        "  Autosize,                !- Maximum Outdoor Air Flow Rate {m3/s}",
        "  NoEconomizer,            !- Economizer Control Type",
        "  ModulateFlow,            !- Economizer Control Action Type",
        "  28,                      !- Economizer Maximum Limit Dry-Bulb Temperature {C}",
        "  64000,                   !- Economizer Maximum Limit Enthalpy {J/kg}",
        "  ,                        !- Economizer Maximum Limit Dewpoint Temperature {C}",
        "  ,                        !- Electronic Enthalpy Limit Curve Name",
        "  -100,                    !- Economizer Minimum Limit Dry-Bulb Temperature {C}",
        "  NoLockout,               !- Lockout Type",
        "  FixedMinimum,            !- Minimum Limit Type",
        "  ,                        !- Minimum Outdoor Air Schedule Name",
        "  ,                        !- Minimum Fraction of Outdoor Air Schedule Name",
        "  ,                        !- Maximum Fraction of Outdoor Air Schedule Name",
        "  Refrig Cond WC PTAC MV Controller,  !- Mechanical Ventilation Controller Name",
        "  ,                        !- Time of Day Economizer Control Schedule Name",
        "  No,                      !- High Humidity Control",
        "  ,                        !- Humidistat Control Zone Name",
        "  ,                        !- High Humidity Outdoor Air Flow Ratio",
        "  Yes,                     !- Control High Indoor Humidity Based on Outdoor Humidity Ratio",
        "  BypassWhenWithinEconomizerLimits,  !- Heat Recovery Bypass Control Type",
        "  InterlockedWithMechanicalCooling;  !- Economizer Operation Staging",

        "OutdoorAir:NodeList,",
        "  Refrig Cond WC PTAC OA Node;  !- Node or NodeList Name 1",

        "Controller:MechanicalVentilation,",
        "  Refrig Cond WC PTAC MV Controller,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  No,                      !- Demand Controlled Ventilation",
        "  ZoneSum,                 !- System Outdoor Air Method",
        "  ,                        !- Zone Maximum Outdoor Air Fraction {dimensionless}",
        "  Refrig Cond WC Zone,     !- Zone or ZoneList 1 Name",
        "  ,                        !- Design Specification Outdoor Air Object Name 1",
        "  ;                        !- Design Specification Zone Air Distribution Object Name 1",

        "AirLoopHVAC:OutdoorAirSystem:EquipmentList,",
        "  Refrig Cond WC PTAC OA System Equipment List,  !- Name",
        "  OutdoorAir:Mixer,        !- Component 1 Object Type",
        "  Refrig Cond WC PTAC OA System Outdoor Air Mixer;  !- Component 1 Name",

        "OutdoorAir:Mixer,",
        "  Refrig Cond WC PTAC OA System Outdoor Air Mixer,  !- Name",
        "  Refrig Cond WC PTAC Mixed Air Node,  !- Mixed Air Node Name",
        "  Refrig Cond WC PTAC OA Node,  !- Outdoor Air Stream Node Name",
        "  Refrig Cond WC PTAC Relief Node,  !- Relief Air Stream Node Name",
        "  Refrig Cond WC PTAC Supply Side Inlet Node;  !- Return Air Stream Node Name",

        "Coil:Cooling:DX:SingleSpeed,",
        "  Refrig Cond WC PTAC Cooling Coil,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  Autosize,                !- Gross Rated Total Cooling Capacity {W}",
        "  Autosize,                !- Gross Rated Sensible Heat Ratio",
        "  3,                       !- Gross Rated Cooling COP {W/W}",
        "  Autosize,                !- Rated Air Flow Rate {m3/s}",
        "  773.3,                   !- 2017 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  934.4,                   !- 2023 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  Refrig Cond WC PTAC Mixed Air Node,  !- Air Inlet Node Name",
        "  Refrig Cond WC PTAC Desuperheater Inlet Node,  !- Air Outlet Node Name",
        "  Generic Curve,           !- Total Cooling Capacity Function of Temperature Curve Name",
        "  Generic Curve,           !- Total Cooling Capacity Function of Flow Fraction Curve Name",
        "  Generic Curve,           !- Energy Input Ratio Function of Temperature Curve Name",
        "  Generic Curve,           !- Energy Input Ratio Function of Flow Fraction Curve Name",
        "  Generic Curve,           !- Part Load Fraction Correlation Curve Name",
        "  -25,                     !- Minimum Outdoor Dry-Bulb Temperature for Compressor Operation {C}",
        "  0,                       !- Nominal Time for Condensate Removal to Begin {s}",
        "  0,                       !- Ratio of Initial Moisture Evaporation Rate and Steady State Latent Capacity {dimensionless}",
        "  0,                       !- Maximum Cycling Rate {cycles/hr}",
        "  0,                       !- Latent Capacity Time Constant {s}",
        "  ,                        !- Condenser Air Inlet Node Name",
        "  AirCooled,               !- Condenser Type",
        "  0.9,                     !- Evaporative Condenser Effectiveness {dimensionless}",
        "  Autosize,                !- Evaporative Condenser Air Flow Rate {m3/s}",
        "  Autosize,                !- Evaporative Condenser Pump Rated Power Consumption {W}",
        "  0,                       !- Crankcase Heater Capacity {W}",
        "  ,                        !- Crankcase Heater Capacity Function of Temperature Curve Name",
        "  10,                      !- Maximum Outdoor Dry-Bulb Temperature for Crankcase Heater Operation {C}",
        "  ,                        !- Supply Water Storage Tank Name",
        "  ,                        !- Condensate Collection Water Storage Tank Name",
        "  0,                       !- Basin Heater Capacity {W/K}",
        "  2;                       !- Basin Heater Setpoint Temperature {C}",

        "Coil:Heating:Desuperheater,",
        "  Refrig Cond WC PTAC Desuperheater,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  0.25,                    !- Heat Reclaim Recovery Efficiency",
        "  Refrig Cond WC PTAC Desuperheater Inlet Node,  !- Air Inlet Node Name",
        "  Refrig Cond WC PTAC Desuperheater Outlet Node,  !- Air Outlet Node Name",
        "  Refrigeration:Condenser:WaterCooled,  !- Heating Source Object Type",
        "  Refrig Cond WC Refrig Condenser,  !- Heating Source Name",
        "  Refrig Cond WC PTAC Desuperheater Outlet Node;  !- Temperature Setpoint Node Name",

        "Coil:Heating:Fuel,",
        "  Refrig Cond WC PTAC Heating Coil,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  NaturalGas,              !- Fuel Type",
        "  0.9,                     !- Burner Efficiency",
        "  Autosize,                !- Nominal Capacity {W}",
        "  Refrig Cond WC PTAC Desuperheater Outlet Node,  !- Air Inlet Node Name",
        "  Refrig Cond WC PTAC Heating Coil Outlet Node,  !- Air Outlet Node Name",
        "  Refrig Cond WC PTAC Heating Coil Outlet Node,  !- Temperature Setpoint Node Name",
        "  0,                       !- On Cycle Parasitic Electric Load {W}",
        "  ,                        !- Part Load Fraction Correlation Curve Name",
        "  0;                       !- Off Cycle Parasitic Fuel Load {W}",

        "Fan:ConstantVolume,",
        "  Refrig Cond WC PTAC Fan,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  0.7,                     !- Fan Total Efficiency",
        "  500,                     !- Pressure Rise {Pa}",
        "  Autosize,                !- Maximum Flow Rate {m3/s}",
        "  0.9,                     !- Motor Efficiency",
        "  1,                       !- Motor In Airstream Fraction",
        "  Refrig Cond WC PTAC Heating Coil Outlet Node,  !- Air Inlet Node Name",
        "  Refrig Cond WC PTAC Supply Side Outlet Node;  !- Air Outlet Node Name",

        "Refrigeration:System,",
        "  Refrig Cond WC Refrig System,  !- Name",
        "  Refrig Cond WC CaseAndWalkInList,  !- Refrigerated Case or Walkin or CaseAndWalkInList Name",
        "  ,                        !- Refrigeration Transfer Load or TransferLoad List Name",
        "  Refrig Cond WC Refrig Condenser,  !- Refrigeration Condenser Name",
        "  Refrig Cond WC Refrig Compressor,  !- Compressor or CompressorList Name",
        "  20,                      !- Minimum Condensing Temperature {C}",
        "  R407a,                   !- Refrigeration System Working Fluid Type",
        "  ConstantSuctionTemperature,  !- Suction Temperature Control Type",
        "  ,                        !- Mechanical Subcooler Name",
        "  ,                        !- Liquid Suction Heat Exchanger Subcooler Name",
        "  0,                       !- Sum UA Suction Piping {W/K}",
        "  Refrig Cond WC Zone,     !- Suction Piping Zone Name",
        "  General,                 !- End-Use Subcategory",
        "  1,                       !- Number of Compressor Stages",
        "  None,                    !- Intercooler Type",
        "  0.8;                     !- Shell-and-Coil Intercooler Effectiveness",

        "Refrigeration:CaseAndWalkInList,",
        "  Refrig Cond WC CaseAndWalkInList,  !- Name",
        "  Refrig Cond WC Case,     !- Case or WalkIn 1 Name",
        "  Refrig Cond WC WalkIn;   !- Case or WalkIn 2 Name",

        "Refrigeration:Case,",
        "  Refrig Cond WC Case,     !- Name",
        "  ,                        !- Availability Schedule Name",
        "  Refrig Cond WC Zone,     !- Zone Name",
        "  24,                      !- Rated Ambient Temperature {C}",
        "  55,                      !- Rated Ambient Relative Humidity {percent}",
        "  1406,                    !- Rated Total Cooling Capacity per Unit Length {W/m}",
        "  0.3,                     !- Rated Latent Heat Ratio",
        "  0.85,                    !- Rated Runtime Fraction",
        "  2.19,                    !- Case Length {m}",
        "  4,                       !- Case Operating Temperature {C}",
        "  CaseTemperatureMethod,   !- Latent Case Credit Curve Type",
        "  Generic Curve,           !- Latent Case Credit Curve Name",
        "  30,                      !- Standard Case Fan Power per Unit Length {W/m}",
        "  30,                      !- Operating Case Fan Power per Unit Length {W/m}",
        "  20,                      !- Standard Case Lighting Power per Unit Length {W/m}",
        "  20,                      !- Installed Case Lighting Power per Unit Length {W/m}",
        "  ,                        !- Case Lighting Schedule Name",
        "  1,                       !- Fraction of Lighting Energy to Case",
        "  0,                       !- Case Anti-Sweat Heater Power per Unit Length {W/m}",
        "  0,                       !- Minimum Anti-Sweat Heater Power per Unit Length {W/m}",
        "  None,                    !- Anti-Sweat Heater Control Type",
        "  -10,                     !- Humidity at Zero Anti-Sweat Heater Energy {percent}",
        "  1.5,                     !- Case Height {m}",
        "  1,                       !- Fraction of Anti-Sweat Heater Energy to Case",
        "  0,                       !- Case Defrost Power per Unit Length {W/m}",
        "  OffCycle,                !- Case Defrost Type",
        "  AlwaysOn,                !- Case Defrost Schedule Name",
        "  ,                        !- Case Defrost Drip-Down Schedule Name",
        "  None,                    !- Defrost Energy Correction Curve Type",
        "  ,                        !- Defrost Energy Correction Curve Name",
        "  0,                       !- Under Case HVAC Return Air Fraction",
        "  ,                        !- Refrigerated Case Restocking Schedule Name",
        "  ,                        !- Case Credit Fraction Schedule Name",
        "  -7,                      !- Design Evaporator Temperature or Brine Inlet Temperature {C}",
        "  0;                       !- Average Refrigerant Charge Inventory {kg/m}",

        "Refrigeration:WalkIn,",
        "  Refrig Cond WC WalkIn,   !- Name",
        "  ,                        !- Availability Schedule Name",
        "  4744,                    !- Rated Coil Cooling Capacity {W}",
        "  4,                       !- Operating Temperature {C}",
        "  -6,                      !- Rated Cooling Source Temperature {C}",
        "  0,                       !- Rated Total Heating Power {W}",
        "  ,                        !- Heating Power Schedule Name",
        "  308,                     !- Rated Cooling Coil Fan Power {W}",
        "  0,                       !- Rated Circulation Fan Power {W}",
        "  100,                     !- Rated Total Lighting Power {W}",
        "  ,                        !- Lighting Schedule Name",
        "  Electric,                !- Defrost Type",
        "  TimeSchedule,            !- Defrost Control Type",
        "  AlwaysOn,                !- Defrost Schedule Name",
        "  ,                        !- Defrost Drip-Down Schedule Name",
        "  2400,                    !- Defrost Power {W}",
        "  ,                        !- Temperature Termination Defrost Fraction to Ice {dimensionless}",
        "  ,                        !- Restocking Schedule Name",
        "  0,                       !- Average Refrigerant Charge Inventory {kg}",
        "  13,                      !- Insulated Floor Surface Area {m2}",
        "  0.25,                    !- Insulated Floor U-Value {W/m2-K}",
        "  Refrig Cond WC Zone,     !- Zone 1 Name",
        "  55,                      !- Total Insulated Surface Area Facing Zone 1 {m2}",
        "  0.3,                     !- Insulated Surface U-Value Facing Zone 1 {W/m2-K}",
        "  0,                       !- Area of Glass Reach In Doors Facing Zone 1 {m2}",
        "  1.5,                     !- Height of Glass Reach In Doors Facing Zone 1 {m}",
        "  1.136,                   !- Glass Reach In Door U Value Facing Zone 1 {W/m2-K}",
        "  ,                        !- Glass Reach In Door Opening Schedule Name Facing Zone 1",
        "  2,                       !- Area of Stocking Doors Facing Zone 1 {m2}",
        "  2,                       !- Height of Stocking Doors Facing Zone 1 {m}",
        "  0.3785,                  !- Stocking Door U Value Facing Zone 1 {W/m2-K}",
        "  ,                        !- Stocking Door Opening Schedule Name Facing Zone 1",
        "  None;                    !- Stocking Door Opening Protection Type Facing Zone 1",

        "Refrigeration:Condenser:WaterCooled,",
        "  Refrig Cond WC Refrig Condenser,  !- Name",
        "  58000.0,                 !- Rated Effective Total Heat Rejection Rate {W}",
        "  29.4,                    !- Rated Condensing Temperature {C}",
        "  0.0,                     !- Rated Subcooling Temperature Difference {deltaC}",
        "  10.0,                    !- Rated Water Inlet Temperature {C}",
        "  Refrig Cond WC Water Inlet Node,  !- Water Inlet Node Name",
        "  Refrig Cond WC Water Outlet Node,  !- Water Outlet Node Name",
        "  ConstantFlow,            !- Water-Cooled Loop Flow Type",
        "  ,                        !- Water Outlet Temperature Schedule Name",
        "  0.003,                   !- Water Design Flow Rate {m3/s}",
        "  0.003,                   !- Water Maximum Flow Rate {m3/s}",
        "  55.0,                    !- Water Maximum Water Outlet Temperature {C}",
        "  ,                        !- Water Minimum Water Inlet Temperature {C}",
        "  ,                        !- End-Use Subcategory",
        "  30.0,                    !- Condenser Refrigerant Operating Charge Inventory {kg}",
        "  65.0,                    !- Condensate Receiver Refrigerant Inventory {kg}",
        "  20.0;                    !- Condensate Piping Refrigerant Inventory {kg}",

        "OutdoorAir:Node,",
        "  Refrig Cond WC Outdoor Air Node;  !- Name",

        "Refrigeration:Compressor,",
        "  Refrig Cond WC Refrig Compressor,  !- Name",
        "  Generic Curve,           !- Refrigeration Compressor Power Curve Name",
        "  Capacity Curve,          !- Refrigeration Compressor Capacity Curve Name",
        "  ,                        !- Rated Superheat {deltaC}",
        "  18.3,                    !- Rated Return Gas Temperature {C}",
        "  ,                        !- Rated Liquid Temperature {C}",
        "  0,                       !- Rated Subcooling {deltaC}",
        "  General,                 !- End-Use Subcategory",
        "  Subcritical;             !- Mode of Operation",

        "Zone,",
        "  Refrig Cond EC Zone,     !- Name",
        "  ,                        !- Direction of Relative North {deg}",
        "  0,                       !- X Origin {m}",
        "  0,                       !- Y Origin {m}",
        "  0,                       !- Z Origin {m}",
        "  ,                        !- Type",
        "  1,                       !- Multiplier",
        "  ,                        !- Ceiling Height {m}",
        "  ,                        !- Volume {m3}",
        "  ,                        !- Floor Area {m2}",
        "  ,                        !- Zone Inside Convection Algorithm",
        "  ,                        !- Zone Outside Convection Algorithm",
        "  Yes;                     !- Part of Total Floor Area",

        "ZoneHVAC:EquipmentConnections,",
        "  Refrig Cond EC Zone,     !- Zone Name",
        "  Refrig Cond EC Zone Equipment List,  !- Zone Conditioning Equipment List Name",
        "  Refrig Cond EC Zone Inlet Node,  !- Zone Air Inlet Node or NodeList Name",
        "  Refrig Cond EC Zone Exhaust Node,  !- Zone Air Exhaust Node or NodeList Name",
        "  Refrig Cond EC Zone Air Node,  !- Zone Air Node Name",
        "  Refrig Cond EC Zone Return Node;  !- Zone Return Air Node or NodeList Name",

        "ZoneHVAC:EquipmentList,",
        "  Refrig Cond EC Zone Equipment List,  !- Name",
        "  SequentialLoad,          !- Load Distribution Scheme",
        "  ZoneHVAC:AirDistributionUnit,  !- Zone Equipment 1 Object Type",
        "  Refrig Cond EC ADU,      !- Zone Equipment 1 Name",
        "  1,                       !- Zone Equipment 1 Cooling Sequence",
        "  1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 1 Sequential Cooling Fraction Schedule Name",
        "  ;                        !- Zone Equipment 1 Sequential Heating Fraction Schedule Name",

        "ZoneHVAC:AirDistributionUnit,",
        "  Refrig Cond EC ADU,      !- Name",
        "  Refrig Cond EC Zone Inlet Node,  !- Air Distribution Unit Outlet Node Name",
        "  AirTerminal:SingleDuct:ConstantVolume:NoReheat,  !- Air Terminal Object Type",
        "  Refrig Cond EC Air Terminal;  !- Air Terminal Name",

        "AirTerminal:SingleDuct:ConstantVolume:NoReheat,",
        "  Refrig Cond EC Air Terminal,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  Refrig Cond EC Air Terminal Inlet Node,  !- Air Inlet Node Name",
        "  Refrig Cond EC Zone Inlet Node,  !- Air Outlet Node Name",
        "  Autosize;                !- Maximum Air Flow Rate {m3/s}",

        "AirLoopHVAC,",
        "  Refrig Cond EC PTAC,     !- Name",
        "  ,                        !- Controller List Name",
        "  ,                        !- Availability Manager List Name",
        "  Autosize,                !- Design Supply Air Flow Rate {m3/s}",
        "  Refrig Cond EC PTAC Supply Branch List,  !- Branch List Name",
        "  ,                        !- Connector List Name",
        "  Refrig Cond EC PTAC Supply Side Inlet Node,  !- Supply Side Inlet Node Name",
        "  Refrig Cond EC PTAC Demand Side Outlet Node,  !- Demand Side Outlet Node Name",
        "  Refrig Cond EC PTAC Demand Side Inlet Node,  !- Demand Side Inlet Node Names",
        "  Refrig Cond EC PTAC Supply Side Outlet Node,  !- Supply Side Outlet Node Names",
        "  1;                       !- Design Return Air Flow Fraction of Supply Air Flow",

        "BranchList,",
        "  Refrig Cond EC PTAC Supply Branch List,  !- Name",
        "  Refrig Cond EC PTAC Branch;  !- Branch 1 Name",

        "Branch,",
        "  Refrig Cond EC PTAC Branch,  !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  AirLoopHVAC:OutdoorAirSystem,  !- Component 1 Object Type",
        "  Refrig Cond EC PTAC OA System,  !- Component 1 Name",
        "  Refrig Cond EC PTAC Supply Side Inlet Node,  !- Component 1 Inlet Node Name",
        "  Refrig Cond EC PTAC Mixed Air Node,  !- Component 1 Outlet Node Name",
        "  Coil:Cooling:DX:SingleSpeed,  !- Component 2 Object Type",
        "  Refrig Cond EC PTAC Cooling Coil,  !- Component 2 Name",
        "  Refrig Cond EC PTAC Mixed Air Node,  !- Component 2 Inlet Node Name",
        "  Refrig Cond EC PTAC Desuperheater Inlet Node,  !- Component 2 Outlet Node Name",
        "  Coil:Heating:Desuperheater,  !- Component 3 Object Type",
        "  Refrig Cond EC PTAC Desuperheater,  !- Component 3 Name",
        "  Refrig Cond EC PTAC Desuperheater Inlet Node,  !- Component 3 Inlet Node Name",
        "  Refrig Cond EC PTAC Desuperheater Outlet Node,  !- Component 3 Outlet Node Name",
        "  Coil:Heating:Fuel,       !- Component 4 Object Type",
        "  Refrig Cond EC PTAC Heating Coil,  !- Component 4 Name",
        "  Refrig Cond EC PTAC Desuperheater Outlet Node,  !- Component 4 Inlet Node Name",
        "  Refrig Cond EC PTAC Heating Coil Outlet Node,  !- Component 4 Outlet Node Name",
        "  Fan:ConstantVolume,      !- Component 5 Object Type",
        "  Refrig Cond EC PTAC Fan,  !- Component 5 Name",
        "  Refrig Cond EC PTAC Heating Coil Outlet Node,  !- Component 5 Inlet Node Name",
        "  Refrig Cond EC PTAC Supply Side Outlet Node;  !- Component 5 Outlet Node Name",

        "AirLoopHVAC:OutdoorAirSystem,",
        "  Refrig Cond EC PTAC OA System,  !- Name",
        "  Refrig Cond EC PTAC OA System Controller List,  !- Controller List Name",
        "  Refrig Cond EC PTAC OA System Equipment List;  !- Outdoor Air Equipment List Name",

        "AirLoopHVAC:ControllerList,",
        "  Refrig Cond EC PTAC OA System Controller List,  !- Name",
        "  Controller:OutdoorAir,   !- Controller 1 Object Type",
        "  Refrig Cond EC PTAC OA Controller;  !- Controller 1 Name",

        "Controller:OutdoorAir,",
        "  Refrig Cond EC PTAC OA Controller,  !- Name",
        "  Refrig Cond EC PTAC Relief Node,  !- Relief Air Outlet Node Name",
        "  Refrig Cond EC PTAC Supply Side Inlet Node,  !- Return Air Node Name",
        "  Refrig Cond EC PTAC Mixed Air Node,  !- Mixed Air Node Name",
        "  Refrig Cond EC PTAC OA Node,  !- Actuator Node Name",
        "  0,                       !- Minimum Outdoor Air Flow Rate {m3/s}",
        "  Autosize,                !- Maximum Outdoor Air Flow Rate {m3/s}",
        "  NoEconomizer,            !- Economizer Control Type",
        "  ModulateFlow,            !- Economizer Control Action Type",
        "  28,                      !- Economizer Maximum Limit Dry-Bulb Temperature {C}",
        "  64000,                   !- Economizer Maximum Limit Enthalpy {J/kg}",
        "  ,                        !- Economizer Maximum Limit Dewpoint Temperature {C}",
        "  ,                        !- Electronic Enthalpy Limit Curve Name",
        "  -100,                    !- Economizer Minimum Limit Dry-Bulb Temperature {C}",
        "  NoLockout,               !- Lockout Type",
        "  FixedMinimum,            !- Minimum Limit Type",
        "  ,                        !- Minimum Outdoor Air Schedule Name",
        "  ,                        !- Minimum Fraction of Outdoor Air Schedule Name",
        "  ,                        !- Maximum Fraction of Outdoor Air Schedule Name",
        "  Refrig Cond EC PTAC MV Controller,  !- Mechanical Ventilation Controller Name",
        "  ,                        !- Time of Day Economizer Control Schedule Name",
        "  No,                      !- High Humidity Control",
        "  ,                        !- Humidistat Control Zone Name",
        "  ,                        !- High Humidity Outdoor Air Flow Ratio",
        "  Yes,                     !- Control High Indoor Humidity Based on Outdoor Humidity Ratio",
        "  BypassWhenWithinEconomizerLimits,  !- Heat Recovery Bypass Control Type",
        "  InterlockedWithMechanicalCooling;  !- Economizer Operation Staging",

        "OutdoorAir:NodeList,",
        "  Refrig Cond EC PTAC OA Node;  !- Node or NodeList Name 1",

        "Controller:MechanicalVentilation,",
        "  Refrig Cond EC PTAC MV Controller,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  No,                      !- Demand Controlled Ventilation",
        "  ZoneSum,                 !- System Outdoor Air Method",
        "  ,                        !- Zone Maximum Outdoor Air Fraction {dimensionless}",
        "  Refrig Cond EC Zone,     !- Zone or ZoneList 1 Name",
        "  ,                        !- Design Specification Outdoor Air Object Name 1",
        "  ;                        !- Design Specification Zone Air Distribution Object Name 1",

        "AirLoopHVAC:OutdoorAirSystem:EquipmentList,",
        "  Refrig Cond EC PTAC OA System Equipment List,  !- Name",
        "  OutdoorAir:Mixer,        !- Component 1 Object Type",
        "  Refrig Cond EC PTAC OA System Outdoor Air Mixer;  !- Component 1 Name",

        "OutdoorAir:Mixer,",
        "  Refrig Cond EC PTAC OA System Outdoor Air Mixer,  !- Name",
        "  Refrig Cond EC PTAC Mixed Air Node,  !- Mixed Air Node Name",
        "  Refrig Cond EC PTAC OA Node,  !- Outdoor Air Stream Node Name",
        "  Refrig Cond EC PTAC Relief Node,  !- Relief Air Stream Node Name",
        "  Refrig Cond EC PTAC Supply Side Inlet Node;  !- Return Air Stream Node Name",

        "Coil:Cooling:DX:SingleSpeed,",
        "  Refrig Cond EC PTAC Cooling Coil,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  Autosize,                !- Gross Rated Total Cooling Capacity {W}",
        "  Autosize,                !- Gross Rated Sensible Heat Ratio",
        "  3,                       !- Gross Rated Cooling COP {W/W}",
        "  Autosize,                !- Rated Air Flow Rate {m3/s}",
        "  773.3,                   !- 2017 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  934.4,                   !- 2023 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  Refrig Cond EC PTAC Mixed Air Node,  !- Air Inlet Node Name",
        "  Refrig Cond EC PTAC Desuperheater Inlet Node,  !- Air Outlet Node Name",
        "  Generic Curve,           !- Total Cooling Capacity Function of Temperature Curve Name",
        "  Generic Curve,           !- Total Cooling Capacity Function of Flow Fraction Curve Name",
        "  Generic Curve,           !- Energy Input Ratio Function of Temperature Curve Name",
        "  Generic Curve,           !- Energy Input Ratio Function of Flow Fraction Curve Name",
        "  Generic Curve,           !- Part Load Fraction Correlation Curve Name",
        "  -25,                     !- Minimum Outdoor Dry-Bulb Temperature for Compressor Operation {C}",
        "  0,                       !- Nominal Time for Condensate Removal to Begin {s}",
        "  0,                       !- Ratio of Initial Moisture Evaporation Rate and Steady State Latent Capacity {dimensionless}",
        "  0,                       !- Maximum Cycling Rate {cycles/hr}",
        "  0,                       !- Latent Capacity Time Constant {s}",
        "  ,                        !- Condenser Air Inlet Node Name",
        "  AirCooled,               !- Condenser Type",
        "  0.9,                     !- Evaporative Condenser Effectiveness {dimensionless}",
        "  Autosize,                !- Evaporative Condenser Air Flow Rate {m3/s}",
        "  Autosize,                !- Evaporative Condenser Pump Rated Power Consumption {W}",
        "  0,                       !- Crankcase Heater Capacity {W}",
        "  ,                        !- Crankcase Heater Capacity Function of Temperature Curve Name",
        "  10,                      !- Maximum Outdoor Dry-Bulb Temperature for Crankcase Heater Operation {C}",
        "  ,                        !- Supply Water Storage Tank Name",
        "  ,                        !- Condensate Collection Water Storage Tank Name",
        "  0,                       !- Basin Heater Capacity {W/K}",
        "  2;                       !- Basin Heater Setpoint Temperature {C}",

        "Coil:Heating:Desuperheater,",
        "  Refrig Cond EC PTAC Desuperheater,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  0.25,                    !- Heat Reclaim Recovery Efficiency",
        "  Refrig Cond EC PTAC Desuperheater Inlet Node,  !- Air Inlet Node Name",
        "  Refrig Cond EC PTAC Desuperheater Outlet Node,  !- Air Outlet Node Name",
        "  Refrigeration:Condenser:EvaporativeCooled,  !- Heating Source Object Type",
        "  Refrig Cond EC Refrig Condenser,  !- Heating Source Name",
        "  Refrig Cond EC PTAC Desuperheater Outlet Node;  !- Temperature Setpoint Node Name",

        "Coil:Heating:Fuel,",
        "  Refrig Cond EC PTAC Heating Coil,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  NaturalGas,              !- Fuel Type",
        "  0.9,                     !- Burner Efficiency",
        "  Autosize,                !- Nominal Capacity {W}",
        "  Refrig Cond EC PTAC Desuperheater Outlet Node,  !- Air Inlet Node Name",
        "  Refrig Cond EC PTAC Heating Coil Outlet Node,  !- Air Outlet Node Name",
        "  Refrig Cond EC PTAC Heating Coil Outlet Node,  !- Temperature Setpoint Node Name",
        "  0,                       !- On Cycle Parasitic Electric Load {W}",
        "  ,                        !- Part Load Fraction Correlation Curve Name",
        "  0;                       !- Off Cycle Parasitic Fuel Load {W}",

        "Fan:ConstantVolume,",
        "  Refrig Cond EC PTAC Fan,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  0.7,                     !- Fan Total Efficiency",
        "  500,                     !- Pressure Rise {Pa}",
        "  Autosize,                !- Maximum Flow Rate {m3/s}",
        "  0.9,                     !- Motor Efficiency",
        "  1,                       !- Motor In Airstream Fraction",
        "  Refrig Cond EC PTAC Heating Coil Outlet Node,  !- Air Inlet Node Name",
        "  Refrig Cond EC PTAC Supply Side Outlet Node;  !- Air Outlet Node Name",

        "Refrigeration:System,",
        "  Refrig Cond EC Refrig System,  !- Name",
        "  Refrig Cond EC CaseAndWalkInList,  !- Refrigerated Case or Walkin or CaseAndWalkInList Name",
        "  ,                        !- Refrigeration Transfer Load or TransferLoad List Name",
        "  Refrig Cond EC Refrig Condenser,  !- Refrigeration Condenser Name",
        "  Refrig Cond EC Refrig Compressor,  !- Compressor or CompressorList Name",
        "  20,                      !- Minimum Condensing Temperature {C}",
        "  R407a,                   !- Refrigeration System Working Fluid Type",
        "  ConstantSuctionTemperature,  !- Suction Temperature Control Type",
        "  ,                        !- Mechanical Subcooler Name",
        "  ,                        !- Liquid Suction Heat Exchanger Subcooler Name",
        "  0,                       !- Sum UA Suction Piping {W/K}",
        "  Refrig Cond EC Zone,     !- Suction Piping Zone Name",
        "  General,                 !- End-Use Subcategory",
        "  1,                       !- Number of Compressor Stages",
        "  None,                    !- Intercooler Type",
        "  0.8;                     !- Shell-and-Coil Intercooler Effectiveness",

        "Refrigeration:CaseAndWalkInList,",
        "  Refrig Cond EC CaseAndWalkInList,  !- Name",
        "  Refrig Cond EC Case,     !- Case or WalkIn 1 Name",
        "  Refrig Cond EC WalkIn;   !- Case or WalkIn 2 Name",

        "Refrigeration:Case,",
        "  Refrig Cond EC Case,     !- Name",
        "  ,                        !- Availability Schedule Name",
        "  Refrig Cond EC Zone,     !- Zone Name",
        "  24,                      !- Rated Ambient Temperature {C}",
        "  55,                      !- Rated Ambient Relative Humidity {percent}",
        "  1406,                    !- Rated Total Cooling Capacity per Unit Length {W/m}",
        "  0.3,                     !- Rated Latent Heat Ratio",
        "  0.85,                    !- Rated Runtime Fraction",
        "  2.19,                    !- Case Length {m}",
        "  4,                       !- Case Operating Temperature {C}",
        "  CaseTemperatureMethod,   !- Latent Case Credit Curve Type",
        "  Generic Curve,           !- Latent Case Credit Curve Name",
        "  30,                      !- Standard Case Fan Power per Unit Length {W/m}",
        "  30,                      !- Operating Case Fan Power per Unit Length {W/m}",
        "  20,                      !- Standard Case Lighting Power per Unit Length {W/m}",
        "  20,                      !- Installed Case Lighting Power per Unit Length {W/m}",
        "  ,                        !- Case Lighting Schedule Name",
        "  1,                       !- Fraction of Lighting Energy to Case",
        "  0,                       !- Case Anti-Sweat Heater Power per Unit Length {W/m}",
        "  0,                       !- Minimum Anti-Sweat Heater Power per Unit Length {W/m}",
        "  None,                    !- Anti-Sweat Heater Control Type",
        "  -10,                     !- Humidity at Zero Anti-Sweat Heater Energy {percent}",
        "  1.5,                     !- Case Height {m}",
        "  1,                       !- Fraction of Anti-Sweat Heater Energy to Case",
        "  0,                       !- Case Defrost Power per Unit Length {W/m}",
        "  OffCycle,                !- Case Defrost Type",
        "  AlwaysOn,                !- Case Defrost Schedule Name",
        "  ,                        !- Case Defrost Drip-Down Schedule Name",
        "  None,                    !- Defrost Energy Correction Curve Type",
        "  ,                        !- Defrost Energy Correction Curve Name",
        "  0,                       !- Under Case HVAC Return Air Fraction",
        "  ,                        !- Refrigerated Case Restocking Schedule Name",
        "  ,                        !- Case Credit Fraction Schedule Name",
        "  -7,                      !- Design Evaporator Temperature or Brine Inlet Temperature {C}",
        "  0;                       !- Average Refrigerant Charge Inventory {kg/m}",

        "Refrigeration:WalkIn,",
        "  Refrig Cond EC WalkIn,   !- Name",
        "  ,                        !- Availability Schedule Name",
        "  4744,                    !- Rated Coil Cooling Capacity {W}",
        "  4,                       !- Operating Temperature {C}",
        "  -6,                      !- Rated Cooling Source Temperature {C}",
        "  0,                       !- Rated Total Heating Power {W}",
        "  ,                        !- Heating Power Schedule Name",
        "  308,                     !- Rated Cooling Coil Fan Power {W}",
        "  0,                       !- Rated Circulation Fan Power {W}",
        "  100,                     !- Rated Total Lighting Power {W}",
        "  ,                        !- Lighting Schedule Name",
        "  Electric,                !- Defrost Type",
        "  TimeSchedule,            !- Defrost Control Type",
        "  AlwaysOn,                !- Defrost Schedule Name",
        "  ,                        !- Defrost Drip-Down Schedule Name",
        "  2400,                    !- Defrost Power {W}",
        "  ,                        !- Temperature Termination Defrost Fraction to Ice {dimensionless}",
        "  ,                        !- Restocking Schedule Name",
        "  0,                       !- Average Refrigerant Charge Inventory {kg}",
        "  13,                      !- Insulated Floor Surface Area {m2}",
        "  0.25,                    !- Insulated Floor U-Value {W/m2-K}",
        "  Refrig Cond EC Zone,     !- Zone 1 Name",
        "  55,                      !- Total Insulated Surface Area Facing Zone 1 {m2}",
        "  0.3,                     !- Insulated Surface U-Value Facing Zone 1 {W/m2-K}",
        "  0,                       !- Area of Glass Reach In Doors Facing Zone 1 {m2}",
        "  1.5,                     !- Height of Glass Reach In Doors Facing Zone 1 {m}",
        "  1.136,                   !- Glass Reach In Door U Value Facing Zone 1 {W/m2-K}",
        "  ,                        !- Glass Reach In Door Opening Schedule Name Facing Zone 1",
        "  2,                       !- Area of Stocking Doors Facing Zone 1 {m2}",
        "  2,                       !- Height of Stocking Doors Facing Zone 1 {m}",
        "  0.3785,                  !- Stocking Door U Value Facing Zone 1 {W/m2-K}",
        "  ,                        !- Stocking Door Opening Schedule Name Facing Zone 1",
        "  None;                    !- Stocking Door Opening Protection Type Facing Zone 1",

        "Refrigeration:Condenser:EvaporativeCooled,",
        "  Refrig Cond EC Refrig Condenser,  !- Name",
        "  107900.0,                !- Rated Effective Total Heat Rejection Rate {W}",
        "  0.0,                     !- Rated Subcooling Temperature Difference {deltaC}",
        "  VariableSpeed,           !- Fan Speed Control Type",
        "  1119.0,                  !- Rated Fan Power {W}",
        "  ,                        !- Minimum Fan Air Flow Ratio {dimensionless}",
        "  6.63,                    !- Approach Temperature Constant Term {C}",
        "  0.468,                   !- Approach Temperature Coefficient 2 {C}",
        "  17.93,                   !- Approach Temperature Coefficient 3 {C}",
        "  -0.322,                  !- Approach Temperature Coefficient 4 {dimensionless}",
        "  0.6,                     !- Minimum Capacity Factor {dimensionless}",
        "  4.8,                     !- Maximum Capacity Factor {dimensionless}",
        "  Refrig Cond EC Outdoor Air Node,  !- Air Inlet Node Name",
        "  3.16,                    !- Rated Air Flow Rate {m3/s}",
        "  66.7,                    !- Basin Heater Capacity {W/K}",
        "  2.0,                     !- Basin Heater Setpoint Temperature {C}",
        "  250.0,                   !- Rated Water Pump Power {W}",
        "  ,                        !- Evaporative Water Supply Tank Name",
        "  ,                        !- Evaporative Condenser Availability Schedule Name",
        "  ,                        !- End-Use Subcategory",
        "  38.5,                    !- Condenser Refrigerant Operating Charge Inventory {kg}",
        "  20.0,                    !- Condensate Receiver Refrigerant Inventory {kg}",
        "  25.0;                    !- Condensate Piping Refrigerant Inventory {kg}",

        "OutdoorAir:Node,",
        "  Refrig Cond EC Outdoor Air Node;  !- Name",

        "Refrigeration:Compressor,",
        "  Refrig Cond EC Refrig Compressor,  !- Name",
        "  Generic Curve,           !- Refrigeration Compressor Power Curve Name",
        "  Capacity Curve,          !- Refrigeration Compressor Capacity Curve Name",
        "  ,                        !- Rated Superheat {deltaC}",
        "  18.3,                    !- Rated Return Gas Temperature {C}",
        "  ,                        !- Rated Liquid Temperature {C}",
        "  0,                       !- Rated Subcooling {deltaC}",
        "  General,                 !- End-Use Subcategory",
        "  Subcritical;             !- Mode of Operation",

        "Zone,",
        "  Refrig Comp Rack Zone,   !- Name",
        "  ,                        !- Direction of Relative North {deg}",
        "  0,                       !- X Origin {m}",
        "  0,                       !- Y Origin {m}",
        "  0,                       !- Z Origin {m}",
        "  ,                        !- Type",
        "  1,                       !- Multiplier",
        "  ,                        !- Ceiling Height {m}",
        "  ,                        !- Volume {m3}",
        "  ,                        !- Floor Area {m2}",
        "  ,                        !- Zone Inside Convection Algorithm",
        "  ,                        !- Zone Outside Convection Algorithm",
        "  Yes;                     !- Part of Total Floor Area",

        "ZoneHVAC:EquipmentConnections,",
        "  Refrig Comp Rack Zone,   !- Zone Name",
        "  Refrig Comp Rack Zone Equipment List,  !- Zone Conditioning Equipment List Name",
        "  Refrig Comp Rack Zone Inlet Node,  !- Zone Air Inlet Node or NodeList Name",
        "  Refrig Comp Rack Zone Exhaust Node,  !- Zone Air Exhaust Node or NodeList Name",
        "  Refrig Comp Rack Zone Air Node,  !- Zone Air Node Name",
        "  Refrig Comp Rack Zone Return Node;  !- Zone Return Air Node or NodeList Name",

        "ZoneHVAC:EquipmentList,",
        "  Refrig Comp Rack Zone Equipment List,  !- Name",
        "  SequentialLoad,          !- Load Distribution Scheme",
        "  ZoneHVAC:AirDistributionUnit,  !- Zone Equipment 1 Object Type",
        "  Refrig Comp Rack ADU,    !- Zone Equipment 1 Name",
        "  1,                       !- Zone Equipment 1 Cooling Sequence",
        "  1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 1 Sequential Cooling Fraction Schedule Name",
        "  ;                        !- Zone Equipment 1 Sequential Heating Fraction Schedule Name",

        "ZoneHVAC:AirDistributionUnit,",
        "  Refrig Comp Rack ADU,    !- Name",
        "  Refrig Comp Rack Zone Inlet Node,  !- Air Distribution Unit Outlet Node Name",
        "  AirTerminal:SingleDuct:ConstantVolume:NoReheat,  !- Air Terminal Object Type",
        "  Refrig Comp Rack Air Terminal;  !- Air Terminal Name",

        "AirTerminal:SingleDuct:ConstantVolume:NoReheat,",
        "  Refrig Comp Rack Air Terminal,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  Refrig Comp Rack Air Terminal Inlet Node,  !- Air Inlet Node Name",
        "  Refrig Comp Rack Zone Inlet Node,  !- Air Outlet Node Name",
        "  Autosize;                !- Maximum Air Flow Rate {m3/s}",

        "AirLoopHVAC,",
        "  Refrig Comp Rack PTAC,   !- Name",
        "  ,                        !- Controller List Name",
        "  ,                        !- Availability Manager List Name",
        "  Autosize,                !- Design Supply Air Flow Rate {m3/s}",
        "  Refrig Comp Rack PTAC Supply Branch List,  !- Branch List Name",
        "  ,                        !- Connector List Name",
        "  Refrig Comp Rack PTAC Supply Side Inlet Node,  !- Supply Side Inlet Node Name",
        "  Refrig Comp Rack PTAC Demand Side Outlet Node,  !- Demand Side Outlet Node Name",
        "  Refrig Comp Rack PTAC Demand Side Inlet Node,  !- Demand Side Inlet Node Names",
        "  Refrig Comp Rack PTAC Supply Side Outlet Node,  !- Supply Side Outlet Node Names",
        "  1;                       !- Design Return Air Flow Fraction of Supply Air Flow",

        "BranchList,",
        "  Refrig Comp Rack PTAC Supply Branch List,  !- Name",
        "  Refrig Comp Rack PTAC Branch;  !- Branch 1 Name",

        "Branch,",
        "  Refrig Comp Rack PTAC Branch,  !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  AirLoopHVAC:OutdoorAirSystem,  !- Component 1 Object Type",
        "  Refrig Comp Rack PTAC OA System,  !- Component 1 Name",
        "  Refrig Comp Rack PTAC Supply Side Inlet Node,  !- Component 1 Inlet Node Name",
        "  Refrig Comp Rack PTAC Mixed Air Node,  !- Component 1 Outlet Node Name",
        "  Coil:Cooling:DX:SingleSpeed,  !- Component 2 Object Type",
        "  Refrig Comp Rack PTAC Cooling Coil,  !- Component 2 Name",
        "  Refrig Comp Rack PTAC Mixed Air Node,  !- Component 2 Inlet Node Name",
        "  Refrig Comp Rack PTAC Desuperheater Inlet Node,  !- Component 2 Outlet Node Name",
        "  Coil:Heating:Desuperheater,  !- Component 3 Object Type",
        "  Refrig Comp Rack PTAC Desuperheater,  !- Component 3 Name",
        "  Refrig Comp Rack PTAC Desuperheater Inlet Node,  !- Component 3 Inlet Node Name",
        "  Refrig Comp Rack PTAC Desuperheater Outlet Node,  !- Component 3 Outlet Node Name",
        "  Coil:Heating:Fuel,       !- Component 4 Object Type",
        "  Refrig Comp Rack PTAC Heating Coil,  !- Component 4 Name",
        "  Refrig Comp Rack PTAC Desuperheater Outlet Node,  !- Component 4 Inlet Node Name",
        "  Refrig Comp Rack PTAC Heating Coil Outlet Node,  !- Component 4 Outlet Node Name",
        "  Fan:ConstantVolume,      !- Component 5 Object Type",
        "  Refrig Comp Rack PTAC Fan,  !- Component 5 Name",
        "  Refrig Comp Rack PTAC Heating Coil Outlet Node,  !- Component 5 Inlet Node Name",
        "  Refrig Comp Rack PTAC Supply Side Outlet Node;  !- Component 5 Outlet Node Name",

        "AirLoopHVAC:OutdoorAirSystem,",
        "  Refrig Comp Rack PTAC OA System,  !- Name",
        "  Refrig Comp Rack PTAC OA System Controller List,  !- Controller List Name",
        "  Refrig Comp Rack PTAC OA System Equipment List;  !- Outdoor Air Equipment List Name",

        "AirLoopHVAC:ControllerList,",
        "  Refrig Comp Rack PTAC OA System Controller List,  !- Name",
        "  Controller:OutdoorAir,   !- Controller 1 Object Type",
        "  Refrig Comp Rack PTAC OA Controller;  !- Controller 1 Name",

        "Controller:OutdoorAir,",
        "  Refrig Comp Rack PTAC OA Controller,  !- Name",
        "  Refrig Comp Rack PTAC Relief Node,  !- Relief Air Outlet Node Name",
        "  Refrig Comp Rack PTAC Supply Side Inlet Node,  !- Return Air Node Name",
        "  Refrig Comp Rack PTAC Mixed Air Node,  !- Mixed Air Node Name",
        "  Refrig Comp Rack PTAC OA Node,  !- Actuator Node Name",
        "  0,                       !- Minimum Outdoor Air Flow Rate {m3/s}",
        "  Autosize,                !- Maximum Outdoor Air Flow Rate {m3/s}",
        "  NoEconomizer,            !- Economizer Control Type",
        "  ModulateFlow,            !- Economizer Control Action Type",
        "  28,                      !- Economizer Maximum Limit Dry-Bulb Temperature {C}",
        "  64000,                   !- Economizer Maximum Limit Enthalpy {J/kg}",
        "  ,                        !- Economizer Maximum Limit Dewpoint Temperature {C}",
        "  ,                        !- Electronic Enthalpy Limit Curve Name",
        "  -100,                    !- Economizer Minimum Limit Dry-Bulb Temperature {C}",
        "  NoLockout,               !- Lockout Type",
        "  FixedMinimum,            !- Minimum Limit Type",
        "  ,                        !- Minimum Outdoor Air Schedule Name",
        "  ,                        !- Minimum Fraction of Outdoor Air Schedule Name",
        "  ,                        !- Maximum Fraction of Outdoor Air Schedule Name",
        "  Refrig Comp Rack PTAC MV Controller,  !- Mechanical Ventilation Controller Name",
        "  ,                        !- Time of Day Economizer Control Schedule Name",
        "  No,                      !- High Humidity Control",
        "  ,                        !- Humidistat Control Zone Name",
        "  ,                        !- High Humidity Outdoor Air Flow Ratio",
        "  Yes,                     !- Control High Indoor Humidity Based on Outdoor Humidity Ratio",
        "  BypassWhenWithinEconomizerLimits,  !- Heat Recovery Bypass Control Type",
        "  InterlockedWithMechanicalCooling;  !- Economizer Operation Staging",

        "OutdoorAir:NodeList,",
        "  Refrig Comp Rack PTAC OA Node;  !- Node or NodeList Name 1",

        "Controller:MechanicalVentilation,",
        "  Refrig Comp Rack PTAC MV Controller,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  No,                      !- Demand Controlled Ventilation",
        "  ZoneSum,                 !- System Outdoor Air Method",
        "  ,                        !- Zone Maximum Outdoor Air Fraction {dimensionless}",
        "  Refrig Comp Rack Zone,   !- Zone or ZoneList 1 Name",
        "  ,                        !- Design Specification Outdoor Air Object Name 1",
        "  ;                        !- Design Specification Zone Air Distribution Object Name 1",

        "AirLoopHVAC:OutdoorAirSystem:EquipmentList,",
        "  Refrig Comp Rack PTAC OA System Equipment List,  !- Name",
        "  OutdoorAir:Mixer,        !- Component 1 Object Type",
        "  Refrig Comp Rack PTAC OA System Outdoor Air Mixer;  !- Component 1 Name",

        "OutdoorAir:Mixer,",
        "  Refrig Comp Rack PTAC OA System Outdoor Air Mixer,  !- Name",
        "  Refrig Comp Rack PTAC Mixed Air Node,  !- Mixed Air Node Name",
        "  Refrig Comp Rack PTAC OA Node,  !- Outdoor Air Stream Node Name",
        "  Refrig Comp Rack PTAC Relief Node,  !- Relief Air Stream Node Name",
        "  Refrig Comp Rack PTAC Supply Side Inlet Node;  !- Return Air Stream Node Name",

        "Coil:Cooling:DX:SingleSpeed,",
        "  Refrig Comp Rack PTAC Cooling Coil,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  Autosize,                !- Gross Rated Total Cooling Capacity {W}",
        "  Autosize,                !- Gross Rated Sensible Heat Ratio",
        "  3,                       !- Gross Rated Cooling COP {W/W}",
        "  Autosize,                !- Rated Air Flow Rate {m3/s}",
        "  773.3,                   !- 2017 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  934.4,                   !- 2023 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  Refrig Comp Rack PTAC Mixed Air Node,  !- Air Inlet Node Name",
        "  Refrig Comp Rack PTAC Desuperheater Inlet Node,  !- Air Outlet Node Name",
        "  Generic Curve,           !- Total Cooling Capacity Function of Temperature Curve Name",
        "  Generic Curve,           !- Total Cooling Capacity Function of Flow Fraction Curve Name",
        "  Generic Curve,           !- Energy Input Ratio Function of Temperature Curve Name",
        "  Generic Curve,           !- Energy Input Ratio Function of Flow Fraction Curve Name",
        "  Generic Curve,           !- Part Load Fraction Correlation Curve Name",
        "  -25,                     !- Minimum Outdoor Dry-Bulb Temperature for Compressor Operation {C}",
        "  0,                       !- Nominal Time for Condensate Removal to Begin {s}",
        "  0,                       !- Ratio of Initial Moisture Evaporation Rate and Steady State Latent Capacity {dimensionless}",
        "  0,                       !- Maximum Cycling Rate {cycles/hr}",
        "  0,                       !- Latent Capacity Time Constant {s}",
        "  ,                        !- Condenser Air Inlet Node Name",
        "  AirCooled,               !- Condenser Type",
        "  0.9,                     !- Evaporative Condenser Effectiveness {dimensionless}",
        "  Autosize,                !- Evaporative Condenser Air Flow Rate {m3/s}",
        "  Autosize,                !- Evaporative Condenser Pump Rated Power Consumption {W}",
        "  0,                       !- Crankcase Heater Capacity {W}",
        "  ,                        !- Crankcase Heater Capacity Function of Temperature Curve Name",
        "  10,                      !- Maximum Outdoor Dry-Bulb Temperature for Crankcase Heater Operation {C}",
        "  ,                        !- Supply Water Storage Tank Name",
        "  ,                        !- Condensate Collection Water Storage Tank Name",
        "  0,                       !- Basin Heater Capacity {W/K}",
        "  2;                       !- Basin Heater Setpoint Temperature {C}",

        "Coil:Heating:Desuperheater,",
        "  Refrig Comp Rack PTAC Desuperheater,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  0.25,                    !- Heat Reclaim Recovery Efficiency",
        "  Refrig Comp Rack PTAC Desuperheater Inlet Node,  !- Air Inlet Node Name",
        "  Refrig Comp Rack PTAC Desuperheater Outlet Node,  !- Air Outlet Node Name",
        "  Refrigeration:CompressorRack,  !- Heating Source Object Type",
        "  Refrig Comp Rack Refrig Rack,  !- Heating Source Name",
        "  Refrig Comp Rack PTAC Desuperheater Outlet Node;  !- Temperature Setpoint Node Name",

        "Coil:Heating:Fuel,",
        "  Refrig Comp Rack PTAC Heating Coil,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  NaturalGas,              !- Fuel Type",
        "  0.9,                     !- Burner Efficiency",
        "  Autosize,                !- Nominal Capacity {W}",
        "  Refrig Comp Rack PTAC Desuperheater Outlet Node,  !- Air Inlet Node Name",
        "  Refrig Comp Rack PTAC Heating Coil Outlet Node,  !- Air Outlet Node Name",
        "  Refrig Comp Rack PTAC Heating Coil Outlet Node,  !- Temperature Setpoint Node Name",
        "  0,                       !- On Cycle Parasitic Electric Load {W}",
        "  ,                        !- Part Load Fraction Correlation Curve Name",
        "  0;                       !- Off Cycle Parasitic Fuel Load {W}",

        "Fan:ConstantVolume,",
        "  Refrig Comp Rack PTAC Fan,  !- Name",
        "  ,                        !- Availability Schedule Name",
        "  0.7,                     !- Fan Total Efficiency",
        "  500,                     !- Pressure Rise {Pa}",
        "  Autosize,                !- Maximum Flow Rate {m3/s}",
        "  0.9,                     !- Motor Efficiency",
        "  1,                       !- Motor In Airstream Fraction",
        "  Refrig Comp Rack PTAC Heating Coil Outlet Node,  !- Air Inlet Node Name",
        "  Refrig Comp Rack PTAC Supply Side Outlet Node;  !- Air Outlet Node Name",

        "Refrigeration:CompressorRack,",
        "  Refrig Comp Rack Refrig Rack,  !- Name",
        "  Zone,                    !- Heat Rejection Location",
        "  4.0,                     !- Design Compressor Rack COP {W/W}",
        "  Condenser Curve,         !- Compressor Rack COP Function of Temperature Curve Name",
        "  175.0,                   !- Design Condenser Fan Power {W}",
        "  ,                        !- Condenser Fan Power Function of Temperature Curve Name",
        "  AirCooled,               !- Condenser Type",
        "  ,                        !- Water-Cooled Condenser Inlet Node Name",
        "  ,                        !- Water-Cooled Condenser Outlet Node Name",
        "  ,                        !- Water-Cooled Loop Flow Type",
        "  ,                        !- Water-Cooled Condenser Outlet Temperature Schedule Name",
        "  ,                        !- Water-Cooled Condenser Design Flow Rate {m3/s}",
        "  ,                        !- Water-Cooled Condenser Maximum Flow Rate {m3/s}",
        "  ,                        !- Water-Cooled Condenser Maximum Water Outlet Temperature {C}",
        "  ,                        !- Water-Cooled Condenser Minimum Water Inlet Temperature {C}",
        "  ,                        !- Evaporative Condenser Availability Schedule Name",
        "  ,                        !- Evaporative Condenser Effectiveness {dimensionless}",
        "  ,                        !- Evaporative Condenser Air Flow Rate {m3/s}",
        "  ,                        !- Basin Heater Capacity {W/K}",
        "  ,                        !- Basin Heater Setpoint Temperature {C}",
        "  ,                        !- Design Evaporative Condenser Water Pump Power {W}",
        "  ,                        !- Evaporative Water Supply Tank Name",
        "  ,                        !- Condenser Air Inlet Node Name",
        "  ,                        !- End-Use Subcategory",
        "  Refrig Comp Rack Case;   !- Refrigeration Case Name or WalkIn Name or CaseAndWalkInList Name",

        "Refrigeration:Case,",
        "  Refrig Comp Rack Case,   !- Name",
        "  ,                        !- Availability Schedule Name",
        "  Refrig Comp Rack Zone,   !- Zone Name",
        "  24,                      !- Rated Ambient Temperature {C}",
        "  55,                      !- Rated Ambient Relative Humidity {percent}",
        "  1406,                    !- Rated Total Cooling Capacity per Unit Length {W/m}",
        "  0.3,                     !- Rated Latent Heat Ratio",
        "  0.85,                    !- Rated Runtime Fraction",
        "  2.19,                    !- Case Length {m}",
        "  4,                       !- Case Operating Temperature {C}",
        "  CaseTemperatureMethod,   !- Latent Case Credit Curve Type",
        "  Generic Curve,           !- Latent Case Credit Curve Name",
        "  30,                      !- Standard Case Fan Power per Unit Length {W/m}",
        "  30,                      !- Operating Case Fan Power per Unit Length {W/m}",
        "  20,                      !- Standard Case Lighting Power per Unit Length {W/m}",
        "  20,                      !- Installed Case Lighting Power per Unit Length {W/m}",
        "  ,                        !- Case Lighting Schedule Name",
        "  1,                       !- Fraction of Lighting Energy to Case",
        "  0,                       !- Case Anti-Sweat Heater Power per Unit Length {W/m}",
        "  0,                       !- Minimum Anti-Sweat Heater Power per Unit Length {W/m}",
        "  None,                    !- Anti-Sweat Heater Control Type",
        "  -10,                     !- Humidity at Zero Anti-Sweat Heater Energy {percent}",
        "  1.5,                     !- Case Height {m}",
        "  1,                       !- Fraction of Anti-Sweat Heater Energy to Case",
        "  0,                       !- Case Defrost Power per Unit Length {W/m}",
        "  OffCycle,                !- Case Defrost Type",
        "  AlwaysOn,                !- Case Defrost Schedule Name",
        "  ,                        !- Case Defrost Drip-Down Schedule Name",
        "  None,                    !- Defrost Energy Correction Curve Type",
        "  ,                        !- Defrost Energy Correction Curve Name",
        "  0,                       !- Under Case HVAC Return Air Fraction",
        "  ,                        !- Refrigerated Case Restocking Schedule Name",
        "  ,                        !- Case Credit Fraction Schedule Name",
        "  -7,                      !- Design Evaporator Temperature or Brine Inlet Temperature {C}",
        "  0;                       !- Average Refrigerant Charge Inventory {kg/m}",

        "ScheduleTypeLimits,",
        "  OnOff,                   !- Name",
        "  0,                       !- Lower Limit Value",
        "  1,                       !- Upper Limit Value",
        "  Discrete,                !- Numeric Type",
        "  Availability;            !- Unit Type",

        "Schedule:Constant,AlwaysOn,OnOff,1;",

        "Curve:Cubic,",
        "  Generic Curve,           !- Name",
        "  1,                       !- Coefficient1 Constant",
        "  0,                       !- Coefficient2 x",
        "  0,                       !- Coefficient3 x**2",
        "  0,                       !- Coefficient4 x**3",
        "  0,                       !- Minimum Value of x",
        "  1;                       !- Maximum Value of x",

        "Curve:Linear,",
        "  Condenser Curve,         !- Name",
        "  0,                       !- Coefficient1 Constant",
        "  14704,                   !- Coefficient2 x",
        "  5,                       !- Minimum Value of x",
        "  22.2;                    !- Maximum Value of x",

        "Curve:Bicubic,",
        "  Capacity Curve,          !- Name",
        "  22520,                   !- Coefficient1 Constant",
        "  755.9,                   !- Coefficient2 x",
        "  9.047,                   !- Coefficient3 x**2",
        "  -69.75,                  !- Coefficient4 y",
        "  -2.849,                  !- Coefficient5 y**2",
        "  -3.813,                  !- Coefficient6 x*y",
        "  0.02657,                 !- Coefficient7 x**3",
        "  0.01137,                 !- Coefficient8 y**3",
        "  -0.06655,                !- Coefficient9 x**2*y",
        "  -0.04638,                !- Coefficient10 x*y**2",
        "  -17.8,                   !- Minimum Value of x",
        "  4.4,                     !- Maximum Value of x",
        "  10,                      !- Minimum Value of y",
        "  48.9,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Capacity;                !- Output Unit Type",

        "FluidProperties:Name,",
        "  R407a,                   !- Fluid Name",
        "  Refrigerant;             !- Fluid Type",

        "FluidProperties:Temperatures,",
        "  R407aSaturatedTemperatures,  !- Name",
        "  -45,-40,-35,-30,-25,-20,-15,",
        "  -10,-5,0,5,10,15,20,",
        "  25,30,35,40,45,50,55,",
        "  60,65,70,75;",

        "FluidProperties:Saturated,",
        "  R407a,                   !- Fluid Name",
        "  Pressure,                !- Fluid Property Type",
        "  FluidGas,                !- Fluid Phase",
        "  R407aSaturatedTemperatures,  !- Temperature Values Name",
        "  73063.0,94480.0,120600.0,152120.0,189760.0,234300.0,286550.0,",
        "  347370.0,417660.0,498340.0,590400.0,694850.0,812750.0,945220.0,",
        "  1093400.0,1258600.0,1442000.0,1645100.0,1869300.0,2116500.0,2388400.0,",
        "  2687300.0,3016000.0,3378400.0,3780600.0;",

        "FluidProperties:Saturated,",
        "  R407a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  Fluid,                   !- Fluid Phase",
        "  R407aSaturatedTemperatures,  !- Temperature Values Name",
        "  139990.0,146440.0,152930.0,159470.0,166070.0,172720.0,179430.0,",
        "  186210.0,193060.0,200000.0,207030.0,214150.0,221380.0,228740.0,",
        "  236230.0,243860.0,251670.0,259680.0,267900.0,276400.0,285230.0,",
        "  294490.0,304340.0,315090.0,327490.0;",

        "FluidProperties:Saturated,",
        "  R407a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  FluidGas,                !- Fluid Phase",
        "  R407aSaturatedTemperatures,  !- Temperature Values Name",
        "  371820.0,374730.0,377610.0,380440.0,383220.0,385950.0,388610.0,",
        "  391210.0,393720.0,396150.0,398470.0,400690.0,402780.0,404730.0,",
        "  406510.0,408110.0,409480.0,410600.0,411410.0,411850.0,411830.0,",
        "  411210.0,409770.0,407110.0,402300.0;",

        "FluidProperties:Saturated,",
        "  R407a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  Fluid,                   !- Fluid Phase",
        "  R407aSaturatedTemperatures,  !- Temperature Values Name",
        "  1403.3,1387.4,1371.2,1354.8,1338.1,1321.1,1303.7,",
        "  1285.9,1267.7,1248.9,1229.6,1209.7,1189.0,1167.5,",
        "  1145.1,1121.6,1096.9,1070.7,1042.7,1012.5,979.62,",
        "  943.04,901.2,850.99,785.1;",

        "FluidProperties:Saturated,",
        "  R407a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  FluidGas,                !- Fluid Phase",
        "  R407aSaturatedTemperatures,  !- Temperature Values Name",
        "  3.5889,4.571,5.7544,7.1675,8.8412,10.81,13.11,",
        "  15.784,18.877,22.441,26.535,31.226,36.594,42.731,",
        "  49.749,57.782,67.001,77.62,89.921,104.29,121.26,",
        "  141.66,166.82,199.24,244.58;",

        "FluidProperties:Saturated,",
        "  R407a,                   !- Fluid Name",
        "  SpecificHeat,            !- Fluid Property Type",
        "  Fluid,                   !- Fluid Phase",
        "  R407aSaturatedTemperatures,  !- Temperature Values Name",
        "  1284.6,1292.4,1301.1,1310.7,1321.4,1333.1,1346.1,",
        "  1360.4,1376.2,1393.7,1413.3,1435.2,1459.8,1487.9,",
        "  1520.0,1557.2,1601.0,1653.6,1718.2,1800.6,1910.7,",
        "  2068.2,2317.0,2775.9,3910.5;",

        "FluidProperties:Saturated,",
        "  R407a,                   !- Fluid Name",
        "  SpecificHeat,            !- Fluid Property Type",
        "  FluidGas,                !- Fluid Phase",
        "  R407aSaturatedTemperatures,  !- Temperature Values Name",
        "  750.47,768.21,786.9,806.6,827.36,849.27,872.44,",
        "  897.01,923.14,951.03,980.96,1013.3,1048.7,1087.9,",
        "  1132.2,1183.1,1242.8,1314.4,1402.5,1514.1,1661.4,",
        "  1866.6,2176.0,2705.5,3846.7;",

        "FluidProperties:Temperatures,",
        "  R407aSuperHeatTemperatures,  !- Name",
        "  -45,-40,-35,-30,-25,-20,-15,",
        "  -10,-5,0,5,10,15,20,",
        "  25,30,35,40,45,50,55,",
        "  60,65,70,75,85,90,95,",
        "  100,105,110,115,120,125,130,",
        "  135,140,145,150,155,160,165,",
        "  170,175,180,185,190,195,200,",
        "  205;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  100000.0,                !- Pressure {Pa}",
        "  0.0,0.0,378370.0,382220.0,386080.0,389950.0,393840.0,",
        "  397760.0,401700.0,405660.0,409660.0,413690.0,417750.0,421840.0,",
        "  425970.0,430130.0,434330.0,438570.0,442830.0,447140.0,451480.0,",
        "  455860.0,460280.0,464730.0,469220.0,478310.0,482910.0,487540.0,",
        "  492220.0,496930.0,501670.0,506460.0,511280.0,516130.0,521030.0,",
        "  525950.0,530920.0,535920.0,540950.0,546020.0,551120.0,556260.0,",
        "  561440.0,566640.0,571890.0,577160.0,582470.0,587810.0,593190.0,",
        "  598600.0;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  100000.0,                !- Pressure {Pa}",
        "  0.0,0.0,4.7301,4.6173,4.5111,4.4106,4.3152,",
        "  4.2246,4.1382,4.0557,3.9768,3.9013,3.8288,3.7593,",
        "  3.6924,3.6281,3.5661,3.5063,3.4486,3.3929,3.3391,",
        "  3.287,3.2366,3.1878,3.1405,3.0502,3.007,2.9651,",
        "  2.9243,2.8847,2.8462,2.8087,2.7722,2.7367,2.7021,",
        "  2.6684,2.6355,2.6035,2.5722,2.5417,2.5119,2.4828,",
        "  2.4544,2.4267,2.3995,2.373,2.3471,2.3217,2.2969,",
        "  2.2726;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  125890.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,381350.0,385290.0,389220.0,393170.0,",
        "  397130.0,401110.0,405110.0,409140.0,413200.0,417290.0,421410.0,",
        "  425560.0,429740.0,433960.0,438210.0,442490.0,446810.0,451170.0,",
        "  455560.0,459990.0,464450.0,468950.0,478060.0,482670.0,487310.0,",
        "  491990.0,496710.0,501470.0,506260.0,511080.0,515940.0,520840.0,",
        "  525780.0,530740.0,535750.0,540790.0,545860.0,550970.0,556110.0,",
        "  561290.0,566500.0,571750.0,577030.0,582340.0,587680.0,593060.0,",
        "  598470.0;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  125890.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,5.8705,5.7301,5.5981,5.4734,",
        "  5.3552,5.243,5.1361,5.0341,4.9366,4.8432,4.7537,",
        "  4.6678,4.5853,4.5059,4.4294,4.3556,4.2845,4.2158,",
        "  4.1494,4.0852,4.023,3.9628,3.8479,3.7931,3.7398,",
        "  3.6881,3.6378,3.5889,3.5414,3.4952,3.4501,3.4063,",
        "  3.3636,3.322,3.2814,3.2418,3.2032,3.1655,3.1287,",
        "  3.0928,3.0577,3.0234,2.9899,2.9571,2.9251,2.8937,",
        "  2.863;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  158490.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,384260.0,388280.0,392290.0,",
        "  396320.0,400350.0,404400.0,408480.0,412570.0,416700.0,420850.0,",
        "  425030.0,429240.0,433480.0,437750.0,442060.0,446400.0,450770.0,",
        "  455180.0,459620.0,464100.0,468610.0,477750.0,482370.0,487020.0,",
        "  491710.0,496440.0,501200.0,506000.0,510840.0,515710.0,520610.0,",
        "  525550.0,530530.0,535540.0,540580.0,545660.0,550770.0,555920.0,",
        "  561110.0,566320.0,571570.0,576860.0,582170.0,587520.0,592900.0,",
        "  598320.0;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  158490.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,7.2985,7.1227,6.9577,",
        "  6.802,6.6548,6.5151,6.3821,6.2554,6.1344,6.0186,",
        "  5.9077,5.8013,5.6991,5.6007,5.5061,5.4148,5.3268,",
        "  5.2419,5.1598,5.0804,5.0035,4.857,4.7871,4.7193,",
        "  4.6535,4.5896,4.5275,4.4671,4.4083,4.3512,4.2955,",
        "  4.2413,4.1886,4.1371,4.0869,4.038,3.9903,3.9437,",
        "  3.8982,3.8537,3.8103,3.7679,3.7265,3.6859,3.6463,",
        "  3.6075;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  199530.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,387040.0,391160.0,",
        "  395270.0,399380.0,403500.0,407630.0,411780.0,415940.0,420140.0,",
        "  424350.0,428600.0,432870.0,437170.0,441500.0,445870.0,450270.0,",
        "  454700.0,459160.0,463650.0,468190.0,477350.0,481980.0,486650.0,",
        "  491360.0,496100.0,500870.0,505680.0,510520.0,515400.0,520320.0,",
        "  525270.0,530250.0,535270.0,540320.0,545410.0,550530.0,555680.0,",
        "  560870.0,566090.0,571350.0,576640.0,581960.0,587320.0,592700.0,",
        "  598120.0;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  199530.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,9.0925,8.8705,",
        "  8.6627,8.4671,8.2825,8.1076,7.9414,7.7832,7.6322,",
        "  7.488,7.3499,7.2175,7.0904,6.9682,6.8506,6.7373,",
        "  6.6281,6.5227,6.4209,6.3225,6.1351,6.0458,5.9592,",
        "  5.8752,5.7937,5.7145,5.6376,5.5628,5.4901,5.4193,",
        "  5.3504,5.2833,5.218,5.1543,5.0922,5.0316,4.9725,",
        "  4.9148,4.8585,4.8034,4.7497,4.6972,4.6458,4.5956,",
        "  4.5465;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  251190.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,389680.0,",
        "  393910.0,398130.0,402330.0,406540.0,410750.0,414980.0,419230.0,",
        "  423490.0,427780.0,432090.0,436430.0,440800.0,445200.0,449620.0,",
        "  454080.0,458570.0,463090.0,467640.0,476850.0,481500.0,486190.0,",
        "  490910.0,495660.0,500450.0,505270.0,510130.0,515020.0,519950.0,",
        "  524910.0,529900.0,534930.0,539990.0,545090.0,550220.0,555380.0,",
        "  560580.0,565810.0,571070.0,576370.0,581700.0,587060.0,592450.0,",
        "  597880.0;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  251190.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,11.355,",
        "  11.072,10.808,10.56,10.327,10.107,9.8974,9.6986,",
        "  9.5093,9.3285,9.1557,8.9902,8.8314,8.6789,8.5323,",
        "  8.3911,8.2551,8.1239,7.9972,7.7564,7.6418,7.5308,",
        "  7.4232,7.3189,7.2177,7.1194,7.024,6.9312,6.8409,",
        "  6.7531,6.6677,6.5844,6.5034,6.4244,6.3473,6.2722,",
        "  6.1989,6.1274,6.0575,5.9893,5.9227,5.8575,5.7938,",
        "  5.7316;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  316230.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  392110.0,396480.0,400810.0,405120.0,409430.0,413740.0,418060.0,",
        "  422390.0,426740.0,431100.0,435490.0,439910.0,444340.0,448810.0,",
        "  453300.0,457820.0,462370.0,466960.0,476210.0,480890.0,485600.0,",
        "  490340.0,495110.0,499920.0,504760.0,509630.0,514540.0,519480.0,",
        "  524460.0,529460.0,534500.0,539580.0,544690.0,549830.0,555000.0,",
        "  560210.0,565450.0,570720.0,576020.0,581360.0,586730.0,592130.0,",
        "  597570.0;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  316230.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  14.223,13.858,13.519,13.202,12.904,12.624,12.359,",
        "  12.107,11.868,11.64,11.422,11.214,11.015,10.823,",
        "  10.64,10.463,10.293,10.129,9.8174,9.6697,9.5267,",
        "  9.3884,9.2543,9.1244,8.9983,8.876,8.7571,8.6417,",
        "  8.5294,8.4202,8.3139,8.2105,8.1097,8.0115,7.9158,",
        "  7.8224,7.7313,7.6425,7.5557,7.4709,7.3881,7.3072,",
        "  7.2282;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  398110.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,394270.0,398790.0,403260.0,407700.0,412130.0,416540.0,",
        "  420970.0,425390.0,429830.0,434290.0,438760.0,443250.0,447770.0,",
        "  452310.0,456870.0,461460.0,466080.0,475400.0,480110.0,484850.0,",
        "  489620.0,494420.0,499250.0,504110.0,509000.0,513930.0,518890.0,",
        "  523880.0,528910.0,533970.0,539060.0,544180.0,549330.0,554520.0,",
        "  559740.0,564990.0,570280.0,575590.0,580940.0,586320.0,591730.0,",
        "  597180.0;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  398110.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,17.882,17.403,16.961,16.551,16.167,15.807,",
        "  15.467,15.146,14.841,14.551,14.275,14.012,13.76,",
        "  13.518,13.286,13.064,12.85,12.445,12.253,12.068,",
        "  11.889,11.715,11.548,11.385,11.228,11.075,10.926,",
        "  10.782,10.642,10.506,10.373,10.244,10.119,9.9965,",
        "  9.8773,9.761,9.6476,9.5369,9.4289,9.3234,9.2203,",
        "  9.1197;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  501190.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,400780.0,405420.0,410010.0,414570.0,",
        "  419110.0,423650.0,428190.0,432730.0,437280.0,441850.0,446430.0,",
        "  451030.0,455650.0,460300.0,464960.0,474380.0,479120.0,483900.0,",
        "  488700.0,493530.0,498390.0,503280.0,508210.0,513160.0,518140.0,",
        "  523160.0,528200.0,533280.0,538390.0,543530.0,548710.0,553910.0,",
        "  559150.0,564420.0,569720.0,575050.0,580410.0,585800.0,591230.0,",
        "  596680.0;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  501190.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,21.949,21.364,20.824,20.322,",
        "  19.852,19.412,18.997,18.605,18.233,17.879,17.542,",
        "  17.221,16.914,16.62,16.337,15.806,15.555,15.313,",
        "  15.079,14.854,14.636,14.425,14.221,14.023,13.831,",
        "  13.645,13.465,13.289,13.119,12.953,12.792,12.635,",
        "  12.482,12.333,12.188,12.046,11.908,11.773,11.641,",
        "  11.513;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  630960.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,402320.0,407170.0,411940.0,",
        "  416670.0,421360.0,426030.0,430700.0,435360.0,440030.0,444700.0,",
        "  449390.0,454080.0,458800.0,463530.0,473060.0,477860.0,482680.0,",
        "  487530.0,492400.0,497300.0,502230.0,507190.0,512180.0,517190.0,",
        "  522240.0,527310.0,532420.0,537550.0,542720.0,547920.0,553140.0,",
        "  558400.0,563690.0,569010.0,574360.0,579740.0,585150.0,590590.0,",
        "  596060.0;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  630960.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,27.837,27.044,26.318,",
        "  25.649,25.028,24.448,23.904,23.393,22.909,22.452,",
        "  22.017,21.603,21.209,20.832,20.125,19.793,19.474,",
        "  19.166,18.87,18.585,18.309,18.042,17.785,17.535,",
        "  17.293,17.059,16.832,16.611,16.397,16.189,15.986,",
        "  15.789,15.597,15.41,15.228,15.051,14.878,14.709,",
        "  14.544;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  794330.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,403250.0,408370.0,",
        "  413380.0,418310.0,423190.0,428030.0,432850.0,437650.0,442450.0,",
        "  447250.0,452050.0,456860.0,461680.0,471370.0,476230.0,481120.0,",
        "  486030.0,490960.0,495910.0,500890.0,505900.0,510930.0,515980.0,",
        "  521070.0,526180.0,531320.0,536490.0,541680.0,546910.0,552170.0,",
        "  557450.0,562770.0,568110.0,573480.0,578880.0,584320.0,589780.0,",
        "  595270.0;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  794330.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,35.574,34.462,",
        "  33.461,32.547,31.707,30.928,30.203,29.525,28.887,",
        "  28.286,27.717,27.178,26.666,25.711,25.265,24.838,",
        "  24.429,24.035,23.657,23.292,22.941,22.602,22.274,",
        "  21.957,21.651,21.354,21.066,20.787,20.517,20.254,",
        "  19.998,19.75,19.508,19.273,19.044,18.821,18.603,",
        "  18.391;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1000000.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  408800.0,414120.0,419320.0,424440.0,429490.0,434510.0,439490.0,",
        "  444450.0,449400.0,454340.0,459280.0,469180.0,474140.0,479110.0,",
        "  484100.0,489110.0,494130.0,499170.0,504240.0,509330.0,514440.0,",
        "  519570.0,524740.0,529920.0,535130.0,540370.0,545640.0,550930.0,",
        "  556250.0,561600.0,566970.0,572380.0,577810.0,583270.0,588760.0,",
        "  594270.0;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1000000.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  44.326,42.889,41.599,40.428,39.355,38.365,37.445,",
        "  36.588,35.784,35.028,34.314,32.997,32.387,31.806,",
        "  31.251,30.72,30.211,29.722,29.253,28.801,28.366,",
        "  27.946,27.541,27.149,26.771,26.404,26.049,25.705,",
        "  25.371,25.047,24.732,24.426,24.129,23.839,23.557,",
        "  23.283;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1258900.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,413880.0,419460.0,424900.0,430240.0,435500.0,",
        "  440710.0,445870.0,451010.0,456120.0,466320.0,471410.0,476510.0,",
        "  481600.0,486710.0,491830.0,496960.0,502110.0,507280.0,512460.0,",
        "  517660.0,522890.0,528140.0,533400.0,538700.0,544010.0,549350.0,",
        "  554720.0,560110.0,565530.0,570970.0,576440.0,581940.0,587460.0,",
        "  593010.0;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1258900.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,55.617,53.706,52.005,50.472,49.077,",
        "  47.795,46.611,45.511,44.484,42.613,41.757,40.947,",
        "  40.177,39.445,38.747,38.08,37.442,36.831,36.244,",
        "  35.68,35.137,34.614,34.109,33.623,33.152,32.697,",
        "  32.256,31.829,31.415,31.014,30.624,30.245,29.877,",
        "  29.519;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1584900.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,412140.0,418320.0,424240.0,429980.0,",
        "  435590.0,441090.0,446530.0,451910.0,462540.0,467820.0,473090.0,",
        "  478340.0,483590.0,488850.0,494100.0,499360.0,504630.0,509910.0,",
        "  515210.0,520520.0,525850.0,531190.0,536550.0,541940.0,547340.0,",
        "  552770.0,558220.0,563690.0,569190.0,574710.0,580250.0,585820.0,",
        "  591410.0;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1584900.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,73.448,70.389,67.761,65.454,",
        "  63.396,61.539,59.847,58.293,55.523,54.277,53.108,",
        "  52.009,50.971,49.988,49.056,48.169,47.324,46.516,",
        "  45.744,45.004,44.293,43.611,42.954,42.321,41.711,",
        "  41.122,40.553,40.003,39.47,38.954,38.453,37.968,",
        "  37.496;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1995300.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,415100.0,421850.0,",
        "  428230.0,434360.0,440300.0,446110.0,457450.0,463020.0,468540.0,",
        "  474020.0,479480.0,484920.0,490350.0,495770.0,501190.0,506610.0,",
        "  512030.0,517460.0,522890.0,528340.0,533800.0,539280.0,544770.0,",
        "  550280.0,555800.0,561350.0,566910.0,572500.0,578110.0,583730.0,",
        "  589380.0;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1995300.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,94.602,90.089,",
        "  86.317,83.071,80.222,77.683,73.307,71.394,69.628,",
        "  67.987,66.457,65.023,63.675,62.404,61.201,60.061,",
        "  58.976,57.943,56.957,56.014,55.111,54.245,53.413,",
        "  52.613,51.842,51.1,50.383,49.69,49.021,48.373,",
        "  47.745;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  2511900.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  416450.0,424070.0,431110.0,437760.0,450350.0,456400.0,462330.0,",
        "  468180.0,473960.0,479690.0,485380.0,491030.0,496660.0,502280.0,",
        "  507880.0,513470.0,519060.0,524650.0,530250.0,535850.0,541460.0,",
        "  547080.0,552700.0,558350.0,564000.0,569680.0,575370.0,581070.0,",
        "  586800.0;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  2511900.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  124.28,117.09,111.35,106.56,98.867,95.672,92.798,",
        "  90.187,87.796,85.592,83.55,81.647,79.868,78.197,",
        "  76.623,75.136,73.727,72.389,71.116,69.901,68.741,",
        "  67.631,66.567,65.545,64.564,63.619,62.708,61.83,",
        "  60.982;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  3162300.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,415170.0,424340.0,439820.0,446820.0,453530.0,",
        "  460020.0,466340.0,472540.0,478640.0,484660.0,490610.0,496520.0,",
        "  502390.0,508230.0,514040.0,519840.0,525620.0,531400.0,537170.0,",
        "  542940.0,548710.0,554490.0,560270.0,566060.0,571870.0,577680.0,",
        "  583500.0;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  3162300.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,168.56,155.61,138.63,132.41,127.11,",
        "  122.49,118.4,114.73,111.41,108.38,105.6,103.02,",
        "  100.63,98.397,96.305,94.337,92.48,90.724,89.059,",
        "  87.476,85.969,84.53,83.155,81.838,80.575,79.362,",
        "  78.196;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  3981100.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,421550.0,431390.0,440000.0,",
        "  447890.0,455300.0,462380.0,469210.0,475850.0,482340.0,488720.0,",
        "  495010.0,501220.0,507370.0,513470.0,519530.0,525570.0,531570.0,",
        "  537560.0,543530.0,549500.0,555460.0,561410.0,567370.0,573330.0,",
        "  579290.0;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  3981100.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,214.0,196.4,183.65,",
        "  173.65,165.43,158.47,152.44,147.13,142.4,138.13,",
        "  134.25,130.69,127.41,124.37,121.54,118.9,116.42,",
        "  114.08,111.87,109.78,107.8,105.92,104.12,102.41,",
        "  100.77;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  5011900.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,351700.0,393880.0,414340.0,",
        "  427300.0,437750.0,446920.0,455300.0,463170.0,470660.0,477870.0,",
        "  484860.0,491680.0,498370.0,504950.0,511440.0,517850.0,524210.0,",
        "  530520.0,536780.0,543020.0,549220.0,555410.0,561580.0,567740.0,",
        "  573890.0;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  5011900.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,670.01,389.44,308.17,",
        "  271.68,248.41,231.44,218.16,207.3,198.14,190.25,",
        "  183.32,177.18,171.65,166.65,162.08,157.88,154.0,",
        "  150.39,147.03,143.89,140.93,138.14,135.51,133.02,",
        "  130.65;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  6309600.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,337280.0,350110.0,366030.0,",
        "  386110.0,405600.0,420920.0,433300.0,443940.0,453490.0,462290.0,",
        "  470580.0,478460.0,486050.0,493410.0,500570.0,507580.0,514470.0,",
        "  521250.0,527950.0,534570.0,541140.0,547650.0,554130.0,560570.0,",
        "  566980.0;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  6309600.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,817.28,749.48,656.6,",
        "  537.78,439.12,378.26,338.97,311.11,289.93,273.05,",
        "  259.12,247.34,237.17,228.26,220.36,213.27,206.85,",
        "  201.01,195.65,190.7,186.12,181.85,177.87,174.13,",
        "  170.61;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  7943300.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,331580.0,341540.0,352190.0,",
        "  363740.0,376330.0,389860.0,403750.0,417110.0,429400.0,440600.0,",
        "  450870.0,460420.0,469410.0,477950.0,486130.0,494030.0,501690.0,",
        "  509170.0,516480.0,523670.0,530740.0,537710.0,544610.0,551430.0,",
        "  558200.0;",

        "FluidProperties:Superheated,",
        "  R407a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R407aSuperHeatTemperatures,  !- Temperature Values Name",
        "  7943300.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,881.81,840.36,793.28,",
        "  739.39,678.34,612.34,547.54,490.96,445.21,408.99,",
        "  380.04,356.46,336.85,320.23,305.93,293.44,282.42,",
        "  272.58,263.73,255.71,248.38,241.66,235.46,229.71,",
        "  224.36;",
    });

    EXPECT_TRUE(process_idf(idf_objects, false));
    state->init_state(*state);

    state->dataZoneEquip->ZoneEquipInputsFilled = true;
    state->dataEnvrn->OutBaroPress = 101325.0;
    bool ErrorsFound(false);

    HeatBalanceManager::GetZoneData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    DataZoneEquipment::GetZoneEquipmentData(*state);

    // ZoneAirLoopEquipmentManager::GetZoneAirLoopEquipment(*state);
    InternalHeatGains::ManageInternalHeatGains(*state, true);
    RefrigeratedCase::ManageRefrigeratedCaseRacks(*state);

    compare_err_stream("", true);

    EXPECT_EQ(state->dataRefrigCase->NumRefrigCondensers, 3);
    EXPECT_EQ(state->dataRefrigCase->NumRefrigeratedRacks, 1);
    EXPECT_EQ(state->dataRefrigCase->NumSimulationCases, 4);
    EXPECT_EQ(state->dataRefrigCase->NumSimulationWalkIns, 3);

    for (auto refrigCase : state->dataRefrigCase->RefrigCase) {
        EXPECT_GT(refrigCase.ActualZoneNum, 0);
        EXPECT_GT(refrigCase.ZoneNodeNum, 0);
    }
    for (auto refrigWalkIn : state->dataRefrigCase->WalkIn) {
        for (int zoneNum : refrigWalkIn.ZoneNum) {
            EXPECT_GT(zoneNum, 0);
        }
        for (int zoneNodeNum : refrigWalkIn.ZoneNodeNum) {
            EXPECT_GT(zoneNodeNum, 0);
        }
    }
}

TEST_F(EnergyPlusFixture, TranscriticalSystem_CapacityCorrectionUsesPostSubcoolerEnthalpy)
{
    constexpr Real64 hCompInHP = 460000.0;
    constexpr Real64 hGasCoolerOut = 300000.0;
    constexpr Real64 delHSubcoolerDis = 15000.0; // positive on discharge side in this convention
    constexpr Real64 caseEnthalpyChangeRatedMT = 120000.0;
    constexpr Real64 massCorrectionMT = 0.92;

    const Real64 totalHDeltaWithSubcooler = RefrigeratedCase::CalcTransMTActualEnthalpyChange(hCompInHP, hGasCoolerOut, delHSubcoolerDis);
    const Real64 totalHDeltaNoSubcooler = RefrigeratedCase::CalcTransMTActualEnthalpyChange(hCompInHP, hGasCoolerOut, 0.0);

    EXPECT_NEAR(totalHDeltaWithSubcooler, 145000.0, 1e-9);
    EXPECT_NEAR(totalHDeltaNoSubcooler, 160000.0, 1e-9);
    EXPECT_LT(totalHDeltaWithSubcooler, totalHDeltaNoSubcooler);

    const Real64 capCorrWithSubcooler = massCorrectionMT * totalHDeltaWithSubcooler / caseEnthalpyChangeRatedMT;
    const Real64 capCorrNoSubcooler = massCorrectionMT * totalHDeltaNoSubcooler / caseEnthalpyChangeRatedMT;

    // Expected direction: adding subcooler term reduces capacity correction
    EXPECT_LT(capCorrWithSubcooler, capCorrNoSubcooler);
}
