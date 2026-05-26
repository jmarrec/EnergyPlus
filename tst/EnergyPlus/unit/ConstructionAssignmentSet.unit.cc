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

// C++ Headers
#include <sstream>

// EnergyPlus Headers
#include <EnergyPlus/Construction.hh>
#include <EnergyPlus/Data/EnergyPlusData.hh>
#include <EnergyPlus/DataHeatBalance.hh>
#include <EnergyPlus/DataSurfaces.hh>
#include <EnergyPlus/ConstructionAssignmentSet.hh>
#include <EnergyPlus/HeatBalanceManager.hh>
#include <EnergyPlus/Material.hh>
#include <EnergyPlus/SurfaceGeometry.hh>
#include <EnergyPlus/UtilityRoutines.hh>

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

TEST_F(EnergyPlusFixture, ConstructionAssignmentSet_ParseAll)
{
    // Happy path: all three objects parse and ConstructionAssignmentSet links correctly.
    std::string const idf_objects = std::string(idf_constructions) + R"(
  SubSurfaceConstructionAssignments,
    Ext SubSurf Constrs,     !- Name
    Constr Window,           !- Fixed Window Construction Name
    Constr Window,           !- Operable Window Construction Name
    Constr Door,             !- Door Construction Name
    Constr Window,           !- Glass Door Construction Name
    Constr Door,             !- Overhead Door Construction Name
    Constr Window;           !- Skylight Construction Name

  SurfaceConstructionAssignments,
    Ext Surf Constrs,        !- Name
    Constr Floor,            !- Floor Construction Name
    Constr Wall,             !- Wall Construction Name
    Constr Roof;             !- Roof Ceiling Construction Name

  SurfaceConstructionAssignments,
    Int Surf Constrs,        !- Name
    Constr Floor,            !- Floor Construction Name
    Constr Wall,             !- Wall Construction Name
    Constr Roof;             !- Roof Ceiling Construction Name

  ConstructionAssignmentSet,
    My DCS,                  !- Name
    Ext Surf Constrs,        !- Exterior Surface Construction Assignments Name
    Int Surf Constrs,        !- Interior Surface Construction Assignments Name
    ,                        !- Ground Contact Surface Construction Assignments Name
    Ext SubSurf Constrs,     !- Exterior SubSurface Construction Assignments Name
    ,                        !- Interior SubSurface Construction Assignments Name
    Constr IntPartition,     !- Interior Partition Construction Name
    Constr Adiabatic;        !- Adiabatic Surface Construction Name
)";

    ASSERT_TRUE(process_idf(idf_objects));
    state->init_state(*state);
    loadConstructions(*state);

    bool ErrorsFound = false;
    ConstructionAssignments::GetConstructionAssignmentSetData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    auto &s_dc = state->dataConstructionAssignments;

    // Sub-surface set
    ASSERT_EQ(1u, s_dc->subSurfaceConstructionAssignments.size());
    auto const &ssc = s_dc->subSurfaceConstructionAssignments[0];
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
    ASSERT_EQ(2u, s_dc->surfaceConstructionAssignments.size());
    EXPECT_EQ("EXT SURF CONSTRS", s_dc->surfaceConstructionAssignments[0].Name);
    EXPECT_EQ("INT SURF CONSTRS", s_dc->surfaceConstructionAssignments[1].Name);

    // Construction set
    ASSERT_EQ(1u, s_dc->constructionAssignmentSets.size());
    auto const &dcs = s_dc->constructionAssignmentSets[0];
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

TEST_F(EnergyPlusFixture, ConstructionAssignmentSet_BadConstructionName)
{
    // Bad construction name in SubSurfaceConstructionAssignments produces an error.
    std::string const idf_objects = std::string(idf_constructions) + R"(
  SubSurfaceConstructionAssignments,
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
    ConstructionAssignments::GetConstructionAssignmentSetData(*state, ErrorsFound);
    EXPECT_TRUE(ErrorsFound);
    EXPECT_TRUE(compare_err_stream_substring(R"(** Severe  ** GetConstructionAssignmentSetData: SubSurfaceConstructionAssignments="BAD SUBSURF")", false));
    EXPECT_TRUE(compare_err_stream_substring(R"(invalid fixed_window_construction_name="NONEXISTENT CONSTR" not found.)", false));
}

TEST_F(EnergyPlusFixture, ConstructionAssignmentSet_BadSurfConstrRef)
{
    // Bad SurfaceConstructionAssignments name referenced from ConstructionAssignmentSet produces an error.
    std::string const idf_objects = std::string(idf_constructions) + R"(
  ConstructionAssignmentSet,
    Bad DCS,                 !- Name
    NONEXISTENT SURF CONSTRS, !- Exterior Surface Construction Assignments Name
    ,                        !- Interior Surface Construction Assignments Name
    ,                        !- Ground Contact Surface Construction Assignments Name
    ,                        !- Exterior SubSurface Construction Assignments Name
    ,                        !- Interior SubSurface Construction Assignments Name
    ,                        !- Interior Partition Construction Name
    ;                        !- Adiabatic Surface Construction Name
)";

    ASSERT_TRUE(process_idf(idf_objects));
    state->init_state(*state);
    loadConstructions(*state);

    bool ErrorsFound = false;
    ConstructionAssignments::GetConstructionAssignmentSetData(*state, ErrorsFound);
    EXPECT_TRUE(ErrorsFound);
    EXPECT_TRUE(compare_err_stream_substring(R"(** Severe  ** GetConstructionAssignmentSetData: ConstructionAssignmentSet="BAD DCS")", false));
    EXPECT_TRUE(compare_err_stream_substring(R"(invalid exterior_surface_construction_assignments_name="NONEXISTENT SURF CONSTRS" not found.)", false));
}

