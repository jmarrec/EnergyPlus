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

// EnergyPlus::AirTerminal:SingleDuct:Mixer Unit Tests

// Google Test Headers
#include <gtest/gtest.h>

// ObjexxFCL Headers

#include "Fixtures/EnergyPlusFixture.hh"
#include <EnergyPlus/BranchInputManager.hh>
#include <EnergyPlus/DXCoils.hh>
#include <EnergyPlus/Data/EnergyPlusData.hh>
#include <EnergyPlus/DataDefineEquip.hh>
#include <EnergyPlus/DataEnvironment.hh>
#include <EnergyPlus/DataHVACGlobals.hh>
#include <EnergyPlus/DataHeatBalFanSys.hh>
#include <EnergyPlus/DataLoopNode.hh>
#include <EnergyPlus/DataSizing.hh>
#include <EnergyPlus/DataZoneEnergyDemands.hh>
#include <EnergyPlus/DataZoneEquipment.hh>
#include <EnergyPlus/FanCoilUnits.hh>
#include <EnergyPlus/Fans.hh>
#include <EnergyPlus/General.hh>
#include <EnergyPlus/HVACVariableRefrigerantFlow.hh>
#include <EnergyPlus/HeatBalanceManager.hh>
#include <EnergyPlus/IOFiles.hh>
#include <EnergyPlus/OutputReportPredefined.hh>
#include <EnergyPlus/Plant/DataPlant.hh>
#include <EnergyPlus/Psychrometrics.hh>
#include <EnergyPlus/ScheduleManager.hh>
#include <EnergyPlus/SimulationManager.hh>
#include <EnergyPlus/SingleDuct.hh>
#include <EnergyPlus/SizingManager.hh>
#include <EnergyPlus/SystemAvailabilityManager.hh>
#include <EnergyPlus/UnitVentilator.hh>
#include <EnergyPlus/UnitarySystem.hh>
#include <EnergyPlus/WaterCoils.hh>
#include <EnergyPlus/ZoneAirLoopEquipmentManager.hh>
#include <EnergyPlus/ZoneEquipmentManager.hh>
#include <EnergyPlus/ZoneTempPredictorCorrector.hh>

// EnergyPlus Headers
using namespace EnergyPlus::BranchInputManager;
using namespace EnergyPlus::DataDefineEquip;
using namespace EnergyPlus::DataEnvironment;
using namespace EnergyPlus::DataHeatBalFanSys;
using namespace EnergyPlus::DataPlant;
using namespace EnergyPlus::DataSizing;
using namespace EnergyPlus::DataZoneEquipment;
using namespace EnergyPlus::DXCoils;
using namespace EnergyPlus::Fans;
using namespace EnergyPlus::FanCoilUnits;
using namespace EnergyPlus::General;
using namespace EnergyPlus::HeatBalanceManager;
using namespace EnergyPlus::HVACVariableRefrigerantFlow;
using namespace EnergyPlus::OutputReportPredefined;
using namespace EnergyPlus::Psychrometrics;
using namespace EnergyPlus::SingleDuct;
using namespace EnergyPlus::UnitVentilator;
using namespace EnergyPlus::WaterCoils;
using namespace EnergyPlus::ZoneAirLoopEquipmentManager;
using namespace EnergyPlus::ZoneTempPredictorCorrector;
using namespace EnergyPlus::DataZoneEnergyDemands;

namespace EnergyPlus {

TEST_F(EnergyPlusFixture, AirTerminalSingleDuctMixer_GetInputPTAC_InletSide)
{

    bool ErrorsFound(false);

    std::string const idf_objects = delimited_string({
        "AirTerminal:SingleDuct:Mixer,",
        "  SPACE1-1 DOAS Air Terminal,  !- Name",
        "  ZoneHVAC:PackagedTerminalAirConditioner,  !- ZoneHVAC Unit Object Type",
        "  SPACE1-1 PTAC,           !- ZoneHVAC Unit Object Name",
        "  SPACE1-1 Heat Pump Inlet,  !- Mixer Outlet Node Name",
        "  SPACE1-1 Air Terminal Mixer Primary Inlet,  !- Mixer Primary Air Inlet Node Name",
        "  SPACE1-1 Air Terminal Mixer Secondary Inlet,  !- Mixer Secondary Air Inlet Node Name",
        "  InletSide;               !- Mixer Connection Type",

        "ZoneHVAC:AirDistributionUnit,",
        "  SPACE1-1 DOAS ATU,       !- Name",
        "  SPACE1-1 Heat Pump Inlet,  !- Air Distribution Unit Outlet Node Name",
        "  AirTerminal:SingleDuct:Mixer,  !- Air Terminal Object Type",
        "  SPACE1-1 DOAS Air Terminal;  !- Air Terminal Name",

        "Schedule:Compact,",
        "  FanAvailSched,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: WeekDays CustomDay1 CustomDay2,  !- Field 2",
        "  Until: 7:00,0.0,         !- Field 3",
        "  Until: 21:00,1.0,        !- Field 5",
        "  Until: 24:00,0.0,        !- Field 7",
        "  For: Weekends Holiday,   !- Field 9",
        "  Until: 24:00,0.0,        !- Field 10",
        "  For: SummerDesignDay,    !- Field 12",
        "  Until: 24:00,1.0,        !- Field 13",
        "  For: WinterDesignDay,    !- Field 15",
        "  Until: 24:00,1.0;        !- Field 16",

        "Schedule:Compact,",
        "  CyclingFanSch,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,0.0;        !- Field 3",

        "ZoneHVAC:EquipmentList,",
        "  SPACE1-1 Equipment,      !- Name",
        "  SequentialLoad,          !- Load Distribution Scheme",
        "  ZoneHVAC:AirDistributionUnit,  !- Zone Equipment 1 Object Type",
        "  SPACE1-1 DOAS ATU,       !- Zone Equipment 1 Name",
        "  1,                       !- Zone Equipment 1 Cooling Sequence",
        "  1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 1 Sequential Cooling Fraction Schedule Name",
        "  ,                        !- Zone Equipment 1 Sequential Heating Fraction Schedule Name",
        "  ZoneHVAC:PackagedTerminalAirConditioner,  !- Zone Equipment 2 Object Type",
        "  SPACE1-1 PTAC,           !- Zone Equipment 2 Name",
        "  2,                       !- Zone Equipment 2 Cooling Sequence",
        "  2,                       !- Zone Equipment 2 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 2 Sequential Cooling Fraction Schedule Name",
        "  ;                        !- Zone Equipment 2 Sequential Heating Fraction Schedule Name",

        "ZoneHVAC:PackagedTerminalAirConditioner,",
        "  SPACE1-1 PTAC,           !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  SPACE1-1 Heat Pump Inlet,  !- Air Inlet Node Name",
        "  SPACE1-1 Supply Inlet,   !- Air Outlet Node Name",
        "  ,                        !- Outdoor Air Mixer Object Type",
        "  ,                        !- Outdoor Air Mixer Name",
        "  0.3,                     !- Cooling Supply Air Flow Rate {m3/s}",
        "  0.3,                     !- Heating Supply Air Flow Rate {m3/s}",
        "  ,                        !- No Load Supply Air Flow Rate {m3/s}",
        "  ,                        !- No Load Supply Air Flow Rate Control Set To Low Speed",
        "  0.0,                     !- Cooling Outdoor Air Flow Rate {m3/s}",
        "  0.0,                     !- Heating Outdoor Air Flow Rate {m3/s}",
        "  0.0,                     !- No Load Outdoor Air Flow Rate {m3/s}",
        "  Fan:OnOff,               !- Supply Air Fan Object Type",
        "  SPACE1-1 Supply Fan,     !- Supply Air Fan Name",
        "  Coil:Heating:Fuel,       !- Heating Coil Object Type",
        "  SPACE1-1 Heating Coil,   !- Heating Coil Name",
        "  Coil:Cooling:DX:SingleSpeed,  !- Cooling Coil Object Type",
        "  SPACE1-1 PTAC CCoil,     !- Cooling Coil Name",
        "  BlowThrough,             !- Fan Placement",
        "  CyclingFanSch;           !- Supply Air Fan Operating Mode Schedule Name",

        "Fan:OnOff,",
        "  SPACE1-1 Supply Fan,     !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  0.7,                     !- Fan Total Efficiency",
        "  75.0,                    !- Pressure Rise {Pa}",
        "  0.3,                     !- Maximum Flow Rate {m3/s}",
        "  0.9,                     !- Motor Efficiency",
        "  1.0,                     !- Motor In Airstream Fraction",
        "  SPACE1-1 Heat Pump Inlet,  !- Air Inlet Node Name",
        "  SPACE1-1 Zone Unit Fan Outlet;  !- Air Outlet Node Name",

        "Coil:Heating:Fuel,",
        "  SPACE1-1 Heating Coil,   !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  NaturalGas,              !- Fuel Type",
        "  0.8,                     !- Burner Efficiency",
        "  10000.0,                 !- Nominal Capacity {W}",
        "  SPACE1-1 Cooling Coil Outlet,  !- Air Inlet Node Name",
        "  SPACE1-1 Supply Inlet;   !- Air Outlet Node Name",

        "Coil:Cooling:DX:SingleSpeed,",
        "  SPACE1-1 PTAC CCoil,     !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  6500.0,                  !- Gross Rated Total Cooling Capacity {W}",
        "  0.75,                    !- Gross Rated Sensible Heat Ratio",
        "  3.0,                     !- Gross Rated Cooling COP {W/W}",
        "  0.3,                     !- Rated Air Flow Rate {m3/s}",
        "  ,                        !- 2017 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  ,                        !- 2023 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  SPACE1-1 Zone Unit Fan Outlet,  !- Air Inlet Node Name",
        "  SPACE1-1 Cooling Coil Outlet,  !- Air Outlet Node Name",
        "  HPACCoolCapFT,           !- Total Cooling Capacity Function of Temperature Curve Name",
        "  HPACCoolCapFFF,          !- Total Cooling Capacity Function of Flow Fraction Curve Name",
        "  HPACEIRFT,               !- Energy Input Ratio Function of Temperature Curve Name",
        "  HPACEIRFFF,              !- Energy Input Ratio Function of Flow Fraction Curve Name",
        "  HPACPLFFPLR,             !- Part Load Fraction Correlation Curve Name",
        "  ,                        !- Minimum Outdoor Dry-Bulb Temperature for Compressor Operation {C}",
        "  ,                        !- Nominal Time for Condensate Removal to Begin {s}",
        "  ;                        !- Ratio of Initial Moisture Evaporation Rate and Steady State Latent Capacity {dimensionless}",

        "Curve:Quadratic,",
        "  HPACCoolCapFFF,          !- Name",
        "  0.8,                     !- Coefficient1 Constant",
        "  0.2,                     !- Coefficient2 x",
        "  0.0,                     !- Coefficient3 x**2",
        "  0.5,                     !- Minimum Value of x",
        "  1.5;                     !- Maximum Value of x",

        "Curve:Quadratic,",
        "  HPACEIRFFF,              !- Name",
        "  1.1552,                  !- Coefficient1 Constant",
        "  -0.1808,                 !- Coefficient2 x",
        "  0.0256,                  !- Coefficient3 x**2",
        "  0.5,                     !- Minimum Value of x",
        "  1.5;                     !- Maximum Value of x",

        "Curve:Quadratic,",
        "  HPACPLFFPLR,             !- Name",
        "  0.85,                    !- Coefficient1 Constant",
        "  0.15,                    !- Coefficient2 x",
        "  0.0,                     !- Coefficient3 x**2",
        "  0.0,                     !- Minimum Value of x",
        "  1.0;                     !- Maximum Value of x",

        "Curve:Cubic,",
        "  FanEffRatioCurve,        !- Name",
        "  0.33856828,              !- Coefficient1 Constant",
        "  1.72644131,              !- Coefficient2 x",
        "  -1.49280132,             !- Coefficient3 x**2",
        "  0.42776208,              !- Coefficient4 x**3",
        "  0.5,                     !- Minimum Value of x",
        "  1.5,                     !- Maximum Value of x",
        "  0.3,                     !- Minimum Curve Output",
        "  1.0;                     !- Maximum Curve Output",

        "Curve:Exponent,",
        "  FanPowerRatioCurve,      !- Name",
        "  0.0,                     !- Coefficient1 Constant",
        "  1.0,                     !- Coefficient2 Constant",
        "  3.0,                     !- Coefficient3 Constant",
        "  0.0,                     !- Minimum Value of x",
        "  1.5,                     !- Maximum Value of x",
        "  0.01,                    !- Minimum Curve Output",
        "  1.5;                     !- Maximum Curve Output",

        "Curve:Biquadratic,",
        "  HPACCoolCapFT,           !- Name",
        "  0.942587793,             !- Coefficient1 Constant",
        "  0.009543347,             !- Coefficient2 x",
        "  0.00068377,              !- Coefficient3 x**2",
        "  -0.011042676,            !- Coefficient4 y",
        "  5.249e-06,               !- Coefficient5 y**2",
        "  -9.72e-06,               !- Coefficient6 x*y",
        "  12.77778,                !- Minimum Value of x",
        "  23.88889,                !- Maximum Value of x",
        "  18.0,                    !- Minimum Value of y",
        "  46.11111,                !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Biquadratic,",
        "  HPACEIRFT,               !- Name",
        "  0.342414409,             !- Coefficient1 Constant",
        "  0.034885008,             !- Coefficient2 x",
        "  -0.0006237,              !- Coefficient3 x**2",
        "  0.004977216,             !- Coefficient4 y",
        "  0.000437951,             !- Coefficient5 y**2",
        "  -0.000728028,            !- Coefficient6 x*y",
        "  12.77778,                !- Minimum Value of x",
        "  23.88889,                !- Maximum Value of x",
        "  18.0,                    !- Minimum Value of y",
        "  46.11111,                !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Zone,",
        "  SPACE1-1,                !- Name",
        "  0.0,                     !- Direction of Relative North {deg}",
        "  0.0,                     !- X Origin {m}",
        "  0.0,                     !- Y Origin {m}",
        "  0.0,                     !- Z Origin {m}",
        "  1,                       !- Type",
        "  1,                       !- Multiplier",
        "  2.438400269,             !- Ceiling Height {m}",
        "  239.247360229;           !- Volume {m3}",

        "ZoneHVAC:EquipmentConnections,",
        "  SPACE1-1,                !- Zone Name",
        "  SPACE1-1 Equipment,      !- Zone Conditioning Equipment List Name",
        "  SPACE1-1 Inlets,         !- Zone Air Inlet Node or NodeList Name",
        "  SPACE1-1 Air Terminal Mixer Secondary Inlet,  !- Zone Air Exhaust Node or NodeList Name",
        "  SPACE1-1 Zone Air Node,  !- Zone Air Node Name",
        "  SPACE1-1 Return Outlet;  !- Zone Return Air Node or NodeList Name",

        "NodeList,",
        "  SPACE1-1 Inlets,         !- Name",
        "  SPACE1-1 Supply Inlet;   !- Node 1 Name",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    state->dataGlobal->TimeStepsInHour = 1;    // must initialize this to get schedules initialized
    state->dataGlobal->MinutesInTimeStep = 60; // must initialize this to get schedules initialized
    state->init_state(*state);

    GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    GetZoneEquipmentData(*state);
    GetZoneAirLoopEquipment(*state);
    UnitarySystems::UnitarySys thisSys;
    UnitarySystems::UnitarySys::factory(*state, HVAC::UnitarySysType::Unitary_AnyCoilType, "SPACE1-1 PTAC", true, 0);
    state->dataZoneEquip->ZoneEquipInputsFilled = true;
    thisSys.getUnitarySystemInput(*state, "SPACE1-1 PTAC", true, 0);
    state->dataUnitarySystems->getInputOnceFlag = false;

    ASSERT_EQ(1, state->dataSingleDuct->NumATMixers);
    EXPECT_EQ("SPACE1-1 DOAS AIR TERMINAL", state->dataSingleDuct->SysATMixer(1).Name);                      // single duct air terminal mixer name
    EXPECT_EQ((int)HVAC::MixerType::InletSide, (int)state->dataSingleDuct->SysATMixer(1).type);              // air terminal mixer connection type
    EXPECT_EQ("AIRTERMINAL:SINGLEDUCT:MIXER", state->dataDefineEquipment->AirDistUnit(1).EquipType(1));      // Air distribution unit equipment type
    EXPECT_EQ("ZoneHVAC:PackagedTerminalAirConditioner", state->dataUnitarySystems->unitarySys[0].UnitType); // zoneHVAC equipment type
}

TEST_F(EnergyPlusFixture, AirTerminalSingleDuctMixer_SimPTAC_ATMInletSide)
{

    bool ErrorsFound(false);
    bool FirstHVACIteration(false);
    Real64 HVACInletMassFlowRate(0.0);
    Real64 PrimaryAirMassFlowRate(0.0);
    Real64 SecondaryAirMassFlowRate(0.0);
    Real64 QUnitOut(0.0);
    Real64 QZnReq(0.0);
    int PTUnitNum(1);

    std::string const idf_objects = delimited_string({
        "AirTerminal:SingleDuct:Mixer,",
        "  SPACE1-1 DOAS Air Terminal,  !- Name",
        "  ZoneHVAC:PackagedTerminalAirConditioner,  !- ZoneHVAC Unit Object Type",
        "  SPACE1-1 PTAC,           !- ZoneHVAC Unit Object Name",
        "  SPACE1-1 Heat Pump Inlet,  !- Mixer Outlet Node Name",
        "  SPACE1-1 Air Terminal Mixer Primary Inlet,  !- Mixer Primary Air Inlet Node Name",
        "  SPACE1-1 Air Terminal Mixer Secondary Inlet,  !- Mixer Secondary Air Inlet Node Name",
        "  InletSide;               !- Mixer Connection Type",

        "ZoneHVAC:AirDistributionUnit,",
        "  SPACE1-1 DOAS ATU,       !- Name",
        "  SPACE1-1 Heat Pump Inlet,  !- Air Distribution Unit Outlet Node Name",
        "  AirTerminal:SingleDuct:Mixer,  !- Air Terminal Object Type",
        "  SPACE1-1 DOAS Air Terminal;  !- Air Terminal Name",

        "Schedule:Compact,",
        "  FanAvailSched,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,1.0;        !- Field 3",

        "Schedule:Compact,",
        "  CyclingFanSch,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,0.0;        !- Field 3",

        "ZoneHVAC:EquipmentList,",
        "  SPACE1-1 Equipment,      !- Name",
        "  SequentialLoad,          !- Load Distribution Scheme",
        "  ZoneHVAC:AirDistributionUnit,  !- Zone Equipment 1 Object Type",
        "  SPACE1-1 DOAS ATU,       !- Zone Equipment 1 Name",
        "  1,                       !- Zone Equipment 1 Cooling Sequence",
        "  1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 1 Sequential Cooling Fraction Schedule Name",
        "  ,                        !- Zone Equipment 1 Sequential Heating Fraction Schedule Name",
        "  ZoneHVAC:PackagedTerminalAirConditioner,  !- Zone Equipment 2 Object Type",
        "  SPACE1-1 PTAC,           !- Zone Equipment 2 Name",
        "  2,                       !- Zone Equipment 2 Cooling Sequence",
        "  2,                       !- Zone Equipment 2 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 2 Sequential Cooling Fraction Schedule Name",
        "  ;                        !- Zone Equipment 2 Sequential Heating Fraction Schedule Name",

        "ZoneHVAC:PackagedTerminalAirConditioner,",
        "  SPACE1-1 PTAC,           !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  SPACE1-1 Heat Pump Inlet,  !- Air Inlet Node Name",
        "  SPACE1-1 Supply Inlet,   !- Air Outlet Node Name",
        "  ,                        !- Outdoor Air Mixer Object Type",
        "  ,                        !- Outdoor Air Mixer Name",
        "  0.5,                     !- Cooling Supply Air Flow Rate {m3/s}",
        "  0.5,                     !- Heating Supply Air Flow Rate {m3/s}",
        "  ,                        !- No Load Supply Air Flow Rate {m3/s}",
        "  ,                        !- No Load Supply Air Flow Rate Control Set To Low Speed",
        "  0.0,                     !- Cooling Outdoor Air Flow Rate {m3/s}",
        "  0.0,                     !- Heating Outdoor Air Flow Rate {m3/s}",
        "  0.0,                     !- No Load Outdoor Air Flow Rate {m3/s}",
        "  Fan:OnOff,               !- Supply Air Fan Object Type",
        "  SPACE1-1 Supply Fan,     !- Supply Air Fan Name",
        "  Coil:Heating:Fuel,       !- Heating Coil Object Type",
        "  SPACE1-1 Heating Coil,   !- Heating Coil Name",
        "  Coil:Cooling:DX:SingleSpeed,  !- Cooling Coil Object Type",
        "  SPACE1-1 PTAC CCoil,     !- Cooling Coil Name",
        "  BlowThrough,             !- Fan Placement",
        "  CyclingFanSch;           !- Supply Air Fan Operating Mode Schedule Name",

        "Fan:OnOff,",
        "  SPACE1-1 Supply Fan,     !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  0.7,                     !- Fan Total Efficiency",
        "  75.0,                    !- Pressure Rise {Pa}",
        "  0.5,                     !- Maximum Flow Rate {m3/s}",
        "  0.9,                     !- Motor Efficiency",
        "  1.0,                     !- Motor In Airstream Fraction",
        "  SPACE1-1 Heat Pump Inlet,  !- Air Inlet Node Name",
        "  SPACE1-1 Zone Unit Fan Outlet;  !- Air Outlet Node Name",

        "Coil:Heating:Fuel,",
        "  SPACE1-1 Heating Coil,   !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  NaturalGas,              !- Fuel Type",
        "  0.8,                     !- Burner Efficiency",
        "  10000.0,                 !- Nominal Capacity {W}",
        "  SPACE1-1 Cooling Coil Outlet,  !- Air Inlet Node Name",
        "  SPACE1-1 Supply Inlet;   !- Air Outlet Node Name",

        "Coil:Cooling:DX:SingleSpeed,",
        "  SPACE1-1 PTAC CCoil,     !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  6680.0,                  !- Gross Rated Total Cooling Capacity {W}",
        "  0.75,                    !- Gross Rated Sensible Heat Ratio",
        "  3.0,                     !- Gross Rated Cooling COP {W/W}",
        "  0.5,                     !- Rated Air Flow Rate {m3/s}",
        "  ,                        !- 2017 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  ,                        !- 2023 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  SPACE1-1 Zone Unit Fan Outlet,  !- Air Inlet Node Name",
        "  SPACE1-1 Cooling Coil Outlet,  !- Air Outlet Node Name",
        "  HPACCoolCapFT,           !- Total Cooling Capacity Function of Temperature Curve Name",
        "  HPACCoolCapFFF,          !- Total Cooling Capacity Function of Flow Fraction Curve Name",
        "  HPACEIRFT,               !- Energy Input Ratio Function of Temperature Curve Name",
        "  HPACEIRFFF,              !- Energy Input Ratio Function of Flow Fraction Curve Name",
        "  HPACPLFFPLR,             !- Part Load Fraction Correlation Curve Name",
        "  ,                        !- Minimum Outdoor Dry-Bulb Temperature for Compressor Operation {C}",
        "  ,                        !- Nominal Time for Condensate Removal to Begin {s}",
        "  ;                        !- Ratio of Initial Moisture Evaporation Rate and Steady State Latent Capacity {dimensionless}",

        "Curve:Quadratic,",
        "  HPACCoolCapFFF,          !- Name",
        "  0.8,                     !- Coefficient1 Constant",
        "  0.2,                     !- Coefficient2 x",
        "  0.0,                     !- Coefficient3 x**2",
        "  0.5,                     !- Minimum Value of x",
        "  1.5;                     !- Maximum Value of x",

        "Curve:Quadratic,",
        "  HPACEIRFFF,              !- Name",
        "  1.1552,                  !- Coefficient1 Constant",
        "  -0.1808,                 !- Coefficient2 x",
        "  0.0256,                  !- Coefficient3 x**2",
        "  0.5,                     !- Minimum Value of x",
        "  1.5;                     !- Maximum Value of x",

        "Curve:Quadratic,",
        "  HPACPLFFPLR,             !- Name",
        "  0.85,                    !- Coefficient1 Constant",
        "  0.15,                    !- Coefficient2 x",
        "  0.0,                     !- Coefficient3 x**2",
        "  0.0,                     !- Minimum Value of x",
        "  1.0;                     !- Maximum Value of x",

        "Curve:Cubic,",
        "  FanEffRatioCurve,        !- Name",
        "  0.33856828,              !- Coefficient1 Constant",
        "  1.72644131,              !- Coefficient2 x",
        "  -1.49280132,             !- Coefficient3 x**2",
        "  0.42776208,              !- Coefficient4 x**3",
        "  0.5,                     !- Minimum Value of x",
        "  1.5,                     !- Maximum Value of x",
        "  0.3,                     !- Minimum Curve Output",
        "  1.0;                     !- Maximum Curve Output",

        "Curve:Exponent,",
        "  FanPowerRatioCurve,      !- Name",
        "  0.0,                     !- Coefficient1 Constant",
        "  1.0,                     !- Coefficient2 Constant",
        "  3.0,                     !- Coefficient3 Constant",
        "  0.0,                     !- Minimum Value of x",
        "  1.5,                     !- Maximum Value of x",
        "  0.01,                    !- Minimum Curve Output",
        "  1.5;                     !- Maximum Curve Output",

        "Curve:Biquadratic,",
        "  HPACCoolCapFT,           !- Name",
        "  0.942587793,             !- Coefficient1 Constant",
        "  0.009543347,             !- Coefficient2 x",
        "  0.00068377,              !- Coefficient3 x**2",
        "  -0.011042676,            !- Coefficient4 y",
        "  5.249e-06,               !- Coefficient5 y**2",
        "  -9.72e-06,               !- Coefficient6 x*y",
        "  12.77778,                !- Minimum Value of x",
        "  23.88889,                !- Maximum Value of x",
        "  18.0,                    !- Minimum Value of y",
        "  46.11111,                !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Biquadratic,",
        "  HPACEIRFT,               !- Name",
        "  0.342414409,             !- Coefficient1 Constant",
        "  0.034885008,             !- Coefficient2 x",
        "  -0.0006237,              !- Coefficient3 x**2",
        "  0.004977216,             !- Coefficient4 y",
        "  0.000437951,             !- Coefficient5 y**2",
        "  -0.000728028,            !- Coefficient6 x*y",
        "  12.77778,                !- Minimum Value of x",
        "  23.88889,                !- Maximum Value of x",
        "  18.0,                    !- Minimum Value of y",
        "  46.11111,                !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Zone,",
        "  SPACE1-1,                !- Name",
        "  0.0,                     !- Direction of Relative North {deg}",
        "  0.0,                     !- X Origin {m}",
        "  0.0,                     !- Y Origin {m}",
        "  0.0,                     !- Z Origin {m}",
        "  1,                       !- Type",
        "  1,                       !- Multiplier",
        "  2.438400269,             !- Ceiling Height {m}",
        "  239.247360229;           !- Volume {m3}",

        "ZoneHVAC:EquipmentConnections,",
        "  SPACE1-1,                !- Zone Name",
        "  SPACE1-1 Equipment,      !- Zone Conditioning Equipment List Name",
        "  SPACE1-1 Inlets,         !- Zone Air Inlet Node or NodeList Name",
        "  SPACE1-1 Air Terminal Mixer Secondary Inlet,  !- Zone Air Exhaust Node or NodeList Name",
        "  SPACE1-1 Zone Air Node,  !- Zone Air Node Name",
        "  SPACE1-1 Return Outlet;  !- Zone Return Air Node or NodeList Name",

        "NodeList,",
        "  SPACE1-1 Inlets,         !- Name",
        "  SPACE1-1 Supply Inlet;   !- Node 1 Name",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    state->dataGlobal->TimeStepsInHour = 1;
    state->dataGlobal->TimeStep = 1;
    state->dataGlobal->MinutesInTimeStep = 60;
    state->init_state(*state);

    GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    GetZoneEquipmentData(*state);
    GetZoneAirLoopEquipment(*state);
    HVACSystemData *mySys;
    mySys = UnitarySystems::UnitarySys::factory(*state, HVAC::UnitarySysType::Unitary_AnyCoilType, "SPACE1-1 PTAC", true, 0);
    UnitarySystems::UnitarySys thisSys;
    state->dataZoneEquip->ZoneEquipInputsFilled = true;
    thisSys = state->dataUnitarySystems->unitarySys[0];
    thisSys.getUnitarySystemInput(*state, "SPACE1-1 PTAC", true, 0);
    state->dataUnitarySystems->getInputOnceFlag = false;

    // get input test for terminal air single duct mixer on inlet side of PTAC
    ASSERT_EQ(1, state->dataSingleDuct->NumATMixers);
    EXPECT_EQ("SPACE1-1 DOAS AIR TERMINAL", state->dataSingleDuct->SysATMixer(1).Name);                      // single duct air terminal mixer name
    EXPECT_EQ((int)HVAC::MixerType::InletSide, (int)state->dataSingleDuct->SysATMixer(1).type);              // air terminal mixer connection type
    EXPECT_EQ("AIRTERMINAL:SINGLEDUCT:MIXER", state->dataDefineEquipment->AirDistUnit(1).EquipType(1));      // Air distribution unit equipment type
    EXPECT_EQ("ZoneHVAC:PackagedTerminalAirConditioner", state->dataUnitarySystems->unitarySys[0].UnitType); // zoneHVAC equipment type

    state->dataGlobal->BeginEnvrnFlag = false;

    // set input variables
    state->dataEnvrn->OutBaroPress = 101325.0;
    state->dataEnvrn->OutDryBulbTemp = 35.0;
    state->dataEnvrn->OutHumRat = 0.0098;
    state->dataEnvrn->OutEnthalpy = Psychrometrics::PsyHFnTdbW(state->dataEnvrn->OutDryBulbTemp, state->dataEnvrn->OutHumRat);
    state->dataEnvrn->StdRhoAir = 1.20;
    HVACInletMassFlowRate = 0.50;
    PrimaryAirMassFlowRate = 0.1;

    // set zoneNode air condition
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp = 24.0;
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat = 0.0075;
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Enthalpy =
        Psychrometrics::PsyHFnTdbW(state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp,
                                   state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat);

    state->dataUnitarySystems->unitarySys[0].MaxCoolAirMassFlow = HVACInletMassFlowRate; // supply air mass flow rate
    state->dataHVACGlobal->TurnFansOff = false;
    state->dataHVACGlobal->TurnFansOn = true;

    state->dataUnitarySystems->unitarySys[0].m_FanOpMode = HVAC::FanOp::Cycling;
    // initialize mass flow rates
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].AirInNode).MassFlowRate = HVACInletMassFlowRate;
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].m_ATMixerPriNode).MassFlowRate = PrimaryAirMassFlowRate;
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].m_ATMixerPriNode).MassFlowRateMaxAvail = PrimaryAirMassFlowRate;

    // set fan parameters
    state->dataFans->fans(1)->maxAirMassFlowRate = HVACInletMassFlowRate;
    state->dataFans->fans(1)->inletAirMassFlowRate = HVACInletMassFlowRate;
    state->dataFans->fans(1)->rhoAirStdInit = state->dataEnvrn->StdRhoAir;
    state->dataLoopNodes->Node(state->dataFans->fans(1)->inletNodeNum).MassFlowRateMaxAvail = HVACInletMassFlowRate;
    state->dataLoopNodes->Node(state->dataFans->fans(1)->outletNodeNum).MassFlowRateMax = HVACInletMassFlowRate;

    // set DX coil rated performance parameters
    state->dataDXCoils->DXCoil(1).RatedCBF(1) = 0.05;
    state->dataDXCoils->DXCoil(1).RatedAirMassFlowRate(1) = HVACInletMassFlowRate;

    // primary air condition set at outdoor air condition
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].m_ATMixerPriNode).Temp = state->dataEnvrn->OutDryBulbTemp;
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].m_ATMixerPriNode).HumRat = state->dataEnvrn->OutHumRat;
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].m_ATMixerPriNode).Enthalpy = state->dataEnvrn->OutEnthalpy;

    // set secondary air (recirculating air) conditions to zone air node
    state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).Temp =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp;
    state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).HumRat =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat;
    state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).Enthalpy =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Enthalpy;

    state->dataUnitarySystems->unitarySys[0].ControlZoneNum = 1;
    state->dataSize->SysSizingRunDone = true;
    state->dataSize->ZoneSizingRunDone = true;
    state->dataGlobal->SysSizingCalc = true;

    state->dataHeatBalFanSys->TempControlType.allocate(1);
    state->dataHeatBalFanSys->TempControlType(1) = HVAC::SetptType::DualHeatCool;
    state->dataZoneEnergyDemand->ZoneSysMoistureDemand.allocate(1);
    state->dataZoneEnergyDemand->ZoneSysMoistureDemand(1).RemainingOutputReqToDehumidSP = 0.0;
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand.allocate(1);
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputReqToHeatSP = 0.0;
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputReqToCoolSP = -5000.0;
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputRequired = -5000.0;
    QZnReq = state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputReqToCoolSP;

    state->dataUnitarySystems->unitarySys[0].m_sysAvailSched->currentVal = 1.0; // unit is always available
    state->dataUnitarySystems->unitarySys[0].m_fanAvailSched->currentVal = 1.0; // fan is always available

    // set secondary air mass flow rate to zero
    state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).MassFlowRate = 0.0;
    // simulate PTAC zoneHVAC equipment
    bool HeatActive = false;
    bool CoolActive = false;
    Real64 latOut = 0.0;
    mySys->simulate(*state,
                    state->dataUnitarySystems->unitarySys[0].Name,
                    FirstHVACIteration,
                    0,
                    PTUnitNum,
                    HeatActive,
                    CoolActive,
                    0,
                    0,
                    true,
                    QUnitOut,
                    latOut);
    // apply mass conservation to determine secondary air mass flow rate
    SecondaryAirMassFlowRate = state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].AirInNode).MassFlowRate - PrimaryAirMassFlowRate;
    // check the terminal air mixer secondary air mass flow rate
    ASSERT_EQ(SecondaryAirMassFlowRate, state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).MassFlowRate);
    // check the cooling output delivered is within 2.0 Watt of zone cooling load
    ASSERT_NEAR(QZnReq, QUnitOut, 2.0);
}

TEST_F(EnergyPlusFixture, AirTerminalSingleDuctMixer_SimPTAC_ATMSupplySide)
{

    bool ErrorsFound(false);
    bool FirstHVACIteration(false);
    Real64 HVACInletMassFlowRate(0.0);
    Real64 PrimaryAirMassFlowRate(0.0);
    Real64 SecondaryAirMassFlowRate(0.0);
    Real64 ATMixerOutletMassFlowRate(0.0);
    Real64 QUnitOut(0.0);
    Real64 QZnReq(0.0);
    int PTUnitNum(1);

    std::string const idf_objects = delimited_string({
        "AirTerminal:SingleDuct:Mixer,",
        "  SPACE1-1 DOAS Air Terminal,  !- Name",
        "  ZoneHVAC:PackagedTerminalAirConditioner,  !- ZoneHVAC Unit Object Type",
        "  SPACE1-1 PTAC,           !- ZoneHVAC Unit Object Name",
        "  SPACE1-1 Supply Inlet,   !- Mixer Outlet Node Name",
        "  SPACE1-1 Air Terminal Mixer Primary Inlet,  !- Mixer Primary Air Inlet Node Name",
        "  SPACE1-1 PTAC Outlet,    !- Mixer Secondary Air Inlet Node Name",
        "  SupplySide;              !- Mixer Connection Type",

        "ZoneHVAC:AirDistributionUnit,",
        "  SPACE1-1 DOAS ATU,       !- Name",
        "  SPACE1-1 Supply Inlet,   !- Air Distribution Unit Outlet Node Name",
        "  AirTerminal:SingleDuct:Mixer,  !- Air Terminal Object Type",
        "  SPACE1-1 DOAS Air Terminal;  !- Air Terminal Name",

        "Schedule:Compact,",
        "  FanAvailSched,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,1.0;        !- Field 3",

        "Schedule:Compact,",
        "  CyclingFanSch,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,0.0;        !- Field 3",

        "ZoneHVAC:EquipmentList,",
        "  SPACE1-1 Equipment,      !- Name",
        "  SequentialLoad,          !- Load Distribution Scheme",
        "  ZoneHVAC:AirDistributionUnit,  !- Zone Equipment 1 Object Type",
        "  SPACE1-1 DOAS ATU,       !- Zone Equipment 1 Name",
        "  1,                       !- Zone Equipment 1 Cooling Sequence",
        "  1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 1 Sequential Cooling Fraction Schedule Name",
        "  ,                        !- Zone Equipment 1 Sequential Heating Fraction Schedule Name",
        "  ZoneHVAC:PackagedTerminalAirConditioner,  !- Zone Equipment 2 Object Type",
        "  SPACE1-1 PTAC,           !- Zone Equipment 2 Name",
        "  2,                       !- Zone Equipment 2 Cooling Sequence",
        "  2,                       !- Zone Equipment 2 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 2 Sequential Cooling Fraction Schedule Name",
        "  ;                        !- Zone Equipment 2 Sequential Heating Fraction Schedule Name",

        "ZoneHVAC:PackagedTerminalAirConditioner,",
        "  SPACE1-1 PTAC,           !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  SPACE1-1 PTAC Inlet,     !- Air Inlet Node Name",
        "  SPACE1-1 PTAC Outlet,    !- Air Outlet Node Name",
        "  ,                        !- Outdoor Air Mixer Object Type",
        "  ,                        !- Outdoor Air Mixer Name",
        "  0.5,                     !- Cooling Supply Air Flow Rate {m3/s}",
        "  0.5,                     !- Heating Supply Air Flow Rate {m3/s}",
        "  ,                        !- No Load Supply Air Flow Rate {m3/s}",
        "  ,                        !- No Load Supply Air Flow Rate Control Set To Low Speed",
        "  0.0,                     !- Cooling Outdoor Air Flow Rate {m3/s}",
        "  0.0,                     !- Heating Outdoor Air Flow Rate {m3/s}",
        "  0.0,                     !- No Load Outdoor Air Flow Rate {m3/s}",
        "  Fan:OnOff,               !- Supply Air Fan Object Type",
        "  SPACE1-1 Supply Fan,     !- Supply Air Fan Name",
        "  Coil:Heating:Fuel,       !- Heating Coil Object Type",
        "  SPACE1-1 Heating Coil,   !- Heating Coil Name",
        "  Coil:Cooling:DX:SingleSpeed,  !- Cooling Coil Object Type",
        "  SPACE1-1 PTAC CCoil,     !- Cooling Coil Name",
        "  BlowThrough,             !- Fan Placement",
        "  CyclingFanSch;           !- Supply Air Fan Operating Mode Schedule Name",

        "Fan:OnOff,",
        "  SPACE1-1 Supply Fan,     !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  0.7,                     !- Fan Total Efficiency",
        "  75.0,                    !- Pressure Rise {Pa}",
        "  0.5,                     !- Maximum Flow Rate {m3/s}",
        "  0.9,                     !- Motor Efficiency",
        "  1.0,                     !- Motor In Airstream Fraction",
        "  SPACE1-1 PTAC Inlet,     !- Air Inlet Node Name",
        "  SPACE1-1 Zone Unit Fan Outlet;  !- Air Outlet Node Name",

        "Coil:Heating:Fuel,",
        "  SPACE1-1 Heating Coil,   !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  NaturalGas,              !- Fuel Type",
        "  0.8,                     !- Burner Efficiency",
        "  10000.0,                 !- Nominal Capacity {W}",
        "  SPACE1-1 Cooling Coil Outlet,  !- Air Inlet Node Name",
        "  SPACE1-1 PTAC Outlet;    !- Air Outlet Node Name",

        "Coil:Cooling:DX:SingleSpeed,",
        "  SPACE1-1 PTAC CCoil,     !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  7030.0,                  !- Gross Rated Total Cooling Capacity {W}",
        "  0.75,                    !- Gross Rated Sensible Heat Ratio",
        "  3.0,                     !- Gross Rated Cooling COP {W/W}",
        "  0.5,                     !- Rated Air Flow Rate {m3/s}",
        "  ,                        !- 2017 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  ,                        !- 2023 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  SPACE1-1 Zone Unit Fan Outlet,  !- Air Inlet Node Name",
        "  SPACE1-1 Cooling Coil Outlet,  !- Air Outlet Node Name",
        "  HPACCoolCapFT,           !- Total Cooling Capacity Function of Temperature Curve Name",
        "  HPACCoolCapFFF,          !- Total Cooling Capacity Function of Flow Fraction Curve Name",
        "  HPACEIRFT,               !- Energy Input Ratio Function of Temperature Curve Name",
        "  HPACEIRFFF,              !- Energy Input Ratio Function of Flow Fraction Curve Name",
        "  HPACPLFFPLR,             !- Part Load Fraction Correlation Curve Name",
        "  ,                        !- Minimum Outdoor Dry-Bulb Temperature for Compressor Operation {C}",
        "  ,                        !- Nominal Time for Condensate Removal to Begin {s}",
        "  ;                        !- Ratio of Initial Moisture Evaporation Rate and Steady State Latent Capacity {dimensionless}",

        "Curve:Quadratic,",
        "  HPACCoolCapFFF,          !- Name",
        "  0.8,                     !- Coefficient1 Constant",
        "  0.2,                     !- Coefficient2 x",
        "  0.0,                     !- Coefficient3 x**2",
        "  0.5,                     !- Minimum Value of x",
        "  1.5;                     !- Maximum Value of x",

        "Curve:Quadratic,",
        "  HPACEIRFFF,              !- Name",
        "  1.1552,                  !- Coefficient1 Constant",
        "  -0.1808,                 !- Coefficient2 x",
        "  0.0256,                  !- Coefficient3 x**2",
        "  0.5,                     !- Minimum Value of x",
        "  1.5;                     !- Maximum Value of x",

        "Curve:Quadratic,",
        "  HPACPLFFPLR,             !- Name",
        "  0.85,                    !- Coefficient1 Constant",
        "  0.15,                    !- Coefficient2 x",
        "  0.0,                     !- Coefficient3 x**2",
        "  0.0,                     !- Minimum Value of x",
        "  1.0;                     !- Maximum Value of x",

        "Curve:Cubic,",
        "  FanEffRatioCurve,        !- Name",
        "  0.33856828,              !- Coefficient1 Constant",
        "  1.72644131,              !- Coefficient2 x",
        "  -1.49280132,             !- Coefficient3 x**2",
        "  0.42776208,              !- Coefficient4 x**3",
        "  0.5,                     !- Minimum Value of x",
        "  1.5,                     !- Maximum Value of x",
        "  0.3,                     !- Minimum Curve Output",
        "  1.0;                     !- Maximum Curve Output",

        "Curve:Exponent,",
        "  FanPowerRatioCurve,      !- Name",
        "  0.0,                     !- Coefficient1 Constant",
        "  1.0,                     !- Coefficient2 Constant",
        "  3.0,                     !- Coefficient3 Constant",
        "  0.0,                     !- Minimum Value of x",
        "  1.5,                     !- Maximum Value of x",
        "  0.01,                    !- Minimum Curve Output",
        "  1.5;                     !- Maximum Curve Output",

        "Curve:Biquadratic,",
        "  HPACCoolCapFT,           !- Name",
        "  0.942587793,             !- Coefficient1 Constant",
        "  0.009543347,             !- Coefficient2 x",
        "  0.00068377,              !- Coefficient3 x**2",
        "  -0.011042676,            !- Coefficient4 y",
        "  5.249e-06,               !- Coefficient5 y**2",
        "  -9.72e-06,               !- Coefficient6 x*y",
        "  12.77778,                !- Minimum Value of x",
        "  23.88889,                !- Maximum Value of x",
        "  18.0,                    !- Minimum Value of y",
        "  46.11111,                !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Biquadratic,",
        "  HPACEIRFT,               !- Name",
        "  0.342414409,             !- Coefficient1 Constant",
        "  0.034885008,             !- Coefficient2 x",
        "  -0.0006237,              !- Coefficient3 x**2",
        "  0.004977216,             !- Coefficient4 y",
        "  0.000437951,             !- Coefficient5 y**2",
        "  -0.000728028,            !- Coefficient6 x*y",
        "  12.77778,                !- Minimum Value of x",
        "  23.88889,                !- Maximum Value of x",
        "  18.0,                    !- Minimum Value of y",
        "  46.11111,                !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Zone,",
        "  SPACE1-1,                !- Name",
        "  0.0,                     !- Direction of Relative North {deg}",
        "  0.0,                     !- X Origin {m}",
        "  0.0,                     !- Y Origin {m}",
        "  0.0,                     !- Z Origin {m}",
        "  1,                       !- Type",
        "  1,                       !- Multiplier",
        "  2.438400269,             !- Ceiling Height {m}",
        "  239.247360229;           !- Volume {m3}",

        "ZoneHVAC:EquipmentConnections,",
        "  SPACE1-1,                !- Zone Name",
        "  SPACE1-1 Equipment,      !- Zone Conditioning Equipment List Name",
        "  SPACE1-1 Inlets,         !- Zone Air Inlet Node or NodeList Name",
        "  SPACE1-1 PTAC Inlet,     !- Zone Air Exhaust Node or NodeList Name",
        "  SPACE1-1 Zone Air Node,  !- Zone Air Node Name",
        "  SPACE1-1 Return Outlet;  !- Zone Return Air Node or NodeList Name",

        "NodeList,",
        "  SPACE1-1 Inlets,         !- Name",
        "  SPACE1-1 Supply Inlet;   !- Node 1 Name",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    state->dataGlobal->TimeStepsInHour = 1;
    state->dataGlobal->TimeStep = 1;
    state->dataGlobal->MinutesInTimeStep = 60;
    state->init_state(*state);

    GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    GetZoneEquipmentData(*state);
    GetZoneAirLoopEquipment(*state);
    HVACSystemData *mySys;
    mySys = UnitarySystems::UnitarySys::factory(*state, HVAC::UnitarySysType::Unitary_AnyCoilType, "SPACE1-1 PTAC", true, 0);
    UnitarySystems::UnitarySys thisSys;
    state->dataZoneEquip->ZoneEquipInputsFilled = true;
    thisSys = state->dataUnitarySystems->unitarySys[0];
    thisSys.getUnitarySystemInput(*state, "SPACE1-1 PTAC", true, 0);
    state->dataUnitarySystems->getInputOnceFlag = false;

    // get input test for terminal air single duct mixer on supply side of PTAC
    ASSERT_EQ(1, state->dataSingleDuct->NumATMixers);
    EXPECT_EQ("SPACE1-1 DOAS AIR TERMINAL", state->dataSingleDuct->SysATMixer(1).Name);                 // single duct air terminal mixer name
    EXPECT_EQ((int)HVAC::MixerType::SupplySide, (int)state->dataSingleDuct->SysATMixer(1).type);        // air terminal mixer connection type
    EXPECT_EQ("AIRTERMINAL:SINGLEDUCT:MIXER", state->dataDefineEquipment->AirDistUnit(1).EquipType(1)); // Air distribution unit equipment type
    EXPECT_EQ(UnitarySystems::UnitarySys::SysType::PackagedAC, state->dataUnitarySystems->unitarySys[0].m_sysType); // zoneHVAC equipment type

    // set input variables
    state->dataEnvrn->OutBaroPress = 101325.0;
    state->dataEnvrn->OutDryBulbTemp = 35.0;
    state->dataEnvrn->OutHumRat = 0.0098;
    state->dataEnvrn->OutEnthalpy = Psychrometrics::PsyHFnTdbW(state->dataEnvrn->OutDryBulbTemp, state->dataEnvrn->OutHumRat);
    state->dataEnvrn->StdRhoAir = 1.20;
    HVACInletMassFlowRate = 0.50;
    PrimaryAirMassFlowRate = 0.1;

    state->dataGlobal->BeginEnvrnFlag = false;

    // set zoneNode air condition
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp = 24.0;
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat = 0.0075;
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Enthalpy =
        Psychrometrics::PsyHFnTdbW(state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp,
                                   state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat);

    state->dataUnitarySystems->unitarySys[0].MaxCoolAirMassFlow = HVACInletMassFlowRate; // supply air mass flow rate
    state->dataHVACGlobal->TurnFansOff = false;
    state->dataHVACGlobal->TurnFansOn = true;

    PTUnitNum = 0;
    state->dataUnitarySystems->unitarySys[0].m_FanOpMode = HVAC::FanOp::Cycling;
    // initialize mass flow rates
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].AirInNode).MassFlowRate = HVACInletMassFlowRate;
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].m_ATMixerPriNode).MassFlowRate = PrimaryAirMassFlowRate;
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].m_ATMixerPriNode).MassFlowRateMaxAvail = PrimaryAirMassFlowRate;

    // set fan parameters
    state->dataFans->fans(1)->maxAirMassFlowRate = HVACInletMassFlowRate;
    state->dataFans->fans(1)->inletAirMassFlowRate = HVACInletMassFlowRate;
    state->dataFans->fans(1)->rhoAirStdInit = state->dataEnvrn->StdRhoAir;
    state->dataLoopNodes->Node(state->dataFans->fans(1)->inletNodeNum).MassFlowRateMaxAvail = HVACInletMassFlowRate;
    state->dataLoopNodes->Node(state->dataFans->fans(1)->outletNodeNum).MassFlowRateMax = HVACInletMassFlowRate;

    // set DX coil rated performance parameters
    state->dataDXCoils->DXCoil(1).RatedCBF(1) = 0.05;
    state->dataDXCoils->DXCoil(1).RatedAirMassFlowRate(1) = HVACInletMassFlowRate;

    // primary air condition at outside air condition
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].m_ATMixerPriNode).Temp = state->dataEnvrn->OutDryBulbTemp;
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].m_ATMixerPriNode).HumRat = state->dataEnvrn->OutHumRat;
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].m_ATMixerPriNode).Enthalpy = state->dataEnvrn->OutEnthalpy;

    // set PTUnit inlet condition to zone air node
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].AirInNode).Temp =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp;
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].AirInNode).HumRat =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat;
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].AirInNode).Enthalpy =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Enthalpy;

    state->dataUnitarySystems->unitarySys[0].ControlZoneNum = 1;
    state->dataSize->SysSizingRunDone = true;
    state->dataSize->ZoneSizingRunDone = true;
    state->dataGlobal->SysSizingCalc = true;

    state->dataHeatBalFanSys->TempControlType.allocate(1);
    state->dataHeatBalFanSys->TempControlType(1) = HVAC::SetptType::DualHeatCool;
    state->dataZoneEnergyDemand->ZoneSysMoistureDemand.allocate(1);
    state->dataZoneEnergyDemand->ZoneSysMoistureDemand(1).RemainingOutputReqToDehumidSP = 0.0;
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand.allocate(1);
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputReqToHeatSP = 0.0;
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputReqToCoolSP = -5000.0;
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputRequired = -5000.0;
    QZnReq = state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputReqToCoolSP;

    state->dataUnitarySystems->unitarySys[0].m_sysAvailSched->currentVal = 1.0; // unit is always available
    state->dataUnitarySystems->unitarySys[0].m_fanAvailSched->currentVal = 1.0; // fan is always available

    // set secondary air mass flow rate to zero
    state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).MassFlowRate = 0.0;
    // simulate PTAC zoneHVAC equipment
    bool HeatActive = false;
    bool CoolActive = false;
    Real64 latOut = 0.0;
    mySys->simulate(*state,
                    state->dataUnitarySystems->unitarySys[0].Name,
                    FirstHVACIteration,
                    0,
                    PTUnitNum,
                    HeatActive,
                    CoolActive,
                    0,
                    0,
                    true,
                    QUnitOut,
                    latOut);
    // apply mass conservation to determine secondary mass flow rate
    SecondaryAirMassFlowRate = state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).MassFlowRate;
    // check the terminal air mixer secondary air mass flow rate
    ASSERT_EQ(SecondaryAirMassFlowRate, state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).MassFlowRate);
    // check the terminal air mixer outlet air mass flow rate
    ATMixerOutletMassFlowRate = SecondaryAirMassFlowRate + PrimaryAirMassFlowRate;
    ASSERT_EQ(ATMixerOutletMassFlowRate, state->dataSingleDuct->SysATMixer(1).MixedAirMassFlowRate);
    // check the cooling output delivered is within 2.0 Watt of zone cooling load
    ASSERT_NEAR(QZnReq, QUnitOut, 2.0);
}

TEST_F(EnergyPlusFixture, AirTerminalSingleDuctMixer_SimPTHP_ATMInletSide)
{

    bool ErrorsFound(false);
    bool FirstHVACIteration(false);
    Real64 HVACInletMassFlowRate(0.0);
    Real64 PrimaryAirMassFlowRate(0.0);
    Real64 SecondaryAirMassFlowRate(0.0);
    Real64 QUnitOut(0.0);
    Real64 QZnReq(0.0);
    int PTUnitNum(1);

    std::string const idf_objects = delimited_string({
        "AirTerminal:SingleDuct:Mixer,",
        "  SPACE1-1 DOAS Air Terminal,  !- Name",
        "  ZoneHVAC:PackagedTerminalHeatPump,  !- ZoneHVAC Unit Object Type",
        "  SPACE1-1 Heat Pump,      !- ZoneHVAC Unit Object Name",
        "  SPACE1-1 Heat Pump Inlet,  !- Mixer Outlet Node Name",
        "  SPACE1-1 Air Terminal Mixer Primary Inlet,  !- Mixer Primary Air Inlet Node Name",
        "  SPACE1-1 Air Terminal Mixer Secondary Inlet,  !- Mixer Secondary Air Inlet Node Name",
        "  InletSide;               !- Mixer Connection Type",

        "ZoneHVAC:AirDistributionUnit,",
        "  SPACE1-1 DOAS ATU,       !- Name",
        "  SPACE1-1 Heat Pump Inlet,  !- Air Distribution Unit Outlet Node Name",
        "  AirTerminal:SingleDuct:Mixer,  !- Air Terminal Object Type",
        "  SPACE1-1 DOAS Air Terminal;  !- Air Terminal Name",

        "Schedule:Compact,",
        "  FanAvailSched,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,1.0;        !- Field 3",

        "Schedule:Compact,",
        "  CyclingFanSch,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,0.0;        !- Field 3",

        "ZoneHVAC:EquipmentList,",
        "  SPACE1-1 Equipment,      !- Name",
        "  SequentialLoad,          !- Load Distribution Scheme",
        "  ZoneHVAC:AirDistributionUnit,  !- Zone Equipment 1 Object Type",
        "  SPACE1-1 DOAS ATU,       !- Zone Equipment 1 Name",
        "  1,                       !- Zone Equipment 1 Cooling Sequence",
        "  1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 1 Sequential Cooling Fraction Schedule Name",
        "  ,                        !- Zone Equipment 1 Sequential Heating Fraction Schedule Name",
        "  ZoneHVAC:PackagedTerminalHeatPump,  !- Zone Equipment 2 Object Type",
        "  SPACE1-1 Heat Pump,      !- Zone Equipment 2 Name",
        "  2,                       !- Zone Equipment 2 Cooling Sequence",
        "  2,                       !- Zone Equipment 2 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 2 Sequential Cooling Fraction Schedule Name",
        "  ;                        !- Zone Equipment 2 Sequential Heating Fraction Schedule Name",

        "ZoneHVAC:PackagedTerminalHeatPump,",
        "  SPACE1-1 Heat Pump,      !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  SPACE1-1 Heat Pump Inlet,  !- Air Inlet Node Name",
        "  SPACE1-1 Supply Inlet,   !- Air Outlet Node Name",
        "  ,                        !- Outdoor Air Mixer Object Type",
        "  ,                        !- Outdoor Air Mixer Name",
        "  0.5,                     !- Cooling Supply Air Flow Rate {m3/s}",
        "  0.5,                     !- Heating Supply Air Flow Rate {m3/s}",
        "  ,                        !- No Load Supply Air Flow Rate {m3/s}",
        "  ,                        !- No Load Supply Air Flow Rate Control Set To Low Speed",
        "  0.0,                     !- Cooling Outdoor Air Flow Rate {m3/s}",
        "  0.0,                     !- Heating Outdoor Air Flow Rate {m3/s}",
        "  0.0,                     !- No Load Outdoor Air Flow Rate {m3/s}",
        "  Fan:OnOff,               !- Supply Air Fan Object Type",
        "  SPACE1-1 Supply Fan,     !- Supply Air Fan Name",
        "  Coil:Heating:DX:SingleSpeed,  !- Heating Coil Object Type",
        "  SPACE1-1 HP Heating Mode,  !- Heating Coil Name",
        "  0.001,                   !- Heating Convergence Tolerance {dimensionless}",
        "  Coil:Cooling:DX:SingleSpeed,  !- Cooling Coil Object Type",
        "  SPACE1-1 HP Cooling Mode,  !- Cooling Coil Name",
        "  0.001,                   !- Cooling Convergence Tolerance {dimensionless}",
        "  Coil:Heating:Fuel,       !- Supplemental Heating Coil Object Type",
        "  SPACE1-1 HP Supp Coil,   !- Supplemental Heating Coil Name",
        "  50.0,                    !- Maximum Supply Air Temperature from Supplemental Heater {C}",
        "  20.0,                    !- Maximum Outdoor Dry-Bulb Temperature for Supplemental Heater Operation {C}",
        "  BlowThrough,             !- Fan Placement",
        "  CyclingFanSch;           !- Supply Air Fan Operating Mode Schedule Name",

        "Fan:OnOff,",
        "  SPACE1-1 Supply Fan,     !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  0.7,                     !- Fan Total Efficiency",
        "  75.0,                    !- Pressure Rise {Pa}",
        "  0.5,                     !- Maximum Flow Rate {m3/s}",
        "  0.9,                     !- Motor Efficiency",
        "  1.0,                     !- Motor In Airstream Fraction",
        "  SPACE1-1 Heat Pump Inlet,  !- Air Inlet Node Name",
        "  SPACE1-1 Zone Unit Fan Outlet;  !- Air Outlet Node Name",

        "Coil:Heating:DX:SingleSpeed,",
        "  SPACE1-1 HP Heating Mode,  !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  7000.0,                  !- Gross Rated Heating Capacity {W}",
        "  3.75,                    !- Gross Rated Heating COP {W/W}",
        "  0.5,                     !- Rated Air Flow Rate {m3/s}",
        "  ,                        !- 2017 Rated Supply Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  ,                        !- 2023 Rated Supply Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  SPACE1-1 Cooling Coil Outlet,  !- Air Inlet Node Name",
        "  SPACE1-1 Heating Coil Outlet,  !- Air Outlet Node Name",
        "  HPACHeatCapFT,           !- Heating Capacity Function of Temperature Curve Name",
        "  HPACHeatCapFFF,          !- Heating Capacity Function of Flow Fraction Curve Name",
        "  HPACHeatEIRFT,           !- Energy Input Ratio Function of Temperature Curve Name",
        "  HPACHeatEIRFFF,          !- Energy Input Ratio Function of Flow Fraction Curve Name",
        "  HPACCOOLPLFFPLR,         !- Part Load Fraction Correlation Curve Name",
        "  ,                        !- Defrost Energy Input Ratio Function of Temperature Curve Name",
        "  2.0,                     !- Minimum Outdoor Dry-Bulb Temperature for Compressor Operation {C}",
        "  ,                        !- Outdoor Dry-Bulb Temperature to Turn On Compressor {C}",
        "  5.0,                     !- Maximum Outdoor Dry-Bulb Temperature for Defrost Operation {C}",
        "  200.0,                   !- Crankcase Heater Capacity {W}",
        "  ,                        !- Crankcase Heater Capacity Function of Temperature Curve Name",
        "  10.0,                    !- Maximum Outdoor Dry-Bulb Temperature for Crankcase Heater Operation {C}",
        "  Resistive,               !- Defrost Strategy",
        "  Timed,                   !- Defrost Control",
        "  0.166667,                !- Defrost Time Period Fraction",
        "  Autosize,                !- Resistive Defrost Heater Capacity {W}",
        "  ,                        !- Region number for calculating HSPF",
        "  ,                        !- Evaporator Air Inlet Node Name",
        "  ,                        !- Zone Name for Evaporator Placement",
        "  ,                        !- Secondary Coil Air Flow Rate {m3/s}",
        "  ;                        !- Secondary Coil Fan Flow Scaling Factor {m3/s}",

        "Coil:Cooling:DX:SingleSpeed,",
        "  SPACE1-1 HP Cooling Mode,  !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  6680.0,                  !- Gross Rated Total Cooling Capacity {W}",
        "  0.75,                    !- Gross Rated Sensible Heat Ratio",
        "  3.0,                     !- Gross Rated Cooling COP {W/W}",
        "  0.5,                     !- Rated Air Flow Rate {m3/s}",
        "  ,                        !- 2017 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  ,                        !- 2023 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  SPACE1-1 Zone Unit Fan Outlet,  !- Air Inlet Node Name",
        "  SPACE1-1 Cooling Coil Outlet,  !- Air Outlet Node Name",
        "  HPACCoolCapFT,           !- Total Cooling Capacity Function of Temperature Curve Name",
        "  HPACCoolCapFFF,          !- Total Cooling Capacity Function of Flow Fraction Curve Name",
        "  HPACEIRFT,               !- Energy Input Ratio Function of Temperature Curve Name",
        "  HPACEIRFFF,              !- Energy Input Ratio Function of Flow Fraction Curve Name",
        "  HPACPLFFPLR,             !- Part Load Fraction Correlation Curve Name",
        "  ,                        !- Minimum Outdoor Dry-Bulb Temperature for Compressor Operation {C}",
        "  ,                        !- Nominal Time for Condensate Removal to Begin {s}",
        "  ;                        !- Ratio of Initial Moisture Evaporation Rate and Steady State Latent Capacity {dimensionless}",

        "Coil:Heating:Fuel,",
        "  SPACE1-1 HP Supp Coil,   !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  NaturalGas,              !- Fuel Type",
        "  0.8,                     !- Burner Efficiency",
        "  10000.0,                 !- Nominal Capacity {W}",
        "  SPACE1-1 Heating Coil Outlet,  !- Air Inlet Node Name",
        "  SPACE1-1 Supply Inlet;   !- Air Outlet Node Name",

        "Curve:Quadratic,",
        "  HPACCoolCapFFF,          !- Name",
        "  0.8,                     !- Coefficient1 Constant",
        "  0.2,                     !- Coefficient2 x",
        "  0.0,                     !- Coefficient3 x**2",
        "  0.5,                     !- Minimum Value of x",
        "  1.5;                     !- Maximum Value of x",

        "Curve:Quadratic,",
        "  HPACEIRFFF,              !- Name",
        "  1.1552,                  !- Coefficient1 Constant",
        "  -0.1808,                 !- Coefficient2 x",
        "  0.0256,                  !- Coefficient3 x**2",
        "  0.5,                     !- Minimum Value of x",
        "  1.5;                     !- Maximum Value of x",

        "Curve:Quadratic,",
        "  HPACPLFFPLR,             !- Name",
        "  0.85,                    !- Coefficient1 Constant",
        "  0.15,                    !- Coefficient2 x",
        "  0.0,                     !- Coefficient3 x**2",
        "  0.0,                     !- Minimum Value of x",
        "  1.0;                     !- Maximum Value of x",

        "Curve:Quadratic,",
        "  HPACHeatEIRFFF,          !- Name",
        "  1.3824,                  !- Coefficient1 Constant",
        "  -0.4336,                 !- Coefficient2 x",
        "  0.0512,                  !- Coefficient3 x**2",
        "  0.0,                     !- Minimum Value of x",
        "  1.0;                     !- Maximum Value of x",

        "Curve:Quadratic,",
        "  HPACCOOLPLFFPLR,         !- Name",
        "  0.75,                    !- Coefficient1 Constant",
        "  0.25,                    !- Coefficient2 x",
        "  0.0,                     !- Coefficient3 x**2",
        "  0.0,                     !- Minimum Value of x",
        "  1.0;                     !- Maximum Value of x",

        "Curve:Cubic,",
        "  HPACHeatCapFT,           !- Name",
        "  0.758746,                !- Coefficient1 Constant",
        "  0.027626,                !- Coefficient2 x",
        "  0.000148716,             !- Coefficient3 x**2",
        "  3.4992e-06,              !- Coefficient4 x**3",
        "  -20.0,                   !- Minimum Value of x",
        "  20.0,                    !- Maximum Value of x",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Cubic,",
        "  HPACHeatCapFFF,          !- Name",
        "  0.84,                    !- Coefficient1 Constant",
        "  0.16,                    !- Coefficient2 x",
        "  0.0,                     !- Coefficient3 x**2",
        "  0.0,                     !- Coefficient4 x**3",
        "  0.5,                     !- Minimum Value of x",
        "  1.5;                     !- Maximum Value of x",

        "Curve:Cubic,",
        "  HPACHeatEIRFT,           !- Name",
        "  1.19248,                 !- Coefficient1 Constant",
        "  -0.0300438,              !- Coefficient2 x",
        "  0.00103745,              !- Coefficient3 x**2",
        "  -2.3328e-05,             !- Coefficient4 x**3",
        "  -20.0,                   !- Minimum Value of x",
        "  20.0,                    !- Maximum Value of x",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Cubic,",
        "  FanEffRatioCurve,        !- Name",
        "  0.33856828,              !- Coefficient1 Constant",
        "  1.72644131,              !- Coefficient2 x",
        "  -1.49280132,             !- Coefficient3 x**2",
        "  0.42776208,              !- Coefficient4 x**3",
        "  0.5,                     !- Minimum Value of x",
        "  1.5,                     !- Maximum Value of x",
        "  0.3,                     !- Minimum Curve Output",
        "  1.0;                     !- Maximum Curve Output",

        "Curve:Exponent,",
        "  FanPowerRatioCurve,      !- Name",
        "  0.0,                     !- Coefficient1 Constant",
        "  1.0,                     !- Coefficient2 Constant",
        "  3.0,                     !- Coefficient3 Constant",
        "  0.0,                     !- Minimum Value of x",
        "  1.5,                     !- Maximum Value of x",
        "  0.01,                    !- Minimum Curve Output",
        "  1.5;                     !- Maximum Curve Output",

        "Curve:Biquadratic,",
        "  HPACCoolCapFT,           !- Name",
        "  0.942587793,             !- Coefficient1 Constant",
        "  0.009543347,             !- Coefficient2 x",
        "  0.00068377,              !- Coefficient3 x**2",
        "  -0.011042676,            !- Coefficient4 y",
        "  5.249e-06,               !- Coefficient5 y**2",
        "  -9.72e-06,               !- Coefficient6 x*y",
        "  12.77778,                !- Minimum Value of x",
        "  23.88889,                !- Maximum Value of x",
        "  18.0,                    !- Minimum Value of y",
        "  46.11111,                !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Biquadratic,",
        "  HPACEIRFT,               !- Name",
        "  0.342414409,             !- Coefficient1 Constant",
        "  0.034885008,             !- Coefficient2 x",
        "  -0.0006237,              !- Coefficient3 x**2",
        "  0.004977216,             !- Coefficient4 y",
        "  0.000437951,             !- Coefficient5 y**2",
        "  -0.000728028,            !- Coefficient6 x*y",
        "  12.77778,                !- Minimum Value of x",
        "  23.88889,                !- Maximum Value of x",
        "  18.0,                    !- Minimum Value of y",
        "  46.11111,                !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Zone,",
        "  SPACE1-1,                !- Name",
        "  0.0,                     !- Direction of Relative North {deg}",
        "  0.0,                     !- X Origin {m}",
        "  0.0,                     !- Y Origin {m}",
        "  0.0,                     !- Z Origin {m}",
        "  1,                       !- Type",
        "  1,                       !- Multiplier",
        "  2.438400269,             !- Ceiling Height {m}",
        "  239.247360229;           !- Volume {m3}",

        "ZoneHVAC:EquipmentConnections,",
        "  SPACE1-1,                !- Zone Name",
        "  SPACE1-1 Equipment,      !- Zone Conditioning Equipment List Name",
        "  SPACE1-1 Inlets,         !- Zone Air Inlet Node or NodeList Name",
        "  SPACE1-1 Air Terminal Mixer Secondary Inlet,  !- Zone Air Exhaust Node or NodeList Name",
        "  SPACE1-1 Zone Air Node,  !- Zone Air Node Name",
        "  SPACE1-1 Return Outlet;  !- Zone Return Air Node or NodeList Name",

        "NodeList,",
        "  SPACE1-1 Inlets,         !- Name",
        "  SPACE1-1 Supply Inlet;   !- Node 1 Name",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    state->dataGlobal->TimeStepsInHour = 1;
    state->dataGlobal->TimeStep = 1;
    state->dataGlobal->MinutesInTimeStep = 60;
    state->init_state(*state);

    GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    GetZoneEquipmentData(*state);
    GetZoneAirLoopEquipment(*state);
    HVACSystemData *mySys;
    mySys = UnitarySystems::UnitarySys::factory(*state, HVAC::UnitarySysType::Unitary_AnyCoilType, "SPACE1-1 Heat Pump", true, 0);
    UnitarySystems::UnitarySys thisSys;
    state->dataZoneEquip->ZoneEquipInputsFilled = true;
    thisSys = state->dataUnitarySystems->unitarySys[0];
    thisSys.getUnitarySystemInput(*state, "SPACE1-1 Heat Pump", true, 0);
    state->dataUnitarySystems->getInputOnceFlag = false;

    // get input test for terminal air single duct mixer on inlet side of PTHP
    ASSERT_EQ(1, state->dataSingleDuct->NumATMixers);
    EXPECT_EQ("SPACE1-1 DOAS AIR TERMINAL", state->dataSingleDuct->SysATMixer(1).Name);                 // single duct air terminal mixer name
    EXPECT_EQ((int)HVAC::MixerType::InletSide, (int)state->dataSingleDuct->SysATMixer(1).type);         // air terminal mixer connection type
    EXPECT_EQ("AIRTERMINAL:SINGLEDUCT:MIXER", state->dataDefineEquipment->AirDistUnit(1).EquipType(1)); // Air distribution unit equipment type
    EXPECT_EQ("ZoneHVAC:PackagedTerminalHeatPump", state->dataUnitarySystems->unitarySys[0].UnitType);  // zoneHVAC equipment type

    state->dataGlobal->BeginEnvrnFlag = false;

    // set input variables
    state->dataEnvrn->OutBaroPress = 101325.0;
    state->dataEnvrn->OutDryBulbTemp = 35.0;
    state->dataEnvrn->OutHumRat = 0.0098;
    state->dataEnvrn->OutEnthalpy = Psychrometrics::PsyHFnTdbW(state->dataEnvrn->OutDryBulbTemp, state->dataEnvrn->OutHumRat);
    state->dataEnvrn->StdRhoAir = 1.20;
    HVACInletMassFlowRate = 0.50;
    PrimaryAirMassFlowRate = 0.1;

    // set zoneNode air condition
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp = 24.0;
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat = 0.0075;
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Enthalpy =
        Psychrometrics::PsyHFnTdbW(state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp,
                                   state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat);

    state->dataUnitarySystems->unitarySys[0].MaxCoolAirMassFlow = HVACInletMassFlowRate; // supply air mass flow rate
    state->dataHVACGlobal->TurnFansOff = false;
    state->dataHVACGlobal->TurnFansOn = true;

    PTUnitNum = 0;
    state->dataUnitarySystems->unitarySys[0].m_FanOpMode = HVAC::FanOp::Cycling;
    // initialize mass flow rates
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].AirInNode).MassFlowRate = HVACInletMassFlowRate;
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].m_ATMixerPriNode).MassFlowRate = PrimaryAirMassFlowRate;
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].m_ATMixerPriNode).MassFlowRateMaxAvail = PrimaryAirMassFlowRate;

    // set fan parameters
    state->dataFans->fans(1)->maxAirMassFlowRate = HVACInletMassFlowRate;
    state->dataFans->fans(1)->inletAirMassFlowRate = HVACInletMassFlowRate;
    state->dataFans->fans(1)->rhoAirStdInit = state->dataEnvrn->StdRhoAir;
    state->dataLoopNodes->Node(state->dataFans->fans(1)->inletNodeNum).MassFlowRateMaxAvail = HVACInletMassFlowRate;
    state->dataLoopNodes->Node(state->dataFans->fans(1)->outletNodeNum).MassFlowRateMax = HVACInletMassFlowRate;

    // set DX coil rated performance parameters
    state->dataDXCoils->DXCoil(1).RatedCBF(1) = 0.05;
    state->dataDXCoils->DXCoil(1).RatedAirMassFlowRate(1) = HVACInletMassFlowRate;

    // primary air condition set at outdoor air condition
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].m_ATMixerPriNode).Temp = state->dataEnvrn->OutDryBulbTemp;
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].m_ATMixerPriNode).HumRat = state->dataEnvrn->OutHumRat;
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].m_ATMixerPriNode).Enthalpy = state->dataEnvrn->OutEnthalpy;

    // set secondary air (recirculating air) conditions to zone air node
    state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).Temp =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp;
    state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).HumRat =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat;
    state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).Enthalpy =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Enthalpy;

    state->dataUnitarySystems->unitarySys[0].ControlZoneNum = 1;
    state->dataSize->SysSizingRunDone = true;
    state->dataSize->ZoneSizingRunDone = true;
    state->dataGlobal->SysSizingCalc = true;

    state->dataHeatBalFanSys->TempControlType.allocate(1);
    state->dataHeatBalFanSys->TempControlType(1) = HVAC::SetptType::DualHeatCool;
    state->dataZoneEnergyDemand->ZoneSysMoistureDemand.allocate(1);
    state->dataZoneEnergyDemand->ZoneSysMoistureDemand(1).RemainingOutputReqToDehumidSP = 0.0;
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand.allocate(1);
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputReqToHeatSP = 0.0;
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputReqToCoolSP = -5000.0;
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputRequired = -5000.0;
    QZnReq = state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputReqToCoolSP;

    state->dataUnitarySystems->unitarySys[0].m_sysAvailSched->currentVal = 1.0; // unit is always available
    state->dataUnitarySystems->unitarySys[0].m_fanAvailSched->currentVal = 1.0; // fan is always available

    // set secondary air mass flow rate to zero
    state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).MassFlowRate = 0.0;
    // simulate PTHP zoneHVAC equipment
    bool HeatActive = false;
    bool CoolActive = false;
    Real64 latOut = 0.0;
    mySys->simulate(*state,
                    state->dataUnitarySystems->unitarySys[0].Name,
                    FirstHVACIteration,
                    0,
                    PTUnitNum,
                    HeatActive,
                    CoolActive,
                    0,
                    0,
                    true,
                    QUnitOut,
                    latOut);
    // apply mass conservation to determine secondary air mass flow rate
    SecondaryAirMassFlowRate = state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].AirInNode).MassFlowRate - PrimaryAirMassFlowRate;
    // check the terminal air mixer secondary air mass flow rate
    ASSERT_EQ(SecondaryAirMassFlowRate, state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).MassFlowRate);
    // check the cooling output delivered is within 2.0 Watt of zone cooling load
    ASSERT_NEAR(QZnReq, QUnitOut, 2.0);
}

TEST_F(EnergyPlusFixture, AirTerminalSingleDuctMixer_SimPTHP_ATMSupplySide)
{

    bool ErrorsFound(false);
    bool FirstHVACIteration(false);
    Real64 HVACInletMassFlowRate(0.0);
    Real64 PrimaryAirMassFlowRate(0.0);
    Real64 SecondaryAirMassFlowRate(0.0);
    Real64 ATMixerOutletMassFlowRate(0.0);
    Real64 QUnitOut(0.0);
    Real64 QZnReq(0.0);
    int PTUnitNum(1);

    std::string const idf_objects = delimited_string({
        "AirTerminal:SingleDuct:Mixer,",
        "  SPACE1-1 DOAS Air Terminal,  !- Name",
        "  ZoneHVAC:PackagedTerminalHeatPump,  !- ZoneHVAC Unit Object Type",
        "  SPACE1-1 Heat Pump,      !- ZoneHVAC Unit Object Name",
        "  SPACE1-1 Supply Inlet,   !- Mixer Outlet Node Name",
        "  SPACE1-1 Air Terminal Mixer Primary Inlet,  !- Mixer Primary Air Inlet Node Name",
        "  SPACE1-1 Heat Pump Outlet,  !- Mixer Secondary Air Inlet Node Name",
        "  SupplySide;              !- Mixer Connection Type",

        "ZoneHVAC:AirDistributionUnit,",
        "  SPACE1-1 DOAS ATU,       !- Name",
        "  SPACE1-1 Supply Inlet,   !- Air Distribution Unit Outlet Node Name",
        "  AirTerminal:SingleDuct:Mixer,  !- Air Terminal Object Type",
        "  SPACE1-1 DOAS Air Terminal;  !- Air Terminal Name",

        "Schedule:Compact,",
        "  FanAvailSched,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,1.0;        !- Field 3",

        "Schedule:Compact,",
        "  CyclingFanSch,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,0.0;        !- Field 3",

        "ZoneHVAC:EquipmentList,",
        "  SPACE1-1 Equipment,      !- Name",
        "  SequentialLoad,          !- Load Distribution Scheme",
        "  ZoneHVAC:AirDistributionUnit,  !- Zone Equipment 1 Object Type",
        "  SPACE1-1 DOAS ATU,       !- Zone Equipment 1 Name",
        "  1,                       !- Zone Equipment 1 Cooling Sequence",
        "  1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 1 Sequential Cooling Fraction Schedule Name",
        "  ,                        !- Zone Equipment 1 Sequential Heating Fraction Schedule Name",
        "  ZoneHVAC:PackagedTerminalHeatPump,  !- Zone Equipment 2 Object Type",
        "  SPACE1-1 Heat Pump,      !- Zone Equipment 2 Name",
        "  2,                       !- Zone Equipment 2 Cooling Sequence",
        "  2,                       !- Zone Equipment 2 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 2 Sequential Cooling Fraction Schedule Name",
        "  ;                        !- Zone Equipment 2 Sequential Heating Fraction Schedule Name",

        "ZoneHVAC:PackagedTerminalHeatPump,",
        "  SPACE1-1 Heat Pump,      !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  SPACE1-1 Heat Pump Inlet,  !- Air Inlet Node Name",
        "  SPACE1-1 Heat Pump Outlet,  !- Air Outlet Node Name",
        "  ,                        !- Outdoor Air Mixer Object Type",
        "  ,                        !- Outdoor Air Mixer Name",
        "  0.5,                     !- Cooling Supply Air Flow Rate {m3/s}",
        "  0.5,                     !- Heating Supply Air Flow Rate {m3/s}",
        "  ,                        !- No Load Supply Air Flow Rate {m3/s}",
        "  ,                        !- No Load Supply Air Flow Rate Control Set To Low Speed",
        "  0.0,                     !- Cooling Outdoor Air Flow Rate {m3/s}",
        "  0.0,                     !- Heating Outdoor Air Flow Rate {m3/s}",
        "  0.0,                     !- No Load Outdoor Air Flow Rate {m3/s}",
        "  Fan:OnOff,               !- Supply Air Fan Object Type",
        "  SPACE1-1 Supply Fan,     !- Supply Air Fan Name",
        "  Coil:Heating:DX:SingleSpeed,  !- Heating Coil Object Type",
        "  SPACE1-1 HP Heating Mode,  !- Heating Coil Name",
        "  0.001,                   !- Heating Convergence Tolerance {dimensionless}",
        "  Coil:Cooling:DX:SingleSpeed,  !- Cooling Coil Object Type",
        "  SPACE1-1 HP Cooling Mode,  !- Cooling Coil Name",
        "  0.001,                   !- Cooling Convergence Tolerance {dimensionless}",
        "  Coil:Heating:Fuel,       !- Supplemental Heating Coil Object Type",
        "  SPACE1-1 HP Supp Coil,   !- Supplemental Heating Coil Name",
        "  50.0,                    !- Maximum Supply Air Temperature from Supplemental Heater {C}",
        "  20.0,                    !- Maximum Outdoor Dry-Bulb Temperature for Supplemental Heater Operation {C}",
        "  BlowThrough,             !- Fan Placement",
        "  CyclingFanSch;           !- Supply Air Fan Operating Mode Schedule Name",

        "Fan:OnOff,",
        "  SPACE1-1 Supply Fan,     !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  0.7,                     !- Fan Total Efficiency",
        "  75.0,                    !- Pressure Rise {Pa}",
        "  0.5,                     !- Maximum Flow Rate {m3/s}",
        "  0.9,                     !- Motor Efficiency",
        "  1.0,                     !- Motor In Airstream Fraction",
        "  SPACE1-1 Heat Pump Inlet,  !- Air Inlet Node Name",
        "  SPACE1-1 Zone Unit Fan Outlet;  !- Air Outlet Node Name",

        "Coil:Heating:DX:SingleSpeed,",
        "  SPACE1-1 HP Heating Mode,  !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  7000.0,                  !- Gross Rated Heating Capacity {W}",
        "  3.75,                    !- Gross Rated Heating COP {W/W}",
        "  0.5,                     !- Rated Air Flow Rate {m3/s}",
        "  ,                        !- 2017 Rated Supply Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  ,                        !- 2023 Rated Supply Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  SPACE1-1 Cooling Coil Outlet,  !- Air Inlet Node Name",
        "  SPACE1-1 Heating Coil Outlet,  !- Air Outlet Node Name",
        "  HPACHeatCapFT,           !- Heating Capacity Function of Temperature Curve Name",
        "  HPACHeatCapFFF,          !- Heating Capacity Function of Flow Fraction Curve Name",
        "  HPACHeatEIRFT,           !- Energy Input Ratio Function of Temperature Curve Name",
        "  HPACHeatEIRFFF,          !- Energy Input Ratio Function of Flow Fraction Curve Name",
        "  HPACCOOLPLFFPLR,         !- Part Load Fraction Correlation Curve Name",
        "  ,                        !- Defrost Energy Input Ratio Function of Temperature Curve Name",
        "  2.0,                     !- Minimum Outdoor Dry-Bulb Temperature for Compressor Operation {C}",
        "  ,                        !- Outdoor Dry-Bulb Temperature to Turn On Compressor {C}",
        "  5.0,                     !- Maximum Outdoor Dry-Bulb Temperature for Defrost Operation {C}",
        "  200.0,                   !- Crankcase Heater Capacity {W}",
        "  ,                        !- Crankcase Heater Capacity Function of Temperature Curve Name",
        "  10.0,                    !- Maximum Outdoor Dry-Bulb Temperature for Crankcase Heater Operation {C}",
        "  Resistive,               !- Defrost Strategy",
        "  Timed,                   !- Defrost Control",
        "  0.166667,                !- Defrost Time Period Fraction",
        "  Autosize,                !- Resistive Defrost Heater Capacity {W}",
        "  ,                        !- Region number for calculating HSPF",
        "  ,                        !- Evaporator Air Inlet Node Name",
        "  ,                        !- Zone Name for Evaporator Placement",
        "  ,                        !- Secondary Coil Air Flow Rate {m3/s}",
        "  ;                        !- Secondary Coil Fan Flow Scaling Factor {m3/s}",

        "Coil:Cooling:DX:SingleSpeed,",
        "  SPACE1-1 HP Cooling Mode,  !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  7030.0,                  !- Gross Rated Total Cooling Capacity {W}",
        "  0.75,                    !- Gross Rated Sensible Heat Ratio",
        "  3.0,                     !- Gross Rated Cooling COP {W/W}",
        "  0.5,                     !- Rated Air Flow Rate {m3/s}",
        "  ,                        !- 2017 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  ,                        !- 2023 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  SPACE1-1 Zone Unit Fan Outlet,  !- Air Inlet Node Name",
        "  SPACE1-1 Cooling Coil Outlet,  !- Air Outlet Node Name",
        "  HPACCoolCapFT,           !- Total Cooling Capacity Function of Temperature Curve Name",
        "  HPACCoolCapFFF,          !- Total Cooling Capacity Function of Flow Fraction Curve Name",
        "  HPACEIRFT,               !- Energy Input Ratio Function of Temperature Curve Name",
        "  HPACEIRFFF,              !- Energy Input Ratio Function of Flow Fraction Curve Name",
        "  HPACPLFFPLR,             !- Part Load Fraction Correlation Curve Name",
        "  ,                        !- Minimum Outdoor Dry-Bulb Temperature for Compressor Operation {C}",
        "  ,                        !- Nominal Time for Condensate Removal to Begin {s}",
        "  ;                        !- Ratio of Initial Moisture Evaporation Rate and Steady State Latent Capacity {dimensionless}",

        "Coil:Heating:Fuel,",
        "  SPACE1-1 HP Supp Coil,   !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  NaturalGas,              !- Fuel Type",
        "  0.8,                     !- Burner Efficiency",
        "  10000.0,                 !- Nominal Capacity {W}",
        "  SPACE1-1 Heating Coil Outlet,  !- Air Inlet Node Name",
        "  SPACE1-1 Heat Pump Outlet;  !- Air Outlet Node Name",

        "Curve:Quadratic,",
        "  HPACCoolCapFFF,          !- Name",
        "  0.8,                     !- Coefficient1 Constant",
        "  0.2,                     !- Coefficient2 x",
        "  0.0,                     !- Coefficient3 x**2",
        "  0.5,                     !- Minimum Value of x",
        "  1.5;                     !- Maximum Value of x",

        "Curve:Quadratic,",
        "  HPACEIRFFF,              !- Name",
        "  1.1552,                  !- Coefficient1 Constant",
        "  -0.1808,                 !- Coefficient2 x",
        "  0.0256,                  !- Coefficient3 x**2",
        "  0.5,                     !- Minimum Value of x",
        "  1.5;                     !- Maximum Value of x",

        "Curve:Quadratic,",
        "  HPACPLFFPLR,             !- Name",
        "  0.85,                    !- Coefficient1 Constant",
        "  0.15,                    !- Coefficient2 x",
        "  0.0,                     !- Coefficient3 x**2",
        "  0.0,                     !- Minimum Value of x",
        "  1.0;                     !- Maximum Value of x",

        "Curve:Quadratic,",
        "  HPACHeatEIRFFF,          !- Name",
        "  1.3824,                  !- Coefficient1 Constant",
        "  -0.4336,                 !- Coefficient2 x",
        "  0.0512,                  !- Coefficient3 x**2",
        "  0.0,                     !- Minimum Value of x",
        "  1.0;                     !- Maximum Value of x",

        "Curve:Quadratic,",
        "  HPACCOOLPLFFPLR,         !- Name",
        "  0.75,                    !- Coefficient1 Constant",
        "  0.25,                    !- Coefficient2 x",
        "  0.0,                     !- Coefficient3 x**2",
        "  0.0,                     !- Minimum Value of x",
        "  1.0;                     !- Maximum Value of x",

        "Curve:Cubic,",
        "  HPACHeatCapFT,           !- Name",
        "  0.758746,                !- Coefficient1 Constant",
        "  0.027626,                !- Coefficient2 x",
        "  0.000148716,             !- Coefficient3 x**2",
        "  3.4992e-06,              !- Coefficient4 x**3",
        "  -20.0,                   !- Minimum Value of x",
        "  20.0,                    !- Maximum Value of x",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Cubic,",
        "  HPACHeatCapFFF,          !- Name",
        "  0.84,                    !- Coefficient1 Constant",
        "  0.16,                    !- Coefficient2 x",
        "  0.0,                     !- Coefficient3 x**2",
        "  0.0,                     !- Coefficient4 x**3",
        "  0.5,                     !- Minimum Value of x",
        "  1.5;                     !- Maximum Value of x",

        "Curve:Cubic,",
        "  HPACHeatEIRFT,           !- Name",
        "  1.19248,                 !- Coefficient1 Constant",
        "  -0.0300438,              !- Coefficient2 x",
        "  0.00103745,              !- Coefficient3 x**2",
        "  -2.3328e-05,             !- Coefficient4 x**3",
        "  -20.0,                   !- Minimum Value of x",
        "  20.0,                    !- Maximum Value of x",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Cubic,",
        "  FanEffRatioCurve,        !- Name",
        "  0.33856828,              !- Coefficient1 Constant",
        "  1.72644131,              !- Coefficient2 x",
        "  -1.49280132,             !- Coefficient3 x**2",
        "  0.42776208,              !- Coefficient4 x**3",
        "  0.5,                     !- Minimum Value of x",
        "  1.5,                     !- Maximum Value of x",
        "  0.3,                     !- Minimum Curve Output",
        "  1.0;                     !- Maximum Curve Output",

        "Curve:Exponent,",
        "  FanPowerRatioCurve,      !- Name",
        "  0.0,                     !- Coefficient1 Constant",
        "  1.0,                     !- Coefficient2 Constant",
        "  3.0,                     !- Coefficient3 Constant",
        "  0.0,                     !- Minimum Value of x",
        "  1.5,                     !- Maximum Value of x",
        "  0.01,                    !- Minimum Curve Output",
        "  1.5;                     !- Maximum Curve Output",

        "Curve:Biquadratic,",
        "  HPACCoolCapFT,           !- Name",
        "  0.942587793,             !- Coefficient1 Constant",
        "  0.009543347,             !- Coefficient2 x",
        "  0.00068377,              !- Coefficient3 x**2",
        "  -0.011042676,            !- Coefficient4 y",
        "  5.249e-06,               !- Coefficient5 y**2",
        "  -9.72e-06,               !- Coefficient6 x*y",
        "  12.77778,                !- Minimum Value of x",
        "  23.88889,                !- Maximum Value of x",
        "  18.0,                    !- Minimum Value of y",
        "  46.11111,                !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Biquadratic,",
        "  HPACEIRFT,               !- Name",
        "  0.342414409,             !- Coefficient1 Constant",
        "  0.034885008,             !- Coefficient2 x",
        "  -0.0006237,              !- Coefficient3 x**2",
        "  0.004977216,             !- Coefficient4 y",
        "  0.000437951,             !- Coefficient5 y**2",
        "  -0.000728028,            !- Coefficient6 x*y",
        "  12.77778,                !- Minimum Value of x",
        "  23.88889,                !- Maximum Value of x",
        "  18.0,                    !- Minimum Value of y",
        "  46.11111,                !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Zone,",
        "  SPACE1-1,                !- Name",
        "  0.0,                     !- Direction of Relative North {deg}",
        "  0.0,                     !- X Origin {m}",
        "  0.0,                     !- Y Origin {m}",
        "  0.0,                     !- Z Origin {m}",
        "  1,                       !- Type",
        "  1,                       !- Multiplier",
        "  2.438400269,             !- Ceiling Height {m}",
        "  239.247360229;           !- Volume {m3}",

        "ZoneHVAC:EquipmentConnections,",
        "  SPACE1-1,                !- Zone Name",
        "  SPACE1-1 Equipment,      !- Zone Conditioning Equipment List Name",
        "  SPACE1-1 Inlets,         !- Zone Air Inlet Node or NodeList Name",
        "  SPACE1-1 Heat Pump Inlet,  !- Zone Air Exhaust Node or NodeList Name",
        "  SPACE1-1 Zone Air Node,  !- Zone Air Node Name",
        "  SPACE1-1 Return Outlet;  !- Zone Return Air Node or NodeList Name",

        "NodeList,",
        "  SPACE1-1 Inlets,         !- Name",
        "  SPACE1-1 Supply Inlet;   !- Node 1 Name",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    state->dataGlobal->TimeStepsInHour = 1;
    state->dataGlobal->TimeStep = 1;
    state->dataGlobal->MinutesInTimeStep = 60;
    state->init_state(*state);

    GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    GetZoneEquipmentData(*state);
    GetZoneAirLoopEquipment(*state);
    HVACSystemData *mySys;
    mySys = UnitarySystems::UnitarySys::factory(*state, HVAC::UnitarySysType::Unitary_AnyCoilType, "SPACE1-1 Heat Pump", true, 0);
    UnitarySystems::UnitarySys thisSys;
    state->dataZoneEquip->ZoneEquipInputsFilled = true;
    thisSys = state->dataUnitarySystems->unitarySys[0];
    thisSys.getUnitarySystemInput(*state, "SPACE1-1 Heat Pump", true, 0);
    state->dataUnitarySystems->getInputOnceFlag = false;

    // get input test for terminal air single duct mixer on supply side of PTHP
    ASSERT_EQ(1, state->dataSingleDuct->NumATMixers);
    EXPECT_EQ("SPACE1-1 DOAS AIR TERMINAL", state->dataSingleDuct->SysATMixer(1).Name);                 // single duct air terminal mixer name
    EXPECT_EQ((int)HVAC::MixerType::SupplySide, (int)state->dataSingleDuct->SysATMixer(1).type);        // air terminal mixer connection type
    EXPECT_EQ("AIRTERMINAL:SINGLEDUCT:MIXER", state->dataDefineEquipment->AirDistUnit(1).EquipType(1)); // Air distribution unit equipment type
    EXPECT_EQ("ZoneHVAC:PackagedTerminalHeatPump", state->dataUnitarySystems->unitarySys[0].UnitType);  // zoneHVAC equipment type

    // set input variables
    state->dataEnvrn->OutBaroPress = 101325.0;
    state->dataEnvrn->OutDryBulbTemp = 35.0;
    state->dataEnvrn->OutHumRat = 0.0098;
    state->dataEnvrn->OutEnthalpy = Psychrometrics::PsyHFnTdbW(state->dataEnvrn->OutDryBulbTemp, state->dataEnvrn->OutHumRat);
    state->dataEnvrn->StdRhoAir = 1.20;
    HVACInletMassFlowRate = 0.50;
    PrimaryAirMassFlowRate = 0.1;

    state->dataGlobal->BeginEnvrnFlag = false;

    // set zoneNode air condition
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp = 24.0;
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat = 0.0075;
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Enthalpy =
        Psychrometrics::PsyHFnTdbW(state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp,
                                   state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat);

    state->dataUnitarySystems->unitarySys[0].MaxCoolAirMassFlow = HVACInletMassFlowRate; // supply air mass flow rate
    state->dataHVACGlobal->TurnFansOff = false;
    state->dataHVACGlobal->TurnFansOn = true;

    PTUnitNum = 1;
    state->dataUnitarySystems->unitarySys[0].m_FanOpMode = HVAC::FanOp::Cycling;
    // initialize mass flow rates
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].AirInNode).MassFlowRate = HVACInletMassFlowRate;
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].m_ATMixerPriNode).MassFlowRate = PrimaryAirMassFlowRate;
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].m_ATMixerPriNode).MassFlowRateMaxAvail = PrimaryAirMassFlowRate;

    // set fan parameters
    state->dataFans->fans(1)->maxAirMassFlowRate = HVACInletMassFlowRate;
    state->dataFans->fans(1)->inletAirMassFlowRate = HVACInletMassFlowRate;
    state->dataFans->fans(1)->rhoAirStdInit = state->dataEnvrn->StdRhoAir;
    state->dataLoopNodes->Node(state->dataFans->fans(1)->inletNodeNum).MassFlowRateMaxAvail = HVACInletMassFlowRate;
    state->dataLoopNodes->Node(state->dataFans->fans(1)->outletNodeNum).MassFlowRateMax = HVACInletMassFlowRate;

    // set DX coil rated performance parameters
    state->dataDXCoils->DXCoil(1).RatedCBF(1) = 0.05;
    state->dataDXCoils->DXCoil(1).RatedAirMassFlowRate(1) = HVACInletMassFlowRate;

    // primary air condition at outside air condition
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].m_ATMixerPriNode).Temp = state->dataEnvrn->OutDryBulbTemp;
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].m_ATMixerPriNode).HumRat = state->dataEnvrn->OutHumRat;
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].m_ATMixerPriNode).Enthalpy = state->dataEnvrn->OutEnthalpy;

    // set PTUnit inlet condition to zone air node
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].AirInNode).Temp =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp;
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].AirInNode).HumRat =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat;
    state->dataLoopNodes->Node(state->dataUnitarySystems->unitarySys[0].AirInNode).Enthalpy =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Enthalpy;

    state->dataUnitarySystems->unitarySys[0].ControlZoneNum = 1;
    state->dataSize->SysSizingRunDone = true;
    state->dataSize->ZoneSizingRunDone = true;
    state->dataGlobal->SysSizingCalc = true;

    state->dataHeatBalFanSys->TempControlType.allocate(1);
    state->dataHeatBalFanSys->TempControlType(1) = HVAC::SetptType::DualHeatCool;
    state->dataZoneEnergyDemand->ZoneSysMoistureDemand.allocate(1);
    state->dataZoneEnergyDemand->ZoneSysMoistureDemand(1).RemainingOutputReqToDehumidSP = 0.0;
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand.allocate(1);
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputReqToHeatSP = 0.0;
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputReqToCoolSP = -5000.0;
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputRequired = -5000.0;
    QZnReq = state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputReqToCoolSP;

    state->dataUnitarySystems->unitarySys[0].m_sysAvailSched->currentVal = 1.0; // unit is always available
    state->dataUnitarySystems->unitarySys[0].m_fanAvailSched->currentVal = 1.0; // fan is always available

    // set secondary air mass flow rate to zero
    state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).MassFlowRate = 0.0;
    // simulate PTHP zoneHVAC equipment
    bool HeatActive = false;
    bool CoolActive = false;
    Real64 latOut = 0.0;
    mySys->simulate(*state,
                    state->dataUnitarySystems->unitarySys[0].Name,
                    FirstHVACIteration,
                    0,
                    PTUnitNum,
                    HeatActive,
                    CoolActive,
                    0,
                    0,
                    true,
                    QUnitOut,
                    latOut);
    // apply mass conservation to determine secondary mass flow rate
    SecondaryAirMassFlowRate = state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).MassFlowRate;
    // check the terminal air mixer secondary air mass flow rate
    ASSERT_EQ(SecondaryAirMassFlowRate, state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).MassFlowRate);
    // check the terminal air mixer outlet air mass flow rate
    ATMixerOutletMassFlowRate = SecondaryAirMassFlowRate + PrimaryAirMassFlowRate;
    ASSERT_EQ(ATMixerOutletMassFlowRate, state->dataSingleDuct->SysATMixer(1).MixedAirMassFlowRate);
    // check the cooling output delivered is within 2.0 Watt of zone cooling load
    ASSERT_NEAR(QZnReq, QUnitOut, 2.0);
}

TEST_F(EnergyPlusFixture, AirTerminalSingleDuctMixer_SimVRF_ATMInletSide)
{

    bool ErrorsFound(false);
    bool FirstHVACIteration(false);
    Real64 HVACInletMassFlowRate(0.0);
    Real64 PrimaryAirMassFlowRate(0.0);
    Real64 SecondaryAirMassFlowRate(0.0);
    Real64 OnOffAirFlowRatio(1.0);
    Real64 LatOutputProvided(0.0);
    Real64 QUnitOutVRFTU(0.0);
    Real64 QZnReq(0.0);
    int VRFNum(1);
    int VRFTUNum(1);

    std::string const idf_objects = delimited_string({
        "AirTerminal:SingleDuct:Mixer,",
        "  SPACE1-1 DOAS Air Terminal,  !- Name",
        "  ZoneHVAC:TerminalUnit:VariableRefrigerantFlow,  !- ZoneHVAC Unit Object Type",
        "  TU1,                     !- ZoneHVAC Unit Object Name",
        "  TU1 Inlet Node,          !- Mixer Outlet Node Name",
        "  SPACE1-1 Air Terminal Mixer Primary Inlet,  !- Mixer Primary Air Inlet Node Name",
        "  SPACE1-1 Air Terminal Mixer Secondary Inlet,  !- Mixer Secondary Air Inlet Node Name",
        "  InletSide;               !- Mixer Connection Type",

        "ZoneHVAC:AirDistributionUnit,",
        "  SPACE1-1 DOAS ATU,       !- Name",
        "  TU1 Inlet Node,          !- Air Distribution Unit Outlet Node Name",
        "  AirTerminal:SingleDuct:Mixer,  !- Air Terminal Object Type",
        "  SPACE1-1 DOAS Air Terminal;  !- Air Terminal Name",

        "Schedule:Compact,",
        "  FanAvailSched,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,1.0;        !- Field 3",

        "Schedule:Compact,",
        "  VRFAvailSched,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,1.0;        !- Field 3",

        "Schedule:Compact,",
        "  CyclingFanSch,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,0.0;        !- Field 3",

        "ZoneHVAC:EquipmentList,",
        "  SPACE1-1 Eq,             !- Name",
        "  SequentialLoad,          !- Load Distribution Scheme",
        "  ZoneHVAC:AirDistributionUnit,  !- Zone Equipment 1 Object Type",
        "  SPACE1-1 DOAS ATU,       !- Zone Equipment 1 Name",
        "  1,                       !- Zone Equipment 1 Cooling Sequence",
        "  1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 1 Sequential Cooling Fraction Schedule Name",
        "  ,                        !- Zone Equipment 1 Sequential Heating Fraction Schedule Name",
        "  ZoneHVAC:TerminalUnit:VariableRefrigerantFlow,  !- Zone Equipment 2 Object Type",
        "  TU1,                     !- Zone Equipment 2 Name",
        "  2,                       !- Zone Equipment 2 Cooling Sequence",
        "  2,                       !- Zone Equipment 2 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 2 Sequential Cooling Fraction Schedule Name",
        "  ;                        !- Zone Equipment 2 Sequential Heating Fraction Schedule Name",

        "ZoneHVAC:TerminalUnit:VariableRefrigerantFlow,",
        "  TU1,                     !- Zone Terminal Unit Name",
        "  VRFAvailSched,           !- Terminal Unit Availability Schedule",
        "  TU1 Inlet Node,          !- Terminal Unit Air Inlet Node Name",
        "  TU1 Outlet Node,         !- Terminal Unit Air Outlet Node Name",
        "  0.5,                     !- Cooling Supply Air Flow Rate {m3/s}",
        "  0.5,                     !- No Cooling Supply Air Flow Rate {m3/s}",
        "  0.5,                     !- Heating Supply Air Flow Rate {m3/s}",
        "  0.5,                     !- No Heating Supply Air Flow Rate {m3/s}",
        "  0.0,                     !- Cooling Outdoor Air Flow Rate {m3/s}",
        "  0.0,                     !- Heating Outdoor Air Flow Rate {m3/s}",
        "  0.0,                     !- No Load Outdoor Air Flow Rate {m3/s}",
        "  VRFFanSchedule,          !- Supply Air Fan Operating Mode Schedule Name",
        "  DrawThrough,             !- Supply Air Fan Placement",
        "  Fan:ConstantVolume,      !- Supply Air Fan Object Type",
        "  TU1 VRF Supply Fan,      !- Supply Air Fan Object Name",
        "  ,                        !- Outside Air Mixer Object Type",
        "  ,                        !- Outside Air Mixer Object Name",
        "  Coil:Cooling:DX:VariableRefrigerantFlow,  !- Cooling Coil Object Type",
        "  TU1 VRF DX Cooling Coil,  !- Cooling Coil Object Name",
        "  Coil:Heating:DX:VariableRefrigerantFlow,  !- Heating Coil Object Type",
        "  TU1 VRF DX Heating Coil,  !- Heating Coil Object Name",
        "  30.0,                    !- Zone Terminal Unit On Parasitic Electric Energy Use {W}",
        "  20.0,                    !- Zone Terminal Unit Off Parasitic Electric Energy Use {W}",
        "  ,                        !- Rated Heating Capacity Sizing Ratio {W/W}",
        "  ,                        !- Availability Manager List Name",
        "  ,                        !- Design Specification ZoneHVAC Sizing Object Name",
        "  ,                        !- Supplemental Heating Coil Object Type",
        "  ,                        !- Supplemental Heating Coil Name",
        "  ,                        !- Maximum Supply Air Temperature from Supplemental Heater {C}",
        "  ;                        !- Maximum Outdoor Dry-Bulb Temperature for Supplemental Heater Operation {C}",

        "Fan:ConstantVolume,",
        "  TU1 VRF Supply Fan,      !- Name",
        "  VRFAvailSched,           !- Availability Schedule Name",
        "  0.7,                     !- Fan Total Efficiency",
        "  600.0,                   !- Pressure Rise {Pa}",
        "  0.5,                     !- Maximum Flow Rate {m3/s}",
        "  0.9,                     !- Motor Efficiency",
        "  1.0,                     !- Motor In Airstream Fraction",
        "  TU1 VRF DX HCoil Outlet Node,  !- Air Inlet Node Name",
        "  TU1 Outlet Node;         !- Air Outlet Node Name",

        "Coil:Heating:DX:VariableRefrigerantFlow,",
        "  TU1 VRF DX Heating Coil,  !- Name",
        "  VRFAvailSched,           !- Availability Schedule",
        "  7000.0,                  !- Gross Rated Heating Capacity {W}",
        "  0.5,                     !- Rated Air Flow Rate {m3/s}",
        "  TU1 VRF DX CCoil Outlet Node,  !- Coil Air Inlet Node",
        "  TU1 VRF DX HCoil Outlet Node,  !- Coil Air Outlet Node",
        "  VRFTUHeatCapFT,          !- Heating Capacity Ratio Modifier Function of Temperature Curve Name",
        "  VRFACCoolCapFFF;         !- Heating Capacity Modifier Function of Flow Fraction Curve Name",

        "Curve:Cubic,",
        "  VRFTUHeatCapFT,          !- Name",
        "  -0.390708928227928,      !- Coefficient1 Constant",
        "  0.261815023760162,       !- Coefficient2 x",
        "  -0.0130431603151873,     !- Coefficient3 x**2",
        "  0.000178131745997821,    !- Coefficient4 x**3",
        "  0.0,                     !- Minimum Value of x",
        "  50.0,                    !- Maximum Value of x",
        "  0.5,                     !- Minimum Curve Output",
        "  1.5,                     !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Dimensionless;           !- Output Unit Type",

        "Coil:Cooling:DX:VariableRefrigerantFlow,",
        "  TU1 VRF DX Cooling Coil,  !- Name",
        "  VRFAvailSched,           !- Availability Schedule Name",
        "  6600.0,                  !- Gross Rated Total Cooling Capacity {W}",
        "  0.75,                    !- Gross Rated Sensible Heat Ratio",
        "  0.5,                     !- Rated Air Flow Rate {m3/s}",
        "  VRFTUCoolCapFT,          !- Cooling Capacity Ratio Modifier Function of Temperature Curve Name",
        "  VRFACCoolCapFFF,         !- Cooling Capacity Modifier Curve Function of Flow Fraction Name",
        "  TU1 Inlet Node,          !- Coil Air Inlet Node",
        "  TU1 VRF DX CCoil Outlet Node,  !- Coil Air Outlet Node",
        "  ;                        !- Name of Water Storage Tank for Condensate Collection",

        "Curve:Cubic,",
        "  VRFTUCoolCapFT,          !- Name",
        "  0.504547273506488,       !- Coefficient1 Constant",
        "  0.0288891279198444,      !- Coefficient2 x",
        "  -1.0819418650677e-05,    !- Coefficient3 x**2",
        "  1.01359395177008e-05,    !- Coefficient4 x**3",
        "  0.0,                     !- Minimum Value of x",
        "  50.0,                    !- Maximum Value of x",
        "  0.5,                     !- Minimum Curve Output",
        "  1.5,                     !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Quadratic,",
        "  VRFACCoolCapFFF,         !- Name",
        "  0.8,                     !- Coefficient1 Constant",
        "  0.2,                     !- Coefficient2 x",
        "  0.0,                     !- Coefficient3 x**2",
        "  0.5,                     !- Minimum Value of x",
        "  1.5;                     !- Maximum Value of x",

        "Zone,",
        "  SPACE1-1,                !- Name",
        "  0.0,                     !- Direction of Relative North {deg}",
        "  0.0,                     !- X Origin {m}",
        "  0.0,                     !- Y Origin {m}",
        "  0.0,                     !- Z Origin {m}",
        "  1,                       !- Type",
        "  1,                       !- Multiplier",
        "  2.438400269,             !- Ceiling Height {m}",
        "  239.247360229;           !- Volume {m3}",

        "ZoneHVAC:EquipmentConnections,",
        "  SPACE1-1,                !- Zone Name",
        "  SPACE1-1 Eq,             !- Zone Conditioning Equipment List Name",
        "  SPACE1-1 In Nodes,       !- Zone Air Inlet Node or NodeList Name",
        "  SPACE1-1 Air Terminal Mixer Secondary Inlet,  !- Zone Air Exhaust Node or NodeList Name",
        "  SPACE1-1 Node,           !- Zone Air Node Name",
        "  SPACE1-1 RETURN OUTLET;  !- Zone Return Air Node or NodeList Name",

        "NodeList,",
        "  SPACE1-1 In Nodes,       !- Name",
        "  TU1 Outlet Node;         !- Node 1 Name",

        "AirConditioner:VariableRefrigerantFlow,",
        "  VRF Heat Pump,           !- Heat Pump Name",
        "  VRFAvailSched,           !- Availability Schedule Name",
        "  15000.0,                 !- Gross Rated Total Cooling Capacity {W}",
        "  3.2917,                  !- Gross Rated Cooling COP {W/W}",
        "  -5.0,                    !- Minimum Condenser Inlet Node Temperature in Cooling Mode {C}",
        "  43.0,                    !- Maximum Condenser Inlet Node Temperature in Cooling Mode {C}",
        "  VRFCoolCapFT,            !- Cooling Capacity Ratio Modifier Function of Low Temperature Curve Name",
        "  VRFCoolCapFTBoundary,    !- Cooling Capacity Ratio Boundary Curve Name",
        "  VRFCoolCapFTHi,          !- Cooling Capacity Ratio Modifier Function of High Temperature Curve Name",
        "  VRFCoolEIRFT,            !- Cooling Energy Input Ratio Modifier Function of Low Temperature Curve Name",
        "  VRFCoolEIRFTBoundary,    !- Cooling Energy Input Ratio Boundary Curve Name",
        "  VRFCoolEIRFTHi,          !- Cooling Energy Input Ratio Modifier Function of High Temperature Curve Name",
        "  CoolingEIRLowPLR,        !- Cooling Energy Input Ratio Modifier Function of Low Part-Load Ratio Curve Name",
        "  CoolingEIRHiPLR,         !- Cooling Energy Input Ratio Modifier Function of High Part-Load Ratio Curve Name",
        "  CoolingCombRatio,        !- Cooling Combination Ratio Correction Factor Curve Name",
        "  VRFCPLFFPLR,             !- Cooling Part-Load Fraction Correlation Curve Name",
        "  15000.0,                 !- Gross Rated Heating Capacity {W}",
        "  ,                        !- Rated Heating Capacity Sizing Ratio {W/W}",
        "  3.5484,                  !- Gross Rated Heating COP {W/W}",
        "  -20.0,                   !- Minimum Condenser Inlet Node Temperature in Heating Mode {C}",
        "  20.0,                    !- Maximum Condenser Inlet Node Temperature in Heating Mode {C}",
        "  VRFHeatCapFT,            !- Heating Capacity Ratio Modifier Function of Low Temperature Curve Name",
        "  VRFHeatCapFTBoundary,    !- Heating Capacity Ratio Boundary Curve Name",
        "  VRFHeatCapFTHi,          !- Heating Capacity Ratio Modifier Function of High Temperature Curve Name",
        "  VRFHeatEIRFT,            !- Heating Energy Input Ratio Modifier Function of Low Temperature Curve Name",
        "  VRFHeatEIRFTBoundary,    !- Heating Energy Input Ratio Boundary Curve Name",
        "  VRFHeatEIRFTHi,          !- Heating Energy Input Ratio Modifier Function of High Temperature Curve Name",
        "  WetBulbTemperature,      !- Heating Performance Curve Outdoor Temperature Type",
        "  HeatingEIRLowPLR,        !- Heating Energy Input Ratio Modifier Function of Low Part-Load Ratio Curve Name",
        "  HeatingEIRHiPLR,         !- Heating Energy Input Ratio Modifier Function of High Part-Load Ratio Curve Name",
        "  HeatingCombRatio,        !- Heating Combination Ratio Correction Factor Curve Name",
        "  VRFCPLFFPLR,             !- Heating Part-Load Fraction Correlation Curve Name",
        "  0.25,                    !- Minimum Heat Pump Part-Load Ratio {dimensionless}",
        "  SPACE1-1,                !- Zone Name for Master Thermostat Location",
        "  LoadPriority,            !- Master Thermostat Priority Control Type",
        "  ,                        !- Thermostat Priority Schedule Name",
        "  VRF Heat Pump TU List,   !- Zone Terminal Unit List Name",
        "  No,                      !- Heat Pump Waste Heat Recovery",
        "  30.0,                    !- Equivalent Piping Length used for Piping Correction Factor in Cooling Mode {m}",
        "  10.0,                    !- Vertical Height used for Piping Correction Factor {m}",
        "  CoolingLengthCorrectionFactor,  !- Piping Correction Factor for Length in Cooling Mode Curve Name",
        "  -0.000386,               !- Piping Correction Factor for Height in Cooling Mode Coefficient {1/m}",
        "  30.0,                    !- Equivalent Piping Length used for Piping Correction Factor in Heating Mode {m}",
        "  ,                        !- Piping Correction Factor for Length in Heating Mode Curve Name",
        "  ,                        !- Piping Correction Factor for Height in Heating Mode Coefficient {1/m}",
        "  15.0,                    !- Crankcase Heater Power per Compressor {W}",
        "  3,                       !- Number of Compressors {dimensionless}",
        "  0.33,                    !- Ratio of Compressor Size to Total Compressor Capacity {W/W}",
        "  7.0,                     !- Maximum Outdoor Dry-Bulb Temperature for Crankcase Heater {C}",
        "  Resistive,               !- Defrost Strategy",
        "  Timed,                   !- Defrost Control",
        "  ,                        !- Defrost Energy Input Ratio Modifier Function of Temperature Curve Name",
        "  ,                        !- Defrost Time Period Fraction {dimensionless}",
        "  15000.0,                 !- Resistive Defrost Heater Capacity {W}",
        "  7.0,                     !- Maximum Outdoor Dry-bulb Temperature for Defrost Operation {C}",
        "  AirCooled,               !- Condenser Type",
        "  MyVRFOANode,             !- Condenser Inlet Node Name",
        "  ,                        !- Condenser Outlet Node Name",
        "  ,                        !- Water Condenser Volume Flow Rate {m3/s}",
        "  ,                        !- Evaporative Condenser Effectiveness {dimensionless}",
        "  ,                        !- Evaporative Condenser Air Flow Rate {m3/s}",
        "  0.0,                     !- Evaporative Condenser Pump Rated Power Consumption {W}",
        "  ,                        !- Supply Water Storage Tank Name",
        "  0.0,                     !- Basin Heater Capacity {W/K}",
        "  ,                        !- Basin Heater Setpoint Temperature {C}",
        "  ,                        !- Basin Heater Operating Schedule Name",
        "  Electricity;             !- Fuel Type",

        "Curve:Biquadratic,",
        "  VRFCoolCapFT,            !- Name",
        "  0.576882692,             !- Coefficient1 Constant",
        "  0.017447952,             !- Coefficient2 x",
        "  0.000583269,             !- Coefficient3 x**2",
        "  -1.76324e-06,            !- Coefficient4 y",
        "  -7.474e-09,              !- Coefficient5 y**2",
        "  -1.30413e-07,            !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  24.0,                    !- Maximum Value of x",
        "  -5.0,                    !- Minimum Value of y",
        "  23.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Cubic,",
        "  VRFCoolCapFTBoundary,    !- Name",
        "  25.73473775,             !- Coefficient1 Constant",
        "  -0.03150043,             !- Coefficient2 x",
        "  -0.01416595,             !- Coefficient3 x**2",
        "  0.0,                     !- Coefficient4 x**3",
        "  11.0,                    !- Minimum Value of x",
        "  30.0,                    !- Maximum Value of x",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature;             !- Output Unit Type",

        "Curve:Biquadratic,",
        "  VRFCoolCapFTHi,          !- Name",
        "  0.6867358,               !- Coefficient1 Constant",
        "  0.0207631,               !- Coefficient2 x",
        "  0.0005447,               !- Coefficient3 x**2",
        "  -0.0016218,              !- Coefficient4 y",
        "  -4.259e-07,              !- Coefficient5 y**2",
        "  -0.0003392,              !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  24.0,                    !- Maximum Value of x",
        "  16.0,                    !- Minimum Value of y",
        "  43.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Biquadratic,",
        "  VRFCoolEIRFT,            !- Name",
        "  0.989010541,             !- Coefficient1 Constant",
        "  -0.02347967,             !- Coefficient2 x",
        "  0.000199711,             !- Coefficient3 x**2",
        "  0.005968336,             !- Coefficient4 y",
        "  -1.0289e-07,             !- Coefficient5 y**2",
        "  -0.00015686,             !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  24.0,                    !- Maximum Value of x",
        "  -5.0,                    !- Minimum Value of y",
        "  23.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Cubic,",
        "  VRFCoolEIRFTBoundary,    !- Name",
        "  25.73473775,             !- Coefficient1 Constant",
        "  -0.03150043,             !- Coefficient2 x",
        "  -0.01416595,             !- Coefficient3 x**2",
        "  0.0,                     !- Coefficient4 x**3",
        "  15.0,                    !- Minimum Value of x",
        "  24.0,                    !- Maximum Value of x",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature;             !- Output Unit Type",

        "Curve:Biquadratic,",
        "  VRFCoolEIRFTHi,          !- Name",
        "  0.1435147,               !- Coefficient1 Constant",
        "  0.01860035,              !- Coefficient2 x",
        "  -0.0003954,              !- Coefficient3 x**2",
        "  0.02485219,              !- Coefficient4 y",
        "  0.00016329,              !- Coefficient5 y**2",
        "  -0.0006244,              !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  24.0,                    !- Maximum Value of x",
        "  16.0,                    !- Minimum Value of y",
        "  43.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Cubic,",
        "  CoolingEIRLowPLR,        !- Name",
        "  0.4628123,               !- Coefficient1 Constant",
        "  -1.0402406,              !- Coefficient2 x",
        "  2.17490997,              !- Coefficient3 x**2",
        "  -0.5974817,              !- Coefficient4 x**3",
        "  0.0,                     !- Minimum Value of x",
        "  1.0,                     !- Maximum Value of x",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature;             !- Output Unit Type",

        "Curve:Quadratic,",
        "  CoolingEIRHiPLR,         !- Name",
        "  1.0,                     !- Coefficient1 Constant",
        "  0.0,                     !- Coefficient2 x",
        "  0.0,                     !- Coefficient3 x**2",
        "  1.0,                     !- Minimum Value of x",
        "  1.5,                     !- Maximum Value of x",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Dimensionless,           !- Input Unit Type for X",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Linear,",
        "  CoolingCombRatio,        !- Name",
        "  0.618055,                !- Coefficient1 Constant",
        "  0.381945,                !- Coefficient2 x",
        "  1.0,                     !- Minimum Value of x",
        "  1.5,                     !- Maximum Value of x",
        "  1.0,                     !- Minimum Curve Output",
        "  1.2,                     !- Maximum Curve Output",
        "  Dimensionless,           !- Input Unit Type for X",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Quadratic,",
        "  VRFCPLFFPLR,             !- Name",
        "  0.85,                    !- Coefficient1 Constant",
        "  0.15,                    !- Coefficient2 x",
        "  0.0,                     !- Coefficient3 x**2",
        "  0.0,                     !- Minimum Value of x",
        "  1.0,                     !- Maximum Value of x",
        "  0.85,                    !- Minimum Curve Output",
        "  1.0,                     !- Maximum Curve Output",
        "  Dimensionless,           !- Input Unit Type for X",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Biquadratic,",
        "  VRFHeatCapFT,            !- Name",
        "  1.014599599,             !- Coefficient1 Constant",
        "  -0.002506703,            !- Coefficient2 x",
        "  -0.000141599,            !- Coefficient3 x**2",
        "  0.026931595,             !- Coefficient4 y",
        "  1.83538e-06,             !- Coefficient5 y**2",
        "  -0.000358147,            !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  27.0,                    !- Maximum Value of x",
        "  -20.0,                   !- Minimum Value of y",
        "  15.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Cubic,",
        "  VRFHeatCapFTBoundary,    !- Name",
        "  -7.6000882,              !- Coefficient1 Constant",
        "  3.05090016,              !- Coefficient2 x",
        "  -0.1162844,              !- Coefficient3 x**2",
        "  0.0,                     !- Coefficient4 x**3",
        "  15.0,                    !- Minimum Value of x",
        "  27.0,                    !- Maximum Value of x",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature;             !- Output Unit Type",

        "Curve:Biquadratic,",
        "  VRFHeatCapFTHi,          !- Name",
        "  1.161134821,             !- Coefficient1 Constant",
        "  0.027478868,             !- Coefficient2 x",
        "  -0.00168795,             !- Coefficient3 x**2",
        "  0.001783378,             !- Coefficient4 y",
        "  2.03208e-06,             !- Coefficient5 y**2",
        "  -6.8969e-05,             !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  27.0,                    !- Maximum Value of x",
        "  -10.0,                   !- Minimum Value of y",
        "  15.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Biquadratic,",
        "  VRFHeatEIRFT,            !- Name",
        "  0.87465501,              !- Coefficient1 Constant",
        "  -0.01319754,             !- Coefficient2 x",
        "  0.00110307,              !- Coefficient3 x**2",
        "  -0.0133118,              !- Coefficient4 y",
        "  0.00089017,              !- Coefficient5 y**2",
        "  -0.00012766,             !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  27.0,                    !- Maximum Value of x",
        "  -20.0,                   !- Minimum Value of y",
        "  12.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Cubic,",
        "  VRFHeatEIRFTBoundary,    !- Name",
        "  -7.6000882,              !- Coefficient1 Constant",
        "  3.05090016,              !- Coefficient2 x",
        "  -0.1162844,              !- Coefficient3 x**2",
        "  0.0,                     !- Coefficient4 x**3",
        "  15.0,                    !- Minimum Value of x",
        "  27.0,                    !- Maximum Value of x",
        "  -20.0,                   !- Minimum Curve Output",
        "  15.0,                    !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature;             !- Output Unit Type",

        "Curve:Biquadratic,",
        "  VRFHeatEIRFTHi,          !- Name",
        "  2.504005146,             !- Coefficient1 Constant",
        "  -0.05736767,             !- Coefficient2 x",
        "  4.07336e-05,             !- Coefficient3 x**2",
        "  -0.12959669,             !- Coefficient4 y",
        "  0.00135839,              !- Coefficient5 y**2",
        "  0.00317047,              !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  27.0,                    !- Maximum Value of x",
        "  -10.0,                   !- Minimum Value of y",
        "  15.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Cubic,",
        "  HeatingEIRLowPLR,        !- Name",
        "  0.1400093,               !- Coefficient1 Constant",
        "  0.6415002,               !- Coefficient2 x",
        "  0.1339047,               !- Coefficient3 x**2",
        "  0.0845859,               !- Coefficient4 x**3",
        "  0.0,                     !- Minimum Value of x",
        "  1.0,                     !- Maximum Value of x",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Dimensionless,           !- Input Unit Type for X",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Quadratic,",
        "  HeatingEIRHiPLR,         !- Name",
        "  2.4294355,               !- Coefficient1 Constant",
        "  -2.235887,               !- Coefficient2 x",
        "  0.8064516,               !- Coefficient3 x**2",
        "  1.0,                     !- Minimum Value of x",
        "  1.5,                     !- Maximum Value of x",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Dimensionless,           !- Input Unit Type for X",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Linear,",
        "  HeatingCombRatio,        !- Name",
        "  0.96034,                 !- Coefficient1 Constant",
        "  0.03966,                 !- Coefficient2 x",
        "  1.0,                     !- Minimum Value of x",
        "  1.5,                     !- Maximum Value of x",
        "  1.0,                     !- Minimum Curve Output",
        "  1.023,                   !- Maximum Curve Output",
        "  Dimensionless,           !- Input Unit Type for X",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Biquadratic,",
        "  CoolingLengthCorrectionFactor,  !- Name",
        "  1.0693794,               !- Coefficient1 Constant",
        "  -0.0014951,              !- Coefficient2 x",
        "  2.56e-06,                !- Coefficient3 x**2",
        "  -0.1151104,              !- Coefficient4 y",
        "  0.0511169,               !- Coefficient5 y**2",
        "  -0.0004369,              !- Coefficient6 x*y",
        "  8.0,                     !- Minimum Value of x",
        "  175.0,                   !- Maximum Value of x",
        "  0.5,                     !- Minimum Value of y",
        "  1.5,                     !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "ZoneTerminalUnitList,",
        "  VRF Heat Pump TU List,   !- Zone Terminal Unit List Name",
        "  TU1;                     !- Zone Terminal Unit Name 1",

        "OutdoorAir:NodeList,",
        "  OutsideAirInletNodes;    !- Node or NodeList Name 1",

        "NodeList,",
        "  OutsideAirInletNodes,    !- Name",
        "  MyVRFOANode;             !- Node 1 Name",
    });

    ASSERT_TRUE(process_idf(idf_objects));
    state->dataGlobal->TimeStepsInHour = 1;
    state->dataGlobal->MinutesInTimeStep = 60;
    state->init_state(*state);

    // set input variables
    state->dataEnvrn->OutBaroPress = 101325.0;
    state->dataEnvrn->OutDryBulbTemp = 35.0;
    state->dataEnvrn->OutHumRat = 0.0098;
    state->dataEnvrn->OutEnthalpy = Psychrometrics::PsyHFnTdbW(state->dataEnvrn->OutDryBulbTemp, state->dataEnvrn->OutHumRat);
    state->dataEnvrn->StdRhoAir = 1.20;

    state->dataGlobal->TimeStep = 1;

    GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    GetZoneEquipmentData(*state);
    GetZoneAirLoopEquipment(*state);

    GetVRFInput(*state);
    state->dataHVACVarRefFlow->GetVRFInputFlag = false;

    // get input test for terminal air single duct mixer on inlet side of VRF terminal unit
    ASSERT_EQ(1, state->dataSingleDuct->NumATMixers);
    EXPECT_EQ("SPACE1-1 DOAS AIR TERMINAL", state->dataSingleDuct->SysATMixer(1).Name);                 // single duct air terminal mixer name
    EXPECT_EQ((int)HVAC::MixerType::InletSide, (int)state->dataSingleDuct->SysATMixer(1).type);         // air terminal mixer connection type
    EXPECT_EQ("AIRTERMINAL:SINGLEDUCT:MIXER", state->dataDefineEquipment->AirDistUnit(1).EquipType(1)); // Air distribution unit equipment type
    EXPECT_EQ("TU1", state->dataHVACVarRefFlow->VRFTU(1).Name);                                         // zoneHVAC equipment name
    // EXPECT_EQ( "ZoneHVAC:TerminalUnit:VariableRefrigerantFlow", VRFTU( 1 ).Name ); // zoneHVAC equipment type

    state->dataGlobal->BeginEnvrnFlag = false;

    HVACInletMassFlowRate = 0.50;
    PrimaryAirMassFlowRate = 0.1;
    SecondaryAirMassFlowRate = HVACInletMassFlowRate - PrimaryAirMassFlowRate; // secondary air flow is VRFTU flow less primary air flow

    // set zoneNode air condition
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp = 24.0;
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat = 0.0075;
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Enthalpy =
        Psychrometrics::PsyHFnTdbW(state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp,
                                   state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat);

    state->dataHVACVarRefFlow->CoolingLoad.allocate(1);
    state->dataHVACVarRefFlow->HeatingLoad.allocate(1);
    state->dataHVACVarRefFlow->HeatingLoad(1) = false;
    state->dataHVACVarRefFlow->CoolingLoad(1) = true;
    state->dataHVACVarRefFlow->CompOnMassFlow = HVACInletMassFlowRate;    // supply air mass flow rate
    state->dataHVACVarRefFlow->OACompOnMassFlow = PrimaryAirMassFlowRate; // OA mass flow rate
    state->dataHVACVarRefFlow->CompOnFlowRatio = 1.0;                     // compressor is on
    state->dataHVACGlobal->TurnFansOff = false;
    state->dataHVACGlobal->TurnFansOn = true;

    VRFNum = 1;
    VRFTUNum = 1;
    state->dataHVACVarRefFlow->VRFTU(VRFTUNum).fanOp = HVAC::FanOp::Continuous;
    state->dataHVACVarRefFlow->VRFTU(VRFTUNum).isInZone = true;
    state->dataHVACVarRefFlow->VRFTU(VRFTUNum).ZoneAirNode = state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode;
    // VRFTU( VRFTUNum ).VRFTUOutletNodeNum
    // initialize mass flow rates
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).VRFTUInletNodeNum).MassFlowRate = HVACInletMassFlowRate;
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).ATMixerPriNode).MassFlowRate = PrimaryAirMassFlowRate;
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).ATMixerPriNode).MassFlowRateMaxAvail = PrimaryAirMassFlowRate;

    // set fan parameters
    state->dataFans->fans(1)->maxAirMassFlowRate = HVACInletMassFlowRate;
    state->dataFans->fans(1)->inletAirMassFlowRate = HVACInletMassFlowRate;
    state->dataFans->fans(1)->rhoAirStdInit = state->dataEnvrn->StdRhoAir;
    state->dataLoopNodes->Node(state->dataFans->fans(1)->inletNodeNum).MassFlowRateMaxAvail = HVACInletMassFlowRate;
    state->dataLoopNodes->Node(state->dataFans->fans(1)->outletNodeNum).MassFlowRateMax = HVACInletMassFlowRate;

    // set DX coil rated performance parameters
    state->dataDXCoils->DXCoil(1).RatedCBF(1) = 0.05;
    state->dataDXCoils->DXCoil(1).RatedAirMassFlowRate(1) = HVACInletMassFlowRate;

    // primary air condition set at outdoor air condition
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).ATMixerPriNode).Temp = state->dataEnvrn->OutDryBulbTemp;
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).ATMixerPriNode).HumRat = state->dataEnvrn->OutHumRat;
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).ATMixerPriNode).Enthalpy = state->dataEnvrn->OutEnthalpy;

    // set secondary air (recirculating air) conditions to zone air node
    state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).Temp =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp;
    state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).HumRat =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat;
    state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).Enthalpy =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Enthalpy;

    state->dataHVACVarRefFlow->VRFTU(1).ZoneNum = 1;
    state->dataSize->SysSizingRunDone = true;
    state->dataSize->ZoneSizingRunDone = true;
    state->dataGlobal->SysSizingCalc = true;

    state->dataZoneEnergyDemand->ZoneSysEnergyDemand.allocate(1);
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputReqToHeatSP = 0.0;
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputReqToCoolSP = -5000.0;
    QZnReq = state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputReqToCoolSP;

    state->dataHVACVarRefFlow->VRFTU(VRFTUNum).availSched->currentVal = 1.0;    // unit is always available
    state->dataHVACVarRefFlow->VRFTU(VRFTUNum).fanAvailSched->currentVal = 1.0; // fan is always available

    // set secondary air mass flow rate to zero
    state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).MassFlowRate = 0.0;
    // Simulate zoneHVAC equipment (VRF terminal unit)
    SimVRF(*state, VRFTUNum, FirstHVACIteration, OnOffAirFlowRatio, QUnitOutVRFTU, LatOutputProvided, QZnReq);
    // check the terminal air mixer secondary air mass flow rate
    ASSERT_EQ(SecondaryAirMassFlowRate, state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).MassFlowRate);
    // check the terminal air mixer outlet flow rate must be equal to VRFTU mass flow rate
    ASSERT_EQ(HVACInletMassFlowRate, state->dataSingleDuct->SysATMixer(1).MixedAirMassFlowRate);
    // check the cooling output delivered is within 2.0 Watt of zone cooling load
    ASSERT_NEAR(QZnReq, QUnitOutVRFTU, 2.0);
}

TEST_F(EnergyPlusFixture, AirTerminalSingleDuctMixer_SimVRF_ATMSupplySide)
{

    bool ErrorsFound(false);
    bool FirstHVACIteration(false);
    Real64 HVACInletMassFlowRate(0.0);
    Real64 PrimaryAirMassFlowRate(0.0);
    Real64 SecondaryAirMassFlowRate(0.0);
    Real64 ATMixerOutletMassFlowRate(0.0);
    Real64 OnOffAirFlowRatio(1.0);
    Real64 LatOutputProvided(0.0);
    Real64 QUnitOutVRFTU(0.0);
    Real64 QZnReq(0.0);
    int VRFNum(1);
    int VRFTUNum(1);

    std::string const idf_objects = delimited_string({
        "AirTerminal:SingleDuct:Mixer,",
        "  SPACE1-1 DOAS Air Terminal,  !- Name",
        "  ZoneHVAC:TerminalUnit:VariableRefrigerantFlow,  !- ZoneHVAC Unit Object Type",
        "  TU1,                     !- ZoneHVAC Unit Object Name",
        "  TU1 Outlet Node,         !- Mixer Outlet Node Name",
        "  SPACE1-1 Air Terminal Mixer Primary Inlet,  !- Mixer Primary Air Inlet Node Name",
        "  SPACE1-1 Air Terminal Mixer Secondary Inlet,  !- Mixer Secondary Air Inlet Node Name",
        "  SupplySide;              !- Mixer Connection Type",

        "ZoneHVAC:AirDistributionUnit,",
        "  SPACE1-1 DOAS ATU,       !- Name",
        "  TU1 Outlet Node,         !- Air Distribution Unit Outlet Node Name",
        "  AirTerminal:SingleDuct:Mixer,  !- Air Terminal Object Type",
        "  SPACE1-1 DOAS Air Terminal;  !- Air Terminal Name",

        "Schedule:Compact,",
        "  FanAvailSched,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,1.0;        !- Field 3",

        "Schedule:Compact,",
        "  VRFAvailSched,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,1.0;        !- Field 3",

        "Schedule:Compact,",
        "  CyclingFanSch,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,0.0;        !- Field 3",

        "ZoneHVAC:EquipmentList,",
        "  SPACE1-1 Eq,             !- Name",
        "  SequentialLoad,          !- Load Distribution Scheme",
        "  ZoneHVAC:AirDistributionUnit,  !- Zone Equipment 1 Object Type",
        "  SPACE1-1 DOAS ATU,       !- Zone Equipment 1 Name",
        "  1,                       !- Zone Equipment 1 Cooling Sequence",
        "  1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 1 Sequential Cooling Fraction Schedule Name",
        "  ,                        !- Zone Equipment 1 Sequential Heating Fraction Schedule Name",
        "  ZoneHVAC:TerminalUnit:VariableRefrigerantFlow,  !- Zone Equipment 2 Object Type",
        "  TU1,                     !- Zone Equipment 2 Name",
        "  2,                       !- Zone Equipment 2 Cooling Sequence",
        "  2,                       !- Zone Equipment 2 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 2 Sequential Cooling Fraction Schedule Name",
        "  ;                        !- Zone Equipment 2 Sequential Heating Fraction Schedule Name",

        "ZoneHVAC:TerminalUnit:VariableRefrigerantFlow,",
        "  TU1,                     !- Zone Terminal Unit Name",
        "  VRFAvailSched,           !- Terminal Unit Availability Schedule",
        "  zTU1 Inlet Node,         !- Terminal Unit Air Inlet Node Name",
        "  SPACE1-1 Air Terminal Mixer Secondary Inlet,  !- Terminal Unit Air Outlet Node Name",
        "  0.5,                     !- Cooling Supply Air Flow Rate {m3/s}",
        "  0.5,                     !- No Cooling Supply Air Flow Rate {m3/s}",
        "  0.5,                     !- Heating Supply Air Flow Rate {m3/s}",
        "  0.5,                     !- No Heating Supply Air Flow Rate {m3/s}",
        "  0.0,                     !- Cooling Outdoor Air Flow Rate {m3/s}",
        "  0.0,                     !- Heating Outdoor Air Flow Rate {m3/s}",
        "  0.0,                     !- No Load Outdoor Air Flow Rate {m3/s}",
        "  VRFFanSchedule,          !- Supply Air Fan Operating Mode Schedule Name",
        "  DrawThrough,             !- Supply Air Fan Placement",
        "  Fan:ConstantVolume,      !- Supply Air Fan Object Type",
        "  TU1 VRF Supply Fan,      !- Supply Air Fan Object Name",
        "  ,                        !- Outside Air Mixer Object Type",
        "  ,                        !- Outside Air Mixer Object Name",
        "  Coil:Cooling:DX:VariableRefrigerantFlow,  !- Cooling Coil Object Type",
        "  TU1 VRF DX Cooling Coil,  !- Cooling Coil Object Name",
        "  Coil:Heating:DX:VariableRefrigerantFlow,  !- Heating Coil Object Type",
        "  TU1 VRF DX Heating Coil,  !- Heating Coil Object Name",
        "  30.0,                    !- Zone Terminal Unit On Parasitic Electric Energy Use {W}",
        "  20.0,                    !- Zone Terminal Unit Off Parasitic Electric Energy Use {W}",
        "  ,                        !- Rated Heating Capacity Sizing Ratio {W/W}",
        "  ,                        !- Availability Manager List Name",
        "  ,                        !- Design Specification ZoneHVAC Sizing Object Name",
        "  ,                        !- Supplemental Heating Coil Object Type",
        "  ,                        !- Supplemental Heating Coil Name",
        "  ,                        !- Maximum Supply Air Temperature from Supplemental Heater {C}",
        "  ;                        !- Maximum Outdoor Dry-Bulb Temperature for Supplemental Heater Operation {C}",

        "Fan:ConstantVolume,",
        "  TU1 VRF Supply Fan,      !- Name",
        "  VRFAvailSched,           !- Availability Schedule Name",
        "  0.7,                     !- Fan Total Efficiency",
        "  600.0,                   !- Pressure Rise {Pa}",
        "  0.5,                     !- Maximum Flow Rate {m3/s}",
        "  0.9,                     !- Motor Efficiency",
        "  1.0,                     !- Motor In Airstream Fraction",
        "  TU1 VRF DX HCoil Outlet Node,  !- Air Inlet Node Name",
        "  SPACE1-1 Air Terminal Mixer Secondary Inlet;  !- Air Outlet Node Name",

        "Coil:Heating:DX:VariableRefrigerantFlow,",
        "  TU1 VRF DX Heating Coil,  !- Name",
        "  VRFAvailSched,           !- Availability Schedule",
        "  7000.0,                  !- Gross Rated Heating Capacity {W}",
        "  0.5,                     !- Rated Air Flow Rate {m3/s}",
        "  TU1 VRF DX CCoil Outlet Node,  !- Coil Air Inlet Node",
        "  TU1 VRF DX HCoil Outlet Node,  !- Coil Air Outlet Node",
        "  VRFTUHeatCapFT,          !- Heating Capacity Ratio Modifier Function of Temperature Curve Name",
        "  VRFACCoolCapFFF;         !- Heating Capacity Modifier Function of Flow Fraction Curve Name",

        "Curve:Cubic,",
        "  VRFTUHeatCapFT,          !- Name",
        "  -0.390708928227928,      !- Coefficient1 Constant",
        "  0.261815023760162,       !- Coefficient2 x",
        "  -0.0130431603151873,     !- Coefficient3 x**2",
        "  0.000178131745997821,    !- Coefficient4 x**3",
        "  0.0,                     !- Minimum Value of x",
        "  50.0,                    !- Maximum Value of x",
        "  0.5,                     !- Minimum Curve Output",
        "  1.5,                     !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Dimensionless;           !- Output Unit Type",

        "Coil:Cooling:DX:VariableRefrigerantFlow,",
        "  TU1 VRF DX Cooling Coil,  !- Name",
        "  VRFAvailSched,           !- Availability Schedule Name",
        "  6600.0,                  !- Gross Rated Total Cooling Capacity {W}",
        "  0.75,                    !- Gross Rated Sensible Heat Ratio",
        "  0.5,                     !- Rated Air Flow Rate {m3/s}",
        "  VRFTUCoolCapFT,          !- Cooling Capacity Ratio Modifier Function of Temperature Curve Name",
        "  VRFACCoolCapFFF,         !- Cooling Capacity Modifier Curve Function of Flow Fraction Name",
        "  zTU1 Inlet Node,         !- Coil Air Inlet Node",
        "  TU1 VRF DX CCoil Outlet Node,  !- Coil Air Outlet Node",
        "  ;                        !- Name of Water Storage Tank for Condensate Collection",

        "Curve:Cubic,",
        "  VRFTUCoolCapFT,          !- Name",
        "  0.504547273506488,       !- Coefficient1 Constant",
        "  0.0288891279198444,      !- Coefficient2 x",
        "  -1.0819418650677e-05,    !- Coefficient3 x**2",
        "  1.01359395177008e-05,    !- Coefficient4 x**3",
        "  0.0,                     !- Minimum Value of x",
        "  50.0,                    !- Maximum Value of x",
        "  0.5,                     !- Minimum Curve Output",
        "  1.5,                     !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Quadratic,",
        "  VRFACCoolCapFFF,         !- Name",
        "  0.8,                     !- Coefficient1 Constant",
        "  0.2,                     !- Coefficient2 x",
        "  0.0,                     !- Coefficient3 x**2",
        "  0.5,                     !- Minimum Value of x",
        "  1.5;                     !- Maximum Value of x",

        "Zone,",
        "  SPACE1-1,                !- Name",
        "  0.0,                     !- Direction of Relative North {deg}",
        "  0.0,                     !- X Origin {m}",
        "  0.0,                     !- Y Origin {m}",
        "  0.0,                     !- Z Origin {m}",
        "  1,                       !- Type",
        "  1,                       !- Multiplier",
        "  2.438400269,             !- Ceiling Height {m}",
        "  239.247360229;           !- Volume {m3}",

        "ZoneHVAC:EquipmentConnections,",
        "  SPACE1-1,                !- Zone Name",
        "  SPACE1-1 Eq,             !- Zone Conditioning Equipment List Name",
        "  SPACE1-1 In Nodes,       !- Zone Air Inlet Node or NodeList Name",
        "  zTU1 Inlet Node,         !- Zone Air Exhaust Node or NodeList Name",
        "  SPACE1-1 Node,           !- Zone Air Node Name",
        "  SPACE1-1 RETURN OUTLET;  !- Zone Return Air Node or NodeList Name",

        "NodeList,",
        "  SPACE1-1 In Nodes,       !- Name",
        "  TU1 Outlet Node;         !- Node 1 Name",

        "AirConditioner:VariableRefrigerantFlow,",
        "  VRF Heat Pump,           !- Heat Pump Name",
        "  VRFAvailSched,           !- Availability Schedule Name",
        "  15000.0,                 !- Gross Rated Total Cooling Capacity {W}",
        "  3.2917,                  !- Gross Rated Cooling COP {W/W}",
        "  -5.0,                    !- Minimum Condenser Inlet Node Temperature in Cooling Mode {C}",
        "  43.0,                    !- Maximum Condenser Inlet Node Temperature in Cooling Mode {C}",
        "  VRFCoolCapFT,            !- Cooling Capacity Ratio Modifier Function of Low Temperature Curve Name",
        "  VRFCoolCapFTBoundary,    !- Cooling Capacity Ratio Boundary Curve Name",
        "  VRFCoolCapFTHi,          !- Cooling Capacity Ratio Modifier Function of High Temperature Curve Name",
        "  VRFCoolEIRFT,            !- Cooling Energy Input Ratio Modifier Function of Low Temperature Curve Name",
        "  VRFCoolEIRFTBoundary,    !- Cooling Energy Input Ratio Boundary Curve Name",
        "  VRFCoolEIRFTHi,          !- Cooling Energy Input Ratio Modifier Function of High Temperature Curve Name",
        "  CoolingEIRLowPLR,        !- Cooling Energy Input Ratio Modifier Function of Low Part-Load Ratio Curve Name",
        "  CoolingEIRHiPLR,         !- Cooling Energy Input Ratio Modifier Function of High Part-Load Ratio Curve Name",
        "  CoolingCombRatio,        !- Cooling Combination Ratio Correction Factor Curve Name",
        "  VRFCPLFFPLR,             !- Cooling Part-Load Fraction Correlation Curve Name",
        "  15000.0,                 !- Gross Rated Heating Capacity {W}",
        "  ,                        !- Rated Heating Capacity Sizing Ratio {W/W}",
        "  3.5484,                  !- Gross Rated Heating COP {W/W}",
        "  -20.0,                   !- Minimum Condenser Inlet Node Temperature in Heating Mode {C}",
        "  20.0,                    !- Maximum Condenser Inlet Node Temperature in Heating Mode {C}",
        "  VRFHeatCapFT,            !- Heating Capacity Ratio Modifier Function of Low Temperature Curve Name",
        "  VRFHeatCapFTBoundary,    !- Heating Capacity Ratio Boundary Curve Name",
        "  VRFHeatCapFTHi,          !- Heating Capacity Ratio Modifier Function of High Temperature Curve Name",
        "  VRFHeatEIRFT,            !- Heating Energy Input Ratio Modifier Function of Low Temperature Curve Name",
        "  VRFHeatEIRFTBoundary,    !- Heating Energy Input Ratio Boundary Curve Name",
        "  VRFHeatEIRFTHi,          !- Heating Energy Input Ratio Modifier Function of High Temperature Curve Name",
        "  WetBulbTemperature,      !- Heating Performance Curve Outdoor Temperature Type",
        "  HeatingEIRLowPLR,        !- Heating Energy Input Ratio Modifier Function of Low Part-Load Ratio Curve Name",
        "  HeatingEIRHiPLR,         !- Heating Energy Input Ratio Modifier Function of High Part-Load Ratio Curve Name",
        "  HeatingCombRatio,        !- Heating Combination Ratio Correction Factor Curve Name",
        "  VRFCPLFFPLR,             !- Heating Part-Load Fraction Correlation Curve Name",
        "  0.25,                    !- Minimum Heat Pump Part-Load Ratio {dimensionless}",
        "  SPACE1-1,                !- Zone Name for Master Thermostat Location",
        "  LoadPriority,            !- Master Thermostat Priority Control Type",
        "  ,                        !- Thermostat Priority Schedule Name",
        "  VRF Heat Pump TU List,   !- Zone Terminal Unit List Name",
        "  No,                      !- Heat Pump Waste Heat Recovery",
        "  30.0,                    !- Equivalent Piping Length used for Piping Correction Factor in Cooling Mode {m}",
        "  10.0,                    !- Vertical Height used for Piping Correction Factor {m}",
        "  CoolingLengthCorrectionFactor,  !- Piping Correction Factor for Length in Cooling Mode Curve Name",
        "  -0.000386,               !- Piping Correction Factor for Height in Cooling Mode Coefficient {1/m}",
        "  30.0,                    !- Equivalent Piping Length used for Piping Correction Factor in Heating Mode {m}",
        "  ,                        !- Piping Correction Factor for Length in Heating Mode Curve Name",
        "  ,                        !- Piping Correction Factor for Height in Heating Mode Coefficient {1/m}",
        "  15.0,                    !- Crankcase Heater Power per Compressor {W}",
        "  3,                       !- Number of Compressors {dimensionless}",
        "  0.33,                    !- Ratio of Compressor Size to Total Compressor Capacity {W/W}",
        "  7.0,                     !- Maximum Outdoor Dry-Bulb Temperature for Crankcase Heater {C}",
        "  Resistive,               !- Defrost Strategy",
        "  Timed,                   !- Defrost Control",
        "  ,                        !- Defrost Energy Input Ratio Modifier Function of Temperature Curve Name",
        "  ,                        !- Defrost Time Period Fraction {dimensionless}",
        "  15000.0,                 !- Resistive Defrost Heater Capacity {W}",
        "  7.0,                     !- Maximum Outdoor Dry-bulb Temperature for Defrost Operation {C}",
        "  AirCooled,               !- Condenser Type",
        "  MyVRFOANode,             !- Condenser Inlet Node Name",
        "  ,                        !- Condenser Outlet Node Name",
        "  ,                        !- Water Condenser Volume Flow Rate {m3/s}",
        "  ,                        !- Evaporative Condenser Effectiveness {dimensionless}",
        "  ,                        !- Evaporative Condenser Air Flow Rate {m3/s}",
        "  0.0,                     !- Evaporative Condenser Pump Rated Power Consumption {W}",
        "  ,                        !- Supply Water Storage Tank Name",
        "  0.0,                     !- Basin Heater Capacity {W/K}",
        "  ,                        !- Basin Heater Setpoint Temperature {C}",
        "  ,                        !- Basin Heater Operating Schedule Name",
        "  Electricity;             !- Fuel Type",

        "Curve:Biquadratic,",
        "  VRFCoolCapFT,            !- Name",
        "  0.576882692,             !- Coefficient1 Constant",
        "  0.017447952,             !- Coefficient2 x",
        "  0.000583269,             !- Coefficient3 x**2",
        "  -1.76324e-06,            !- Coefficient4 y",
        "  -7.474e-09,              !- Coefficient5 y**2",
        "  -1.30413e-07,            !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  24.0,                    !- Maximum Value of x",
        "  -5.0,                    !- Minimum Value of y",
        "  23.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Cubic,",
        "  VRFCoolCapFTBoundary,    !- Name",
        "  25.73473775,             !- Coefficient1 Constant",
        "  -0.03150043,             !- Coefficient2 x",
        "  -0.01416595,             !- Coefficient3 x**2",
        "  0.0,                     !- Coefficient4 x**3",
        "  11.0,                    !- Minimum Value of x",
        "  30.0,                    !- Maximum Value of x",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature;             !- Output Unit Type",

        "Curve:Biquadratic,",
        "  VRFCoolCapFTHi,          !- Name",
        "  0.6867358,               !- Coefficient1 Constant",
        "  0.0207631,               !- Coefficient2 x",
        "  0.0005447,               !- Coefficient3 x**2",
        "  -0.0016218,              !- Coefficient4 y",
        "  -4.259e-07,              !- Coefficient5 y**2",
        "  -0.0003392,              !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  24.0,                    !- Maximum Value of x",
        "  16.0,                    !- Minimum Value of y",
        "  43.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Biquadratic,",
        "  VRFCoolEIRFT,            !- Name",
        "  0.989010541,             !- Coefficient1 Constant",
        "  -0.02347967,             !- Coefficient2 x",
        "  0.000199711,             !- Coefficient3 x**2",
        "  0.005968336,             !- Coefficient4 y",
        "  -1.0289e-07,             !- Coefficient5 y**2",
        "  -0.00015686,             !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  24.0,                    !- Maximum Value of x",
        "  -5.0,                    !- Minimum Value of y",
        "  23.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Cubic,",
        "  VRFCoolEIRFTBoundary,    !- Name",
        "  25.73473775,             !- Coefficient1 Constant",
        "  -0.03150043,             !- Coefficient2 x",
        "  -0.01416595,             !- Coefficient3 x**2",
        "  0.0,                     !- Coefficient4 x**3",
        "  15.0,                    !- Minimum Value of x",
        "  24.0,                    !- Maximum Value of x",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature;             !- Output Unit Type",

        "Curve:Biquadratic,",
        "  VRFCoolEIRFTHi,          !- Name",
        "  0.1435147,               !- Coefficient1 Constant",
        "  0.01860035,              !- Coefficient2 x",
        "  -0.0003954,              !- Coefficient3 x**2",
        "  0.02485219,              !- Coefficient4 y",
        "  0.00016329,              !- Coefficient5 y**2",
        "  -0.0006244,              !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  24.0,                    !- Maximum Value of x",
        "  16.0,                    !- Minimum Value of y",
        "  43.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Cubic,",
        "  CoolingEIRLowPLR,        !- Name",
        "  0.4628123,               !- Coefficient1 Constant",
        "  -1.0402406,              !- Coefficient2 x",
        "  2.17490997,              !- Coefficient3 x**2",
        "  -0.5974817,              !- Coefficient4 x**3",
        "  0.0,                     !- Minimum Value of x",
        "  1.0,                     !- Maximum Value of x",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature;             !- Output Unit Type",

        "Curve:Quadratic,",
        "  CoolingEIRHiPLR,         !- Name",
        "  1.0,                     !- Coefficient1 Constant",
        "  0.0,                     !- Coefficient2 x",
        "  0.0,                     !- Coefficient3 x**2",
        "  1.0,                     !- Minimum Value of x",
        "  1.5,                     !- Maximum Value of x",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Dimensionless,           !- Input Unit Type for X",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Linear,",
        "  CoolingCombRatio,        !- Name",
        "  0.618055,                !- Coefficient1 Constant",
        "  0.381945,                !- Coefficient2 x",
        "  1.0,                     !- Minimum Value of x",
        "  1.5,                     !- Maximum Value of x",
        "  1.0,                     !- Minimum Curve Output",
        "  1.2,                     !- Maximum Curve Output",
        "  Dimensionless,           !- Input Unit Type for X",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Quadratic,",
        "  VRFCPLFFPLR,             !- Name",
        "  0.85,                    !- Coefficient1 Constant",
        "  0.15,                    !- Coefficient2 x",
        "  0.0,                     !- Coefficient3 x**2",
        "  0.0,                     !- Minimum Value of x",
        "  1.0,                     !- Maximum Value of x",
        "  0.85,                    !- Minimum Curve Output",
        "  1.0,                     !- Maximum Curve Output",
        "  Dimensionless,           !- Input Unit Type for X",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Biquadratic,",
        "  VRFHeatCapFT,            !- Name",
        "  1.014599599,             !- Coefficient1 Constant",
        "  -0.002506703,            !- Coefficient2 x",
        "  -0.000141599,            !- Coefficient3 x**2",
        "  0.026931595,             !- Coefficient4 y",
        "  1.83538e-06,             !- Coefficient5 y**2",
        "  -0.000358147,            !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  27.0,                    !- Maximum Value of x",
        "  -20.0,                   !- Minimum Value of y",
        "  15.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Cubic,",
        "  VRFHeatCapFTBoundary,    !- Name",
        "  -7.6000882,              !- Coefficient1 Constant",
        "  3.05090016,              !- Coefficient2 x",
        "  -0.1162844,              !- Coefficient3 x**2",
        "  0.0,                     !- Coefficient4 x**3",
        "  15.0,                    !- Minimum Value of x",
        "  27.0,                    !- Maximum Value of x",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature;             !- Output Unit Type",

        "Curve:Biquadratic,",
        "  VRFHeatCapFTHi,          !- Name",
        "  1.161134821,             !- Coefficient1 Constant",
        "  0.027478868,             !- Coefficient2 x",
        "  -0.00168795,             !- Coefficient3 x**2",
        "  0.001783378,             !- Coefficient4 y",
        "  2.03208e-06,             !- Coefficient5 y**2",
        "  -6.8969e-05,             !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  27.0,                    !- Maximum Value of x",
        "  -10.0,                   !- Minimum Value of y",
        "  15.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Biquadratic,",
        "  VRFHeatEIRFT,            !- Name",
        "  0.87465501,              !- Coefficient1 Constant",
        "  -0.01319754,             !- Coefficient2 x",
        "  0.00110307,              !- Coefficient3 x**2",
        "  -0.0133118,              !- Coefficient4 y",
        "  0.00089017,              !- Coefficient5 y**2",
        "  -0.00012766,             !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  27.0,                    !- Maximum Value of x",
        "  -20.0,                   !- Minimum Value of y",
        "  12.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Cubic,",
        "  VRFHeatEIRFTBoundary,    !- Name",
        "  -7.6000882,              !- Coefficient1 Constant",
        "  3.05090016,              !- Coefficient2 x",
        "  -0.1162844,              !- Coefficient3 x**2",
        "  0.0,                     !- Coefficient4 x**3",
        "  15.0,                    !- Minimum Value of x",
        "  27.0,                    !- Maximum Value of x",
        "  -20.0,                   !- Minimum Curve Output",
        "  15.0,                    !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature;             !- Output Unit Type",

        "Curve:Biquadratic,",
        "  VRFHeatEIRFTHi,          !- Name",
        "  2.504005146,             !- Coefficient1 Constant",
        "  -0.05736767,             !- Coefficient2 x",
        "  4.07336e-05,             !- Coefficient3 x**2",
        "  -0.12959669,             !- Coefficient4 y",
        "  0.00135839,              !- Coefficient5 y**2",
        "  0.00317047,              !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  27.0,                    !- Maximum Value of x",
        "  -10.0,                   !- Minimum Value of y",
        "  15.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Cubic,",
        "  HeatingEIRLowPLR,        !- Name",
        "  0.1400093,               !- Coefficient1 Constant",
        "  0.6415002,               !- Coefficient2 x",
        "  0.1339047,               !- Coefficient3 x**2",
        "  0.0845859,               !- Coefficient4 x**3",
        "  0.0,                     !- Minimum Value of x",
        "  1.0,                     !- Maximum Value of x",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Dimensionless,           !- Input Unit Type for X",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Quadratic,",
        "  HeatingEIRHiPLR,         !- Name",
        "  2.4294355,               !- Coefficient1 Constant",
        "  -2.235887,               !- Coefficient2 x",
        "  0.8064516,               !- Coefficient3 x**2",
        "  1.0,                     !- Minimum Value of x",
        "  1.5,                     !- Maximum Value of x",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Dimensionless,           !- Input Unit Type for X",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Linear,",
        "  HeatingCombRatio,        !- Name",
        "  0.96034,                 !- Coefficient1 Constant",
        "  0.03966,                 !- Coefficient2 x",
        "  1.0,                     !- Minimum Value of x",
        "  1.5,                     !- Maximum Value of x",
        "  1.0,                     !- Minimum Curve Output",
        "  1.023,                   !- Maximum Curve Output",
        "  Dimensionless,           !- Input Unit Type for X",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Biquadratic,",
        "  CoolingLengthCorrectionFactor,  !- Name",
        "  1.0693794,               !- Coefficient1 Constant",
        "  -0.0014951,              !- Coefficient2 x",
        "  2.56e-06,                !- Coefficient3 x**2",
        "  -0.1151104,              !- Coefficient4 y",
        "  0.0511169,               !- Coefficient5 y**2",
        "  -0.0004369,              !- Coefficient6 x*y",
        "  8.0,                     !- Minimum Value of x",
        "  175.0,                   !- Maximum Value of x",
        "  0.5,                     !- Minimum Value of y",
        "  1.5,                     !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "ZoneTerminalUnitList,",
        "  VRF Heat Pump TU List,   !- Zone Terminal Unit List Name",
        "  TU1;                     !- Zone Terminal Unit Name 1",

        "OutdoorAir:NodeList,",
        "  OutsideAirInletNodes;    !- Node or NodeList Name 1",

        "NodeList,",
        "  OutsideAirInletNodes,    !- Name",
        "  MyVRFOANode;             !- Node 1 Name",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    state->dataGlobal->TimeStepsInHour = 1;
    state->dataGlobal->TimeStep = 1;
    state->dataGlobal->MinutesInTimeStep = 60;
    state->init_state(*state);

    // set input variables before input processing
    state->dataEnvrn->OutBaroPress = 101325.0;
    state->dataEnvrn->OutDryBulbTemp = 35.0;
    state->dataEnvrn->OutHumRat = 0.0098;
    state->dataEnvrn->OutEnthalpy = Psychrometrics::PsyHFnTdbW(state->dataEnvrn->OutDryBulbTemp, state->dataEnvrn->OutHumRat);
    state->dataEnvrn->StdRhoAir = 1.20;

    GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    GetZoneEquipmentData(*state);
    GetZoneAirLoopEquipment(*state);

    GetVRFInput(*state);
    state->dataHVACVarRefFlow->GetVRFInputFlag = false;

    // get input test for terminal air single duct mixer on inlet side of VRF terminal unit
    ASSERT_EQ(1, state->dataSingleDuct->NumATMixers);
    EXPECT_EQ("SPACE1-1 DOAS AIR TERMINAL", state->dataSingleDuct->SysATMixer(1).Name);                 // single duct air terminal mixer name
    EXPECT_EQ((int)HVAC::MixerType::SupplySide, (int)state->dataSingleDuct->SysATMixer(1).type);        // air terminal mixer connection type
    EXPECT_EQ("AIRTERMINAL:SINGLEDUCT:MIXER", state->dataDefineEquipment->AirDistUnit(1).EquipType(1)); // Air distribution unit equipment type
    EXPECT_EQ("TU1", state->dataHVACVarRefFlow->VRFTU(1).Name);                                         // zoneHVAC equipment name

    state->dataGlobal->BeginEnvrnFlag = false;

    HVACInletMassFlowRate = 0.50;
    PrimaryAirMassFlowRate = 0.1;
    SecondaryAirMassFlowRate = HVACInletMassFlowRate; // secondary air mass flow rate is the same as that of the VRF terminal unit

    // set zoneNode air condition
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp = 24.0;
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat = 0.0075;
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Enthalpy =
        Psychrometrics::PsyHFnTdbW(state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp,
                                   state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat);

    state->dataHVACVarRefFlow->CoolingLoad.allocate(1);
    state->dataHVACVarRefFlow->HeatingLoad.allocate(1);
    state->dataHVACVarRefFlow->HeatingLoad(1) = false;
    state->dataHVACVarRefFlow->CoolingLoad(1) = true;
    state->dataHVACVarRefFlow->CompOnMassFlow = HVACInletMassFlowRate;    // supply air mass flow rate
    state->dataHVACVarRefFlow->OACompOnMassFlow = PrimaryAirMassFlowRate; // OA mass flow rate
    state->dataHVACVarRefFlow->CompOnFlowRatio = 1.0;                     // compressor is on
    state->dataHVACGlobal->TurnFansOff = false;
    state->dataHVACGlobal->TurnFansOn = true;

    VRFNum = 1;
    VRFTUNum = 1;
    state->dataHVACVarRefFlow->VRFTU(VRFTUNum).fanOp = HVAC::FanOp::Continuous;

    // initialize mass flow rates
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).VRFTUInletNodeNum).MassFlowRate = HVACInletMassFlowRate;
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).ATMixerPriNode).MassFlowRate = PrimaryAirMassFlowRate;
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).ATMixerPriNode).MassFlowRateMaxAvail = PrimaryAirMassFlowRate;

    // set fan parameters
    state->dataFans->fans(1)->maxAirMassFlowRate = HVACInletMassFlowRate;
    state->dataFans->fans(1)->inletAirMassFlowRate = HVACInletMassFlowRate;
    state->dataFans->fans(1)->rhoAirStdInit = state->dataEnvrn->StdRhoAir;
    state->dataLoopNodes->Node(state->dataFans->fans(1)->inletNodeNum).MassFlowRateMaxAvail = HVACInletMassFlowRate;
    state->dataLoopNodes->Node(state->dataFans->fans(1)->outletNodeNum).MassFlowRateMax = HVACInletMassFlowRate;

    // set DX coil rated performance parameters
    state->dataDXCoils->DXCoil(1).RatedCBF(1) = 0.05;
    state->dataDXCoils->DXCoil(1).RatedAirMassFlowRate(1) = HVACInletMassFlowRate;

    // primary air condition set at outdoor air condition
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).ATMixerPriNode).Temp = state->dataEnvrn->OutDryBulbTemp;
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).ATMixerPriNode).HumRat = state->dataEnvrn->OutHumRat;
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).ATMixerPriNode).Enthalpy = state->dataEnvrn->OutEnthalpy;

    // set VRF terminal unit inlet condition to zone air node
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).VRFTUInletNodeNum).Temp =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp;
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).VRFTUInletNodeNum).HumRat =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat;
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).VRFTUInletNodeNum).Enthalpy =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Enthalpy;

    state->dataHVACVarRefFlow->VRFTU(1).ZoneNum = 1;
    state->dataSize->SysSizingRunDone = true;
    state->dataSize->ZoneSizingRunDone = true;
    state->dataGlobal->SysSizingCalc = true;

    state->dataZoneEnergyDemand->ZoneSysEnergyDemand.allocate(1);
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputReqToHeatSP = 0.0;
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputReqToCoolSP = -4000.0;
    QZnReq = state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputReqToCoolSP;

    state->dataHVACVarRefFlow->VRFTU(VRFTUNum).availSched->currentVal = 1.0;    // unit is always available
    state->dataHVACVarRefFlow->VRFTU(VRFTUNum).fanAvailSched->currentVal = 1.0; // fan is always available

    // set secondary air mass flow rate to zero
    state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).MassFlowRate = 0.0;
    // simulate zoneHVAC equipment (VRF terminal unit)
    SimVRF(*state, VRFTUNum, FirstHVACIteration, OnOffAirFlowRatio, QUnitOutVRFTU, LatOutputProvided, QZnReq);

    // check the terminal air mixer secondary air mass flow rate
    ASSERT_EQ(SecondaryAirMassFlowRate, state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).MassFlowRate);
    // check the terminal air mixer outlet air mass flow rate
    ATMixerOutletMassFlowRate = SecondaryAirMassFlowRate + PrimaryAirMassFlowRate;
    ASSERT_EQ(ATMixerOutletMassFlowRate, state->dataSingleDuct->SysATMixer(1).MixedAirMassFlowRate);
    // check the cooling output delivered is within 4.0 Watt of zone cooling load
    ASSERT_NEAR(QZnReq, QUnitOutVRFTU, 4.0);
}

TEST_F(EnergyPlusFixture, AirTerminalSingleDuctMixer_SimVRFfluidCntrl_ATMInletSide)
{

    bool ErrorsFound(false);
    bool FirstHVACIteration(false);
    Real64 HVACInletMassFlowRate(0.0);
    Real64 PrimaryAirMassFlowRate(0.0);
    Real64 SecondaryAirMassFlowRate(0.0);
    Real64 OnOffAirFlowRatio(1.0);
    Real64 LatOutputProvided(0.0);
    Real64 QUnitOutVRFTU(0.0);
    Real64 QZnReq(0.0);
    int VRFNum(1);
    int VRFTUNum(1);

    std::string const idf_objects = delimited_string({
        "AirTerminal:SingleDuct:Mixer,",
        "  SPACE1-1 DOAS Air Terminal,  !- Name",
        "  ZoneHVAC:TerminalUnit:VariableRefrigerantFlow,  !- ZoneHVAC Unit Object Type",
        "  TU1,                     !- ZoneHVAC Unit Object Name",
        "  TU1 Inlet Node,          !- Mixer Outlet Node Name",
        "  SPACE1-1 Air Terminal Mixer Primary Inlet,  !- Mixer Primary Air Inlet Node Name",
        "  SPACE1-1 Air Terminal Mixer Secondary Inlet,  !- Mixer Secondary Air Inlet Node Name",
        "  InletSide;               !- Mixer Connection Type",

        "ZoneHVAC:AirDistributionUnit,",
        "  SPACE1-1 DOAS ATU,       !- Name",
        "  TU1 Inlet Node,          !- Air Distribution Unit Outlet Node Name",
        "  AirTerminal:SingleDuct:Mixer,  !- Air Terminal Object Type",
        "  SPACE1-1 DOAS Air Terminal;  !- Air Terminal Name",

        "Schedule:Compact,",
        "  FanAvailSched,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,1.0;        !- Field 3",

        "Schedule:Compact,",
        "  VRFAvailSched,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,1.0;        !- Field 3",

        "Schedule:Compact,",
        "  CyclingFanSch,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,0.0;        !- Field 3",

        "ZoneHVAC:EquipmentList,",
        "  SPACE1-1 Eq,             !- Name",
        "  SequentialLoad,          !- Load Distribution Scheme",
        "  ZoneHVAC:AirDistributionUnit,  !- Zone Equipment 1 Object Type",
        "  SPACE1-1 DOAS ATU,       !- Zone Equipment 1 Name",
        "  1,                       !- Zone Equipment 1 Cooling Sequence",
        "  1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 1 Sequential Cooling Fraction Schedule Name",
        "  ,                        !- Zone Equipment 1 Sequential Heating Fraction Schedule Name",
        "  ZoneHVAC:TerminalUnit:VariableRefrigerantFlow,  !- Zone Equipment 2 Object Type",
        "  TU1,                     !- Zone Equipment 2 Name",
        "  2,                       !- Zone Equipment 2 Cooling Sequence",
        "  2,                       !- Zone Equipment 2 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 2 Sequential Cooling Fraction Schedule Name",
        "  ;                        !- Zone Equipment 2 Sequential Heating Fraction Schedule Name",

        "ZoneHVAC:TerminalUnit:VariableRefrigerantFlow,",
        "  TU1,                     !- Zone Terminal Unit Name",
        "  VRFAvailSched,           !- Terminal Unit Availability Schedule",
        "  TU1 Inlet Node,          !- Terminal Unit Air Inlet Node Name",
        "  TU1 Outlet Node,         !- Terminal Unit Air Outlet Node Name",
        "  0.5,                     !- Cooling Supply Air Flow Rate {m3/s}",
        "  0.5,                     !- No Cooling Supply Air Flow Rate {m3/s}",
        "  0.5,                     !- Heating Supply Air Flow Rate {m3/s}",
        "  0.5,                     !- No Heating Supply Air Flow Rate {m3/s}",
        "  0.0,                     !- Cooling Outdoor Air Flow Rate {m3/s}",
        "  0.0,                     !- Heating Outdoor Air Flow Rate {m3/s}",
        "  0.0,                     !- No Load Outdoor Air Flow Rate {m3/s}",
        "  VRFFanSchedule,          !- Supply Air Fan Operating Mode Schedule Name",
        "  DrawThrough,             !- Supply Air Fan Placement",
        "  Fan:SystemModel,         !- Supply Air Fan Object Type",
        "  TU1 VRF Supply Fan,      !- Supply Air Fan Object Name",
        "  ,                        !- Outside Air Mixer Object Type",
        "  ,                        !- Outside Air Mixer Object Name",
        "  Coil:Cooling:DX:VariableRefrigerantFlow:FluidTemperatureControl,  !- Cooling Coil Object Type",
        "  TU1 VRF DX Cooling Coil,  !- Cooling Coil Object Name",
        "  Coil:Heating:DX:VariableRefrigerantFlow:FluidTemperatureControl,  !- Heating Coil Object Type",
        "  TU1 VRF DX Heating Coil,  !- Heating Coil Object Name",
        "  30.0,                    !- Zone Terminal Unit On Parasitic Electric Energy Use {W}",
        "  20.0,                    !- Zone Terminal Unit Off Parasitic Electric Energy Use {W}",
        "  ,                        !- Rated Heating Capacity Sizing Ratio {W/W}",
        "  ,                        !- Availability Manager List Name",
        "  ,                        !- Design Specification ZoneHVAC Sizing Object Name",
        "  ,                        !- Supplemental Heating Coil Object Type",
        "  ,                        !- Supplemental Heating Coil Name",
        "  ,                        !- Maximum Supply Air Temperature from Supplemental Heater {C}",
        "  ;                        !- Maximum Outdoor Dry-Bulb Temperature for Supplemental Heater Operation {C}",

        "Fan:SystemModel,",
        "  TU1 VRF Supply Fan,      !- Name",
        "  VRFAvailSched,           !- Availability Schedule Name",
        "  TU1 VRF DX HCoil Outlet Node,  !- Air Inlet Node Name",
        "  TU1 Outlet Node,         !- Air Outlet Node Name",
        "  0.5,                     !- Design Maximum Air Flow Rate {m3/s}",
        "  Continuous,              !- Speed Control Method",
        "  0.0,                     !- Electric Power Minimum Flow Rate Fraction",
        "  600.0,                   !- Design Pressure Rise {Pa}",
        "  0.9,                     !- Motor Efficiency",
        "  1.0,                     !- Motor In Air Stream Fraction",
        "  Autosize,                !- Design Electric Power Consumption {W}",
        "  TotalEfficiencyAndPressure,  !- Design Power Sizing Method",
        "  ,                        !- Electric Power Per Unit Flow Rate {W/(m3/s)}",
        "  ,                        !- Electric Power Per Unit Flow Rate Per Unit Pressure {W/((m3/s)-Pa)}",
        "  0.7,                     !- Fan Total Efficiency",
        "  TU1 Fan Power Curve,     !- Electric Power Function of Flow Fraction Curve Name",
        "  ,                        !- Night Ventilation Mode Pressure Rise {Pa}",
        "  ,                        !- Night Ventilation Mode Flow Fraction",
        "  ,                        !- Motor Loss Zone Name",
        "  ,                        !- Motor Loss Radiative Fraction",
        "  General;                 !- End-Use Subcategory",

        "Curve:Quartic,",
        "  TU1 Fan Power Curve,     !- Name",
        "  0.059,                   !- Coefficient1 Constant",
        "  0.0,                     !- Coefficient2 x",
        "  0.0,                     !- Coefficient3 x**2",
        "  0.928,                   !- Coefficient4 x**3",
        "  0.0,                     !- Coefficient5 x**4",
        "  0.0,                     !- Minimum Value of x",
        "  1.0,                     !- Maximum Value of x",
        "  0.0,                     !- Minimum Curve Output",
        "  1.0,                     !- Maximum Curve Output",
        "  Dimensionless,           !- Input Unit Type for X",
        "  Dimensionless;           !- Output Unit Type",

        "Coil:Heating:DX:VariableRefrigerantFlow:FluidTemperatureControl,",
        "  TU1 VRF DX Heating Coil,  !- Name",
        "  VRFAvailSched,           !- Availability Schedule",
        "  TU1 VRF DX CCoil Outlet Node,  !- Coil Air Inlet Node",
        "  TU1 VRF DX HCoil Outlet Node,  !- Coil Air Outlet Node",
        "  6500.0,                  !- Rated Total Heating Capacity {W}",
        "  5.0,                     !- Indoor Unit Reference Subcooling {deltaC}",
        "  IUCondTempCurve;         !- Indoor Unit Condensing Temperature Function of Subcooling Curve Name",

        "Curve:Quadratic,",
        "  IUCondTempCurve,         !- Name",
        "  -1.85,                   !- Coefficient1 Constant",
        "  0.411,                   !- Coefficient2 x",
        "  0.0196,                  !- Coefficient3 x**2",
        "  0.0,                     !- Minimum Value of x",
        "  20.0,                    !- Maximum Value of x",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature;             !- Output Unit Type",

        "Coil:Cooling:DX:VariableRefrigerantFlow:FluidTemperatureControl,",
        "  TU1 VRF DX Cooling Coil,  !- Name",
        "  VRFAvailSched,           !- Availability Schedule Name",
        "  TU1 INLET NODE,          !- Coil Air Inlet Node",
        "  TU1 VRF DX CCoil Outlet Node,  !- Coil Air Outlet Node",
        "  6600.0,                  !- Rated Total Cooling Capacity {W}",
        "  0.75,                    !- Rated Sensible Heat Ratio",
        "  3.0,                     !- Indoor Unit Reference Superheating {deltaC}",
        "  IUEvapTempCurve,         !- Indoor Unit Evaporating Temperature Function of Superheating Curve Name",
        "  ;                        !- Name of Water Storage Tank for Condensate Collection",

        "Curve:Quadratic,",
        "  IUEvapTempCurve,         !- Name",
        "  0.0,                     !- Coefficient1 Constant",
        "  0.843,                   !- Coefficient2 x",
        "  0.0,                     !- Coefficient3 x**2",
        "  0.0,                     !- Minimum Value of x",
        "  15.0,                    !- Maximum Value of x",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature;             !- Output Unit Type",

        "Zone,",
        "  SPACE1-1,                !- Name",
        "  0.0,                     !- Direction of Relative North {deg}",
        "  0.0,                     !- X Origin {m}",
        "  0.0,                     !- Y Origin {m}",
        "  0.0,                     !- Z Origin {m}",
        "  1,                       !- Type",
        "  1,                       !- Multiplier",
        "  2.438400269,             !- Ceiling Height {m}",
        "  239.247360229;           !- Volume {m3}",

        "ZoneHVAC:EquipmentConnections,",
        "  SPACE1-1,                !- Zone Name",
        "  SPACE1-1 Eq,             !- Zone Conditioning Equipment List Name",
        "  SPACE1-1 In Nodes,       !- Zone Air Inlet Node or NodeList Name",
        "  SPACE1-1 Air Terminal Mixer Secondary Inlet,  !- Zone Air Exhaust Node or NodeList Name",
        "  SPACE1-1 Node,           !- Zone Air Node Name",
        "  SPACE1-1 RETURN OUTLET;  !- Zone Return Air Node or NodeList Name",

        "NodeList,",
        "  SPACE1-1 In Nodes,       !- Name",
        "  TU1 Outlet Node;         !- Node 1 Name",

        "AirConditioner:VariableRefrigerantFlow:FluidTemperatureControl,",
        "  VRF Heat Pump,           !- Heat Pump Name",
        "  VRFAvailSched,           !- Availability Schedule Name",
        "  VRF Heat Pump TU List,   !- Zone Terminal Unit List Name",
        "  R410A,                   !- Refrigerant Type",
        "  41300.0,                 !- Rated Evaporative Capacity {W}",
        "  0.344,                   !- Rated Compressor Power Per Unit of Rated Evaporative Capacity {dimensionless}",
        "  -5.0,                    !- Minimum Outdoor Air Temperature in Cooling Mode {C}",
        "  43.0,                    !- Maximum Outdoor Air Temperature in Cooling Mode {C}",
        "  -20.0,                   !- Minimum Outdoor Air Temperature in Heating Mode {C}",
        "  22.0,                    !- Maximum Outdoor Air Temperature in Heating Mode {C}",
        "  3.0,                     !- Reference Outdoor Unit Superheating {deltaC}",
        "  3.0,                     !- Reference Outdoor Unit Subcooling {deltaC}",
        "  ConstantTemp,            !- Refrigerant Temperature Control Algorithm for Indoor Unit",
        "  6.0,                     !- Reference Evaporating Temperature for Indoor Unit {C}",
        "  44.0,                    !- Reference Condensing Temperature for Indoor Unit {C}",
        "  6.0,                     !- Variable Evaporating Temperature Minimum for Indoor Unit {C}",
        "  13.0,                    !- Variable Evaporating Temperature Maximum for Indoor Unit {C}",
        "  42.0,                    !- Variable Condensing Temperature Minimum for Indoor Unit {C}",
        "  46.0,                    !- Variable Condensing Temperature Maximum for Indoor Unit {C}",
        "  0.00412,                 !- Outdoor Unit Fan Power Per Unit of Rated Evaporative Capacity {dimensionless}",
        "  7.26e-05,                !- Outdoor Unit Fan Flow Rate Per Unit of Rated Evaporative Capacity {m3/s-W}",
        "  OUEvapTempCurve,         !- Outdoor Unit Evaporating Temperature Function of Superheating Curve Name",
        "  OUCondTempCurve,         !- Outdoor Unit Condensing Temperature Function of Subcooling Curve Name",
        "  0.0508,                  !- Diameter of Main Pipe Connecting Outdoor Unit to the First Branch Joint {m}",
        "  30.0,                    !- Length of Main Pipe Connecting Outdoor Unit to the First Branch Joint {m}",
        "  36.0,                    !- Equivalent Length of Main Pipe Connecting Outdoor Unit to the First Branch Joint {m}",
        "  5.0,                     !- Height Difference Between Outdoor Unit and Indoor Units {m}",
        "  0.02,                    !- Main Pipe Insulation Thickness {m}",
        "  0.032,                   !- Main Pipe Insulation Thermal Conductivity {W/m-K}",
        "  33.0,                    !- Crankcase Heater Power per Compressor {W}",
        "  1,                       !- Number of Compressors {dimensionless}",
        "  0.33,                    !- Ratio of Compressor Size to Total Compressor Capacity {W/W}",
        "  7.0,                     !- Maximum Outdoor Dry-Bulb Temperature for Crankcase Heater {C}",
        "  ,                        !- Defrost Strategy",
        "  ,                        !- Defrost Control",
        "  ,                        !- Defrost Energy Input Ratio Modifier Function of Temperature Curve Name",
        "  ,                        !- Defrost Time Period Fraction",
        "  ,                        !- Resistive Defrost Heater Capacity {W}",
        "  ,                        !- Maximum Outdoor Dry-bulb Temperature for Defrost Operation {C}",
        "  4500000.0,               !- Compressor maximum delta Pressure {Pa}",
        "  3,                       !- Number of Compressor Loading Index Entries",
        "  1500.0,                  !- Compressor Speed at Loading Index 1 {rev/min}",
        "  MinSpdCooling,           !- Loading Index 1 Evaporative Capacity Multiplier Function of Temperature Curve Name",
        "  MinSpdPower,             !- Loading Index 1 Compressor Power Multiplier Function of Temperature Curve Name",
        "  3600.0,                  !- Compressor Speed at Loading Index 2 {rev/min}",
        "  Spd1Cooling,             !- Loading Index 2 Evaporative Capacity Multiplier Function of Temperature Curve Name",
        "  Spd1Power,               !- Loading Index 2 Compressor Power Multiplier Function of Temperature Curve Name",
        "  6000.0,                  !- Compressor Speed at Loading Index 3 {rev/min}",
        "  Spd2Cooling,             !- Loading Index 3 Evaporative Capacity Multiplier Function of Temperature Curve Name",
        "  Spd2Power;               !- Loading Index 3 Compressor Power Multiplier Function of Temperature Curve Name",

        "Curve:Quadratic,",
        "  OUEvapTempCurve,         !- Name",
        "  0.0,                     !- Coefficient1 Constant",
        "  0.605,                   !- Coefficient2 x",
        "  0.025,                   !- Coefficient3 x**2",
        "  0.0,                     !- Minimum Value of x",
        "  15.0,                    !- Maximum Value of x",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature;             !- Output Unit Type",

        "Curve:Quadratic,",
        "  OUCondTempCurve,         !- Name",
        "  0.0,                     !- Coefficient1 Constant",
        "  -2.91,                   !- Coefficient2 x",
        "  1.18,                    !- Coefficient3 x**2",
        "  0.0,                     !- Minimum Value of x",
        "  20.0,                    !- Maximum Value of x",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature;             !- Output Unit Type",

        "Curve:Biquadratic,",
        "  MinSpdCooling,           !- Name",
        "  0.319,                   !- Coefficient1 Constant",
        "  -0.00126,                !- Coefficient2 x",
        "  -2.15e-05,               !- Coefficient3 x**2",
        "  0.012,                   !- Coefficient4 y",
        "  0.000105,                !- Coefficient5 y**2",
        "  -8.66e-05,               !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  65.0,                    !- Maximum Value of x",
        "  -30.0,                   !- Minimum Value of y",
        "  15.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Biquadratic,",
        "  MinSpdPower,             !- Name",
        "  0.0879,                  !- Coefficient1 Constant",
        "  -0.000172,               !- Coefficient2 x",
        "  6.93e-05,                !- Coefficient3 x**2",
        "  -3.38e-05,               !- Coefficient4 y",
        "  -8.1e-06,                !- Coefficient5 y**2",
        "  -1.04e-05,               !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  65.0,                    !- Maximum Value of x",
        "  -30.0,                   !- Minimum Value of y",
        "  15.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Biquadratic,",
        "  Spd1Cooling,             !- Name",
        "  0.812,                   !- Coefficient1 Constant",
        "  -0.00423,                !- Coefficient2 x",
        "  -4.11e-05,               !- Coefficient3 x**2",
        "  0.0297,                  !- Coefficient4 y",
        "  0.000267,                !- Coefficient5 y**2",
        "  -0.000223,               !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  65.0,                    !- Maximum Value of x",
        "  -30.0,                   !- Minimum Value of y",
        "  15.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Biquadratic,",
        "  Spd1Power,               !- Name",
        "  0.326,                   !- Coefficient1 Constant",
        "  -0.0022,                 !- Coefficient2 x",
        "  0.000142,                !- Coefficient3 x**2",
        "  0.00282,                 !- Coefficient4 y",
        "  2.86e-05,                !- Coefficient5 y**2",
        "  -3.5e-05,                !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  65.0,                    !- Maximum Value of x",
        "  -30.0,                   !- Minimum Value of y",
        "  15.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Biquadratic,",
        "  Spd2Cooling,             !- Name",
        "  1.32,                    !- Coefficient1 Constant",
        "  -0.0062,                 !- Coefficient2 x",
        "  -7.1e-05,                !- Coefficient3 x**2",
        "  0.0489,                  !- Coefficient4 y",
        "  0.000459,                !- Coefficient5 y**2",
        "  -0.000367,               !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  65.0,                    !- Maximum Value of x",
        "  -30.0,                   !- Minimum Value of y",
        "  15.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Biquadratic,",
        "  Spd2Power,               !- Name",
        "  0.656,                   !- Coefficient1 Constant",
        "  -0.00371,                !- Coefficient2 x",
        "  0.000207,                !- Coefficient3 x**2",
        "  0.0105,                  !- Coefficient4 y",
        "  7.36e-05,                !- Coefficient5 y**2",
        "  -0.000157,               !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  65.0,                    !- Maximum Value of x",
        "  -30.0,                   !- Minimum Value of y",
        "  15.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "ZoneTerminalUnitList,",
        "  VRF Heat Pump TU List,   !- Zone Terminal Unit List Name",
        "  TU1;                     !- Zone Terminal Unit Name 1",

        "OutdoorAir:NodeList,",
        "  OutsideAirInletNodes;    !- Node or NodeList Name 1",

        "NodeList,",
        "  OutsideAirInletNodes,    !- Name",
        "  MyVRFOANode;             !- Node 1 Name",

        "FluidProperties:Name,",
        "  R410a,                   !- Fluid Name",
        "  Refrigerant;             !- Fluid Type",

        "FluidProperties:Temperatures,",
        "  R410aSaturatedTemperatures,  !- Name",
        "  -72.0,-69.0,-66.0,-63.0,-60.0,-57.0,-54.0,",
        "  -51.0,-48.0,-45.0,-42.0,-39.0,-36.0,-33.0,",
        "  -30.0,-27.0,-24.0,-21.0,-18.0,-15.0,-12.0,",
        "  -9.0,-6.0,-3.0,0.0,3.0,6.0,9.0,",
        "  12.0,15.0,18.0,21.0,24.0,27.0,30.0,",
        "  33.0,36.0,39.0,42.0,45.0,48.0,51.0,",
        "  54.0,57.0,60.0,63.0,66.0,69.0;",

        "FluidProperties:Temperatures,",
        "  R410aSuperHeatTemperatures,  !- Name",
        "  -72.0,-66.0,-60.0,-54.0,-48.0,-45.0,-42.0,",
        "  -39.0,-36.0,-33.0,-30.0,-27.0,-24.0,-21.0,",
        "  -18.0,-15.0,-12.0,-9.0,-6.0,-3.0,0.0,",
        "  3.0,6.0,9.0,12.0,15.0,18.0,21.0,",
        "  24.0,27.0,30.0,33.0,36.0,39.0,42.0,",
        "  45.0,48.0,51.0,54.0,57.0,60.0,63.0,",
        "  66.0,69.0;",

        "FluidProperties:Saturated,",
        "  R410a,                   !- Fluid Name",
        "  Pressure,                !- Fluid Property Type",
        "  FluidGas,                !- Fluid Phase",
        "  R410aSaturatedTemperatures,  !- Temperature Values Name",
        "  31238.0,37717.0,45248.0,53954.0,63963.0,75412.0,88445.0,",
        "  103210.0,119880.0,138600.0,159550.0,182920.0,208880.0,237620.0,",
        "  269350.0,304260.0,342570.0,384490.0,430240.0,480040.0,534120.0,",
        "  592730.0,656090.0,724460.0,798080.0,877220.0,962140.0,1053100.0,",
        "  1150400.0,1254300.0,1365100.0,1483100.0,1608600.0,1741900.0,1883400.0,",
        "  2033400.0,2192300.0,2360400.0,2538200.0,2726100.0,2924600.0,3134100.0,",
        "  3355200.0,3588600.0,3834800.0,4094900.0,4369700.0,4660700.0;",

        "FluidProperties:Saturated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  Fluid,                   !- Fluid Phase",
        "  R410aSaturatedTemperatures,  !- Temperature Values Name",
        "  98535.0,102590.0,106650.0,110720.0,114790.0,118880.0,122970.0,",
        "  127070.0,131190.0,135320.0,139470.0,143630.0,147820.0,152020.0,",
        "  156240.0,160480.0,164750.0,169040.0,173370.0,177720.0,182100.0,",
        "  186520.0,190970.0,195470.0,200000.0,204580.0,209200.0,213880.0,",
        "  218610.0,223400.0,228250.0,233160.0,238150.0,243220.0,248380.0,",
        "  253630.0,258990.0,264470.0,270080.0,275850.0,281800.0,287970.0,",
        "  294410.0,301200.0,308480.0,316500.0,325780.0,338150.0;",

        "FluidProperties:Saturated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  FluidGas,                !- Fluid Phase",
        "  R410aSaturatedTemperatures,  !- Temperature Values Name",
        "  388130.0,389810.0,391480.0,393130.0,394760.0,396370.0,397960.0,",
        "  399530.0,401080.0,402600.0,404100.0,405570.0,407010.0,408420.0,",
        "  409800.0,411140.0,412450.0,413730.0,414960.0,416150.0,417300.0,",
        "  418400.0,419450.0,420450.0,421390.0,422270.0,423080.0,423820.0,",
        "  424480.0,425070.0,425560.0,425950.0,426240.0,426410.0,426460.0,",
        "  426350.0,426090.0,425640.0,424980.0,424080.0,422900.0,421370.0,",
        "  419410.0,416920.0,413700.0,409420.0,403430.0,393730.0;",

        "FluidProperties:Saturated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  Fluid,                   !- Fluid Phase",
        "  R410aSaturatedTemperatures,  !- Temperature Values Name",
        "  1412.7,1403.6,1394.6,1385.4,1376.2,1366.9,1357.6,",
        "  1348.2,1338.7,1329.1,1319.4,1309.7,1299.8,1289.8,",
        "  1279.7,1269.4,1259.1,1248.6,1237.9,1227.1,1216.0,",
        "  1204.8,1193.4,1181.8,1169.9,1157.8,1145.4,1132.8,",
        "  1119.7,1106.4,1092.7,1078.5,1063.9,1048.8,1033.1,",
        "  1016.7,999.71,981.87,963.08,943.19,921.98,899.16,",
        "  874.29,846.72,815.37,778.25,730.95,659.03;",

        "FluidProperties:Saturated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  FluidGas,                !- Fluid Phase",
        "  R410aSaturatedTemperatures,  !- Temperature Values Name",
        "  1.3845,1.6517,1.9588,2.31,2.7097,3.1627,3.6737,",
        "  4.2482,4.8916,5.6098,6.4088,7.2952,8.2758,9.3578,",
        "  10.549,11.857,13.292,14.861,16.576,18.447,20.485,",
        "  22.702,25.113,27.732,30.575,33.659,37.005,40.634,",
        "  44.571,48.844,53.483,58.525,64.012,69.991,76.52,",
        "  83.666,91.511,100.16,109.73,120.38,132.33,145.85,",
        "  161.35,179.4,200.95,227.66,263.01,317.59;",

        "FluidProperties:Saturated,",
        "  R410a,                   !- Fluid Name",
        "  SpecificHeat,            !- Fluid Property Type",
        "  Fluid,                   !- Fluid Phase",
        "  R410aSaturatedTemperatures,  !- Temperature Values Name",
        "  1349.9,1351.5,1353.4,1355.7,1358.4,1361.4,1364.8,",
        "  1368.6,1372.8,1377.4,1382.5,1388.1,1394.1,1400.7,",
        "  1407.8,1415.5,1423.8,1432.7,1442.4,1452.7,1463.9,",
        "  1475.9,1488.8,1502.7,1517.7,1534.0,1551.5,1570.6,",
        "  1591.4,1614.1,1639.0,1666.4,1696.8,1730.7,1768.9,",
        "  1812.3,1862.2,1920.4,1989.5,2073.2,2177.4,2311.6,",
        "  2492.4,2750.7,3153.4,3872.3,5519.0,12701.0;",

        "FluidProperties:Saturated,",
        "  R410a,                   !- Fluid Name",
        "  SpecificHeat,            !- Fluid Property Type",
        "  FluidGas,                !- Fluid Phase",
        "  R410aSaturatedTemperatures,  !- Temperature Values Name",
        "  723.87,735.19,746.93,759.1,771.67,784.65,798.02,",
        "  811.78,825.94,840.5,855.46,870.85,886.68,902.98,",
        "  919.79,937.15,955.11,973.72,993.07,1013.2,1034.3,",
        "  1056.4,1079.6,1104.2,1130.2,1158.0,1187.7,1219.6,",
        "  1254.1,1291.7,1332.9,1378.3,1428.7,1485.3,1549.4,",
        "  1622.8,1707.8,1807.8,1927.4,2073.5,2256.2,2492.2,",
        "  2809.4,3259.6,3950.4,5146.5,7718.5,17076.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  31238.0,                 !- Pressure {Pa}",
        "  388130.0,392450.0,396750.0,401050.0,405360.0,407530.0,409700.0,",
        "  411890.0,414080.0,416280.0,418490.0,420710.0,422940.0,425180.0,",
        "  427430.0,429690.0,431960.0,434250.0,436550.0,438850.0,441180.0,",
        "  443510.0,445860.0,448210.0,450580.0,452970.0,455360.0,457770.0,",
        "  460200.0,462630.0,465080.0,467540.0,470020.0,472510.0,475010.0,",
        "  477520.0,480050.0,482590.0,485150.0,487720.0,490300.0,492900.0,",
        "  495510.0,498130.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  31238.0,                 !- Pressure {Pa}",
        "  1.3845,1.3404,1.2997,1.2617,1.2262,1.2092,1.1928,",
        "  1.1768,1.1613,1.1462,1.1316,1.1173,1.1034,1.0898,",
        "  1.0766,1.0638,1.0512,1.039,1.0271,1.0154,1.004,",
        "  0.99285,0.98197,0.97133,0.96093,0.95075,0.94079,0.93104,",
        "  0.9215,0.91215,0.90299,0.89403,0.88524,0.87662,0.86817,",
        "  0.85989,0.85177,0.8438,0.83598,0.82831,0.82077,0.81338,",
        "  0.80612,0.79899;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  45248.0,                 !- Pressure {Pa}",
        "  0.0,391480.0,395930.0,400340.0,404740.0,406940.0,409150.0,",
        "  411360.0,413580.0,415800.0,418030.0,420270.0,422520.0,424780.0,",
        "  427050.0,429330.0,431610.0,433910.0,436220.0,438540.0,440880.0,",
        "  443220.0,445580.0,447940.0,450320.0,452720.0,455120.0,457540.0,",
        "  459970.0,462410.0,464860.0,467330.0,469810.0,472310.0,474810.0,",
        "  477330.0,479870.0,482410.0,484970.0,487550.0,490130.0,492730.0,",
        "  495350.0,497970.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  45248.0,                 !- Pressure {Pa}",
        "  0.0,1.9588,1.8968,1.8395,1.7863,1.761,1.7365,",
        "  1.7128,1.6898,1.6674,1.6457,1.6246,1.6041,1.5842,",
        "  1.5647,1.5458,1.5273,1.5093,1.4918,1.4747,1.458,",
        "  1.4416,1.4257,1.4101,1.3949,1.38,1.3654,1.3512,",
        "  1.3372,1.3236,1.3102,1.2971,1.2843,1.2717,1.2594,",
        "  1.2473,1.2355,1.2239,1.2125,1.2013,1.1903,1.1796,",
        "  1.169,1.1586;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  63963.0,                 !- Pressure {Pa}",
        "  0.0,0.0,394760.0,399350.0,403880.0,406140.0,408390.0,",
        "  410640.0,412900.0,415160.0,417420.0,419690.0,421960.0,424250.0,",
        "  426540.0,428840.0,431140.0,433460.0,435790.0,438130.0,440470.0,",
        "  442830.0,445200.0,447580.0,449970.0,452380.0,454790.0,457220.0,",
        "  459660.0,462110.0,464570.0,467050.0,469540.0,472040.0,474550.0,",
        "  477080.0,479620.0,482170.0,484740.0,487320.0,489910.0,492520.0,",
        "  495130.0,497770.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  63963.0,                 !- Pressure {Pa}",
        "  0.0,0.0,2.7097,2.624,2.5451,2.5078,2.4718,",
        "  2.437,2.4034,2.3708,2.3393,2.3086,2.2789,2.25,",
        "  2.2219,2.1945,2.1679,2.142,2.1167,2.0921,2.0681,",
        "  2.0446,2.0217,1.9994,1.9776,1.9562,1.9354,1.915,",
        "  1.895,1.8755,1.8564,1.8377,1.8194,1.8014,1.7839,",
        "  1.7666,1.7497,1.7332,1.7169,1.701,1.6854,1.67,",
        "  1.655,1.6402;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  88445.0,                 !- Pressure {Pa}",
        "  0.0,0.0,0.0,397960.0,402700.0,405030.0,407360.0,",
        "  409670.0,411980.0,414290.0,416600.0,418910.0,421220.0,423540.0,",
        "  425860.0,428190.0,430520.0,432860.0,435210.0,437570.0,439940.0,",
        "  442320.0,444700.0,447100.0,449510.0,451930.0,454360.0,456800.0,",
        "  459250.0,461710.0,464190.0,466680.0,469180.0,471690.0,474210.0,",
        "  476750.0,479300.0,481860.0,484430.0,487020.0,489620.0,492230.0,",
        "  494860.0,497490.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  88445.0,                 !- Pressure {Pa}",
        "  0.0,0.0,0.0,3.6737,3.557,3.5024,3.45,",
        "  3.3995,3.3509,3.3039,3.2585,3.2146,3.172,3.1308,",
        "  3.0907,3.0518,3.014,2.9772,2.9414,2.9065,2.8726,",
        "  2.8395,2.8072,2.7757,2.7449,2.7149,2.6856,2.6569,",
        "  2.6289,2.6015,2.5747,2.5485,2.5228,2.4977,2.4731,",
        "  2.449,2.4254,2.4022,2.3795,2.3573,2.3354,2.314,",
        "  2.293,2.2724;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  119880.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,401080.0,403540.0,405970.0,",
        "  408380.0,410770.0,413150.0,415520.0,417880.0,420250.0,422610.0,",
        "  424970.0,427340.0,429710.0,432090.0,434470.0,436850.0,439250.0,",
        "  441650.0,444060.0,446480.0,448910.0,451350.0,453800.0,456260.0,",
        "  458730.0,461210.0,463700.0,466200.0,468710.0,471240.0,473770.0,",
        "  476320.0,478880.0,481450.0,484040.0,486630.0,489240.0,491860.0,",
        "  494500.0,497150.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  119880.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,4.8918,4.8116,4.7352,",
        "  4.6621,4.592,4.5247,4.4599,4.3974,4.337,4.2787,",
        "  4.2221,4.1674,4.1143,4.0627,4.0126,3.9639,3.9165,",
        "  3.8704,3.8255,3.7817,3.739,3.6974,3.6567,3.6171,",
        "  3.5783,3.5405,3.5035,3.4673,3.4319,3.3973,3.3634,",
        "  3.3302,3.2977,3.2659,3.2347,3.2041,3.1742,3.1448,",
        "  3.116,3.0877;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  138600.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,402600.0,405100.0,",
        "  407570.0,410020.0,412440.0,414850.0,417260.0,419650.0,422040.0,",
        "  424440.0,426830.0,429220.0,431620.0,434020.0,436420.0,438830.0,",
        "  441250.0,443680.0,446110.0,448550.0,451000.0,453460.0,455930.0,",
        "  458410.0,460900.0,463400.0,465910.0,468430.0,470970.0,473510.0,",
        "  476060.0,478630.0,481210.0,483800.0,486400.0,489020.0,491650.0,",
        "  494280.0,496940.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  138600.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,5.6098,5.5173,",
        "  5.4293,5.3451,5.2645,5.1871,5.1127,5.0409,4.9717,",
        "  4.9047,4.8399,4.7772,4.7163,4.6573,4.5999,4.5442,",
        "  4.49,4.4372,4.3859,4.3358,4.287,4.2394,4.193,",
        "  4.1476,4.1033,4.0601,4.0178,3.9765,3.936,3.8965,",
        "  3.8578,3.8199,3.7828,3.7464,3.7108,3.6759,3.6417,",
        "  3.6081,3.5752;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  159550.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,404100.0,",
        "  406640.0,409150.0,411630.0,414090.0,416540.0,418980.0,421400.0,",
        "  423830.0,426250.0,428670.0,431090.0,433510.0,435930.0,438360.0,",
        "  440800.0,443240.0,445690.0,448150.0,450610.0,453090.0,455570.0,",
        "  458060.0,460560.0,463070.0,465590.0,468120.0,470660.0,473210.0,",
        "  475780.0,478350.0,480940.0,483540.0,486150.0,488770.0,491400.0,",
        "  494040.0,496700.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  159550.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,6.4087,",
        "  6.3023,6.201,6.1045,6.012,5.9233,5.838,5.7559,",
        "  5.6767,5.6001,5.5261,5.4544,5.385,5.3176,5.2521,",
        "  5.1885,5.1267,5.0666,5.008,4.9509,4.8953,4.8411,",
        "  4.7882,4.7366,4.6862,4.6369,4.5888,4.5417,4.4957,",
        "  4.4507,4.4066,4.3635,4.3212,4.2799,4.2393,4.1996,",
        "  4.1607,4.1225;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  182920.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  405570.0,408160.0,410710.0,413230.0,415730.0,418210.0,420680.0,",
        "  423130.0,425590.0,428040.0,430490.0,432930.0,435380.0,437840.0,",
        "  440290.0,442750.0,445220.0,447690.0,450170.0,452660.0,455160.0,",
        "  457660.0,460170.0,462700.0,465230.0,467770.0,470320.0,472880.0,",
        "  475460.0,478040.0,480630.0,483240.0,485860.0,488480.0,491120.0,",
        "  493780.0,496440.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  182920.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  7.2953,7.1732,7.0571,6.9465,6.8408,6.7394,6.642,",
        "  6.5482,6.4578,6.3706,6.2862,6.2046,6.1255,6.0488,",
        "  5.9743,5.902,5.8317,5.7633,5.6968,5.632,5.5688,",
        "  5.5072,5.4472,5.3885,5.3313,5.2754,5.2208,5.1674,",
        "  5.1152,5.0641,5.0141,4.9652,4.9173,4.8703,4.8244,",
        "  4.7793,4.7352;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  208880.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,407010.0,409640.0,412240.0,414800.0,417330.0,419850.0,",
        "  422350.0,424850.0,427330.0,429810.0,432290.0,434770.0,437240.0,",
        "  439720.0,442210.0,444690.0,447190.0,449680.0,452190.0,454700.0,",
        "  457220.0,459740.0,462280.0,464820.0,467380.0,469940.0,472510.0,",
        "  475100.0,477690.0,480290.0,482910.0,485530.0,488170.0,490820.0,",
        "  493480.0,496150.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  208880.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,8.2759,8.1361,8.0034,7.877,7.7563,7.6407,",
        "  7.5297,7.423,7.3201,7.2209,7.1251,7.0323,6.9425,",
        "  6.8555,6.771,6.689,6.6093,6.5318,6.4564,6.383,",
        "  6.3115,6.2417,6.1738,6.1074,6.0426,5.9794,5.9176,",
        "  5.8572,5.7981,5.7404,5.6839,5.6286,5.5744,5.5214,",
        "  5.4694,5.4185;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  237620.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,408420.0,411100.0,413740.0,416340.0,418920.0,",
        "  421470.0,424010.0,426540.0,429050.0,431570.0,434070.0,436580.0,",
        "  439090.0,441590.0,444100.0,446620.0,449140.0,451660.0,454190.0,",
        "  456720.0,459270.0,461820.0,464370.0,466940.0,469520.0,472100.0,",
        "  474700.0,477300.0,479920.0,482540.0,485170.0,487820.0,490480.0,",
        "  493150.0,495820.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  237620.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,9.3578,9.1979,9.0465,8.9024,8.765,",
        "  8.6335,8.5073,8.3861,8.2694,8.1569,8.0482,7.9431,",
        "  7.8414,7.7429,7.6473,7.5546,7.4645,7.3769,7.2917,",
        "  7.2088,7.128,7.0493,6.9726,6.8977,6.8246,6.7533,",
        "  6.6836,6.6155,6.5489,6.4838,6.42,6.3577,6.2967,",
        "  6.2369,6.1783;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  269350.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,409800.0,412530.0,415210.0,417860.0,",
        "  420470.0,423070.0,425640.0,428200.0,430750.0,433300.0,435840.0,",
        "  438370.0,440910.0,443450.0,445990.0,448530.0,451070.0,453620.0,",
        "  456170.0,458730.0,461300.0,463880.0,466460.0,469050.0,471650.0,",
        "  474250.0,476870.0,479500.0,482130.0,484780.0,487430.0,490100.0,",
        "  492780.0,495460.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  269350.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,10.549,10.367,10.194,10.03,",
        "  9.8741,9.7248,9.5817,9.4443,9.3122,9.1848,9.0619,",
        "  8.9431,8.8282,8.717,8.6091,8.5045,8.4029,8.3042,",
        "  8.2081,8.1147,8.0237,7.9351,7.8487,7.7644,7.6822,",
        "  7.6019,7.5235,7.4469,7.3721,7.2989,7.2273,7.1572,",
        "  7.0886,7.0214;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  304260.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,411140.0,413920.0,416650.0,",
        "  419340.0,422000.0,424630.0,427250.0,429840.0,432430.0,435010.0,",
        "  437580.0,440150.0,442720.0,445280.0,447850.0,450420.0,452990.0,",
        "  455560.0,458140.0,460730.0,463320.0,465920.0,468530.0,471140.0,",
        "  473760.0,476390.0,479030.0,481680.0,484340.0,487010.0,489680.0,",
        "  492370.0,495070.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  304260.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,11.857,11.65,11.453,",
        "  11.267,11.09,10.921,10.759,10.604,10.454,10.31,",
        "  10.172,10.038,9.9083,9.783,9.6615,9.5438,9.4294,",
        "  9.3184,9.2104,9.1054,9.0032,8.9037,8.8067,8.7121,",
        "  8.6198,8.5297,8.4418,8.3559,8.2719,8.1898,8.1095,",
        "  8.031,7.9541;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  342570.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,412450.0,415290.0,",
        "  418070.0,420800.0,423500.0,426170.0,428830.0,431460.0,434080.0,",
        "  436700.0,439300.0,441900.0,444500.0,447090.0,449690.0,452290.0,",
        "  454890.0,457490.0,460100.0,462710.0,465330.0,467950.0,470580.0,",
        "  473220.0,475870.0,478520.0,481180.0,483850.0,486530.0,489220.0,",
        "  491920.0,494630.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  342570.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,13.292,13.056,",
        "  12.833,12.622,12.421,12.23,12.047,11.871,11.703,",
        "  11.541,11.385,11.234,11.088,10.947,10.811,10.678,",
        "  10.55,10.425,10.304,10.187,10.072,9.9605,9.8518,",
        "  9.7459,9.6426,9.5417,9.4433,9.3472,9.2533,9.1615,",
        "  9.0717,8.9839;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  384490.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,413730.0,",
        "  416610.0,419440.0,422220.0,424970.0,427680.0,430380.0,433050.0,",
        "  435710.0,438360.0,441000.0,443630.0,446260.0,448890.0,451510.0,",
        "  454140.0,456770.0,459400.0,462030.0,464670.0,467320.0,469970.0,",
        "  472620.0,475290.0,477960.0,480630.0,483320.0,486010.0,488720.0,",
        "  491430.0,494150.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  384490.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,14.861,",
        "  14.593,14.341,14.102,13.875,13.659,13.452,13.255,",
        "  13.065,12.882,12.707,12.537,12.374,12.216,12.063,",
        "  11.914,11.771,11.631,11.495,11.364,11.236,11.111,",
        "  10.989,10.871,10.755,10.643,10.533,10.426,10.321,",
        "  10.218,10.118;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  430240.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  414960.0,417900.0,420780.0,423610.0,426410.0,429160.0,431900.0,",
        "  434610.0,437310.0,439990.0,442670.0,445330.0,448000.0,450660.0,",
        "  453310.0,455970.0,458630.0,461290.0,463950.0,466620.0,469290.0,",
        "  471970.0,474650.0,477340.0,480030.0,482730.0,485440.0,488160.0,",
        "  490890.0,493620.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  430240.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  16.576,16.272,15.986,15.716,15.46,15.216,14.983,",
        "  14.761,14.547,14.343,14.145,13.955,13.772,13.595,",
        "  13.424,13.258,13.097,12.941,12.789,12.642,12.499,",
        "  12.36,12.224,12.092,11.964,11.838,11.716,11.596,",
        "  11.48,11.365;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  480040.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,416150.0,419150.0,422090.0,424970.0,427810.0,430610.0,",
        "  433390.0,436140.0,438880.0,441600.0,444310.0,447010.0,449710.0,",
        "  452400.0,455090.0,457780.0,460470.0,463160.0,465850.0,468550.0,",
        "  471240.0,473950.0,476660.0,479370.0,482090.0,484820.0,487550.0,",
        "  490290.0,493040.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  480040.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,18.447,18.102,17.778,17.473,17.184,16.91,",
        "  16.648,16.398,16.158,15.928,15.707,15.495,15.289,",
        "  15.091,14.9,14.715,14.535,14.361,14.192,14.028,",
        "  13.869,13.714,13.563,13.416,13.273,13.133,12.997,",
        "  12.864,12.734;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  534120.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,417300.0,420360.0,423350.0,426290.0,429170.0,",
        "  432020.0,434850.0,437640.0,440420.0,443180.0,445930.0,448670.0,",
        "  451400.0,454130.0,456850.0,459570.0,462290.0,465010.0,467730.0,",
        "  470450.0,473180.0,475910.0,478650.0,481390.0,484130.0,486890.0,",
        "  489650.0,492410.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  534120.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,20.485,20.094,19.728,19.383,19.058,",
        "  18.749,18.455,18.174,17.905,17.648,17.4,17.162,",
        "  16.933,16.712,16.498,16.292,16.092,15.898,15.71,",
        "  15.527,15.35,15.178,15.01,14.847,14.688,14.533,",
        "  14.382,14.234;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  592730.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,418400.0,421530.0,424580.0,427560.0,",
        "  430500.0,433400.0,436270.0,439110.0,441930.0,444730.0,447520.0,",
        "  450290.0,453060.0,455820.0,458580.0,461330.0,464080.0,466830.0,",
        "  469590.0,472340.0,475090.0,477850.0,480620.0,483380.0,486160.0,",
        "  488940.0,491720.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  592730.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,22.703,22.26,21.846,21.458,",
        "  21.091,20.744,20.413,20.098,19.798,19.509,19.233,",
        "  18.967,18.711,18.465,18.227,17.996,17.774,17.558,",
        "  17.349,17.146,16.95,16.758,16.572,16.391,16.215,",
        "  16.043,15.876;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  656090.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,419450.0,422640.0,425750.0,",
        "  428800.0,431790.0,434740.0,437650.0,440540.0,443400.0,446250.0,",
        "  449070.0,451890.0,454690.0,457490.0,460280.0,463070.0,465850.0,",
        "  468640.0,471420.0,474200.0,476990.0,479780.0,482570.0,485360.0,",
        "  488160.0,490970.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  656090.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,25.113,24.612,24.145,",
        "  23.707,23.294,22.904,22.533,22.18,21.844,21.522,",
        "  21.213,20.916,20.631,20.356,20.091,19.836,19.588,",
        "  19.349,19.117,18.892,18.673,18.461,18.255,18.055,",
        "  17.86,17.67;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  724460.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,420450.0,423710.0,",
        "  426880.0,429990.0,433040.0,436040.0,439000.0,441940.0,444840.0,",
        "  447730.0,450600.0,453450.0,456300.0,459130.0,461960.0,464780.0,",
        "  467600.0,470410.0,473230.0,476040.0,478860.0,481680.0,484500.0,",
        "  487320.0,490150.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  724460.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,27.732,27.164,",
        "  26.636,26.143,25.678,25.24,24.825,24.43,24.053,",
        "  23.694,23.349,23.019,22.701,22.396,22.101,21.817,",
        "  21.542,21.277,21.019,20.77,20.529,20.294,20.066,",
        "  19.844,19.629;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  798080.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,421390.0,",
        "  424720.0,427970.0,431130.0,434240.0,437300.0,440310.0,443290.0,",
        "  446250.0,449180.0,452090.0,454990.0,457870.0,460740.0,463610.0,",
        "  466460.0,469320.0,472170.0,475020.0,477860.0,480710.0,483560.0,",
        "  486410.0,489260.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  798080.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,30.574,",
        "  29.931,29.335,28.779,28.257,27.765,27.299,26.857,",
        "  26.436,26.035,25.651,25.283,24.93,24.59,24.263,",
        "  23.948,23.644,23.349,23.065,22.789,22.521,22.262,",
        "  22.01,21.766;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  877220.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  422270.0,425680.0,428990.0,432230.0,435400.0,438510.0,441580.0,",
        "  444610.0,447610.0,450590.0,453550.0,456490.0,459410.0,462320.0,",
        "  465230.0,468120.0,471010.0,473890.0,476780.0,479660.0,482540.0,",
        "  485420.0,488300.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  877220.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  33.659,32.93,32.256,31.629,31.042,30.49,29.968,",
        "  29.474,29.004,28.557,28.129,27.72,27.327,26.95,",
        "  26.587,26.238,25.9,25.575,25.26,24.955,24.66,",
        "  24.374,24.096;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  962140.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,423080.0,426580.0,429970.0,433270.0,436500.0,439680.0,",
        "  442800.0,445890.0,448940.0,451970.0,454970.0,457950.0,460920.0,",
        "  463870.0,466810.0,469750.0,472670.0,475590.0,478510.0,481420.0,",
        "  484340.0,487250.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  962140.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,37.005,36.178,35.416,34.709,34.049,33.429,",
        "  32.845,32.292,31.768,31.269,30.793,30.338,29.902,",
        "  29.484,29.082,28.694,28.321,27.961,27.613,27.277,",
        "  26.951,26.636;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1053100.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,423820.0,427410.0,430880.0,434260.0,437560.0,",
        "  440790.0,443980.0,447120.0,450230.0,453300.0,456350.0,459380.0,",
        "  462390.0,465390.0,468370.0,471340.0,474300.0,477260.0,480210.0,",
        "  483160.0,486110.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1053100.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,40.634,39.695,38.832,38.035,37.292,",
        "  36.597,35.943,35.325,34.74,34.184,33.654,33.149,",
        "  32.665,32.201,31.755,31.327,30.915,30.517,30.133,",
        "  29.762,29.403;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1150400.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,424480.0,428170.0,431730.0,435180.0,",
        "  438550.0,441860.0,445110.0,448310.0,451470.0,454590.0,457690.0,",
        "  460770.0,463830.0,466860.0,469890.0,472900.0,475910.0,478900.0,",
        "  481890.0,484880.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1150400.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,44.572,43.503,42.527,41.626,",
        "  40.79,40.01,39.277,38.587,37.934,37.314,36.725,",
        "  36.164,35.627,35.113,34.62,34.146,33.691,33.252,",
        "  32.828,32.419;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1254300.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,425070.0,428860.0,432510.0,",
        "  436050.0,439490.0,442870.0,446180.0,449440.0,452660.0,455840.0,",
        "  458990.0,462120.0,465220.0,468310.0,471380.0,474430.0,477480.0,",
        "  480510.0,483540.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1254300.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,48.844,47.627,46.519,",
        "  45.502,44.56,43.684,42.863,42.091,41.363,40.673,",
        "  40.018,39.394,38.799,38.23,37.685,37.161,36.658,",
        "  36.174,35.708;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1365100.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,425560.0,429470.0,",
        "  433220.0,436840.0,440370.0,443820.0,447200.0,450530.0,453810.0,",
        "  457050.0,460250.0,463430.0,466580.0,469710.0,472830.0,475920.0,",
        "  479010.0,482090.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1365100.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,53.484,52.094,",
        "  50.835,49.684,48.623,47.638,46.718,45.855,45.042,",
        "  44.274,43.546,42.854,42.194,41.564,40.961,40.383,",
        "  39.828,39.294;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1483100.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,425950.0,",
        "  429990.0,433850.0,437570.0,441190.0,444710.0,448170.0,451560.0,",
        "  454900.0,458200.0,461460.0,464690.0,467900.0,471080.0,474240.0,",
        "  477380.0,480510.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1483100.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,58.526,",
        "  56.935,55.502,54.199,53.001,51.893,50.861,49.896,",
        "  48.989,48.133,47.324,46.555,45.824,45.127,44.461,",
        "  43.823,43.211;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1608600.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  426240.0,430410.0,434390.0,438220.0,441930.0,445540.0,449070.0,",
        "  452540.0,455950.0,459310.0,462630.0,465910.0,469170.0,472400.0,",
        "  475610.0,478800.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1608600.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  64.013,62.185,60.551,59.071,57.718,56.47,55.313,",
        "  54.232,53.22,52.267,51.367,50.514,49.704,48.933,",
        "  48.196,47.492;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1741900.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,426420.0,430740.0,434850.0,438790.0,442600.0,446300.0,",
        "  449910.0,453450.0,456930.0,460360.0,463740.0,467090.0,470400.0,",
        "  473680.0,476940.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1741900.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,69.99,67.883,66.014,64.331,62.8,61.394,",
        "  60.094,58.884,57.753,56.691,55.691,54.744,53.847,",
        "  52.994,52.181;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1883400.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,426460.0,430960.0,435210.0,439270.0,443190.0,",
        "  446990.0,450690.0,454310.0,457860.0,461360.0,464810.0,468210.0,",
        "  471580.0,474920.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1883400.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,76.519,74.08,71.935,70.017,68.281,",
        "  66.694,65.232,63.876,62.613,61.429,60.316,59.266,",
        "  58.272,57.328;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  2033400.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,426350.0,431050.0,435460.0,439660.0,",
        "  443690.0,447600.0,451390.0,455100.0,458730.0,462300.0,465820.0,",
        "  469290.0,472720.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  2033400.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,83.665,80.827,78.356,76.164,",
        "  74.192,72.398,70.753,69.232,67.819,66.499,65.261,",
        "  64.095,62.993;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  2192300.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,426090.0,431010.0,435600.0,",
        "  439950.0,444110.0,448120.0,452020.0,455820.0,459530.0,463180.0,",
        "  466770.0,470300.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  2192300.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,91.511,88.19,85.332,",
        "  82.819,80.573,78.542,76.687,74.98,73.398,71.925,",
        "  70.547,69.252;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  2360400.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,425640.0,430820.0,",
        "  435610.0,440130.0,444430.0,448560.0,452570.0,456460.0,460270.0,",
        "  464000.0,467660.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  2360400.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,100.15,96.239,",
        "  92.918,90.027,87.463,85.159,83.065,81.145,79.374,",
        "  77.729,76.195;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  2538200.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,424980.0,",
        "  430470.0,435490.0,440190.0,444640.0,448910.0,453030.0,457030.0,",
        "  460930.0,464750.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  2538200.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,109.72,",
        "  105.07,101.19,97.851,94.915,92.295,89.926,87.766,",
        "  85.78,83.942;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  2726100.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  424080.0,429930.0,435220.0,440120.0,444750.0,449160.0,453410.0,",
        "  457520.0,461520.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  2726100.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  120.38,114.79,110.23,106.35,102.98,99.995,97.312,",
        "  94.877,92.647;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  2924600.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,422900.0,429180.0,434780.0,439920.0,444730.0,449310.0,",
        "  453690.0,457920.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  2924600.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,132.33,125.54,120.13,115.62,111.73,108.31,",
        "  105.27,102.52;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  3134100.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,421370.0,428200.0,434160.0,439570.0,444590.0,",
        "  449340.0,453870.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  3134100.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,145.85,137.48,131.02,125.72,121.22,",
        "  117.31,113.84;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  3355200.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,419410.0,426950.0,433340.0,439050.0,",
        "  444320.0,449260.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  3355200.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,161.35,150.82,143.02,136.77,",
        "  131.54,127.04;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  3588600.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,416920.0,425390.0,432290.0,",
        "  438360.0,443890.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  3588600.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,179.41,165.85,156.32,",
        "  148.9,142.79;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  3834800.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,413700.0,423460.0,",
        "  431000.0,437480.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  3834800.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,200.95,182.89,",
        "  171.11,162.23;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  4094900.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,409420.0,",
        "  421090.0,429430.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  4094900.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,227.66,",
        "  202.46,187.65;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  4369700.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  403430.0,418230.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  4369700.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  263.02,225.13;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  4660700.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,393730.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  4660700.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,317.58;",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    state->dataGlobal->TimeStepsInHour = 1;
    state->dataGlobal->TimeStep = 1;
    state->dataGlobal->MinutesInTimeStep = 60;
    state->init_state(*state);

    GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    GetZoneEquipmentData(*state);
    GetZoneAirLoopEquipment(*state);

    GetVRFInput(*state);
    state->dataHVACVarRefFlow->GetVRFInputFlag = false;

    // get input test for terminal air single duct mixer on inlet side of VRF terminal unit
    ASSERT_EQ(1, state->dataSingleDuct->NumATMixers);
    EXPECT_EQ("SPACE1-1 DOAS AIR TERMINAL", state->dataSingleDuct->SysATMixer(1).Name);                 // single duct air terminal mixer name
    EXPECT_EQ((int)HVAC::MixerType::InletSide, (int)state->dataSingleDuct->SysATMixer(1).type);         // air terminal mixer connection type
    EXPECT_EQ("AIRTERMINAL:SINGLEDUCT:MIXER", state->dataDefineEquipment->AirDistUnit(1).EquipType(1)); // Air distribution unit equipment type
    EXPECT_EQ("TU1", state->dataHVACVarRefFlow->VRFTU(1).Name);                                         // zoneHVAC equipment name

    state->dataGlobal->BeginEnvrnFlag = false;

    // set input variables
    state->dataEnvrn->OutBaroPress = 101325.0;
    state->dataEnvrn->OutDryBulbTemp = 35.0;
    state->dataEnvrn->OutHumRat = 0.0098;
    state->dataEnvrn->OutEnthalpy = Psychrometrics::PsyHFnTdbW(state->dataEnvrn->OutDryBulbTemp, state->dataEnvrn->OutHumRat);
    state->dataEnvrn->OutWetBulbTemp =
        PsyTwbFnTdbWPb(*state, state->dataEnvrn->OutDryBulbTemp, state->dataEnvrn->OutHumRat, state->dataEnvrn->OutBaroPress);
    state->dataEnvrn->StdRhoAir = 1.20;
    HVACInletMassFlowRate = 0.50;
    PrimaryAirMassFlowRate = 0.1;
    SecondaryAirMassFlowRate = HVACInletMassFlowRate - PrimaryAirMassFlowRate; // secondary air flow is VRFTU flow less primary air flow

    // set zoneNode air condition
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp = 24.0;
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat = 0.0075;
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Enthalpy =
        Psychrometrics::PsyHFnTdbW(state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp,
                                   state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat);

    state->dataHVACVarRefFlow->CoolingLoad.allocate(1);
    state->dataHVACVarRefFlow->HeatingLoad.allocate(1);
    state->dataHVACVarRefFlow->HeatingLoad(1) = false;
    state->dataHVACVarRefFlow->CoolingLoad(1) = true;
    state->dataHVACVarRefFlow->CompOnMassFlow = HVACInletMassFlowRate;    // supply air mass flow rate
    state->dataHVACVarRefFlow->OACompOnMassFlow = PrimaryAirMassFlowRate; // OA mass flow rate
    state->dataHVACVarRefFlow->CompOnFlowRatio = 1.0;                     // compressor is on
    state->dataHVACGlobal->TurnFansOff = false;
    state->dataHVACGlobal->TurnFansOn = true;

    VRFNum = 1;
    VRFTUNum = 1;
    state->dataHVACVarRefFlow->VRFTU(VRFTUNum).fanOp = HVAC::FanOp::Cycling;
    state->dataHVACVarRefFlow->VRFTU(VRFTUNum).isInZone = true;
    state->dataHVACVarRefFlow->VRFTU(VRFTUNum).ZoneAirNode = state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode;
    // initialize mass flow rates
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).VRFTUInletNodeNum).MassFlowRate = HVACInletMassFlowRate;
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).ATMixerPriNode).MassFlowRate = PrimaryAirMassFlowRate;
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).ATMixerPriNode).MassFlowRateMaxAvail = PrimaryAirMassFlowRate;

    // set fan parameters
    state->dataFans->fans(1)->maxAirMassFlowRate = HVACInletMassFlowRate;
    state->dataFans->fans(1)->inletAirMassFlowRate = HVACInletMassFlowRate;
    state->dataFans->fans(1)->rhoAirStdInit = state->dataEnvrn->StdRhoAir;
    state->dataLoopNodes->Node(state->dataFans->fans(1)->inletNodeNum).MassFlowRateMaxAvail = HVACInletMassFlowRate;
    state->dataLoopNodes->Node(state->dataFans->fans(1)->outletNodeNum).MassFlowRateMax = HVACInletMassFlowRate;

    // set DX coil rated performance parameters
    state->dataDXCoils->DXCoil(1).RatedCBF(1) = 0.05;
    state->dataDXCoils->DXCoil(1).RatedAirMassFlowRate(1) = HVACInletMassFlowRate;

    // primary air condition set at outdoor air condition
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).ATMixerPriNode).Temp = state->dataEnvrn->OutDryBulbTemp;
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).ATMixerPriNode).HumRat = state->dataEnvrn->OutHumRat;
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).ATMixerPriNode).Enthalpy = state->dataEnvrn->OutEnthalpy;

    // set secondary air (recirculating air) conditions to zone air node
    state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).Temp =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp;
    state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).HumRat =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat;
    state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).Enthalpy =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Enthalpy;

    state->dataHVACVarRefFlow->VRFTU(1).ZoneNum = 1;
    state->dataSize->SysSizingRunDone = true;
    state->dataSize->ZoneSizingRunDone = true;
    state->dataGlobal->SysSizingCalc = true;

    state->dataZoneEnergyDemand->ZoneSysEnergyDemand.allocate(1);
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputReqToHeatSP = 0.0;
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputReqToCoolSP = -5000.0;
    QZnReq = state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputReqToCoolSP;

    state->dataHVACVarRefFlow->VRFTU(VRFTUNum).availSched->currentVal = 1.0;    // unit is always available
    state->dataHVACVarRefFlow->VRFTU(VRFTUNum).fanAvailSched->currentVal = 1.0; // fan is always available

    // set secondary air mass flow rate to zero
    state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).MassFlowRate = 0.0;
    state->dataFans->fans(1)->set_size(*state); // add sizing
    state->dataHVACVarRefFlow->VRF(1).VRFCondCyclingRatio = 1.0;
    // Simulate zoneHVAC equipment (VRF terminal unit)
    SimVRF(*state, VRFTUNum, FirstHVACIteration, OnOffAirFlowRatio, QUnitOutVRFTU, LatOutputProvided, QZnReq);

    // check the terminal air mixer secondary air mass flow rate, requires updating the secondary flow
    SecondaryAirMassFlowRate =
        state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).VRFTUInletNodeNum).MassFlowRate - PrimaryAirMassFlowRate;
    ASSERT_EQ(SecondaryAirMassFlowRate, state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).MassFlowRate);
    // check the terminal air mixer outlet flow rate must be equal to VRFTU mass flow rate
    HVACInletMassFlowRate = state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).VRFTUInletNodeNum).MassFlowRate;
    ASSERT_EQ(HVACInletMassFlowRate, state->dataSingleDuct->SysATMixer(1).MixedAirMassFlowRate);
    // check the cooling output delivered is within 5.0 Watt of zone cooling load
    ASSERT_NEAR(QZnReq, QUnitOutVRFTU, 5.0);
    EXPECT_NEAR(0.965, state->dataDXCoils->DXCoil(1).PartLoadRatio, 0.001);
}

TEST_F(EnergyPlusFixture, AirTerminalSingleDuctMixer_SimVRFfluidCntrl_ATMSupplySide)
{

    bool ErrorsFound(false);
    bool FirstHVACIteration(false);
    Real64 HVACInletMassFlowRate(0.0);
    Real64 PrimaryAirMassFlowRate(0.0);
    Real64 SecondaryAirMassFlowRate(0.0);
    Real64 ATMixerOutletMassFlowRate(0.0);
    Real64 OnOffAirFlowRatio(1.0);
    Real64 LatOutputProvided(0.0);
    Real64 QUnitOutVRFTU(0.0);
    Real64 QZnReq(0.0);
    int VRFNum(1);
    int VRFTUNum(1);

    std::string const idf_objects = delimited_string({
        "AirTerminal:SingleDuct:Mixer,",
        "  SPACE1-1 DOAS Air Terminal,  !- Name",
        "  ZoneHVAC:TerminalUnit:VariableRefrigerantFlow,  !- ZoneHVAC Unit Object Type",
        "  TU1,                     !- ZoneHVAC Unit Object Name",
        "  TU1 Outlet Node,         !- Mixer Outlet Node Name",
        "  SPACE1-1 Air Terminal Mixer Primary Inlet,  !- Mixer Primary Air Inlet Node Name",
        "  SPACE1-1 Air Terminal Mixer Secondary Inlet,  !- Mixer Secondary Air Inlet Node Name",
        "  SupplySide;              !- Mixer Connection Type",

        "ZoneHVAC:AirDistributionUnit,",
        "  SPACE1-1 DOAS ATU,       !- Name",
        "  TU1 Outlet Node,         !- Air Distribution Unit Outlet Node Name",
        "  AirTerminal:SingleDuct:Mixer,  !- Air Terminal Object Type",
        "  SPACE1-1 DOAS Air Terminal;  !- Air Terminal Name",

        "Schedule:Compact,",
        "  FanAvailSched,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,1.0;        !- Field 3",

        "Schedule:Compact,",
        "  VRFAvailSched,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,1.0;        !- Field 3",

        "Schedule:Compact,",
        "  CyclingFanSch,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,0.0;        !- Field 3",

        "ZoneHVAC:EquipmentList,",
        "  SPACE1-1 Eq,             !- Name",
        "  SequentialLoad,          !- Load Distribution Scheme",
        "  ZoneHVAC:AirDistributionUnit,  !- Zone Equipment 1 Object Type",
        "  SPACE1-1 DOAS ATU,       !- Zone Equipment 1 Name",
        "  1,                       !- Zone Equipment 1 Cooling Sequence",
        "  1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 1 Sequential Cooling Fraction Schedule Name",
        "  ,                        !- Zone Equipment 1 Sequential Heating Fraction Schedule Name",
        "  ZoneHVAC:TerminalUnit:VariableRefrigerantFlow,  !- Zone Equipment 2 Object Type",
        "  TU1,                     !- Zone Equipment 2 Name",
        "  2,                       !- Zone Equipment 2 Cooling Sequence",
        "  2,                       !- Zone Equipment 2 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 2 Sequential Cooling Fraction Schedule Name",
        "  ;                        !- Zone Equipment 2 Sequential Heating Fraction Schedule Name",

        "ZoneHVAC:TerminalUnit:VariableRefrigerantFlow,",
        "  TU1,                     !- Zone Terminal Unit Name",
        "  VRFAvailSched,           !- Terminal Unit Availability Schedule",
        "  zTU1 Inlet Node,         !- Terminal Unit Air Inlet Node Name",
        "  SPACE1-1 Air Terminal Mixer Secondary Inlet,  !- Terminal Unit Air Outlet Node Name",
        "  0.5,                     !- Cooling Supply Air Flow Rate {m3/s}",
        "  0.5,                     !- No Cooling Supply Air Flow Rate {m3/s}",
        "  0.5,                     !- Heating Supply Air Flow Rate {m3/s}",
        "  0.5,                     !- No Heating Supply Air Flow Rate {m3/s}",
        "  0.0,                     !- Cooling Outdoor Air Flow Rate {m3/s}",
        "  0.0,                     !- Heating Outdoor Air Flow Rate {m3/s}",
        "  0.0,                     !- No Load Outdoor Air Flow Rate {m3/s}",
        "  VRFFanSchedule,          !- Supply Air Fan Operating Mode Schedule Name",
        "  DrawThrough,             !- Supply Air Fan Placement",
        "  Fan:SystemModel,         !- Supply Air Fan Object Type",
        "  TU1 VRF Supply Fan,      !- Supply Air Fan Object Name",
        "  ,                        !- Outside Air Mixer Object Type",
        "  ,                        !- Outside Air Mixer Object Name",
        "  Coil:Cooling:DX:VariableRefrigerantFlow:FluidTemperatureControl,  !- Cooling Coil Object Type",
        "  TU1 VRF DX Cooling Coil,  !- Cooling Coil Object Name",
        "  Coil:Heating:DX:VariableRefrigerantFlow:FluidTemperatureControl,  !- Heating Coil Object Type",
        "  TU1 VRF DX Heating Coil,  !- Heating Coil Object Name",
        "  30.0,                    !- Zone Terminal Unit On Parasitic Electric Energy Use {W}",
        "  20.0,                    !- Zone Terminal Unit Off Parasitic Electric Energy Use {W}",
        "  ,                        !- Rated Heating Capacity Sizing Ratio {W/W}",
        "  ,                        !- Availability Manager List Name",
        "  ,                        !- Design Specification ZoneHVAC Sizing Object Name",
        "  ,                        !- Supplemental Heating Coil Object Type",
        "  ,                        !- Supplemental Heating Coil Name",
        "  ,                        !- Maximum Supply Air Temperature from Supplemental Heater {C}",
        "  ;                        !- Maximum Outdoor Dry-Bulb Temperature for Supplemental Heater Operation {C}",

        "Fan:SystemModel,",
        "  TU1 VRF Supply Fan,      !- Name",
        "  VRFAvailSched,           !- Availability Schedule Name",
        "  TU1 VRF DX HCoil Outlet Node,  !- Air Inlet Node Name",
        "  SPACE1-1 AIR TERMINAL MIXER SECONDARY INLET,  !- Air Outlet Node Name",
        "  0.5,                     !- Design Maximum Air Flow Rate {m3/s}",
        "  Continuous,              !- Speed Control Method",
        "  0.0,                     !- Electric Power Minimum Flow Rate Fraction",
        "  600.0,                   !- Design Pressure Rise {Pa}",
        "  0.9,                     !- Motor Efficiency",
        "  1.0,                     !- Motor In Air Stream Fraction",
        "  Autosize,                !- Design Electric Power Consumption {W}",
        "  TotalEfficiencyAndPressure,  !- Design Power Sizing Method",
        "  ,                        !- Electric Power Per Unit Flow Rate {W/(m3/s)}",
        "  ,                        !- Electric Power Per Unit Flow Rate Per Unit Pressure {W/((m3/s)-Pa)}",
        "  0.7,                     !- Fan Total Efficiency",
        "  TU1 Fan Power Curve,     !- Electric Power Function of Flow Fraction Curve Name",
        "  ,                        !- Night Ventilation Mode Pressure Rise {Pa}",
        "  ,                        !- Night Ventilation Mode Flow Fraction",
        "  ,                        !- Motor Loss Zone Name",
        "  ,                        !- Motor Loss Radiative Fraction",
        "  General;                 !- End-Use Subcategory",

        "Curve:Quartic,",
        "  TU1 Fan Power Curve,     !- Name",
        "  0.059,                   !- Coefficient1 Constant",
        "  0.0,                     !- Coefficient2 x",
        "  0.0,                     !- Coefficient3 x**2",
        "  0.928,                   !- Coefficient4 x**3",
        "  0.0,                     !- Coefficient5 x**4",
        "  0.0,                     !- Minimum Value of x",
        "  1.0,                     !- Maximum Value of x",
        "  0.0,                     !- Minimum Curve Output",
        "  1.0,                     !- Maximum Curve Output",
        "  Dimensionless,           !- Input Unit Type for X",
        "  Dimensionless;           !- Output Unit Type",

        "Coil:Heating:DX:VariableRefrigerantFlow:FluidTemperatureControl,",
        "  TU1 VRF DX Heating Coil,  !- Name",
        "  VRFAvailSched,           !- Availability Schedule",
        "  TU1 VRF DX CCoil Outlet Node,  !- Coil Air Inlet Node",
        "  TU1 VRF DX HCoil Outlet Node,  !- Coil Air Outlet Node",
        "  6600.0,                  !- Rated Total Heating Capacity {W}",
        "  5.0,                     !- Indoor Unit Reference Subcooling {deltaC}",
        "  IUCondTempCurve;         !- Indoor Unit Condensing Temperature Function of Subcooling Curve Name",

        "Curve:Quadratic,",
        "  IUCondTempCurve,         !- Name",
        "  -1.85,                   !- Coefficient1 Constant",
        "  0.411,                   !- Coefficient2 x",
        "  0.0196,                  !- Coefficient3 x**2",
        "  0.0,                     !- Minimum Value of x",
        "  20.0,                    !- Maximum Value of x",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature;             !- Output Unit Type",

        "Coil:Cooling:DX:VariableRefrigerantFlow:FluidTemperatureControl,",
        "  TU1 VRF DX Cooling Coil,  !- Name",
        "  VRFAvailSched,           !- Availability Schedule Name",
        "  zTU1 INLET NODE,         !- Coil Air Inlet Node",
        "  TU1 VRF DX CCoil Outlet Node,  !- Coil Air Outlet Node",
        "  6600.0,                  !- Rated Total Cooling Capacity {W}",
        "  0.5,                     !- Rated Sensible Heat Ratio",
        "  3.0,                     !- Indoor Unit Reference Superheating {deltaC}",
        "  IUEvapTempCurve,         !- Indoor Unit Evaporating Temperature Function of Superheating Curve Name",
        "  ;                        !- Name of Water Storage Tank for Condensate Collection",

        "Curve:Quadratic,",
        "  IUEvapTempCurve,         !- Name",
        "  0.0,                     !- Coefficient1 Constant",
        "  0.843,                   !- Coefficient2 x",
        "  0.0,                     !- Coefficient3 x**2",
        "  0.0,                     !- Minimum Value of x",
        "  15.0,                    !- Maximum Value of x",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature;             !- Output Unit Type",

        "Zone,",
        "  SPACE1-1,                !- Name",
        "  0.0,                     !- Direction of Relative North {deg}",
        "  0.0,                     !- X Origin {m}",
        "  0.0,                     !- Y Origin {m}",
        "  0.0,                     !- Z Origin {m}",
        "  1,                       !- Type",
        "  1,                       !- Multiplier",
        "  2.438400269,             !- Ceiling Height {m}",
        "  239.247360229;           !- Volume {m3}",

        "ZoneHVAC:EquipmentConnections,",
        "  SPACE1-1,                !- Zone Name",
        "  SPACE1-1 Eq,             !- Zone Conditioning Equipment List Name",
        "  SPACE1-1 In Nodes,       !- Zone Air Inlet Node or NodeList Name",
        "  zTU1 Inlet Node,         !- Zone Air Exhaust Node or NodeList Name",
        "  SPACE1-1 Node,           !- Zone Air Node Name",
        "  SPACE1-1 Return Outlet;  !- Zone Return Air Node or NodeList Name",

        "NodeList,",
        "  SPACE1-1 In Nodes,       !- Name",
        "  TU1 Outlet Node;         !- Node 1 Name",

        "AirConditioner:VariableRefrigerantFlow:FluidTemperatureControl,",
        "  VRF Heat Pump,           !- Heat Pump Name",
        "  VRFAvailSched,           !- Availability Schedule Name",
        "  VRF Heat Pump TU List,   !- Zone Terminal Unit List Name",
        "  R410A,                   !- Refrigerant Type",
        "  41300.0,                 !- Rated Evaporative Capacity {W}",
        "  0.344,                   !- Rated Compressor Power Per Unit of Rated Evaporative Capacity {dimensionless}",
        "  -5.0,                    !- Minimum Outdoor Air Temperature in Cooling Mode {C}",
        "  43.0,                    !- Maximum Outdoor Air Temperature in Cooling Mode {C}",
        "  -20.0,                   !- Minimum Outdoor Air Temperature in Heating Mode {C}",
        "  22.0,                    !- Maximum Outdoor Air Temperature in Heating Mode {C}",
        "  3.0,                     !- Reference Outdoor Unit Superheating {deltaC}",
        "  3.0,                     !- Reference Outdoor Unit Subcooling {deltaC}",
        "  ConstantTemp,            !- Refrigerant Temperature Control Algorithm for Indoor Unit",
        "  6.0,                     !- Reference Evaporating Temperature for Indoor Unit {C}",
        "  44.0,                    !- Reference Condensing Temperature for Indoor Unit {C}",
        "  6.0,                     !- Variable Evaporating Temperature Minimum for Indoor Unit {C}",
        "  13.0,                    !- Variable Evaporating Temperature Maximum for Indoor Unit {C}",
        "  42.0,                    !- Variable Condensing Temperature Minimum for Indoor Unit {C}",
        "  46.0,                    !- Variable Condensing Temperature Maximum for Indoor Unit {C}",
        "  0.00412,                 !- Outdoor Unit Fan Power Per Unit of Rated Evaporative Capacity {dimensionless}",
        "  7.26e-05,                !- Outdoor Unit Fan Flow Rate Per Unit of Rated Evaporative Capacity {m3/s-W}",
        "  OUEvapTempCurve,         !- Outdoor Unit Evaporating Temperature Function of Superheating Curve Name",
        "  OUCondTempCurve,         !- Outdoor Unit Condensing Temperature Function of Subcooling Curve Name",
        "  0.0508,                  !- Diameter of Main Pipe Connecting Outdoor Unit to the First Branch Joint {m}",
        "  30.0,                    !- Length of Main Pipe Connecting Outdoor Unit to the First Branch Joint {m}",
        "  36.0,                    !- Equivalent Length of Main Pipe Connecting Outdoor Unit to the First Branch Joint {m}",
        "  5.0,                     !- Height Difference Between Outdoor Unit and Indoor Units {m}",
        "  0.02,                    !- Main Pipe Insulation Thickness {m}",
        "  0.032,                   !- Main Pipe Insulation Thermal Conductivity {W/m-K}",
        "  33.0,                    !- Crankcase Heater Power per Compressor {W}",
        "  1,                       !- Number of Compressors {dimensionless}",
        "  0.33,                    !- Ratio of Compressor Size to Total Compressor Capacity {W/W}",
        "  7.0,                     !- Maximum Outdoor Dry-Bulb Temperature for Crankcase Heater {C}",
        "  ,                        !- Defrost Strategy",
        "  ,                        !- Defrost Control",
        "  ,                        !- Defrost Energy Input Ratio Modifier Function of Temperature Curve Name",
        "  ,                        !- Defrost Time Period Fraction",
        "  ,                        !- Resistive Defrost Heater Capacity {W}",
        "  ,                        !- Maximum Outdoor Dry-bulb Temperature for Defrost Operation {C}",
        "  4500000.0,               !- Compressor maximum delta Pressure {Pa}",
        "  3,                       !- Number of Compressor Loading Index Entries",
        "  1500.0,                  !- Compressor Speed at Loading Index 1 {rev/min}",
        "  MinSpdCooling,           !- Loading Index 1 Evaporative Capacity Multiplier Function of Temperature Curve Name",
        "  MinSpdPower,             !- Loading Index 1 Compressor Power Multiplier Function of Temperature Curve Name",
        "  3600.0,                  !- Compressor Speed at Loading Index 2 {rev/min}",
        "  Spd1Cooling,             !- Loading Index 2 Evaporative Capacity Multiplier Function of Temperature Curve Name",
        "  Spd1Power,               !- Loading Index 2 Compressor Power Multiplier Function of Temperature Curve Name",
        "  6000.0,                  !- Compressor Speed at Loading Index 3 {rev/min}",
        "  Spd2Cooling,             !- Loading Index 3 Evaporative Capacity Multiplier Function of Temperature Curve Name",
        "  Spd2Power;               !- Loading Index 3 Compressor Power Multiplier Function of Temperature Curve Name",

        "Curve:Quadratic,",
        "  OUEvapTempCurve,         !- Name",
        "  0.0,                     !- Coefficient1 Constant",
        "  0.605,                   !- Coefficient2 x",
        "  0.025,                   !- Coefficient3 x**2",
        "  0.0,                     !- Minimum Value of x",
        "  15.0,                    !- Maximum Value of x",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature;             !- Output Unit Type",

        "Curve:Quadratic,",
        "  OUCondTempCurve,         !- Name",
        "  0.0,                     !- Coefficient1 Constant",
        "  -2.91,                   !- Coefficient2 x",
        "  1.18,                    !- Coefficient3 x**2",
        "  0.0,                     !- Minimum Value of x",
        "  20.0,                    !- Maximum Value of x",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature;             !- Output Unit Type",

        "Curve:Biquadratic,",
        "  MinSpdCooling,           !- Name",
        "  0.319,                   !- Coefficient1 Constant",
        "  -0.00126,                !- Coefficient2 x",
        "  -2.15e-05,               !- Coefficient3 x**2",
        "  0.012,                   !- Coefficient4 y",
        "  0.000105,                !- Coefficient5 y**2",
        "  -8.66e-05,               !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  65.0,                    !- Maximum Value of x",
        "  -30.0,                   !- Minimum Value of y",
        "  15.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Biquadratic,",
        "  MinSpdPower,             !- Name",
        "  0.0879,                  !- Coefficient1 Constant",
        "  -0.000172,               !- Coefficient2 x",
        "  6.93e-05,                !- Coefficient3 x**2",
        "  -3.38e-05,               !- Coefficient4 y",
        "  -8.1e-06,                !- Coefficient5 y**2",
        "  -1.04e-05,               !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  65.0,                    !- Maximum Value of x",
        "  -30.0,                   !- Minimum Value of y",
        "  15.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Biquadratic,",
        "  Spd1Cooling,             !- Name",
        "  0.812,                   !- Coefficient1 Constant",
        "  -0.00423,                !- Coefficient2 x",
        "  -4.11e-05,               !- Coefficient3 x**2",
        "  0.0297,                  !- Coefficient4 y",
        "  0.000267,                !- Coefficient5 y**2",
        "  -0.000223,               !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  65.0,                    !- Maximum Value of x",
        "  -30.0,                   !- Minimum Value of y",
        "  15.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Biquadratic,",
        "  Spd1Power,               !- Name",
        "  0.326,                   !- Coefficient1 Constant",
        "  -0.0022,                 !- Coefficient2 x",
        "  0.000142,                !- Coefficient3 x**2",
        "  0.00282,                 !- Coefficient4 y",
        "  2.86e-05,                !- Coefficient5 y**2",
        "  -3.5e-05,                !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  65.0,                    !- Maximum Value of x",
        "  -30.0,                   !- Minimum Value of y",
        "  15.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Biquadratic,",
        "  Spd2Cooling,             !- Name",
        "  1.32,                    !- Coefficient1 Constant",
        "  -0.0062,                 !- Coefficient2 x",
        "  -7.1e-05,                !- Coefficient3 x**2",
        "  0.0489,                  !- Coefficient4 y",
        "  0.000459,                !- Coefficient5 y**2",
        "  -0.000367,               !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  65.0,                    !- Maximum Value of x",
        "  -30.0,                   !- Minimum Value of y",
        "  15.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "Curve:Biquadratic,",
        "  Spd2Power,               !- Name",
        "  0.656,                   !- Coefficient1 Constant",
        "  -0.00371,                !- Coefficient2 x",
        "  0.000207,                !- Coefficient3 x**2",
        "  0.0105,                  !- Coefficient4 y",
        "  7.36e-05,                !- Coefficient5 y**2",
        "  -0.000157,               !- Coefficient6 x*y",
        "  15.0,                    !- Minimum Value of x",
        "  65.0,                    !- Maximum Value of x",
        "  -30.0,                   !- Minimum Value of y",
        "  15.0,                    !- Maximum Value of y",
        "  ,                        !- Minimum Curve Output",
        "  ,                        !- Maximum Curve Output",
        "  Temperature,             !- Input Unit Type for X",
        "  Temperature,             !- Input Unit Type for Y",
        "  Dimensionless;           !- Output Unit Type",

        "ZoneTerminalUnitList,",
        "  VRF Heat Pump TU List,   !- Zone Terminal Unit List Name",
        "  TU1;                     !- Zone Terminal Unit Name 1",

        "OutdoorAir:NodeList,",
        "  OutsideAirInletNodes;    !- Node or NodeList Name 1",

        "NodeList,",
        "  OutsideAirInletNodes,    !- Name",
        "  MyVRFOANode;             !- Node 1 Name",

        "FluidProperties:Name,",
        "  R410a,                   !- Fluid Name",
        "  Refrigerant;             !- Fluid Type",

        "FluidProperties:Temperatures,",
        "  R410aSaturatedTemperatures,  !- Name",
        "  -72.0,-69.0,-66.0,-63.0,-60.0,-57.0,-54.0,",
        "  -51.0,-48.0,-45.0,-42.0,-39.0,-36.0,-33.0,",
        "  -30.0,-27.0,-24.0,-21.0,-18.0,-15.0,-12.0,",
        "  -9.0,-6.0,-3.0,0.0,3.0,6.0,9.0,",
        "  12.0,15.0,18.0,21.0,24.0,27.0,30.0,",
        "  33.0,36.0,39.0,42.0,45.0,48.0,51.0,",
        "  54.0,57.0,60.0,63.0,66.0,69.0;",

        "FluidProperties:Temperatures,",
        "  R410aSuperHeatTemperatures,  !- Name",
        "  -72.0,-66.0,-60.0,-54.0,-48.0,-45.0,-42.0,",
        "  -39.0,-36.0,-33.0,-30.0,-27.0,-24.0,-21.0,",
        "  -18.0,-15.0,-12.0,-9.0,-6.0,-3.0,0.0,",
        "  3.0,6.0,9.0,12.0,15.0,18.0,21.0,",
        "  24.0,27.0,30.0,33.0,36.0,39.0,42.0,",
        "  45.0,48.0,51.0,54.0,57.0,60.0,63.0,",
        "  66.0,69.0;",

        "FluidProperties:Saturated,",
        "  R410a,                   !- Fluid Name",
        "  Pressure,                !- Fluid Property Type",
        "  FluidGas,                !- Fluid Phase",
        "  R410aSaturatedTemperatures,  !- Temperature Values Name",
        "  31238.0,37717.0,45248.0,53954.0,63963.0,75412.0,88445.0,",
        "  103210.0,119880.0,138600.0,159550.0,182920.0,208880.0,237620.0,",
        "  269350.0,304260.0,342570.0,384490.0,430240.0,480040.0,534120.0,",
        "  592730.0,656090.0,724460.0,798080.0,877220.0,962140.0,1053100.0,",
        "  1150400.0,1254300.0,1365100.0,1483100.0,1608600.0,1741900.0,1883400.0,",
        "  2033400.0,2192300.0,2360400.0,2538200.0,2726100.0,2924600.0,3134100.0,",
        "  3355200.0,3588600.0,3834800.0,4094900.0,4369700.0,4660700.0;",

        "FluidProperties:Saturated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  Fluid,                   !- Fluid Phase",
        "  R410aSaturatedTemperatures,  !- Temperature Values Name",
        "  98535.0,102590.0,106650.0,110720.0,114790.0,118880.0,122970.0,",
        "  127070.0,131190.0,135320.0,139470.0,143630.0,147820.0,152020.0,",
        "  156240.0,160480.0,164750.0,169040.0,173370.0,177720.0,182100.0,",
        "  186520.0,190970.0,195470.0,200000.0,204580.0,209200.0,213880.0,",
        "  218610.0,223400.0,228250.0,233160.0,238150.0,243220.0,248380.0,",
        "  253630.0,258990.0,264470.0,270080.0,275850.0,281800.0,287970.0,",
        "  294410.0,301200.0,308480.0,316500.0,325780.0,338150.0;",

        "FluidProperties:Saturated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  FluidGas,                !- Fluid Phase",
        "  R410aSaturatedTemperatures,  !- Temperature Values Name",
        "  388130.0,389810.0,391480.0,393130.0,394760.0,396370.0,397960.0,",
        "  399530.0,401080.0,402600.0,404100.0,405570.0,407010.0,408420.0,",
        "  409800.0,411140.0,412450.0,413730.0,414960.0,416150.0,417300.0,",
        "  418400.0,419450.0,420450.0,421390.0,422270.0,423080.0,423820.0,",
        "  424480.0,425070.0,425560.0,425950.0,426240.0,426410.0,426460.0,",
        "  426350.0,426090.0,425640.0,424980.0,424080.0,422900.0,421370.0,",
        "  419410.0,416920.0,413700.0,409420.0,403430.0,393730.0;",

        "FluidProperties:Saturated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  Fluid,                   !- Fluid Phase",
        "  R410aSaturatedTemperatures,  !- Temperature Values Name",
        "  1412.7,1403.6,1394.6,1385.4,1376.2,1366.9,1357.6,",
        "  1348.2,1338.7,1329.1,1319.4,1309.7,1299.8,1289.8,",
        "  1279.7,1269.4,1259.1,1248.6,1237.9,1227.1,1216.0,",
        "  1204.8,1193.4,1181.8,1169.9,1157.8,1145.4,1132.8,",
        "  1119.7,1106.4,1092.7,1078.5,1063.9,1048.8,1033.1,",
        "  1016.7,999.71,981.87,963.08,943.19,921.98,899.16,",
        "  874.29,846.72,815.37,778.25,730.95,659.03;",

        "FluidProperties:Saturated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  FluidGas,                !- Fluid Phase",
        "  R410aSaturatedTemperatures,  !- Temperature Values Name",
        "  1.3845,1.6517,1.9588,2.31,2.7097,3.1627,3.6737,",
        "  4.2482,4.8916,5.6098,6.4088,7.2952,8.2758,9.3578,",
        "  10.549,11.857,13.292,14.861,16.576,18.447,20.485,",
        "  22.702,25.113,27.732,30.575,33.659,37.005,40.634,",
        "  44.571,48.844,53.483,58.525,64.012,69.991,76.52,",
        "  83.666,91.511,100.16,109.73,120.38,132.33,145.85,",
        "  161.35,179.4,200.95,227.66,263.01,317.59;",

        "FluidProperties:Saturated,",
        "  R410a,                   !- Fluid Name",
        "  SpecificHeat,            !- Fluid Property Type",
        "  Fluid,                   !- Fluid Phase",
        "  R410aSaturatedTemperatures,  !- Temperature Values Name",
        "  1349.9,1351.5,1353.4,1355.7,1358.4,1361.4,1364.8,",
        "  1368.6,1372.8,1377.4,1382.5,1388.1,1394.1,1400.7,",
        "  1407.8,1415.5,1423.8,1432.7,1442.4,1452.7,1463.9,",
        "  1475.9,1488.8,1502.7,1517.7,1534.0,1551.5,1570.6,",
        "  1591.4,1614.1,1639.0,1666.4,1696.8,1730.7,1768.9,",
        "  1812.3,1862.2,1920.4,1989.5,2073.2,2177.4,2311.6,",
        "  2492.4,2750.7,3153.4,3872.3,5519.0,12701.0;",

        "FluidProperties:Saturated,",
        "  R410a,                   !- Fluid Name",
        "  SpecificHeat,            !- Fluid Property Type",
        "  FluidGas,                !- Fluid Phase",
        "  R410aSaturatedTemperatures,  !- Temperature Values Name",
        "  723.87,735.19,746.93,759.1,771.67,784.65,798.02,",
        "  811.78,825.94,840.5,855.46,870.85,886.68,902.98,",
        "  919.79,937.15,955.11,973.72,993.07,1013.2,1034.3,",
        "  1056.4,1079.6,1104.2,1130.2,1158.0,1187.7,1219.6,",
        "  1254.1,1291.7,1332.9,1378.3,1428.7,1485.3,1549.4,",
        "  1622.8,1707.8,1807.8,1927.4,2073.5,2256.2,2492.2,",
        "  2809.4,3259.6,3950.4,5146.5,7718.5,17076.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  31238.0,                 !- Pressure {Pa}",
        "  388130.0,392450.0,396750.0,401050.0,405360.0,407530.0,409700.0,",
        "  411890.0,414080.0,416280.0,418490.0,420710.0,422940.0,425180.0,",
        "  427430.0,429690.0,431960.0,434250.0,436550.0,438850.0,441180.0,",
        "  443510.0,445860.0,448210.0,450580.0,452970.0,455360.0,457770.0,",
        "  460200.0,462630.0,465080.0,467540.0,470020.0,472510.0,475010.0,",
        "  477520.0,480050.0,482590.0,485150.0,487720.0,490300.0,492900.0,",
        "  495510.0,498130.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  31238.0,                 !- Pressure {Pa}",
        "  1.3845,1.3404,1.2997,1.2617,1.2262,1.2092,1.1928,",
        "  1.1768,1.1613,1.1462,1.1316,1.1173,1.1034,1.0898,",
        "  1.0766,1.0638,1.0512,1.039,1.0271,1.0154,1.004,",
        "  0.99285,0.98197,0.97133,0.96093,0.95075,0.94079,0.93104,",
        "  0.9215,0.91215,0.90299,0.89403,0.88524,0.87662,0.86817,",
        "  0.85989,0.85177,0.8438,0.83598,0.82831,0.82077,0.81338,",
        "  0.80612,0.79899;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  45248.0,                 !- Pressure {Pa}",
        "  0.0,391480.0,395930.0,400340.0,404740.0,406940.0,409150.0,",
        "  411360.0,413580.0,415800.0,418030.0,420270.0,422520.0,424780.0,",
        "  427050.0,429330.0,431610.0,433910.0,436220.0,438540.0,440880.0,",
        "  443220.0,445580.0,447940.0,450320.0,452720.0,455120.0,457540.0,",
        "  459970.0,462410.0,464860.0,467330.0,469810.0,472310.0,474810.0,",
        "  477330.0,479870.0,482410.0,484970.0,487550.0,490130.0,492730.0,",
        "  495350.0,497970.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  45248.0,                 !- Pressure {Pa}",
        "  0.0,1.9588,1.8968,1.8395,1.7863,1.761,1.7365,",
        "  1.7128,1.6898,1.6674,1.6457,1.6246,1.6041,1.5842,",
        "  1.5647,1.5458,1.5273,1.5093,1.4918,1.4747,1.458,",
        "  1.4416,1.4257,1.4101,1.3949,1.38,1.3654,1.3512,",
        "  1.3372,1.3236,1.3102,1.2971,1.2843,1.2717,1.2594,",
        "  1.2473,1.2355,1.2239,1.2125,1.2013,1.1903,1.1796,",
        "  1.169,1.1586;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  63963.0,                 !- Pressure {Pa}",
        "  0.0,0.0,394760.0,399350.0,403880.0,406140.0,408390.0,",
        "  410640.0,412900.0,415160.0,417420.0,419690.0,421960.0,424250.0,",
        "  426540.0,428840.0,431140.0,433460.0,435790.0,438130.0,440470.0,",
        "  442830.0,445200.0,447580.0,449970.0,452380.0,454790.0,457220.0,",
        "  459660.0,462110.0,464570.0,467050.0,469540.0,472040.0,474550.0,",
        "  477080.0,479620.0,482170.0,484740.0,487320.0,489910.0,492520.0,",
        "  495130.0,497770.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  63963.0,                 !- Pressure {Pa}",
        "  0.0,0.0,2.7097,2.624,2.5451,2.5078,2.4718,",
        "  2.437,2.4034,2.3708,2.3393,2.3086,2.2789,2.25,",
        "  2.2219,2.1945,2.1679,2.142,2.1167,2.0921,2.0681,",
        "  2.0446,2.0217,1.9994,1.9776,1.9562,1.9354,1.915,",
        "  1.895,1.8755,1.8564,1.8377,1.8194,1.8014,1.7839,",
        "  1.7666,1.7497,1.7332,1.7169,1.701,1.6854,1.67,",
        "  1.655,1.6402;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  88445.0,                 !- Pressure {Pa}",
        "  0.0,0.0,0.0,397960.0,402700.0,405030.0,407360.0,",
        "  409670.0,411980.0,414290.0,416600.0,418910.0,421220.0,423540.0,",
        "  425860.0,428190.0,430520.0,432860.0,435210.0,437570.0,439940.0,",
        "  442320.0,444700.0,447100.0,449510.0,451930.0,454360.0,456800.0,",
        "  459250.0,461710.0,464190.0,466680.0,469180.0,471690.0,474210.0,",
        "  476750.0,479300.0,481860.0,484430.0,487020.0,489620.0,492230.0,",
        "  494860.0,497490.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  88445.0,                 !- Pressure {Pa}",
        "  0.0,0.0,0.0,3.6737,3.557,3.5024,3.45,",
        "  3.3995,3.3509,3.3039,3.2585,3.2146,3.172,3.1308,",
        "  3.0907,3.0518,3.014,2.9772,2.9414,2.9065,2.8726,",
        "  2.8395,2.8072,2.7757,2.7449,2.7149,2.6856,2.6569,",
        "  2.6289,2.6015,2.5747,2.5485,2.5228,2.4977,2.4731,",
        "  2.449,2.4254,2.4022,2.3795,2.3573,2.3354,2.314,",
        "  2.293,2.2724;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  119880.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,401080.0,403540.0,405970.0,",
        "  408380.0,410770.0,413150.0,415520.0,417880.0,420250.0,422610.0,",
        "  424970.0,427340.0,429710.0,432090.0,434470.0,436850.0,439250.0,",
        "  441650.0,444060.0,446480.0,448910.0,451350.0,453800.0,456260.0,",
        "  458730.0,461210.0,463700.0,466200.0,468710.0,471240.0,473770.0,",
        "  476320.0,478880.0,481450.0,484040.0,486630.0,489240.0,491860.0,",
        "  494500.0,497150.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  119880.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,4.8918,4.8116,4.7352,",
        "  4.6621,4.592,4.5247,4.4599,4.3974,4.337,4.2787,",
        "  4.2221,4.1674,4.1143,4.0627,4.0126,3.9639,3.9165,",
        "  3.8704,3.8255,3.7817,3.739,3.6974,3.6567,3.6171,",
        "  3.5783,3.5405,3.5035,3.4673,3.4319,3.3973,3.3634,",
        "  3.3302,3.2977,3.2659,3.2347,3.2041,3.1742,3.1448,",
        "  3.116,3.0877;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  138600.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,402600.0,405100.0,",
        "  407570.0,410020.0,412440.0,414850.0,417260.0,419650.0,422040.0,",
        "  424440.0,426830.0,429220.0,431620.0,434020.0,436420.0,438830.0,",
        "  441250.0,443680.0,446110.0,448550.0,451000.0,453460.0,455930.0,",
        "  458410.0,460900.0,463400.0,465910.0,468430.0,470970.0,473510.0,",
        "  476060.0,478630.0,481210.0,483800.0,486400.0,489020.0,491650.0,",
        "  494280.0,496940.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  138600.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,5.6098,5.5173,",
        "  5.4293,5.3451,5.2645,5.1871,5.1127,5.0409,4.9717,",
        "  4.9047,4.8399,4.7772,4.7163,4.6573,4.5999,4.5442,",
        "  4.49,4.4372,4.3859,4.3358,4.287,4.2394,4.193,",
        "  4.1476,4.1033,4.0601,4.0178,3.9765,3.936,3.8965,",
        "  3.8578,3.8199,3.7828,3.7464,3.7108,3.6759,3.6417,",
        "  3.6081,3.5752;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  159550.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,404100.0,",
        "  406640.0,409150.0,411630.0,414090.0,416540.0,418980.0,421400.0,",
        "  423830.0,426250.0,428670.0,431090.0,433510.0,435930.0,438360.0,",
        "  440800.0,443240.0,445690.0,448150.0,450610.0,453090.0,455570.0,",
        "  458060.0,460560.0,463070.0,465590.0,468120.0,470660.0,473210.0,",
        "  475780.0,478350.0,480940.0,483540.0,486150.0,488770.0,491400.0,",
        "  494040.0,496700.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  159550.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,6.4087,",
        "  6.3023,6.201,6.1045,6.012,5.9233,5.838,5.7559,",
        "  5.6767,5.6001,5.5261,5.4544,5.385,5.3176,5.2521,",
        "  5.1885,5.1267,5.0666,5.008,4.9509,4.8953,4.8411,",
        "  4.7882,4.7366,4.6862,4.6369,4.5888,4.5417,4.4957,",
        "  4.4507,4.4066,4.3635,4.3212,4.2799,4.2393,4.1996,",
        "  4.1607,4.1225;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  182920.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  405570.0,408160.0,410710.0,413230.0,415730.0,418210.0,420680.0,",
        "  423130.0,425590.0,428040.0,430490.0,432930.0,435380.0,437840.0,",
        "  440290.0,442750.0,445220.0,447690.0,450170.0,452660.0,455160.0,",
        "  457660.0,460170.0,462700.0,465230.0,467770.0,470320.0,472880.0,",
        "  475460.0,478040.0,480630.0,483240.0,485860.0,488480.0,491120.0,",
        "  493780.0,496440.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  182920.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  7.2953,7.1732,7.0571,6.9465,6.8408,6.7394,6.642,",
        "  6.5482,6.4578,6.3706,6.2862,6.2046,6.1255,6.0488,",
        "  5.9743,5.902,5.8317,5.7633,5.6968,5.632,5.5688,",
        "  5.5072,5.4472,5.3885,5.3313,5.2754,5.2208,5.1674,",
        "  5.1152,5.0641,5.0141,4.9652,4.9173,4.8703,4.8244,",
        "  4.7793,4.7352;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  208880.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,407010.0,409640.0,412240.0,414800.0,417330.0,419850.0,",
        "  422350.0,424850.0,427330.0,429810.0,432290.0,434770.0,437240.0,",
        "  439720.0,442210.0,444690.0,447190.0,449680.0,452190.0,454700.0,",
        "  457220.0,459740.0,462280.0,464820.0,467380.0,469940.0,472510.0,",
        "  475100.0,477690.0,480290.0,482910.0,485530.0,488170.0,490820.0,",
        "  493480.0,496150.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  208880.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,8.2759,8.1361,8.0034,7.877,7.7563,7.6407,",
        "  7.5297,7.423,7.3201,7.2209,7.1251,7.0323,6.9425,",
        "  6.8555,6.771,6.689,6.6093,6.5318,6.4564,6.383,",
        "  6.3115,6.2417,6.1738,6.1074,6.0426,5.9794,5.9176,",
        "  5.8572,5.7981,5.7404,5.6839,5.6286,5.5744,5.5214,",
        "  5.4694,5.4185;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  237620.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,408420.0,411100.0,413740.0,416340.0,418920.0,",
        "  421470.0,424010.0,426540.0,429050.0,431570.0,434070.0,436580.0,",
        "  439090.0,441590.0,444100.0,446620.0,449140.0,451660.0,454190.0,",
        "  456720.0,459270.0,461820.0,464370.0,466940.0,469520.0,472100.0,",
        "  474700.0,477300.0,479920.0,482540.0,485170.0,487820.0,490480.0,",
        "  493150.0,495820.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  237620.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,9.3578,9.1979,9.0465,8.9024,8.765,",
        "  8.6335,8.5073,8.3861,8.2694,8.1569,8.0482,7.9431,",
        "  7.8414,7.7429,7.6473,7.5546,7.4645,7.3769,7.2917,",
        "  7.2088,7.128,7.0493,6.9726,6.8977,6.8246,6.7533,",
        "  6.6836,6.6155,6.5489,6.4838,6.42,6.3577,6.2967,",
        "  6.2369,6.1783;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  269350.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,409800.0,412530.0,415210.0,417860.0,",
        "  420470.0,423070.0,425640.0,428200.0,430750.0,433300.0,435840.0,",
        "  438370.0,440910.0,443450.0,445990.0,448530.0,451070.0,453620.0,",
        "  456170.0,458730.0,461300.0,463880.0,466460.0,469050.0,471650.0,",
        "  474250.0,476870.0,479500.0,482130.0,484780.0,487430.0,490100.0,",
        "  492780.0,495460.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  269350.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,10.549,10.367,10.194,10.03,",
        "  9.8741,9.7248,9.5817,9.4443,9.3122,9.1848,9.0619,",
        "  8.9431,8.8282,8.717,8.6091,8.5045,8.4029,8.3042,",
        "  8.2081,8.1147,8.0237,7.9351,7.8487,7.7644,7.6822,",
        "  7.6019,7.5235,7.4469,7.3721,7.2989,7.2273,7.1572,",
        "  7.0886,7.0214;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  304260.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,411140.0,413920.0,416650.0,",
        "  419340.0,422000.0,424630.0,427250.0,429840.0,432430.0,435010.0,",
        "  437580.0,440150.0,442720.0,445280.0,447850.0,450420.0,452990.0,",
        "  455560.0,458140.0,460730.0,463320.0,465920.0,468530.0,471140.0,",
        "  473760.0,476390.0,479030.0,481680.0,484340.0,487010.0,489680.0,",
        "  492370.0,495070.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  304260.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,11.857,11.65,11.453,",
        "  11.267,11.09,10.921,10.759,10.604,10.454,10.31,",
        "  10.172,10.038,9.9083,9.783,9.6615,9.5438,9.4294,",
        "  9.3184,9.2104,9.1054,9.0032,8.9037,8.8067,8.7121,",
        "  8.6198,8.5297,8.4418,8.3559,8.2719,8.1898,8.1095,",
        "  8.031,7.9541;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  342570.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,412450.0,415290.0,",
        "  418070.0,420800.0,423500.0,426170.0,428830.0,431460.0,434080.0,",
        "  436700.0,439300.0,441900.0,444500.0,447090.0,449690.0,452290.0,",
        "  454890.0,457490.0,460100.0,462710.0,465330.0,467950.0,470580.0,",
        "  473220.0,475870.0,478520.0,481180.0,483850.0,486530.0,489220.0,",
        "  491920.0,494630.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  342570.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,13.292,13.056,",
        "  12.833,12.622,12.421,12.23,12.047,11.871,11.703,",
        "  11.541,11.385,11.234,11.088,10.947,10.811,10.678,",
        "  10.55,10.425,10.304,10.187,10.072,9.9605,9.8518,",
        "  9.7459,9.6426,9.5417,9.4433,9.3472,9.2533,9.1615,",
        "  9.0717,8.9839;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  384490.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,413730.0,",
        "  416610.0,419440.0,422220.0,424970.0,427680.0,430380.0,433050.0,",
        "  435710.0,438360.0,441000.0,443630.0,446260.0,448890.0,451510.0,",
        "  454140.0,456770.0,459400.0,462030.0,464670.0,467320.0,469970.0,",
        "  472620.0,475290.0,477960.0,480630.0,483320.0,486010.0,488720.0,",
        "  491430.0,494150.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  384490.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,14.861,",
        "  14.593,14.341,14.102,13.875,13.659,13.452,13.255,",
        "  13.065,12.882,12.707,12.537,12.374,12.216,12.063,",
        "  11.914,11.771,11.631,11.495,11.364,11.236,11.111,",
        "  10.989,10.871,10.755,10.643,10.533,10.426,10.321,",
        "  10.218,10.118;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  430240.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  414960.0,417900.0,420780.0,423610.0,426410.0,429160.0,431900.0,",
        "  434610.0,437310.0,439990.0,442670.0,445330.0,448000.0,450660.0,",
        "  453310.0,455970.0,458630.0,461290.0,463950.0,466620.0,469290.0,",
        "  471970.0,474650.0,477340.0,480030.0,482730.0,485440.0,488160.0,",
        "  490890.0,493620.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  430240.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  16.576,16.272,15.986,15.716,15.46,15.216,14.983,",
        "  14.761,14.547,14.343,14.145,13.955,13.772,13.595,",
        "  13.424,13.258,13.097,12.941,12.789,12.642,12.499,",
        "  12.36,12.224,12.092,11.964,11.838,11.716,11.596,",
        "  11.48,11.365;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  480040.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,416150.0,419150.0,422090.0,424970.0,427810.0,430610.0,",
        "  433390.0,436140.0,438880.0,441600.0,444310.0,447010.0,449710.0,",
        "  452400.0,455090.0,457780.0,460470.0,463160.0,465850.0,468550.0,",
        "  471240.0,473950.0,476660.0,479370.0,482090.0,484820.0,487550.0,",
        "  490290.0,493040.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  480040.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,18.447,18.102,17.778,17.473,17.184,16.91,",
        "  16.648,16.398,16.158,15.928,15.707,15.495,15.289,",
        "  15.091,14.9,14.715,14.535,14.361,14.192,14.028,",
        "  13.869,13.714,13.563,13.416,13.273,13.133,12.997,",
        "  12.864,12.734;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  534120.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,417300.0,420360.0,423350.0,426290.0,429170.0,",
        "  432020.0,434850.0,437640.0,440420.0,443180.0,445930.0,448670.0,",
        "  451400.0,454130.0,456850.0,459570.0,462290.0,465010.0,467730.0,",
        "  470450.0,473180.0,475910.0,478650.0,481390.0,484130.0,486890.0,",
        "  489650.0,492410.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  534120.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,20.485,20.094,19.728,19.383,19.058,",
        "  18.749,18.455,18.174,17.905,17.648,17.4,17.162,",
        "  16.933,16.712,16.498,16.292,16.092,15.898,15.71,",
        "  15.527,15.35,15.178,15.01,14.847,14.688,14.533,",
        "  14.382,14.234;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  592730.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,418400.0,421530.0,424580.0,427560.0,",
        "  430500.0,433400.0,436270.0,439110.0,441930.0,444730.0,447520.0,",
        "  450290.0,453060.0,455820.0,458580.0,461330.0,464080.0,466830.0,",
        "  469590.0,472340.0,475090.0,477850.0,480620.0,483380.0,486160.0,",
        "  488940.0,491720.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  592730.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,22.703,22.26,21.846,21.458,",
        "  21.091,20.744,20.413,20.098,19.798,19.509,19.233,",
        "  18.967,18.711,18.465,18.227,17.996,17.774,17.558,",
        "  17.349,17.146,16.95,16.758,16.572,16.391,16.215,",
        "  16.043,15.876;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  656090.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,419450.0,422640.0,425750.0,",
        "  428800.0,431790.0,434740.0,437650.0,440540.0,443400.0,446250.0,",
        "  449070.0,451890.0,454690.0,457490.0,460280.0,463070.0,465850.0,",
        "  468640.0,471420.0,474200.0,476990.0,479780.0,482570.0,485360.0,",
        "  488160.0,490970.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  656090.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,25.113,24.612,24.145,",
        "  23.707,23.294,22.904,22.533,22.18,21.844,21.522,",
        "  21.213,20.916,20.631,20.356,20.091,19.836,19.588,",
        "  19.349,19.117,18.892,18.673,18.461,18.255,18.055,",
        "  17.86,17.67;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  724460.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,420450.0,423710.0,",
        "  426880.0,429990.0,433040.0,436040.0,439000.0,441940.0,444840.0,",
        "  447730.0,450600.0,453450.0,456300.0,459130.0,461960.0,464780.0,",
        "  467600.0,470410.0,473230.0,476040.0,478860.0,481680.0,484500.0,",
        "  487320.0,490150.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  724460.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,27.732,27.164,",
        "  26.636,26.143,25.678,25.24,24.825,24.43,24.053,",
        "  23.694,23.349,23.019,22.701,22.396,22.101,21.817,",
        "  21.542,21.277,21.019,20.77,20.529,20.294,20.066,",
        "  19.844,19.629;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  798080.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,421390.0,",
        "  424720.0,427970.0,431130.0,434240.0,437300.0,440310.0,443290.0,",
        "  446250.0,449180.0,452090.0,454990.0,457870.0,460740.0,463610.0,",
        "  466460.0,469320.0,472170.0,475020.0,477860.0,480710.0,483560.0,",
        "  486410.0,489260.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  798080.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,30.574,",
        "  29.931,29.335,28.779,28.257,27.765,27.299,26.857,",
        "  26.436,26.035,25.651,25.283,24.93,24.59,24.263,",
        "  23.948,23.644,23.349,23.065,22.789,22.521,22.262,",
        "  22.01,21.766;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  877220.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  422270.0,425680.0,428990.0,432230.0,435400.0,438510.0,441580.0,",
        "  444610.0,447610.0,450590.0,453550.0,456490.0,459410.0,462320.0,",
        "  465230.0,468120.0,471010.0,473890.0,476780.0,479660.0,482540.0,",
        "  485420.0,488300.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  877220.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  33.659,32.93,32.256,31.629,31.042,30.49,29.968,",
        "  29.474,29.004,28.557,28.129,27.72,27.327,26.95,",
        "  26.587,26.238,25.9,25.575,25.26,24.955,24.66,",
        "  24.374,24.096;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  962140.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,423080.0,426580.0,429970.0,433270.0,436500.0,439680.0,",
        "  442800.0,445890.0,448940.0,451970.0,454970.0,457950.0,460920.0,",
        "  463870.0,466810.0,469750.0,472670.0,475590.0,478510.0,481420.0,",
        "  484340.0,487250.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  962140.0,                !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,37.005,36.178,35.416,34.709,34.049,33.429,",
        "  32.845,32.292,31.768,31.269,30.793,30.338,29.902,",
        "  29.484,29.082,28.694,28.321,27.961,27.613,27.277,",
        "  26.951,26.636;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1053100.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,423820.0,427410.0,430880.0,434260.0,437560.0,",
        "  440790.0,443980.0,447120.0,450230.0,453300.0,456350.0,459380.0,",
        "  462390.0,465390.0,468370.0,471340.0,474300.0,477260.0,480210.0,",
        "  483160.0,486110.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1053100.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,40.634,39.695,38.832,38.035,37.292,",
        "  36.597,35.943,35.325,34.74,34.184,33.654,33.149,",
        "  32.665,32.201,31.755,31.327,30.915,30.517,30.133,",
        "  29.762,29.403;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1150400.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,424480.0,428170.0,431730.0,435180.0,",
        "  438550.0,441860.0,445110.0,448310.0,451470.0,454590.0,457690.0,",
        "  460770.0,463830.0,466860.0,469890.0,472900.0,475910.0,478900.0,",
        "  481890.0,484880.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1150400.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,44.572,43.503,42.527,41.626,",
        "  40.79,40.01,39.277,38.587,37.934,37.314,36.725,",
        "  36.164,35.627,35.113,34.62,34.146,33.691,33.252,",
        "  32.828,32.419;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1254300.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,425070.0,428860.0,432510.0,",
        "  436050.0,439490.0,442870.0,446180.0,449440.0,452660.0,455840.0,",
        "  458990.0,462120.0,465220.0,468310.0,471380.0,474430.0,477480.0,",
        "  480510.0,483540.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1254300.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,48.844,47.627,46.519,",
        "  45.502,44.56,43.684,42.863,42.091,41.363,40.673,",
        "  40.018,39.394,38.799,38.23,37.685,37.161,36.658,",
        "  36.174,35.708;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1365100.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,425560.0,429470.0,",
        "  433220.0,436840.0,440370.0,443820.0,447200.0,450530.0,453810.0,",
        "  457050.0,460250.0,463430.0,466580.0,469710.0,472830.0,475920.0,",
        "  479010.0,482090.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1365100.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,53.484,52.094,",
        "  50.835,49.684,48.623,47.638,46.718,45.855,45.042,",
        "  44.274,43.546,42.854,42.194,41.564,40.961,40.383,",
        "  39.828,39.294;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1483100.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,425950.0,",
        "  429990.0,433850.0,437570.0,441190.0,444710.0,448170.0,451560.0,",
        "  454900.0,458200.0,461460.0,464690.0,467900.0,471080.0,474240.0,",
        "  477380.0,480510.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1483100.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,58.526,",
        "  56.935,55.502,54.199,53.001,51.893,50.861,49.896,",
        "  48.989,48.133,47.324,46.555,45.824,45.127,44.461,",
        "  43.823,43.211;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1608600.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  426240.0,430410.0,434390.0,438220.0,441930.0,445540.0,449070.0,",
        "  452540.0,455950.0,459310.0,462630.0,465910.0,469170.0,472400.0,",
        "  475610.0,478800.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1608600.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  64.013,62.185,60.551,59.071,57.718,56.47,55.313,",
        "  54.232,53.22,52.267,51.367,50.514,49.704,48.933,",
        "  48.196,47.492;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1741900.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,426420.0,430740.0,434850.0,438790.0,442600.0,446300.0,",
        "  449910.0,453450.0,456930.0,460360.0,463740.0,467090.0,470400.0,",
        "  473680.0,476940.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1741900.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,69.99,67.883,66.014,64.331,62.8,61.394,",
        "  60.094,58.884,57.753,56.691,55.691,54.744,53.847,",
        "  52.994,52.181;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1883400.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,426460.0,430960.0,435210.0,439270.0,443190.0,",
        "  446990.0,450690.0,454310.0,457860.0,461360.0,464810.0,468210.0,",
        "  471580.0,474920.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  1883400.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,76.519,74.08,71.935,70.017,68.281,",
        "  66.694,65.232,63.876,62.613,61.429,60.316,59.266,",
        "  58.272,57.328;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  2033400.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,426350.0,431050.0,435460.0,439660.0,",
        "  443690.0,447600.0,451390.0,455100.0,458730.0,462300.0,465820.0,",
        "  469290.0,472720.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  2033400.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,83.665,80.827,78.356,76.164,",
        "  74.192,72.398,70.753,69.232,67.819,66.499,65.261,",
        "  64.095,62.993;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  2192300.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,426090.0,431010.0,435600.0,",
        "  439950.0,444110.0,448120.0,452020.0,455820.0,459530.0,463180.0,",
        "  466770.0,470300.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  2192300.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,91.511,88.19,85.332,",
        "  82.819,80.573,78.542,76.687,74.98,73.398,71.925,",
        "  70.547,69.252;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  2360400.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,425640.0,430820.0,",
        "  435610.0,440130.0,444430.0,448560.0,452570.0,456460.0,460270.0,",
        "  464000.0,467660.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  2360400.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,100.15,96.239,",
        "  92.918,90.027,87.463,85.159,83.065,81.145,79.374,",
        "  77.729,76.195;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  2538200.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,424980.0,",
        "  430470.0,435490.0,440190.0,444640.0,448910.0,453030.0,457030.0,",
        "  460930.0,464750.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  2538200.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,109.72,",
        "  105.07,101.19,97.851,94.915,92.295,89.926,87.766,",
        "  85.78,83.942;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  2726100.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  424080.0,429930.0,435220.0,440120.0,444750.0,449160.0,453410.0,",
        "  457520.0,461520.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  2726100.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  120.38,114.79,110.23,106.35,102.98,99.995,97.312,",
        "  94.877,92.647;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  2924600.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,422900.0,429180.0,434780.0,439920.0,444730.0,449310.0,",
        "  453690.0,457920.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  2924600.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,132.33,125.54,120.13,115.62,111.73,108.31,",
        "  105.27,102.52;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  3134100.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,421370.0,428200.0,434160.0,439570.0,444590.0,",
        "  449340.0,453870.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  3134100.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,145.85,137.48,131.02,125.72,121.22,",
        "  117.31,113.84;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  3355200.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,419410.0,426950.0,433340.0,439050.0,",
        "  444320.0,449260.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  3355200.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,161.35,150.82,143.02,136.77,",
        "  131.54,127.04;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  3588600.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,416920.0,425390.0,432290.0,",
        "  438360.0,443890.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  3588600.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,179.41,165.85,156.32,",
        "  148.9,142.79;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  3834800.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,413700.0,423460.0,",
        "  431000.0,437480.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  3834800.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,200.95,182.89,",
        "  171.11,162.23;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  4094900.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,409420.0,",
        "  421090.0,429430.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  4094900.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,227.66,",
        "  202.46,187.65;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  4369700.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  403430.0,418230.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  4369700.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  263.02,225.13;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Enthalpy,                !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  4660700.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,393730.0;",

        "FluidProperties:Superheated,",
        "  R410a,                   !- Fluid Name",
        "  Density,                 !- Fluid Property Type",
        "  R410aSuperHeatTemperatures,  !- Temperature Values Name",
        "  4660700.0,               !- Pressure {Pa}",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,0.0,0.0,0.0,0.0,0.0,0.0,",
        "  0.0,317.58;",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    state->dataGlobal->TimeStepsInHour = 1;
    state->dataGlobal->TimeStep = 1;
    state->dataGlobal->MinutesInTimeStep = 60;
    state->init_state(*state);

    GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    GetZoneEquipmentData(*state);
    GetZoneAirLoopEquipment(*state);

    GetVRFInput(*state);
    state->dataHVACVarRefFlow->GetVRFInputFlag = false;

    // get input test for terminal air single duct mixer on supply side of VRF terminal unit
    ASSERT_EQ(1, state->dataSingleDuct->NumATMixers);
    EXPECT_EQ("SPACE1-1 DOAS AIR TERMINAL", state->dataSingleDuct->SysATMixer(1).Name);                 // single duct air terminal mixer name
    EXPECT_EQ((int)HVAC::MixerType::SupplySide, (int)state->dataSingleDuct->SysATMixer(1).type);        // air terminal mixer connection type
    EXPECT_EQ("AIRTERMINAL:SINGLEDUCT:MIXER", state->dataDefineEquipment->AirDistUnit(1).EquipType(1)); // Air distribution unit equipment type
    EXPECT_EQ("TU1", state->dataHVACVarRefFlow->VRFTU(1).Name);                                         // zoneHVAC equipment name

    state->dataGlobal->BeginEnvrnFlag = false;

    // set input variables
    state->dataEnvrn->OutBaroPress = 101325.0;
    state->dataEnvrn->OutDryBulbTemp = 35.0;
    state->dataEnvrn->OutHumRat = 0.0098;
    state->dataEnvrn->OutEnthalpy = Psychrometrics::PsyHFnTdbW(state->dataEnvrn->OutDryBulbTemp, state->dataEnvrn->OutHumRat);
    state->dataEnvrn->StdRhoAir = 1.20;
    HVACInletMassFlowRate = 0.50;
    PrimaryAirMassFlowRate = 0.1;

    // set zoneNode air condition
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp = 24.0;
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat = 0.0075;
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Enthalpy =
        Psychrometrics::PsyHFnTdbW(state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp,
                                   state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat);

    state->dataHVACVarRefFlow->CoolingLoad.allocate(1);
    state->dataHVACVarRefFlow->HeatingLoad.allocate(1);
    state->dataHVACVarRefFlow->HeatingLoad(1) = false;
    state->dataHVACVarRefFlow->CoolingLoad(1) = true;
    state->dataHVACVarRefFlow->CompOnMassFlow = HVACInletMassFlowRate;    // supply air mass flow rate
    state->dataHVACVarRefFlow->OACompOnMassFlow = PrimaryAirMassFlowRate; // OA mass flow rate
    state->dataHVACVarRefFlow->CompOnFlowRatio = 1.0;                     // compressor is on
    state->dataHVACGlobal->TurnFansOff = false;
    state->dataHVACGlobal->TurnFansOn = true;

    VRFNum = 1;
    VRFTUNum = 1;
    state->dataHVACVarRefFlow->VRFTU(VRFTUNum).fanOp = HVAC::FanOp::Cycling;
    state->dataHVACVarRefFlow->VRFTU(VRFTUNum).isInZone = true;
    state->dataHVACVarRefFlow->VRFTU(VRFTUNum).ZoneAirNode = state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode;
    // initialize mass flow rates
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).VRFTUInletNodeNum).MassFlowRate = HVACInletMassFlowRate;
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).ATMixerPriNode).MassFlowRate = PrimaryAirMassFlowRate;
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).ATMixerPriNode).MassFlowRateMaxAvail = PrimaryAirMassFlowRate;

    // set fan parameters
    state->dataFans->fans(1)->maxAirMassFlowRate = HVACInletMassFlowRate;
    state->dataFans->fans(1)->inletAirMassFlowRate = HVACInletMassFlowRate;
    state->dataFans->fans(1)->rhoAirStdInit = state->dataEnvrn->StdRhoAir;
    state->dataLoopNodes->Node(state->dataFans->fans(1)->inletNodeNum).MassFlowRateMaxAvail = HVACInletMassFlowRate;
    state->dataLoopNodes->Node(state->dataFans->fans(1)->outletNodeNum).MassFlowRateMax = HVACInletMassFlowRate;

    // set DX coil rated performance parameters
    state->dataDXCoils->DXCoil(1).RatedCBF(1) = 0.05;
    state->dataDXCoils->DXCoil(1).RatedAirMassFlowRate(1) = HVACInletMassFlowRate;

    // primary air condition set at outdoor air condition
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).ATMixerPriNode).Temp = state->dataEnvrn->OutDryBulbTemp;
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).ATMixerPriNode).HumRat = state->dataEnvrn->OutHumRat;
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).ATMixerPriNode).Enthalpy = state->dataEnvrn->OutEnthalpy;

    // set VRF terminal unit inlet condition to zone air node
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).VRFTUInletNodeNum).Temp =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp;
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).VRFTUInletNodeNum).HumRat =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat;
    state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).VRFTUInletNodeNum).Enthalpy =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Enthalpy;

    state->dataHVACVarRefFlow->VRFTU(1).ZoneNum = 1;
    state->dataSize->SysSizingRunDone = true;
    state->dataSize->ZoneSizingRunDone = true;
    state->dataGlobal->SysSizingCalc = true;

    state->dataZoneEnergyDemand->ZoneSysEnergyDemand.allocate(1);
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputReqToHeatSP = 0.0;
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputReqToCoolSP = -4000.0;
    QZnReq = state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputReqToCoolSP;

    state->dataHVACVarRefFlow->VRFTU(VRFTUNum).availSched->currentVal = 1.0;    // unit is always available
    state->dataHVACVarRefFlow->VRFTU(VRFTUNum).fanAvailSched->currentVal = 1.0; // fan is always available
    state->dataHVACVarRefFlow->VRF(1).VRFCondCyclingRatio = 1.0;
    state->dataFans->fans(1)->set_size(*state); // add fan sizing

    // set secondary air mass flow rate to zero
    state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).MassFlowRate = 0.0;
    // Simulate zoneHVAC equipment (VRF terminal unit)
    SimVRF(*state, VRFTUNum, FirstHVACIteration, OnOffAirFlowRatio, QUnitOutVRFTU, LatOutputProvided, QZnReq);

    // check the terminal air mixer secondary air mass flow rate, requires updating the secondary flow
    SecondaryAirMassFlowRate = state->dataLoopNodes->Node(state->dataHVACVarRefFlow->VRFTU(VRFTUNum).VRFTUInletNodeNum).MassFlowRate;
    ASSERT_EQ(SecondaryAirMassFlowRate, state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).MassFlowRate);
    // check the terminal air mixer outlet flow rate must be equal to the mass flow rate of VRFTU + the primary air
    ATMixerOutletMassFlowRate = SecondaryAirMassFlowRate + PrimaryAirMassFlowRate;
    ASSERT_EQ(ATMixerOutletMassFlowRate, state->dataSingleDuct->SysATMixer(1).MixedAirMassFlowRate);
    //    changed the threshold from 2 to 3 as the fan was changed from hard-sized to autosize, a little more error is expected
    ASSERT_NEAR(QZnReq, QUnitOutVRFTU, 3.0);
}

TEST_F(EnergyPlusFixture, AirTerminalSingleDuctMixer_SimUnitVent_ATMInletSide)
{

    bool ErrorsFound(false);
    bool FirstHVACIteration(false);
    Real64 HVACInletMassFlowRate(0.0);
    Real64 PrimaryAirMassFlowRate(0.0);
    Real64 SecondaryAirMassFlowRate(0.0);
    Real64 LatOutputProvided(0.0);
    Real64 QUnitOut(0.0);
    state->dataUnitVentilators->QZnReq = 0.0;
    int ZoneNum(1);
    int UnitVentNum(1);

    std::string const idf_objects = delimited_string({
        "AirTerminal:SingleDuct:Mixer,",
        "  SPACE1-1 DOAS Air Terminal,  !- Name",
        "  ZoneHVAC:UnitVentilator,  !- ZoneHVAC Unit Object Type",
        "  SPACE1-1 Unit Vent,      !- ZoneHVAC Unit Object Name",
        "  SPACE1-1 Unit Vent Inlet,  !- Mixer Outlet Node Name",
        "  SPACE1-1 Mixer Primary Inlet,  !- Mixer Primary Air Inlet Node Name",
        "  SPACE1-1 Mixer Secondary Inlet,  !- Mixer Secondary Air Inlet Node Name",
        "  InletSide;               !- Mixer Connection Type",

        "ZoneHVAC:AirDistributionUnit,",
        "  SPACE1-1 DOAS ATU,       !- Name",
        "  SPACE1-1 Unit Vent Inlet,  !- Air Distribution Unit Outlet Node Name",
        "  AirTerminal:SingleDuct:Mixer,  !- Air Terminal Object Type",
        "  SPACE1-1 DOAS Air Terminal;  !- Air Terminal Name",

        "Schedule:Compact,",
        "  FanAvailSched,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,1.0;        !- Field 3",

        "ZoneHVAC:EquipmentList,",
        "  SPACE1-1 Equipment,      !- Name",
        "  SequentialLoad,          !- Load Distribution Scheme",
        "  ZoneHVAC:AirDistributionUnit,  !- Zone Equipment 1 Object Type",
        "  SPACE1-1 DOAS ATU,       !- Zone Equipment 1 Name",
        "  1,                       !- Zone Equipment 1 Cooling Sequence",
        "  1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 1 Sequential Cooling Fraction Schedule Name",
        "  ,                        !- Zone Equipment 1 Sequential Heating Fraction Schedule Name",
        "  ZoneHVAC:UnitVentilator,  !- Zone Equipment 2 Object Type",
        "  SPACE1-1 Unit Vent,      !- Zone Equipment 2 Name",
        "  2,                       !- Zone Equipment 2 Cooling Sequence",
        "  2,                       !- Zone Equipment 2 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 2 Sequential Cooling Fraction Schedule Name",
        "  ;                        !- Zone Equipment 2 Sequential Heating Fraction Schedule Name",

        "ZoneHVAC:UnitVentilator,",
        "  SPACE1-1 Unit Vent,      !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  0.5,                     !- Maximum Supply Air Flow Rate {m3/s}",
        "  VariablePercent,         !- Outdoor Air Control Type",
        "  0.2,                     !- Minimum Outdoor Air Flow Rate {m3/s}",
        "  U2MinOASched,            !- Minimum Outdoor Air Schedule Name",
        "  0.5,                     !- Maximum Outdoor Air Flow Rate {m3/s}",
        "  UnitVentMaxOA,           !- Maximum Outdoor Air Fraction or Temperature Schedule Name",
        "  SPACE1-1 Unit Vent Inlet,  !- Air Inlet Node Name",
        "  SPACE1-1 Supply Inlet,   !- Air Outlet Node Name",
        "  ,                        !- Outdoor Air Node Name",
        "  ,                        !- Exhaust Air Node Name",
        "  ,                        !- Mixed Air Node Name",
        "  Fan:ConstantVolume,      !- Supply Air Fan Object Type",
        "  Zone1UnitVentFan,        !- Supply Air Fan Name",
        "  Heating,                 !- Coil Option",
        "  ,                        !- Supply Air Fan Operating Mode Schedule Name",
        "  Coil:Heating:Fuel,       !- Heating Coil Object Type",
        "  Zone1UnitVentHeatingCoil,  !- Heating Coil Name",
        "  0.001;                   !- Heating Convergence Tolerance",

        "Schedule:Compact,",
        "  UnitVentMaxOA,           !- Name",
        "  Any Number,              !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,1.0;        !- Field 3",

        "Schedule:Compact,",
        "  U2MinOASched,            !- Name",
        "  Any Number,              !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,0.5;        !- Field 3",

        "Fan:ConstantVolume,",
        "  Zone1UnitVentFan,        !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  0.5,                     !- Fan Total Efficiency",
        "  0.0,                     !- Pressure Rise {Pa}",
        "  0.5,                     !- Maximum Flow Rate {m3/s}",
        "  0.9,                     !- Motor Efficiency",
        "  1.0,                     !- Motor In Airstream Fraction",
        "  SPACE1-1 Unit Vent Inlet,  !- Air Inlet Node Name",
        "  SPACE1-1 Fan Outlet;     !- Air Outlet Node Name",

        "Coil:Heating:Fuel,",
        "  Zone1UnitVentHeatingCoil,  !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  NaturalGas,              !- Fuel Type",
        "  0.8,                     !- Burner Efficiency",
        "  10000.0,                 !- Nominal Capacity {W}",
        "  SPACE1-1 Fan Outlet,     !- Air Inlet Node Name",
        "  SPACE1-1 Supply Inlet;   !- Air Outlet Node Name",

        "Zone,",
        "  SPACE1-1,                !- Name",
        "  0.0,                     !- Direction of Relative North {deg}",
        "  0.0,                     !- X Origin {m}",
        "  0.0,                     !- Y Origin {m}",
        "  0.0,                     !- Z Origin {m}",
        "  1,                       !- Type",
        "  1,                       !- Multiplier",
        "  2.438400269,             !- Ceiling Height {m}",
        "  239.247360229;           !- Volume {m3}",

        "ZoneHVAC:EquipmentConnections,",
        "  SPACE1-1,                !- Zone Name",
        "  SPACE1-1 Equipment,      !- Zone Conditioning Equipment List Name",
        "  SPACE1-1 Inlets,         !- Zone Air Inlet Node or NodeList Name",
        "  SPACE1-1 Mixer Secondary Inlet,  !- Zone Air Exhaust Node or NodeList Name",
        "  SPACE1-1 Zone Air Node,  !- Zone Air Node Name",
        "  SPACE1-1 Return Outlet;  !- Zone Return Air Node or NodeList Name",

        "NodeList,",
        "  SPACE1-1 Inlets,         !- Name",
        "  SPACE1-1 Supply Inlet;   !- Node 1 Name",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    state->dataGlobal->TimeStepsInHour = 1;
    state->dataGlobal->TimeStep = 1;
    state->dataGlobal->MinutesInTimeStep = 60;
    state->init_state(*state);

    GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    GetZoneEquipmentData(*state);
    GetZoneAirLoopEquipment(*state);
    GetUnitVentilatorInput(*state);
    state->dataUnitVentilators->GetUnitVentilatorInputFlag = false;

    // get input test for terminal air single duct mixer on inlet side of PTHP
    ASSERT_EQ(1, state->dataSingleDuct->NumATMixers);
    EXPECT_EQ("SPACE1-1 DOAS AIR TERMINAL", state->dataSingleDuct->SysATMixer(1).Name);                 // single duct air terminal mixer name
    EXPECT_EQ((int)HVAC::MixerType::InletSide, (int)state->dataSingleDuct->SysATMixer(1).type);         // air terminal mixer connection type
    EXPECT_EQ("AIRTERMINAL:SINGLEDUCT:MIXER", state->dataDefineEquipment->AirDistUnit(1).EquipType(1)); // Air distribution unit equipment type

    state->dataGlobal->BeginEnvrnFlag = false;

    // set input variables
    state->dataEnvrn->OutBaroPress = 101325.0;
    state->dataEnvrn->OutDryBulbTemp = 10.0;
    state->dataEnvrn->OutHumRat = 0.0070;
    state->dataEnvrn->OutEnthalpy = Psychrometrics::PsyHFnTdbW(state->dataEnvrn->OutDryBulbTemp, state->dataEnvrn->OutHumRat);
    state->dataEnvrn->StdRhoAir = 1.00;
    HVACInletMassFlowRate = 0.50;
    PrimaryAirMassFlowRate = 0.1;

    // set zoneNode air condition
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp = 24.0;
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat = 0.0070;
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Enthalpy =
        Psychrometrics::PsyHFnTdbW(state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp,
                                   state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat);

    state->dataHVACGlobal->TurnFansOff = false;
    state->dataHVACGlobal->TurnFansOn = true;

    UnitVentNum = 1;
    // set the mass flow rates from the input volume flow rates
    state->dataUnitVentilators->UnitVent(UnitVentNum).MaxAirMassFlow =
        state->dataEnvrn->StdRhoAir * state->dataUnitVentilators->UnitVent(UnitVentNum).MaxAirVolFlow;
    state->dataUnitVentilators->UnitVent(UnitVentNum).OutAirMassFlow =
        state->dataEnvrn->StdRhoAir * state->dataUnitVentilators->UnitVent(UnitVentNum).OutAirVolFlow;
    state->dataUnitVentilators->UnitVent(UnitVentNum).MinOutAirMassFlow =
        state->dataEnvrn->StdRhoAir * state->dataUnitVentilators->UnitVent(UnitVentNum).MinOutAirVolFlow;

    state->dataUnitVentilators->UnitVent(UnitVentNum).fanOp = HVAC::FanOp::Cycling;
    // initialize mass flow rates
    state->dataLoopNodes->Node(state->dataUnitVentilators->UnitVent(UnitVentNum).AirInNode).MassFlowRate = HVACInletMassFlowRate;
    state->dataLoopNodes->Node(state->dataUnitVentilators->UnitVent(UnitVentNum).AirInNode).MassFlowRateMax = HVACInletMassFlowRate;
    state->dataLoopNodes->Node(state->dataUnitVentilators->UnitVent(UnitVentNum).ATMixerPriNode).MassFlowRate = PrimaryAirMassFlowRate;
    state->dataLoopNodes->Node(state->dataUnitVentilators->UnitVent(UnitVentNum).ATMixerPriNode).MassFlowRateMaxAvail = PrimaryAirMassFlowRate;

    // set fan parameters
    state->dataFans->fans(1)->maxAirMassFlowRate = HVACInletMassFlowRate;
    state->dataFans->fans(1)->inletAirMassFlowRate = HVACInletMassFlowRate;
    state->dataFans->fans(1)->rhoAirStdInit = state->dataEnvrn->StdRhoAir;
    state->dataLoopNodes->Node(state->dataFans->fans(1)->inletNodeNum).MassFlowRateMaxAvail = HVACInletMassFlowRate;
    state->dataLoopNodes->Node(state->dataFans->fans(1)->outletNodeNum).MassFlowRateMax = HVACInletMassFlowRate;

    // primary air condition set at outdoor air condition
    state->dataLoopNodes->Node(state->dataUnitVentilators->UnitVent(UnitVentNum).ATMixerPriNode).Temp = state->dataEnvrn->OutDryBulbTemp;
    state->dataLoopNodes->Node(state->dataUnitVentilators->UnitVent(UnitVentNum).ATMixerPriNode).HumRat = state->dataEnvrn->OutHumRat;
    state->dataLoopNodes->Node(state->dataUnitVentilators->UnitVent(UnitVentNum).ATMixerPriNode).Enthalpy = state->dataEnvrn->OutEnthalpy;

    // set secondary air (recirculating air) conditions to zone air node
    state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).Temp =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp;
    state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).HumRat =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat;
    state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).Enthalpy =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Enthalpy;

    state->dataUnitVentilators->UnitVent(1).ZonePtr = 1;
    state->dataSize->SysSizingRunDone = true;
    state->dataSize->ZoneSizingRunDone = true;
    state->dataGlobal->SysSizingCalc = true;

    state->dataZoneEnergyDemand->CurDeadBandOrSetback.allocate(1);
    state->dataZoneEnergyDemand->CurDeadBandOrSetback(1) = false;
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand.allocate(1);
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputRequired = 5000.0;
    state->dataUnitVentilators->QZnReq = state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputRequired;

    state->dataUnitVentilators->UnitVent(UnitVentNum).availSched->currentVal = 1.0;    // unit is always available
    state->dataUnitVentilators->UnitVent(UnitVentNum).fanAvailSched->currentVal = 1.0; // fan is always available
    state->dataUnitVentilators->UnitVent(UnitVentNum).minOASched->currentVal = 0.5;    // min OA fraction is always available

    // set secondary air mass flow rate to zero
    state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).MassFlowRate = 0.0;
    // simulate Unit Ventilator zoneHVAC equipment
    SimUnitVentilator(
        *state, state->dataUnitVentilators->UnitVent(UnitVentNum).Name, ZoneNum, FirstHVACIteration, QUnitOut, LatOutputProvided, UnitVentNum);
    // apply mass conservation to determine secondary air mass flow rate
    SecondaryAirMassFlowRate =
        state->dataLoopNodes->Node(state->dataUnitVentilators->UnitVent(UnitVentNum).AirInNode).MassFlowRate - PrimaryAirMassFlowRate;
    // check the air mixer secondary air mass flow rate
    ASSERT_EQ(SecondaryAirMassFlowRate, state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).MassFlowRate);
    // check the cooling output delivered is within 2.0 Watt of zone cooling load
    ASSERT_NEAR(state->dataUnitVentilators->QZnReq, QUnitOut, 0.001);
}

TEST_F(EnergyPlusFixture, AirTerminalSingleDuctMixer_SimUnitVent_ATMSupplySide)
{

    bool ErrorsFound(false);
    bool FirstHVACIteration(false);
    Real64 HVACInletMassFlowRate(0.0);
    Real64 PrimaryAirMassFlowRate(0.0);
    Real64 SecondaryAirMassFlowRate(0.0);
    Real64 ATMixerOutletMassFlowRate(0.0);
    Real64 LatOutputProvided(0.0);
    Real64 QUnitOut(0.0);
    Real64 QZnReq(0.0);
    int ZoneNum(1);
    int UnitVentNum(1);

    std::string const idf_objects = delimited_string({
        "AirTerminal:SingleDuct:Mixer,",
        "  SPACE1-1 DOAS Air Terminal,  !- Name",
        "  ZoneHVAC:UnitVentilator,  !- ZoneHVAC Unit Object Type",
        "  SPACE1-1 Unit Vent,      !- ZoneHVAC Unit Object Name",
        "  SPACE1-1 Supply Inlet,   !- Mixer Outlet Node Name",
        "  SPACE1-1 Primary Air Inlet,  !- Mixer Primary Air Inlet Node Name",
        "  SPACE1-1 Unit Vent Outlet,  !- Mixer Secondary Air Inlet Node Name",
        "  SupplySide;              !- Mixer Connection Type",

        "ZoneHVAC:AirDistributionUnit,",
        "  SPACE1-1 DOAS ATU,       !- Name",
        "  SPACE1-1 Supply Inlet,   !- Air Distribution Unit Outlet Node Name",
        "  AirTerminal:SingleDuct:Mixer,  !- Air Terminal Object Type",
        "  SPACE1-1 DOAS Air Terminal;  !- Air Terminal Name",

        "Schedule:Compact,",
        "  FanAvailSched,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,1.0;        !- Field 3",

        "ZoneHVAC:EquipmentList,",
        "  SPACE1-1 Equipment,      !- Name",
        "  SequentialLoad,          !- Load Distribution Scheme",
        "  ZoneHVAC:AirDistributionUnit,  !- Zone Equipment 1 Object Type",
        "  SPACE1-1 DOAS ATU,       !- Zone Equipment 1 Name",
        "  1,                       !- Zone Equipment 1 Cooling Sequence",
        "  1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 1 Sequential Cooling Fraction Schedule Name",
        "  ,                        !- Zone Equipment 1 Sequential Heating Fraction Schedule Name",
        "  ZoneHVAC:UnitVentilator,  !- Zone Equipment 2 Object Type",
        "  SPACE1-1 Unit Vent,      !- Zone Equipment 2 Name",
        "  2,                       !- Zone Equipment 2 Cooling Sequence",
        "  2,                       !- Zone Equipment 2 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 2 Sequential Cooling Fraction Schedule Name",
        "  ;                        !- Zone Equipment 2 Sequential Heating Fraction Schedule Name",

        "ZoneHVAC:UnitVentilator,",
        "  SPACE1-1 Unit Vent,      !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  0.5,                     !- Maximum Supply Air Flow Rate {m3/s}",
        "  VariablePercent,         !- Outdoor Air Control Type",
        "  0.2,                     !- Minimum Outdoor Air Flow Rate {m3/s}",
        "  U2MinOASched,            !- Minimum Outdoor Air Schedule Name",
        "  0.5,                     !- Maximum Outdoor Air Flow Rate {m3/s}",
        "  UnitVentMaxOA,           !- Maximum Outdoor Air Fraction or Temperature Schedule Name",
        "  SPACE1-1 Unit Vent Inlet,  !- Air Inlet Node Name",
        "  SPACE1-1 Unit Vent Outlet,  !- Air Outlet Node Name",
        "  ,                        !- Outdoor Air Node Name",
        "  ,                        !- Exhaust Air Node Name",
        "  ,                        !- Mixed Air Node Name",
        "  Fan:ConstantVolume,      !- Supply Air Fan Object Type",
        "  Zone1UnitVentFan,        !- Supply Air Fan Name",
        "  Heating,                 !- Coil Option",
        "  ,                        !- Supply Air Fan Operating Mode Schedule Name",
        "  Coil:Heating:Fuel,       !- Heating Coil Object Type",
        "  Zone1UnitVentHeatingCoil,  !- Heating Coil Name",
        "  0.001;                   !- Heating Convergence Tolerance",

        "Schedule:Compact,",
        "  UnitVentMaxOA,           !- Name",
        "  Any Number,              !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,1.0;        !- Field 3",

        "Schedule:Compact,",
        "  U2MinOASched,            !- Name",
        "  Any Number,              !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,0.5;        !- Field 3",

        "Fan:ConstantVolume,",
        "  Zone1UnitVentFan,        !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  0.5,                     !- Fan Total Efficiency",
        "  0.0,                     !- Pressure Rise {Pa}",
        "  0.5,                     !- Maximum Flow Rate {m3/s}",
        "  0.9,                     !- Motor Efficiency",
        "  1.0,                     !- Motor In Airstream Fraction",
        "  SPACE1-1 Unit Vent Inlet,  !- Air Inlet Node Name",
        "  SPACE1-1 Fan Outlet;     !- Air Outlet Node Name",

        "Coil:Heating:Fuel,",
        "  Zone1UnitVentHeatingCoil,  !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  NaturalGas,              !- Fuel Type",
        "  0.8,                     !- Burner Efficiency",
        "  10000.0,                 !- Nominal Capacity {W}",
        "  SPACE1-1 Fan Outlet,     !- Air Inlet Node Name",
        "  SPACE1-1 Unit Vent Outlet;  !- Air Outlet Node Name",

        "Zone,",
        "  SPACE1-1,                !- Name",
        "  0.0,                     !- Direction of Relative North {deg}",
        "  0.0,                     !- X Origin {m}",
        "  0.0,                     !- Y Origin {m}",
        "  0.0,                     !- Z Origin {m}",
        "  1,                       !- Type",
        "  1,                       !- Multiplier",
        "  2.438400269,             !- Ceiling Height {m}",
        "  239.247360229;           !- Volume {m3}",

        "ZoneHVAC:EquipmentConnections,",
        "  SPACE1-1,                !- Zone Name",
        "  SPACE1-1 Equipment,      !- Zone Conditioning Equipment List Name",
        "  SPACE1-1 Inlets,         !- Zone Air Inlet Node or NodeList Name",
        "  SPACE1-1 Unit Vent Inlet,  !- Zone Air Exhaust Node or NodeList Name",
        "  SPACE1-1 Zone Air Node,  !- Zone Air Node Name",
        "  SPACE1-1 Return Outlet;  !- Zone Return Air Node or NodeList Name",

        "NodeList,",
        "  SPACE1-1 Inlets,         !- Name",
        "  SPACE1-1 Supply Inlet;   !- Node 1 Name",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    state->dataGlobal->TimeStepsInHour = 1;
    state->dataGlobal->TimeStep = 1;
    state->dataGlobal->MinutesInTimeStep = 60;
    state->init_state(*state);

    GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    GetZoneEquipmentData(*state);
    GetZoneAirLoopEquipment(*state);
    GetUnitVentilatorInput(*state);
    state->dataUnitVentilators->GetUnitVentilatorInputFlag = false;

    // get input test for terminal air single duct mixer on supply side of PTHP
    ASSERT_EQ(1, state->dataSingleDuct->NumATMixers);
    EXPECT_EQ("SPACE1-1 DOAS AIR TERMINAL", state->dataSingleDuct->SysATMixer(1).Name);                 // single duct air terminal mixer name
    EXPECT_EQ((int)HVAC::MixerType::SupplySide, (int)state->dataSingleDuct->SysATMixer(1).type);        // air terminal mixer connection type
    EXPECT_EQ("AIRTERMINAL:SINGLEDUCT:MIXER", state->dataDefineEquipment->AirDistUnit(1).EquipType(1)); // Air distribution unit equipment type

    // set input variables
    state->dataEnvrn->OutBaroPress = 101325.0;
    state->dataEnvrn->OutDryBulbTemp = 10.0;
    state->dataEnvrn->OutHumRat = 0.0070;
    state->dataEnvrn->OutEnthalpy = Psychrometrics::PsyHFnTdbW(state->dataEnvrn->OutDryBulbTemp, state->dataEnvrn->OutHumRat);
    state->dataEnvrn->StdRhoAir = 1.00;
    HVACInletMassFlowRate = 0.50;
    PrimaryAirMassFlowRate = 0.1;

    state->dataGlobal->BeginEnvrnFlag = false;

    // set zoneNode air condition
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp = 24.0;
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat = 0.0070;
    state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Enthalpy =
        Psychrometrics::PsyHFnTdbW(state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp,
                                   state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat);

    state->dataHVACGlobal->TurnFansOff = false;
    state->dataHVACGlobal->TurnFansOn = true;

    UnitVentNum = 1;
    // set the mass flow rates from the input volume flow rates
    state->dataUnitVentilators->UnitVent(UnitVentNum).MaxAirMassFlow =
        state->dataEnvrn->StdRhoAir * state->dataUnitVentilators->UnitVent(UnitVentNum).MaxAirVolFlow;
    state->dataUnitVentilators->UnitVent(UnitVentNum).OutAirMassFlow =
        state->dataEnvrn->StdRhoAir * state->dataUnitVentilators->UnitVent(UnitVentNum).OutAirVolFlow;
    state->dataUnitVentilators->UnitVent(UnitVentNum).MinOutAirMassFlow =
        state->dataEnvrn->StdRhoAir * state->dataUnitVentilators->UnitVent(UnitVentNum).MinOutAirVolFlow;

    state->dataUnitVentilators->UnitVent(UnitVentNum).fanOp = HVAC::FanOp::Cycling;
    // initialize mass flow rates
    state->dataLoopNodes->Node(state->dataUnitVentilators->UnitVent(UnitVentNum).AirInNode).MassFlowRate = HVACInletMassFlowRate;
    state->dataLoopNodes->Node(state->dataUnitVentilators->UnitVent(UnitVentNum).AirInNode).MassFlowRateMax = HVACInletMassFlowRate;
    state->dataLoopNodes->Node(state->dataUnitVentilators->UnitVent(UnitVentNum).ATMixerPriNode).MassFlowRate = PrimaryAirMassFlowRate;
    state->dataLoopNodes->Node(state->dataUnitVentilators->UnitVent(UnitVentNum).ATMixerPriNode).MassFlowRateMaxAvail = PrimaryAirMassFlowRate;

    // set fan parameters
    state->dataFans->fans(1)->maxAirMassFlowRate = HVACInletMassFlowRate;
    state->dataFans->fans(1)->inletAirMassFlowRate = HVACInletMassFlowRate;
    state->dataFans->fans(1)->rhoAirStdInit = state->dataEnvrn->StdRhoAir;
    state->dataLoopNodes->Node(state->dataFans->fans(1)->inletNodeNum).MassFlowRateMaxAvail = HVACInletMassFlowRate;
    state->dataLoopNodes->Node(state->dataFans->fans(1)->outletNodeNum).MassFlowRateMax = HVACInletMassFlowRate;

    // primary air condition at outside air condition
    state->dataLoopNodes->Node(state->dataUnitVentilators->UnitVent(UnitVentNum).ATMixerPriNode).Temp = state->dataEnvrn->OutDryBulbTemp;
    state->dataLoopNodes->Node(state->dataUnitVentilators->UnitVent(UnitVentNum).ATMixerPriNode).HumRat = state->dataEnvrn->OutHumRat;
    state->dataLoopNodes->Node(state->dataUnitVentilators->UnitVent(UnitVentNum).ATMixerPriNode).Enthalpy = state->dataEnvrn->OutEnthalpy;

    // set UnitVent inlet condition to zone air node
    state->dataLoopNodes->Node(state->dataUnitVentilators->UnitVent(UnitVentNum).AirInNode).Temp =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Temp;
    state->dataLoopNodes->Node(state->dataUnitVentilators->UnitVent(UnitVentNum).AirInNode).HumRat =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).HumRat;
    state->dataLoopNodes->Node(state->dataUnitVentilators->UnitVent(UnitVentNum).AirInNode).Enthalpy =
        state->dataLoopNodes->Node(state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode).Enthalpy;

    state->dataUnitVentilators->UnitVent(1).ZonePtr = 1;
    state->dataSize->SysSizingRunDone = true;
    state->dataSize->ZoneSizingRunDone = true;
    state->dataGlobal->SysSizingCalc = true;

    state->dataZoneEnergyDemand->CurDeadBandOrSetback.allocate(1);
    state->dataZoneEnergyDemand->CurDeadBandOrSetback(1) = false;
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand.allocate(1);
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(ZoneNum).RemainingOutputRequired = 5000.0;
    QZnReq = state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).RemainingOutputRequired;

    state->dataUnitVentilators->UnitVent(UnitVentNum).availSched->currentVal = 1.0;    // unit is always available
    state->dataUnitVentilators->UnitVent(UnitVentNum).fanAvailSched->currentVal = 1.0; // fan is always available
    state->dataUnitVentilators->UnitVent(UnitVentNum).minOASched->currentVal = 0.5;    // min OA fraction is always available

    // set secondary air mass flow rate to zero
    state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).MassFlowRate = 0.0;
    // simulate Unit Ventilator ZoneHVAC equipment
    SimUnitVentilator(
        *state, state->dataUnitVentilators->UnitVent(UnitVentNum).Name, ZoneNum, FirstHVACIteration, QUnitOut, LatOutputProvided, UnitVentNum);
    // apply mass conservation to determine secondary mass flow rate
    SecondaryAirMassFlowRate = state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).MassFlowRate;
    // check the terminal air mixer secondary air mass flow rate
    ASSERT_EQ(SecondaryAirMassFlowRate, state->dataLoopNodes->Node(state->dataSingleDuct->SysATMixer(1).SecInNode).MassFlowRate);
    // check the air mixer outlet air mass flow rate
    ATMixerOutletMassFlowRate = SecondaryAirMassFlowRate + PrimaryAirMassFlowRate;
    ASSERT_EQ(ATMixerOutletMassFlowRate, state->dataSingleDuct->SysATMixer(1).MixedAirMassFlowRate);
    // check the cooling output delivered is within 2.0 Watt of zone cooling load
    ASSERT_NEAR(QZnReq, QUnitOut, 0.001);
}
TEST_F(EnergyPlusFixture, AirTerminalSingleDuctMixer_GetInputDOASpecs)
{

    // This test was added for #6399 to confirm that the correct SysATMixer( ATMixerNum ).OARequirementsPtr
    // is found when searching Sizing:Zone objects. The defect is exposed when there are more Sizing:Zone objects than
    // DesignSpecification:OutdoorAir objects. In this test, there are 2 zones and one DSOA object.
    // In this test, the first mixer declares "DSOA 1" directly, and the second mixer leaves it blank so
    // it searches the sizing objects. Both mixers should find OARequirementsPtr = 1.

    bool ErrorsFound(false);

    std::string const idf_objects = delimited_string({
        "DesignSpecification:OutdoorAir,",
        "  DSOA 1,                  !- Name",
        "  Sum,                     !- Outdoor Air Method",
        "  0.0,                     !- Outdoor Air Flow per Person {m3/s-person}",
        "  0.0009,                  !- Outdoor Air Flow per Zone Floor Area {m3/s-m2}",
        "  0.0;                     !- Outdoor Air Flow per Zone {m3/s}",

        "Sizing:Zone,",
        "  SPACE1-1,                !- Zone or ZoneList Name",
        "  SupplyAirTemperature,    !- Zone Cooling Design Supply Air Temperature Input Method",
        "  12.0,                    !- Zone Cooling Design Supply Air Temperature {C}",
        "  ,                        !- Zone Cooling Design Supply Air Temperature Difference {deltaC}",
        "  SupplyAirTemperature,    !- Zone Heating Design Supply Air Temperature Input Method",
        "  50.0,                    !- Zone Heating Design Supply Air Temperature {C}",
        "  ,                        !- Zone Heating Design Supply Air Temperature Difference {deltaC}",
        "  0.008,                   !- Zone Cooling Design Supply Air Humidity Ratio {kgWater/kgDryAir}",
        "  0.008,                   !- Zone Heating Design Supply Air Humidity Ratio {kgWater/kgDryAir}",
        "  DSOA 1,                  !- Design Specification Outdoor Air Object Name",
        "  0.0,                     !- Zone Heating Sizing Factor",
        "  0.0,                     !- Zone Cooling Sizing Factor",
        "  DesignDay,               !- Cooling Design Air Flow Method",
        "  0.0,                     !- Cooling Design Air Flow Rate {m3/s}",
        "  ,                        !- Cooling Minimum Air Flow per Zone Floor Area {m3/s-m2}",
        "  ,                        !- Cooling Minimum Air Flow {m3/s}",
        "  ,                        !- Cooling Minimum Air Flow Fraction",
        "  DesignDay,               !- Heating Design Air Flow Method",
        "  0.0,                     !- Heating Design Air Flow Rate {m3/s}",
        "  ,                        !- Heating Maximum Air Flow per Zone Floor Area {m3/s-m2}",
        "  ,                        !- Heating Maximum Air Flow {m3/s}",
        "  ,                        !- Heating Maximum Air Flow Fraction",
        "  ,                        !- Design Specification Zone Air Distribution Object Name",
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

        "Sizing:Zone,",
        "  SPACE1-2,                !- Zone or ZoneList Name",
        "  SupplyAirTemperature,    !- Zone Cooling Design Supply Air Temperature Input Method",
        "  12.0,                    !- Zone Cooling Design Supply Air Temperature {C}",
        "  ,                        !- Zone Cooling Design Supply Air Temperature Difference {deltaC}",
        "  SupplyAirTemperature,    !- Zone Heating Design Supply Air Temperature Input Method",
        "  50.0,                    !- Zone Heating Design Supply Air Temperature {C}",
        "  ,                        !- Zone Heating Design Supply Air Temperature Difference {deltaC}",
        "  0.008,                   !- Zone Cooling Design Supply Air Humidity Ratio {kgWater/kgDryAir}",
        "  0.008,                   !- Zone Heating Design Supply Air Humidity Ratio {kgWater/kgDryAir}",
        "  DSOA 1,                  !- Design Specification Outdoor Air Object Name",
        "  0.0,                     !- Zone Heating Sizing Factor",
        "  0.0,                     !- Zone Cooling Sizing Factor",
        "  DesignDay,               !- Cooling Design Air Flow Method",
        "  0.0,                     !- Cooling Design Air Flow Rate {m3/s}",
        "  ,                        !- Cooling Minimum Air Flow per Zone Floor Area {m3/s-m2}",
        "  ,                        !- Cooling Minimum Air Flow {m3/s}",
        "  ,                        !- Cooling Minimum Air Flow Fraction",
        "  DesignDay,               !- Heating Design Air Flow Method",
        "  0.0,                     !- Heating Design Air Flow Rate {m3/s}",
        "  ,                        !- Heating Maximum Air Flow per Zone Floor Area {m3/s-m2}",
        "  ,                        !- Heating Maximum Air Flow {m3/s}",
        "  ,                        !- Heating Maximum Air Flow Fraction",
        "  ,                        !- Design Specification Zone Air Distribution Object Name",
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

        "AirTerminal:SingleDuct:Mixer,",
        "  SPACE1-1 DOAS Air Terminal,  !- Name",
        "  ZoneHVAC:PackagedTerminalAirConditioner,  !- ZoneHVAC Unit Object Type",
        "  SPACE1-1 PTAC,           !- ZoneHVAC Unit Object Name",
        "  SPACE1-1 Heat Pump Inlet,  !- Mixer Outlet Node Name",
        "  SPACE1-1 Air Terminal Mixer Primary Inlet,  !- Mixer Primary Air Inlet Node Name",
        "  SPACE1-1 Air Terminal Mixer Secondary Inlet,  !- Mixer Secondary Air Inlet Node Name",
        "  InletSide,               !- Mixer Connection Type",
        "  DSOA 1;                  !- Design Specification Outdoor Air Object Name",

        "ZoneHVAC:AirDistributionUnit,",
        "  SPACE1-1 DOAS ATU,       !- Name",
        "  SPACE1-1 Heat Pump Inlet,  !- Air Distribution Unit Outlet Node Name",
        "  AirTerminal:SingleDuct:Mixer,  !- Air Terminal Object Type",
        "  SPACE1-1 DOAS Air Terminal;  !- Air Terminal Name",

        "ZoneHVAC:EquipmentList,",
        "  SPACE1-1 Equipment,      !- Name",
        "  SequentialLoad,          !- Load Distribution Scheme",
        "  ZoneHVAC:AirDistributionUnit,  !- Zone Equipment 1 Object Type",
        "  SPACE1-1 DOAS ATU,       !- Zone Equipment 1 Name",
        "  1,                       !- Zone Equipment 1 Cooling Sequence",
        "  1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 1 Sequential Cooling Fraction Schedule Name",
        "  ;                        !- Zone Equipment 1 Sequential Heating Fraction Schedule Name",

        "Zone,",
        "  SPACE1-1;                !- Name",

        "ZoneHVAC:EquipmentConnections,",
        "  SPACE1-1,                !- Zone Name",
        "  SPACE1-1 Equipment,      !- Zone Conditioning Equipment List Name",
        "  ,                        !- Zone Air Inlet Node or NodeList Name",
        "  SPACE1-1 Air Terminal Mixer Secondary Inlet,  !- Zone Air Exhaust Node or NodeList Name",
        "  SPACE1-1 Zone Air Node,  !- Zone Air Node Name",
        "  SPACE1-1 Return Outlet;  !- Zone Return Air Node or NodeList Name",

        "AirTerminal:SingleDuct:Mixer,",
        "  SPACE1-2 DOAS Air Terminal,  !- Name",
        "  ZoneHVAC:PackagedTerminalAirConditioner,  !- ZoneHVAC Unit Object Type",
        "  SPACE1-2 PTAC,           !- ZoneHVAC Unit Object Name",
        "  SPACE1-2 Supply Inlet,   !- Mixer Outlet Node Name",
        "  SPACE1-2 Air Terminal Mixer Primary Inlet,  !- Mixer Primary Air Inlet Node Name",
        "  SPACE1-2 Air Terminal Mixer Secondary Inlet,  !- Mixer Secondary Air Inlet Node Name",
        "  SupplySide,              !- Mixer Connection Type",
        "  ;                        !- Design Specification Outdoor Air Object Name",

        "ZoneHVAC:AirDistributionUnit,",
        "  SPACE1-2 DOAS ATU,       !- Name",
        "  SPACE1-2 Supply Inlet,   !- Air Distribution Unit Outlet Node Name",
        "  AirTerminal:SingleDuct:Mixer,  !- Air Terminal Object Type",
        "  SPACE1-2 DOAS Air Terminal;  !- Air Terminal Name",

        "ZoneHVAC:EquipmentList,",
        "  SPACE1-2 Equipment,      !- Name",
        "  SequentialLoad,          !- Load Distribution Scheme",
        "  ZoneHVAC:AirDistributionUnit,  !- Zone Equipment 1 Object Type",
        "  SPACE1-2 DOAS ATU,       !- Zone Equipment 1 Name",
        "  1,                       !- Zone Equipment 1 Cooling Sequence",
        "  1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 1 Sequential Cooling Fraction Schedule Name",
        "  ;                        !- Zone Equipment 1 Sequential Heating Fraction Schedule Name",

        "Zone,",
        "  SPACE1-2;                !- Name",

        "ZoneHVAC:EquipmentConnections,",
        "  SPACE1-2,                !- Zone Name",
        "  SPACE1-2 Equipment,      !- Zone Conditioning Equipment List Name",
        "  SPACE1-2 Supply Inlet,   !- Zone Air Inlet Node or NodeList Name",
        "  SPACE1-2 Air Terminal Mixer Secondary Inlet,  !- Zone Air Exhaust Node or NodeList Name",
        "  SPACE1-2 Zone Air Node,  !- Zone Air Node Name",
        "  SPACE1-2 Return Outlet;  !- Zone Return Air Node or NodeList Name",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    state->init_state(*state);

    GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    SizingManager::GetOARequirements(*state);
    SizingManager::GetZoneSizingInput(*state);
    GetZoneEquipmentData(*state);
    ZoneEquipmentManager::SetUpZoneSizingArrays(*state);
    GetZoneAirLoopEquipment(*state);
    GetATMixers(*state);

    ASSERT_EQ(2, state->dataSingleDuct->NumATMixers);
    EXPECT_EQ("SPACE1-1 DOAS AIR TERMINAL", state->dataSingleDuct->SysATMixer(1).Name);                 // single duct air terminal mixer name
    EXPECT_EQ((int)HVAC::MixerType::InletSide, (int)state->dataSingleDuct->SysATMixer(1).type);         // air terminal mixer connection type
    EXPECT_EQ("AIRTERMINAL:SINGLEDUCT:MIXER", state->dataDefineEquipment->AirDistUnit(1).EquipType(1)); // Air distribution unit equipment type
    EXPECT_EQ(1, state->dataSingleDuct->SysATMixer(1).OARequirementsPtr); // design spec OA pointer - for both mixers this pointer should be 1

    EXPECT_EQ("SPACE1-2 DOAS AIR TERMINAL", state->dataSingleDuct->SysATMixer(2).Name);                 // single duct air terminal mixer name
    EXPECT_EQ((int)HVAC::MixerType::SupplySide, (int)state->dataSingleDuct->SysATMixer(2).type);        // air terminal mixer connection type
    EXPECT_EQ("AIRTERMINAL:SINGLEDUCT:MIXER", state->dataDefineEquipment->AirDistUnit(2).EquipType(1)); // Air distribution unit equipment type
    // design spec OA pointer - for both mixers this pointer should be 1
    // before the fix, this was 2 which later caused an array bounds error
    EXPECT_EQ(1, state->dataSingleDuct->SysATMixer(2).OARequirementsPtr);
}

TEST_F(EnergyPlusFixture, AirTerminalSingleDuctMixer_SimFCU_ATMInletSideTest)
{

    std::string const idf_objects = delimited_string({
        "Schedule:Compact,",
        "  FanAvailSched,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,1.0;        !- Field 3",

        "ScheduleTypeLimits,",
        "  Fraction,                !- Name",
        "  0.0,                     !- Lower Limit Value",
        "  1.0,                     !- Upper Limit Value",
        "  Continuous;              !- Numeric Type",

        "Zone,",
        "  Zone One,                !- Name",
        "  0.0,                     !- Direction of Relative North {deg}",
        "  0.0,                     !- X Origin {m}",
        "  0.0,                     !- Y Origin {m}",
        "  0.0,                     !- Z Origin {m}",
        "  ,                        !- Type",
        "  1;                       !- Multiplier",

        "ZoneHVAC:FourPipeFanCoil,",
        "  FCU VarFan VarFluidFlow,  !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  VariableFanVariableFlow,  !- Capacity Control Method",
        "  Autosize,                !- Maximum Supply Air Flow Rate {m3/s}",
        "  ,                        !- Low Speed Supply Air Flow Ratio",
        "  ,                        !- Medium Speed Supply Air Flow Ratio",
        "  Autosize,                !- Maximum Outdoor Air Flow Rate {m3/s}",
        "  ,                        !- Outdoor Air Schedule Name",
        "  Node 5,                  !- Air Inlet Node Name",
        "  Node 63,                 !- Air Outlet Node Name",
        "  ,                        !- Outdoor Air Mixer Object Type",
        "  ,                        !- Outdoor Air Mixer Name",
        "  Fan:VariableVolume,      !- Supply Air Fan Object Type",
        "  FCU VarFan,              !- Supply Air Fan Name",
        "  Coil:Cooling:Water,      !- Cooling Coil Object Type",
        "  FCU Cooling Coil,        !- Cooling Coil Name",
        "  Autosize,                !- Maximum Cold Water Flow Rate {m3/s}",
        "  ,                        !- Minimum Cold Water Flow Rate {m3/s}",
        "  ,                        !- Cooling Convergence Tolerance",
        "  Coil:Heating:Water,      !- Heating Coil Object Type",
        "  FCU Heating Coil,        !- Heating Coil Name",
        "  Autosize,                !- Maximum Hot Water Flow Rate {m3/s}",
        "  ,                        !- Minimum Hot Water Flow Rate {m3/s}",
        "  ,                        !- Heating Convergence Tolerance",
        "  ,                        !- Availability Manager List Name",
        "  ,                        !- Design Specification ZoneHVAC Sizing Object Name",
        "  ,                        !- Supply Air Fan Operating Mode Schedule Name",
        "  Autosize,                !- Minimum Supply Air Temperature in Cooling Mode {C}",
        "  Autosize;                !- Maximum Supply Air Temperature in Heating Mode {C}",

        "AirTerminal:SingleDuct:Mixer,",
        "  Inlet Side Mixer,        !- Name",
        "  ZoneHVAC:FourPipeFanCoil,  !- ZoneHVAC Unit Object Type",
        "  FCU VarFan VarFluidFlow,  !- ZoneHVAC Unit Object Name",
        "  Node 5,                  !- Mixer Outlet Node Name",
        "  Node 62,                 !- Mixer Primary Air Inlet Node Name",
        "  Node 64,                 !- Mixer Secondary Air Inlet Node Name",
        "  InletSide;               !- Mixer Connection Type",

        "ZoneHVAC:AirDistributionUnit,",
        "  ADU Inlet Side Mixer,    !- Name",
        "  Node 5,                  !- Air Distribution Unit Outlet Node Name",
        "  AirTerminal:SingleDuct:Mixer,  !- Air Terminal Object Type",
        "  Inlet Side Mixer;        !- Air Terminal Name",

        "ZoneHVAC:EquipmentList,",
        "  Zone one Equipment List,  !- Name",
        "  SequentialLoad,          !- Load Distribution Scheme",
        "  ZoneHVAC:AirDistributionUnit,  !- Zone Equipment 1 Object Type",
        "  ADU Inlet Side Mixer,    !- Zone Equipment 1 Name",
        "  1,                       !- Zone Equipment 1 Cooling Sequence",
        "  1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 1 Sequential Cooling Fraction Schedule Name",
        "  ,                        !- Zone Equipment 1 Sequential Heating Fraction Schedule Name",
        "  ZoneHVAC:FourPipeFanCoil,  !- Zone Equipment 2 Object Type",
        "  FCU VarFan VarFluidFlow,  !- Zone Equipment 2 Name",
        "  2,                       !- Zone Equipment 2 Cooling Sequence",
        "  2,                       !- Zone Equipment 2 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 2 Sequential Cooling Fraction Schedule Name",
        "  ;                        !- Zone Equipment 2 Sequential Heating Fraction Schedule Name",

        "ZoneHVAC:EquipmentConnections,",
        "  Zone One,                !- Zone Name",
        "  Zone one Equipment List,  !- Zone Conditioning Equipment List Name",
        "  Node 63,                 !- Zone Air Inlet Node or NodeList Name",
        "  Node 64,                 !- Zone Air Exhaust Node or NodeList Name",
        "  Zone one Air Node,       !- Zone Air Node Name",
        "  Node 61;                 !- Zone Return Air Node or NodeList Name",

        "Fan:VariableVolume,",
        "  FCU VarFan,              !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  0.6045,                  !- Fan Total Efficiency",
        "  600.0,                   !- Pressure Rise {Pa}",
        "  Autosize,                !- Maximum Flow Rate {m3/s}",
        "  FixedFlowRate,           !- Fan Power Minimum Flow Rate Input Method",
        "  0.0,                     !- Fan Power Minimum Flow Fraction",
        "  0.0,                     !- Fan Power Minimum Air Flow Rate {m3/s}",
        "  0.93,                    !- Motor Efficiency",
        "  1.0,                     !- Motor In Airstream Fraction",
        "  0.040759894,             !- Fan Power Coefficient 1",
        "  0.08804497,              !- Fan Power Coefficient 2",
        "  -0.07292612,             !- Fan Power Coefficient 3",
        "  0.943739823,             !- Fan Power Coefficient 4",
        "  0.0,                     !- Fan Power Coefficient 5",
        "  Node 5,                  !- Air Inlet Node Name",
        "  FCU Fan Outlet Node,     !- Air Outlet Node Name",
        "  General;                 !- End-Use Subcategory",

        "Coil:Cooling:Water,",
        "  FCU Cooling Coil,        !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  Autosize,                !- Design Water Flow Rate {m3/s}",
        "  Autosize,                !- Design Air Flow Rate {m3/s}",
        "  Autosize,                !- Design Inlet Water Temperature {C}",
        "  Autosize,                !- Design Inlet Air Temperature {C}",
        "  Autosize,                !- Design Outlet Air Temperature {C}",
        "  Autosize,                !- Design Inlet Air Humidity Ratio {kgWater/kgDryAir}",
        "  Autosize,                !- Design Outlet Air Humidity Ratio {kgWater/kgDryAir}",
        "  Node 66,                 !- Water Inlet Node Name",
        "  Node 68,                 !- Water Outlet Node Name",
        "  FCU Fan Outlet Node,     !- Air Inlet Node Name",
        "  FCU CCoil Outlet Node,   !- Air Outlet Node Name",
        "  SimpleAnalysis,          !- Type of Analysis",
        "  CrossFlow;               !- Heat Exchanger Configuration",

        "Coil:Heating:Water,",
        "  FCU Heating Coil,        !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  Autosize,                !- U-Factor Times Area Value {W/K}",
        "  Autosize,                !- Maximum Water Flow Rate {m3/s}",
        "  Node 67,                 !- Water Inlet Node Name",
        "  Node 65,                 !- Water Outlet Node Name",
        "  FCU CCoil Outlet Node,   !- Air Inlet Node Name",
        "  Node 63,                 !- Air Outlet Node Name",
        "  UFactorTimesAreaAndDesignWaterFlowRate,  !- Performance Input Method",
        "  Autosize,                !- Rated Capacity {W}",
        "  82.2,                    !- Rated Inlet Water Temperature {C}",
        "  16.6,                    !- Rated Inlet Air Temperature {C}",
        "  71.1,                    !- Rated Outlet Water Temperature {C}",
        "  32.2,                    !- Rated Outlet Air Temperature {C}",
        "  0.5;                     !- Rated Ratio for Air and Water Convection",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    bool ErrorsFound(false);
    bool FirstHVACIteration(false);
    Real64 PrimaryAirMassFlowRate(0.0);
    Real64 SecondaryAirMassFlowRate(0.0);
    Real64 DesignHeatAirVolFlow(0.50);
    Real64 DesignCoolAirVolFlow(0.60);
    Real64 QUnitOut(0.0);
    Real64 QLatOut(0.0);
    Real64 QZnReq(0.0);
    int ZoneNum(1);
    int FanCoilNum(1);

    state->dataGlobal->TimeStepsInHour = 1;
    state->dataGlobal->TimeStep = 1;
    state->dataGlobal->MinutesInTimeStep = 60;
    state->init_state(*state);

    state->dataSize->CurZoneEqNum = 1;

    state->dataEnvrn->OutBaroPress = 101325.0;
    state->dataEnvrn->StdRhoAir = Psychrometrics::PsyRhoAirFnPbTdbW(*state, state->dataEnvrn->OutBaroPress, 20.0, 0.0);
    state->dataWaterCoils->GetWaterCoilsInputFlag = true;
    GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    GetZoneEquipmentData(*state);
    GetFanCoilUnits(*state);

    auto &thisFanCoil(state->dataFanCoilUnits->FanCoil(1));
    auto &thisATMixer(state->dataSingleDuct->SysATMixer(1));
    auto *thisFan = state->dataFans->fans(1);

    // get input test for terminal air single duct mixer on inlet side of PTAC
    ASSERT_EQ(1, state->dataSingleDuct->NumATMixers);
    EXPECT_EQ("INLET SIDE MIXER", thisATMixer.Name);                                                    // single duct air terminal mixer name
    EXPECT_EQ((int)HVAC::MixerType::InletSide, (int)thisATMixer.type);                                  // air terminal mixer connection type
    EXPECT_EQ("AIRTERMINAL:SINGLEDUCT:MIXER", state->dataDefineEquipment->AirDistUnit(1).EquipType(1)); // Air distribution unit equipment type
    EXPECT_EQ((int)HVAC::FanType::VAV, (int)thisFanCoil.fanType);
    EXPECT_EQ("COIL:COOLING:WATER", thisFanCoil.CCoilType);
    EXPECT_EQ("FCU COOLING COIL", thisFanCoil.CCoilName);
    EXPECT_EQ("COIL:HEATING:WATER", thisFanCoil.HCoilType);
    EXPECT_EQ("FCU HEATING COIL", thisFanCoil.HCoilName);

    state->dataPlnt->TotNumLoops = 2;
    state->dataPlnt->PlantLoop.allocate(state->dataPlnt->TotNumLoops);
    state->dataSize->NumPltSizInput = 2;
    state->dataSize->PlantSizData.allocate(state->dataSize->NumPltSizInput);
    // chilled water coil
    auto &CWCoil(state->dataWaterCoils->WaterCoil(2));
    thisFanCoil.CCoilName_Index = 2;
    state->dataLoopNodes->Node(CWCoil.WaterInletNodeNum).Temp = 6.0;
    CWCoil.WaterPlantLoc.loopNum = 2;
    CWCoil.WaterPlantLoc.loopSideNum = DataPlant::LoopSideLocation::Demand;
    CWCoil.WaterPlantLoc.branchNum = 1;
    CWCoil.WaterPlantLoc.compNum = 1;
    // hot water coil
    auto &HWCoil(state->dataWaterCoils->WaterCoil(1));
    thisFanCoil.HCoilName_Index = 1;
    state->dataLoopNodes->Node(HWCoil.WaterInletNodeNum).Temp = 60.0;
    HWCoil.WaterPlantLoc.loopNum = 1;
    HWCoil.WaterPlantLoc.loopSideNum = DataPlant::LoopSideLocation::Demand;
    HWCoil.WaterPlantLoc.branchNum = 1;
    HWCoil.WaterPlantLoc.compNum = 1;
    for (int l = 1; l <= state->dataPlnt->TotNumLoops; ++l) {

        auto &loopside(state->dataPlnt->PlantLoop(l).LoopSide(DataPlant::LoopSideLocation::Demand));
        loopside.TotalBranches = 1;
        loopside.Branch.allocate(1);
        auto &loopsidebranch(state->dataPlnt->PlantLoop(l).LoopSide(DataPlant::LoopSideLocation::Demand).Branch(1));
        loopsidebranch.TotalComponents = 1;
        loopsidebranch.Comp.allocate(1);
    }
    // chilled water plant loop
    auto &CWLoop(state->dataPlnt->PlantLoop(2));
    CWLoop.Name = "ChilledWaterLoop";
    CWLoop.FluidName = "WATER";
    CWLoop.glycol = Fluid::GetWater(*state);
    CWLoop.LoopSide(DataPlant::LoopSideLocation::Demand).Branch(1).Comp(1).Name = CWCoil.Name;
    CWLoop.LoopSide(DataPlant::LoopSideLocation::Demand).Branch(1).Comp(1).Type = DataPlant::PlantEquipmentType::CoilWaterCooling;
    CWLoop.LoopSide(DataPlant::LoopSideLocation::Demand).Branch(1).Comp(1).NodeNumIn = CWCoil.WaterInletNodeNum;
    CWLoop.LoopSide(DataPlant::LoopSideLocation::Demand).Branch(1).Comp(1).NodeNumOut = CWCoil.WaterOutletNodeNum;
    auto &CWLoopSizingData(state->dataSize->PlantSizData(2));
    // Chilled Water Loop
    CWLoop.PlantSizNum = 2;
    CWLoopSizingData.PlantLoopName = CWLoop.Name;
    CWLoopSizingData.DesVolFlowRate = 1.0;
    CWLoopSizingData.DeltaT = 5.6;
    state->dataPlnt->PlantFirstSizesOkayToFinalize = true;
    // hot water plant loop
    auto &HWLoop(state->dataPlnt->PlantLoop(1));
    HWLoop.Name = "HotWaterLoop";
    HWLoop.FluidName = "WATER";
    HWLoop.glycol = Fluid::GetWater(*state);
    HWLoop.LoopSide(DataPlant::LoopSideLocation::Demand).Branch(1).Comp(1).Name = HWCoil.Name;
    HWLoop.LoopSide(DataPlant::LoopSideLocation::Demand).Branch(1).Comp(1).Type = DataPlant::PlantEquipmentType::CoilWaterSimpleHeating;
    HWLoop.LoopSide(DataPlant::LoopSideLocation::Demand).Branch(1).Comp(1).NodeNumIn = HWCoil.WaterInletNodeNum;
    HWLoop.LoopSide(DataPlant::LoopSideLocation::Demand).Branch(1).Comp(1).NodeNumOut = HWCoil.WaterOutletNodeNum;
    auto &HWLoopSizingData(state->dataSize->PlantSizData(1));
    // Hot Water Loop
    HWLoop.PlantSizNum = 1;
    HWLoopSizingData.PlantLoopName = HWLoop.Name;
    HWLoopSizingData.DesVolFlowRate = 1.0;
    HWLoopSizingData.DeltaT = 10.0;
    state->dataPlnt->PlantFirstSizesOkayToFinalize = true;
    state->dataGlobal->BeginEnvrnFlag = true;
    state->dataGlobal->DoingSizing = true;
    state->dataHVACGlobal->TurnFansOff = false;
    state->dataHVACGlobal->TurnFansOn = true;
    state->dataEnvrn->Month = 1;
    state->dataEnvrn->DayOfMonth = 21;
    state->dataGlobal->HourOfDay = 1;
    state->dataEnvrn->DSTIndicator = 0;
    state->dataEnvrn->DayOfWeek = 2;
    state->dataEnvrn->HolidayIndex = 0;
    state->dataEnvrn->DayOfYear_Schedule = General::OrdinalDay(state->dataEnvrn->Month, state->dataEnvrn->DayOfMonth, 1);
    Sched::UpdateScheduleVals(*state);

    state->dataSize->ZoneEqSizing.allocate(1);
    auto &zoneEqSizing(state->dataSize->ZoneEqSizing(1));
    zoneEqSizing.SizingMethod.allocate(HVAC::NumOfSizingTypes);
    state->dataZoneEnergyDemand->CurDeadBandOrSetback.allocate(1);
    state->dataZoneEnergyDemand->CurDeadBandOrSetback(1) = false;
    state->dataHeatBalFanSys->TempControlType.allocate(1);
    state->dataHeatBalFanSys->TempControlType(1) = HVAC::SetptType::DualHeatCool;
    state->dataSize->ZoneSizingRunDone = true;

    state->dataSize->FinalZoneSizing.allocate(1);
    auto &finalZoneSizing(state->dataSize->FinalZoneSizing(1));
    finalZoneSizing.DesCoolVolFlow = DesignHeatAirVolFlow;
    finalZoneSizing.DesCoolMassFlow = finalZoneSizing.DesCoolVolFlow * state->dataEnvrn->StdRhoAir;
    finalZoneSizing.DesHeatVolFlow = DesignCoolAirVolFlow;
    finalZoneSizing.DesHeatMassFlow = finalZoneSizing.DesHeatVolFlow * state->dataEnvrn->StdRhoAir;
    finalZoneSizing.ZoneTempAtHeatPeak = 20.0;
    finalZoneSizing.ZoneRetTempAtHeatPeak = finalZoneSizing.ZoneTempAtHeatPeak;
    zoneEqSizing.ATMixerHeatPriDryBulb = 4.0;
    finalZoneSizing.ZoneHumRatAtHeatPeak = 0.075;
    zoneEqSizing.ATMixerHeatPriHumRat = 0.005;
    finalZoneSizing.ZoneTempAtCoolPeak = 24.0;
    finalZoneSizing.ZoneRetTempAtCoolPeak = finalZoneSizing.ZoneTempAtCoolPeak;
    zoneEqSizing.ATMixerCoolPriDryBulb = 30.0;
    finalZoneSizing.ZoneHumRatAtCoolPeak = 0.0075;
    zoneEqSizing.ATMixerCoolPriHumRat = 0.0095;
    finalZoneSizing.DesCoolLoad = 10000.0;
    finalZoneSizing.DesHeatLoad = 10000.0;
    finalZoneSizing.CoolDesTemp = 12.8;
    finalZoneSizing.CoolDesHumRat = 0.0085;
    finalZoneSizing.HeatDesTemp = 40.0;
    finalZoneSizing.HeatDesHumRat = 0.0075;

    // heating mode tests
    state->dataFanCoilUnits->CoolingLoad = false;
    state->dataFanCoilUnits->HeatingLoad = true;
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand.allocate(1);
    auto &zoneSysEnergyDemand(state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1));
    auto &zoneEquipConfig(state->dataZoneEquip->ZoneEquipConfig(1));

    // set zone air node conditions
    state->dataLoopNodes->Node(zoneEquipConfig.ZoneNode).Temp = 20.0;
    state->dataLoopNodes->Node(zoneEquipConfig.ZoneNode).HumRat = 0.0075;
    state->dataLoopNodes->Node(zoneEquipConfig.ZoneNode).Enthalpy = Psychrometrics::PsyHFnTdbW(
        state->dataLoopNodes->Node(zoneEquipConfig.ZoneNode).Temp, state->dataLoopNodes->Node(zoneEquipConfig.ZoneNode).HumRat);
    // primary air conditions
    state->dataEnvrn->OutDryBulbTemp = 5.0;
    state->dataEnvrn->OutHumRat = 0.005;
    state->dataEnvrn->OutEnthalpy = Psychrometrics::PsyHFnTdbW(state->dataEnvrn->OutDryBulbTemp, state->dataEnvrn->OutHumRat);
    // primary air condition set at outdoor air condition
    state->dataLoopNodes->Node(thisFanCoil.ATMixerPriNode).Temp = state->dataEnvrn->OutDryBulbTemp;
    state->dataLoopNodes->Node(thisFanCoil.ATMixerPriNode).HumRat = state->dataEnvrn->OutHumRat;
    state->dataLoopNodes->Node(thisFanCoil.ATMixerPriNode).Enthalpy = state->dataEnvrn->OutEnthalpy;
    // initialize air terminal mixer primary air mass flow rate
    PrimaryAirMassFlowRate = 0.1;
    state->dataLoopNodes->Node(thisFanCoil.ATMixerPriNode).MassFlowRate = PrimaryAirMassFlowRate;
    state->dataLoopNodes->Node(thisFanCoil.ATMixerPriNode).MassFlowRateMaxAvail = PrimaryAirMassFlowRate;
    // set secondary air (recirculating air) conditions to zone air node
    state->dataLoopNodes->Node(thisATMixer.SecInNode).Temp = state->dataLoopNodes->Node(zoneEquipConfig.ZoneNode).Temp;
    state->dataLoopNodes->Node(thisATMixer.SecInNode).HumRat = state->dataLoopNodes->Node(zoneEquipConfig.ZoneNode).HumRat;
    state->dataLoopNodes->Node(thisATMixer.SecInNode).Enthalpy = state->dataLoopNodes->Node(zoneEquipConfig.ZoneNode).Enthalpy;

    state->dataGlobal->BeginEnvrnFlag = true;
    state->dataSize->ZoneEqFanCoil = true;

    // set predicted heating load
    zoneSysEnergyDemand.RemainingOutputReqToCoolSP = 0;
    zoneSysEnergyDemand.RemainingOutputReqToHeatSP = 4000.0;
    zoneSysEnergyDemand.RemainingOutputRequired = 4000.0;
    QZnReq = zoneSysEnergyDemand.RemainingOutputRequired;
    QUnitOut = 0.0;
    QLatOut = 0.0;

    InitFanCoilUnits(*state, FanCoilNum, ZoneNum);
    EXPECT_EQ(state->dataLoopNodes->Node(thisFanCoil.AirInNode).MassFlowRateMinAvail, 0.0); // check init value
    Sim4PipeFanCoil(*state, FanCoilNum, ZoneNum, FirstHVACIteration, QUnitOut, QLatOut);
    SecondaryAirMassFlowRate = state->dataLoopNodes->Node(thisFanCoil.AirInNode).MassFlowRate - PrimaryAirMassFlowRate;
    // check results in heating mode operation
    EXPECT_NEAR(QZnReq, QUnitOut, 5.0);
    EXPECT_NEAR(thisFanCoil.PLR, 0.18700, 0.0001); // Why was the precision on this 0.00001?
    // check mass flow rates
    EXPECT_NEAR(PrimaryAirMassFlowRate, 0.1, 0.0001); // user input
    EXPECT_NEAR(SecondaryAirMassFlowRate, 0.035129, 0.00003);
    EXPECT_NEAR(state->dataLoopNodes->Node(thisFanCoil.AirInNode).MassFlowRate, thisFan->inletAirMassFlowRate, 0.000001);
    EXPECT_NEAR(state->dataLoopNodes->Node(thisFanCoil.ATMixerPriNode).MassFlowRate, 0.1, 0.000001);
    EXPECT_NEAR(state->dataLoopNodes->Node(thisFanCoil.ATMixerSecNode).MassFlowRate, 0.035129, 0.00003);
    EXPECT_NEAR(state->dataLoopNodes->Node(thisFanCoil.ATMixerOutNode).MassFlowRate, 0.135129, 0.00003);

    // set zone air node conditions
    state->dataLoopNodes->Node(zoneEquipConfig.ZoneNode).Temp = 24.0;
    state->dataLoopNodes->Node(zoneEquipConfig.ZoneNode).HumRat = 0.0085;
    state->dataLoopNodes->Node(zoneEquipConfig.ZoneNode).Enthalpy = Psychrometrics::PsyHFnTdbW(
        state->dataLoopNodes->Node(zoneEquipConfig.ZoneNode).Temp, state->dataLoopNodes->Node(zoneEquipConfig.ZoneNode).HumRat);
    // primary air conditions
    state->dataEnvrn->OutDryBulbTemp = 28.0;
    state->dataEnvrn->OutHumRat = 0.0095;
    state->dataEnvrn->OutEnthalpy = Psychrometrics::PsyHFnTdbW(state->dataEnvrn->OutDryBulbTemp, state->dataEnvrn->OutHumRat);
    // primary air condition set at outdoor air condition
    state->dataLoopNodes->Node(thisFanCoil.ATMixerPriNode).Temp = state->dataEnvrn->OutDryBulbTemp;
    state->dataLoopNodes->Node(thisFanCoil.ATMixerPriNode).HumRat = state->dataEnvrn->OutHumRat;
    state->dataLoopNodes->Node(thisFanCoil.ATMixerPriNode).Enthalpy = state->dataEnvrn->OutEnthalpy;
    // initialize air terminal mixer primary air mass flow rate
    PrimaryAirMassFlowRate = 0.2;
    state->dataLoopNodes->Node(thisFanCoil.ATMixerPriNode).MassFlowRate = PrimaryAirMassFlowRate;
    state->dataLoopNodes->Node(thisFanCoil.ATMixerPriNode).MassFlowRateMaxAvail = PrimaryAirMassFlowRate;

    state->dataSize->SysSizingRunDone = true;
    state->dataSize->ZoneSizingRunDone = true;
    state->dataGlobal->SysSizingCalc = true;
    state->dataGlobal->BeginEnvrnFlag = true;
    // set predicted cooling load
    zoneSysEnergyDemand.RemainingOutputReqToHeatSP = 0.0;
    zoneSysEnergyDemand.RemainingOutputReqToCoolSP = -5000.0;
    zoneSysEnergyDemand.RemainingOutputRequired = -5000.0;
    QZnReq = zoneSysEnergyDemand.RemainingOutputRequired;
    QUnitOut = 0.0;
    QLatOut = 0.0;
    InitFanCoilUnits(*state, FanCoilNum, ZoneNum);
    EXPECT_EQ(state->dataLoopNodes->Node(thisFanCoil.AirInNode).MassFlowRateMinAvail, 0.0); // check init value
    Sim4PipeFanCoil(*state, FanCoilNum, ZoneNum, FirstHVACIteration, QUnitOut, QLatOut);
    SecondaryAirMassFlowRate = state->dataLoopNodes->Node(thisFanCoil.AirInNode).MassFlowRate - PrimaryAirMassFlowRate;
    // check results in cooling mode operation
    EXPECT_NEAR(QZnReq, QUnitOut, 5.0);
    // Also, tolerance was 0.00001, why?  Why is tolerance of less
    // than 1/10th or even 1/100th of a percent needed on anything?
    // Also, the tolerance of finding water coil UA is 0.001, how
    // can this be lower?
    EXPECT_NEAR(thisFanCoil.PLR, 0.76235, 0.001); // Was 0.78843

    // check mass flow rates
    EXPECT_NEAR(PrimaryAirMassFlowRate, 0.2, 0.000001);
    // Tolerance here was 0.000001, why?  Why is tolerance of less
    // than 1/10th or even 1/100th of a percent needed on anything?
    EXPECT_NEAR(SecondaryAirMassFlowRate, 0.350865, 0.001);
    EXPECT_NEAR(state->dataLoopNodes->Node(thisFanCoil.AirInNode).MassFlowRate, thisFan->inletAirMassFlowRate, 0.000001);
    EXPECT_NEAR(state->dataLoopNodes->Node(thisFanCoil.ATMixerPriNode).MassFlowRate, 0.2, 0.0001);
    EXPECT_NEAR(state->dataLoopNodes->Node(thisFanCoil.ATMixerSecNode).MassFlowRate, 0.350865, 0.0005); // Was 0.369714
    EXPECT_NEAR(state->dataLoopNodes->Node(thisFanCoil.ATMixerOutNode).MassFlowRate, 0.550865, 0.0005); // Was 0.569714
}

TEST_F(EnergyPlusFixture, AirTerminalSingleDuctMixer_FCU_NightCycleTest)
{

    std::string const idf_objects = delimited_string({
        "Schedule:Compact,",
        "  FanAvailSched,           !- Name",
        "  Fraction,                !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field 1",
        "  For: AllDays,            !- Field 2",
        "  Until: 24:00,0.0;        !- Field 3",

        "ScheduleTypeLimits,",
        "  Fraction,                !- Name",
        "  0.0,                     !- Lower Limit Value",
        "  1.0,                     !- Upper Limit Value",
        "  Continuous;              !- Numeric Type",

        "Zone,",
        "  Zone One,                !- Name",
        "  0.0,                     !- Direction of Relative North {deg}",
        "  0.0,                     !- X Origin {m}",
        "  0.0,                     !- Y Origin {m}",
        "  0.0,                     !- Z Origin {m}",
        "  ,                        !- Type",
        "  1;                       !- Multiplier",

        "ZoneHVAC:FourPipeFanCoil,",
        "  FCU VarFan VarFluidFlow,  !- Name",
        "  FanAvailSched,           !- Availability Schedule Name",
        "  VariableFanVariableFlow,  !- Capacity Control Method",
        "  Autosize,                !- Maximum Supply Air Flow Rate {m3/s}",
        "  ,                        !- Low Speed Supply Air Flow Ratio",
        "  ,                        !- Medium Speed Supply Air Flow Ratio",
        "  Autosize,                !- Maximum Outdoor Air Flow Rate {m3/s}",
        "  ,                        !- Outdoor Air Schedule Name",
        "  Node 5,                  !- Air Inlet Node Name",
        "  Node 63,                 !- Air Outlet Node Name",
        "  ,                        !- Outdoor Air Mixer Object Type",
        "  ,                        !- Outdoor Air Mixer Name",
        "  Fan:VariableVolume,      !- Supply Air Fan Object Type",
        "  FCU VarFan,              !- Supply Air Fan Name",
        "  Coil:Cooling:Water,      !- Cooling Coil Object Type",
        "  FCU Cooling Coil,        !- Cooling Coil Name",
        "  Autosize,                !- Maximum Cold Water Flow Rate {m3/s}",
        "  ,                        !- Minimum Cold Water Flow Rate {m3/s}",
        "  ,                        !- Cooling Convergence Tolerance",
        "  Coil:Heating:Water,      !- Heating Coil Object Type",
        "  FCU Heating Coil,        !- Heating Coil Name",
        "  Autosize,                !- Maximum Hot Water Flow Rate {m3/s}",
        "  ,                        !- Minimum Hot Water Flow Rate {m3/s}",
        "  ,                        !- Heating Convergence Tolerance",
        "  ,                        !- Availability Manager List Name",
        "  ,                        !- Design Specification ZoneHVAC Sizing Object Name",
        "  ,                        !- Supply Air Fan Operating Mode Schedule Name",
        "  Autosize,                !- Minimum Supply Air Temperature in Cooling Mode {C}",
        "  Autosize;                !- Maximum Supply Air Temperature in Heating Mode {C}",

        "AirTerminal:SingleDuct:Mixer,",
        "  Inlet Side Mixer,        !- Name",
        "  ZoneHVAC:FourPipeFanCoil,  !- ZoneHVAC Unit Object Type",
        "  FCU VarFan VarFluidFlow,  !- ZoneHVAC Unit Object Name",
        "  Node 5,                  !- Mixer Outlet Node Name",
        "  Node 62,                 !- Mixer Primary Air Inlet Node Name",
        "  Node 64,                 !- Mixer Secondary Air Inlet Node Name",
        "  InletSide;               !- Mixer Connection Type",

        "ZoneHVAC:AirDistributionUnit,",
        "  ADU Inlet Side Mixer,    !- Name",
        "  Node 5,                  !- Air Distribution Unit Outlet Node Name",
        "  AirTerminal:SingleDuct:Mixer,  !- Air Terminal Object Type",
        "  Inlet Side Mixer;        !- Air Terminal Name",

        "ZoneHVAC:EquipmentList,",
        "  Zone one Equipment List,  !- Name",
        "  SequentialLoad,          !- Load Distribution Scheme",
        "  ZoneHVAC:AirDistributionUnit,  !- Zone Equipment 1 Object Type",
        "  ADU Inlet Side Mixer,    !- Zone Equipment 1 Name",
        "  1,                       !- Zone Equipment 1 Cooling Sequence",
        "  1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 1 Sequential Cooling Fraction Schedule Name",
        "  ,                        !- Zone Equipment 1 Sequential Heating Fraction Schedule Name",
        "  ZoneHVAC:FourPipeFanCoil,  !- Zone Equipment 2 Object Type",
        "  FCU VarFan VarFluidFlow,  !- Zone Equipment 2 Name",
        "  2,                       !- Zone Equipment 2 Cooling Sequence",
        "  2,                       !- Zone Equipment 2 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 2 Sequential Cooling Fraction Schedule Name",
        "  ;                        !- Zone Equipment 2 Sequential Heating Fraction Schedule Name",

        "ZoneHVAC:EquipmentConnections,",
        "  Zone One,                !- Zone Name",
        "  Zone one Equipment List,  !- Zone Conditioning Equipment List Name",
        "  Node 63,                 !- Zone Air Inlet Node or NodeList Name",
        "  Node 64,                 !- Zone Air Exhaust Node or NodeList Name",
        "  Zone one Air Node,       !- Zone Air Node Name",
        "  Node 61;                 !- Zone Return Air Node or NodeList Name",

        "Fan:VariableVolume,",
        "  FCU VarFan,              !- Name",
        "  Constant-1.0,            !- Availability Schedule Name",
        "  0.6045,                  !- Fan Total Efficiency",
        "  600.0,                   !- Pressure Rise {Pa}",
        "  Autosize,                !- Maximum Flow Rate {m3/s}",
        "  FixedFlowRate,           !- Fan Power Minimum Flow Rate Input Method",
        "  0.0,                     !- Fan Power Minimum Flow Fraction",
        "  0.0,                     !- Fan Power Minimum Air Flow Rate {m3/s}",
        "  0.93,                    !- Motor Efficiency",
        "  1.0,                     !- Motor In Airstream Fraction",
        "  0.040759894,             !- Fan Power Coefficient 1",
        "  0.08804497,              !- Fan Power Coefficient 2",
        "  -0.07292612,             !- Fan Power Coefficient 3",
        "  0.943739823,             !- Fan Power Coefficient 4",
        "  0.0,                     !- Fan Power Coefficient 5",
        "  Node 5,                  !- Air Inlet Node Name",
        "  FCU Fan Outlet Node,     !- Air Outlet Node Name",
        "  General;                 !- End-Use Subcategory",

        "Coil:Cooling:Water,",
        "  FCU Cooling Coil,        !- Name",
        "  Constant-1.0,            !- Availability Schedule Name",
        "  Autosize,                !- Design Water Flow Rate {m3/s}",
        "  Autosize,                !- Design Air Flow Rate {m3/s}",
        "  Autosize,                !- Design Inlet Water Temperature {C}",
        "  Autosize,                !- Design Inlet Air Temperature {C}",
        "  Autosize,                !- Design Outlet Air Temperature {C}",
        "  Autosize,                !- Design Inlet Air Humidity Ratio {kgWater/kgDryAir}",
        "  Autosize,                !- Design Outlet Air Humidity Ratio {kgWater/kgDryAir}",
        "  Node 66,                 !- Water Inlet Node Name",
        "  Node 68,                 !- Water Outlet Node Name",
        "  FCU Fan Outlet Node,     !- Air Inlet Node Name",
        "  FCU CCoil Outlet Node,   !- Air Outlet Node Name",
        "  SimpleAnalysis,          !- Type of Analysis",
        "  CrossFlow;               !- Heat Exchanger Configuration",

        "Coil:Heating:Water,",
        "  FCU Heating Coil,        !- Name",
        "  Constant-1.0,            !- Availability Schedule Name",
        "  Autosize,                !- U-Factor Times Area Value {W/K}",
        "  Autosize,                !- Maximum Water Flow Rate {m3/s}",
        "  Node 67,                 !- Water Inlet Node Name",
        "  Node 65,                 !- Water Outlet Node Name",
        "  FCU CCoil Outlet Node,   !- Air Inlet Node Name",
        "  Node 63,                 !- Air Outlet Node Name",
        "  UFactorTimesAreaAndDesignWaterFlowRate,  !- Performance Input Method",
        "  Autosize,                !- Rated Capacity {W}",
        "  82.2,                    !- Rated Inlet Water Temperature {C}",
        "  16.6,                    !- Rated Inlet Air Temperature {C}",
        "  71.1,                    !- Rated Outlet Water Temperature {C}",
        "  32.2,                    !- Rated Outlet Air Temperature {C}",
        "  0.5;                     !- Rated Ratio for Air and Water Convection",

        "AvailabilityManagerAssignmentList,",
        "  Zone Availability Manager,  !- Name",
        "  AvailabilityManager:NightCycle,  !- Availability Manager 1 Object Type",
        "  NightCycle AvailMgr;     !- Availability Manager 1 Name",

        "AvailabilityManager:NightCycle,",
        "  NightCycle AvailMgr,     !- Name",
        "  Constant-1.0,            !- Applicability Schedule Name",
        "  FanAvailSched,           !- Fan Schedule Name",
        "  CycleOnControlZone,      !- Control Type",
        "  0.2,                     !- Thermostat Tolerance {deltaC}",
        "  ThermostatWithMinimumRunTime,  !- Cycling Run Time Control Type",
        "  300.0,                   !- Cycling Run Time {s}",
        "  Zone one;                !- Control Zone or Zone List Name",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    bool ErrorsFound(false);
    bool FirstHVACIteration(false);
    Real64 PrimaryAirMassFlowRate(0.0);
    Real64 DesignHeatAirVolFlow(0.50);
    Real64 DesignCoolAirVolFlow(0.60);
    Real64 QUnitOut(0.0);
    Real64 QLatOut(0.0);
    Real64 QZnReq(0.0);
    int ZoneNum(1);
    int FanCoilNum(1);

    state->dataSize->CurZoneEqNum = 1;
    state->dataEnvrn->OutBaroPress = 101325.0;
    state->dataEnvrn->StdRhoAir = Psychrometrics::PsyRhoAirFnPbTdbW(*state, state->dataEnvrn->OutBaroPress, 20.0, 0.0);
    state->dataWaterCoils->GetWaterCoilsInputFlag = true;
    state->dataGlobal->TimeStepsInHour = 1;
    state->dataGlobal->TimeStep = 1;
    state->dataGlobal->MinutesInTimeStep = 60;
    state->init_state(*state);

    GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    GetZoneEquipmentData(*state);
    GetFanCoilUnits(*state);
    Avail::GetSysAvailManagerInputs(*state);

    auto &thisFanCoil(state->dataFanCoilUnits->FanCoil(1));
    auto &thisATMixer(state->dataSingleDuct->SysATMixer(1));
    auto &thisAvaiManager(state->dataAvail->NightCycleData(1));

    // get input test for terminal air single duct mixer on inlet side of PTAC
    ASSERT_EQ(1, state->dataSingleDuct->NumATMixers);
    EXPECT_EQ("INLET SIDE MIXER", thisATMixer.Name);                                                    // single duct air terminal mixer name
    EXPECT_EQ((int)HVAC::MixerType::InletSide, (int)thisATMixer.type);                                  // air terminal mixer connection type
    EXPECT_EQ("AIRTERMINAL:SINGLEDUCT:MIXER", state->dataDefineEquipment->AirDistUnit(1).EquipType(1)); // Air distribution unit equipment type
    EXPECT_EQ((int)HVAC::FanType::VAV, (int)thisFanCoil.fanType);
    EXPECT_EQ("COIL:COOLING:WATER", thisFanCoil.CCoilType);
    EXPECT_EQ("FCU COOLING COIL", thisFanCoil.CCoilName);
    EXPECT_EQ("COIL:HEATING:WATER", thisFanCoil.HCoilType);
    EXPECT_EQ("FCU HEATING COIL", thisFanCoil.HCoilName);
    EXPECT_EQ("NIGHTCYCLE AVAILMGR", thisAvaiManager.Name);
    EXPECT_EQ((int)Avail::ManagerType::NightCycle, (int)thisAvaiManager.type);

    state->dataPlnt->TotNumLoops = 2;
    state->dataPlnt->PlantLoop.allocate(state->dataPlnt->TotNumLoops);
    state->dataSize->NumPltSizInput = 2;
    state->dataSize->PlantSizData.allocate(state->dataSize->NumPltSizInput);
    // chilled water coil
    auto &CWCoil(state->dataWaterCoils->WaterCoil(2));
    thisFanCoil.CCoilName_Index = 2;
    state->dataLoopNodes->Node(CWCoil.WaterInletNodeNum).Temp = 6.0;
    CWCoil.WaterPlantLoc.loopNum = 2;
    CWCoil.WaterPlantLoc.loopSideNum = DataPlant::LoopSideLocation::Demand;
    CWCoil.WaterPlantLoc.branchNum = 1;
    CWCoil.WaterPlantLoc.compNum = 1;
    // hot water coil
    auto &HWCoil(state->dataWaterCoils->WaterCoil(1));
    thisFanCoil.HCoilName_Index = 1;
    state->dataLoopNodes->Node(HWCoil.WaterInletNodeNum).Temp = 60.0;
    HWCoil.WaterPlantLoc.loopNum = 1;
    HWCoil.WaterPlantLoc.loopSideNum = DataPlant::LoopSideLocation::Demand;
    HWCoil.WaterPlantLoc.branchNum = 1;
    HWCoil.WaterPlantLoc.compNum = 1;
    for (int l = 1; l <= state->dataPlnt->TotNumLoops; ++l) {

        auto &loopside(state->dataPlnt->PlantLoop(l).LoopSide(DataPlant::LoopSideLocation::Demand));
        loopside.TotalBranches = 1;
        loopside.Branch.allocate(1);
        auto &loopsidebranch(state->dataPlnt->PlantLoop(l).LoopSide(DataPlant::LoopSideLocation::Demand).Branch(1));
        loopsidebranch.TotalComponents = 1;
        loopsidebranch.Comp.allocate(1);
    }
    // chilled water plant loop
    auto &CWLoop(state->dataPlnt->PlantLoop(2));
    CWLoop.Name = "ChilledWaterLoop";
    CWLoop.FluidName = "WATER";
    CWLoop.glycol = Fluid::GetWater(*state);
    CWLoop.LoopSide(DataPlant::LoopSideLocation::Demand).Branch(1).Comp(1).Name = CWCoil.Name;
    CWLoop.LoopSide(DataPlant::LoopSideLocation::Demand).Branch(1).Comp(1).Type = DataPlant::PlantEquipmentType::CoilWaterCooling;
    CWLoop.LoopSide(DataPlant::LoopSideLocation::Demand).Branch(1).Comp(1).NodeNumIn = CWCoil.WaterInletNodeNum;
    CWLoop.LoopSide(DataPlant::LoopSideLocation::Demand).Branch(1).Comp(1).NodeNumOut = CWCoil.WaterOutletNodeNum;
    auto &CWLoopSizingData(state->dataSize->PlantSizData(2));
    // Chilled Water Loop
    CWLoop.PlantSizNum = 2;
    CWLoopSizingData.PlantLoopName = CWLoop.Name;
    CWLoopSizingData.DesVolFlowRate = 1.0;
    CWLoopSizingData.DeltaT = 5.6;
    state->dataPlnt->PlantFirstSizesOkayToFinalize = true;
    // hot water plant loop
    auto &HWLoop(state->dataPlnt->PlantLoop(1));
    HWLoop.Name = "HotWaterLoop";
    HWLoop.FluidName = "WATER";
    HWLoop.glycol = Fluid::GetWater(*state);
    HWLoop.LoopSide(DataPlant::LoopSideLocation::Demand).Branch(1).Comp(1).Name = HWCoil.Name;
    HWLoop.LoopSide(DataPlant::LoopSideLocation::Demand).Branch(1).Comp(1).Type = DataPlant::PlantEquipmentType::CoilWaterSimpleHeating;
    HWLoop.LoopSide(DataPlant::LoopSideLocation::Demand).Branch(1).Comp(1).NodeNumIn = HWCoil.WaterInletNodeNum;
    HWLoop.LoopSide(DataPlant::LoopSideLocation::Demand).Branch(1).Comp(1).NodeNumOut = HWCoil.WaterOutletNodeNum;
    auto &HWLoopSizingData(state->dataSize->PlantSizData(1));
    // Hot Water Loop
    HWLoop.PlantSizNum = 1;
    HWLoopSizingData.PlantLoopName = HWLoop.Name;
    HWLoopSizingData.DesVolFlowRate = 1.0;
    HWLoopSizingData.DeltaT = 10.0;
    state->dataPlnt->PlantFirstSizesOkayToFinalize = true;
    state->dataGlobal->BeginEnvrnFlag = true;
    state->dataGlobal->DoingSizing = true;
    state->dataHVACGlobal->TurnFansOff = false;
    state->dataHVACGlobal->TurnFansOn = false;
    state->dataEnvrn->Month = 1;
    state->dataEnvrn->DayOfMonth = 21;
    state->dataGlobal->HourOfDay = 1;
    state->dataEnvrn->DSTIndicator = 0;
    state->dataEnvrn->DayOfWeek = 2;
    state->dataEnvrn->HolidayIndex = 0;
    state->dataEnvrn->DayOfYear_Schedule = General::OrdinalDay(state->dataEnvrn->Month, state->dataEnvrn->DayOfMonth, 1);
    Sched::UpdateScheduleVals(*state);

    state->dataSize->ZoneEqSizing.allocate(1);
    auto &zoneEqSizing(state->dataSize->ZoneEqSizing(1));
    zoneEqSizing.SizingMethod.allocate(HVAC::NumOfSizingTypes);
    state->dataZoneEnergyDemand->CurDeadBandOrSetback.allocate(1);
    state->dataZoneEnergyDemand->CurDeadBandOrSetback(1) = false;
    state->dataHeatBalFanSys->TempControlType.allocate(1);
    state->dataHeatBalFanSys->TempControlType(1) = HVAC::SetptType::DualHeatCool;
    state->dataSize->ZoneSizingRunDone = true;

    state->dataSize->FinalZoneSizing.allocate(1);
    auto &finalZoneSizing(state->dataSize->FinalZoneSizing(1));
    finalZoneSizing.DesCoolVolFlow = DesignHeatAirVolFlow;
    finalZoneSizing.DesCoolMassFlow = finalZoneSizing.DesCoolVolFlow * state->dataEnvrn->StdRhoAir;
    finalZoneSizing.DesHeatVolFlow = DesignCoolAirVolFlow;
    finalZoneSizing.DesHeatMassFlow = finalZoneSizing.DesHeatVolFlow * state->dataEnvrn->StdRhoAir;
    finalZoneSizing.ZoneTempAtHeatPeak = 20.0;
    finalZoneSizing.ZoneRetTempAtHeatPeak = finalZoneSizing.ZoneTempAtHeatPeak;
    zoneEqSizing.ATMixerHeatPriDryBulb = 4.0;
    finalZoneSizing.ZoneHumRatAtHeatPeak = 0.075;
    zoneEqSizing.ATMixerHeatPriHumRat = 0.005;
    finalZoneSizing.ZoneTempAtCoolPeak = 24.0;
    finalZoneSizing.ZoneRetTempAtCoolPeak = finalZoneSizing.ZoneTempAtCoolPeak;
    zoneEqSizing.ATMixerCoolPriDryBulb = 30.0;
    finalZoneSizing.ZoneHumRatAtCoolPeak = 0.0075;
    zoneEqSizing.ATMixerCoolPriHumRat = 0.0095;
    finalZoneSizing.DesCoolLoad = 10000.0;
    finalZoneSizing.DesHeatLoad = 10000.0;
    finalZoneSizing.CoolDesTemp = 12.8;
    finalZoneSizing.CoolDesHumRat = 0.0085;
    finalZoneSizing.HeatDesTemp = 40.0;
    finalZoneSizing.HeatDesHumRat = 0.0075;

    // heating mode tests
    state->dataFanCoilUnits->CoolingLoad = false;
    state->dataFanCoilUnits->HeatingLoad = true;
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand.allocate(1);
    auto &zoneSysEnergyDemand(state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1));
    auto &zoneEquipConfig(state->dataZoneEquip->ZoneEquipConfig(1));

    // set zone air node conditions
    state->dataLoopNodes->Node(zoneEquipConfig.ZoneNode).Temp = 20.0;
    state->dataLoopNodes->Node(zoneEquipConfig.ZoneNode).HumRat = 0.0075;
    state->dataLoopNodes->Node(zoneEquipConfig.ZoneNode).Enthalpy = Psychrometrics::PsyHFnTdbW(
        state->dataLoopNodes->Node(zoneEquipConfig.ZoneNode).Temp, state->dataLoopNodes->Node(zoneEquipConfig.ZoneNode).HumRat);
    // primary air conditions
    state->dataEnvrn->OutDryBulbTemp = 5.0;
    state->dataEnvrn->OutHumRat = 0.005;
    state->dataEnvrn->OutEnthalpy = Psychrometrics::PsyHFnTdbW(state->dataEnvrn->OutDryBulbTemp, state->dataEnvrn->OutHumRat);
    // primary air condition set at outdoor air condition
    state->dataLoopNodes->Node(thisFanCoil.ATMixerPriNode).Temp = state->dataEnvrn->OutDryBulbTemp;
    state->dataLoopNodes->Node(thisFanCoil.ATMixerPriNode).HumRat = state->dataEnvrn->OutHumRat;
    state->dataLoopNodes->Node(thisFanCoil.ATMixerPriNode).Enthalpy = state->dataEnvrn->OutEnthalpy;
    // initialize air terminal mixer primary air mass flow rate
    PrimaryAirMassFlowRate = 0.1;
    state->dataLoopNodes->Node(thisFanCoil.ATMixerPriNode).MassFlowRate = PrimaryAirMassFlowRate;
    state->dataLoopNodes->Node(thisFanCoil.ATMixerPriNode).MassFlowRateMaxAvail = PrimaryAirMassFlowRate;
    // set secondary air (recirculating air) conditions to zone air node
    state->dataLoopNodes->Node(thisATMixer.SecInNode).Temp = state->dataLoopNodes->Node(zoneEquipConfig.ZoneNode).Temp;
    state->dataLoopNodes->Node(thisATMixer.SecInNode).HumRat = state->dataLoopNodes->Node(zoneEquipConfig.ZoneNode).HumRat;
    state->dataLoopNodes->Node(thisATMixer.SecInNode).Enthalpy = state->dataLoopNodes->Node(zoneEquipConfig.ZoneNode).Enthalpy;

    state->dataGlobal->BeginEnvrnFlag = true;
    state->dataSize->ZoneEqFanCoil = true;
    // check availability manager Night Cycle parameters
    EXPECT_EQ((int)Avail::CyclingRunTimeControl::ThermostatWithMinimumRunTime, (int)state->dataAvail->NightCycleData(1).cyclingRunTimeControl);
    EXPECT_EQ((int)Avail::Status::NoAction, (int)state->dataAvail->NightCycleData(1).availStatus);

    // set predicted heating load
    zoneSysEnergyDemand.RemainingOutputReqToCoolSP = 4000.0;
    zoneSysEnergyDemand.RemainingOutputReqToHeatSP = 4000.0;
    zoneSysEnergyDemand.RemainingOutputRequired = 4000.0;
    QZnReq = zoneSysEnergyDemand.RemainingOutputRequired;
    QUnitOut = 0.0;
    QLatOut = 0.0;
    InitFanCoilUnits(*state, FanCoilNum, ZoneNum);
    Sim4PipeFanCoil(*state, FanCoilNum, ZoneNum, FirstHVACIteration, QUnitOut, QLatOut);
    // check results when the fan coil unit is not available
    EXPECT_NEAR(0.0, QUnitOut, 0.1); // fan coil unit is off
    EXPECT_NEAR(thisFanCoil.PLR, 0.0, 0.00001);
    EXPECT_NEAR(state->dataLoopNodes->Node(thisFanCoil.AirInNode).MassFlowRate, 0.0, 0.000001);

    int SysAvailNum = 1;
    int PriAirSysNum = 0;
    int constexpr ZoneEquipType = 1;
    int constexpr CompNum = 1;
    // current time is within the run time period, starting time is less than stopping time
    state->dataGlobal->SimTimeSteps = 0;
    state->dataAvail->ZoneComp(1).ZoneCompAvailMgrs(1).StartTime = 0.0;
    state->dataAvail->ZoneComp(1).ZoneCompAvailMgrs(1).StopTime = 4.0;
    state->dataAvail->NightCycleData(1).availStatus = Avail::Status::NoAction;
    // run CalcNCycSysAvailMgr to the availability of the fan coil unit on
    Avail::Status availStatus = Avail::CalcNCycSysAvailMgr(*state, SysAvailNum, PriAirSysNum, ZoneEquipType, CompNum);
    // check that the NightCycle has turned on the equipment
    EXPECT_EQ((int)Avail::Status::CycleOn, (int)availStatus);
    EXPECT_EQ((int)Avail::Status::CycleOn, (int)state->dataAvail->NightCycleData(1).availStatus);
    // set zone equipment is CyclOn based on night cycle manager status
    if (state->dataAvail->NightCycleData(1).availStatus == Avail::Status::CycleOn) {
        state->dataHVACGlobal->TurnFansOn = true;
        state->dataHVACGlobal->TurnFansOff = false;
    }
    InitFanCoilUnits(*state, FanCoilNum, ZoneNum);
    Sim4PipeFanCoil(*state, FanCoilNum, ZoneNum, FirstHVACIteration, QUnitOut, QLatOut);
    EXPECT_NEAR(QZnReq, QUnitOut, 3.0);
    EXPECT_NEAR(thisFanCoil.PLR, 0.187, 0.001);
    EXPECT_NEAR(state->dataLoopNodes->Node(thisFanCoil.AirInNode).MassFlowRate,
                thisFanCoil.PLR * state->dataLoopNodes->Node(thisFanCoil.AirInNode).MassFlowRateMax,
                0.000001);
}

} // namespace EnergyPlus
