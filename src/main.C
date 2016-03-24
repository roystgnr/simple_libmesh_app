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
                                       3, 3,
                                       -1., 1.,
                                       -1., 1.,
                                       QUAD4);

  EquationSystems equation_systems(mesh);

  auto & output_system = equation_systems.add_system<ExplicitSystem>("output_system");

  output_system.add_variable("processor", CONSTANT, MONOMIAL);

  equation_systems.init();

  auto & solution_vector = output_system.solution;

  for (auto elem_it = mesh.local_elements_begin(); elem_it != mesh.local_elements_end(); ++elem_it)
  {
    auto elem = *elem_it;

    auto dof = elem->dof_number(0, 0, 0);

    solution_vector->set(dof, elem->processor_id());
  }

  solution_vector->close();


  ExodusII_IO(mesh).write_equation_systems("out.e", equation_systems);

  return 0;
}