TEST(ConstructionAssignmentSet_GetDefaultConstruction, AllCases)
{
    // Verify getConstructionAssignment returns the correct slot for every surface type and BC.
    // No EnergyPlus state needed — builds the struct directly.
    // Unique sentinel per slot so any wrong return is immediately obvious.
    auto extSurf = std::make_shared<ConstructionAssignments::SurfaceConstructionAssignmentsData>();
    extSurf->wallConstrNum = 1;
    extSurf->floorConstrNum = 2;
    extSurf->roofCeilingConstrNum = 3;

    auto intSurf = std::make_shared<ConstructionAssignments::SurfaceConstructionAssignmentsData>();
    intSurf->wallConstrNum = 4;
    intSurf->floorConstrNum = 5;
    intSurf->roofCeilingConstrNum = 6;

    auto gndSurf = std::make_shared<ConstructionAssignments::SurfaceConstructionAssignmentsData>();
    gndSurf->wallConstrNum = 7;
    gndSurf->floorConstrNum = 8;
    gndSurf->roofCeilingConstrNum = 9;

    auto extSubSurf = std::make_shared<ConstructionAssignments::SubSurfaceConstructionAssignmentsData>();
    extSubSurf->fixedWindowConstrNum = 10;
    extSubSurf->operableWindowConstrNum = 11;
    extSubSurf->doorConstrNum = 12;
    extSubSurf->glassDoorConstrNum = 13;
    extSubSurf->overheadDoorConstrNum = 14;
    extSubSurf->skylightConstrNum = 15;
    extSubSurf->tddDomeConstrNum = 16;
    extSubSurf->tddDiffuserConstrNum = 17;

    auto intSubSurf = std::make_shared<ConstructionAssignments::SubSurfaceConstructionAssignmentsData>();
    intSubSurf->fixedWindowConstrNum = 18;
    intSubSurf->operableWindowConstrNum = 19;

    ConstructionAssignments::ConstructionAssignmentSetData dcs;
    dcs.exteriorSurfConstructions = extSurf;
    dcs.interiorSurfConstructions = intSurf;
    dcs.groundContactSurfConstructions = gndSurf;
    dcs.exteriorSubSurfConstructions = extSubSurf;
    dcs.interiorSubSurfConstructions = intSubSurf;
    dcs.interiorPartitionConstrNum = 20;
    dcs.adiabaticSurfConstrNum = 21;

    auto makeSurf = [](DataSurfaces::SurfaceClass cls, int extBC, const std::string &name = "S", const std::string &bcName = "") {
        DataSurfaces::SurfaceData surf;
        surf.Class = cls;

        surf.OriginalClass = surf.Class;
        if (DataSurfaces::SurfaceClassIsGlazed(surf.Class) || surf.Class == DataSurfaces::SurfaceClass::TDD_Diffuser) {
            surf.Class = DataSurfaces::SurfaceClass::Window;
        }
        surf.ExtBoundCond = extBC;
        surf.Name = name;
        surf.ExtBoundCondName = bcName.empty() ? name : bcName;
        return surf;
    };

    // Base surfaces — exterior
    EXPECT_EQ(1, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Wall, DataSurfaces::ExternalEnvironment)));
    EXPECT_EQ(2, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Floor, DataSurfaces::ExternalEnvironment)));
    EXPECT_EQ(3, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Roof, DataSurfaces::ExternalEnvironment)));
    // OtherSideCoeff -> exterior
    EXPECT_EQ(1, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Wall, DataSurfaces::OtherSideCoefNoCalcExt)));
    EXPECT_EQ(1, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Wall, DataSurfaces::OtherSideCoefCalcExt)));

    // Ground contact
    EXPECT_EQ(7, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Wall, DataSurfaces::Ground)));
    EXPECT_EQ(8, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Floor, DataSurfaces::Ground)));
    EXPECT_EQ(8, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Floor, DataSurfaces::GroundFCfactorMethod)));
    EXPECT_EQ(8, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Floor, DataSurfaces::KivaFoundation)));

    // Interior (pre-reconciliation: ExtBoundCond < -6, ExtBoundCondName != Name)
    EXPECT_EQ(4, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Wall, -999, "S", "Other")));
    EXPECT_EQ(5, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Floor, -999, "S", "Other")));
    EXPECT_EQ(6, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Roof, -999, "S", "Other")));

    // Adiabatic (ExtBoundCondName == Name, ExtBoundCond < -6)
    EXPECT_EQ(21, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Wall, -999)));

    // Interior mass
    EXPECT_EQ(20, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::IntMass, -999)));

    // Exterior subsurfaces
    EXPECT_EQ(10, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::FixedWindow, DataSurfaces::ExternalEnvironment)));
    EXPECT_EQ(
        10,
        dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Window, DataSurfaces::ExternalEnvironment))); // generic -> FixedWindow slot
    EXPECT_EQ(11, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::OperableWindow, DataSurfaces::ExternalEnvironment)));
    EXPECT_EQ(12, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Door, DataSurfaces::ExternalEnvironment)));
    EXPECT_EQ(13, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::GlassDoor, DataSurfaces::ExternalEnvironment)));
    EXPECT_EQ(14, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::OverheadDoor, DataSurfaces::ExternalEnvironment)));
    EXPECT_EQ(15, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Skylight, DataSurfaces::ExternalEnvironment)));
    EXPECT_EQ(16, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::TDD_Dome, DataSurfaces::ExternalEnvironment)));
    EXPECT_EQ(17, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::TDD_Diffuser, DataSurfaces::ExternalEnvironment)));

    // Interior subsurfaces
    EXPECT_EQ(18, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::FixedWindow, -999, "S", "Other")));
    EXPECT_EQ(19, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::OperableWindow, -999, "S", "Other")));

    // nullptr sub-objects -> 0
    ConstructionAssignments::ConstructionAssignmentSetData dcsEmpty;
    EXPECT_EQ(0, dcsEmpty.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Wall, DataSurfaces::ExternalEnvironment)));
    EXPECT_EQ(0, dcsEmpty.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::FixedWindow, DataSurfaces::ExternalEnvironment)));
    EXPECT_EQ(0, dcsEmpty.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Floor, DataSurfaces::Ground)));
}

