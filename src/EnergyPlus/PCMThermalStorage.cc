// EnergyPlus, Copyright (c) 1996-2025, The Board of Trustees of the University of Illinois,
// The Regents of the University of California, through Lawrence Berkeley National Laboratory
// (subject to receipt of any required approvals from the U.S. Dept. of Energy), Oak Ridge
// National Laboratory, managed by UT-Battelle, Alliance for Sustainable Energy, LLC, and other
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

// PCM Thermal Storage Module - PCMThermalStorage.cc

#include "PCMThermalStorage.hh"
#include <EnergyPlus/BranchNodeConnections.hh>
#include <EnergyPlus/Data/EnergyPlusData.hh>
#include <EnergyPlus/DataGlobals.hh>
#include <EnergyPlus/DataHeatBalance.hh>
#include <EnergyPlus/DataIPShortCuts.hh>
#include <EnergyPlus/DataLoopNode.hh>
#include <EnergyPlus/InputProcessing/InputProcessor.hh>
#include <EnergyPlus/NodeInputManager.hh>
#include <EnergyPlus/OutputProcessor.hh>
#include <EnergyPlus/PhaseChangeModeling/HysteresisModel.hh>
#include <EnergyPlus/Plant/DataPlant.hh>
#include <EnergyPlus/Plant/PlantLocation.hh>
#include <EnergyPlus/PlantComponent.hh>
#include <EnergyPlus/PlantUtilities.hh>
#include <EnergyPlus/HeatBalFiniteDiffManager.hh>
#include <EnergyPlus/ScheduleManager.hh>
#include <EnergyPlus/UtilityRoutines.hh>

namespace EnergyPlus {
namespace PCMStorage {

    PlantComponent *PCMStorageData::factory(EnergyPlusData &state, std::string const &objectName)
    {
        static bool getPCMInputFlag = true;
        if (getPCMInputFlag) {
            PCMStorage::GetPCMStorageInput(state);
            getPCMInputFlag = false;
        }

        auto &pcm = PCMStorageData::instance();
        if (pcm.Name == objectName) {
            return static_cast<PlantComponent *>(&pcm);
        }

        ShowFatalError(state, "PCMStorage factory: No PCM storage found with name: " + objectName);
        return nullptr;
    }

    void SimulatePCMStorage(EnergyPlusData &state, PlantLocation const &plantLoc, bool FirstHVACIteration, Real64 &CurLoad, bool RunFlag)
    {
        auto &PCM = PCMStorageData::instance();

        if (!PCM.Initialized) {
            PCM.Init(state);
        }

        PCM.Calculate(state, plantLoc);
    }

    void
    PCMStorageData::simulate(EnergyPlusData &state, const PlantLocation &calledFromLocation, bool FirstHVACIteration, Real64 &CurLoad, bool RunFlag)
    {
        SimulatePCMStorage(state, calledFromLocation, FirstHVACIteration, CurLoad, RunFlag);
    }

    void PCMStorageData::oneTimeInit(EnergyPlusData &state)
    {
        this->Init(state);
    }

