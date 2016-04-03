#ifndef THREADEDCOMPUTATION_H
#define THREADEDCOMPUTATION_H

// libMesh Includes
#include "libmesh/threads.h"
#include "libmesh/parallel.h"
#include "libmesh/elem_range.h"
#include "libmesh/elem.h"

using namespace libMesh;

class ThreadedComputation
{
public:
  ThreadedComputation()
  {
  }

  /**
   * Splitting Constructor
   */
  ThreadedComputation(ThreadedComputation & x, Threads::split split)
  {
  }

  void operator() (ConstElemRange & range)
  {
    for (auto & elem : range)
    {
      std::cout<<elem->id()<<std::endl;
    }
  }

  void join(const ThreadedComputation & tc)
  {
  }
};


#endif
