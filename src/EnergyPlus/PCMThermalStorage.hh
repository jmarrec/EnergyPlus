// PCM Thermal Storage Module - PCMThermalStorage.hh

#ifndef ENERGYPLUS_PCMTHERMALSTORAGE_HH
#define ENERGYPLUS_PCMTHERMALSTORAGE_HH

#include <EnergyPlus/Data/EnergyPlusData.hh>
#include <EnergyPlus/Plant/PlantLocation.hh>
#include <EnergyPlus/PlantComponent.hh>
#include <string>

namespace EnergyPlus {
namespace PCMStorage {

    struct PCMStorageData : public PlantComponent
    {
        bool Initialized = false;

        // Inputs
        std::string Name;
        std::string AvailabilityScheduleName;
        Real64 TankCapacity = 0.0;  // kg
        Real64 HeatLossRate = 0.0;  // W
        Real64 MeltingTemp = 0.0;   // C
        Real64 FreezingTemp = 0.0;  // C
        Real64 LatentHeat = 0.0;    // J/kg
        Real64 SpecificHeat = 0.0;  // J/kg-C
        Real64 Effectiveness = 0.9; // Will probably change it later as an input

        EnergyPlus::PlantLocation plantLoc;
        // Node numbers
        int PlantSideInletNode = 0;
        int PlantSideOutletNode = 0;
        int UseSideInletNode = 0;
        int UseSideOutletNode = 0;

        // Derived data
        int AvailabilityScheduleIndex = 0;
        Real64 EnergyStored = 0.0;      // J
        Real64 PercentCapacity = 0.0;   // %
        Real64 HeatLossRate_W = 0.0;    // W
        Real64 useheatTransfer = 0.0;   // W
        Real64 plantheatTransfer = 0.0; // W
        Real64 DesignMassFlowRate = 0.0;

        // Initialization flags
        bool MyPlantScanFlag = true;
        bool MyEnvrnFlag = true;

        void Init(EnergyPlusData &state);
        void Calculate(EnergyPlusData &state, PlantLocation const &plantLoc);

        static PCMStorageData &instance();
        static PlantComponent *factory(EnergyPlusData &state, std::string const &objectName);

        // Required overrides from PlantComponent
        void
        simulate(EnergyPlusData &state, const PlantLocation &calledFromLocation, bool FirstHVACIteration, Real64 &CurLoad, bool RunFlag) override;

        void oneTimeInit(EnergyPlusData &state) override;
    };

    void SimulatePCMStorage(EnergyPlusData &state, PlantLocation const &plantLoc, bool FirstHVACIteration, Real64 &CurLoad, bool RunFlag);

    void GetPCMStorageInput(EnergyPlusData &state);
    void RegisterPCMStorageOutputVariables(EnergyPlusData &state);

} // namespace PCMStorage
} // namespace EnergyPlus

#endif // ENERGYPLUS_PCMTHERMALSTORAGE_HH