    void PCMStorageData::Init(EnergyPlusData &state)
    {
        // Run once to find this component in the plant loops
        if (this->MyPlantScanFlag) {
            bool errFlag = false;

            PlantUtilities::ScanPlantLoopsForObject(state, this->Name, DataPlant::PlantEquipmentType::TS_PCM, this->plantLoc, errFlag, _, _, _, _, _);

            if (errFlag) {
                ShowFatalError(state, "PCMStorageData::Init: Error scanning plant loops for PCM tank named: " + this->Name);
            }

            RegisterPCMStorageOutputVariables(state);
            this->MyPlantScanFlag = false;
        }

        // Reset at the start of each environment (e.g. design day)
        if (state.dataGlobal->BeginEnvrnFlag && this->MyEnvrnFlag) {
            this->DesignMassFlowRate = state.dataPlnt->PlantLoop(this->plantLoc.loopNum).MaxMassFlowRate;

            PlantUtilities::InitComponentNodes(state, 0.0, this->DesignMassFlowRate, this->PlantSideInletNode, this->PlantSideOutletNode);

            if ((state.dataPlnt->PlantLoop(this->plantLoc.loopNum).CommonPipeType == DataPlant::CommonPipeType::TwoWay) &&
                (this->plantLoc.loopSideNum == DataPlant::LoopSideLocation::Supply)) {
                for (int compNum = 1; compNum <= state.dataPlnt->PlantLoop(this->plantLoc.loopNum)
                                                     .LoopSide(DataPlant::LoopSideLocation::Supply)
                                                     .Branch(this->plantLoc.branchNum)
                                                     .TotalComponents;
                     ++compNum) {
                    state.dataPlnt->PlantLoop(this->plantLoc.loopNum)
                        .LoopSide(DataPlant::LoopSideLocation::Supply)
                        .Branch(this->plantLoc.branchNum)
                        .Comp(compNum)
                        .FlowPriority = DataPlant::LoopFlowStatus::NeedyAndTurnsLoopOn;
                }
            }

            // Reset state
            this->EnergyStored = 0.0;
            this->PercentCapacity = 100.0;

            this->MyEnvrnFlag = false;
        }

        if (!state.dataGlobal->BeginEnvrnFlag) {
            this->MyEnvrnFlag = true;
        }

        // Always recheck schedule (safe, and efficient)
        this->AvailabilityScheduleIndex = Sched::GetScheduleNum(state, this->AvailabilityScheduleName);
        if (this->AvailabilityScheduleIndex == 0) {
            ShowFatalError(state, "PCMStorageData::Init: Invalid schedule name: " + this->AvailabilityScheduleName);
        }
    }

    void PCMStorageData::Calculate(EnergyPlusData &state, PlantLocation const &)
    {
        auto &useInlet = state.dataLoopNodes->Node(UseSideInletNode);
        auto &useOutlet = state.dataLoopNodes->Node(UseSideOutletNode);
        auto &plantInlet = state.dataLoopNodes->Node(PlantSideInletNode);
        auto &plantOutlet = state.dataLoopNodes->Node(PlantSideOutletNode);

        int index = Sched::GetScheduleNum(state, this->AvailabilityScheduleName);

        Sched::Schedule *schedPtr = state.dataSched->schedules[index];

        Real64 avail = schedPtr->getCurrentVal();

        if (avail <= 0.0) return;

        Real64 CpWater = 4180.0; // J/kg-C
        Real64 massFlowUse = useInlet.MassFlowRate;
        Real64 massFlowPlant = plantInlet.MassFlowRate;

        Real64 plantOutletTemp = plantInlet.Temp - (Effectiveness * (plantInlet.Temp - FreezingTemp)); // Calculate Plant Outlet Temperature
        Real64 useOutletTemp = useInlet.Temp + (Effectiveness * (MeltingTemp - useInlet.Temp));        // Calculate Use Outlet Temperature

        Real64 deltaTUse = useInlet.Temp - useOutletTemp;       // Heat to Water Heater
        Real64 deltaTPlant = plantInlet.Temp - plantOutletTemp; // Heat to PCM Tank

        Real64 useheatTransfer = massFlowUse * CpWater * deltaTUse;       // Heat to Water Heater
        Real64 plantheatTransfer = massFlowPlant * CpWater * deltaTPlant; // Heat to PCM Tank
        HeatLossRate_W = HeatLossRate;

        // Calculate tank temperature from stored energy
        if (this->PCMmat) {
            Real64 targetEnthalpy = EnergyStored / TankCapacity;
            Real64 Tlow = this->PCMmat->peakTempMelting - 30.0;
            Real64 Thigh = this->PCMmat->peakTempMelting + 30.0;
            Real64 approxTemp = this->PCMmat->peakTempMelting;

            for (int i = 0; i < 25; ++i) {
                Real64 Tmid = 0.5 * (Tlow + Thigh);
                Real64 hMid = this->PCMmat->getEnthalpy(
                    Tmid, this->PCMmat->peakTempMelting, this->PCMmat->deltaTempMeltingLow, this->PCMmat->deltaTempMeltingHigh);
                if (std::abs(hMid - targetEnthalpy) < 0.1) {
                    approxTemp = Tmid;
                    break;
                }
                if (hMid > targetEnthalpy) {
                    Thigh = Tmid;
                } else {
                    Tlow = Tmid;
                }
            }

            this->PCM_TankTemp = approxTemp;
        }

        // Use PCM_TankTemp to determine operation mode
        bool charging = plantheatTransfer > 0.0 && PCM_TankTemp < MeltingTemp;
        bool discharging = useheatTransfer < 0.0 && PCM_TankTemp > FreezingTemp;

        // Apply operation mode
        if (discharging) {
            EnergyStored += useheatTransfer - HeatLossRate_W;
            useOutlet.Temp = useOutletTemp;
        } else if (charging) {
            EnergyStored += plantheatTransfer - HeatLossRate_W;
            plantOutlet.Temp = plantOutletTemp;
        } else {
            EnergyStored -= HeatLossRate_W;
        }

        EnergyStored = max(0.0, min(EnergyStored, TankCapacity * LatentHeat));
        PercentCapacity = 100.0 * EnergyStored / (TankCapacity * LatentHeat);
    }

