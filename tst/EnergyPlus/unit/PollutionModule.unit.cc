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
#include <EnergyPlus/Data/EnergyPlusData.hh>
#include <EnergyPlus/OutputProcessor.hh>
#include <EnergyPlus/PollutionModule.hh>

using namespace EnergyPlus;
using namespace EnergyPlus::Pollution;

TEST_F(EnergyPlusFixture, PollutionModule_TestOutputVariables)
{
    // This unit test checks pollution module output variables

    std::string const idf_objects = delimited_string({
        "Output:EnvironmentalImpactFactors,",
        "  Monthly;                 !- Reporting Frequency",

        "EnvironmentalImpactFactors,",
        "  0.3,                     !- District Heating Water Efficiency",
        "  3.0,                     !- District Cooling COP {W/W}",
        "  0.25,                    !- District Heating Steam Conversion Efficiency",
        "  80.7272,                 !- Total Carbon Equivalent Emission Factor From N2O {kg/kg}",
        "  6.2727,                  !- Total Carbon Equivalent Emission Factor From CH4 {kg/kg}",
        "  0.2727;                  !- Total Carbon Equivalent Emission Factor From CO2 {kg/kg}",

        "! USEPA Compilation of Air Pollutant Emission Factors, AP-42, Fifth Edition",
        "! Volume I: Stationary Point and Area Sources",
        "! Supplement D, July 1998",
        "! Chapter 1 External Combustion Sources",
        "! Section 1.4 Natural Gas Combustion",
        "!",

        "FuelFactors,",
        "  NaturalGas,              !- Existing Fuel Resource Name",
        "  1.0,                     !- Source Energy Factor {J/J}",
        "  ,                        !- Source Energy Schedule Name",
        "  50.23439,                !- CO2 Emission Factor {g/MJ}",
        "  ,                        !- CO2 Emission Factor Schedule Name",
        "  0.0351641,               !- CO Emission Factor {g/MJ}",
        "  ,                        !- CO Emission Factor Schedule Name",
        "  0.000962826,             !- CH4 Emission Factor {g/MJ}",
        "  ,                        !- CH4 Emission Factor Schedule Name",
        "  0.041862,                !- NOx Emission Factor {g/MJ}",
        "  ,                        !- NOx Emission Factor Schedule Name",
        "  0.000920964,             !- N2O Emission Factor {g/MJ}",
        "  ,                        !- N2O Emission Factor Schedule Name",
        "  0.000251172,             !- SO2 Emission Factor {g/MJ}",
        "  ,                        !- SO2 Emission Factor Schedule Name",
        "  0.00318151,              !- PM Emission Factor {g/MJ}",
        "  ,                        !- PM Emission Factor Schedule Name",
        "  0.00238613,              !- PM10 Emission Factor {g/MJ}",
        "  ,                        !- PM10 Emission Factor Schedule Name",
        "  0.000795378,             !- PM2.5 Emission Factor {g/MJ}",
        "  ,                        !- PM2.5 Emission Factor Schedule Name",
        "  0,                       !- NH3 Emission Factor {g/MJ}",
        "  ,                        !- NH3 Emission Factor Schedule Name",
        "  0.00230241,              !- NMVOC Emission Factor {g/MJ}",
        "  ,                        !- NMVOC Emission Factor Schedule Name",
        "  1.08841e-07,             !- Hg Emission Factor {g/MJ}",
        "  ,                        !- Hg Emission Factor Schedule Name",
        "  2.0931e-07,              !- Pb Emission Factor {g/MJ}",
        "  ,                        !- Pb Emission Factor Schedule Name",
        "  0,                       !- Water Emission Factor {L/MJ}",
        "  ,                        !- Water Emission Factor Schedule Name",
        "  0,                       !- Nuclear High Level Emission Factor {g/MJ}",
        "  ,                        !- Nuclear High Level Emission Factor Schedule Name",
        "  0;                       !- Nuclear Low Level Emission Factor {m3/MJ}",

        "! USEPA Compilation of Air Pollutant Emission Factors, AP-42, Fifth Edition",
        "! Volume I: Stationary Point and Area Sources",
        "! Supplement B, October 1996",
        "! Chapter 3 Stationary Internal Combustion Sources",
        "! Section 3.3 Gasoline And Diesel Industrial Engines",
        "!",

        "FuelFactors,",
        "  Diesel,                  !- Existing Fuel Resource Name",
        "  1,                       !- Source Energy Factor {J/J}",
        "  ,                        !- Source Energy Schedule Name",
        "  70.50731,                !- CO2 Emission Factor {g/MJ}",
        "  ,                        !- CO2 Emission Factor Schedule Name",
        "  0.408426,                !- CO Emission Factor {g/MJ}",
        "  ,                        !- CO Emission Factor Schedule Name",
        "  0,                       !- CH4 Emission Factor {g/MJ}",
        "  ,                        !- CH4 Emission Factor Schedule Name",
        "  1.89596,                 !- NOx Emission Factor {g/MJ}",
        "  ,                        !- NOx Emission Factor Schedule Name",
        "  0,                       !- N2O Emission Factor {g/MJ}",
        "  ,                        !- N2O Emission Factor Schedule Name",
        "  0.124678,                !- SO2 Emission Factor {g/MJ}",
        "  ,                        !- SO2 Emission Factor Schedule Name",
        "  0,                       !- PM Emission Factor {g/MJ}",
        "  ,                        !- PM Emission Factor Schedule Name",
        "  0.133276,                !- PM10 Emission Factor {g/MJ}",
        "  ,                        !- PM10 Emission Factor Schedule Name",
        "  0,                       !- PM2.5 Emission Factor {g/MJ}",
        "  ,                        !- PM2.5 Emission Factor Schedule Name",
        "  0,                       !- NH3 Emission Factor {g/MJ}",
        "  ,                        !- NH3 Emission Factor Schedule Name",
        "  0.150473,                !- NMVOC Emission Factor {g/MJ}",
        "  ,                        !- NMVOC Emission Factor Schedule Name",
        "  0,                       !- Hg Emission Factor {g/MJ}",
        "  ,                        !- Hg Emission Factor Schedule Name",
        "  0,                       !- Pb Emission Factor {g/MJ}",
        "  ,                        !- Pb Emission Factor Schedule Name",
        "  0,                       !- Water Emission Factor {L/MJ}",
        "  ,                        !- Water Emission Factor Schedule Name",
        "  0,                       !- Nuclear High Level Emission Factor {g/MJ}",
        "  ,                        !- Nuclear High Level Emission Factor Schedule Name",
        "  0;                       !- Nuclear Low Level Emission Factor {m3/MJ}",

        "! USEPA Compilation of Air Pollutant Emission Factors, AP-42, Fifth Edition",
        "! Volume I: Stationary Point and Area Sources",
        "! Supplement B, October 1996",
        "! Chapter 3 Stationary Internal Combustion Sources",
        "! Section 3.3 Gasoline And Diesel Industrial Engines",
        "!",

        "FuelFactors,",
        "  Gasoline,                !- Existing Fuel Resource Name",
        "  1,                       !- Source Energy Factor {J/J}",
        "  ,                        !- Source Energy Schedule Name",
        "  66.20808,                !- CO2 Emission Factor {g/MJ}",
        "  ,                        !- CO2 Emission Factor Schedule Name",
        "  26.9561,                 !- CO Emission Factor {g/MJ}",
        "  ,                        !- CO Emission Factor Schedule Name",
        "  0,                       !- CH4 Emission Factor {g/MJ}",
        "  ,                        !- CH4 Emission Factor Schedule Name",
        "  0.700774,                !- NOx Emission Factor {g/MJ}",
        "  ,                        !- NOx Emission Factor Schedule Name",
        "  0,                       !- N2O Emission Factor {g/MJ}",
        "  ,                        !- N2O Emission Factor Schedule Name",
        "  0.0361135,               !- SO2 Emission Factor {g/MJ}",
        "  ,                        !- SO2 Emission Factor Schedule Name",
        "  0,                       !- PM Emission Factor {g/MJ}",
        "  ,                        !- PM Emission Factor Schedule Name",
        "  0.0429923,               !- PM10 Emission Factor {g/MJ}",
        "  ,                        !- PM10 Emission Factor Schedule Name",
        "  0,                       !- PM2.5 Emission Factor {g/MJ}",
        "  ,                        !- PM2.5 Emission Factor Schedule Name",
        "  0,                       !- NH3 Emission Factor {g/MJ}",
        "  ,                        !- NH3 Emission Factor Schedule Name",
        "  0.902837,                !- NMVOC Emission Factor {g/MJ}",
        "  ,                        !- NMVOC Emission Factor Schedule Name",
        "  0,                       !- Hg Emission Factor {g/MJ}",
        "  ,                        !- Hg Emission Factor Schedule Name",
        "  0,                       !- Pb Emission Factor {g/MJ}",
        "  ,                        !- Pb Emission Factor Schedule Name",
        "  0,                       !- Water Emission Factor {L/MJ}",
        "  ,                        !- Water Emission Factor Schedule Name",
        "  0,                       !- Nuclear High Level Emission Factor {g/MJ}",
        "  ,                        !- Nuclear High Level Emission Factor Schedule Name",
        "  0;                       !- Nuclear Low Level Emission Factor {m3/MJ}",

        "!NOTE:  EnergyPlus does not include LPG as a fuel type, use Propane as the resource name.",
        "!",
        "! USEPA Compilation of Air Pollutant Emission Factors, AP-42, Fifth Edition",
        "! Volume I: Stationary Point and Area Sources",
        "! Supplement B, October 1996",
        "! Chapter 1 External Combustion Sources",
        "! Section 1.5 Liquefied Petroleum Gas Combustion",
        "!",

        "FuelFactors,",
        "  Propane,                 !- Existing Fuel Resource Name",
        "  1,                       !- Source Energy Factor {J/J}",
        "  ,                        !- Source Energy Schedule Name",
        "  62.70851,                !- CO2 Emission Factor {g/MJ}",
        "  ,                        !- CO2 Emission Factor Schedule Name",
        "  0.00920894,              !- CO Emission Factor {g/MJ}",
        "  ,                        !- CO Emission Factor Schedule Name",
        "  0.000877042,             !- CH4 Emission Factor {g/MJ}",
        "  ,                        !- CH4 Emission Factor Schedule Name",
        "  0.0657782,               !- NOx Emission Factor {g/MJ}",
        "  ,                        !- NOx Emission Factor Schedule Name",
        "  0.00394669,              !- N2O Emission Factor {g/MJ}",
        "  ,                        !- N2O Emission Factor Schedule Name",
        "  0.000394669,             !- SO2 Emission Factor {g/MJ}",
        "  ,                        !- SO2 Emission Factor Schedule Name",
        "  0.00219261,              !- PM Emission Factor {g/MJ}",
        "  ,                        !- PM Emission Factor Schedule Name",
        "  0.00164445,              !- PM10 Emission Factor {g/MJ}",
        "  ,                        !- PM10 Emission Factor Schedule Name",
        "  0.000548151,             !- PM2.5 Emission Factor {g/MJ}",
        "  ,                        !- PM2.5 Emission Factor Schedule Name",
        "  0,                       !- NH3 Emission Factor {g/MJ}",
        "  ,                        !- NH3 Emission Factor Schedule Name",
        "  0.00175408,              !- NMVOC Emission Factor {g/MJ}",
        "  ,                        !- NMVOC Emission Factor Schedule Name",
        "  0,                       !- Hg Emission Factor {g/MJ}",
        "  ,                        !- Hg Emission Factor Schedule Name",
        "  0,                       !- Pb Emission Factor {g/MJ}",
        "  ,                        !- Pb Emission Factor Schedule Name",
        "  0,                       !- Water Emission Factor {L/MJ}",
        "  ,                        !- Water Emission Factor Schedule Name",
        "  0,                       !- Nuclear High Level Emission Factor {g/MJ}",
        "  ,                        !- Nuclear High Level Emission Factor Schedule Name",
        "  0;                       !- Nuclear Low Level Emission Factor {m3/MJ}",

        "! USEPA Compilation of Air Pollutant Emission Factors, AP-42, Fifth Edition",
        "! Volume I: Stationary Point and Area Sources",
        "! Supplement E, September 1998",
        "! Chapter 1 External Combustion Sources",
        "! Section 1.3 Fuel Oil Combustion",
        "!",

        "FuelFactors,",
        "  FuelOilNo1,              !- Existing Fuel Resource Name",
        "  1,                       !- Source Energy Factor {J/J}",
        "  ,                        !- Source Energy Schedule Name",
        "  66.0233,                 !- CO2 Emission Factor {g/MJ}",
        "  ,                        !- CO2 Emission Factor Schedule Name",
        "  0.0153543,               !- CO Emission Factor {g/MJ}",
        "  ,                        !- CO Emission Factor Schedule Name",
        "  0.000663304,             !- CH4 Emission Factor {g/MJ}",
        "  ,                        !- CH4 Emission Factor Schedule Name",
        "  0.061417,                !- NOx Emission Factor {g/MJ}",
        "  ,                        !- NOx Emission Factor Schedule Name",
        "  0.000337794,             !- N2O Emission Factor {g/MJ}",
        "  ,                        !- N2O Emission Factor Schedule Name",
        "  0.436061,                !- SO2 Emission Factor {g/MJ}",
        "  ,                        !- SO2 Emission Factor Schedule Name",
        "  0.0061417,               !- PM Emission Factor {g/MJ}",
        "  ,                        !- PM Emission Factor Schedule Name",
        "  0.00331652,              !- PM10 Emission Factor {g/MJ}",
        "  ,                        !- PM10 Emission Factor Schedule Name",
        "  0.00254881,              !- PM2.5 Emission Factor {g/MJ}",
        "  ,                        !- PM2.5 Emission Factor Schedule Name",
        "  0,                       !- NH3 Emission Factor {g/MJ}",
        "  ,                        !- NH3 Emission Factor Schedule Name",
        "  0.00104409,              !- NMVOC Emission Factor {g/MJ}",
        "  ,                        !- NMVOC Emission Factor Schedule Name",
        "  3.47006e-06,             !- Hg Emission Factor {g/MJ}",
        "  ,                        !- Hg Emission Factor Schedule Name",
        "  4.63699e-06,             !- Pb Emission Factor {g/MJ}",
        "  ,                        !- Pb Emission Factor Schedule Name",
        "  0,                       !- Water Emission Factor {L/MJ}",
        "  ,                        !- Water Emission Factor Schedule Name",
        "  0,                       !- Nuclear High Level Emission Factor {g/MJ}",
        "  ,                        !- Nuclear High Level Emission Factor Schedule Name",
        "  0;                       !- Nuclear Low Level Emission Factor {m3/MJ}",

        "! USEPA Compilation of Air Pollutant Emission Factors, AP-42, Fifth Edition",
        "! Volume I: Stationary Point and Area Sources",
        "! Supplement E, September 1998",
        "! Chapter 1 External Combustion Sources",
        "! Section 1.3 Fuel Oil Combustion",
        "!",

        "FuelFactors,",
        "  FuelOilNo2,              !- Existing Fuel Resource Name",
        "  1,                       !- Source Energy Factor {J/J}",
        "  ,                        !- Source Energy Schedule Name",
        "  68.47998,                !- CO2 Emission Factor {g/MJ}",
        "  ,                        !- CO2 Emission Factor Schedule Name",
        "  0.0153543,               !- CO Emission Factor {g/MJ}",
        "  ,                        !- CO Emission Factor Schedule Name",
        "  0.000663304,             !- CH4 Emission Factor {g/MJ}",
        "  ,                        !- CH4 Emission Factor Schedule Name",
        "  0.0737004,               !- NOx Emission Factor {g/MJ}",
        "  ,                        !- NOx Emission Factor Schedule Name",
        "  0.000337794,             !- N2O Emission Factor {g/MJ}",
        "  ,                        !- N2O Emission Factor Schedule Name",
        "  0.482124,                !- SO2 Emission Factor {g/MJ}",
        "  ,                        !- SO2 Emission Factor Schedule Name",
        "  0.0061417,               !- PM Emission Factor {g/MJ}",
        "  ,                        !- PM Emission Factor Schedule Name",
        "  0.00331652,              !- PM10 Emission Factor {g/MJ}",
        "  ,                        !- PM10 Emission Factor Schedule Name",
        "  0.00254881,              !- PM2.5 Emission Factor {g/MJ}",
        "  ,                        !- PM2.5 Emission Factor Schedule Name",
        "  0,                       !- NH3 Emission Factor {g/MJ}",
        "  ,                        !- NH3 Emission Factor Schedule Name",
        "  0.00104409,              !- NMVOC Emission Factor {g/MJ}",
        "  ,                        !- NMVOC Emission Factor Schedule Name",
        "  3.47006e-06,             !- Hg Emission Factor {g/MJ}",
        "  ,                        !- Hg Emission Factor Schedule Name",
        "  4.63699e-06,             !- Pb Emission Factor {g/MJ}",
        "  ,                        !- Pb Emission Factor Schedule Name",
        "  0,                       !- Water Emission Factor {L/MJ}",
        "  ,                        !- Water Emission Factor Schedule Name",
        "  0,                       !- Nuclear High Level Emission Factor {g/MJ}",
        "  ,                        !- Nuclear High Level Emission Factor Schedule Name",
        "  0;                       !- Nuclear Low Level Emission Factor {m3/MJ}",

        "!NOTE:  EnergyPlus does not include FuelOil#4 as a fuel, use OtherFuel1 as the resource name",
        "!",
        "! USEPA Compilation of Air Pollutant Emission Factors, AP-42, Fifth Edition",
        "! Volume I: Stationary Point and Area Sources",
        "! Supplement E, September 1998",
        "! Chapter 1 External Combustion Sources",
        "! Section 1.3 Fuel Oil Combustion",
        "!",

        "FuelFactors,",
        "  OtherFuel1,              !- Existing Fuel Resource Name",
        "  1,                       !- Source Energy Factor {J/J}",
        "  ,                        !- Source Energy Schedule Name",
        "  76.77128,                !- CO2 Emission Factor {g/MJ}",
        "  ,                        !- CO2 Emission Factor Schedule Name",
        "  0.0153543,               !- CO Emission Factor {g/MJ}",
        "  ,                        !- CO Emission Factor Schedule Name",
        "  0.000663304,             !- CH4 Emission Factor {g/MJ}",
        "  ,                        !- CH4 Emission Factor Schedule Name",
        "  0.061417,                !- NOx Emission Factor {g/MJ}",
        "  ,                        !- NOx Emission Factor Schedule Name",
        "  0.000337794,             !- N2O Emission Factor {g/MJ}",
        "  ,                        !- N2O Emission Factor Schedule Name",
        "  0.460628,                !- SO2 Emission Factor {g/MJ}",
        "  ,                        !- SO2 Emission Factor Schedule Name",
        "  0.021496,                !- PM Emission Factor {g/MJ}",
        "  ,                        !- PM Emission Factor Schedule Name",
        "  0.0158763,               !- PM10 Emission Factor {g/MJ}",
        "  ,                        !- PM10 Emission Factor Schedule Name",
        "  0.00589603,              !- PM2.5 Emission Factor {g/MJ}",
        "  ,                        !- PM2.5 Emission Factor Schedule Name",
        "  0,                       !- NH3 Emission Factor {g/MJ}",
        "  ,                        !- NH3 Emission Factor Schedule Name",
        "  0.00104409,              !- NMVOC Emission Factor {g/MJ}",
        "  ,                        !- NMVOC Emission Factor Schedule Name",
        "  3.47006e-06,             !- Hg Emission Factor {g/MJ}",
        "  ,                        !- Hg Emission Factor Schedule Name",
        "  4.63699e-06,             !- Pb Emission Factor {g/MJ}",
        "  ,                        !- Pb Emission Factor Schedule Name",
        "  0,                       !- Water Emission Factor {L/MJ}",
        "  ,                        !- Water Emission Factor Schedule Name",
        "  0,                       !- Nuclear High Level Emission Factor {g/MJ}",
        "  ,                        !- Nuclear High Level Emission Factor Schedule Name",
        "  0;                       !- Nuclear Low Level Emission Factor {m3/MJ}",

        "! USEPA Compilation of Air Pollutant Emission Factors, AP-42, Fifth Edition",
        "! Volume I: Stationary Point and Area Sources",
        "! Supplement E, September 1998",
        "! Chapter 1 External Combustion Sources",
        "! Section 1.1 Bituminous And Subbituminous Coal Combustion",
        "!",

        "FuelFactors,",
        "  Coal,                    !- Existing Fuel Resource Name",
        "  1,                       !- Source Energy Factor {J/J}",
        "  ,                        !- Source Energy Schedule Name",
        "  91.11052,                !- CO2 Emission Factor {g/MJ}",
        "  ,                        !- CO2 Emission Factor Schedule Name",
        "  0.00826774,              !- CO Emission Factor {g/MJ}",
        "  ,                        !- CO Emission Factor Schedule Name",
        "  0.000661419,             !- CH4 Emission Factor {g/MJ}",
        "  ,                        !- CH4 Emission Factor Schedule Name",
        "  0.198426,                !- NOx Emission Factor {g/MJ}",
        "  ,                        !- NOx Emission Factor Schedule Name",
        "  0.000496065,             !- N2O Emission Factor {g/MJ}",
        "  ,                        !- N2O Emission Factor Schedule Name",
        "  0.628348,                !- SO2 Emission Factor {g/MJ}",
        "  ,                        !- SO2 Emission Factor Schedule Name",
        "  0.165355,                !- PM Emission Factor {g/MJ}",
        "  ,                        !- PM Emission Factor Schedule Name",
        "  0.0380316,               !- PM10 Emission Factor {g/MJ}",
        "  ,                        !- PM10 Emission Factor Schedule Name",
        "  0.00992129,              !- PM2.5 Emission Factor {g/MJ}",
        "  ,                        !- PM2.5 Emission Factor Schedule Name",
        "  0,                       !- NH3 Emission Factor {g/MJ}",
        "  ,                        !- NH3 Emission Factor Schedule Name",
        "  0.000992129,             !- NMVOC Emission Factor {g/MJ}",
        "  ,                        !- NMVOC Emission Factor Schedule Name",
        "  6.9449e-06,              !- Hg Emission Factor {g/MJ}",
        "  ,                        !- Hg Emission Factor Schedule Name",
        "  1.37245e-06,             !- Pb Emission Factor {g/MJ}",
        "  ,                        !- Pb Emission Factor Schedule Name",
        "  0,                       !- Water Emission Factor {L/MJ}",
        "  ,                        !- Water Emission Factor Schedule Name",
        "  0,                       !- Nuclear High Level Emission Factor {g/MJ}",
        "  ,                        !- Nuclear High Level Emission Factor Schedule Name",
        "  0;                       !- Nuclear Low Level Emission Factor {m3/MJ}",

        "! United States 1999 national average electricity emissions factors based on eGRID, 1605, AirData",
        "! United States Water Emission Fuel Factors are the combined thermoelectric and hydroelectric weighted",
        "! averages from:",
        "!   Torcellini, Paul; Long, Nicholas; Judkoff, Ron; Consumptive Water Use for U.S. Power Production;",
        "!      NREL Report No. TP-550-33905.  Golden, CO; 2003; http://www.nlr.gov/docs/fy04osti/33905.pdf;",
        "!    or",
        "!   Torcellini, Paul; Long, Nicholas; Judkoff, Ron; Consumptive Water Use for U.S. Power Production;",
        "!      ASHRAE Transactions 2003, Vol 110, Part 1.  Atlanta, GA; January 2004;",

        "FuelFactors,",
        "  Electricity,             !- Existing Fuel Resource Name",
        "  2.253,                   !- Source Energy Factor {J/J}",
        "  ,                        !- Source Energy Schedule Name",
        "  168.33317,               !- CO2 Emission Factor {g/MJ}",
        "  ,                        !- CO2 Emission Factor Schedule Name",
        "  0.0420616,               !- CO Emission Factor {g/MJ}",
        "  ,                        !- CO Emission Factor Schedule Name",
        "  0.00139858,              !- CH4 Emission Factor {g/MJ}",
        "  ,                        !- CH4 Emission Factor Schedule Name",
        "  0.410753,                !- NOx Emission Factor {g/MJ}",
        "  ,                        !- NOx Emission Factor Schedule Name",
        "  0.00241916,              !- N2O Emission Factor {g/MJ}",
        "  ,                        !- N2O Emission Factor Schedule Name",
        "  0.865731,                !- SO2 Emission Factor {g/MJ}",
        "  ,                        !- SO2 Emission Factor Schedule Name",
        "  0.0295827,               !- PM Emission Factor {g/MJ}",
        "  ,                        !- PM Emission Factor Schedule Name",
        "  0.018045,                !- PM10 Emission Factor {g/MJ}",
        "  ,                        !- PM10 Emission Factor Schedule Name",
        "  0.0115377,               !- PM2.5 Emission Factor {g/MJ}",
        "  ,                        !- PM2.5 Emission Factor Schedule Name",
        "  0.00110837,              !- NH3 Emission Factor {g/MJ}",
        "  ,                        !- NH3 Emission Factor Schedule Name",
        "  0.00372332,              !- NMVOC Emission Factor {g/MJ}",
        "  ,                        !- NMVOC Emission Factor Schedule Name",
        "  3.36414e-06,             !- Hg Emission Factor {g/MJ}",
        "  ,                        !- Hg Emission Factor Schedule Name",
        "  0,                       !- Pb Emission Factor {g/MJ}",
        "  ,                        !- Pb Emission Factor Schedule Name",
        "  2.10074,                 !- Water Emission Factor {L/MJ}",
        "  ,                        !- Water Emission Factor Schedule Name",
        "  0,                       !- Nuclear High Level Emission Factor {g/MJ}",
        "  ,                        !- Nuclear High Level Emission Factor Schedule Name",
        "  0;                       !- Nuclear Low Level Emission Factor {m3/MJ}",
    });
    ASSERT_TRUE(process_idf(idf_objects));

    state->dataPollution->GetInputFlagPollution = true;
    Pollution::SetupPollutionMeterReporting(*state);

    // Test get output variables for Total Sky Cover and Opaque Sky Cover
    std::string fuelTypeNames[9] = {"NaturalGas", "Diesel", "Gasoline", "Propane", "FuelOilNo1", "FuelOilNo2", "OtherFuel1", "Coal", "Electricity"};
    for (unsigned long i = 0; i < size(fuelTypeNames); i++) {
        EXPECT_EQ(std::format("Site:Environmental Impact {} Source Energy", fuelTypeNames[i]),
                  state->dataOutputProcessor->outVars[i * 17 + 0]->keyColonName);
        EXPECT_EQ(std::format("Site:Environmental Impact {} CO2 Emissions Mass", fuelTypeNames[i]),
                  state->dataOutputProcessor->outVars[i * 17 + 1]->keyColonName);
        EXPECT_EQ(std::format("Site:Environmental Impact {} CO Emissions Mass", fuelTypeNames[i]),
                  state->dataOutputProcessor->outVars[i * 17 + 2]->keyColonName);
        EXPECT_EQ(std::format("Site:Environmental Impact {} CH4 Emissions Mass", fuelTypeNames[i]),
                  state->dataOutputProcessor->outVars[i * 17 + 3]->keyColonName);
        EXPECT_EQ(std::format("Site:Environmental Impact {} NOx Emissions Mass", fuelTypeNames[i]),
                  state->dataOutputProcessor->outVars[i * 17 + 4]->keyColonName);
        EXPECT_EQ(std::format("Site:Environmental Impact {} N2O Emissions Mass", fuelTypeNames[i]),
                  state->dataOutputProcessor->outVars[i * 17 + 5]->keyColonName);
        EXPECT_EQ(std::format("Site:Environmental Impact {} SO2 Emissions Mass", fuelTypeNames[i]),
                  state->dataOutputProcessor->outVars[i * 17 + 6]->keyColonName);
        EXPECT_EQ(std::format("Site:Environmental Impact {} PM Emissions Mass", fuelTypeNames[i]),
                  state->dataOutputProcessor->outVars[i * 17 + 7]->keyColonName);
        EXPECT_EQ(std::format("Site:Environmental Impact {} PM10 Emissions Mass", fuelTypeNames[i]),
                  state->dataOutputProcessor->outVars[i * 17 + 8]->keyColonName);
        EXPECT_EQ(std::format("Site:Environmental Impact {} PM2.5 Emissions Mass", fuelTypeNames[i]),
                  state->dataOutputProcessor->outVars[i * 17 + 9]->keyColonName);
        EXPECT_EQ(std::format("Site:Environmental Impact {} NH3 Emissions Mass", fuelTypeNames[i]),
                  state->dataOutputProcessor->outVars[i * 17 + 10]->keyColonName);
        EXPECT_EQ(std::format("Site:Environmental Impact {} NMVOC Emissions Mass", fuelTypeNames[i]),
                  state->dataOutputProcessor->outVars[i * 17 + 11]->keyColonName);
        EXPECT_EQ(std::format("Site:Environmental Impact {} Hg Emissions Mass", fuelTypeNames[i]),
                  state->dataOutputProcessor->outVars[i * 17 + 12]->keyColonName);
        EXPECT_EQ(std::format("Site:Environmental Impact {} Pb Emissions Mass", fuelTypeNames[i]),
                  state->dataOutputProcessor->outVars[i * 17 + 13]->keyColonName);
        EXPECT_EQ(std::format("Site:Environmental Impact {} Water Consumption Volume", fuelTypeNames[i]),
                  state->dataOutputProcessor->outVars[i * 17 + 14]->keyColonName);
        EXPECT_EQ(std::format("Site:Environmental Impact {} Nuclear High Level Waste Mass", fuelTypeNames[i]),
                  state->dataOutputProcessor->outVars[i * 17 + 15]->keyColonName);
        EXPECT_EQ(std::format("Site:Environmental Impact {} Nuclear Low Level Waste Volume", fuelTypeNames[i]),
                  state->dataOutputProcessor->outVars[i * 17 + 16]->keyColonName);
    }

    // Variables specific to the Electricity fuel type
    EXPECT_EQ("Site:Environmental Impact Purchased Electricity Source Energy",
              state->dataOutputProcessor->outVars[size(fuelTypeNames) * 17 + 0]->keyColonName);
    EXPECT_EQ("Site:Environmental Impact Surplus Sold Electricity Source",
              state->dataOutputProcessor->outVars[size(fuelTypeNames) * 17 + 1]->keyColonName);

    // Variables always setup for total carbon equivalent
    EXPECT_EQ("Site:Environmental Impact Total N2O Emissions Carbon Equivalent Mass",
              state->dataOutputProcessor->outVars[size(fuelTypeNames) * 17 + 2]->keyColonName);
    EXPECT_EQ("Site:Environmental Impact Total CH4 Emissions Carbon Equivalent Mass",
              state->dataOutputProcessor->outVars[size(fuelTypeNames) * 17 + 3]->keyColonName);
    EXPECT_EQ("Site:Environmental Impact Total CO2 Emissions Carbon Equivalent Mass",
              state->dataOutputProcessor->outVars[size(fuelTypeNames) * 17 + 4]->keyColonName);
}

