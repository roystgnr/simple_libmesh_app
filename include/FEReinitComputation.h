#ifndef FEREINITCOMPUTATION_H
#define FEREINITCOMPUTATION_H

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

using namespace libMesh;

class FEReinitComputation : public ThreadedComputation
{
public:
  FEReinitComputation(std::vector<std::shared_ptr<FEData> > & fe_data):
      ThreadedComputation(fe_data)
  {
  }

  /**
   * Splitting Constructor
   */
  FEReinitComputation(FEReinitComputation & x, Threads::split split):
      ThreadedComputation(x, split),
      _custom_point(x._custom_point)
  {
  }

  void join(const FEReinitComputation & tc)
  {
  }

  void useCustomPoint(bool custom_point) { _custom_point = custom_point; }

protected:
  virtual void onElement(const Elem * elem)
  {
    if (_custom_point)
      _fe->reinit(elem, &_evaluation_points);
    else
      _fe->reinit(elem);
  }

  bool _custom_point = false;
};


#endif