    void RegisterPCMStorageOutputVariables(EnergyPlusData &state)
    {
        auto &PCM = PCMStorageData::instance();

        using Constant::Units;
        using OutputProcessor::StoreType;
        using OutputProcessor::TimeStepType;

        SetupOutputVariable(
            state, "Thermal Energy Storage Percent Capacity", Units::None, PCM.PercentCapacity, TimeStepType::System, StoreType::Average, PCM.Name);

        SetupOutputVariable(
            state, "Thermal Energy Storage Heat Loss Rate", Units::W, PCM.HeatLossRate_W, TimeStepType::System, StoreType::Average, PCM.Name);

        SetupOutputVariable(
            state, "Thermal Energy Storage Energy Stored", Units::W, PCM.EnergyStored, TimeStepType::System, StoreType::Average, PCM.Name);

        SetupOutputVariable(
            state, "Thermal Energy Storage Tank Temperature", Units::C, PCM.PCM_TankTemp, TimeStepType::System, StoreType::Average, PCM.Name);

        SetupOutputVariable(state,
                            "Thermal Energy Storage Use Side Heat Transfer Rate",
                            Units::W,
                            PCM.useheatTransfer,
                            TimeStepType::System,
                            StoreType::Average,
                            PCM.Name);

        SetupOutputVariable(state,
                            "Thermal Energy Storage Plant Side Heat Transfer Rate",
                            Units::W,
                            PCM.plantheatTransfer,
                            TimeStepType::System,
                            StoreType::Average,
                            PCM.Name);

        SetupOutputVariable(
            state, "Thermal Energy Storage Latent Heat Capacity", Units::J, PCM.EnergyStored, TimeStepType::System, StoreType::Sum, PCM.Name);

        SetupOutputVariable(state,
                            "Thermal Energy Storage Use Side Inlet Temperature",
                            Units::C,
                            state.dataLoopNodes->Node(PCM.UseSideInletNode).Temp,
                            TimeStepType::System,
                            StoreType::Average,
                            PCM.Name);

        SetupOutputVariable(state,
                            "Thermal Energy Storage Use Side Outlet Temperature",
                            Units::C,
                            state.dataLoopNodes->Node(PCM.UseSideOutletNode).Temp,
                            TimeStepType::System,
                            StoreType::Average,
                            PCM.Name);

        SetupOutputVariable(state,
                            "Thermal Energy Storage Use Side Mass Flow Rate",
                            Units::kg_s,
                            state.dataLoopNodes->Node(PCM.UseSideInletNode).MassFlowRate,
                            TimeStepType::System,
                            StoreType::Average,
                            PCM.Name);

        SetupOutputVariable(state,
                            "Thermal Energy Storage Plant Side Inlet Temperature",
                            Units::C,
                            state.dataLoopNodes->Node(PCM.PlantSideInletNode).Temp,
                            TimeStepType::System,
                            StoreType::Average,
                            PCM.Name);

        SetupOutputVariable(state,
                            "Thermal Energy Storage Plant Side Outlet Temperature",
                            Units::C,
                            state.dataLoopNodes->Node(PCM.PlantSideOutletNode).Temp,
                            TimeStepType::System,
                            StoreType::Average,
                            PCM.Name);

        SetupOutputVariable(state,
                            "Thermal Energy Storage Plant Side Mass Flow Rate",
                            Units::kg_s,
                            state.dataLoopNodes->Node(PCM.PlantSideInletNode).MassFlowRate,
                            TimeStepType::System,
                            StoreType::Average,
                            PCM.Name);
    }

