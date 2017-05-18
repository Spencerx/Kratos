// ==============================================================================
//  ChimeraApplication
//
//  License:         BSD License
//                   license: ChimeraApplication/license.txt
//
//  Main authors:    Aditya Ghantasala, https://github.com/adityaghantasala
//
// ==============================================================================
//

#if !defined(KRATOS_CUSTOM_APPLY_CHIMERA_USING_CHIMERA_H_INCLUDED )
#define  KRATOS_CUSTOM_APPLY_CHIMERA_USING_CHIMERA_H_INCLUDED


// System includes

#include <string>
#include <iostream>
#include <algorithm>

// External includes
#include "includes/kratos_flags.h"

#include "utilities/binbased_fast_point_locator.h"

// Project includes

#include "includes/define.h"
#include "processes/process.h"
#include "includes/kratos_flags.h"
#include "includes/element.h"
#include "includes/model_part.h"
#include "geometries/geometry_data.h"
#include "includes/variables.h"

#include "spaces/ublas_space.h"
#include "linear_solvers/linear_solver.h"
#include "solving_strategies/schemes/residualbased_incrementalupdate_static_scheme.h"
#include "solving_strategies/builder_and_solvers/residualbased_block_builder_and_solver.h"
#include "solving_strategies/strategies/residualbased_linear_strategy.h"
#include "elements/distance_calculation_element_simplex.h"

// Application includes
#include "custom_utilities/multipoint_constraint_data.hpp"
#include "custom_processes/custom_calculate_and_extract_distance_process.h"
#include "custom_hole_cutting_process.h"
#include "custom_processes/apply_multi_point_constraints_process_chimera.h"

namespace Kratos {

///@name Kratos Globals
///@{

///@}
///@name Type Definitions
///@{

///@}
///@name  Enum's
///@{

///@}
///@name  Functions
///@{

///@}
///@name Kratos Classes
///@{

/// Short class definition.

template<unsigned int TDim>

class CustomApplyChimeraUsingMpcProcess {
public:

	///@name Type Definitions
	///@{

	///@}
	///@name Pointer Definitions
	/// Pointer definition of CustomExtractVariablesProcess
	KRATOS_CLASS_POINTER_DEFINITION(CustomApplyChimeraUsingMpcProcess);


	typedef typename BinBasedFastPointLocator<TDim>::Pointer BinBasedPointLocatorPointerType;
	///@}
	///@name Life Cycle
	///@{
CustomApplyChimeraUsingMpcProcess(ModelPart& AllModelPart,ModelPart& BackgroundModelPart,ModelPart& PatchModelPart,double distance = 0) : mrAllModelPart(AllModelPart), mrBackgroundModelPart(BackgroundModelPart), mrPatchModelPart(PatchModelPart), overlap_distance(distance) 

{		
		this->pBinLocatorForBackground = BinBasedPointLocatorPointerType ( new BinBasedFastPointLocator<TDim>(mrBackgroundModelPart) );
		this->pBinLocatorForPatch = BinBasedPointLocatorPointerType( new BinBasedFastPointLocator<TDim>(mrPatchModelPart) );
		this->pMpcProcess =  ApplyMultipointConstraintsProcessChimera::Pointer(new ApplyMultipointConstraintsProcessChimera(mrAllModelPart)); 
		this->pHoleCuttingProcess = CustomHoleCuttingProcess::Pointer(new CustomHoleCuttingProcess());
		this->pCalculateDistanceProcess = typename CustomCalculateAndExtractDistanceProcess<TDim>::Pointer(new CustomCalculateAndExtractDistanceProcess<TDim>());

	}

	/// Destructor.
	virtual ~CustomApplyChimeraUsingMpcProcess() {
	}

	///@}
	///@name Operators
	///@{

	void operator()() {
		Execute();
	}

	///@}
	///@name Operations
	///@{

	virtual void Execute() {
	}

