//
//   Project Name:        KratosDamApplication    $
//   Last modified by:    $Author: Lorenzo Gracia $
//   Date:                $Date:       March 2017 $
//   Revision:            $Revision:          0.0 $
//

#if !defined(KRATOS_DAM_NODAL_YOUNG_MODULUS_PROCESS )
#define  KRATOS_DAM_NODAL_YOUNG_MODULUS_PROCESS

#include <cmath>

#include "includes/kratos_flags.h"
#include "includes/kratos_parameters.h"
#include "processes/process.h"

#include "custom_utilities/pendulum_convergence_utility.hpp"

#include "dam_application_variables.h"

namespace Kratos
{

class DamNodalYoungModulusProcess : public Process
{
    
public:

    KRATOS_CLASS_POINTER_DEFINITION(DamNodalYoungModulusProcess);
    
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    /// Constructor
    DamNodalYoungModulusProcess(ModelPart& model_part,
                                Parameters rParameters
                                ) : Process(Flags()) , mr_model_part(model_part) , mrParameters(rParameters)
    {
        KRATOS_TRY
			 
        //only include validation with c++11 since raw_literals do not exist in c++03
        Parameters default_parameters( R"(
            {
                "model_part_name":"PLEASE_CHOOSE_MODEL_PART_NAME",
                "mesh_id": 0,
                "variable_name": "PLEASE_PRESCRIBE_VARIABLE_NAME",
                "is_fixed"                                         : false,
                "Young_Modulus_1"                                  : 10.0,
                "Young_Modulus_2"                                  : 60.0,
                "Young_Modulus_3"                                  : 50.0,
                "Young_Modulus_4"                                  : 70.0
            }  )" );
        
        // Some values need to be mandatorily prescribed since no meaningful default value exist. For this reason try accessing to them
        // So that an error is thrown if they don't exist
        rParameters["Young_Modulus_1"];
        rParameters["variable_name"];
        rParameters["model_part_name"];

        // Now validate agains defaults -- this also ensures no type mismatch
        rParameters.ValidateAndAssignDefaults(default_parameters);

        mmesh_id = rParameters["mesh_id"].GetInt();
        mvariable_name = rParameters["variable_name"].GetString();
        mis_fixed = rParameters["is_fixed"].GetBool();
        myoung_1 = rParameters["Young_Modulus_1"].GetDouble();
        myoung_2 = rParameters["Young_Modulus_2"].GetDouble();
        myoung_3 = rParameters["Young_Modulus_3"].GetDouble();
        myoung_4 = rParameters["Young_Modulus_4"].GetDouble();

        KRATOS_CATCH("");
    }

    ///------------------------------------------------------------------------------------
    
    /// Destructor
    virtual ~DamNodalYoungModulusProcess() {}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Execute()
    {
        
        KRATOS_TRY;
        
        Variable<double> var = KratosComponents< Variable<double> >::Get(mvariable_name);
        const int nnodes = mr_model_part.GetMesh(mmesh_id).Nodes().size();
        
        if(nnodes != 0)
        {
            ModelPart::NodesContainerType::iterator it_begin = mr_model_part.GetMesh(mmesh_id).NodesBegin();
        
            #pragma omp parallel for
            for(int i = 0; i<nnodes; i++)
            {
                ModelPart::NodesContainerType::iterator it = it_begin + i;
                
                if(mis_fixed)
                {
                    it->Fix(var);
                }
                                
                double Young = myoung_1 + (myoung_2*it->Coordinate(1)) + (myoung_3*it->Coordinate(2)) + (myoung_4*it->Coordinate(3));

                if(Young <= 0.0)
                {                   
                    it->FastGetSolutionStepValue(var) = 0.0;
                    
                }
                else
                    it->FastGetSolutionStepValue(var) = Young;
            }
        }
        
        KRATOS_CATCH("");
    }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void ExecuteInitializeSolutionStep()
    {
        KRATOS_TRY;

        double tolerance = 0.01;
        double reference = -0.0416059;
        Vector Vectorconvergence;
        Vectorconvergence.resize(2);
        int factor = 1;

        Vectorconvergence = PendulumConvergenceUtility(mr_model_part).CheckConvergence(tolerance,reference);
        this-> ComputingFactor(factor,Vectorconvergence[1]);

        KRATOS_WATCH(Vectorconvergence)
        KRATOS_WATCH(factor)

        if(Vectorconvergence[0] < 0.5 )
        {
            Variable<double> var = KratosComponents< Variable<double> >::Get(mvariable_name);
            const int nnodes = mr_model_part.GetMesh(mmesh_id).Nodes().size();

            if(nnodes != 0)
            {
                ModelPart::NodesContainerType::iterator it_begin = mr_model_part.GetMesh(mmesh_id).NodesBegin();
                #pragma omp parallel for
                for(int i = 0; i<nnodes; i++)
                {
                    ModelPart::NodesContainerType::iterator it = it_begin + i;
            
                    if(mis_fixed)
                    {
                        it->Fix(var);
                    }
            
                    double Young = myoung_1/factor + (myoung_2*it->Coordinate(1)) + (myoung_3*it->Coordinate(2)) + (myoung_4*it->Coordinate(3));
            
                    if(Young <= 0.0)
                    {                   
                        it->FastGetSolutionStepValue(var) = 0.0;
                
                    }
                    else
                        it->FastGetSolutionStepValue(var) = Young;
                }
            }

        }

        
            KRATOS_CATCH("");
    }  

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    /// Turn back information as a string.
    std::string Info() const
    {
        return "DamNodalYoungModulusProcess";
    }

    /// Print information about this object.
    void PrintInfo(std::ostream& rOStream) const
    {
        rOStream << "DamNodalYoungModulusProcess";
    }

    /// Print object's data.
    void PrintData(std::ostream& rOStream) const
    {
    }

///----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

protected:

    /// Member Variables
    ModelPart& mr_model_part;
    Parameters& mrParameters;
    std::size_t mmesh_id;
    std::string mvariable_name;
    bool mis_fixed;
    double myoung_1;
    double myoung_2;
    double myoung_3;
    double myoung_4;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void ComputingFactor(int& rfactor, double value )
    {
        KRATOS_TRY;

        if(value > 0.038)
            rfactor = 1;
        
        if(value < 0.038)
            rfactor = 2;
        
        if(value < 0.035)
            rfactor = 4;
        
        if(value < 0.025)
            rfactor = 8;
        
            KRATOS_CATCH("");
    }  


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

private:

    /// Assignment operator.
    DamNodalYoungModulusProcess& operator=(DamNodalYoungModulusProcess const& rOther);

};//Class


/// input stream function
inline std::istream& operator >> (std::istream& rIStream,
                                  DamNodalYoungModulusProcess& rThis);

/// output stream function
inline std::ostream& operator << (std::ostream& rOStream,
                                  const DamNodalYoungModulusProcess& rThis)
{
    rThis.PrintInfo(rOStream);
    rOStream << std::endl;
    rThis.PrintData(rOStream);

    return rOStream;
}

} /* namespace Kratos.*/

#endif /* KRATOS_DAM_NODAL_YOUNG_MODULUS_PROCESS defined */

