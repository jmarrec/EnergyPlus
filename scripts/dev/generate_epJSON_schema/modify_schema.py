# EnergyPlus, Copyright (c) 1996-2021, The Board of Trustees of the University
# of Illinois, The Regents of the University of California, through Lawrence
# Berkeley National Laboratory (subject to receipt of any required approvals
# from the U.S. Dept. of Energy), Oak Ridge National Laboratory, managed by UT-
# Battelle, Alliance for Sustainable Energy, LLC, and other contributors. All
# rights reserved.
#
# NOTICE: This Software was developed under funding from the U.S. Department of
# Energy and the U.S. Government consequently retains certain rights. As such,
# the U.S. Government has been granted for itself and others acting on its
# behalf a paid-up, nonexclusive, irrevocable, worldwide license in the
# Software to reproduce, distribute copies to the public, prepare derivative
# works, and perform publicly and display publicly, and to permit others to do
# so.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# (1) Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
#
# (2) Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
#
# (3) Neither the name of the University of California, Lawrence Berkeley
#     National Laboratory, the University of Illinois, U.S. Dept. of Energy nor
#     the names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
# (4) Use of EnergyPlus(TM) Name. If Licensee (i) distributes the software in
#     stand-alone form without changes from the version obtained under this
#     License, or (ii) Licensee makes a reference solely to the software
#     portion of its product, Licensee must refer to the software as
#     "EnergyPlus version X" software, where "X" is the version number Licensee
#     obtained under this License and may not use a different name for the
#     software. Except as specifically required in this Section (4), Licensee
#     shall not use in a company name, a product name, in advertising,
#     publicity, or other promotional activities any name, trade name,
#     trademark, logo, or other designation of "EnergyPlus", "E+", "e+" or
#     confusingly similar designation, without the U.S. Department of Energy's
#     prior written consent.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

def anyOf():
    return [
        {
            "type": "number",
        },
        {
            "type": "string"
        }
    ]


def isInt(s):
    try:
        int(s)
        return True
    except ValueError:
        return False

