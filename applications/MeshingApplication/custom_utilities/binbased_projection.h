// KRATOS  __  __ _____ ____  _   _ ___ _   _  ____
//        |  \/  | ____/ ___|| | | |_ _| \ | |/ ___|
//        | |\/| |  _| \___ \| |_| || ||  \| | |  _
//        | |  | | |___ ___) |  _  || || |\  | |_| |
//        |_|  |_|_____|____/|_| |_|___|_| \_|\____| APPLICATION
//
//  License:		 BSD License
//                                       Kratos default license: kratos/license.txt
//
//  Main authors:    Antonia Larese De Tetto
//

#if !defined(KRATOS_BINBASED_PROJECTION )
#define  KRATOS_BINBASED_PROJECTION

//External includes

// System includes
#include <string>
#include <iostream>
#include <cstdlib>

// Project includes
#include "includes/define.h"
#include "includes/model_part.h"
#include "utilities/timer.h"
#include "meshing_application_variables.h"

//Database includes
#include "spatial_containers/spatial_containers.h"
#include "utilities/binbased_fast_point_locator.h"
#include "utilities/binbased_nodes_in_element_locator.h"

namespace Kratos
{
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

/// This class allows the interpolation between non-matching meshes in 2D and 3D.
/** @author  Antonia Larese De Tetto <antoldt@cimne.upc.edu>
*
* This class allows the interpolation of a scalar or vectorial variable  between non-matching meshes
* in 2D and 3D.
*
* For every node of the destination model part it is checked in which element of the origin model part it is
* contained and a linear interpolation is performed
*
* The data structure used by default is static bin.
* In order to use this utility the construction of a bin of object @see BinBasedNodesInElementLocator
* and a bin of nodes @see BinBasedFastPointLocator
* is required at the beginning of the calculation (only ONCE).
*/

//class BinBasedMeshTransfer
template<std::size_t TDim >
class BinBasedMeshTransfer
{
public:
    ///@name Type Definitions
    ///@{

    /// Pointer definition of BinBasedMeshTransfer
    KRATOS_CLASS_POINTER_DEFINITION(BinBasedMeshTransfer<TDim >);

    /// Node type definition
    typedef Node NodeType;
    typedef Geometry<NodeType> GeometryType;

    ///@}
    ///@name Life Cycle
    ///@{

    /// Default constructor.
    BinBasedMeshTransfer() = default; //

    /// Destructor.
    virtual ~BinBasedMeshTransfer() = default;


    ///@}
    ///@name Operators
    ///@{


    ///@}
    ///@name Operations
    ///@{

    //If you want to pass the whole model part
    //**********************************************************************
    //**********************************************************************
    /// Interpolate the whole problem type
    /**
      * @param rOrigin_ModelPart: the model part  all the variable should be taken from
      * @param rDestination_ModelPart: the destination model part where we want to know the values of the variables
      */
    void DirectInterpolation(
        ModelPart& rOrigin_ModelPart ,
        ModelPart& rDestination_ModelPart
    )
    {
        KRATOS_TRY

        KRATOS_ERROR << "Not implemented yet" << std::endl;

        KRATOS_CATCH("")
    }


    //If you want to pass only one variable
    //**********************************************************************
    //**********************************************************************
    /// Interpolate one variable from the fixed mesh to the moving one
    /**
      * @param rFixed_ModelPart: the model part  all the variable should be taken from
      * @param rMoving_ModelPart: the destination model part where we want to know the values of the variables
      * @param rFixedDomainVariable: the name of the interpolated variable in the origin model part
      * @param rMovingDomainVariable: the name of the interpolated variable in the destination model part
      * @param node_locator: precomputed bin of objects. It is to be constructed separately @see binbased_fast_point_locator.h
      */
    // Form fixed to moving model part
    template<class TDataType>
    void DirectVariableInterpolation(
        ModelPart& rFixed_ModelPart ,
        ModelPart& rMoving_ModelPart,
        Variable<TDataType>& rFixedDomainVariable ,
        Variable<TDataType>& rMovingDomainVariable,
        BinBasedFastPointLocator<TDim>& node_locator
        )
    {
        KRATOS_TRY

        KRATOS_INFO("BinBasedMeshTransfer") << "Interpolate From Fixed Mesh*************************************" << std::endl;

        //creating an auxiliary list for the new nodes
        for(auto node_it = rMoving_ModelPart.NodesBegin(); node_it != rMoving_ModelPart.NodesEnd(); ++node_it) {
            ClearVariables(node_it, rMovingDomainVariable);
        }

        Vector N(TDim + 1);
        const int max_results = 10000;
        typename BinBasedFastPointLocator<TDim>::ResultContainerType results(max_results);
        const int nparticles = rMoving_ModelPart.Nodes().size();

        #pragma omp parallel for firstprivate(results,N)
        for (int i = 0; i < nparticles; i++) {
            ModelPart::NodesContainerType::iterator iparticle = rMoving_ModelPart.NodesBegin() + i;
            NodeType::Pointer pparticle = *(iparticle.base());
            auto result_begin = results.begin();
            Element::Pointer pelement;

            bool is_found = node_locator.FindPointOnMesh(pparticle->Coordinates(), N, pelement, result_begin, max_results);

            if (is_found == true) {
                //Interpolate(  ElemIt,  N, *it_found , rFixedDomainVariable , rMovingDomainVariable  );
                Interpolate(  pelement,  N, pparticle, rFixedDomainVariable , rMovingDomainVariable  );
            }
        }

        KRATOS_CATCH("")
    }

