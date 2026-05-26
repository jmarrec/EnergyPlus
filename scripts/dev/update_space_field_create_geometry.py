import geomeffibem
import numpy as np
import openstudio

openstudio.Logger.instance().standardOutLogger().setLogLevel(openstudio.Debug)


def create_skylight(space) -> openstudio.model.SubSurface:
    roof = next(s for s in space.surfaces() if s.surfaceType().lower() == "roofceiling")

    vertices = roof.vertices()
    g = openstudio.getCentroid(vertices).get()
    scale_factor = 0.4**0.5  # sqrt(40%)

    new_vertices = []
    for vertex in vertices:
        # Point3d - Point3d = Vector3d
        # Vector from centroid to vertex (GA, GB, GC, etc)
        centroid_vector = vertex - g

        # Resize the vector (done in place) according to scale_factor
        centroid_vector.setLength(centroid_vector.length() * scale_factor)

        # Move the vertex toward the centroid
        vertex = g + centroid_vector

        new_vertices.append(vertex)
    ss = openstudio.model.SubSurface(new_vertices, m)
    ss.setName("Window Skylight")
    ss.setSurface(roof)

    return ss


def offset_sf(sf, x_offset_both: float, z_offset_top: float) -> geomeffibem.Surface:
    x_idx = 0
    z_idx = 2

    v_np = sf.to_numpy()

    min_x = v_np[:, x_idx].min()
    max_x = v_np[:, x_idx].max()

    v_np[v_np[:, x_idx] == min_x, x_idx] = min_x + x_offset_both
    v_np[v_np[:, x_idx] == max_x, x_idx] = max_x - x_offset_both

    # z
    max_z = v_np[:, z_idx].max()
    v_np[v_np[:, z_idx] == max_z, z_idx] = max_z - z_offset_top

    v_np = np.roll(v_np, shift=-1, axis=0)

    new_sf = geomeffibem.Surface.from_numpy_array(v_np)
    new_sf.name = f"{sf.name} Offset"
    return new_sf


def center_door(sf, width=0.8, height=2.0) -> geomeffibem.Surface:
    """Create a door (default 80cm x 200cm) centered"""
    x_idx = 0
    z_idx = 2

    v_np = sf.to_numpy()

    min_x = v_np[:, x_idx].min()
    max_x = v_np[:, x_idx].max()
    center_x = min_x + (max_x - min_x) / 2

    v_np[v_np[:, x_idx] == min_x, x_idx] = center_x - width / 2
    v_np[v_np[:, x_idx] == max_x, x_idx] = center_x + width / 2

    # z
    max_z = v_np[:, z_idx].max()
    v_np[v_np[:, z_idx] == max_z, z_idx] = height

    v_np = np.roll(v_np, shift=-1, axis=0)

    new_sf = geomeffibem.Surface.from_numpy_array(v_np)
    new_sf.name = f"{sf.name} Offset"
    return new_sf


def create_3_doors(w, do_plot=False) -> list[geomeffibem.Surface]:
    wall = geomeffibem.Surface.from_Surface(w)
    assert wall.get_plot_axis() == "xz"
    xsfs = wall.split_into_n_segments(n_segments=3, axis="x")

    new_doors = []

    overhead_door = offset_sf(sf=xsfs[0], x_offset_both=0.5, z_offset_top=0.5)
    overhead_door.name = "OverheadDoor"
    new_doors.append(overhead_door)

    door = center_door(sf=xsfs[1], width=1.0, height=2.0)
    door.name = "Door"
    new_doors.append(door)

    glass_door = center_door(sf=xsfs[2], width=1.0, height=2.0)
    glass_door.name = "GlassDoor"
    new_doors.append(glass_door)

    # If the door azimuth isn't the same as the well, reverse the vertices
    for door in new_doors:
        assert (
            door.azimuth() == wall.azimuth()
        ), f"Door azimuth {door.azimuth()} doesn't match wall azimuth {wall.azimuth()}"

    if do_plot:
        ax = wall.plot()

        for new_sf in xsfs:
            new_sf.plot(ax=ax, annotate=False)  # , with_os_centroid=True)

        for new_sf in new_doors:
            new_sf.plot(ax=ax, annotate=False)  # , with_os_centroid=True)
    return new_doors


m = openstudio.model.Model()

c = openstudio.model.Construction(m)
c.setName("R13 Construction")
m15_200mm_heavyweight_concrete = openstudio.model.StandardOpaqueMaterial(m)
m15_200mm_heavyweight_concrete.setName("M15 200mm heavyweight concrete")
m15_200mm_heavyweight_concrete.setRoughness("MediumRough")
m15_200mm_heavyweight_concrete.setThickness(0.2032)
m15_200mm_heavyweight_concrete.setThermalConductivity(1.95)
m15_200mm_heavyweight_concrete.setDensity(2240.0)
m15_200mm_heavyweight_concrete.setSpecificHeat(900.0)
insulation_mat = openstudio.model.MasslessOpaqueMaterial(m)
insulation_mat.setName("R13-IP")
insulation_mat.setThermalResistance(openstudio.convert(13, "ft^2*h*R/Btu", "m^2*K/W").get())
assert c.setLayers([m15_200mm_heavyweight_concrete, insulation_mat])
assert len(c.layers()) == 2
print(c)

