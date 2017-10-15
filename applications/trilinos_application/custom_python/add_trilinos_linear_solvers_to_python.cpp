//
//   Project Name:        Kratos
//   Last modified by:    $Author: rrossi $
//   Date:                $Date: 2008-12-09 20:20:55 $
//   Revision:            $Revision: 1.5 $
//
//

// System includes

#if defined(KRATOS_PYTHON)
// External includes
#include <boost/python.hpp>

#include "custom_python/add_trilinos_strategies_to_python.h"

//Trilinos includes
#include "mpi.h"
#include "Epetra_MpiComm.h"
#include "Epetra_Comm.h"
#include "Epetra_Map.h"
#include "Epetra_Vector.h"
#include "Epetra_FECrsGraph.h"
#include "Epetra_FECrsMatrix.h"
#include "Epetra_FEVector.h"
#include "Epetra_IntSerialDenseVector.h"
#include "Epetra_SerialDenseMatrix.h"


// Project includes
#include "includes/define.h"
#include "trilinos_application.h"
#include "trilinos_space.h"
#include "spaces/ublas_space.h"
#include "includes/model_part.h"


//linear solvers
#include "linear_solvers/linear_solver.h"

//utilities
#include "python/pointer_vector_set_python_interface.h"

//teuchos parameter list
#include "Teuchos_ParameterList.hpp"

#include "external_includes/epetra_default_utility.h"
#include "external_includes/aztec_solver.h"
#include "external_includes/amesos_solver.h"
#include "external_includes/ml_solver.h"

#include "external_includes/amgcl_mpi_solver.h"
#include "external_includes/amgcl_mpi_schur_complement_solver.h"

namespace Kratos
{

namespace Python
{

using namespace boost::python;

typedef TrilinosSpace<Epetra_FECrsMatrix, Epetra_FEVector> TrilinosSparseSpaceType;
typedef UblasSpace<double, Matrix, Vector> TrilinosLocalSpaceType;
typedef LinearSolver<TrilinosSparseSpaceType, TrilinosLocalSpaceType > TrilinosLinearSolverType;

void Solve(TrilinosLinearSolverType& solver, 
            TrilinosSparseSpaceType::MatrixType& rA, 
            TrilinosSparseSpaceType::VectorType& rX, 
            TrilinosSparseSpaceType::VectorType& rB
            
            )
{
    solver.Solve(rA,rX,rB);
}

void  AddLinearSolvers()
{
    
    class_<TrilinosLinearSolverType, TrilinosLinearSolverType::Pointer, boost::noncopyable > ("TrilinosLinearSolver", init<>())
    .def("Solve", Solve);

    class_<EpetraDefaultSetter, boost::noncopyable>("EpetraDefaultSetter",init<>())
    .def("SetDefaults", &EpetraDefaultSetter::SetDefaults);

    typedef AztecSolver<TrilinosSparseSpaceType, TrilinosLocalSpaceType > AztecSolverType;
    class_<AztecSolverType, bases<TrilinosLinearSolverType>, boost::noncopyable >
    ("AztecSolver", init< Teuchos::ParameterList&, std::string, Teuchos::ParameterList&, double, int, int >())
    .def(init<Parameters>())
    .def("SetScalingType", &AztecSolverType::SetScalingType)
    ;

    typedef AmesosSolver<TrilinosSparseSpaceType, TrilinosLocalSpaceType > AmesosSolverType;
    class_<AmesosSolverType, bases<TrilinosLinearSolverType>, boost::noncopyable >
    ("AmesosSolver", init<const std::string&, Teuchos::ParameterList& >())
    .def(init<Parameters>())
    .def(init<Parameters>())
    ;

    typedef MultiLevelSolver<TrilinosSparseSpaceType, TrilinosLocalSpaceType > MLSolverType;
    class_<MLSolverType, bases<TrilinosLinearSolverType>, boost::noncopyable >
    ("MultiLevelSolver", init<Teuchos::ParameterList&, Teuchos::ParameterList&, double, int >())
    .def(init<Parameters>())
    .def("SetScalingType", &MLSolverType::SetScalingType)
    .def("SetReformPrecAtEachStep", &MLSolverType::SetReformPrecAtEachStep)
    ;

    typedef AmgclMPISolver<TrilinosSparseSpaceType, TrilinosLocalSpaceType > AmgclMPISolverType;
    class_<AmgclMPISolverType, bases<TrilinosLinearSolverType>, boost::noncopyable >
    ("AmgclMPISolver",init<Parameters>()) //init<double, int,int,bool >())
    .def("SetDoubleParameter", &AmgclMPISolverType::SetDoubleParameter)
    .def("SetIntParameter", &AmgclMPISolverType::SetIntParameter)
    ;
    
    typedef AmgclMPISchurComplementSolver<TrilinosSparseSpaceType, TrilinosLocalSpaceType > AmgclMPISchurComplementSolverType;
    class_<AmgclMPISchurComplementSolverType, bases<TrilinosLinearSolverType>, boost::noncopyable >
    ("AmgclMPISchurComplementSolver",init<Parameters>()) 
    ;
    
    enum_<AztecScalingType>("AztecScalingType")
    .value("NoScaling", NoScaling)
    .value("LeftScaling", LeftScaling)
    .value("SymmetricScaling", SymmetricScaling)
    ;

    enum_<MLSolverType::ScalingType>("MLSolverScalingType")
    .value("NoScaling", MLSolverType::NoScaling)
    .value("LeftScaling", MLSolverType::LeftScaling)
    ;
    
}


} // namespace Python.

} // namespace Kratos.

#endif // KRATOS_PYTHON defined