    /// Map one variable from the moving mesh to the fixed one -The two meshes should be of the same dimensions otherwise better to use
    /// MappingFromMovingMesh_VariableMeshes that is a much generic tool.
    /**
      * @param rFixed_ModelPart: the model part  all the variable should be taken from
      * @param rMoving_ModelPart: the destination model part where we want to know the values of the variables
      * @param rFixedDomainVariable: the name of the interpolated variable in the origin model part
      * @param rMovingDomainVariable: the name of the interpolated variable in the destination model part
      * @param node_locator: precomputed bin of objects (elements of the fixed mesh). It is to be constructed separately @see binbased_nodes_in_element_locator
      */
    // From moving to fixed model part
    template<class TDataType>
    void MappingFromMovingMesh(
        ModelPart& rMoving_ModelPart ,
        ModelPart& rFixed_ModelPart,
        Variable<TDataType>& rMovingDomainVariable ,
        Variable<TDataType>& rFixedDomainVariable,
        BinBasedFastPointLocator<TDim>& node_locator //this is a bin of objects which contains the FIXED model part
        )
    {
        KRATOS_TRY

        KRATOS_INFO("BinBasedMeshTransfer") << "Transfer From Moving Mesh*************************************" << std::endl;

        if (rMoving_ModelPart.NodesBegin()->SolutionStepsDataHas(rMovingDomainVariable) == false)
            KRATOS_THROW_ERROR(std::logic_error, "Add  MovingDomain VARIABLE!!!!!! ERROR", "");
        if (rFixed_ModelPart.NodesBegin()->SolutionStepsDataHas(rFixedDomainVariable) == false)
            KRATOS_THROW_ERROR(std::logic_error, "Add  FixedDomain VARIABLE!!!!!! ERROR", "");


        //creating an auxiliary list for the new nodes
        for(ModelPart::NodesContainerType::iterator node_it = rFixed_ModelPart.NodesBegin();
                node_it != rFixed_ModelPart.NodesEnd(); ++node_it)
        {

            ClearVariables(node_it, rFixedDomainVariable);

        }

        for (ModelPart::NodesContainerType::iterator node_it = rFixed_ModelPart.NodesBegin();
                node_it != rFixed_ModelPart.NodesEnd(); node_it++)
        {
// 			if (node_it->IsFixed(VELOCITY_X) == false)
// 			{
// 			    (node_it)->FastGetSolutionStepValue(VELOCITY) = ZeroVector(3);
// 			    (node_it)->FastGetSolutionStepValue(TEMPERATURE) = 0.0;
            (node_it)->GetValue(YOUNG_MODULUS) = 0.0;
// 			}
        }
        //definitions for spatial search
//         typedef NodeType PointType;
//         typedef NodeType::Pointer PointTypePointer;

        Vector N(TDim + 1);
        const int max_results = 10000;
        typename BinBasedFastPointLocator<TDim>::ResultContainerType results(max_results);
        const int nparticles = rMoving_ModelPart.Nodes().size();

        #pragma omp parallel for firstprivate(results,N)
        for (int i = 0; i < nparticles; i++)
        {
            ModelPart::NodesContainerType::iterator iparticle = rMoving_ModelPart.NodesBegin() + i;

            NodeType::Pointer pparticle = *(iparticle.base());
            auto result_begin = results.begin();

            Element::Pointer pelement;

            bool is_found = node_locator.FindPointOnMesh(pparticle->Coordinates(), N, pelement, result_begin, max_results);

            if (is_found == true)
            {
                GeometryType& geom = pelement->GetGeometry();
                //                  const array_1d<double, 3 > & vel_particle = (iparticle)->FastGetSolutionStepValue(VELOCITY);
                //                  const double& temperature_particle = (iparticle)->FastGetSolutionStepValue(TEMPERATURE);
                const TDataType& value = (iparticle)->FastGetSolutionStepValue(rMovingDomainVariable);

                for (std::size_t k = 0; k < geom.size(); k++)
                {
                    geom[k].SetLock();
                    geom[k].FastGetSolutionStepValue(rFixedDomainVariable) += N[k] * value;
                    geom[k].GetValue(YOUNG_MODULUS) += N[k];
                    geom[k].UnSetLock();

                }

            }

        }


        for (ModelPart::NodesContainerType::iterator node_it = rFixed_ModelPart.NodesBegin();
                node_it != rFixed_ModelPart.NodesEnd(); node_it++)
        {
            const double NN = (node_it)->GetValue(YOUNG_MODULUS);
            if (NN != 0.0)
            {
                (node_it)->FastGetSolutionStepValue(rFixedDomainVariable) /= NN;
            }
        }

        KRATOS_CATCH("")
    }

