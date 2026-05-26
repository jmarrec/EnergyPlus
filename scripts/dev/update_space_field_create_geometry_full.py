import json
from pathlib import Path

import geomeffibem
import openstudio
import pandas as pd

NEW_IDD_PATH = Path("/home/julien/Software/Others/EnergyPlus-build-release/Products/Energy+.idd")


def create_opaque_construction(m):
    """Create an opaque construction with a 200mm concrete layer and R13 insulation."""
    c = openstudio.model.Construction(m)
    c.setName("Opaque Construction")
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
    return c


def create_window_construction(m):
    """Create a window construction with simple glazing."""
    simple_glazing = openstudio.model.SimpleGlazing(m)
    simple_glazing.setName("Simple Glazing Mat")
    simple_glazing.setSolarHeatGainCoefficient(0.65)
    window_cons = openstudio.model.Construction(m)
    window_cons.setName("Simple Glazing")
    window_cons.setLayers([simple_glazing])

    return window_cons


def create_space(
    name: str,
    m: openstudio.model.Model,
    xOffset: int = 0,
    zOffset: int = 0,
    floor_width: float = 10.0,
    floor_depth: float = 10.0,
    floor_height: float = 3.0,
):
    """Create a space."""
    floor_sf = geomeffibem.Surface.Floor(min_x=0.0, max_x=floor_width, min_y=0.0, max_y=floor_depth, z=0.0)

    space = openstudio.model.Space.fromFloorPrint(floor_sf.to_Point3dVector(), floor_height, m, name).get()
    space.setXOrigin(floor_width * xOffset)
    space.setZOrigin(floor_height * zOffset)

    z = openstudio.model.ThermalZone(m)
    z.setName(name.replace("Space", "Zone"))
    space.setThermalZone(z)
    return space


def add_interior_partition(space2):
    """Add a desk to space 2."""
    deskGroup = openstudio.model.InteriorPartitionSurfaceGroup(space2.model())
    deskGroup.setSpace(space2)

    deskPoints = [
        openstudio.Point3d(5, 8, 1),
        openstudio.Point3d(5, 6, 1),
        openstudio.Point3d(8, 6, 1),
        openstudio.Point3d(8, 8, 1),
    ]
    desk = openstudio.model.InteriorPartitionSurface(deskPoints, space2.model())
    desk.setInteriorPartitionSurfaceGroup(deskGroup)
    return desk


def add_skylight(space) -> openstudio.model.SubSurface:
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
    ss.setName("ExteriorWindow - Skylight")
    ss.setSurface(roof)

    return ss


def add_door(wall, subsurface_type: str, width=0.8, height=2.0) -> geomeffibem.Surface:
    """Create a door (default 80cm x 200cm) centered."""
    surfaceVertices = wall.vertices()

    # new coordinate system has z' in direction of outward normal, y' is up
    transformation = openstudio.Transformation.alignFace(surfaceVertices)
    faceVertices = transformation.inverse() * surfaceVertices

    xs = [pt.x() for pt in faceVertices]
    min_x = min(xs)
    max_x = max(xs)

    ys = [pt.y() for pt in faceVertices]
    min_y = min(ys)
    # max_y = max(ys)

    center_x = min_x + (max_x - min_x) / 2

    viewMinX = center_x - width / 2.0
    viewMaxX = center_x + width / 2.0

    viewVertices = [
        openstudio.Point3d(viewMinX, min_y + height, 0),
        openstudio.Point3d(viewMinX, min_y, 0),
        openstudio.Point3d(viewMaxX, min_y, 0),
        openstudio.Point3d(viewMaxX, min_y + height, 0),
    ]
    # Go back to input coordinate system
    viewVertices = transformation * viewVertices
    ss = openstudio.model.SubSurface(viewVertices, wall.model())
    win_type = "Exterior" if wall.outsideBoundaryCondition().lower() == "outdoors" else "Interior"
    ss.setName(f"{win_type}Door - {subsurface_type}")
    ss.setSurface(wall)
    ss.setSubSurfaceType(subsurface_type)
    return ss