TEST_F(EnergyPlusFixture, PollutionModule_TestEnvironmentalImpactFactors)
{

    std::string const idf_objects = delimited_string({
        "EnvironmentalImpactFactors,",
        "  0.3,                     !- District Heating Water Efficiency",
        "  3.0,                     !- District Cooling COP {W/W}",
        "  0.3,                     !- District Heating Steam Conversion Efficiency",
        "  80.7272,                 !- Total Carbon Equivalent Emission Factor From N2O {kg/kg}",
        "  6.2727,                  !- Total Carbon Equivalent Emission Factor From CH4 {kg/kg}",
        "  0.2727;                  !- Total Carbon Equivalent Emission Factor From CO2 {kg/kg}",

        "FuelFactors,",
        "  NaturalGas,              !- Existing Fuel Resource Name",
        "  1.0,                     !- Source Energy Factor {J/J}",
        "  ,                        !- Source Energy Schedule Name",
        "  50.23439,                !- CO2 Emission Factor {g/MJ}",
        "  ,                        !- CO2 Emission Factor Schedule Name",
        "  0.0351641,               !- CO Emission Factor {g/MJ}",
        "  ,                        !- CO Emission Factor Schedule Name",
        "  0.000962826,             !- CH4 Emission Factor {g/MJ}",
        "  ,                        !- CH4 Emission Factor Schedule Name",
        "  0.041862,                !- NOx Emission Factor {g/MJ}",
        "  ,                        !- NOx Emission Factor Schedule Name",
        "  0.000920964,             !- N2O Emission Factor {g/MJ}",
        "  ,                        !- N2O Emission Factor Schedule Name",
        "  0.000251172,             !- SO2 Emission Factor {g/MJ}",
        "  ,                        !- SO2 Emission Factor Schedule Name",
        "  0.00318151,              !- PM Emission Factor {g/MJ}",
        "  ,                        !- PM Emission Factor Schedule Name",
        "  0.00238613,              !- PM10 Emission Factor {g/MJ}",
        "  ,                        !- PM10 Emission Factor Schedule Name",
        "  0.000795378,             !- PM2.5 Emission Factor {g/MJ}",
        "  ,                        !- PM2.5 Emission Factor Schedule Name",
        "  0,                       !- NH3 Emission Factor {g/MJ}",
        "  ,                        !- NH3 Emission Factor Schedule Name",
        "  0.00230241,              !- NMVOC Emission Factor {g/MJ}",
        "  ,                        !- NMVOC Emission Factor Schedule Name",
        "  1.08841e-07,             !- Hg Emission Factor {g/MJ}",
        "  ,                        !- Hg Emission Factor Schedule Name",
        "  2.0931e-07,              !- Pb Emission Factor {g/MJ}",
        "  ,                        !- Pb Emission Factor Schedule Name",
        "  0,                       !- Water Emission Factor {L/MJ}",
        "  ,                        !- Water Emission Factor Schedule Name",
        "  0,                       !- Nuclear High Level Emission Factor {g/MJ}",
        "  ,                        !- Nuclear High Level Emission Factor Schedule Name",
        "  0;                       !- Nuclear Low Level Emission Factor {m3/MJ}",
    });
    ASSERT_TRUE(process_idf(idf_objects));

    Real64 ExpectedOutput(0.3);
    Real64 AllowedTolerance(0.001);

    Pollution::GetPollutionFactorInput(*state);

    // The get routine should rest the steam conversion efficiency to the default value of 0.25.
    // Previously because of a typo, it would reset it to the input value of zero (or even a negative number).
    ASSERT_NEAR(state->dataPollution->SteamConvEffic, ExpectedOutput, AllowedTolerance);
}