    // From moving to fixed model part
    /// Interpolate one variable from the moving mesh to the fixed one
    /**
      * @param rFixed_ModelPart: the model part  all the variable should be taken from
      * @param rMoving_ModelPart: the destination model part where we want to know the values of the variables
      * @param rFixedDomainVariable: the name of the interpolated variable in the origin model part
      * @param rMovingDomainVariable: the name of the interpolated variable in the destination model part
      * @param node_locator: precomputed bin of nodes of the fixed mesh. It is to be constructed separately @see binbased_nodes_in_element_locator
      */
    template<class TDataType>
    void MappingFromMovingMesh_VariableMeshes(
        ModelPart& rMoving_ModelPart ,
        ModelPart& rFixed_ModelPart,
        Variable<TDataType>& rMovingDomainVariable ,
        Variable<TDataType>& rFixedDomainVariable,
        BinBasedNodesInElementLocator<TDim>& node_locator //this is a bin of objects which contains the FIXED model part
    )
    {
        KRATOS_TRY

        KRATOS_WATCH("Transfer From Moving Mesh*************************************")
        if (rMoving_ModelPart.NodesBegin()->SolutionStepsDataHas(rMovingDomainVariable) == false)
            KRATOS_THROW_ERROR(std::logic_error, "Add  MovingDomain VARIABLE!!!!!! ERROR", "");
        if (rFixed_ModelPart.NodesBegin()->SolutionStepsDataHas(rFixedDomainVariable) == false)
            KRATOS_THROW_ERROR(std::logic_error, "Add  FixedDomain VARIABLE!!!!!! ERROR", "");


        //creating an auxiliary list for the new nodes
        for(ModelPart::NodesContainerType::iterator node_it = rFixed_ModelPart.NodesBegin();
                node_it != rFixed_ModelPart.NodesEnd(); ++node_it)
        {
            ClearVariables(node_it, rFixedDomainVariable);
        }

        //definitions for spatial search
        typedef typename BinBasedNodesInElementLocator<TDim>::PointVector PointVector;
        typedef typename BinBasedNodesInElementLocator<TDim>::DistanceVector DistanceVector;
        const std::size_t max_results = 5000;
        Matrix Nmat(max_results,TDim+1);
        boost::numeric::ublas::vector<int> positions(max_results);
        PointVector work_results(max_results);
        DistanceVector work_distances(max_results);
        Node work_point(0,0.0,0.0,0.0);
        for(ModelPart::ElementsContainerType::iterator elem_it = rMoving_ModelPart.ElementsBegin(); elem_it != rMoving_ModelPart.ElementsEnd(); ++elem_it)
        {
            std::size_t nfound = node_locator.FindNodesInElement(*(elem_it.base()), positions, Nmat, max_results, work_results.begin(), work_distances.begin(), work_point);
            for(std::size_t k=0; k<nfound; k++)
            {
                auto it = work_results.begin() + positions[k];


                array_1d<double,TDim+1> N = row(Nmat,k);
                Interpolate(  *(elem_it.base()),  N, *it, rMovingDomainVariable , rFixedDomainVariable);
            }

        }


        KRATOS_CATCH("")
    }

    ///@}
    ///@name Access
    ///@{


