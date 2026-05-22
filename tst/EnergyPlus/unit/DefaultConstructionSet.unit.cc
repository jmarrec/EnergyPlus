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
#include <EnergyPlus/Data/EnergyPlusData.hh>
#include <EnergyPlus/DataHeatBalance.hh>
#include <EnergyPlus/DataSurfaces.hh>
#include <EnergyPlus/DefaultConstructionSet.hh>
#include <EnergyPlus/HeatBalanceManager.hh>
#include <EnergyPlus/Material.hh>

#include "Fixtures/EnergyPlusFixture.hh"

using namespace EnergyPlus;

namespace {

// Minimal IDF preamble: a few NoMass constructions covering each surface/subsurface type.
// Using NoMass to keep it short; the parser only needs valid construction names.
constexpr std::string_view idf_constructions = R"(
  Material:NoMass,
    Mat Opaque,              !- Name
    Rough,                   !- Roughness
    0.5,                     !- Thermal Resistance {m2-K/W}
    0.9, 0.9, 0.9;           !- Thermal/Solar/Visible Absorptance

  WindowMaterial:SimpleGlazingSystem,
    Mat Glazing,             !- Name
    2.0,                     !- U-Factor {W/m2-K}
    0.4;                     !- Solar Heat Gain Coefficient

  Construction,
    Constr Wall,             !- Name
    Mat Opaque;              !- Layer 1

  Construction,
    Constr Floor,            !- Name
    Mat Opaque;              !- Layer 1

  Construction,
    Constr Roof,             !- Name
    Mat Opaque;              !- Layer 1

  Construction,
    Constr Door,             !- Name
    Mat Opaque;              !- Layer 1

  Construction,
    Constr Window,           !- Name
    Mat Glazing;             !- Layer 1

  Construction,
    Constr Adiabatic,        !- Name
    Mat Opaque;              !- Layer 1

  Construction,
    Constr IntPartition,     !- Name
    Mat Opaque;              !- Layer 1
)";

void loadConstructions(EnergyPlusData &state)
{
    bool ErrorsFound = false;
    HeatBalanceManager::GetFrameAndDividerData(state);
    HeatBalanceManager::SetPreConstructionInputParameters(state);
    Material::GetMaterialData(state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);
    HeatBalanceManager::GetConstructData(state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);
}

} // namespace

// ---------------------------------------------------------------------------
// Happy path: all three objects parse, DefaultConstructionSet links correctly
// ---------------------------------------------------------------------------
TEST_F(EnergyPlusFixture, DefaultConstructionSet_ParseAll)
{
    std::string const idf_objects = std::string(idf_constructions) + R"(
  DefaultSubSurfaceConstructions,
    Ext SubSurf Constrs,     !- Name
    Constr Window,           !- Fixed Window Construction Name
    Constr Window,           !- Operable Window Construction Name
    Constr Door,             !- Door Construction Name
    Constr Window,           !- Glass Door Construction Name
    Constr Door,             !- Overhead Door Construction Name
    Constr Window;           !- Skylight Construction Name

  DefaultSurfaceConstructions,
    Ext Surf Constrs,        !- Name
    Constr Floor,            !- Floor Construction Name
    Constr Wall,             !- Wall Construction Name
    Constr Roof;             !- Roof Ceiling Construction Name

  DefaultSurfaceConstructions,
    Int Surf Constrs,        !- Name
    Constr Floor,            !- Floor Construction Name
    Constr Wall,             !- Wall Construction Name
    Constr Roof;             !- Roof Ceiling Construction Name

  DefaultConstructionSet,
    My DCS,                  !- Name
    Ext Surf Constrs,        !- Default Exterior Surface Constructions Name
    Int Surf Constrs,        !- Default Interior Surface Constructions Name
    ,                        !- Default Ground Contact Surface Constructions Name
    Ext SubSurf Constrs,     !- Default Exterior SubSurface Constructions Name
    ,                        !- Default Interior SubSurface Constructions Name
    Constr IntPartition,     !- Interior Partition Construction Name
    Constr Adiabatic;        !- Adiabatic Surface Construction Name
)";

    ASSERT_TRUE(process_idf(idf_objects));
    state->init_state(*state);
    loadConstructions(*state);

    bool ErrorsFound = false;
    DefaultConstructions::GetDefaultConstructionSetData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    auto &s_dc = state->dataDefaultConstructions;

    // Sub-surface set
    ASSERT_EQ(1u, s_dc->defaultSubSurfaceConstructions.size());
    auto const &ssc = s_dc->defaultSubSurfaceConstructions[0];
    EXPECT_EQ("EXT SUBSURF CONSTRS", ssc.Name);
    EXPECT_GT(ssc.fixedWindowConstrNum, 0);
    EXPECT_GT(ssc.operableWindowConstrNum, 0);
    EXPECT_GT(ssc.doorConstrNum, 0);
    EXPECT_GT(ssc.glassDoorConstrNum, 0);
    EXPECT_GT(ssc.overheadDoorConstrNum, 0);
    EXPECT_GT(ssc.skylightConstrNum, 0);
    EXPECT_EQ(0, ssc.tddDomeConstrNum);     // not specified
    EXPECT_EQ(0, ssc.tddDiffuserConstrNum); // not specified

    // Surface sets
    ASSERT_EQ(2u, s_dc->defaultSurfaceConstructions.size());
    EXPECT_EQ("EXT SURF CONSTRS", s_dc->defaultSurfaceConstructions[0].Name);
    EXPECT_EQ("INT SURF CONSTRS", s_dc->defaultSurfaceConstructions[1].Name);

    // Construction set
    ASSERT_EQ(1u, s_dc->defaultConstructionSets.size());
    auto const &dcs = s_dc->defaultConstructionSets[0];
    EXPECT_EQ("MY DCS", dcs.Name);

    ASSERT_NE(nullptr, dcs.exteriorSurfConstructions);
    EXPECT_EQ("EXT SURF CONSTRS", dcs.exteriorSurfConstructions->Name);
    EXPECT_GT(dcs.exteriorSurfConstructions->wallConstrNum, 0);
    EXPECT_GT(dcs.exteriorSurfConstructions->floorConstrNum, 0);
    EXPECT_GT(dcs.exteriorSurfConstructions->roofCeilingConstrNum, 0);

    ASSERT_NE(nullptr, dcs.interiorSurfConstructions);
    EXPECT_EQ("INT SURF CONSTRS", dcs.interiorSurfConstructions->Name);

    EXPECT_EQ(nullptr, dcs.groundContactSurfConstructions); // blank -> nullptr

    ASSERT_NE(nullptr, dcs.exteriorSubSurfConstructions);
    EXPECT_EQ("EXT SUBSURF CONSTRS", dcs.exteriorSubSurfConstructions->Name);

    EXPECT_EQ(nullptr, dcs.interiorSubSurfConstructions); // blank -> nullptr

    EXPECT_GT(dcs.interiorPartitionConstrNum, 0);
    EXPECT_GT(dcs.adiabaticSurfConstrNum, 0);
}

