#ifndef INVERSEMAPCOMPUTATION_H
#define INVERSEMAPCOMPUTATION_H

// Local Includes
#include "ParallelUniqueId.h"
#include "FEData.h"
#include "ThreadedComputation.h"

// libMesh Includes
#include "libmesh/threads.h"
#include "libmesh/parallel.h"
#include "libmesh/elem_range.h"
#include "libmesh/elem.h"
#include "libmesh/point.h"
#include "libmesh/mesh.h"
#include "libmesh/fe.h"
#include "libmesh/quadrature.h"
#include "libmesh/fe_interface.h"

using namespace libMesh;

class InverseMapComputation : public ThreadedComputation
{
public:
  InverseMapComputation(std::vector<std::shared_ptr<FEData> > & fe_data):
      ThreadedComputation(fe_data)
  {
  }

  /**
   * Splitting Constructor
   */
  InverseMapComputation(InverseMapComputation & x, Threads::split split):
      ThreadedComputation(x, split),
      _custom_point(x._custom_point)
  {
  }

  void join(const InverseMapComputation & tc)
  {
  }

  void useCustomPoint(bool custom_point) { _custom_point = custom_point; }

protected:
  virtual void onElement(const Elem * elem)
  {
    Point & point = _evaluation_points[0];

    point.zero();

    // Compute the average nodal position to evaluate at:
    for (unsigned int i=0; i<elem->n_nodes(); i++)
      point += *elem->get_node(i);

    point /= elem->n_nodes();

    FEInterface::inverse_map(elem->dim(), _this_fe_data->_fe_type, elem, _evaluation_points, _reference_points);
  }

  bool _custom_point = false;

  std::vector<Point> _reference_points;
};


#endif
