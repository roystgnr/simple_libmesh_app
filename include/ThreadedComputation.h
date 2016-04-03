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
      _fe_data(fe_data)
  {
  }

  /**
   * Splitting Constructor
   */
  ThreadedComputation(ThreadedComputation & x, Threads::split split):
      _fe_data(x._fe_data)
  {
  }

  void operator() (ConstElemRange & range)
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
      fe->reinit(elem);
    }
  }

  void join(const ThreadedComputation & tc)
  {
  }

protected:
  std::vector<std::shared_ptr<FEData> > & _fe_data;
};


#endif