def add_window(wall, subsurface_type: str):
    """Create a window on the wall."""
    ss = wall.setWindowToWallRatio(0.4).get()
    win_type = "Exterior" if wall.outsideBoundaryCondition().lower() == "outdoors" else "Interior"
    ss.setName(f"{win_type}Window - {subsurface_type}")
    ss.setSubSurfaceType(subsurface_type)
    return ss


def get_construction_name(surface_boundary_type: str, surface_type: str):
    return f"{surface_boundary_type} {surface_type} Construction"


def get_constructions_and_materials():
    idd = openstudio.IddFile.load(NEW_IDD_PATH).get()

    o_ = idd.getObject("DefaultConstructionSet")
    assert o_.is_initialized()
    idd_default_construction_set = o_.get()

    o_ = idd.getObject("DefaultSurfaceConstructions")
    assert o_.is_initialized()
    idd_default_surfaces_construction = o_.get()

    o_ = idd.getObject("DefaultSubSurfaceConstructions")
    assert o_.is_initialized()
    idd_default_subsurfaces_construction = o_.get()

    objects = []
    # name: is_glazed
    construction_infos = {}
    mat_glazing_name = "Mat Glazing"
    mat_opaque_name = "Mat Opaque"

    dc = openstudio.IdfObject(idd_default_construction_set)
    dc.setName("Default Construction Set")

    objects.append(dc)

    # Surface Constructions
    surface_types = ["Floor", "Wall", "Roof"]

    surface_boundary_types = ["Exterior", "Interior", "Ground"]

    for k, surface_boundary_type in enumerate(surface_boundary_types):

        sc = openstudio.IdfObject(idd_default_surfaces_construction)
        name = f"{surface_boundary_type} Surface Constructions"
        sc.setName(name)
        dc.setString(k + 1, name)
        for i, surface_type in enumerate(surface_types):
            cons_name = get_construction_name(surface_boundary_type=surface_boundary_type, surface_type=surface_type)
            construction_infos[cons_name] = False
            sc.setString(i + 1, cons_name)

        objects.append(sc)

    # SubSurface Constructions
    subsurface_types = [
        "FixedWindow",
        "OperableWindow",
        "Door",
        "GlassDoor",
        "OverheadDoor",
        "Skylight",
        "TubularDaylightDome",
        "TubularDaylightDiffuser",
    ]

    subsurface_boundary_types = ["Exterior", "Interior"]

    for k, subsurface_boundary_type in enumerate(subsurface_boundary_types):

        sc = openstudio.IdfObject(idd_default_subsurfaces_construction)
        name = f"{subsurface_boundary_type} SubSurface Constructions"
        sc.setName(name)
        dc.setString(k + 4, name)
        for i, subsurface_type in enumerate(subsurface_types):
            cons_name = get_construction_name(
                surface_boundary_type=subsurface_boundary_type, surface_type=subsurface_type
            )
            construction_infos[cons_name] = subsurface_type not in ["Door", "OverheadDoor"]
            sc.setString(i + 1, cons_name)

        objects.append(sc)

    # Interior Partition
    name = "Interior Partition Construction"
    dc.setString(6, name)
    construction_infos[name] = False

    # Adiabatic Partition
    name = "Adiabatic Surface Construction"
    dc.setString(7, name)
    construction_infos[name] = False

    for construction_name, is_glazed in construction_infos.items():
        construction = openstudio.IdfObject(idd.getObject("Construction").get())
        construction.setName(construction_name)
        if is_glazed:
            construction.setString(1, mat_glazing_name)
        else:
            construction.setString(1, mat_opaque_name)
        objects.append(construction)

    mat_opaque = openstudio.IdfObject(idd.getObject("Material:NoMass").get())
    mat_opaque.setName(mat_opaque_name)
    mat_opaque.setString(1, "Rough")
    mat_opaque.setDouble(2, 0.5)
    mat_opaque.setDouble(3, 0.9)
    mat_opaque.setDouble(4, 0.9)
    mat_opaque.setDouble(5, 0.9)
    objects.append(mat_opaque)

    mat_glazing = openstudio.IdfObject(idd.getObject("WindowMaterial:SimpleGlazingSystem").get())
    mat_glazing.setName(mat_glazing_name)
    mat_glazing.setDouble(1, 0.1)
    mat_glazing.setDouble(2, 0.65)
    objects.append(mat_glazing)

    return objects


