#ifndef THREADEDCOMPUTATION_H
#define THREADEDCOMPUTATION_H

// Local Includes
#include "ParallelUniqueId.h"
#include "FEData.h"

// libMesh Includes
#include "libmesh/threads.h"
#include "libmesh/parallel.h"
#include "libmesh/elem_range.h"
#include "libmesh/elem.h"
#include "libmesh/point.h"
#include "libmesh/mesh.h"
#include "libmesh/fe.h"
#include "libmesh/quadrature.h"

using namespace libMesh;

class ThreadedComputation
{
public:
  ThreadedComputation(std::vector<std::shared_ptr<FEData> > & fe_data):
      _fe_data(fe_data),
      _evaluation_points(1)
  {
  }

  /**
   * Splitting Constructor
   */
  ThreadedComputation(ThreadedComputation & x, Threads::split split):
      _fe_data(x._fe_data),
      _evaluation_points(1),
      _custom_point(x._custom_point)
  {
  }

  void operator() (const ConstElemRange & range)
  {
    ParallelUniqueId pid;
    auto tid = pid.id;

    // Get our own unique FEData
    auto & fe_data = _fe_data[tid];

    auto & fe = fe_data->_fe;
    auto & phi = fe_data->_phi;

    auto & fe_face = fe_data->_fe_face;
    auto & normals = fe_data->_normals;

    for (auto & elem : range)
    {
      if (_custom_point)
        fe->reinit(elem, &_evaluation_points);
      else
        fe->reinit(elem);
    }
  }

  void join(const ThreadedComputation & tc)
  {
  }

  void useCustomPoint(bool custom_point) { _custom_point = custom_point; }

protected:
  std::vector<std::shared_ptr<FEData> > & _fe_data;

  std::vector<Point> _evaluation_points;

  bool _custom_point = false;
};


#endif