TEST_F(EnergyPlusFixture, ConstructionAssignmentSet_GetDefaultConstruction)
{
    // Tests getConstructionAssignment() against real E+ parsed surfaces, verifying that
    // the BC values produced by GetHTSurfaceData/GetHTSubSurfaceData map to the correct
    // DCS slot both before and after BC reconciliation.
    //
    // The geometry covers every BC category (exterior, ground, adiabatic, interior) and
    // every fenestration type. Explicit constructions are provided for all surfaces so the
    // model loads; the DCS uses distinct named constructions per slot for unambiguous assertions.

    std::string const geometry = delimited_string({
        "Zone,",
        "  Zone1,                                  !- Name",
        "  0,                                      !- Direction of Relative North {deg}",
        "  0,                                      !- X Origin {m}",
        "  0,                                      !- Y Origin {m}",
        "  0,                                      !- Z Origin {m}",
        "  ,                                       !- Type",
        "  1,                                      !- Multiplier",
        "  ,                                       !- Ceiling Height {m}",
        "  ,                                       !- Volume {m3}",
        "  ,                                       !- Floor Area {m2}",
        "  ,                                       !- Zone Inside Convection Algorithm",
        "  ,                                       !- Zone Outside Convection Algorithm",
        "  Yes;                                    !- Part of Total Floor Area",

        "BuildingSurface:Detailed,",
        "  Space1 Floor,                           !- Name",
        "  Floor,                                  !- Surface Type",
        "  Exterior Floor Construction,            !- Construction Name",
        "  Zone1,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Outdoors,                               !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  SunExposed,                             !- Sun Exposure",
        "  WindExposed,                            !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  10, 10, 0,                              !- X,Y,Z Vertex 1 {m}",
        "  10, 0, 0,                               !- X,Y,Z Vertex 2 {m}",
        "  0, 0, 0,                                !- X,Y,Z Vertex 3 {m}",
        "  0, 10, 0;                               !- X,Y,Z Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        "  Space1 RoofCeiling,                     !- Name",
        "  Ceiling,                                !- Surface Type",
        "  Interior Roof Construction,             !- Construction Name",
        "  Zone1,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Surface,                                !- Outside Boundary Condition",
        "  Space3 Floor,                           !- Outside Boundary Condition Object",
        "  NoSun,                                  !- Sun Exposure",
        "  NoWind,                                 !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  0, 10, 3,                               !- X,Y,Z Vertex 1 {m}",
        "  0, 0, 3,                                !- X,Y,Z Vertex 2 {m}",
        "  10, 0, 3,                               !- X,Y,Z Vertex 3 {m}",
        "  10, 10, 3;                              !- X,Y,Z Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        "  Space1 Wall 1,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Interior Wall Construction,             !- Construction Name",
        "  Zone1,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Surface,                                !- Outside Boundary Condition",
        "  Space2 Wall 3,                          !- Outside Boundary Condition Object",
        "  NoSun,                                  !- Sun Exposure",
        "  NoWind,                                 !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  10, 10, 3,                              !- X,Y,Z Vertex 1 {m}",
        "  10, 0, 3,                               !- X,Y,Z Vertex 2 {m}",
        "  10, 0, 0,                               !- X,Y,Z Vertex 3 {m}",
        "  10, 10, 0;                              !- X,Y,Z Vertex 4 {m}",

        "FenestrationSurface:Detailed,",
        "  InteriorDoor - Door,                    !- Name",
        "  Door,                                   !- Surface Type",
        "  Interior Door Construction,             !- Construction Name",
        "  Space1 Wall 1,                          !- Building Surface Name",
        "  InteriorDoor - Door - Reversed,         !- Outside Boundary Condition Object",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Frame and Divider Name",
        "  ,                                       !- Multiplier",
        "  ,                                       !- Number of Vertices",
        "  10,                                     !- Vertex 1 X-coordinate {m}",
        "  4.6,                                    !- Vertex 1 Y-coordinate {m}",
        "  2,                                      !- Vertex 1 Z-coordinate {m}",
        "  10,                                     !- Vertex 2 X-coordinate {m}",
        "  4.6,                                    !- Vertex 2 Y-coordinate {m}",
        "  0,                                      !- Vertex 2 Z-coordinate {m}",
        "  10,                                     !- Vertex 3 X-coordinate {m}",
        "  5.4,                                    !- Vertex 3 Y-coordinate {m}",
        "  0,                                      !- Vertex 3 Z-coordinate {m}",
        "  10,                                     !- Vertex 4 X-coordinate {m}",
        "  5.4,                                    !- Vertex 4 Y-coordinate {m}",
        "  2;                                      !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Space1 Wall 2,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Exterior Wall Construction,             !- Construction Name",
        "  Zone1,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Outdoors,                               !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  SunExposed,                             !- Sun Exposure",
        "  WindExposed,                            !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  10, 0, 3,                               !- X,Y,Z Vertex 1 {m}",
        "  0, 0, 3,                                !- X,Y,Z Vertex 2 {m}",
        "  0, 0, 0,                                !- X,Y,Z Vertex 3 {m}",
        "  10, 0, 0;                               !- X,Y,Z Vertex 4 {m}",

        "FenestrationSurface:Detailed,",
        "  ExteriorDoor - Door,                    !- Name",
        "  Door,                                   !- Surface Type",
        "  Exterior Door Construction,             !- Construction Name",
        "  Space1 Wall 2,                          !- Building Surface Name",
        "  ,                                       !- Outside Boundary Condition Object",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Frame and Divider Name",
        "  ,                                       !- Multiplier",
        "  ,                                       !- Number of Vertices",
        "  4.6,                                    !- Vertex 1 X-coordinate {m}",
        "  0,                                      !- Vertex 1 Y-coordinate {m}",
        "  2,                                      !- Vertex 1 Z-coordinate {m}",
        "  4.6,                                    !- Vertex 2 X-coordinate {m}",
        "  0,                                      !- Vertex 2 Y-coordinate {m}",
        "  0,                                      !- Vertex 2 Z-coordinate {m}",
        "  5.4,                                    !- Vertex 3 X-coordinate {m}",
        "  0,                                      !- Vertex 3 Y-coordinate {m}",
        "  0,                                      !- Vertex 3 Z-coordinate {m}",
        "  5.4,                                    !- Vertex 4 X-coordinate {m}",
        "  0,                                      !- Vertex 4 Y-coordinate {m}",
        "  2;                                      !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Space1 Wall 3,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Exterior Wall Construction,             !- Construction Name",
        "  Zone1,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Outdoors,                               !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  SunExposed,                             !- Sun Exposure",
        "  WindExposed,                            !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  0, 0, 3,                                !- X,Y,Z Vertex 1 {m}",
        "  0, 10, 3,                               !- X,Y,Z Vertex 2 {m}",
        "  0, 10, 0,                               !- X,Y,Z Vertex 3 {m}",
        "  0, 0, 0;                                !- X,Y,Z Vertex 4 {m}",

        "FenestrationSurface:Detailed,",
        "  ExteriorDoor - OverheadDoor,            !- Name",
        "  OverheadDoor,                           !- Surface Type",
        "  Exterior OverheadDoor Construction,     !- Construction Name",
        "  Space1 Wall 3,                          !- Building Surface Name",
        "  ,                                       !- Outside Boundary Condition Object",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Frame and Divider Name",
        "  ,                                       !- Multiplier",
        "  ,                                       !- Number of Vertices",
        "  0,                                      !- Vertex 1 X-coordinate {m}",
        "  7.5,                                    !- Vertex 1 Y-coordinate {m}",
        "  2.5,                                    !- Vertex 1 Z-coordinate {m}",
        "  0,                                      !- Vertex 2 X-coordinate {m}",
        "  7.5,                                    !- Vertex 2 Y-coordinate {m}",
        "  0,                                      !- Vertex 2 Z-coordinate {m}",
        "  0,                                      !- Vertex 3 X-coordinate {m}",
        "  2.5,                                    !- Vertex 3 Y-coordinate {m}",
        "  0,                                      !- Vertex 3 Z-coordinate {m}",
        "  0,                                      !- Vertex 4 X-coordinate {m}",
        "  2.5,                                    !- Vertex 4 Y-coordinate {m}",
        "  2.5;                                    !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Space1 Wall 4,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Exterior Wall Construction,             !- Construction Name",
        "  Zone1,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Outdoors,                               !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  SunExposed,                             !- Sun Exposure",
        "  WindExposed,                            !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  0, 10, 3,                               !- X,Y,Z Vertex 1 {m}",
        "  10, 10, 3,                              !- X,Y,Z Vertex 2 {m}",
        "  10, 10, 0,                              !- X,Y,Z Vertex 3 {m}",
        "  0, 10, 0;                               !- X,Y,Z Vertex 4 {m}",

        "FenestrationSurface:Detailed,",
        "  ExteriorDoor - GlassDoor,               !- Name",
        "  GlassDoor,                              !- Surface Type",
        "  Exterior GlassDoor Construction,        !- Construction Name",
        "  Space1 Wall 4,                          !- Building Surface Name",
        "  ,                                       !- Outside Boundary Condition Object",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Frame and Divider Name",
        "  ,                                       !- Multiplier",
        "  ,                                       !- Number of Vertices",
        "  5.4,                                    !- Vertex 1 X-coordinate {m}",
        "  10,                                     !- Vertex 1 Y-coordinate {m}",
        "  2,                                      !- Vertex 1 Z-coordinate {m}",
        "  5.4,                                    !- Vertex 2 X-coordinate {m}",
        "  10,                                     !- Vertex 2 Y-coordinate {m}",
        "  0,                                      !- Vertex 2 Z-coordinate {m}",
        "  4.6,                                    !- Vertex 3 X-coordinate {m}",
        "  10,                                     !- Vertex 3 Y-coordinate {m}",
        "  0,                                      !- Vertex 3 Z-coordinate {m}",
        "  4.6,                                    !- Vertex 4 X-coordinate {m}",
        "  10,                                     !- Vertex 4 Y-coordinate {m}",
        "  2;                                      !- Vertex 4 Z-coordinate {m}",

        "Zone,",
        "  Zone2,                                  !- Name",
        "  0,                                      !- Direction of Relative North {deg}",
        "  10,                                     !- X Origin {m}",
        "  0,                                      !- Y Origin {m}",
        "  0,                                      !- Z Origin {m}",
        "  ,                                       !- Type",
        "  1,                                      !- Multiplier",
        "  ,                                       !- Ceiling Height {m}",
        "  ,                                       !- Volume {m3}",
        "  ,                                       !- Floor Area {m2}",
        "  ,                                       !- Zone Inside Convection Algorithm",
        "  ,                                       !- Zone Outside Convection Algorithm",
        "  Yes;                                    !- Part of Total Floor Area",

        "BuildingSurface:Detailed,",
        "  Space2 Floor,                           !- Name",
        "  Floor,                                  !- Surface Type",
        "  Ground Floor Construction,              !- Construction Name",
        "  Zone2,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Ground,                                 !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  NoSun,                                  !- Sun Exposure",
        "  NoWind,                                 !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  10, 10, 0,                              !- X,Y,Z Vertex 1 {m}",
        "  10, 0, 0,                               !- X,Y,Z Vertex 2 {m}",
        "  0, 0, 0,                                !- X,Y,Z Vertex 3 {m}",
        "  0, 10, 0;                               !- X,Y,Z Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        "  Space2 RoofCeiling,                     !- Name",
        "  Ceiling,                                !- Surface Type",
        "  Interior Roof Construction,             !- Construction Name",
        "  Zone2,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Surface,                                !- Outside Boundary Condition",
        "  Space4 Floor,                           !- Outside Boundary Condition Object",
        "  NoSun,                                  !- Sun Exposure",
        "  NoWind,                                 !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  0, 10, 3,                               !- X,Y,Z Vertex 1 {m}",
        "  0, 0, 3,                                !- X,Y,Z Vertex 2 {m}",
        "  10, 0, 3,                               !- X,Y,Z Vertex 3 {m}",
        "  10, 10, 3;                              !- X,Y,Z Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        "  Space2 Wall 1,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Adiabatic Surface Construction,         !- Construction Name",
        "  Zone2,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Adiabatic,                              !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  NoSun,                                  !- Sun Exposure",
        "  NoWind,                                 !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  10, 10, 3,                              !- X,Y,Z Vertex 1 {m}",
        "  10, 0, 3,                               !- X,Y,Z Vertex 2 {m}",
        "  10, 0, 0,                               !- X,Y,Z Vertex 3 {m}",
        "  10, 10, 0;                              !- X,Y,Z Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        "  Space2 Wall 2,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Ground Wall Construction,               !- Construction Name",
        "  Zone2,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Ground,                                 !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  NoSun,                                  !- Sun Exposure",
        "  NoWind,                                 !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  10, 0, 3,                               !- X,Y,Z Vertex 1 {m}",
        "  0, 0, 3,                                !- X,Y,Z Vertex 2 {m}",
        "  0, 0, 0,                                !- X,Y,Z Vertex 3 {m}",
        "  10, 0, 0;                               !- X,Y,Z Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        "  Space2 Wall 3,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Interior Wall Construction,             !- Construction Name",
        "  Zone2,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Surface,                                !- Outside Boundary Condition",
        "  Space1 Wall 1,                          !- Outside Boundary Condition Object",
        "  NoSun,                                  !- Sun Exposure",
        "  NoWind,                                 !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  0, 0, 3,                                !- X,Y,Z Vertex 1 {m}",
        "  0, 10, 3,                               !- X,Y,Z Vertex 2 {m}",
        "  0, 10, 0,                               !- X,Y,Z Vertex 3 {m}",
        "  0, 0, 0;                                !- X,Y,Z Vertex 4 {m}",

        "FenestrationSurface:Detailed,",
        "  InteriorDoor - Door - Reversed,         !- Name",
        "  Door,                                   !- Surface Type",
        "  Interior Door Construction,             !- Construction Name",
        "  Space2 Wall 3,                          !- Building Surface Name",
        "  InteriorDoor - Door,                    !- Outside Boundary Condition Object",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Frame and Divider Name",
        "  ,                                       !- Multiplier",
        "  ,                                       !- Number of Vertices",
        "  10,                                     !- Vertex 1 X-coordinate {m}",
        "  5.4,                                    !- Vertex 1 Y-coordinate {m}",
        "  2,                                      !- Vertex 1 Z-coordinate {m}",
        "  10,                                     !- Vertex 2 X-coordinate {m}",
        "  5.4,                                    !- Vertex 2 Y-coordinate {m}",
        "  0,                                      !- Vertex 2 Z-coordinate {m}",
        "  10,                                     !- Vertex 3 X-coordinate {m}",
        "  4.6,                                    !- Vertex 3 Y-coordinate {m}",
        "  0,                                      !- Vertex 3 Z-coordinate {m}",
        "  10,                                     !- Vertex 4 X-coordinate {m}",
        "  4.6,                                    !- Vertex 4 Y-coordinate {m}",
        "  2;                                      !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Space2 Wall 4,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Exterior Wall Construction,             !- Construction Name",
        "  Zone2,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Outdoors,                               !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  SunExposed,                             !- Sun Exposure",
        "  WindExposed,                            !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  0, 10, 3,                               !- X,Y,Z Vertex 1 {m}",
        "  10, 10, 3,                              !- X,Y,Z Vertex 2 {m}",
        "  10, 10, 0,                              !- X,Y,Z Vertex 3 {m}",
        "  0, 10, 0;                               !- X,Y,Z Vertex 4 {m}",

        "FenestrationSurface:Detailed,",
        "  ExteriorWindow - FixedWindow,           !- Name",
        "  FixedWindow,                            !- Surface Type",
        "  Exterior FixedWindow Construction,      !- Construction Name",
        "  Space2 Wall 4,                          !- Building Surface Name",
        "  ,                                       !- Outside Boundary Condition Object",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Frame and Divider Name",
        "  ,                                       !- Multiplier",
        "  ,                                       !- Number of Vertices",
        "  9.9746,                                 !- Vertex 1 X-coordinate {m}",
        "  10,                                     !- Vertex 1 Y-coordinate {m}",
        "  1.96812712579906,                       !- Vertex 1 Z-coordinate {m}",
        "  9.9746,                                 !- Vertex 2 X-coordinate {m}",
        "  10,                                     !- Vertex 2 Y-coordinate {m}",
        "  0.762,                                  !- Vertex 2 Z-coordinate {m}",
        "  0.0254000000000012,                     !- Vertex 3 X-coordinate {m}",
        "  10,                                     !- Vertex 3 Y-coordinate {m}",
        "  0.762,                                  !- Vertex 3 Z-coordinate {m}",
        "  0.0254000000000012,                     !- Vertex 4 X-coordinate {m}",
        "  10,                                     !- Vertex 4 Y-coordinate {m}",
        "  1.96812712579906;                       !- Vertex 4 Z-coordinate {m}",

        "Zone,",
        "  Zone3,                                  !- Name",
        "  0,                                      !- Direction of Relative North {deg}",
        "  0,                                      !- X Origin {m}",
        "  0,                                      !- Y Origin {m}",
        "  3,                                      !- Z Origin {m}",
        "  ,                                       !- Type",
        "  1,                                      !- Multiplier",
        "  ,                                       !- Ceiling Height {m}",
        "  ,                                       !- Volume {m3}",
        "  ,                                       !- Floor Area {m2}",
        "  ,                                       !- Zone Inside Convection Algorithm",
        "  ,                                       !- Zone Outside Convection Algorithm",
        "  Yes;                                    !- Part of Total Floor Area",

        "BuildingSurface:Detailed,",
        "  Space3 Floor,                           !- Name",
        "  Floor,                                  !- Surface Type",
        "  Interior Floor Construction,            !- Construction Name",
        "  Zone3,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Surface,                                !- Outside Boundary Condition",
        "  Space1 RoofCeiling,                     !- Outside Boundary Condition Object",
        "  NoSun,                                  !- Sun Exposure",
        "  NoWind,                                 !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  10, 10, 0,                              !- X,Y,Z Vertex 1 {m}",
        "  10, 0, 0,                               !- X,Y,Z Vertex 2 {m}",
        "  0, 0, 0,                                !- X,Y,Z Vertex 3 {m}",
        "  0, 10, 0;                               !- X,Y,Z Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        "  Space3 RoofCeiling,                     !- Name",
        "  Roof,                                   !- Surface Type",
        "  Exterior Roof Construction,             !- Construction Name",
        "  Zone3,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Outdoors,                               !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  SunExposed,                             !- Sun Exposure",
        "  WindExposed,                            !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  0, 10, 3,                               !- X,Y,Z Vertex 1 {m}",
        "  0, 0, 3,                                !- X,Y,Z Vertex 2 {m}",
        "  10, 0, 3,                               !- X,Y,Z Vertex 3 {m}",
        "  10, 10, 3;                              !- X,Y,Z Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        "  Space3 Wall 1,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Interior Wall Construction,             !- Construction Name",
        "  Zone3,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Surface,                                !- Outside Boundary Condition",
        "  Space4 Wall 3,                          !- Outside Boundary Condition Object",
        "  NoSun,                                  !- Sun Exposure",
        "  NoWind,                                 !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  10, 10, 3,                              !- X,Y,Z Vertex 1 {m}",
        "  10, 0, 3,                               !- X,Y,Z Vertex 2 {m}",
        "  10, 0, 0,                               !- X,Y,Z Vertex 3 {m}",
        "  10, 10, 0;                              !- X,Y,Z Vertex 4 {m}",

        "FenestrationSurface:Detailed,",
        "  InteriorWindow - FixedWindow,           !- Name",
        "  FixedWindow,                            !- Surface Type",
        "  Interior FixedWindow Construction,      !- Construction Name",
        "  Space3 Wall 1,                          !- Building Surface Name",
        "  InteriorWindow - FixedWindow - Reversed, !- Outside Boundary Condition Object",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Frame and Divider Name",
        "  ,                                       !- Multiplier",
        "  ,                                       !- Number of Vertices",
        "  10,                                     !- Vertex 1 X-coordinate {m}",
        "  0.0254,                                 !- Vertex 1 Y-coordinate {m}",
        "  1.96812712579906,                       !- Vertex 1 Z-coordinate {m}",
        "  10,                                     !- Vertex 2 X-coordinate {m}",
        "  0.0254,                                 !- Vertex 2 Y-coordinate {m}",
        "  0.762,                                  !- Vertex 2 Z-coordinate {m}",
        "  10,                                     !- Vertex 3 X-coordinate {m}",
        "  9.9746,                                 !- Vertex 3 Y-coordinate {m}",
        "  0.762,                                  !- Vertex 3 Z-coordinate {m}",
        "  10,                                     !- Vertex 4 X-coordinate {m}",
        "  9.9746,                                 !- Vertex 4 Y-coordinate {m}",
        "  1.96812712579906;                       !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Space3 Wall 2,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Exterior Wall Construction,             !- Construction Name",
        "  Zone3,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Outdoors,                               !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  SunExposed,                             !- Sun Exposure",
        "  WindExposed,                            !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  10, 0, 3,                               !- X,Y,Z Vertex 1 {m}",
        "  0, 0, 3,                                !- X,Y,Z Vertex 2 {m}",
        "  0, 0, 0,                                !- X,Y,Z Vertex 3 {m}",
        "  10, 0, 0;                               !- X,Y,Z Vertex 4 {m}",

        "FenestrationSurface:Detailed,",
        "  ExteriorWindow - OperableWindow,        !- Name",
        "  OperableWindow,                         !- Surface Type",
        "  Exterior OperableWindow Construction,   !- Construction Name",
        "  Space3 Wall 2,                          !- Building Surface Name",
        "  ,                                       !- Outside Boundary Condition Object",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Frame and Divider Name",
        "  ,                                       !- Multiplier",
        "  ,                                       !- Number of Vertices",
        "  0.0254,                                 !- Vertex 1 X-coordinate {m}",
        "  0,                                      !- Vertex 1 Y-coordinate {m}",
        "  1.96812712579906,                       !- Vertex 1 Z-coordinate {m}",
        "  0.0254,                                 !- Vertex 2 X-coordinate {m}",
        "  0,                                      !- Vertex 2 Y-coordinate {m}",
        "  0.762,                                  !- Vertex 2 Z-coordinate {m}",
        "  9.9746,                                 !- Vertex 3 X-coordinate {m}",
        "  0,                                      !- Vertex 3 Y-coordinate {m}",
        "  0.762,                                  !- Vertex 3 Z-coordinate {m}",
        "  9.9746,                                 !- Vertex 4 X-coordinate {m}",
        "  0,                                      !- Vertex 4 Y-coordinate {m}",
        "  1.96812712579906;                       !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Space3 Wall 3,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Exterior Wall Construction,             !- Construction Name",
        "  Zone3,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Outdoors,                               !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  SunExposed,                             !- Sun Exposure",
        "  WindExposed,                            !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  0, 0, 3,                                !- X,Y,Z Vertex 1 {m}",
        "  0, 10, 3,                               !- X,Y,Z Vertex 2 {m}",
        "  0, 10, 0,                               !- X,Y,Z Vertex 3 {m}",
        "  0, 0, 0;                                !- X,Y,Z Vertex 4 {m}",

        "FenestrationSurface:Detailed,",
        "  ExteriorWindow - Window,                !- Name",
        "  Window,                                 !- Surface Type",
        "  Exterior FixedWindow Construction,      !- Construction Name",
        "  Space3 Wall 3,                          !- Building Surface Name",
        "  ,                                       !- Outside Boundary Condition Object",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Frame and Divider Name",
        "  ,                                       !- Multiplier",
        "  ,                                       !- Number of Vertices",
        "  0,                                      !- Vertex 1 X-coordinate {m}",
        "  9.9746,                                 !- Vertex 1 Y-coordinate {m}",
        "  1.96812712579906,                       !- Vertex 1 Z-coordinate {m}",
        "  0,                                      !- Vertex 2 X-coordinate {m}",
        "  9.9746,                                 !- Vertex 2 Y-coordinate {m}",
        "  0.762,                                  !- Vertex 2 Z-coordinate {m}",
        "  0,                                      !- Vertex 3 X-coordinate {m}",
        "  0.0254000000000012,                     !- Vertex 3 Y-coordinate {m}",
        "  0.762,                                  !- Vertex 3 Z-coordinate {m}",
        "  0,                                      !- Vertex 4 X-coordinate {m}",
        "  0.0254000000000012,                     !- Vertex 4 Y-coordinate {m}",
        "  1.96812712579906;                       !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Space3 Wall 4,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Exterior Wall Construction,             !- Construction Name",
        "  Zone3,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Outdoors,                               !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  SunExposed,                             !- Sun Exposure",
        "  WindExposed,                            !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  0, 10, 3,                               !- X,Y,Z Vertex 1 {m}",
        "  10, 10, 3,                              !- X,Y,Z Vertex 2 {m}",
        "  10, 10, 0,                              !- X,Y,Z Vertex 3 {m}",
        "  0, 10, 0;                               !- X,Y,Z Vertex 4 {m}",

        "Zone,",
        "  Zone4,                                  !- Name",
        "  0,                                      !- Direction of Relative North {deg}",
        "  10,                                     !- X Origin {m}",
        "  0,                                      !- Y Origin {m}",
        "  3,                                      !- Z Origin {m}",
        "  ,                                       !- Type",
        "  1,                                      !- Multiplier",
        "  ,                                       !- Ceiling Height {m}",
        "  ,                                       !- Volume {m3}",
        "  ,                                       !- Floor Area {m2}",
        "  ,                                       !- Zone Inside Convection Algorithm",
        "  ,                                       !- Zone Outside Convection Algorithm",
        "  Yes;                                    !- Part of Total Floor Area",

        "BuildingSurface:Detailed,",
        "  Space4 Floor,                           !- Name",
        "  Floor,                                  !- Surface Type",
        "  Interior Floor Construction,            !- Construction Name",
        "  Zone4,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Surface,                                !- Outside Boundary Condition",
        "  Space2 RoofCeiling,                     !- Outside Boundary Condition Object",
        "  NoSun,                                  !- Sun Exposure",
        "  NoWind,                                 !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  10, 10, 0,                              !- X,Y,Z Vertex 1 {m}",
        "  10, 0, 0,                               !- X,Y,Z Vertex 2 {m}",
        "  0, 0, 0,                                !- X,Y,Z Vertex 3 {m}",
        "  0, 10, 0;                               !- X,Y,Z Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        "  Space4 RoofCeiling,                     !- Name",
        "  Roof,                                   !- Surface Type",
        "  Exterior Roof Construction,             !- Construction Name",
        "  Zone4,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Outdoors,                               !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  SunExposed,                             !- Sun Exposure",
        "  WindExposed,                            !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  0, 10, 3,                               !- X,Y,Z Vertex 1 {m}",
        "  0, 0, 3,                                !- X,Y,Z Vertex 2 {m}",
        "  10, 0, 3,                               !- X,Y,Z Vertex 3 {m}",
        "  10, 10, 3;                              !- X,Y,Z Vertex 4 {m}",

        "FenestrationSurface:Detailed,",
        "  ExteriorWindow - Skylight,              !- Name",
        "  Skylight,                               !- Surface Type",
        "  Exterior Skylight Construction,         !- Construction Name",
        "  Space4 RoofCeiling,                     !- Building Surface Name",
        "  ,                                       !- Outside Boundary Condition Object",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Frame and Divider Name",
        "  ,                                       !- Multiplier",
        "  ,                                       !- Number of Vertices",
        "  1.83772233983162,                       !- Vertex 1 X-coordinate {m}",
        "  8.16227766016838,                       !- Vertex 1 Y-coordinate {m}",
        "  3,                                      !- Vertex 1 Z-coordinate {m}",
        "  1.83772233983162,                       !- Vertex 2 X-coordinate {m}",
        "  1.83772233983162,                       !- Vertex 2 Y-coordinate {m}",
        "  3,                                      !- Vertex 2 Z-coordinate {m}",
        "  8.16227766016838,                       !- Vertex 3 X-coordinate {m}",
        "  1.83772233983162,                       !- Vertex 3 Y-coordinate {m}",
        "  3,                                      !- Vertex 3 Z-coordinate {m}",
        "  8.16227766016838,                       !- Vertex 4 X-coordinate {m}",
        "  8.16227766016838,                       !- Vertex 4 Y-coordinate {m}",
        "  3;                                      !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Space4 Wall 1,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Exterior Wall Construction,             !- Construction Name",
        "  Zone4,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Outdoors,                               !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  SunExposed,                             !- Sun Exposure",
        "  WindExposed,                            !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  10, 10, 3,                              !- X,Y,Z Vertex 1 {m}",
        "  10, 0, 3,                               !- X,Y,Z Vertex 2 {m}",
        "  10, 0, 0,                               !- X,Y,Z Vertex 3 {m}",
        "  10, 10, 0;                              !- X,Y,Z Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        "  Space4 Wall 2,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Exterior Wall Construction,             !- Construction Name",
        "  Zone4,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Outdoors,                               !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  SunExposed,                             !- Sun Exposure",
        "  WindExposed,                            !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  10, 0, 3,                               !- X,Y,Z Vertex 1 {m}",
        "  0, 0, 3,                                !- X,Y,Z Vertex 2 {m}",
        "  0, 0, 0,                                !- X,Y,Z Vertex 3 {m}",
        "  10, 0, 0;                               !- X,Y,Z Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        "  Space4 Wall 3,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Interior Wall Construction,             !- Construction Name",
        "  Zone4,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Surface,                                !- Outside Boundary Condition",
        "  Space3 Wall 1,                          !- Outside Boundary Condition Object",
        "  NoSun,                                  !- Sun Exposure",
        "  NoWind,                                 !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  0, 0, 3,                                !- X,Y,Z Vertex 1 {m}",
        "  0, 10, 3,                               !- X,Y,Z Vertex 2 {m}",
        "  0, 10, 0,                               !- X,Y,Z Vertex 3 {m}",
        "  0, 0, 0;                                !- X,Y,Z Vertex 4 {m}",

        "FenestrationSurface:Detailed,",
        "  InteriorWindow - FixedWindow - Reversed, !- Name",
        "  FixedWindow,                            !- Surface Type",
        "  Interior FixedWindow Construction,      !- Construction Name",
        "  Space4 Wall 3,                          !- Building Surface Name",
        "  InteriorWindow - FixedWindow,           !- Outside Boundary Condition Object",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Frame and Divider Name",
        "  ,                                       !- Multiplier",
        "  ,                                       !- Number of Vertices",
        "  10,                                     !- Vertex 1 X-coordinate {m}",
        "  9.9746,                                 !- Vertex 1 Y-coordinate {m}",
        "  1.96812712579906,                       !- Vertex 1 Z-coordinate {m}",
        "  10,                                     !- Vertex 2 X-coordinate {m}",
        "  9.9746,                                 !- Vertex 2 Y-coordinate {m}",
        "  0.762,                                  !- Vertex 2 Z-coordinate {m}",
        "  10,                                     !- Vertex 3 X-coordinate {m}",
        "  0.0254,                                 !- Vertex 3 Y-coordinate {m}",
        "  0.762,                                  !- Vertex 3 Z-coordinate {m}",
        "  10,                                     !- Vertex 4 X-coordinate {m}",
        "  0.0254,                                 !- Vertex 4 Y-coordinate {m}",
        "  1.96812712579906;                       !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Space4 Wall 4,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Exterior Wall Construction,             !- Construction Name",
        "  Zone4,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Outdoors,                               !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  SunExposed,                             !- Sun Exposure",
        "  WindExposed,                            !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  0, 10, 3,                               !- X,Y,Z Vertex 1 {m}",
        "  10, 10, 3,                              !- X,Y,Z Vertex 2 {m}",
        "  10, 10, 0,                              !- X,Y,Z Vertex 3 {m}",
        "  0, 10, 0;                               !- X,Y,Z Vertex 4 {m}",
    });

    std::string const constructions = delimited_string({
        "ConstructionAssignmentSet,",
        "  Default Construction Set,               !- Name",
        "  Exterior Surface Constructions,         !- Exterior Surface Construction Assignments Name",
        "  Interior Surface Constructions,         !- Interior Surface Construction Assignments Name",
        "  Ground Surface Constructions,           !- Ground Contact Surface Construction Assignments Name",
        "  Exterior SubSurface Constructions,      !- Exterior SubSurface Construction Assignments Name",
        "  Interior SubSurface Constructions,      !- Interior SubSurface Construction Assignments Name",
        "  Interior Partition Construction,        !- Interior Partition Construction Name",
        "  Adiabatic Surface Construction;         !- Adiabatic Surface Construction Name",

        "SurfaceConstructionAssignments,",
        "  Exterior Surface Constructions,         !- Name",
        "  Exterior Floor Construction,            !- Floor Construction Name",
        "  Exterior Wall Construction,             !- Wall Construction Name",
        "  Exterior Roof Construction;             !- Roof Ceiling Construction Name",

        "SurfaceConstructionAssignments,",
        "  Interior Surface Constructions,         !- Name",
        "  Interior Floor Construction,            !- Floor Construction Name",
        "  Interior Wall Construction,             !- Wall Construction Name",
        "  Interior Roof Construction;             !- Roof Ceiling Construction Name",

        "SurfaceConstructionAssignments,",
        "  Ground Surface Constructions,           !- Name",
        "  Ground Floor Construction,              !- Floor Construction Name",
        "  Ground Wall Construction,               !- Wall Construction Name",
        "  Ground Roof Construction;               !- Roof Ceiling Construction Name",

        "SubSurfaceConstructionAssignments,",
        "  Exterior SubSurface Constructions,      !- Name",
        "  Exterior FixedWindow Construction,      !- Fixed Window Construction Name",
        "  Exterior OperableWindow Construction,   !- Operable Window Construction Name",
        "  Exterior Door Construction,             !- Door Construction Name",
        "  Exterior GlassDoor Construction,        !- Glass Door Construction Name",
        "  Exterior OverheadDoor Construction,     !- Overhead Door Construction Name",
        "  Exterior Skylight Construction,         !- Skylight Construction Name",
        "  Exterior TubularDaylightDome Construction, !- Tubular Daylight Dome Construction Name",
        "  Exterior TubularDaylightDiffuser Construction; !- Tubular Daylight Diffuser Construction Name",

        "SubSurfaceConstructionAssignments,",
        "  Interior SubSurface Constructions,      !- Name",
        "  Interior FixedWindow Construction,      !- Fixed Window Construction Name",
        "  Interior OperableWindow Construction,   !- Operable Window Construction Name",
        "  Interior Door Construction,             !- Door Construction Name",
        "  Interior GlassDoor Construction,        !- Glass Door Construction Name",
        "  Interior OverheadDoor Construction,     !- Overhead Door Construction Name",
        "  Interior Skylight Construction,         !- Skylight Construction Name",
        "  Interior TubularDaylightDome Construction, !- Tubular Daylight Dome Construction Name",
        "  Interior TubularDaylightDiffuser Construction; !- Tubular Daylight Diffuser Construction Name",

        "Construction,",
        "  Exterior Floor Construction,            !- Name",
        "  Mat Opaque;                             !- Outside Layer",

        "Construction,",
        "  Exterior Wall Construction,             !- Name",
        "  Mat Opaque;                             !- Outside Layer",

        "Construction,",
        "  Exterior Roof Construction,             !- Name",
        "  Mat Opaque;                             !- Outside Layer",

        "Construction,",
        "  Interior Floor Construction,            !- Name",
        "  Mat Opaque;                             !- Outside Layer",

        "Construction,",
        "  Interior Wall Construction,             !- Name",
        "  Mat Opaque;                             !- Outside Layer",

        "Construction,",
        "  Interior Roof Construction,             !- Name",
        "  Mat Opaque;                             !- Outside Layer",

        "Construction,",
        "  Ground Floor Construction,              !- Name",
        "  Mat Opaque;                             !- Outside Layer",

        "Construction,",
        "  Ground Wall Construction,               !- Name",
        "  Mat Opaque;                             !- Outside Layer",

        "Construction,",
        "  Ground Roof Construction,               !- Name",
        "  Mat Opaque;                             !- Outside Layer",

        "Construction,",
        "  Exterior FixedWindow Construction,      !- Name",
        "  Mat Glazing;                            !- Outside Layer",

        "Construction,",
        "  Exterior OperableWindow Construction,   !- Name",
        "  Mat Glazing;                            !- Outside Layer",

        "Construction,",
        "  Exterior Door Construction,             !- Name",
        "  Mat Opaque;                             !- Outside Layer",

        "Construction,",
        "  Exterior GlassDoor Construction,        !- Name",
        "  Mat Glazing;                            !- Outside Layer",

        "Construction,",
        "  Exterior OverheadDoor Construction,     !- Name",
        "  Mat Opaque;                             !- Outside Layer",

        "Construction,",
        "  Exterior Skylight Construction,         !- Name",
        "  Mat Glazing;                            !- Outside Layer",

        "Construction,",
        "  Exterior TubularDaylightDome Construction, !- Name",
        "  Mat Glazing;                            !- Outside Layer",

        "Construction,",
        "  Exterior TubularDaylightDiffuser Construction, !- Name",
        "  Mat Glazing;                            !- Outside Layer",

        "Construction,",
        "  Interior FixedWindow Construction,      !- Name",
        "  Mat Glazing;                            !- Outside Layer",

        "Construction,",
        "  Interior OperableWindow Construction,   !- Name",
        "  Mat Glazing;                            !- Outside Layer",

        "Construction,",
        "  Interior Door Construction,             !- Name",
        "  Mat Opaque;                             !- Outside Layer",

        "Construction,",
        "  Interior GlassDoor Construction,        !- Name",
        "  Mat Glazing;                            !- Outside Layer",

        "Construction,",
        "  Interior OverheadDoor Construction,     !- Name",
        "  Mat Opaque;                             !- Outside Layer",

        "Construction,",
        "  Interior Skylight Construction,         !- Name",
        "  Mat Glazing;                            !- Outside Layer",

        "Construction,",
        "  Interior TubularDaylightDome Construction, !- Name",
        "  Mat Glazing;                            !- Outside Layer",

        "Construction,",
        "  Interior TubularDaylightDiffuser Construction, !- Name",
        "  Mat Glazing;                            !- Outside Layer",

        "Construction,",
        "  Interior Partition Construction,        !- Name",
        "  Mat Opaque;                             !- Outside Layer",

        "Construction,",
        "  Adiabatic Surface Construction,         !- Name",
        "  Mat Opaque;                             !- Outside Layer",

        "Material:NoMass,",
        "  Mat Opaque,                             !- Name",
        "  Rough,                                  !- Roughness",
        "  0.5,                                    !- Thermal Resistance {m2-K/W}",
        "  0.9,                                    !- Thermal Absorptance",
        "  0.9,                                    !- Solar Absorptance",
        "  0.9;                                    !- Visible Absorptance",

        "WindowMaterial:SimpleGlazingSystem,",
        "  Mat Glazing,                            !- Name",
        "  0.1,                                    !- U-Factor {W/m2-K}",
        "  0.65;                                   !- Solar Heat Gain Coefficient",
    });

    ASSERT_TRUE(process_idf(constructions + geometry));
    state->init_state(*state);

    bool ErrorsFound = false;
    Material::GetMaterialData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);
    HeatBalanceManager::GetFrameAndDividerData(*state);
    HeatBalanceManager::SetPreConstructionInputParameters(*state);
    HeatBalanceManager::GetConstructData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);
    ConstructionAssignments::GetConstructionAssignmentSetData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);
    HeatBalanceManager::GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);
    SurfaceGeometry::GetGeometryParameters(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);
    SurfaceGeometry::SetupZoneGeometry(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    ASSERT_EQ(1u, state->dataConstructionAssignments->constructionAssignmentSets.size());
    auto const &dcs = state->dataConstructionAssignments->constructionAssignmentSets[0];

    auto checkDefaultConstruction = [&](const std::string &expectedConstrName, const std::string &surfName) {
        const std::string &expectedConstrNameUpper = Util::makeUPPER(expectedConstrName);
        int const surfIdx = Util::FindItemInList(Util::makeUPPER(surfName), state->dataSurface->Surface);
        ASSERT_GT(surfIdx, 0) << "Surface not found: " << surfName;
        DataSurfaces::SurfaceData const &surf = state->dataSurface->Surface(surfIdx);

        int const defaultConstrIdx = dcs.getConstructionAssignment(surf);
        ASSERT_GT(defaultConstrIdx, 0) << "No default construction for surface: " << surf.Name;
        EXPECT_EQ(expectedConstrNameUpper, state->dataConstruction->Construct(defaultConstrIdx).Name)
            << "Default construction mismatch for surface: " << surf.Name;

        int const hardAssignedConstruction = surf.Construction;
        ASSERT_GT(hardAssignedConstruction, 0) << "Surface has no assigned construction: " << surf.Name;
        EXPECT_EQ(expectedConstrNameUpper, state->dataConstruction->Construct(hardAssignedConstruction).Name);
    };

    // Exterior Surfaces
    /// Exterior Floor
    checkDefaultConstruction("Exterior Floor Construction", "Space1 Floor");

    /// Exterior Wall
    checkDefaultConstruction("Exterior Wall Construction", "Space4 Wall 4");
    checkDefaultConstruction("Exterior Wall Construction", "Space4 Wall 2");
    checkDefaultConstruction("Exterior Wall Construction", "Space4 Wall 1");
    checkDefaultConstruction("Exterior Wall Construction", "Space3 Wall 4");
    checkDefaultConstruction("Exterior Wall Construction", "Space3 Wall 3");
    checkDefaultConstruction("Exterior Wall Construction", "Space3 Wall 2");
    checkDefaultConstruction("Exterior Wall Construction", "Space2 Wall 4");
    checkDefaultConstruction("Exterior Wall Construction", "Space1 Wall 4");
    checkDefaultConstruction("Exterior Wall Construction", "Space1 Wall 3");
    checkDefaultConstruction("Exterior Wall Construction", "Space1 Wall 2");

    /// Exterior Roof
    checkDefaultConstruction("Exterior Roof Construction", "Space4 RoofCeiling");
    checkDefaultConstruction("Exterior Roof Construction", "Space3 RoofCeiling");

    // Interior Surfaces
    /// Interior Floor
    checkDefaultConstruction("Interior Floor Construction", "Space4 Floor");
    checkDefaultConstruction("Interior Floor Construction", "Space3 Floor");

    /// Interior Wall
    checkDefaultConstruction("Interior Wall Construction", "Space4 Wall 3");
    checkDefaultConstruction("Interior Wall Construction", "Space3 Wall 1");
    checkDefaultConstruction("Interior Wall Construction", "Space2 Wall 3");
    checkDefaultConstruction("Interior Wall Construction", "Space1 Wall 1");

    /// Interior Roof
    checkDefaultConstruction("Interior Roof Construction", "Space2 RoofCeiling");
    checkDefaultConstruction("Interior Roof Construction", "Space1 RoofCeiling");

    // Ground Surfaces
    /// Ground Floor
    checkDefaultConstruction("Ground Floor Construction", "Space2 Floor");

    /// Ground Wall
    checkDefaultConstruction("Ground Wall Construction", "Space2 Wall 2");

    // Adiabatic Surfaces
    /// Adiabatic Wall
    checkDefaultConstruction("Adiabatic Surface Construction", "Space2 Wall 1");

    // ==============================================================
    // Exterior SubSurfaces
    /// Exterior Window
    checkDefaultConstruction("Exterior FixedWindow Construction", "ExteriorWindow - Window");

    /// Exterior FixedWindow
    checkDefaultConstruction("Exterior FixedWindow Construction", "ExteriorWindow - FixedWindow");

    /// Exterior OperableWindow
    checkDefaultConstruction("Exterior OperableWindow Construction", "ExteriorWindow - OperableWindow");

    /// Exterior Door
    checkDefaultConstruction("Exterior Door Construction", "ExteriorDoor - Door");

    /// Exterior GlassDoor
    checkDefaultConstruction("Exterior GlassDoor Construction", "ExteriorDoor - GlassDoor");

    /// Exterior OverheadDoor
    checkDefaultConstruction("Exterior OverheadDoor Construction", "ExteriorDoor - OverheadDoor");

    /// Exterior Skylight
    checkDefaultConstruction("Exterior Skylight Construction", "ExteriorWindow - Skylight");

    // Interior SubSurfaces
    /// Interior FixedWindow
    checkDefaultConstruction("Interior FixedWindow Construction", "InteriorWindow - FixedWindow - Reversed");
    checkDefaultConstruction("Interior FixedWindow Construction", "InteriorWindow - FixedWindow");

    /// Interior Door
    checkDefaultConstruction("Interior Door Construction", "InteriorDoor - Door - Reversed");
    checkDefaultConstruction("Interior Door Construction", "InteriorDoor - Door");
}