def format_unit_test_s(surface_infos):
    df = pd.DataFrame(surface_infos)

    df.set_index(["surface_boundary_type", "surface_type"], inplace=True)
    df.sort_index(axis=0, inplace=True)
    surface_types = ["Floor", "Wall", "Roof"]
    surface_boundary_types = ["Exterior", "Interior", "Ground", "Adiabatic"]

    for surface_boundary_type in surface_boundary_types:
        print(f"    // {surface_boundary_type} Surfaces")
        df2 = df.loc[surface_boundary_type]
        for surface_type in [s for s in surface_types if s in df2.index]:
            print(f"    /// {surface_boundary_type} {surface_type}")
            df3 = df2.loc[[surface_type]]
            for _, s in df3.iterrows():
                sf_name = s["name"]
                cons_name = s["construction"]
                print(f'    checkDefaultConstruction("{cons_name}", "{sf_name}");')
            print("")
        print("\n")


def format_unit_test_ss(subsurface_infos):
    df = pd.DataFrame(subsurface_infos)

    df.set_index(["surface_boundary_type", "surface_type"], inplace=True)
    df.sort_index(axis=0, inplace=True)
    subsurface_types = [
        "Window",
        "FixedWindow",
        "OperableWindow",
        "Door",
        "GlassDoor",
        "OverheadDoor",
        "Skylight",
        "TubularDaylightDome",
        "TubularDaylightDiffuser",
    ]

    subsurface_boundary_types = ["Exterior", "Interior"]

    for subsurface_boundary_type in subsurface_boundary_types:
        print(f"    // {subsurface_boundary_type} SubSurfaces")
        df2 = df.loc[subsurface_boundary_type]
        for subsurface_type in [s for s in subsurface_types if s in df2.index]:
            print(f"    /// {subsurface_boundary_type} {subsurface_type}")
            df3 = df2.loc[[subsurface_type]]
            for _, s in df3.iterrows():
                sf_name = s["name"]
                cons_name = s["construction"]
                print(f'    checkDefaultConstruction("{cons_name}", "{sf_name}");')
            print("")
        print("\n")


