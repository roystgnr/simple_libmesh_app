#include "libmesh/libmesh.h"
#include "libmesh/mesh.h"
#include "libmesh/node.h"
#include "libmesh/mesh_refinement.h"
#include "libmesh/mesh.h"
#include "libmesh/mesh_tools.h"
#include "libmesh/equation_systems.h"
#include "libmesh/explicit_system.h"
#include "libmesh/elem.h"
#include "libmesh/numeric_vector.h"
#include "libmesh/exodusII_io.h"
#include "libmesh/mesh_generation.h"

using namespace libMesh;

int main (int argc, char ** argv)
{

  LibMeshInit init (argc, argv);

  Parallel::Communicator & comm = init.comm();

  Mesh mesh(init.comm());

  MeshTools::Generation::build_square (mesh,
                                       100, 100,
                                       -1., 1.,
                                       -1., 1.,
                                       QUAD4);

  EquationSystems equation_systems(mesh);

  auto & primary = equation_systems.add_system<ExplicitSystem>("primary");

  primary.add_variable("u", CONSTANT, MONOMIAL);

  equation_systems.init();

  return 0;
}