    ///@}
    ///@name Inquiry
    ///@{


    ///@}
    ///@name Input and output
    ///@{

    /// Turn back information as a stemplate<class T, std::size_t dim> string.
    virtual std::string Info() const
    {
        return "";
    }

    /// Print information about this object.
    virtual void PrintInfo(std::ostream& rOStream) const {}

    /// Print object's data.
    virtual void PrintData(std::ostream& rOStream) const {}


    ///@}
    ///@name Friends
    ///@{

    ///@}

protected:
    ///@name Protected static Member rVariables
    ///@{


    ///@}
    ///@name Protected member rVariables
    ///@{ template<class T, std::size_t dim>


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
    ///@name Static Member rVariables
    ///@{


    ///@}
    ///@name Member rVariables
    ///@{



    inline void CalculateCenterAndSearchRadius(GeometryType&geom,
            double& xc, double& yc, double& zc, double& R, array_1d<double,3>& N
                                              )
    {
        double x0 = geom[0].X();
        double  y0 = geom[0].Y();
        double x1 = geom[1].X();
        double  y1 = geom[1].Y();
        double x2 = geom[2].X();
        double  y2 = geom[2].Y();


        xc = 0.3333333333333333333*(x0+x1+x2);
        yc = 0.3333333333333333333*(y0+y1+y2);
        zc = 0.0;

        double R1 = (xc-x0)*(xc-x0) + (yc-y0)*(yc-y0);
        double R2 = (xc-x1)*(xc-x1) + (yc-y1)*(yc-y1);
        double R3 = (xc-x2)*(xc-x2) + (yc-y2)*(yc-y2);

        R = R1;
        if(R2 > R) R = R2;
        if(R3 > R) R = R3;

        R = 1.01 * sqrt(R);
    }
    //***************************************
    //***************************************
    inline void CalculateCenterAndSearchRadius(GeometryType&geom,
            double& xc, double& yc, double& zc, double& R, array_1d<double,4>& N

                                              )
    {
        double x0 = geom[0].X();
        double  y0 = geom[0].Y();
        double  z0 = geom[0].Z();
        double x1 = geom[1].X();
        double  y1 = geom[1].Y();
        double  z1 = geom[1].Z();
        double x2 = geom[2].X();
        double  y2 = geom[2].Y();
        double  z2 = geom[2].Z();
        double x3 = geom[3].X();
        double  y3 = geom[3].Y();
        double  z3 = geom[3].Z();


        xc = 0.25*(x0+x1+x2+x3);
        yc = 0.25*(y0+y1+y2+y3);
        zc = 0.25*(z0+z1+z2+z3);

        double R1 = (xc-x0)*(xc-x0) + (yc-y0)*(yc-y0) + (zc-z0)*(zc-z0);
        double R2 = (xc-x1)*(xc-x1) + (yc-y1)*(yc-y1) + (zc-z1)*(zc-z1);
        double R3 = (xc-x2)*(xc-x2) + (yc-y2)*(yc-y2) + (zc-z2)*(zc-z2);
        double R4 = (xc-x3)*(xc-x3) + (yc-y3)*(yc-y3) + (zc-z3)*(zc-z3);

        R = R1;
        if(R2 > R) R = R2;
        if(R3 > R) R = R3;
        if(R4 > R) R = R4;

        R = sqrt(R);
    }
    //***************************************
    //***************************************
    inline double CalculateVol(	const double x0, const double y0,
                                const double x1, const double y1,
                                const double x2, const double y2
                              )
    {
        return 0.5*( (x1-x0)*(y2-y0)- (y1-y0)*(x2-x0) );
    }
    //***************************************
    //***************************************
    inline double CalculateVol(	const double x0, const double y0, const double z0,
                                const double x1, const double y1, const double z1,
                                const double x2, const double y2, const double z2,
                                const double x3, const double y3, const double z3
                              )
    {
        double x10 = x1 - x0;
        double y10 = y1 - y0;
        double z10 = z1 - z0;

        double x20 = x2 - x0;
        double y20 = y2 - y0;
        double z20 = z2 - z0;

        double x30 = x3 - x0;
        double y30 = y3 - y0;
        double z30 = z3 - z0;

        double detJ = x10 * y20 * z30 - x10 * y30 * z20 + y10 * z20 * x30 - y10 * x20 * z30 + z10 * x20 * y30 - z10 * y20 * x30;
        return  detJ*0.1666666666666666666667;

        //return 0.5*( (x1-x0)*(y2-y0)- (y1-y0)*(x2-x0) );
    }
    //***************************************
    //***************************************
    inline bool CalculatePosition(	GeometryType&geom,
                                    const double xc, const double yc, const double zc,
                                    array_1d<double,3>& N
                                 )
    {
        double x0 = geom[0].X();
        double  y0 = geom[0].Y();
        double x1 = geom[1].X();
        double  y1 = geom[1].Y();
        double x2 = geom[2].X();
        double  y2 = geom[2].Y();

        double area = CalculateVol(x0,y0,x1,y1,x2,y2);
        double inv_area = 0.0;
        if(area == 0.0)
        {

// 				KRATOS_THROW_ERROR(std::logic_error,"element with zero area found","");
            //The interpolated node will not be inside an element with zero area
            return false;

        }
        else
        {
            inv_area = 1.0 / area;
        }


        N[0] = CalculateVol(x1,y1,x2,y2,xc,yc) * inv_area;
        N[1] = CalculateVol(x2,y2,x0,y0,xc,yc) * inv_area;
        N[2] = CalculateVol(x0,y0,x1,y1,xc,yc) * inv_area;


        if(N[0] >= 0.0 && N[1] >= 0.0 && N[2] >= 0.0 && N[0] <=1.0 && N[1]<= 1.0 && N[2] <= 1.0) //if the xc yc is inside the triangle return true
            return true;

        return false;
    }

