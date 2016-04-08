#ifndef FEDATA_H
#define FEDATA_H

// libMesh Includes
#include "libmesh/mesh.h"
#include "libmesh/fe.h"
#include "libmesh/quadrature.h"
#include "libmesh/fe_type.h"

using namespace libMesh;

/**
 * Simple holder class for all of the types of FE data that are needed on a thread
 * This is just to help organize things... and so we don't have to keep recreating these objects
 */
class FEData
{
public:
  FEData(MeshBase & mesh):
      _dim(mesh.mesh_dimension()),

      // Volume Stuff:

      _fe_type(FIRST,LAGRANGE),
      _fe(FEBase::build(_dim, _fe_type)),
      _qrule(QBase::build(QGAUSS, _dim, FIRST)),
      _phi(_fe->get_phi()),


      // Face Stuff:

      // We use CONSTANT MONOMIAL here because we want a low order qrule on the face
      // we only want one single point to get the normal with
      _fe_type_face(CONSTANT,MONOMIAL),
      _fe_face(FEBase::build(_dim, _fe_type_face)),
      _qface(QBase::build(QGAUSS, _dim-1, FIRST)),
      _normals(_fe_face->get_normals())
  {
    _fe->attach_quadrature_rule(_qrule.get());
    _fe_face->attach_quadrature_rule(_qface.get());

    _fe->get_phi(); // Keeps libMesh from calculating too much
    _fe_face->get_phi(); // Keeps libMesh from calculating too much
  }


  const unsigned int _dim;

  FEType _fe_type;
  UniquePtr<FEBase> _fe;
  UniquePtr<QBase>  _qrule;
  const std::vector< std::vector< Real > > & _phi;

  FEType _fe_type_face;
  UniquePtr<FEBase> _fe_face;
  UniquePtr<QBase>  _qface;
  const std::vector<Point> & _normals;
};

#endif
