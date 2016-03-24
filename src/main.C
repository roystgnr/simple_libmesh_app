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
#include "libmesh/parallel.h"

using namespace libMesh;

class CustomClass
{
public:
  CustomClass()
  {
    data1 = rand() / (double)RAND_MAX;
    data2 = rand() / (double)RAND_MAX;
  }

  Real data1;
  Real data2;
};


namespace libMesh {
namespace Parallel {
template<>
class Packing<std::shared_ptr<CustomClass> > {
public:

  typedef Real buffer_type;

  static unsigned int packed_size(typename std::vector<Real>::const_iterator in)
  {
    unsigned int total_size = 0;

    // data1
    total_size += 1;

    // data2
    total_size += 1;

    return total_size;
  }

  static unsigned int packable_size(const std::shared_ptr<CustomClass> & cc, const void *)
  {
    unsigned int total_size = 0;

    // data1
    total_size += 1;

    // data2
    total_size += 1;

    return total_size;
  }

  template <typename Iter>
  static void pack (const std::shared_ptr<CustomClass> & cc, Iter data_out, const void *)
  {
    data_out = cc->data1;
    data_out = cc->data2;
  }

  static std::shared_ptr<CustomClass> unpack(typename std::vector<Real>::const_iterator in, void * mesh_context)
  {
    std::shared_ptr<CustomClass> cc = std::make_shared<CustomClass>();

    cc->data1 = (*in++);
    cc->data2 = (*in++);

    return cc;
  }

};

} // namespace Parallel

} // namespace libMesh


using namespace libMesh;

int main (int argc, char ** argv)
{

  LibMeshInit init (argc, argv);

  Parallel::Communicator & comm = init.comm();

  Parallel::MessageTag tag(210);

  if (comm.rank() == 0)
  {
    std::vector<std::shared_ptr<CustomClass> > custom_class_vector { std::make_shared<CustomClass>(), std::make_shared<CustomClass>() };

    for (auto & cc : custom_class_vector)
      std::cerr<<comm.rank()<<" "<<cc->data1<<" "<<cc->data2<<std::endl;

    comm.send_packed_range(1, &custom_class_vector, custom_class_vector.begin(), custom_class_vector.end(), tag);
  }
  else
  {
    std::vector<std::shared_ptr<CustomClass> > custom_class_vector;

    comm.receive_packed_range(0, &custom_class_vector, std::back_inserter(custom_class_vector), (std::shared_ptr<CustomClass>*)(libmesh_nullptr), tag);

    for (auto & cc : custom_class_vector)
      std::cerr<<comm.rank()<<" "<<cc->data1<<" "<<cc->data2<<std::endl;
  }

  return 0;
}