    //***************************************
    //***************************************

    inline bool CalculatePosition(	GeometryType&geom,
                                    const double xc, const double yc, const double zc,
                                    array_1d<double,4>& N
                                 )
    {

        double x0 = geom[0].X();
        double  y0 = geom[0].Y();
        double  z0 = geom[0].Z();
        double x1 = geom[1].X();
        double  y1 = geom[1].Y();
        double  z1 = geom[1].Z();
        double x2 = geom[2].X();
        double  y2 = geom[2].Y();
        double  z2 = geom[2].Z();
        double x3 = geom[3].X();
        double  y3 = geom[3].Y();
        double  z3 = geom[3].Z();

        double vol = CalculateVol(x0,y0,z0,x1,y1,z1,x2,y2,z2,x3,y3,z3);

        double inv_vol = 0.0;
        if(vol < 0.0000000000001)
        {

// 				KRATOS_THROW_ERROR(std::logic_error,"element with zero vol found","");
            //The interpolated node will not be inside an element with zero volume
            return false;
// 				KRATOS_WATCH("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++")
        }
        else
        {
            inv_vol = 1.0 / vol;
        }

        N[0] = CalculateVol(x1,y1,z1,x3,y3,z3,x2,y2,z2,xc,yc,zc) * inv_vol;
        N[1] = CalculateVol(x3,y3,z3,x0,y0,z0,x2,y2,z2,xc,yc,zc) * inv_vol;
        N[2] = CalculateVol(x3,y3,z3,x1,y1,z1,x0,y0,z0,xc,yc,zc) * inv_vol;
        N[3] = CalculateVol(x0,y0,z0,x1,y1,z1,x2,y2,z2,xc,yc,zc) * inv_vol;


        if(N[0] >= 0.0 && N[1] >= 0.0 && N[2] >= 0.0 && N[3] >=0.0 &&
                N[0] <= 1.0 && N[1] <= 1.0 && N[2] <= 1.0 && N[3] <=1.0)
            //if the xc yc zc is inside the tetrahedron return true
            return true;

        return false;
    }

    //ElemI          	Element iterator
    //N			Shape functions
    //step_data_size
    //pnode			pointer to the node
    //projecting total model part 2Dversion
    void Interpolate(
        Element::Pointer ElemIt,
        const Vector& N,
        int step_data_size,
        NodeType::Pointer pnode)
    {
        //Geometry element of the rOrigin_ModelPart
        GeometryType& geom = ElemIt->GetGeometry();

        const std::size_t buffer_size = pnode->GetBufferSize();

        const std::size_t vector_size = N.size();

        for(std::size_t step = 0; step<buffer_size; step++) {
            //getting the data of the solution step
            double* step_data = (pnode)->SolutionStepData().Data(step);

            double* node0_data = geom[0].SolutionStepData().Data(step);

            //copying this data in the position of the vector we are interested in
            for(int j= 0; j< step_data_size; j++) {
                step_data[j] = N[0]*node0_data[j];
            }
            for(std::size_t k= 1; k< vector_size; k++) {
                double* node1_data = geom[k].SolutionStepData().Data(step);
                for(int j= 0; j< step_data_size; j++) {
                    step_data[j] += N[k]*node1_data[j];
                }
            }
        }
//         pnode->GetValue(IS_VISITED) = 1.0;

    }

