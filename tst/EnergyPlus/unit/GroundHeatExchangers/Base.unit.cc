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
#include "../Fixtures/EnergyPlusFixture.hh"
#include <EnergyPlus/Data/EnergyPlusData.hh>
#include <EnergyPlus/DataSystemVariables.hh>
#include <EnergyPlus/GroundHeatExchangers/Base.hh>
#include <EnergyPlus/GroundHeatExchangers/State.hh>
#include <EnergyPlus/OutputReportPredefined.hh>
#include <EnergyPlus/Plant/PlantManager.hh>
#include <gtest/gtest.h>

using namespace EnergyPlus;
using namespace EnergyPlus::GroundHeatExchangers;

TEST_F(EnergyPlusFixture, GroundHeatExchangerTest_System_Properties_IDF_Check)
{
    std::string const idf_objects = delimited_string({
        "GroundHeatExchanger:Vertical:Properties,",
        "  GHE-1 Props,             !- Name",
        "  1,                       !- Depth of Top of Borehole {m}",
        "  100,                     !- Borehole Length {m}",
        "  0.109982,                !- Borehole Diameter {m}",
        "  0.744,                   !- Grout Thermal Conductivity {W/m-K}",
        "  3900000.0,               !- Grout Thermal Heat Capacity {J/m3-K}",
        "  0.389,                   !- Pipe Thermal Conductivity {W/m-K}",
        "  1770000.0,               !- Pipe Thermal Heat Capacity {J/m3-K}",
        "  0.0267,                  !- Pipe Outer Diameter {m}",
        "  0.00243,                 !- Pipe Thickness {m}",
        "  0.04556;                 !- U-Tube Distance {m}",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    GetGroundHeatExchangerInput(*state);

    EXPECT_EQ(1u, state->dataGroundHeatExchanger->vertPropsVector.size());

    auto &thisProp(state->dataGroundHeatExchanger->vertPropsVector[0]);

    EXPECT_EQ("GHE-1 PROPS", thisProp->name);
    EXPECT_EQ(1, thisProp->bhTopDepth);
    EXPECT_EQ(100, thisProp->bhLength);
    EXPECT_EQ(0.109982, thisProp->bhDiameter);
    EXPECT_EQ(0.744, thisProp->grout.k);
    EXPECT_EQ(3.90E+06, thisProp->grout.rhoCp);
    EXPECT_EQ(0.389, thisProp->pipe.k);
    EXPECT_EQ(1.77e6, thisProp->pipe.rhoCp);
    EXPECT_EQ(0.0267, thisProp->pipe.outDia);
    EXPECT_EQ(0.00243, thisProp->pipe.thickness);
    EXPECT_EQ(0.04556, thisProp->bhUTubeDist);
}

TEST_F(EnergyPlusFixture, GroundHeatExchangerTest_Slinky_IDF_Check)
{
    std::string const idf_objects = delimited_string({
        "GroundHeatExchanger:Slinky,",
        "  Slinky GHX,              !- Name",
        "  GHE Inlet Node,          !- Inlet Node Name",
        "  GHE Outlet Node,         !- Outlet Node Name",
        "  0.0033,                  !- Design Flow Rate {m3/s}",
        "  1.2,                     !- Soil Thermal Conductivity {W/m-K}",
        "  3200,                    !- Soil Density {kg/m3}",
        "  850,                     !- Soil Specific Heat {J/kg-K}",
        "  1.8,                     !- Pipe Thermal Conductivity {W/m-K}",
        "  920,                     !- Pipe Density {kg/m3}",
        "  2200,                    !- Pipe Specific Heat {J/kg-K}",
        "  0.02667,                 !- Pipe Outer Diameter {m}",
        "  0.002413,                !- Pipe Thickness {m}",
        "  Vertical,                !- Heat Exchanger Configuration",
        "  1,                       !- Coil Diameter {m}",
        "  0.2,                     !- Coil Pitch {m}",
        "  2.5,                     !- Trench Depth {m}",
        "  40,                      !- Trench Length {m}",
        "  15,                      !- Number of Trenches",
        "  2,                       !- Horizontal Spacing Between Pipes {m}",
        "  Site:GroundTemperature:Undisturbed:KusudaAchenbach,  !- Undisturbed Ground Temperature Model Type",
        "  KATemps,                 !- Undisturbed Ground Temperature Model Name",
        "  10;                      !- Maximum Length of Simulation {years}",

        "Site:GroundTemperature:Undisturbed:KusudaAchenbach,",
        "  KATemps,                 !- Name",
        "  1.8,                     !- Soil Thermal Conductivity {W/m-K}",
        "  920,                     !- Soil Density {kg/m3}",
        "  2200,                    !- Soil Specific Heat {J/kg-K}",
        "  15.5,                    !- Average Soil Surface Temperature {C}",
        "  3.2,                     !- Average Amplitude of Surface Temperature {deltaC}",
        "  8;                       !- Phase Shift of Minimum Surface Temperature {days}",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    GetGroundHeatExchangerInput(*state);

    EXPECT_EQ(1u, state->dataGroundHeatExchanger->slinkyGLHE.size());

    auto &thisGHE(state->dataGroundHeatExchanger->slinkyGLHE[0]);

    EXPECT_NEAR(thisGHE.designFlow, 0.0033, 0.000001);
    EXPECT_NEAR(thisGHE.soil.k, 1.2, 0.001);
    EXPECT_NEAR(thisGHE.soil.rho, 3200, 1.0);
    EXPECT_NEAR(thisGHE.soil.cp, 850, 1.0);
    EXPECT_NEAR(thisGHE.pipe.k, 1.8, 0.001);
    EXPECT_NEAR(thisGHE.pipe.rho, 920, 1.0);
    EXPECT_NEAR(thisGHE.pipe.cp, 2200, 1.0);
    EXPECT_NEAR(thisGHE.pipe.outDia, 0.02667, 0.00001);
    EXPECT_NEAR(thisGHE.pipe.thickness, 0.002413, 0.00001);
    EXPECT_NEAR(thisGHE.pipe.thickness, 0.002413, 0.00001);
    EXPECT_NEAR(thisGHE.coilDiameter, 1.0, 0.01);
    EXPECT_NEAR(thisGHE.coilPitch, 0.2, 0.01);
    EXPECT_NEAR(thisGHE.trenchDepth, 2.5, 0.01);
    EXPECT_NEAR(thisGHE.trenchLength, 40.0, 0.01);
    EXPECT_NEAR(thisGHE.numTrenches, 15.0, 0.1);
    EXPECT_NEAR(thisGHE.trenchSpacing, 2.0, 0.1);

    auto &orp = *state->dataOutRptPredefined;
    std::string const GLHEName = thisGHE.name;
    EXPECT_EQ("GroundHeatExchanger:Slinky", OutputReportPredefined::RetrievePreDefTableEntry(*state, orp.pdchGLHEType, GLHEName));
    EXPECT_EQ("9424.78", OutputReportPredefined::RetrievePreDefTableEntry(*state, orp.pdchGLHETubeLength, GLHEName));
    EXPECT_EQ("0.003300", OutputReportPredefined::RetrievePreDefTableEntry(*state, orp.pdchGLHEVolFlow, GLHEName));
    EXPECT_EQ("2.50", OutputReportPredefined::RetrievePreDefTableEntry(*state, orp.pdchGLHEbhDepth, GLHEName));
    EXPECT_EQ("1.00", OutputReportPredefined::RetrievePreDefTableEntry(*state, orp.pdchGLHEbhDiam, GLHEName));
    EXPECT_EQ("40.00", OutputReportPredefined::RetrievePreDefTableEntry(*state, orp.pdchGLHEbhLeng, GLHEName));
    EXPECT_EQ("15.00", OutputReportPredefined::RetrievePreDefTableEntry(*state, orp.pdchGLHENumHolesTrenches, GLHEName));
}

TEST_F(EnergyPlusFixture, GroundHeatExchangerTest_System_Resp_Factors_IDF_Check)
{
    std::string const idf_objects = delimited_string({
        "GroundHeatExchanger:Vertical:Properties,",
        "  GHE-1 Props,             !- Name",
        "  1,                       !- Depth of Top of Borehole {m}",
        "  100,                     !- Borehole Length {m}",
        "  0.109982,                !- Borehole Diameter {m}",
        "  0.744,                   !- Grout Thermal Conductivity {W/m-K}",
        "  3900000.0,               !- Grout Thermal Heat Capacity {J/m3-K}",
        "  0.389,                   !- Pipe Thermal Conductivity {W/m-K}",
        "  1770000.0,               !- Pipe Thermal Heat Capacity {J/m3-K}",
        "  0.0267,                  !- Pipe Outer Diameter {m}",
        "  0.00243,                 !- Pipe Thickness {m}",
        "  0.04556;                 !- U-Tube Distance {m}",

        "GroundHeatExchanger:ResponseFactors,",
        "  GHE-1 g-functions,       !- Name",
        "  GHE-1 Props,             !- GHE:Vertical:Properties Object Name",
        "  4,                       !- Number of Boreholes",
        "  0.00043,                 !- G-Function Reference Ratio {dimensionless}",
        "  -15.585075,              !- g-Function Ln(T/Ts) Value",
        "  -2.672011,               !- g-Function g Value",
        "  -15.440481,              !- g-Function Ln(T/Ts) Value",
        "  -2.575897,               !- g-Function g Value",
        "  -15.295888,              !- g-Function Ln(T/Ts) Value",
        "  -2.476279,               !- g-Function g Value",
        "  -15.151295,              !- g-Function Ln(T/Ts) Value",
        "  -2.372609,               !- g-Function g Value",
        "  -15.006701,              !- g-Function Ln(T/Ts) Value",
        "  -2.264564,               !- g-Function g Value",
        "  -14.862108,              !- g-Function Ln(T/Ts) Value",
        "  -2.151959,               !- g-Function g Value",
        "  -14.717515,              !- g-Function Ln(T/Ts) Value",
        "  -2.034708,               !- g-Function g Value",
        "  -14.572921,              !- g-Function Ln(T/Ts) Value",
        "  -1.912801,               !- g-Function g Value",
        "  -14.428328,              !- g-Function Ln(T/Ts) Value",
        "  -1.786299,               !- g-Function g Value",
        "  -14.283734,              !- g-Function Ln(T/Ts) Value",
        "  -1.655324,               !- g-Function g Value",
        "  -14.139141,              !- g-Function Ln(T/Ts) Value",
        "  -1.520066,               !- g-Function g Value",
        "  -13.994548,              !- g-Function Ln(T/Ts) Value",
        "  -1.380782,               !- g-Function g Value",
        "  -13.849954,              !- g-Function Ln(T/Ts) Value",
        "  -1.237813,               !- g-Function g Value",
        "  -13.705361,              !- g-Function Ln(T/Ts) Value",
        "  -1.091594,               !- g-Function g Value",
        "  -13.560768,              !- g-Function Ln(T/Ts) Value",
        "  -0.94267,                !- g-Function g Value",
        "  -13.416174,              !- g-Function Ln(T/Ts) Value",
        "  -0.791704,               !- g-Function g Value",
        "  -13.271581,              !- g-Function Ln(T/Ts) Value",
        "  -0.639479,               !- g-Function g Value",
        "  -13.126988,              !- g-Function Ln(T/Ts) Value",
        "  -0.486879,               !- g-Function g Value",
        "  -12.982394,              !- g-Function Ln(T/Ts) Value",
        "  -0.334866,               !- g-Function g Value",
        "  -12.837801,              !- g-Function Ln(T/Ts) Value",
        "  -0.184431,               !- g-Function g Value",
        "  -12.693207,              !- g-Function Ln(T/Ts) Value",
        "  -0.036546,               !- g-Function g Value",
        "  -12.548614,              !- g-Function Ln(T/Ts) Value",
        "  0.107892,                !- g-Function g Value",
        "  -12.404021,              !- g-Function Ln(T/Ts) Value",
        "  0.248115,                !- g-Function g Value",
        "  -12.259427,              !- g-Function Ln(T/Ts) Value",
        "  0.38352,                 !- g-Function g Value",
        "  -12.114834,              !- g-Function Ln(T/Ts) Value",
        "  0.5137,                  !- g-Function g Value",
        "  -11.970241,              !- g-Function Ln(T/Ts) Value",
        "  0.63845,                 !- g-Function g Value",
        "  -11.825647,              !- g-Function Ln(T/Ts) Value",
        "  0.757758,                !- g-Function g Value",
        "  -11.681054,              !- g-Function Ln(T/Ts) Value",
        "  0.87178,                 !- g-Function g Value",
        "  -11.536461,              !- g-Function Ln(T/Ts) Value",
        "  0.980805,                !- g-Function g Value",
        "  -11.391867,              !- g-Function Ln(T/Ts) Value",
        "  1.085218,                !- g-Function g Value",
        "  -11.247274,              !- g-Function Ln(T/Ts) Value",
        "  1.185457,                !- g-Function g Value",
        "  -11.10268,               !- g-Function Ln(T/Ts) Value",
        "  1.28198,                 !- g-Function g Value",
        "  -10.958087,              !- g-Function Ln(T/Ts) Value",
        "  1.375237,                !- g-Function g Value",
        "  -10.813494,              !- g-Function Ln(T/Ts) Value",
        "  1.465651,                !- g-Function g Value",
        "  -10.6689,                !- g-Function Ln(T/Ts) Value",
        "  1.553606,                !- g-Function g Value",
        "  -10.524307,              !- g-Function Ln(T/Ts) Value",
        "  1.639445,                !- g-Function g Value",
        "  -10.379714,              !- g-Function Ln(T/Ts) Value",
        "  1.723466,                !- g-Function g Value",
        "  -10.23512,               !- g-Function Ln(T/Ts) Value",
        "  1.805924,                !- g-Function g Value",
        "  -10.090527,              !- g-Function Ln(T/Ts) Value",
        "  1.887041,                !- g-Function g Value",
        "  -9.945934,               !- g-Function Ln(T/Ts) Value",
        "  1.967002,                !- g-Function g Value",
        "  -9.80134,                !- g-Function Ln(T/Ts) Value",
        "  2.045967,                !- g-Function g Value",
        "  -9.656747,               !- g-Function Ln(T/Ts) Value",
        "  2.124073,                !- g-Function g Value",
        "  -9.512154,               !- g-Function Ln(T/Ts) Value",
        "  2.201436,                !- g-Function g Value",
        "  -9.36756,                !- g-Function Ln(T/Ts) Value",
        "  2.278154,                !- g-Function g Value",
        "  -9.222967,               !- g-Function Ln(T/Ts) Value",
        "  2.354312,                !- g-Function g Value",
        "  -9.078373,               !- g-Function Ln(T/Ts) Value",
        "  2.429984,                !- g-Function g Value",
        "  -8.93378,                !- g-Function Ln(T/Ts) Value",
        "  2.505232,                !- g-Function g Value",
        "  -8.789187,               !- g-Function Ln(T/Ts) Value",
        "  2.580112,                !- g-Function g Value",
        "  -8.644593,               !- g-Function Ln(T/Ts) Value",
        "  2.654669,                !- g-Function g Value",
        "  -8.5,                    !- g-Function Ln(T/Ts) Value",
        "  2.830857,                !- g-Function g Value",
        "  -7.8,                    !- g-Function Ln(T/Ts) Value",
        "  3.176174,                !- g-Function g Value",
        "  -7.2,                    !- g-Function Ln(T/Ts) Value",
        "  3.484017,                !- g-Function g Value",
        "  -6.5,                    !- g-Function Ln(T/Ts) Value",
        "  3.88777,                 !- g-Function g Value",
        "  -5.9,                    !- g-Function Ln(T/Ts) Value",
        "  4.311301,                !- g-Function g Value",
        "  -5.2,                    !- g-Function Ln(T/Ts) Value",
        "  4.928223,                !- g-Function g Value",
        "  -4.5,                    !- g-Function Ln(T/Ts) Value",
        "  5.696283,                !- g-Function g Value",
        "  -3.963,                  !- g-Function Ln(T/Ts) Value",
        "  6.361422,                !- g-Function g Value",
        "  -3.27,                   !- g-Function Ln(T/Ts) Value",
        "  7.375959,                !- g-Function g Value",
        "  -2.864,                  !- g-Function Ln(T/Ts) Value",
        "  7.994729,                !- g-Function g Value",
        "  -2.577,                  !- g-Function Ln(T/Ts) Value",
        "  8.438474,                !- g-Function g Value",
        "  -2.171,                  !- g-Function Ln(T/Ts) Value",
        "  9.059916,                !- g-Function g Value",
        "  -1.884,                  !- g-Function Ln(T/Ts) Value",
        "  9.492228,                !- g-Function g Value",
        "  -1.191,                  !- g-Function Ln(T/Ts) Value",
        "  10.444276,               !- g-Function g Value",
        "  -0.497,                  !- g-Function Ln(T/Ts) Value",
        "  11.292233,               !- g-Function g Value",
        "  -0.274,                  !- g-Function Ln(T/Ts) Value",
        "  11.525537,               !- g-Function g Value",
        "  -0.051,                  !- g-Function Ln(T/Ts) Value",
        "  11.735157,               !- g-Function g Value",
        "  0.196,                   !- g-Function Ln(T/Ts) Value",
        "  11.942392,               !- g-Function g Value",
        "  0.419,                   !- g-Function Ln(T/Ts) Value",
        "  12.103282,               !- g-Function g Value",
        "  0.642,                   !- g-Function Ln(T/Ts) Value",
        "  12.243398,               !- g-Function g Value",
        "  0.873,                   !- g-Function Ln(T/Ts) Value",
        "  12.365217,               !- g-Function g Value",
        "  1.112,                   !- g-Function Ln(T/Ts) Value",
        "  12.469007,               !- g-Function g Value",
        "  1.335,                   !- g-Function Ln(T/Ts) Value",
        "  12.547123,               !- g-Function g Value",
        "  1.679,                   !- g-Function Ln(T/Ts) Value",
        "  12.63789,                !- g-Function g Value",
        "  2.028,                   !- g-Function Ln(T/Ts) Value",
        "  12.699245,               !- g-Function g Value",
        "  2.275,                   !- g-Function Ln(T/Ts) Value",
        "  12.729288,               !- g-Function g Value",
        "  3.003,                   !- g-Function Ln(T/Ts) Value",
        "  12.778359;               !- g-Function g Value",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    GetGroundHeatExchangerInput(*state);

    EXPECT_EQ(1u, state->dataGroundHeatExchanger->responseFactorsVector.size());

    auto &thisRF(state->dataGroundHeatExchanger->responseFactorsVector[0]);

    EXPECT_EQ("GHE-1 G-FUNCTIONS", thisRF->name);
    EXPECT_EQ("GHE-1 PROPS", thisRF->props->name);
    EXPECT_EQ(4, thisRF->numBoreholes);
    EXPECT_EQ(0.00043, thisRF->gRefRatio);
    EXPECT_EQ(76, thisRF->numGFuncPairs);
    EXPECT_EQ(-15.585075, thisRF->LNTTS[0]);
    EXPECT_EQ(-2.672011, thisRF->GFNC[0]);
    EXPECT_EQ(3.003000, thisRF->LNTTS[75]);
    EXPECT_EQ(12.778359, thisRF->GFNC[75]);
}

TEST_F(EnergyPlusFixture, GroundHeatExchangerTest_System_Resp_Factors_Mismatched_Pairs)
{
    std::string const idf_objects = delimited_string({
        "GroundHeatExchanger:Vertical:Properties,",
        "  GHE-1 Props,             !- Name",
        "  1,                       !- Depth of Top of Borehole {m}",
        "  100,                     !- Borehole Length {m}",
        "  0.109982,                !- Borehole Diameter {m}",
        "  0.744,                   !- Grout Thermal Conductivity {W/m-K}",
        "  3900000.0,               !- Grout Thermal Heat Capacity {J/m3-K}",
        "  0.389,                   !- Pipe Thermal Conductivity {W/m-K}",
        "  1770000.0,               !- Pipe Thermal Heat Capacity {J/m3-K}",
        "  0.0267,                  !- Pipe Outer Diameter {m}",
        "  0.00243,                 !- Pipe Thickness {m}",
        "  0.04556;                 !- U-Tube Distance {m}",

        "GroundHeatExchanger:ResponseFactors,",
        "  GHE-1 g-functions,       !- Name",
        "  GHE-1 Props,             !- GHE:Vertical:Properties Object Name",
        "  4,                       !- Number of Boreholes",
        "  0.00043,                 !- G-Function Reference Ratio {dimensionless}",
        "  -15.585075,              !- g-Function Ln(T/Ts) Value",
        "  -2.672011,               !- g-Function g Value",
        "  2.275,                   !- g-Function Ln(T/Ts) Value",
        "  12.729288,               !- g-Function g Value",
        "  3.003,                   !- g-Function Ln(T/Ts) Value",
        "  ;                        !- g-Function g Value",
    });

    EXPECT_FALSE(process_idf(idf_objects, false));
}

TEST_F(EnergyPlusFixture, GroundHeatExchangerTest_System_Vertical_Array_IDF_Check)
{
    std::string const idf_objects = delimited_string({
        "GroundHeatExchanger:Vertical:Properties,",
        "  GHE-1 Props,             !- Name",
        "  1,                       !- Depth of Top of Borehole {m}",
        "  100,                     !- Borehole Length {m}",
        "  0.109982,                !- Borehole Diameter {m}",
        "  0.744,                   !- Grout Thermal Conductivity {W/m-K}",
        "  3900000.0,               !- Grout Thermal Heat Capacity {J/m3-K}",
        "  0.389,                   !- Pipe Thermal Conductivity {W/m-K}",
        "  1770000.0,               !- Pipe Thermal Heat Capacity {J/m3-K}",
        "  0.0267,                  !- Pipe Outer Diameter {m}",
        "  0.00243,                 !- Pipe Thickness {m}",
        "  0.04556;                 !- U-Tube Distance {m}",

        "GroundHeatExchanger:Vertical:Array,",
        "  GHE-Array,               !- Name",
        "  GHE-1 Props,             !- GHE:Vertical:Properties Object Name",
        "  2,                       !- Number of Boreholes in X-Direction",
        "  2,                       !- Number of Boreholes in Y-Direction",
        "  2;                       !- Borehole Spacing {m}",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    GetGroundHeatExchangerInput(*state);

    EXPECT_EQ(1u, state->dataGroundHeatExchanger->vertArraysVector.size());

    auto &thisArray(state->dataGroundHeatExchanger->vertArraysVector[0]);

    EXPECT_EQ("GHE-ARRAY", thisArray->name);
    EXPECT_EQ("GHE-1 PROPS", thisArray->props->name);
    EXPECT_EQ(2, thisArray->numBHinXDirection);
    EXPECT_EQ(2, thisArray->numBHinYDirection);
}

TEST_F(EnergyPlusFixture, GroundHeatExchangerTest_System_Given_Response_Factors_IDF_Check)
{
    std::string const idf_objects = delimited_string({
        "Site:GroundTemperature:Undisturbed:KusudaAchenbach,",
        "  KATemps,                 !- Name",
        "  1.8,                     !- Soil Thermal Conductivity {W/m-K}",
        "  920,                     !- Soil Density {kg/m3}",
        "  2200,                    !- Soil Specific Heat {J/kg-K}",
        "  15.5,                    !- Average Soil Surface Temperature {C}",
        "  3.2,                     !- Average Amplitude of Surface Temperature {deltaC}",
        "  8;                       !- Phase Shift of Minimum Surface Temperature {days}",

        "GroundHeatExchanger:Vertical:Properties,",
        "  GHE-1 Props,             !- Name",
        "  1,                       !- Depth of Top of Borehole {m}",
        "  100,                     !- Borehole Length {m}",
        "  0.109982,                !- Borehole Diameter {m}",
        "  0.744,                   !- Grout Thermal Conductivity {W/m-K}",
        "  3900000.0,               !- Grout Thermal Heat Capacity {J/m3-K}",
        "  0.389,                   !- Pipe Thermal Conductivity {W/m-K}",
        "  1770000.0,               !- Pipe Thermal Heat Capacity {J/m3-K}",
        "  0.0267,                  !- Pipe Outer Diameter {m}",
        "  0.00243,                 !- Pipe Thickness {m}",
        "  0.04556;                 !- U-Tube Distance {m}",

        "GroundHeatExchanger:ResponseFactors,",
        "  GHE-1 g-functions,       !- Name",
        "  GHE-1 Props,             !- GHE:Vertical:Properties Object Name",
        "  4,                       !- Number of Boreholes",
        "  0.00043,                 !- G-Function Reference Ratio {dimensionless}",
        "  -15.585075,              !- g-Function Ln(T/Ts) Value",
        "  -2.672011,               !- g-Function g Value",
        "  -15.440481,              !- g-Function Ln(T/Ts) Value",
        "  -2.575897,               !- g-Function g Value",
        "  -15.295888,              !- g-Function Ln(T/Ts) Value",
        "  -2.476279,               !- g-Function g Value",
        "  -15.151295,              !- g-Function Ln(T/Ts) Value",
        "  -2.372609,               !- g-Function g Value",
        "  -15.006701,              !- g-Function Ln(T/Ts) Value",
        "  -2.264564,               !- g-Function g Value",
        "  -14.862108,              !- g-Function Ln(T/Ts) Value",
        "  -2.151959,               !- g-Function g Value",
        "  -14.717515,              !- g-Function Ln(T/Ts) Value",
        "  -2.034708,               !- g-Function g Value",
        "  -14.572921,              !- g-Function Ln(T/Ts) Value",
        "  -1.912801,               !- g-Function g Value",
        "  -14.428328,              !- g-Function Ln(T/Ts) Value",
        "  -1.786299,               !- g-Function g Value",
        "  -14.283734,              !- g-Function Ln(T/Ts) Value",
        "  -1.655324,               !- g-Function g Value",
        "  -14.139141,              !- g-Function Ln(T/Ts) Value",
        "  -1.520066,               !- g-Function g Value",
        "  -13.994548,              !- g-Function Ln(T/Ts) Value",
        "  -1.380782,               !- g-Function g Value",
        "  -13.849954,              !- g-Function Ln(T/Ts) Value",
        "  -1.237813,               !- g-Function g Value",
        "  -13.705361,              !- g-Function Ln(T/Ts) Value",
        "  -1.091594,               !- g-Function g Value",
        "  -13.560768,              !- g-Function Ln(T/Ts) Value",
        "  -0.94267,                !- g-Function g Value",
        "  -13.416174,              !- g-Function Ln(T/Ts) Value",
        "  -0.791704,               !- g-Function g Value",
        "  -13.271581,              !- g-Function Ln(T/Ts) Value",
        "  -0.639479,               !- g-Function g Value",
        "  -13.126988,              !- g-Function Ln(T/Ts) Value",
        "  -0.486879,               !- g-Function g Value",
        "  -12.982394,              !- g-Function Ln(T/Ts) Value",
        "  -0.334866,               !- g-Function g Value",
        "  -12.837801,              !- g-Function Ln(T/Ts) Value",
        "  -0.184431,               !- g-Function g Value",
        "  -12.693207,              !- g-Function Ln(T/Ts) Value",
        "  -0.036546,               !- g-Function g Value",
        "  -12.548614,              !- g-Function Ln(T/Ts) Value",
        "  0.107892,                !- g-Function g Value",
        "  -12.404021,              !- g-Function Ln(T/Ts) Value",
        "  0.248115,                !- g-Function g Value",
        "  -12.259427,              !- g-Function Ln(T/Ts) Value",
        "  0.38352,                 !- g-Function g Value",
        "  -12.114834,              !- g-Function Ln(T/Ts) Value",
        "  0.5137,                  !- g-Function g Value",
        "  -11.970241,              !- g-Function Ln(T/Ts) Value",
        "  0.63845,                 !- g-Function g Value",
        "  -11.825647,              !- g-Function Ln(T/Ts) Value",
        "  0.757758,                !- g-Function g Value",
        "  -11.681054,              !- g-Function Ln(T/Ts) Value",
        "  0.87178,                 !- g-Function g Value",
        "  -11.536461,              !- g-Function Ln(T/Ts) Value",
        "  0.980805,                !- g-Function g Value",
        "  -11.391867,              !- g-Function Ln(T/Ts) Value",
        "  1.085218,                !- g-Function g Value",
        "  -11.247274,              !- g-Function Ln(T/Ts) Value",
        "  1.185457,                !- g-Function g Value",
        "  -11.10268,               !- g-Function Ln(T/Ts) Value",
        "  1.28198,                 !- g-Function g Value",
        "  -10.958087,              !- g-Function Ln(T/Ts) Value",
        "  1.375237,                !- g-Function g Value",
        "  -10.813494,              !- g-Function Ln(T/Ts) Value",
        "  1.465651,                !- g-Function g Value",
        "  -10.6689,                !- g-Function Ln(T/Ts) Value",
        "  1.553606,                !- g-Function g Value",
        "  -10.524307,              !- g-Function Ln(T/Ts) Value",
        "  1.639445,                !- g-Function g Value",
        "  -10.379714,              !- g-Function Ln(T/Ts) Value",
        "  1.723466,                !- g-Function g Value",
        "  -10.23512,               !- g-Function Ln(T/Ts) Value",
        "  1.805924,                !- g-Function g Value",
        "  -10.090527,              !- g-Function Ln(T/Ts) Value",
        "  1.887041,                !- g-Function g Value",
        "  -9.945934,               !- g-Function Ln(T/Ts) Value",
        "  1.967002,                !- g-Function g Value",
        "  -9.80134,                !- g-Function Ln(T/Ts) Value",
        "  2.045967,                !- g-Function g Value",
        "  -9.656747,               !- g-Function Ln(T/Ts) Value",
        "  2.124073,                !- g-Function g Value",
        "  -9.512154,               !- g-Function Ln(T/Ts) Value",
        "  2.201436,                !- g-Function g Value",
        "  -9.36756,                !- g-Function Ln(T/Ts) Value",
        "  2.278154,                !- g-Function g Value",
        "  -9.222967,               !- g-Function Ln(T/Ts) Value",
        "  2.354312,                !- g-Function g Value",
        "  -9.078373,               !- g-Function Ln(T/Ts) Value",
        "  2.429984,                !- g-Function g Value",
        "  -8.93378,                !- g-Function Ln(T/Ts) Value",
        "  2.505232,                !- g-Function g Value",
        "  -8.789187,               !- g-Function Ln(T/Ts) Value",
        "  2.580112,                !- g-Function g Value",
        "  -8.644593,               !- g-Function Ln(T/Ts) Value",
        "  2.654669,                !- g-Function g Value",
        "  -8.5,                    !- g-Function Ln(T/Ts) Value",
        "  2.830857,                !- g-Function g Value",
        "  -7.8,                    !- g-Function Ln(T/Ts) Value",
        "  3.176174,                !- g-Function g Value",
        "  -7.2,                    !- g-Function Ln(T/Ts) Value",
        "  3.484017,                !- g-Function g Value",
        "  -6.5,                    !- g-Function Ln(T/Ts) Value",
        "  3.88777,                 !- g-Function g Value",
        "  -5.9,                    !- g-Function Ln(T/Ts) Value",
        "  4.311301,                !- g-Function g Value",
        "  -5.2,                    !- g-Function Ln(T/Ts) Value",
        "  4.928223,                !- g-Function g Value",
        "  -4.5,                    !- g-Function Ln(T/Ts) Value",
        "  5.696283,                !- g-Function g Value",
        "  -3.963,                  !- g-Function Ln(T/Ts) Value",
        "  6.361422,                !- g-Function g Value",
        "  -3.27,                   !- g-Function Ln(T/Ts) Value",
        "  7.375959,                !- g-Function g Value",
        "  -2.864,                  !- g-Function Ln(T/Ts) Value",
        "  7.994729,                !- g-Function g Value",
        "  -2.577,                  !- g-Function Ln(T/Ts) Value",
        "  8.438474,                !- g-Function g Value",
        "  -2.171,                  !- g-Function Ln(T/Ts) Value",
        "  9.059916,                !- g-Function g Value",
        "  -1.884,                  !- g-Function Ln(T/Ts) Value",
        "  9.492228,                !- g-Function g Value",
        "  -1.191,                  !- g-Function Ln(T/Ts) Value",
        "  10.444276,               !- g-Function g Value",
        "  -0.497,                  !- g-Function Ln(T/Ts) Value",
        "  11.292233,               !- g-Function g Value",
        "  -0.274,                  !- g-Function Ln(T/Ts) Value",
        "  11.525537,               !- g-Function g Value",
        "  -0.051,                  !- g-Function Ln(T/Ts) Value",
        "  11.735157,               !- g-Function g Value",
        "  0.196,                   !- g-Function Ln(T/Ts) Value",
        "  11.942392,               !- g-Function g Value",
        "  0.419,                   !- g-Function Ln(T/Ts) Value",
        "  12.103282,               !- g-Function g Value",
        "  0.642,                   !- g-Function Ln(T/Ts) Value",
        "  12.243398,               !- g-Function g Value",
        "  0.873,                   !- g-Function Ln(T/Ts) Value",
        "  12.365217,               !- g-Function g Value",
        "  1.112,                   !- g-Function Ln(T/Ts) Value",
        "  12.469007,               !- g-Function g Value",
        "  1.335,                   !- g-Function Ln(T/Ts) Value",
        "  12.547123,               !- g-Function g Value",
        "  1.679,                   !- g-Function Ln(T/Ts) Value",
        "  12.63789,                !- g-Function g Value",
        "  2.028,                   !- g-Function Ln(T/Ts) Value",
        "  12.699245,               !- g-Function g Value",
        "  2.275,                   !- g-Function Ln(T/Ts) Value",
        "  12.729288,               !- g-Function g Value",
        "  3.003,                   !- g-Function Ln(T/Ts) Value",
        "  12.778359;               !- g-Function g Value",

        "GroundHeatExchanger:System,",
        "  Vertical GHE 1x4 Std,    !- Name",
        "  GHLE Inlet,              !- Inlet Node Name",
        "  GHLE Outlet,             !- Outlet Node Name",
        "  0.0007571,               !- Design Flow Rate {m3/s}",
        "  Site:GroundTemperature:Undisturbed:KusudaAchenbach,  !- Undisturbed Ground Temperature Model Type",
        "  KATemps,                 !- Undisturbed Ground Temperature Model Name",
        "  2.423,                   !- Ground Thermal Conductivity {W/m-K}",
        "  2343000.0,               !- Ground Thermal Heat Capacity {J/m3-K}",
        "  GHE-1 g-functions;       !- GHE:Vertical:ResponseFactors Object Name",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    GetGroundHeatExchangerInput(*state);

    EXPECT_EQ(1u, state->dataGroundHeatExchanger->vertPropsVector.size());
    EXPECT_EQ(1u, state->dataGroundHeatExchanger->responseFactorsVector.size());
    EXPECT_EQ(1u, state->dataGroundHeatExchanger->verticalGLHE.size());

    auto &thisRF(state->dataGroundHeatExchanger->responseFactorsVector[0]);
    auto &thisGLHE(state->dataGroundHeatExchanger->verticalGLHE[0]);

    EXPECT_EQ("VERTICAL GHE 1X4 STD", thisGLHE.name);
    EXPECT_EQ(true, thisGLHE.available);
    EXPECT_EQ(true, thisGLHE.on);
    EXPECT_EQ(2.423, thisGLHE.soil.k);
    EXPECT_EQ(2.343E6, thisGLHE.soil.rhoCp);
    EXPECT_EQ(GetResponseFactor(*state, thisRF->name).get(), thisGLHE.myRespFactors.get());
    EXPECT_EQ(0.109982, thisGLHE.bhDiameter);
    EXPECT_EQ(0.109982 / 2, thisGLHE.bhRadius);
    EXPECT_EQ(100, thisGLHE.bhLength);
    EXPECT_EQ(0.04556, thisGLHE.bhUTubeDist);
    EXPECT_EQ(0, thisGLHE.myRespFactors->maxSimYears);
    EXPECT_EQ(400, thisGLHE.totalTubeLength);
    EXPECT_EQ(thisGLHE.soil.k / thisGLHE.soil.rhoCp, thisGLHE.soil.diffusivity);

    auto &orp = *state->dataOutRptPredefined;
    std::string const GLHEName = thisGLHE.name;
    EXPECT_EQ("GroundHeatExchanger:System", OutputReportPredefined::RetrievePreDefTableEntry(*state, orp.pdchGLHEType, GLHEName));
    EXPECT_EQ("400.00", OutputReportPredefined::RetrievePreDefTableEntry(*state, orp.pdchGLHETubeLength, GLHEName));
    EXPECT_EQ("0.000757", OutputReportPredefined::RetrievePreDefTableEntry(*state, orp.pdchGLHEVolFlow, GLHEName));
    EXPECT_EQ("1.00", OutputReportPredefined::RetrievePreDefTableEntry(*state, orp.pdchGLHEbhDepth, GLHEName));
    EXPECT_EQ("0.11", OutputReportPredefined::RetrievePreDefTableEntry(*state, orp.pdchGLHEbhDiam, GLHEName));
    EXPECT_EQ("100.00", OutputReportPredefined::RetrievePreDefTableEntry(*state, orp.pdchGLHEbhLeng, GLHEName));
    EXPECT_EQ("4.00", OutputReportPredefined::RetrievePreDefTableEntry(*state, orp.pdchGLHENumHolesTrenches, GLHEName));
}

TEST_F(EnergyPlusFixture, GroundHeatExchangerTest_System_Given_Array_IDF_Check)
{
    std::string const idf_objects = delimited_string({
        "Site:GroundTemperature:Undisturbed:KusudaAchenbach,",
        "  KATemps,                 !- Name",
        "  1.8,                     !- Soil Thermal Conductivity {W/m-K}",
        "  920,                     !- Soil Density {kg/m3}",
        "  2200,                    !- Soil Specific Heat {J/kg-K}",
        "  15.5,                    !- Average Soil Surface Temperature {C}",
        "  3.2,                     !- Average Amplitude of Surface Temperature {deltaC}",
        "  8;                       !- Phase Shift of Minimum Surface Temperature {days}",

        "GroundHeatExchanger:Vertical:Properties,",
        "  GHE-1 Props,             !- Name",
        "  1,                       !- Depth of Top of Borehole {m}",
        "  100,                     !- Borehole Length {m}",
        "  0.109982,                !- Borehole Diameter {m}",
        "  0.744,                   !- Grout Thermal Conductivity {W/m-K}",
        "  3900000.0,               !- Grout Thermal Heat Capacity {J/m3-K}",
        "  0.389,                   !- Pipe Thermal Conductivity {W/m-K}",
        "  1770000.0,               !- Pipe Thermal Heat Capacity {J/m3-K}",
        "  0.0267,                  !- Pipe Outer Diameter {m}",
        "  0.00243,                 !- Pipe Thickness {m}",
        "  0.04556;                 !- U-Tube Distance {m}",

        "GroundHeatExchanger:Vertical:Array,",
        "  GHE-Array,               !- Name",
        "  GHE-1 Props,             !- GHE:Vertical:Properties Object Name",
        "  2,                       !- Number of Boreholes in X-Direction",
        "  2,                       !- Number of Boreholes in Y-Direction",
        "  2;                       !- Borehole Spacing {m}",

        "GroundHeatExchanger:System,",
        "  Vertical GHE 1x4 Std,    !- Name",
        "  GHLE Inlet,              !- Inlet Node Name",
        "  GHLE Outlet,             !- Outlet Node Name",
        "  0.0007571,               !- Design Flow Rate {m3/s}",
        "  Site:GroundTemperature:Undisturbed:KusudaAchenbach,  !- Undisturbed Ground Temperature Model Type",
        "  KATemps,                 !- Undisturbed Ground Temperature Model Name",
        "  2.423,                   !- Ground Thermal Conductivity {W/m-K}",
        "  2343000.0,               !- Ground Thermal Heat Capacity {J/m3-K}",
        "  ,                        !- GHE:Vertical:ResponseFactors Object Name",
        "  UHFcalc,                 !- g-Function Calculation Method",
        "  ,                        !- GHE:Vertical:Sizing Object Type",
        "  ,                        !- GHE:Vertical:Sizing Object Name",
        "  GHE-Array;               !- GHE:Vertical:Array Object Name",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    GetGroundHeatExchangerInput(*state);

    EXPECT_EQ(1u, state->dataGroundHeatExchanger->vertPropsVector.size());
    EXPECT_EQ(1u, state->dataGroundHeatExchanger->vertArraysVector.size());
    EXPECT_EQ(1u, state->dataGroundHeatExchanger->verticalGLHE.size());

    auto &thisArray(state->dataGroundHeatExchanger->vertArraysVector[0]);
    auto &thisGLHE(state->dataGroundHeatExchanger->verticalGLHE[0]);

    EXPECT_EQ("VERTICAL GHE 1X4 STD", thisGLHE.name);
    EXPECT_EQ(true, thisGLHE.available);
    EXPECT_EQ(true, thisGLHE.on);
    EXPECT_EQ(2.423, thisGLHE.soil.k);
    EXPECT_EQ(2.343E6, thisGLHE.soil.rhoCp);
    EXPECT_EQ(GetResponseFactor(*state, thisArray->name).get(), thisGLHE.myRespFactors.get());
    EXPECT_EQ(0.109982, thisGLHE.bhDiameter);
    EXPECT_EQ(0.109982 / 2, thisGLHE.bhRadius);
    EXPECT_EQ(100, thisGLHE.bhLength);
    EXPECT_EQ(0.04556, thisGLHE.bhUTubeDist);
    EXPECT_EQ(0, thisGLHE.myRespFactors->maxSimYears);
    EXPECT_EQ(400, thisGLHE.totalTubeLength);
    EXPECT_EQ(thisGLHE.soil.k / thisGLHE.soil.rhoCp, thisGLHE.soil.diffusivity);
}

TEST_F(EnergyPlusFixture, GroundHeatExchangerTest_System_Given_Single_BHs_IDF_Check)
{
    std::string const idf_objects = delimited_string({
        "Site:GroundTemperature:Undisturbed:KusudaAchenbach,",
        "  KATemps,                 !- Name",
        "  1.8,                     !- Soil Thermal Conductivity {W/m-K}",
        "  920,                     !- Soil Density {kg/m3}",
        "  2200,                    !- Soil Specific Heat {J/kg-K}",
        "  15.5,                    !- Average Soil Surface Temperature {C}",
        "  3.2,                     !- Average Amplitude of Surface Temperature {deltaC}",
        "  8;                       !- Phase Shift of Minimum Surface Temperature {days}",

        "GroundHeatExchanger:Vertical:Properties,",
        "  GHE-1 Props,             !- Name",
        "  1,                       !- Depth of Top of Borehole {m}",
        "  100,                     !- Borehole Length {m}",
        "  0.109982,                !- Borehole Diameter {m}",
        "  0.744,                   !- Grout Thermal Conductivity {W/m-K}",
        "  3900000.0,               !- Grout Thermal Heat Capacity {J/m3-K}",
        "  0.389,                   !- Pipe Thermal Conductivity {W/m-K}",
        "  1770000.0,               !- Pipe Thermal Heat Capacity {J/m3-K}",
        "  0.0267,                  !- Pipe Outer Diameter {m}",
        "  0.00243,                 !- Pipe Thickness {m}",
        "  0.04556;                 !- U-Tube Distance {m}",

        "GroundHeatExchanger:Vertical:Single,",
        "  GHE-1,                   !- Name",
        "  GHE-1 Props,             !- GHE:Vertical:Properties Object Name",
        "  0,                       !- X-Location {m}",
        "  0;                       !- Y-Location {m}",

        "GroundHeatExchanger:Vertical:Single,",
        "  GHE-2,                   !- Name",
        "  GHE-1 Props,             !- GHE:Vertical:Properties Object Name",
        "  5.5,                     !- X-Location {m}",
        "  0;                       !- Y-Location {m}",

        "GroundHeatExchanger:Vertical:Single,",
        "  GHE-3,                   !- Name",
        "  GHE-1 Props,             !- GHE:Vertical:Properties Object Name",
        "  0,                       !- X-Location {m}",
        "  5.5;                     !- Y-Location {m}",

        "GroundHeatExchanger:Vertical:Single,",
        "  GHE-4,                   !- Name",
        "  GHE-1 Props,             !- GHE:Vertical:Properties Object Name",
        "  5.5,                     !- X-Location {m}",
        "  5.5;                     !- Y-Location {m}",

        "GroundHeatExchanger:Vertical:Single,",
        "  UNUSED,                  !- Name",
        "  GHE-1 Props,             !- GHE:Vertical:Properties Object Name",
        "  8.0,                     !- X-Location {m}",
        "  8.0;                     !- Y-Location {m}",

        "GroundHeatExchanger:System,",
        "  Vertical GHE 1x4 Std,    !- Name",
        "  GHLE Inlet,              !- Inlet Node Name",
        "  GHLE Outlet,             !- Outlet Node Name",
        "  0.0007571,               !- Design Flow Rate {m3/s}",
        "  Site:GroundTemperature:Undisturbed:KusudaAchenbach,  !- Undisturbed Ground Temperature Model Type",
        "  KATemps,                 !- Undisturbed Ground Temperature Model Name",
        "  2.423,                   !- Ground Thermal Conductivity {W/m-K}",
        "  2343000.0,               !- Ground Thermal Heat Capacity {J/m3-K}",
        "  ,                        !- GHE:Vertical:ResponseFactors Object Name",
        "  UHFcalc,                 !- g-Function Calculation Method",
        "  ,                        !- GHE:Vertical:Sizing Object Type",
        "  ,                        !- GHE:Vertical:Sizing Object Name",
        "  ,                        !- GHE:Vertical:Array Object Name",
        "  GHE-1,                   !- GHE:Vertical:Single Object Name",
        "  GHE-2,                   !- GHE:Vertical:Single Object Name",
        "  GHE-3,                   !- GHE:Vertical:Single Object Name",
        "  GHE-4;                   !- GHE:Vertical:Single Object Name",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    GetGroundHeatExchangerInput(*state);

    EXPECT_EQ(2u, state->dataGroundHeatExchanger->vertPropsVector.size());
    EXPECT_EQ(5u, state->dataGroundHeatExchanger->singleBoreholesVector.size());
    EXPECT_EQ(1u, state->dataGroundHeatExchanger->verticalGLHE.size());

    auto &thisGLHE(state->dataGroundHeatExchanger->verticalGLHE[0]);

    EXPECT_EQ("VERTICAL GHE 1X4 STD", thisGLHE.name);
    EXPECT_EQ(true, thisGLHE.available);
    EXPECT_EQ(true, thisGLHE.on);
    EXPECT_EQ(2.423, thisGLHE.soil.k);
    EXPECT_EQ(2.343E6, thisGLHE.soil.rhoCp);
    EXPECT_EQ(0.109982, thisGLHE.bhDiameter);
    EXPECT_EQ(0.109982 / 2, thisGLHE.bhRadius);
    EXPECT_EQ(100, thisGLHE.bhLength);
    EXPECT_EQ(0.04556, thisGLHE.bhUTubeDist);
    EXPECT_EQ(0, thisGLHE.myRespFactors->maxSimYears);
    EXPECT_EQ(4, thisGLHE.myRespFactors->numBoreholes);
    EXPECT_EQ(400, thisGLHE.totalTubeLength);
    EXPECT_EQ(thisGLHE.soil.k / thisGLHE.soil.rhoCp, thisGLHE.soil.diffusivity);
}

TEST_F(EnergyPlusFixture, GroundHeatExchangerTest_System_calcGFunction_UHF)
{
    using namespace DataSystemVariables;

    std::string const idf_objects = delimited_string({
        "Site:GroundTemperature:Undisturbed:KusudaAchenbach,",
        "  KATemps,                 !- Name",
        "  1.8,                     !- Soil Thermal Conductivity {W/m-K}",
        "  920,                     !- Soil Density {kg/m3}",
        "  2200,                    !- Soil Specific Heat {J/kg-K}",
        "  15.5,                    !- Average Soil Surface Temperature {C}",
        "  3.2,                     !- Average Amplitude of Surface Temperature {deltaC}",
        "  8;                       !- Phase Shift of Minimum Surface Temperature {days}",

        "GroundHeatExchanger:Vertical:Properties,",
        "  GHE-1 Props,             !- Name",
        "  1,                       !- Depth of Top of Borehole {m}",
        "  100,                     !- Borehole Length {m}",
        "  0.109982,                !- Borehole Diameter {m}",
        "  0.744,                   !- Grout Thermal Conductivity {W/m-K}",
        "  3900000.0,               !- Grout Thermal Heat Capacity {J/m3-K}",
        "  0.389,                   !- Pipe Thermal Conductivity {W/m-K}",
        "  1770000.0,               !- Pipe Thermal Heat Capacity {J/m3-K}",
        "  0.0267,                  !- Pipe Outer Diameter {m}",
        "  0.00243,                 !- Pipe Thickness {m}",
        "  0.04556;                 !- U-Tube Distance {m}",

        "GroundHeatExchanger:Vertical:Single,",
        "  GHE-1,                   !- Name",
        "  GHE-1 Props,             !- GHE:Vertical:Properties Object Name",
        "  0,                       !- X-Location {m}",
        "  0;                       !- Y-Location {m}",

        "GroundHeatExchanger:Vertical:Single,",
        "  GHE-2,                   !- Name",
        "  GHE-1 Props,             !- GHE:Vertical:Properties Object Name",
        "  5.0,                     !- X-Location {m}",
        "  0;                       !- Y-Location {m}",

        "GroundHeatExchanger:Vertical:Single,",
        "  GHE-3,                   !- Name",
        "  GHE-1 Props,             !- GHE:Vertical:Properties Object Name",
        "  0,                       !- X-Location {m}",
        "  5.0;                     !- Y-Location {m}",

        "GroundHeatExchanger:Vertical:Single,",
        "  GHE-4,                   !- Name",
        "  GHE-1 Props,             !- GHE:Vertical:Properties Object Name",
        "  5.0,                     !- X-Location {m}",
        "  5.0;                     !- Y-Location {m}",

        "GroundHeatExchanger:System,",
        "  Vertical GHE 1x4 Std,    !- Name",
        "  GHLE Inlet,              !- Inlet Node Name",
        "  GHLE Outlet,             !- Outlet Node Name",
        "  0.00075708,              !- Design Flow Rate {m3/s}",
        "  Site:GroundTemperature:Undisturbed:KusudaAchenbach,  !- Undisturbed Ground Temperature Model Type",
        "  KATemps,                 !- Undisturbed Ground Temperature Model Name",
        "  2.423,                   !- Ground Thermal Conductivity {W/m-K}",
        "  2343000.0,               !- Ground Thermal Heat Capacity {J/m3-K}",
        "  ,                        !- GHE:Vertical:ResponseFactors Object Name",
        "  UHFcalc,                 !- g-Function Calculation Method",
        "  ,                        !- GHE:Vertical:Sizing Object Type",
        "  ,                        !- GHE:Vertical:Sizing Object Name",
        "  ,                        !- GHE:Vertical:Array Object Name",
        "  GHE-1,                   !- GHE:Vertical:Single Object Name",
        "  GHE-2,                   !- GHE:Vertical:Single Object Name",
        "  GHE-3,                   !- GHE:Vertical:Single Object Name",
        "  GHE-4;                   !- GHE:Vertical:Single Object Name",

        "Branch,",
        "  Main Floor Cooling Condenser Branch,  !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  Coil:Cooling:WaterToAirHeatPump:EquationFit,  !- Component Object Type",
        "  Main Floor WAHP Cooling Coil,  !- Component Name",
        "  Main Floor WAHP Cooling Water Inlet Node,  !- Component Inlet Node Name",
        "  Main Floor WAHP Cooling Water Outlet Node;  !- Component Outlet Node Name",

        "Branch,",
        "  Main Floor Heating Condenser Branch,  !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  Coil:Heating:WaterToAirHeatPump:EquationFit,  !- Component Object Type",
        "  Main Floor WAHP Heating Coil,  !- Component Name",
        "  Main Floor WAHP Heating Water Inlet Node,  !- Component Inlet Node Name",
        "  Main Floor WAHP Heating Water Outlet Node;  !- Component Outlet Node Name",

        "Branch,",
        "  GHE-Vert Branch,         !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  GroundHeatExchanger:System,  !- Component Object Type",
        "  Vertical GHE 1x4 Std,    !- Component Name",
        "  GHLE Inlet,              !- Component Inlet Node Name",
        "  GHLE Outlet;             !- Component Outlet Node Name",

        "Branch,",
        "  Ground Loop Supply Inlet Branch,  !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  Pump:ConstantSpeed,      !- Component Object Type",
        "  Ground Loop Supply Pump,  !- Component Name",
        "  Ground Loop Supply Inlet,  !- Component Inlet Node Name",
        "  Ground Loop Pump Outlet;  !- Component Outlet Node Name",

        "Branch,",
        "  Ground Loop Supply Outlet Branch,  !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  Pipe:Adiabatic,          !- Component Object Type",
        "  Ground Loop Supply Outlet Pipe,  !- Component Name",
        "  Ground Loop Supply Outlet Pipe Inlet,  !- Component Inlet Node Name",
        "  Ground Loop Supply Outlet;  !- Component Outlet Node Name",

        "Branch,",
        "  Ground Loop Demand Inlet Branch,  !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  Pipe:Adiabatic,          !- Component Object Type",
        "  Ground Loop Demand Inlet Pipe,  !- Component Name",
        "  Ground Loop Demand Inlet,  !- Component Inlet Node Name",
        "  Ground Loop Demand Inlet Pipe Outlet;  !- Component Outlet Node Name",

        "Branch,",
        "  Ground Loop Demand Bypass Branch,  !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  Pipe:Adiabatic,          !- Component Object Type",
        "  Ground Loop Demand Side Bypass Pipe,  !- Component Name",
        "  Ground Loop Demand Bypass Inlet,  !- Component Inlet Node Name",
        "  Ground Loop Demand Bypass Outlet;  !- Component Outlet Node Name",

        "Branch,",
        "  Ground Loop Demand Outlet Branch,  !- Name",
        "  ,                        !- Pressure Drop Curve Name",
        "  Pipe:Adiabatic,          !- Component Object Type",
        "  Ground Loop Demand Outlet Pipe,  !- Component Name",
        "  Ground Loop Demand Outlet Pipe Inlet,  !- Component Inlet Node Name",
        "  Ground Loop Demand Outlet;  !- Component Outlet Node Name",

        "BranchList,",
        "  Ground Loop Supply Side Branches,  !- Name",
        "  Ground Loop Supply Inlet Branch,  !- Branch Name",
        "  GHE-Vert Branch,         !- Branch Name",
        "  Ground Loop Supply Outlet Branch;  !- Branch Name",

        "BranchList,",
        "  Ground Loop Demand Side Branches,  !- Name",
        "  Ground Loop Demand Inlet Branch,  !- Branch Name",
        "  Main Floor Cooling Condenser Branch,  !- Branch Name",
        "  Main Floor Heating Condenser Branch,  !- Branch Name",
        "  Ground Loop Demand Bypass Branch,  !- Branch Name",
        "  Ground Loop Demand Outlet Branch;  !- Branch Name",

        "Connector:Splitter,",
        "  Ground Loop Supply Splitter,  !- Name",
        "  Ground Loop Supply Inlet Branch,  !- Inlet Branch Name",
        "  GHE-Vert Branch;         !- Outlet Branch Name",

        "Connector:Splitter,",
        "  Ground Loop Demand Splitter,  !- Name",
        "  Ground Loop Demand Inlet Branch,  !- Inlet Branch Name",
        "  Ground Loop Demand Bypass Branch,  !- Outlet Branch Name",
        "  Main Floor Cooling Condenser Branch,  !- Outlet Branch Name",
        "  Main Floor Heating Condenser Branch;  !- Outlet Branch Name",

        "Connector:Mixer,",
        "  Ground Loop Supply Mixer,  !- Name",
        "  Ground Loop Supply Outlet Branch,  !- Outlet Branch Name",
        "  GHE-Vert Branch;         !- Inlet Branch Name",

        "Connector:Mixer,",
        "  Ground Loop Demand Mixer,  !- Name",
        "  Ground Loop Demand Outlet Branch,  !- Outlet Branch Name",
        "  Ground Loop Demand Bypass Branch,  !- Inlet Branch Name",
        "  Main Floor Cooling Condenser Branch,  !- Inlet Branch Name",
        "  Main Floor Heating Condenser Branch;  !- Inlet Branch Name",

        "ConnectorList,",
        "  Ground Loop Supply Side Connectors,  !- Name",
        "  Connector:Splitter,      !- Connector 1 Object Type",
        "  Ground Loop Supply Splitter,  !- Connector 1 Name",
        "  Connector:Mixer,         !- Connector 2 Object Type",
        "  Ground Loop Supply Mixer;  !- Connector 2 Name",

        "ConnectorList,",
        "  Ground Loop Demand Side Connectors,  !- Name",
        "  Connector:Splitter,      !- Connector 1 Object Type",
        "  Ground Loop Demand Splitter,  !- Connector 1 Name",
        "  Connector:Mixer,         !- Connector 2 Object Type",
        "  Ground Loop Demand Mixer;  !- Connector 2 Name",

        "NodeList,",
        "  Ground Loop Supply Setpoint Nodes,  !- Name",
        "  GHLE Outlet,             !- Node Name",
        "  Ground Loop Supply Outlet;  !- Node Name",

        "OutdoorAir:Node,",
        "  Main Floor WAHP Outside Air Inlet,  !- Name",
        "  -1;                      !- Height Above Ground {m}",

        "Pipe:Adiabatic,",
        "  Ground Loop Supply Outlet Pipe,  !- Name",
        "  Ground Loop Supply Outlet Pipe Inlet,  !- Inlet Node Name",
        "  Ground Loop Supply Outlet;  !- Outlet Node Name",

        "Pipe:Adiabatic,",
        "  Ground Loop Demand Inlet Pipe,  !- Name",
        "  Ground Loop Demand Inlet,  !- Inlet Node Name",
        "  Ground Loop Demand Inlet Pipe Outlet;  !- Outlet Node Name",

        "Pipe:Adiabatic,",
        "  Ground Loop Demand Side Bypass Pipe,  !- Name",
        "  Ground Loop Demand Bypass Inlet,  !- Inlet Node Name",
        "  Ground Loop Demand Bypass Outlet;  !- Outlet Node Name",

        "Pipe:Adiabatic,",
        "  Ground Loop Demand Outlet Pipe,  !- Name",
        "  Ground Loop Demand Outlet Pipe Inlet,  !- Inlet Node Name",
        "  Ground Loop Demand Outlet;  !- Outlet Node Name",

        "Pump:ConstantSpeed,",
        "  Ground Loop Supply Pump,  !- Name",
        "  Ground Loop Supply Inlet,  !- Inlet Node Name",
        "  Ground Loop Pump Outlet,  !- Outlet Node Name",
        "  Autosize,                !- Design Flow Rate {m3/s}",
        "  179352,                  !- Design Pump Head {Pa}",
        "  Autosize,                !- Design Power Consumption {W}",
        "  0.9,                     !- Motor Efficiency",
        "  0,                       !- Fraction of Motor Inefficiencies to Fluid Stream",
        "  Intermittent;            !- Pump Control Type",

        "PlantLoop,",
        "  Ground Loop Water Loop,  !- Name",
        "  Water,                   !- Fluid Type",
        "  ,                        !- User Defined Fluid Type",
        "  Only Water Loop Operation,  !- Plant Equipment Operation Scheme Name",
        "  Ground Loop Supply Outlet,  !- Loop Temperature Setpoint Node Name",
        "  100,                     !- Maximum Loop Temperature {C}",
        "  10,                      !- Minimum Loop Temperature {C}",
        "  Autosize,                !- Maximum Loop Flow Rate {m3/s}",
        "  0,                       !- Minimum Loop Flow Rate {m3/s}",
        "  Autocalculate,           !- Plant Loop Volume {m3}",
        "  Ground Loop Supply Inlet,  !- Plant Side Inlet Node Name",
        "  Ground Loop Supply Outlet,  !- Plant Side Outlet Node Name",
        "  Ground Loop Supply Side Branches,  !- Plant Side Branch List Name",
        "  Ground Loop Supply Side Connectors,  !- Plant Side Connector List Name",
        "  Ground Loop Demand Inlet,  !- Demand Side Inlet Node Name",
        "  Ground Loop Demand Outlet,  !- Demand Side Outlet Node Name",
        "  Ground Loop Demand Side Branches,  !- Demand Side Branch List Name",
        "  Ground Loop Demand Side Connectors,  !- Demand Side Connector List Name",
        "  SequentialLoad,          !- Load Distribution Scheme",
        "  ,                        !- Availability Manager List Name",
        "  DualSetpointDeadband;    !- Plant Loop Demand Calculation Scheme",

        "PlantEquipmentList,",
        "  Only Water Loop All Cooling Equipment,  !- Name",
        "  GroundHeatExchanger:System,  !- Equipment Object Type",
        "  Vertical GHE 1x4 Std;    !- Equipment Name",

        "PlantEquipmentOperation:CoolingLoad,",
        "  Only Water Loop Cool Operation All Hours,  !- Name",
        "  0,                       !- Load Range 1 Lower Limit {W}",
        "  1000000000000000,        !- Load Range 1 Upper Limit {W}",
        "  Only Water Loop All Cooling Equipment;  !- Range 1 Equipment List Name",

        "PlantEquipmentOperationSchemes,",
        "  Only Water Loop Operation,  !- Name",
        "  PlantEquipmentOperation:CoolingLoad,  !- Control Scheme 1 Object Type",
        "  Only Water Loop Cool Operation All Hours,  !- Control Scheme 1 Name",
        "  HVACTemplate-Always 1;   !- Control Scheme 1 Schedule Name",

        "SetpointManager:Scheduled:DualSetpoint,",
        "  Ground Loop Temp Manager,  !- Name",
        "  Temperature,             !- Control Variable",
        "  HVACTemplate-Always 34,  !- High Setpoint Schedule Name",
        "  HVACTemplate-Always 20,  !- Low Setpoint Schedule Name",
        "  Ground Loop Supply Setpoint Nodes;  !- Setpoint Node or NodeList Name",

        "Schedule:Compact,",
        "  HVACTemplate-Always 4,   !- Name",
        "  HVACTemplate Any Number,  !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  4;                       !- Field",

        "Schedule:Compact,",
        "  HVACTemplate-Always 34,  !- Name",
        "  HVACTemplate Any Number,  !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  34;                      !- Field",

        "Schedule:Compact,",
        "  HVACTemplate-Always 20,  !- Name",
        "  HVACTemplate Any Number,  !- Schedule Type Limits Name",
        "  Through: 12/31,          !- Field",
        "  For: AllDays,            !- Field",
        "  Until: 24:00,            !- Field",
        "  20;                      !- Field",
    });

    // Setup
    ASSERT_TRUE(process_idf(idf_objects));
    state->init_state(*state);

    PlantManager::GetPlantLoopData(*state);
    PlantManager::GetPlantInput(*state);
    PlantManager::SetupInitialPlantCallingOrder(*state);
    PlantManager::SetupBranchControlTypes(*state);

    auto &thisGLHE(state->dataGroundHeatExchanger->verticalGLHE[0]);
    thisGLHE.plantLoc.loopNum = 1;
    state->dataLoopNodes->Node(thisGLHE.inletNodeNum).Temp = 20;
    thisGLHE.designFlow = 0.00075708;

    Real64 rho = 998.207; // Density at 20 C using CoolProp
    thisGLHE.designMassFlow = thisGLHE.designFlow * rho;

    thisGLHE.myRespFactors->maxSimYears = 1;

    thisGLHE.calcGFunctions(*state);

    Real64 constexpr tolerance = 0.1;

    // Test g-function values from GLHEPro
    EXPECT_NEAR(thisGLHE.interpGFunc(-11.939864), 0.37, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-11.802269), 0.48, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-11.664675), 0.59, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-11.52708), 0.69, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-11.389486), 0.79, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-11.251891), 0.89, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-11.114296), 0.99, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-10.976702), 1.09, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-10.839107), 1.18, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-10.701513), 1.27, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-10.563918), 1.36, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-10.426324), 1.44, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-10.288729), 1.53, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-10.151135), 1.61, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-10.01354), 1.69, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-9.875946), 1.77, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-9.738351), 1.85, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-9.600756), 1.93, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-9.463162), 2.00, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-9.325567), 2.08, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-9.187973), 2.15, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-9.050378), 2.23, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-8.912784), 2.30, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-8.775189), 2.37, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-8.637595), 2.45, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-8.5), 2.53, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-7.8), 2.87, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-7.2), 3.17, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-6.5), 3.52, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-5.9), 3.85, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-5.2), 4.37, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-4.5), 5.11, tolerance);
    EXPECT_NEAR(thisGLHE.interpGFunc(-3.963), 5.82, tolerance);
}