if __name__ == "__main__":
    m = openstudio.model.Model()
    c_opaque = create_opaque_construction(m)
    c_window = create_window_construction(m)
    space1 = create_space(name="Space1", m=m, xOffset=0, zOffset=0)
    space2 = create_space(name="Space2", m=m, xOffset=1, zOffset=0)
    space3 = create_space(name="Space3", m=m, xOffset=0, zOffset=1)
    space4 = create_space(name="Space4", m=m, xOffset=1, zOffset=1)

    [s.setConstruction(c_opaque) for s in m.getSurfaces()]

    openstudio.model.matchSurfaces(openstudio.model.SpaceVector([space1, space2, space3, space4]))

    desk = add_interior_partition(space2=space2)

    space1_floor = next(s for s in space1.surfaces() if s.surfaceType().lower() == "floor")
    space1_floor.setOutsideBoundaryCondition("Outdoors")

    space2_ext_walls = [
        s
        for s in space2.surfaces()
        if s.surfaceType().lower() == "wall" and s.outsideBoundaryCondition().lower() == "outdoors"
    ]
    assert len(space2_ext_walls) == 3, f"Expected 3 exterior walls for space 2, found {len(space2_ext_walls)}"

    space2_ext_walls[0].setOutsideBoundaryCondition("Adiabatic")
    space2_ext_walls[1].setOutsideBoundaryCondition("Ground")

    ext_walls = sorted(
        [
            s
            for s in m.getSurfaces()
            if s.surfaceType().lower() == "wall" and s.outsideBoundaryCondition().lower() == "outdoors"
        ],
        key=lambda s: s.nameString(),
    )
    assert len(ext_walls) == 10  # 12 total walls - 2 I reassigned = 10 exterior walls
    ss = add_door(ext_walls[0], "Door")
    ss.setConstruction(c_opaque)
    ss = add_door(ext_walls[1], "OverheadDoor", width=5.0, height=2.5)
    ss.setConstruction(c_opaque)
    ss = add_door(ext_walls[2], "GlassDoor")
    ss.setConstruction(c_window)

    ss = add_skylight(space4)
    ss.setConstruction(c_window)

    for w, ss_type in zip(ext_walls[3:], ["FixedWindow", "OperableWindow", "Window"]):
        ss = add_window(w, subsurface_type=ss_type)
        ss.setConstruction(c_window)

    int_walls = sorted(
        [
            s
            for s in m.getSurfaces()
            if s.surfaceType().lower() == "wall" and s.outsideBoundaryCondition().lower() == "surface"
        ],
        key=lambda s: s.nameString(),
    )
    assert len(int_walls) == 4, f"Expected 2 interior walls (+ 2 reversed), found {len(int_walls)}"
    int_wall1 = int_walls[0]
    ss = add_door(int_wall1, "Door")
    ss.setConstruction(c_opaque)

    adjacent_sub_surface = openstudio.model.SubSurface(list(reversed(ss.vertices())), m)
    adjacent_sub_surface.setName(f"{ss.nameString()} - Reversed")
    adjacent_sub_surface.setSurface(int_wall1.adjacentSurface().get())
    adjacent_sub_surface.setSubSurfaceType("Door")
    ss.setAdjacentSubSurface(adjacent_sub_surface)

    int_wall2 = next(w for w in int_walls[1:] if w.adjacentSurface().get() != int_wall1)
    ss = add_window(int_wall2, subsurface_type="FixedWindow")
    ss.setConstruction(c_window)

    adjacent_sub_surface = openstudio.model.SubSurface(list(reversed(ss.vertices())), m)
    adjacent_sub_surface.setName(f"{ss.nameString()} - Reversed")
    adjacent_sub_surface.setSurface(int_wall2.adjacentSurface().get())
    adjacent_sub_surface.setSubSurfaceType("FixedWindow")
    ss.setAdjacentSubSurface(adjacent_sub_surface)

    spaceVector = openstudio.model.SpaceVector([space1, space2, space3, space4])
    openstudio.model.intersectSurfaces(spaceVector)
    openstudio.model.matchSurfaces(spaceVector)

    m.save("test.osm", True)

    ft = openstudio.energyplus.ForwardTranslator()
    ft.setExcludeHTMLOutputReport(True)
    ft.setExcludeLCCObjects(True)
    ft.setExcludeSpaceTranslation(True)
    ft.setExcludeVariableDictionary(True)
    ft.setExcludeSQliteOutputReport(True)
    w = ft.translateModel(m)

    idf_path = Path("test.idf")
    w.save(idf_path, True)

    # Cleanup pass

    print({x.iddObject().name() for x in w.objects()})
    obj_types_to_keep = {
        #'Building',
        "BuildingSurface:Detailed",
        "FenestrationSurface:Detailed",
        # "GlobalGeometryRules",
        # I'm going to swap with mine
        # "Construction",
        # "Material",
        # "Material:NoMass",
        # "WindowMaterial:SimpleGlazingSystem",
        #'OutdoorAir:Node',
        #'RunPeriod',
        #'Schedule:Constant',
        #'ScheduleTypeLimits',
        #'SimulationControl',
        #'Sizing:Parameters',
        #'Timestep',
        "Zone",
    }

    for o in w.objects():
        if o.iddObject().name() not in obj_types_to_keep:
            o.remove()

    n_doors = 0
    n_windows = 0
    for o in w.getObjectsByType("FenestrationSurface:Detailed"):
        print(o.nameString())
        ori_ssType = o.getString(1).get()

        ssType = None

        if ori_ssType == "GlassDoor":
            n_doors += 1
        if ori_ssType == "Door":
            n_doors += 1

            remap = ["OverheadDoor", "Door"]
            for r in remap:
                print(o.nameString())
                if f"Door - {r}" in o.nameString():
                    ssType = r
                    break

        elif ori_ssType == "Window":
            n_windows += 1

            remap = {"FixedWindow", "OperableWindow", "Window", "Skylight"}
            for r in remap:
                if f"Window - {r}" in o.nameString():
                    ssType = r
                    break
        else:
            ssType = ori_ssType

        assert ssType is not None, f"{o.nameString()}:\n{str(o)}"

        print(f"{o.nameString()}, {ori_ssType} -> {ssType}")
        assert o.setString(1, ssType)

    # We have one reversed for the interior door and interior window, so +1 for each
    assert n_doors == 5, f"Expected 5 doors, found {n_doors}"
    assert n_windows == 6, f"Expected 6 windows, found {n_windows}"

    # Serialize, and reload with a different IDD
    idf_path = Path("test.idf")
    w.save(idf_path, True)
    idd = openstudio.IddFile.load(NEW_IDD_PATH).get()
    w = openstudio.Workspace.load(idf_path, idd).get()

    n_ori = len(w.objects())
    objects = get_constructions_and_materials()
    w.addObjects(objects)
    assert len(w.objects()) == n_ori + len(
        objects
    ), f"Expected {n_ori + len(objects)} objects, found {len(w.objects())}"

    # Clear the constructions for the BuildingSurface:Detailed and FenestrationSurface:Detailed objects since we don't
    # have the materials in the IDF
    idd_surface_ = idd.getObject("BuildingSurface:Detailed")
    assert idd_surface_.is_initialized()
    idd_surface = idd_surface_.get()

    idd_subsurface_ = idd.getObject("FenestrationSurface:Detailed")
    assert idd_subsurface_.is_initialized()
    idd_subsurface = idd_subsurface_.get()

    surface_infos = []

    wo_surfaces = w.getObjectsByType(idd_surface)
    assert len(wo_surfaces) == 24, f"Expected 24 surfaces, found {len(wo_surfaces)}"
    for o in wo_surfaces:
        # o.setString(2, "")
        surface_type = o.getString(1).get()
        surface_type = "Roof" if surface_type == "Ceiling" else surface_type
        surface_boundary_type = o.getString(5).get()
        surface_boundary_type = {
            "Outdoors": "Exterior",
            "Surface": "Interior",
        }.get(surface_boundary_type, surface_boundary_type)
        if surface_boundary_type == "Adiabatic":
            cons_name = "Adiabatic Surface Construction"
        else:
            cons_name = get_construction_name(surface_boundary_type=surface_boundary_type, surface_type=surface_type)
        assert o.setString(2, cons_name), (
            f"Failed to set construction '{cons_name}' for surface '{o.nameString()}', "
            f"{surface_type=}, {surface_boundary_type=}"
        )
        surface_infos.append(
            {
                "name": o.nameString(),
                "construction": cons_name,
                "surface_type": surface_type,
                "surface_boundary_type": surface_boundary_type,
            }
        )

    subsurface_infos = []

    wo_subsurfaces = w.getObjectsByType(idd_subsurface)
    assert len(wo_subsurfaces) == 11, f"Expected 11 SubSurfaces, found {len(wo_subsurfaces)}"
    for o in wo_subsurfaces:
        # o.setString(2, "")
        ori_surface_type = o.getString(1).get()
        surface_type = "FixedWindow" if ori_surface_type == "Window" else ori_surface_type
        surface_name = o.getString(3).get()
        surface = next(s for s in wo_surfaces if s.nameString() == surface_name)
        surface_boundary_type = surface.getString(5).get()
        surface_boundary_type = {
            "Outdoors": "Exterior",
            "Surface": "Interior",
        }.get(surface_boundary_type, surface_boundary_type)
        cons_name = get_construction_name(surface_boundary_type=surface_boundary_type, surface_type=surface_type)
        assert o.setString(2, cons_name), (
            f"Failed to set construction '{cons_name}' for subsurface '{o.nameString()}', "
            f"{surface_type=}, {surface_boundary_type=}"
        )
        subsurface_infos.append(
            {
                "name": o.nameString(),
                "construction": cons_name,
                "surface_type": ori_surface_type,
                "surface_boundary_type": surface_boundary_type,
            }
        )

    # print(json.dumps(subsurface_infos, indent=2))

    w.save("test.idf", True)

    objects = get_constructions_and_materials()
    Path("constructions_and_materials.idf").write_text("".join(str(o) for o in objects))

    print("\n\n========   UNIT TEST =========\n")

    format_unit_test_s(surface_infos=surface_infos)
    format_unit_test_ss(subsurface_infos=subsurface_infos)