extension_renaming = {
    'OS:LifeCycleCost:UseAdjustment': 'multipliers',
    'OS:LifeCycleCost:UsePriceEscalation': 'escalations',
    'OS:ElectricLoadCenter:Transformer': 'meters',
    'OS:ElectricLoadCenter:Generators': 'generator_outputs',
    'OS:Generator:FuelCell:AirSupply': 'constituent_fractions',
    'OS:DemandManager:ExteriorLights': 'lights',
    'OS:DemandManager:Ventilation': 'controllers',
    'OS:DemandManagerAssignmentList': 'manager_data',
    'OS:DemandManager:Lights': 'lights',
    'OS:DemandManager:Thermostats': 'thermostats',
    'OS:DemandManager:ElectricEquipment': 'equipment',
    'OS:DaylightingDevice:Tubular': 'transition_lengths',
    'OS:Daylighting:Controls': 'control_data',
    'OS:ZoneHVAC:Baseboard:RadiantConvective:Steam': 'surface_fractions',
    'OS:ZoneHVAC:Baseboard:RadiantConvective:Electric': 'surface_fractions',
    'OS:ZoneHVAC:HighTemperatureRadiant': 'surface_fractions',
    'OS:ZoneHVAC:LowTemperatureRadiant:SurfaceGroup': 'surface_fractions',
    'OS:ZoneHVAC:Baseboard:RadiantConvective:Water': 'surface_fractions',
    'OS:ZoneHVAC:VentilatedSlab:SlabGroup': 'data',
    'OS:ZoneHVAC:CoolingPanel:RadiantConvective:Water': 'surface_fractions',
    'OS:AirConditioner:VariableRefrigerantFlow:FluidTemperatureControl:HR': 'loading_indices',
    'OS:AirConditioner:VariableRefrigerantFlow:FluidTemperatureControl': 'loading_indices',
    'OS:ZoneTerminalUnitList': 'terminal_units',
    'OS:RoomAir:TemperaturePattern:NondimensionalHeight': 'pairs',
    'OS:RoomAir:Node:AirflowNetwork:InternalGains': 'gains',
    'OS:RoomAirSettings:AirflowNetwork': 'nodes',
    'OS:RoomAir:Node:AirflowNetwork:HVACEquipment': 'equipment_fractions',
    'OS:RoomAir:TemperaturePattern:SurfaceMapping': 'surface_deltas',
    'OS:RoomAir:Node:AirflowNetwork:AdjacentSurfaceList': 'surfaces',
    'OS:Controller:MechanicalVentilation': 'zone_specifications',
    'OS:WaterUse:Connections': 'connections',
    'OS:WaterUse:RainCollector': 'surfaces',
    'OS:Output:Table:Annual': 'variable_details',
    'OS:Meter:CustomDecrement': 'variable_details',
    'OS:Output:Table:Monthly': 'variable_details',
    'OS:Output:Table:SummaryReports': 'reports',
    'OS:Meter:Custom': 'variable_details',
    'OS:UnitarySystemPerformance:Multispeed': 'flow_ratios',
    'OS:SurfaceProperty:ExteriorNaturalVentedCavity': 'surface',
    'OS:ZoneProperty:UserViewFactors:BySurfaceName': 'view_factors',
    'OS:SurfaceProperty:HeatTransferAlgorithm:SurfaceList': 'surface',
    'OS:AirLoopHVAC:ZoneSplitter': 'nodes',
    'OS:AirLoopHVAC:SupplyPath': 'components',
    'OS:AirLoopHVAC:ReturnPath': 'components',
    'OS:AirLoopHVAC:ReturnPlenum': 'nodes',
    'OS:AirLoopHVAC:ZoneMixer': 'nodes',
    'OS:AirLoopHVAC:SupplyPlenum': 'nodes',
    'OS:BuildingSurface:Detailed': 'vertices',
    'OS:Shading:Zone:Detailed': 'vertices',
    'OS:RoofCeiling:Detailed': 'vertices',
    'OS:Shading:Site:Detailed': 'vertices',
    'OS:Wall:Detailed': 'vertices',
    'OS:ZoneList': 'zones',
    'OS:Floor:Detailed': 'vertices',
    'OS:Shading:Building:Detailed': 'vertices',
    'OS:SolarCollector:UnglazedTranspired:Multisystem': 'systems',
    'OS:SolarCollector:UnglazedTranspired': 'surfaces',
    'OS:Parametric:SetValueForRun': 'values',
    'OS:Parametric:Logic': 'lines',
    'OS:Parametric:FileNameSuffix': 'suffixes',
    'OS:Parametric:RunControl': 'runs',
    'OS:ZoneHVAC:EquipmentList': 'equipment',
    'OS:AvailabilityManagerAssignmentList': 'managers',
    'OS:Table:IndependentVariable': 'values',
    'OS:Table:IndependentVariableList': 'independent_variables',
    'OS:Table:Lookup': 'values',
    'OS:Matrix:TwoDimension': 'values',
    'OS:WindowMaterial:GlazingGroup:Thermochromic': 'temperature_data',
    'OS:Schedule:Compact': 'data',
    'OS:Schedule:Day:Interval': 'data',
    'OS:Schedule:Week:Compact': 'data',
    'OS:EnergyManagementSystem:GlobalVariable': 'variables',
    'OS:EnergyManagementSystem:ProgramCallingManager': 'programs',
    'OS:EnergyManagementSystem:Program': 'lines',
    'OS:EnergyManagementSystem:Subroutine': 'lines',
    'OS:Refrigeration:CaseAndWalkInList': 'cases_and_walkins',
    'OS:Refrigeration:CompressorList': 'compressors',
    'OS:ZoneHVAC:RefrigerationChillerSet': 'chillers',
    'OS:Refrigeration:WalkIn': 'zone_data',
    'OS:Refrigeration:TransferLoadList': 'transfer_loads',
    'OS:Branch': 'components',
    'OS:PipingSystem:Underground:Domain': 'pipe_circuits',
    'OS:Connector:Splitter': 'branches',
    'OS:Connector:Mixer': 'branches',
    'OS:BranchList': 'branches',
    'OS:PipingSystem:Underground:PipeCircuit': 'pipe_segments',
    'OS:NodeList': 'nodes',
    'OS:OutdoorAir:NodeList': 'nodes',
    'OS:Fan:SystemModel': 'speed_fractions',
    'OS:AirflowNetwork:Distribution:DuctViewFactors': 'surfaces',
    'OS:GroundHeatExchanger:System': 'vertical_well_locations',
    'OS:GroundHeatExchanger:ResponseFactors': 'g_functions',
    'OS:Foundation:Kiva': 'blocks',
    'OS:SurfaceProperty:ExposedFoundationPerimeter': 'surfaces',
    'OS:SurfaceProperty:SurroundingSurfaces': 'surfaces',
    'OS:ZoneHVAC:HybridUnitaryHVAC': 'modes',
    'OS:ShadowCalculation': 'shading_zone_groups',
    'OS:Schedule:Year': 'schedule_weeks',
    'OS:WindowShadingControl': 'fenestration_surfaces',
    'OS:PlantEquipmentList': 'equipment',
    'OS:CondenserEquipmentList': 'equipment',
    'OS:AirLoopHVAC:Mixer': 'nodes',
    'OS:AirLoopHVAC:Splitter': 'nodes',
    'OS:AirLoopHVAC:DedicatedOutdoorAirSystem': 'airloophvacs',
    'OS:PythonPlugin:Variables': 'global_py_vars',
    'OS:PythonPlugin:SearchPaths': 'py_search_paths',
    'OS:Output:Diagnostics': 'diagnostics',
}
remaining_objects = [
    'OS:Site:SpectrumData',
    'OS:Schedule:Day:List',
    'OS:MaterialProperty:GlazingSpectralData',
]