    PCMStorageData &PCMStorageData::instance()
    {
        static PCMStorageData inst;
        return inst;
    }

    void GetPCMStorageInput(EnergyPlusData &state)
    {
        constexpr std::string_view RoutineName = "GetPCMStorageInput";

        auto &PCM = PCMStorageData::instance();
        bool ErrorsFound = false;

        int NumPCMObjs = state.dataInputProcessing->inputProcessor->getNumObjectsFound(state, "ThermalStorage:PCM");
        if (NumPCMObjs != 1) {
            ShowSevereError(state, "Exactly one ThermalStorage:PCM object is required.");
            ErrorsFound = true;
            return;
        }

        state.dataIPShortCut->cCurrentModuleObject = "ThermalStorage:PCM";

        int NumAlphas;
        int NumNums;
        int IOStat;

        state.dataInputProcessing->inputProcessor->getObjectItem(state,
                                                                 state.dataIPShortCut->cCurrentModuleObject,
                                                                 1,
                                                                 state.dataIPShortCut->cAlphaArgs,
                                                                 NumAlphas,
                                                                 state.dataIPShortCut->rNumericArgs,
                                                                 NumNums,
                                                                 IOStat,
                                                                 _,
                                                                 state.dataIPShortCut->lAlphaFieldBlanks,
                                                                 state.dataIPShortCut->cAlphaFieldNames,
                                                                 state.dataIPShortCut->cNumericFieldNames);

        Util::IsNameEmpty(state, state.dataIPShortCut->cAlphaArgs(1), state.dataIPShortCut->cCurrentModuleObject, ErrorsFound);

        PCM.Name = state.dataIPShortCut->cAlphaArgs(1);
        PCM.AvailabilityScheduleName = state.dataIPShortCut->cAlphaArgs(2);

        int matNum = Material::GetMaterialNum(state, state.dataIPShortCut->cAlphaArgs(7));
        if (matNum == 0) {
            ShowSevereError(
                state, format("{}: Invalid PCM material name: {}", state.dataIPShortCut->cCurrentModuleObject, state.dataIPShortCut->cAlphaArgs(7)));
            ErrorsFound = true;
           } else {
            // Obtains conduction FD related parameters from input file
               std::string savedModuleObj = state.dataIPShortCut->cCurrentModuleObject;

               if (state.dataHeatBalFiniteDiffMgr->GetHBFiniteDiffInputFlag) {
                   HeatBalFiniteDiffManager::GetCondFDInput(state);
                   state.dataHeatBalFiniteDiffMgr->GetHBFiniteDiffInputFlag = false;
               }

               // --- Restore the module name for proper error reporting
               state.dataIPShortCut->cCurrentModuleObject = savedModuleObj;

            auto *mat = state.dataMaterial->materials(matNum);

            // If using MaterialProperty:PhaseChange, set hasPCM manually after FD input is read
            if (!mat->hasPCM) {
                if (dynamic_cast<Material::MaterialPhaseChange *>(mat) != nullptr) {
                    mat->hasPCM = true;
                }
            }

            if (!mat->hasPCM) {
                ShowSevereError(state,
                                format("{}: Material {} is not a phase change material.", state.dataIPShortCut->cCurrentModuleObject, mat->Name));
                ErrorsFound = true;
            } else {
                PCM.PCMMaterialNum = matNum;
                PCM.PCMmat = dynamic_cast<Material::MaterialPhaseChange *>(mat);

                PCM.TankCapacity = state.dataIPShortCut->rNumericArgs(1);
                PCM.HeatLossRate = state.dataIPShortCut->rNumericArgs(2);

                PCM.MeltingTemp = PCM.PCMmat->peakTempMelting;
                PCM.FreezingTemp = PCM.PCMmat->peakTempFreezing;
                PCM.LatentHeat = PCM.PCMmat->totalLatentHeat;
                PCM.SpecificHeat = (PCM.PCMmat->specificHeatSolid + PCM.PCMmat->specificHeatLiquid) / 2.0;
            }
        }

        PCM.PlantSideInletNode = NodeInputManager::GetOnlySingleNode(state,
                                                                     state.dataIPShortCut->cAlphaArgs(3),
                                                                     ErrorsFound,
                                                                     DataLoopNode::ConnectionObjectType::ThermalStoragePCM,
                                                                     PCM.Name,
                                                                     DataLoopNode::NodeFluidType::Water,
                                                                     DataLoopNode::ConnectionType::Inlet,
                                                                     NodeInputManager::CompFluidStream::Primary,
                                                                     DataLoopNode::ObjectIsNotParent);

        PCM.PlantSideOutletNode = NodeInputManager::GetOnlySingleNode(state,
                                                                      state.dataIPShortCut->cAlphaArgs(4),
                                                                      ErrorsFound,
                                                                      DataLoopNode::ConnectionObjectType::ThermalStoragePCM,
                                                                      PCM.Name,
                                                                      DataLoopNode::NodeFluidType::Water,
                                                                      DataLoopNode::ConnectionType::Outlet,
                                                                      NodeInputManager::CompFluidStream::Primary,
                                                                      DataLoopNode::ObjectIsNotParent);

        PCM.UseSideInletNode = NodeInputManager::GetOnlySingleNode(state,
                                                                   state.dataIPShortCut->cAlphaArgs(5),
                                                                   ErrorsFound,
                                                                   DataLoopNode::ConnectionObjectType::ThermalStoragePCM,
                                                                   PCM.Name,
                                                                   DataLoopNode::NodeFluidType::Water,
                                                                   DataLoopNode::ConnectionType::Inlet,
                                                                   NodeInputManager::CompFluidStream::Secondary,
                                                                   DataLoopNode::ObjectIsNotParent);

        PCM.UseSideOutletNode = NodeInputManager::GetOnlySingleNode(state,
                                                                    state.dataIPShortCut->cAlphaArgs(6),
                                                                    ErrorsFound,
                                                                    DataLoopNode::ConnectionObjectType::ThermalStoragePCM,
                                                                    PCM.Name,
                                                                    DataLoopNode::NodeFluidType::Water,
                                                                    DataLoopNode::ConnectionType::Outlet,
                                                                    NodeInputManager::CompFluidStream::Secondary,
                                                                    DataLoopNode::ObjectIsNotParent);

        BranchNodeConnections::TestCompSet(state,
                                           state.dataIPShortCut->cCurrentModuleObject,
                                           PCM.Name,
                                           state.dataIPShortCut->cAlphaArgs(3),
                                           state.dataIPShortCut->cAlphaArgs(4),
                                           "PCM Storage Plant Side");

        BranchNodeConnections::TestCompSet(state,
                                           state.dataIPShortCut->cCurrentModuleObject,
                                           PCM.Name,
                                           state.dataIPShortCut->cAlphaArgs(5),
                                           state.dataIPShortCut->cAlphaArgs(6),
                                           "PCM Storage Use Side");

        if (PCM.TankCapacity <= 0.0) {
            ShowSevereError(state, format("{}={}, Tank Capacity must be > 0.0.", state.dataIPShortCut->cCurrentModuleObject, PCM.Name));
            ErrorsFound = true;
        }

        if (ErrorsFound) {
            ShowFatalError(state, format("Errors found in processing input for {}", state.dataIPShortCut->cCurrentModuleObject));
        }
    }

} // namespace PCMStorage
} // namespace EnergyPlus