simple_glazing = openstudio.model.SimpleGlazing(m)
simple_glazing.setName("Simple Glazing Mat")
window_cons = openstudio.model.Construction(m)
window_cons.setName("Simple Glazing")
window_cons.setLayers([simple_glazing])

z = openstudio.model.ThermalZone(m)
z.setName("Zone1")


floor_sf = geomeffibem.Surface.Floor(min_x=0.0, max_x=15.0, min_y=0.0, max_y=10.0, z=0.0)
floor_sf.to_Point3dVector()
space = openstudio.model.Space.fromFloorPrint(floor_sf.to_Point3dVector(), 3.0, m, "Space1").get()
space.setThermalZone(z)

[s.setConstruction(c) for s in m.getSurfaces()]

ss = create_skylight(space)
ss.setConstruction(window_cons)


for sf in space.surfaces():
    if sf.surfaceType().lower() != "wall":
        continue
    group = sf.planarSurfaceGroup().get()
    site_transformation = group.siteTransformation()
    site_vertices = site_transformation * sf.vertices()
    site_outward_normal = openstudio.getOutwardNormal(site_vertices).get()
    north = openstudio.Vector3d(0.0, 1.0, 0.0)
    if site_outward_normal.x() < 0.0:
        azimuth = 360.0 - openstudio.radToDeg(openstudio.getAngle(site_outward_normal, north))
    else:
        azimuth = openstudio.radToDeg(openstudio.getAngle(site_outward_normal, north))

    if azimuth >= 315.0 or azimuth < 45.0:
        facade = "4-North"
    elif azimuth >= 45.0 and azimuth < 135.0:
        facade = "3-East"
    elif azimuth >= 135.0 and azimuth < 225.0:
        facade = "1-South"
    elif azimuth >= 225.0 and azimuth < 315.0:
        facade = "2-West"

    sf.setName(f"{facade}".upper())  # - Abs azimuth {azimuth:.2f}".upper())

walls = sorted([s for s in space.surfaces() if s.surfaceType().lower() == "wall"], key=lambda s: s.nameString())
assert len(walls) == 4

for w, ss_type in zip(walls[1:], ["FixedWindow", "OperableWindow", "Window"]):
    ss = w.setWindowToWallRatio(0.4).get()
    ss.setSubSurfaceType(ss_type)
    ss.setName(f"Window {ss_type}")
    ss.setConstruction(window_cons)

wall_with_door = walls[0]
new_doors = create_3_doors(w=wall_with_door, do_plot=False)

for door in new_doors:
    ss = openstudio.model.SubSurface(door.to_Point3dVector(), m)
    ss.setName(f"Door {door.name}")
    ss.setSurface(wall_with_door)
    ss.setSubSurfaceType(door.name)
    if door.name == "GlassDoor":
        ss.setConstruction(window_cons)
    else:
        ss.setConstruction(c)

m.save("update_space_field_create_geometry.osm", True)

ft = openstudio.energyplus.ForwardTranslator()
ft.setExcludeHTMLOutputReport(True)
ft.setExcludeLCCObjects(True)
ft.setExcludeSpaceTranslation(True)
ft.setExcludeVariableDictionary(True)
ft.setExcludeSQliteOutputReport(True)
w = ft.translateModel(m)

# Cleanup pass

# {x.iddObject().name() for x in w.objects()}
obj_types_to_keep = {
    #'Building',
    "BuildingSurface:Detailed",
    "Construction",
    "FenestrationSurface:Detailed",
    # "GlobalGeometryRules",
    "Material",
    "Material:NoMass",
    #'OutdoorAir:Node',
    #'RunPeriod',
    #'Schedule:Constant',
    #'ScheduleTypeLimits',
    #'SimulationControl',
    #'Sizing:Parameters',
    #'Timestep',
    "WindowMaterial:SimpleGlazingSystem",
    "Zone",
}

for o in w.objects():
    if not o.iddObject().name() in obj_types_to_keep:
        o.remove()

n_doors = 0
n_windows = 0
for o in w.getObjectsByType("FenestrationSurface:Detailed"):
    ori_ssType = o.getString(1).get()

    ssType = None

    if ori_ssType == "Door":
        n_doors += 1

        remap = {"Door", "OverheadDoor"}
        for r in remap:
            if o.nameString() == f"Door {r}":
                ssType = r
                break

    elif ori_ssType == "Window":
        n_windows += 1

        remap = {"FixedWindow", "OperableWindow", "Window", "Skylight"}
        for r in remap:
            if o.nameString() == f"Window {r}":
                ssType = r
                break
    else:
        ssType = ori_ssType

    assert ssType is not None, o.nameString()

    # print(f"{o.nameString()}, {ori_ssType} -> {ssType}")
    o.setString(1, ssType)

assert n_doors == 2
assert n_windows == 4

w.save("update_space_field_create_geometry.idf", True)