	virtual void Clear() {
	}

	
	void ApplyMpcConstraint(ModelPart& mrBoundaryModelPart,BinBasedPointLocatorPointerType& pBinLocator){

		/*
		 * This part of the code below is adapted from "MappingPressureToStructure" function of class CalculateSignedDistanceTo3DSkinProcess
		 */

		{
			//loop over nodes and find the triangle in which it falls, than do interpolation
			array_1d<double, TDim+1 > N;
			const int max_results = 10000;
			typename BinBasedFastPointLocator<TDim>::ResultContainerType results(max_results);
			const int n_boundary_nodes = mrBoundaryModelPart.Nodes().size();
			
			
			
			#pragma omp parallel for firstprivate(results,N)
			//MY NEW LOOP: reset the visited flag
			for (int i = 0; i < n_boundary_nodes; i++)
			{
				ModelPart::NodesContainerType::iterator iparticle = mrBoundaryModelPart.NodesBegin() + i;
				Node < 3 > ::Pointer p_boundary_node = *(iparticle.base());
				p_boundary_node->Set(VISITED, false);
			}
			
			for (int i = 0; i < n_boundary_nodes; i++)
			
			{
				ModelPart::NodesContainerType::iterator iparticle = mrBoundaryModelPart.NodesBegin() + i;
				Node < 3 > ::Pointer p_boundary_node = *(iparticle.base());
				
				
				typename BinBasedFastPointLocator<TDim>::ResultIteratorType result_begin = results.begin();
				
				Element::Pointer pElement;
				
				bool is_found = false;
				is_found = pBinLocator->FindPointOnMesh(p_boundary_node->Coordinates(), N, pElement,result_begin, max_results);
				
				if (is_found == true)
				{
					// TODO : For now it only does velocities by components
					// This should be extended to a general variable
					
					//Check if some of the host elements are made inactive
					if ((pElement)->IsDefined(ACTIVE)){
						
						if(!(pElement->Is(ACTIVE))){
							std::cout<<"Warning : One of the hole element is used for MPC constraint"<<std::endl;
							pElement->Set(ACTIVE);
							std::cout<<"Setting the element: "<<pElement->Id()<<" to active"<<std::endl;
						
						
						}
				
					
						 




					}
						
					Geometry<Node<3> >& geom = pElement->GetGeometry();
					//std::cout<<"Element Id: "<< pElement->Id();

					// TO DO Apply MPC constraints. Here have to use the mpc functions to apply the constraint
					
					//std::cout<<p_boundary_node->Id()<<" \t"<<geom[0].Id()<<" \t"<<geom[1].Id()<<" \t"<<geom[2].Id()<<" \t"<<N[0]<<" \t "<<N[1]<<" \t"<< N[2]<<std::endl;										
					//for debugging the patch elements which are host	
					//pElement->Set(ACTIVE,false);
					//p_boundary_node->Set(VISITED);
					for(int i = 0; i < geom.size(); i++){
					
					pMpcProcess->AddMasterSlaveRelation( geom[i],VELOCITY_X,*p_boundary_node,VELOCITY_X,N[i], 0 );
					pMpcProcess->AddMasterSlaveRelation( geom[i],VELOCITY_Y,*p_boundary_node,VELOCITY_Y,N[i], 0 );
					//pMpcProcess->AddMasterSlaveRelationVariables( geom[i],PRESSURE,*p_boundary_node,PRESSURE,N[i], 0 );
					
					


					}	//Nodes loop inside the found element ends here
										

					
				}
			}
			
	
			std::cout<<"mpcProcess ends"<<std::endl;

		}
	}




//Apply Chimera without overlap
	void ApplyChimeraUsingMpc(ModelPart& mrPatchBoundaryModelPart){

		if(overlap_distance < 0) {

			std::cout<<"Overlap distance should be a positive number"<<std::endl;
			std::exit(-1);
		}
		if (overlap_distance > 0){
          
		ModelPart HoleModelPart = ModelPart("HoleModelpart");
		ModelPart HoleBoundaryModelPart = ModelPart("HoleBoundaryModelPart");

		this->pCalculateDistanceProcess->ExtractDistance(mrPatchModelPart, mrBackgroundModelPart, mrPatchBoundaryModelPart);

	
		this->pHoleCuttingProcess->CreateHoleAfterDistance(mrBackgroundModelPart,HoleModelPart,HoleBoundaryModelPart, overlap_distance);
		

		ApplyMpcConstraint(mrPatchBoundaryModelPart,pBinLocatorForBackground);

		ApplyMpcConstraint(HoleBoundaryModelPart,pBinLocatorForPatch);




		}

		else {

		ModelPart HoleModelPart =  ModelPart("HoleModelPart");
		ModelPart HoleBoundaryModelPart =  ModelPart("HoleBoundaryModelPart");

		this->pCalculateDistanceProcess->ExtractDistance(mrPatchModelPart, mrBackgroundModelPart, mrPatchBoundaryModelPart);

	
		this->pHoleCuttingProcess->CreateHoleAfterDistance(mrBackgroundModelPart,HoleModelPart,HoleBoundaryModelPart,overlap_distance);
		

		ApplyMpcConstraint(mrPatchBoundaryModelPart,pBinLocatorForBackground);






		}
		
		


		}


   


	virtual std::string Info() const {
		return "CustomApplyChimeraUsingMpcProcess";
	}

	/// Print information about this object.
	virtual void PrintInfo(std::ostream& rOStream) const {
		rOStream << "CustomApplyChimeraUsingMpcProcess";
	}

	/// Print object's data.
	virtual void PrintData(std::ostream& rOStream) const {
	}

	///@}
	///@name Friends
	///@{

	///@}

protected:

	///@name Protected static Member Variables
	///@{

	///@}
	///@name Protected member Variables
	///@{

	///@}
	///@name Protected Operators
	///@{

	///@}
	///@name Protected Operations
	///@{

	///@}
	///@name Protected  Access
	///@{

	///@}
	///@name Protected Inquiry
	///@{

	///@}
	///@name Protected LifeCycle
	///@{

	///@}

private:

	///@name Static Member Variables
	///@{

	///@}
	///@name Member Variables
	///@{
	
	//ModelPart &mrBackGroundModelPart;
	//ModelPart &mrPatchSurfaceModelPart;
	BinBasedPointLocatorPointerType pBinLocatorForBackground; // Template argument 3 stands for 3D case
	BinBasedPointLocatorPointerType pBinLocatorForPatch;
	ApplyMultipointConstraintsProcessChimera::Pointer pMpcProcess;
	CustomHoleCuttingProcess::Pointer pHoleCuttingProcess;
	typename CustomCalculateAndExtractDistanceProcess<TDim>::Pointer pCalculateDistanceProcess;
	double overlap_distance;
	ModelPart& mrBackgroundModelPart;
	ModelPart& mrPatchModelPart;
	ModelPart& mrAllModelPart;

	///@}
	///@name Private Operators
	///@{

	///@}
	///@name Private Operations
	///@{

	///@}
	///@name Private  Access
	///@{

	///@}
	///@name Private Inquiry
	///@{

	///@}
	///@name Un accessible methods
	///@{

	/// Assignment operator.
	CustomApplyChimeraUsingMpcProcess& operator=(CustomApplyChimeraUsingMpcProcess const& rOther);

	/// Copy constructor.
	//CustomExtractVariablesProcess(CustomExtractVariablesProcess const& rOther);

	///@}

}; // Class CustomExtractVariablesProcess

}  // namespace Kratos.

#endif // KRATOS_CUSTOM_EXTRACT_VARIABLES_PROCESS_H_INCLUDED  defined