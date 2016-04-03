// Local Includes
#include "ThreadedComputation.h"
#include "FEData.h"

// libMesh Includes
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
                                       400, 400,
                                       -1., 1.,
                                       -1., 1.,
                                       QUAD4);

  EquationSystems equation_systems(mesh);

  auto & primary = equation_systems.add_system<ExplicitSystem>("primary");

  primary.add_variable("u", FIRST, LAGRANGE);

  equation_systems.init();

  std::vector<std::shared_ptr<FEData> > fe_data(libMesh::n_threads());

  for (unsigned int i=0; i<libMesh::n_threads(); i++)
    fe_data[i] = std::make_shared<FEData>(mesh);

  ConstElemRange elem_range(mesh.local_elements_begin(), mesh.local_elements_end(), 1);

  ThreadedComputation tc(fe_data);

  // How many times to go through the mesh
  unsigned int n_sweeps = 10;


  // Using qrule
  {
    std::cout<<"\nReinit with qrule..."<<std::endl;

    auto execution_start_time = std::chrono::steady_clock::now();

    for (unsigned int sweep=0; sweep<n_sweeps; sweep++)
      Threads::parallel_reduce(elem_range, tc);

    comm.barrier();

    auto execution_time = std::chrono::steady_clock::now() - execution_start_time;

    auto execution_time_seconds = std::chrono::duration<Real>(execution_time).count();
    std::cout<<"Total Time: "<<execution_time_seconds<<std::endl;

    auto ns_per_element = std::chrono::duration<Real, std::nano>(execution_time).count() / (Real)(mesh.n_elem() * n_sweeps);
    std::cout<<"ns/element: "<<ns_per_element<<std::endl;
    std::cout<<"Levelized ns/element: "<<comm.size() * libMesh::n_threads() * ns_per_element<<std::endl;
  }


  // Using custom point
  {
    std::cout<<"\nReinit with custom point..."<<std::endl;

    tc.useCustomPoint(true);

    auto execution_start_time = std::chrono::steady_clock::now();

    for (unsigned int sweep=0; sweep<n_sweeps; sweep++)
      Threads::parallel_reduce(elem_range, tc);

    comm.barrier();

    auto execution_time = std::chrono::steady_clock::now() - execution_start_time;

    auto execution_time_seconds = std::chrono::duration<Real>(execution_time).count();
    std::cout<<"Total Time: "<<execution_time_seconds<<std::endl;

    auto ns_per_element = std::chrono::duration<Real, std::nano>(execution_time).count() / (Real)(mesh.n_elem() * n_sweeps);
    std::cout<<"ns/element: "<<ns_per_element<<std::endl;
    std::cout<<"Levelized ns/element: "<<comm.size() * libMesh::n_threads() * ns_per_element<<std::endl;
  }

  return 0;
}