def get_schema_object(schema, object_key):
    if object_key not in schema['properties']:
        print("{} is not in the SCHEMA".format(object_key))
        return dict()
    if '.*' in schema['properties'][object_key]['patternProperties']:
        return schema['properties'][object_key]['patternProperties']['.*']
    if R'^.*\S.*$' in schema['properties'][object_key]['patternProperties']:
        return schema['properties'][object_key]['patternProperties'][R'^.*\S.*$']
    raise KeyError(R'The patternProperties value is not a valid choice (".*", "^.*\S.*$")')


def change_version(schema):
    schema["epJSON_schema_version"] = "3.1.1"
    schema["epJSON_schema_build"] = "3.1.1"
    loc = get_schema_object(schema, 'OS:Version')['properties']['version_identifier']
    loc['default'] = "3.1.1"
    loc['type'] = "string"


def change_schedule_compact(schema):
    loc = get_schema_object(schema, 'OS:Schedule:Compact')['properties']['extensions']['items']['properties']['field']
    loc.pop('type')
    loc['anyOf'] = anyOf()


def change_special_cased_enums(schema):
    loc = get_schema_object(schema, 'OS:GroundHeatTransfer:Slab:Insulation')
    if loc:
        loc = loc['properties']['ivins_flag_is_there_vertical_insulation']
        loc.pop('type')
        newAnyOf = anyOf()
        newAnyOf[0]['enum'] = [int(i) for i in loc['enum'][:] if isInt(i)]
        newAnyOf[1]['enum'] = ['']
        loc['anyOf'] = newAnyOf
        loc.pop('enum')

    loc = get_schema_object(schema, 'WindowMaterial:Screen')
    if loc:
        loc = loc ['properties']['angle_of_resolution_for_screen_transmittance_output_map']
        loc.pop('type')
        newAnyOf = anyOf()
        newAnyOf[0]['enum'] = [int(i) for i in loc['enum'][:] if isInt(i)]
        newAnyOf[1]['enum'] = ['']
        loc['anyOf'] = newAnyOf
        loc.pop('enum')

    loc = get_schema_object(schema, 'Refrigeration:System')
    if loc:
        loc = loc['properties']['number_of_compressor_stages']
        loc.pop('type')
        newAnyOf = anyOf()
        newAnyOf[0]['enum'] = [int(i) for i in loc['enum'][:] if isInt(i)]
        newAnyOf[1]['enum'] = ['']
        loc['anyOf'] = newAnyOf
        loc.pop('enum')

    loc = get_schema_object(schema, 'ElectricLoadCenter:Transformer')
    if loc:
        loc = loc['properties']['phase']
        loc.pop('type')
        newAnyOf = anyOf()
        newAnyOf[0]['enum'] = [int(i) for i in loc['enum'][:] if isInt(i)]
        newAnyOf[1]['enum'] = ['']
        loc['anyOf'] = newAnyOf
        loc.pop('enum')

    loc = get_schema_object(schema, 'OS:ThermalZone')
    if loc:
        loc = loc['properties']['zone_outside_convection_algorithm']['enum']
        loc.insert(0, '')

    loc = get_schema_object(schema, 'OS:ThermalZone')
    if loc:
        loc = loc['properties']['zone_inside_convection_algorithm']['enum']
        loc.insert(0, '')


def change_utility_cost(schema):
    legacy_idd = schema['properties']['OS:UtilityCost:Charge:Block']['legacy_idd']['fields']
    loc = get_schema_object(schema, 'OS:UtilityCost:Charge:Block')['properties']
    for i in range(6, len(legacy_idd)):
        field = legacy_idd[i]
        loc[field].pop('type')
        loc[field]['anyOf'] = anyOf()

    loc = get_schema_object(schema, 'OS:UtilityCost:Ratchet')['properties']
    loc['offset_value_or_variable_name'].pop('type')
    loc['offset_value_or_variable_name']['anyOf'] = anyOf()
    loc['multiplier_value_or_variable_name'].pop('type')
    loc['multiplier_value_or_variable_name']['anyOf'] = anyOf()

    loc = get_schema_object(schema, 'OS:UtilityCost:Charge:Simple')['properties']
    loc['cost_per_unit_value_or_variable_name'].pop('type')
    loc['cost_per_unit_value_or_variable_name']['anyOf'] = anyOf()

    loc = get_schema_object(schema, 'OS:UtilityCost:Qualify')['properties']
    loc['threshold_value_or_variable_name'].pop('type')
    loc['threshold_value_or_variable_name']['anyOf'] = anyOf()

    loc = get_schema_object(schema, 'OS:UtilityCost:Tariff')['properties']
    loc['minimum_monthly_charge_or_variable_name'].pop('type')
    loc['minimum_monthly_charge_or_variable_name']['anyOf'] = anyOf()
    loc['monthly_charge_or_variable_name'].pop('type')
    loc['monthly_charge_or_variable_name']['anyOf'] = anyOf()