    //projecting an array1D 2Dversion
    void Interpolate(
        Element::Pointer ElemIt,
        const Vector& N,
        NodeType::Pointer pnode,
        Variable<array_1d<double,3> >& rOriginVariable,
        Variable<array_1d<double,3> >& rDestinationVariable)
    {
        //Geometry element of the rOrigin_ModelPart
        GeometryType& geom = ElemIt->GetGeometry();

        const std::size_t buffer_size = pnode->GetBufferSize();

        const std::size_t vector_size = N.size();

        for(std::size_t step = 0; step<buffer_size; step++) {
            //getting the data of the solution step
            array_1d<double,3>& step_data = (pnode)->FastGetSolutionStepValue(rDestinationVariable , step);
            //Reference or no reference???//CANCELLA
            step_data = N[0] * geom[0].FastGetSolutionStepValue(rOriginVariable , step);

            // Copying this data in the position of the vector we are interested in
            for(std::size_t j= 1; j< vector_size; j++) {
                const array_1d<double,3>& node_data = geom[j].FastGetSolutionStepValue(rOriginVariable , step);
                step_data += N[j] * node_data;
            }
        }
//         pnode->GetValue(IS_VISITED) = 1.0;
    }

    //projecting a scalar 2Dversion
    void Interpolate(
        Element::Pointer ElemIt,
        const Vector& N,
        NodeType::Pointer pnode,
        Variable<double>& rOriginVariable,
        Variable<double>& rDestinationVariable)
    {
        //Geometry element of the rOrigin_ModelPart
        GeometryType& geom = ElemIt->GetGeometry();

        const std::size_t buffer_size = pnode->GetBufferSize();

        const std::size_t vector_size = N.size();

        //facendo un loop sugli step temporali step_data come salva i dati al passo anteriore? Cioś dove passiamo l'informazione ai nodi???
        for(std::size_t step = 0; step<buffer_size; step++) {
            //getting the data of the solution step
            double& step_data = (pnode)->FastGetSolutionStepValue(rDestinationVariable , step);
            //Reference or no reference???//CANCELLA

            //copying this data in the position of the vector we are interested in
            step_data = N[0] * geom[0].FastGetSolutionStepValue(rOriginVariable , step);

            // Copying this data in the position of the vector we are interested in
            for(std::size_t j= 1; j< vector_size; j++) {
                const double node_data = geom[j].FastGetSolutionStepValue(rOriginVariable , step);
                step_data += N[j] * node_data;
            }

        }
//         pnode->GetValue(IS_VISITED) = 1.0;

    }

    inline void Clear(ModelPart::NodesContainerType::iterator node_it,  int step_data_size )
    {
        std::size_t buffer_size = node_it->GetBufferSize();

        for(std::size_t step = 0; step<buffer_size; step++)
        {
            //getting the data of the solution step
            double* step_data = (node_it)->SolutionStepData().Data(step);

            //copying this data in the position of the vector we are interested in
            for(int j= 0; j< step_data_size; j++)
            {
                step_data[j] = 0.0;
            }
        }

    }

    inline void ClearVariables(ModelPart::NodesContainerType::iterator node_it , Variable<array_1d<double,3> >& rVariable)
    {
        array_1d<double, 3>& Aux_var = node_it->FastGetSolutionStepValue(rVariable, 0);

        noalias(Aux_var) = ZeroVector(3);

    }

    inline void ClearVariables(ModelPart::NodesContainerType::iterator node_it,  Variable<double>& rVariable)
    {
        double& Aux_var = node_it->FastGetSolutionStepValue(rVariable, 0);

        Aux_var = 0.0;

    }

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
    BinBasedMeshTransfer& operator=(BinBasedMeshTransfer const& rOther);


    ///@}

}; // Class BinBasedMeshTransfer

///@}

///@name Type Definitions
///@{


///@}
///@name Input and output
///@{




/// output stream function
template<std::size_t TDim>
inline std::ostream& operator << (std::ostream& rOStream,
                                  const BinBasedMeshTransfer<TDim>& rThis)
{
    rThis.PrintInfo(rOStream);
    rOStream << std::endl;
    rThis.PrintData(rOStream);

    return rOStream;
}
///@}


}  // namespace Kratos.

#endif // KRATOS_BINBASED_PROJECTION  defined