// ---------------------------------------------------------------------------
// Bad construction name in DefaultSubSurfaceConstructions -> error
// ---------------------------------------------------------------------------
TEST_F(EnergyPlusFixture, DefaultConstructionSet_BadConstructionName)
{
    std::string const idf_objects = std::string(idf_constructions) + R"(
  DefaultSubSurfaceConstructions,
    Bad SubSurf,             !- Name
    NONEXISTENT CONSTR,      !- Fixed Window Construction Name
    ,                        !- Operable Window Construction Name
    ,                        !- Door Construction Name
    ,                        !- Glass Door Construction Name
    ,                        !- Overhead Door Construction Name
    ;                        !- Skylight Construction Name
)";

    ASSERT_TRUE(process_idf(idf_objects));
    state->init_state(*state);
    loadConstructions(*state);

    bool ErrorsFound = false;
    DefaultConstructions::GetDefaultConstructionSetData(*state, ErrorsFound);
    EXPECT_TRUE(ErrorsFound);
    EXPECT_TRUE(compare_err_stream_substring(R"(** Severe  ** GetDefaultConstructionSetData: DefaultSubSurfaceConstructions="BAD SUBSURF")", false));
    EXPECT_TRUE(compare_err_stream_substring(R"(invalid fixed_window_construction_name="NONEXISTENT CONSTR" not found.)", false));
}

// ---------------------------------------------------------------------------
// Bad DefaultSurfaceConstructions name in DefaultConstructionSet -> error
// ---------------------------------------------------------------------------
TEST_F(EnergyPlusFixture, DefaultConstructionSet_BadSurfConstrRef)
{
    std::string const idf_objects = std::string(idf_constructions) + R"(
  DefaultConstructionSet,
    Bad DCS,                 !- Name
    NONEXISTENT SURF CONSTRS, !- Default Exterior Surface Constructions Name
    ,                        !- Default Interior Surface Constructions Name
    ,                        !- Default Ground Contact Surface Constructions Name
    ,                        !- Default Exterior SubSurface Constructions Name
    ,                        !- Default Interior SubSurface Constructions Name
    ,                        !- Interior Partition Construction Name
    ;                        !- Adiabatic Surface Construction Name
)";

    ASSERT_TRUE(process_idf(idf_objects));
    state->init_state(*state);
    loadConstructions(*state);

    bool ErrorsFound = false;
    DefaultConstructions::GetDefaultConstructionSetData(*state, ErrorsFound);
    EXPECT_TRUE(ErrorsFound);
    EXPECT_TRUE(compare_err_stream_substring(R"(** Severe  ** GetDefaultConstructionSetData: DefaultConstructionSet="BAD DCS")", false));
    EXPECT_TRUE(compare_err_stream_substring(R"(invalid default_exterior_surface_constructions_name="NONEXISTENT SURF CONSTRS" not found.)", false));
}