def add_explicit_extensible_bounds(schema):

    # EnergyManagementSystem:Program
    loc = get_schema_object(schema, 'OS:EnergyManagementSystem:Program')
    if loc:
        if 'required' in loc and 'lines' not in loc['required']:
            loc['required'].append('lines')
        if 'required' not in loc:
            loc['required'] = ['lines']
        if 'minItems' not in loc['properties']['lines']:
            loc['properties']['lines']['minItems'] = 1


def change_special_cased_name_fields(schema):
    #original_name = schema['properties']['OS:ZoneHVAC:TerminalUnit:VariableRefrigerantFlow']['legacy_idd']['field_info'].pop('zone_terminal_unit_name')
    #schema['properties']['OS:ZoneHVAC:TerminalUnit:VariableRefrigerantFlow']['legacy_idd']['field_info']['name'] = original_name
    schema['properties']['OS:ZoneHVAC:TerminalUnit:VariableRefrigerantFlow']['legacy_idd']['fields'][0] = 'name'
    schema['properties']['OS:ZoneHVAC:TerminalUnit:VariableRefrigerantFlow']['legacy_idd']['alphas']['fields'][0] = 'name'
    del get_schema_object(schema, 'OS:ZoneHVAC:TerminalUnit:VariableRefrigerantFlow')['required'][0]
    #schema['properties']['OS:ZoneHVAC:TerminalUnit:VariableRefrigerantFlow']['name'] = \
    #    get_schema_object(schema, 'OS:ZoneHVAC:TerminalUnit:VariableRefrigerantFlow')['properties'].pop('zone_terminal_unit_name')

    # original_name = schema['properties']['OS:AirConditioner:VariableRefrigerantFlow']['legacy_idd']['field_info'].pop('heat_pump_name')
    # schema['properties']['OS:AirConditioner:VariableRefrigerantFlow']['legacy_idd']['field_info']['name'] = original_name
    # schema['properties']['OS:AirConditioner:VariableRefrigerantFlow']['legacy_idd']['fields'][0] = 'name'
    # schema['properties']['OS:AirConditioner:VariableRefrigerantFlow']['legacy_idd']['alphas']['fields'][0] = 'name'
    # del get_schema_object(schema, 'OS:AirConditioner:VariableRefrigerantFlow')['required'][0]
    # schema['properties']['OS:AirConditioner:VariableRefrigerantFlow']['name'] = \
        # get_schema_object(schema, 'OS:AirConditioner:VariableRefrigerantFlow')['properties'].pop('heat_pump_name')


def change_extensions_name(schema):
    for key, value in extension_renaming.items():
        obj = get_schema_object(schema, key)
        if obj:
            if 'extensions' not in obj['properties']:
                print(f"extensions not found for {key}")
                continue
            obj['properties'][value] = obj['properties']['extensions']
            loc = obj['properties']
            del loc['extensions']
            schema['properties'][key]['legacy_idd']['extension'] = value

    for key in remaining_objects:
        if key not in schema['properties']:
            print(f"{key} not found in schema['properties'] for remaining_objects")
            continue
        schema['properties'][key]['legacy_idd']['extension'] = 'extensions'


def change_89_release_issues(schema):
    curves = [
        'OS:Curve:Linear', 'OS:Curve:Quadratic', 'OS:Curve:Cubic', 'OS:Curve:Quartic', 'OS:Curve:Exponent',
        'OS:Curve:Bicubic', 'OS:Curve:Biquadratic', 'OS:Curve:QuadraticLinear', 'OS:Curve:CubicLinear', 'OS:Curve:Triquadratic',
        'OS:Curve:ExponentialSkewNormal', 'OS:Curve:Sigmoid', 'OS:Curve:RectangularHyperbola1', 'OS:Curve:RectangularHyperbola2', 'OS:Curve:ExponentialDecay',
        'OS:Curve:DoubleExponentialDecay', 'OS:Curve:ChillerPartLoadWithLift', 'OS:Table:Lookup'
    ]
    for curve in curves:
        loc = get_schema_object(schema, curve)
        if loc:
            loc['properties']['output_unit_type']['enum'] = [
                '',
                'Capacity',
                'Dimensionless',
                'Power',
                'Pressure',
                'Temperature'
            ]

    loc = get_schema_object(schema, 'OS:Schedule:Week:Compact')
    if loc:
        loc['properties']['data']['items']['properties']['daytype_list'].pop('enum')


