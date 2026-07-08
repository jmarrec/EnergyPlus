# Output Changes

This file documents the structural changes on the output of EnergyPlus that could affect interfaces, etc.

### Description

This will eventually become a more structured file, but currently it isn't clear what format is best. As an intermediate solution, and to allow the form to be formed organically, this plain text file is being used. Entries should be clearly delimited. It isn't expected that there will be but maybe a couple each release at most. Entries should also include some reference back to the repo. At least a PR number or whatever.



Report variable name change (added the word "Coil") for duplicate Refrigeration:AirChiller reports via PR 11677:



Refrigeration Zone Air Chiller Coil Total Cooling Rate

Refrigeration Zone Air Chiller Coil Total Cooling Energy

Refrigeration Zone Air Chiller Coil Sensible Cooling Rate

Refrigeration Zone Air Chiller Coil Sensible Cooling Energy

Refrigeration Zone Air Chiller Coil Latent Cooling Rate

Refrigeration Zone Air Chiller Coil Latent Cooling Energy

Refrigeration Zone Air Chiller Coil Water Removed Mass Flow Rate



Report name change via PR 11677 (added the word "Coil"):



Refrigeration Zone Air Chiller Coil Sensible Heat Ratio

Refrigeration Zone Air Chiller Coil Frost Accumulation Mass

Refrigeration Zone Air Chiller Coil Defrost Electricity Rate

Refrigeration Zone Air Chiller Coil Defrost Electricity Energy



