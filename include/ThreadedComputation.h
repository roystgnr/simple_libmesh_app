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
      _evaluation_points(1)
  {
  }

  void operator() (const ConstElemRange & range)
  {
    ParallelUniqueId pid;
    auto tid = pid.id;

    // Get our own unique FEData
    _this_fe_data = _fe_data[tid];

    for (auto & elem : range)
    {
      onElement(elem);
    }
  }

  void join(const ThreadedComputation & tc)
  {
  }

protected:
  virtual void onElement(const Elem * elem) = 0;

  std::vector<std::shared_ptr<FEData> > & _fe_data;

  std::shared_ptr<FEData> _this_fe_data;

  std::vector<Point> _evaluation_points;
};


#endif
