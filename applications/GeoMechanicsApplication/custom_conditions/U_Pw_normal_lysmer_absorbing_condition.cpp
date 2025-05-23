// KRATOS___
//     //   ) )
//    //         ___      ___
//   //  ____  //___) ) //   ) )
//  //    / / //       //   / /
// ((____/ / ((____   ((___/ /  MECHANICS
//
//  License:         geo_mechanics_application/license.txt
//
//
//  Main authors:    Aron Noordam
//

// Application includes
#include "custom_conditions/U_Pw_normal_lysmer_absorbing_condition.hpp"
#include "custom_utilities/condition_utilities.hpp"
#include "custom_utilities/dof_utilities.h"
#include "custom_utilities/linear_nodal_extrapolator.h"

namespace Kratos
{

template <unsigned int TDim, unsigned int TNumNodes>
Condition::Pointer UPwLysmerAbsorbingCondition<TDim, TNumNodes>::Create(IndexType NewId,
                                                                        NodesArrayType const& ThisNodes,
                                                                        PropertiesType::Pointer pProperties) const
{
    return Condition::Pointer(
        new UPwLysmerAbsorbingCondition(NewId, this->GetGeometry().Create(ThisNodes), pProperties));
}

template <unsigned int TDim, unsigned int TNumNodes>
void UPwLysmerAbsorbingCondition<TDim, TNumNodes>::CalculateLeftHandSide(Matrix& rLeftHandSideMatrix,
                                                                         const ProcessInfo& rCurrentProcessInfo)
{
    ElementMatrixType stiffness_matrix;

    this->CalculateConditionStiffnessMatrix(stiffness_matrix, rCurrentProcessInfo);

    this->AddLHS(rLeftHandSideMatrix, stiffness_matrix);
}

template <unsigned int TDim, unsigned int TNumNodes>
void UPwLysmerAbsorbingCondition<TDim, TNumNodes>::CalculateLocalSystem(Matrix& rLhsMatrix,
                                                                        Vector& rRightHandSideVector,
                                                                        const ProcessInfo& rCurrentProcessInfo)
{
    this->CalculateLeftHandSide(rLhsMatrix, rCurrentProcessInfo);

    this->CalculateAndAddRHS(rRightHandSideVector, rLhsMatrix);
}

template <unsigned int TDim, unsigned int TNumNodes>
void UPwLysmerAbsorbingCondition<TDim, TNumNodes>::CalculateConditionStiffnessMatrix(ElementMatrixType& rStiffnessMatrix,
                                                                                     const ProcessInfo& rCurrentProcessInfo)
{
    GeometryType& r_geom = this->GetGeometry();

    GeometryData::IntegrationMethod integration_method = this->GetIntegrationMethod();
    const GeometryType::IntegrationPointsArrayType& r_integration_points =
        r_geom.IntegrationPoints(integration_method);
    const unsigned int num_g_points = r_integration_points.size();
    const unsigned int local_dim    = r_geom.LocalSpaceDimension();

    // Containers of variables at all integration points
    const Matrix&               r_n_container = r_geom.ShapeFunctionsValues(integration_method);
    GeometryType::JacobiansType jacobians(num_g_points);
    for (unsigned int i = 0; i < num_g_points; ++i)
        jacobians[i].resize(TDim, local_dim, false);
    r_geom.Jacobian(jacobians, integration_method);

    // Condition variables
    BoundedMatrix<double, TDim, N_DOF> nu_matrix = ZeroMatrix(TDim, N_DOF);

    NormalLysmerAbsorbingVariables absorbing_variables;

    this->GetVariables(absorbing_variables, rCurrentProcessInfo);

    BoundedMatrix<double, TDim, N_DOF> aux_abs_k_matrix;
    rStiffnessMatrix = ZeroMatrix(N_DOF, N_DOF);

    for (unsigned int g_point = 0; g_point < num_g_points; ++g_point) {
        // calculate
        absorbing_variables.Ec = 0.0;
        absorbing_variables.G  = 0.0;
        for (unsigned int node = 0; node < r_geom.size(); ++node) {
            absorbing_variables.Ec += r_n_container(g_point, node) * absorbing_variables.EcNodes[node];
            absorbing_variables.G += r_n_container(g_point, node) * absorbing_variables.GNodes[node];
        }

        this->CalculateNodalStiffnessMatrix(absorbing_variables, r_geom);

        // calculate displacement shape function matrix
        GeoElementUtilities::CalculateNuMatrix<TDim, TNumNodes>(nu_matrix, r_n_container, g_point);

        // Compute weighting coefficient for integration
        double integration_coefficient = ConditionUtilities::CalculateIntegrationCoefficient(
            jacobians[g_point], r_integration_points[g_point].Weight());

        // set stiffness part of absorbing matrix
        aux_abs_k_matrix = prod(absorbing_variables.KAbsMatrix, nu_matrix);
        rStiffnessMatrix += prod(trans(nu_matrix), aux_abs_k_matrix) * integration_coefficient;
    }
}

template <unsigned int TDim, unsigned int TNumNodes>
void UPwLysmerAbsorbingCondition<TDim, TNumNodes>::CalculateRightHandSide(Vector& rRightHandSideVector,
                                                                          const ProcessInfo& rCurrentProcessInfo)
{
    ElementMatrixType stiffness_matrix;

    this->CalculateConditionStiffnessMatrix(stiffness_matrix, rCurrentProcessInfo);

    Matrix global_stiffness_matrix = ZeroMatrix(CONDITION_SIZE, CONDITION_SIZE);
    GeoElementUtilities::AssembleUUBlockMatrix(global_stiffness_matrix, stiffness_matrix);

    this->CalculateAndAddRHS(rRightHandSideVector, global_stiffness_matrix);
}

template <unsigned int TDim, unsigned int TNumNodes>
void UPwLysmerAbsorbingCondition<TDim, TNumNodes>::CalculateDampingMatrix(Matrix& rDampingMatrix,
                                                                          const ProcessInfo& rCurrentProcessInfo)
{
    GeometryType& r_geom = this->GetGeometry();

    GeometryData::IntegrationMethod r_integration_method = this->GetIntegrationMethod();
    const GeometryType::IntegrationPointsArrayType& r_integration_points =
        r_geom.IntegrationPoints(r_integration_method);
    const unsigned int num_g_points = r_integration_points.size();
    const unsigned int local_dim    = r_geom.LocalSpaceDimension();

    // Containers of variables at all integration points
    const Matrix&               r_n_container = r_geom.ShapeFunctionsValues(r_integration_method);
    GeometryType::JacobiansType jacobians(num_g_points);
    for (unsigned int i = 0; i < num_g_points; ++i)
        (jacobians[i]).resize(TDim, local_dim, false);
    r_geom.Jacobian(jacobians, r_integration_method);

    // Condition variables
    BoundedMatrix<double, TDim, N_DOF> nu_matrix = ZeroMatrix(TDim, N_DOF);

    NormalLysmerAbsorbingVariables absorbing_variables;
    this->GetVariables(absorbing_variables, rCurrentProcessInfo);

    BoundedMatrix<double, TDim, N_DOF> aux_abs_matrix;
    ElementMatrixType                  abs_matrix = ZeroMatrix(N_DOF, N_DOF);

    for (unsigned int g_point = 0; g_point < num_g_points; ++g_point) {
        // calculate
        absorbing_variables.rho = 0.0;
        absorbing_variables.Ec  = 0.0;
        absorbing_variables.G   = 0.0;
        for (unsigned int node = 0; node < r_geom.size(); ++node) {
            absorbing_variables.rho += r_n_container(g_point, node) * absorbing_variables.rhoNodes[node];
            absorbing_variables.Ec += r_n_container(g_point, node) * absorbing_variables.EcNodes[node];
            absorbing_variables.G += r_n_container(g_point, node) * absorbing_variables.GNodes[node];
        }
        absorbing_variables.vp = sqrt(absorbing_variables.Ec / absorbing_variables.rho);
        absorbing_variables.vs = sqrt(absorbing_variables.G / absorbing_variables.rho);

        this->CalculateNodalDampingMatrix(absorbing_variables, r_geom);

        // calculate displacement shape function matrix
        GeoElementUtilities::CalculateNuMatrix<TDim, TNumNodes>(nu_matrix, r_n_container, g_point);

        // Compute weighting coefficient for integration
        double integration_coefficient = ConditionUtilities::CalculateIntegrationCoefficient(
            jacobians[g_point], r_integration_points[g_point].Weight());

        // set damping part of absorbing matrix
        aux_abs_matrix = prod(absorbing_variables.CAbsMatrix, nu_matrix);
        abs_matrix += prod(trans(nu_matrix), aux_abs_matrix) * integration_coefficient;
    }

    // assemble left hand side vector
    this->AddLHS(rDampingMatrix, abs_matrix);
}

template <unsigned int TDim, unsigned int TNumNodes>
void UPwLysmerAbsorbingCondition<TDim, TNumNodes>::GetValuesVector(Vector& rValues, int Step) const
{
    rValues = Geo::DofUtilities::ExtractSolutionStepValuesOfUPwDofs(this->GetDofs(), Step);
}

template <unsigned int TDim, unsigned int TNumNodes>
void UPwLysmerAbsorbingCondition<TDim, TNumNodes>::GetFirstDerivativesVector(Vector& rValues, int Step) const
{
    rValues = Geo::DofUtilities::ExtractFirstTimeDerivativesOfUPwDofs(this->GetDofs(), Step);
}

template <unsigned int TDim, unsigned int TNumNodes>
void UPwLysmerAbsorbingCondition<TDim, TNumNodes>::CalculateNodalDampingMatrix(
    NormalLysmerAbsorbingVariables& rVariables, const Element::GeometryType& rGeom)
{
    array_1d<double, 2> damping_constants;

    // calculate rotation matrix
    DimensionMatrixType rotation_matrix;
    CalculateRotationMatrix(rotation_matrix, rGeom);

    const int local_perpendicular_direction = TDim - 1;

    // calculate constant traction vector part
    damping_constants[0] = rVariables.vs * rVariables.rho * rVariables.s_factor;
    damping_constants[1] = rVariables.vp * rVariables.rho * rVariables.p_factor;

    DimensionMatrixType local_c_matrix     = ZeroMatrix(TDim, TDim);
    DimensionMatrixType aux_local_c_matrix = ZeroMatrix(TDim, TDim);

    rVariables.CAbsMatrix = ZeroMatrix(TDim, TDim);

    for (unsigned int idim = 0; idim < TDim; ++idim) {
        local_c_matrix(idim, idim) = damping_constants[0];
    }
    local_c_matrix(local_perpendicular_direction, local_perpendicular_direction) = damping_constants[1];

    aux_local_c_matrix    = prod(local_c_matrix, rotation_matrix);
    rVariables.CAbsMatrix = prod(trans(rotation_matrix), aux_local_c_matrix);

    for (unsigned int idim = 0; idim < TDim; ++idim) {
        rVariables.CAbsMatrix(idim, idim) = std::abs(rVariables.CAbsMatrix(idim, idim));
    }
}

template <unsigned int TDim, unsigned int TNumNodes>
void UPwLysmerAbsorbingCondition<TDim, TNumNodes>::CalculateNodalStiffnessMatrix(
    NormalLysmerAbsorbingVariables& rVariables, const Element::GeometryType& rGeom)
{
    array_1d<double, 2> stiffness_constants;

    // calculate rotation matrix
    DimensionMatrixType rotation_matrix;
    CalculateRotationMatrix(rotation_matrix, rGeom);

    const int local_perpendicular_direction = TDim - 1;

    // calculate constant traction vector part
    stiffness_constants[0] = rVariables.G / rVariables.virtual_thickness;
    stiffness_constants[1] = rVariables.Ec / rVariables.virtual_thickness;

    DimensionMatrixType local_k_matrix     = ZeroMatrix(TDim, TDim);
    DimensionMatrixType aux_local_k_matrix = ZeroMatrix(TDim, TDim);

    rVariables.KAbsMatrix = ZeroMatrix(TDim, TDim);

    for (unsigned int idim = 0; idim < TDim; ++idim) {
        local_k_matrix(idim, idim) = stiffness_constants[0];
    }
    local_k_matrix(local_perpendicular_direction, local_perpendicular_direction) = stiffness_constants[1];

    aux_local_k_matrix    = prod(local_k_matrix, rotation_matrix);
    rVariables.KAbsMatrix = prod(trans(rotation_matrix), aux_local_k_matrix);

    for (unsigned int idim = 0; idim < TDim; ++idim) {
        rVariables.KAbsMatrix(idim, idim) = std::abs(rVariables.KAbsMatrix(idim, idim));
    }
}

template <unsigned int TDim, unsigned int TNumNodes>
Matrix UPwLysmerAbsorbingCondition<TDim, TNumNodes>::CalculateExtrapolationMatrixNeighbour(const Element& rNeighbourElement)
{
    LinearNodalExtrapolator extrapolator;
    return extrapolator.CalculateElementExtrapolationMatrix(
        rNeighbourElement.GetGeometry(), rNeighbourElement.GetIntegrationMethod());
}

template <unsigned int TDim, unsigned int TNumNodes>
void UPwLysmerAbsorbingCondition<TDim, TNumNodes>::GetNeighbourElementVariables(
    NormalLysmerAbsorbingVariables& rVariables, const ProcessInfo& rCurrentProcessInfo)
{
    // get neighbour elements
    auto          neighbour_elements = this->GetValue(NEIGHBOUR_ELEMENTS);
    GeometryType& r_geom             = this->GetGeometry();

    if (neighbour_elements.size() == 0) {
        KRATOS_ERROR << "Condition: " << this->Id() << " does not have neighbour elements" << std::endl;
    }

    rVariables.EcNodes.resize(TNumNodes);
    rVariables.GNodes.resize(TNumNodes);
    rVariables.rhoNodes.resize(TNumNodes);

    // only get values from first neighbour
    Element& r_neighbour_element = neighbour_elements[0];
    auto     prop_neighbour      = r_neighbour_element.GetProperties();

    const GeometryType&                   r_neighbour_geom = r_neighbour_element.GetGeometry();
    const GeometryData::IntegrationMethod rIntegrationMethodNeighbour =
        r_neighbour_element.GetIntegrationMethod();
    const IndexType num_nodes_neighbour = r_neighbour_geom.size();
    const IndexType num_g_points_neighbour =
        r_neighbour_geom.IntegrationPointsNumber(rIntegrationMethodNeighbour);

    // get density and porosity from element
    std::vector<double> saturation_std_vector;
    Vector              density_vector(num_g_points_neighbour);
    Vector              confined_stiffness_vector(num_g_points_neighbour);
    Vector              shear_stiffness_vector(num_g_points_neighbour);

    std::vector<double> confined_stiffness_std_vector(num_g_points_neighbour);
    std::vector<double> shear_stiffness_std_vector(num_g_points_neighbour);

    // get parameters at neighbour element integration points
    r_neighbour_element.CalculateOnIntegrationPoints(DEGREE_OF_SATURATION, saturation_std_vector, rCurrentProcessInfo);
    r_neighbour_element.CalculateOnIntegrationPoints(
        CONFINED_STIFFNESS, confined_stiffness_std_vector, rCurrentProcessInfo);
    r_neighbour_element.CalculateOnIntegrationPoints(SHEAR_STIFFNESS, shear_stiffness_std_vector, rCurrentProcessInfo);

    for (unsigned int g_point = 0; g_point < num_g_points_neighbour; ++g_point) {
        // transform std vectors to boost vectors
        confined_stiffness_vector[g_point] = confined_stiffness_std_vector[g_point];
        shear_stiffness_vector[g_point]    = shear_stiffness_std_vector[g_point];

        // calculate density mixture
        density_vector[g_point] =
            (saturation_std_vector[g_point] * prop_neighbour[POROSITY] * prop_neighbour[DENSITY_WATER]) +
            (1.0 - prop_neighbour[POROSITY]) * prop_neighbour[DENSITY_SOLID];
    }

    Matrix extrapolation_matrix = CalculateExtrapolationMatrixNeighbour(r_neighbour_element);

    // project parameters on neighbour nodes
    Vector Ec_nodes_neighbour  = prod(extrapolation_matrix, confined_stiffness_vector);
    Vector G_nodes_neighbour   = prod(extrapolation_matrix, shear_stiffness_vector);
    Vector rho_nodes_neighbour = prod(extrapolation_matrix, density_vector);

    // add parameters to condition nodes
    for (unsigned int k = 0; k < num_nodes_neighbour; ++k) {
        for (unsigned int node = 0; node < TNumNodes; ++node) {
            // add parameter if neighbour node id and condition node id are the same
            if (r_neighbour_geom[k].Id() == r_geom[node].Id()) {
                rVariables.EcNodes[node]  = Ec_nodes_neighbour(k);
                rVariables.GNodes[node]   = G_nodes_neighbour(k);
                rVariables.rhoNodes[node] = rho_nodes_neighbour(k);
            }
        }
    }
}

template <unsigned int TDim, unsigned int TNumNodes>
void UPwLysmerAbsorbingCondition<TDim, TNumNodes>::GetVariables(NormalLysmerAbsorbingVariables& rVariables,
                                                                const ProcessInfo& rCurrentProcessInfo)
{
    // gets variables from neighbour elements
    this->GetNeighbourElementVariables(rVariables, rCurrentProcessInfo);

    // get condition specific variables
    Vector absorbing_factors     = this->GetValue(ABSORBING_FACTORS);
    rVariables.p_factor          = absorbing_factors(0);
    rVariables.s_factor          = absorbing_factors(1);
    rVariables.virtual_thickness = this->GetValue(VIRTUAL_THICKNESS);
}

template <unsigned int TDim, unsigned int TNumNodes>
void UPwLysmerAbsorbingCondition<TDim, TNumNodes>::CalculateAndAddRHS(Vector& rRightHandSideVector,
                                                                      const Matrix& rStiffnessMatrix)
{
    rRightHandSideVector = ZeroVector(CONDITION_SIZE);

    Vector displacements_vector = ZeroVector(CONDITION_SIZE);
    this->GetValuesVector(displacements_vector, 0);

    rRightHandSideVector -= prod(rStiffnessMatrix, displacements_vector);
}

template <unsigned int TDim, unsigned int TNumNodes>
void UPwLysmerAbsorbingCondition<TDim, TNumNodes>::AddLHS(Matrix& rLeftHandSideMatrix,
                                                          const ElementMatrixType& rUUMatrix)
{
    // assemble left hand side vector
    rLeftHandSideMatrix = ZeroMatrix(CONDITION_SIZE, CONDITION_SIZE);

    // Adding contribution to left hand side
    GeoElementUtilities::AssembleUUBlockMatrix(rLeftHandSideMatrix, rUUMatrix);
}

template <unsigned int TDim, unsigned int TNumNodes>
void UPwLysmerAbsorbingCondition<TDim, TNumNodes>::CalculateRotationMatrix2DLine(DimensionMatrixType& rRotationMatrix,
                                                                                 const Element::GeometryType& rGeom)
{
    // Unitary vector in local x direction
    array_1d<double, 3> v_x;
    noalias(v_x)        = rGeom.GetPoint(1) - rGeom.GetPoint(0);
    const double norm_x = norm_2(v_x);

    v_x[0] *= 1.0 / norm_x;
    v_x[1] *= 1.0 / norm_x;

    // Rotation Matrix
    rRotationMatrix(0, 0) = v_x[0];
    rRotationMatrix(0, 1) = v_x[1];

    // We need to determine the unitary vector in local y direction pointing towards the TOP face of the joint

    // Unitary vector in local x direction (3D)
    array_1d<double, 3> v_x_3d;
    v_x_3d[0] = v_x[0];
    v_x_3d[1] = v_x[1];
    v_x_3d[2] = 0.0;

    // Unitary vector in local y direction (first option)
    array_1d<double, 3> v_y_3d;
    v_y_3d[0] = -v_x[1];
    v_y_3d[1] = v_x[0];
    v_y_3d[2] = 0.0;

    // Vector in global z direction (first option)
    array_1d<double, 3> v_z;
    MathUtils<double>::CrossProduct(v_z, v_x_3d, v_y_3d);

    // v_z must have the same sign as vector (0,0,1)
    if (v_z[2] > 0.0) {
        rRotationMatrix(1, 0) = -v_x[1];
        rRotationMatrix(1, 1) = v_x[0];
    } else {
        rRotationMatrix(1, 0) = v_x[1];
        rRotationMatrix(1, 1) = -v_x[0];
    }
}

template <unsigned int TDim, unsigned int TNumNodes>
void UPwLysmerAbsorbingCondition<TDim, TNumNodes>::CalculateRotationMatrix3DTriangle(
    DimensionMatrixType& rRotationMatrix, const Element::GeometryType& rGeom)
{
    // triangle_3d_3
    array_1d<double, 3> p_mid_0;
    array_1d<double, 3> p_mid_1;
    noalias(p_mid_0) = 0.5 * (rGeom.GetPoint(0) + rGeom.GetPoint(1));
    noalias(p_mid_1) = 0.5 * (rGeom.GetPoint(0) + rGeom.GetPoint(2));

    // Unitary vector in local x direction
    array_1d<double, 3> v_x;
    noalias(v_x)            = rGeom.GetPoint(1) - rGeom.GetPoint(0);
    const double inv_norm_x = 1.0 / norm_2(v_x);
    v_x[0] *= inv_norm_x;
    v_x[1] *= inv_norm_x;
    v_x[2] *= inv_norm_x;

    // Unitary vector in local z direction
    array_1d<double, 3> v_y;
    noalias(v_y) = rGeom.GetPoint(2) - rGeom.GetPoint(0);

    array_1d<double, 3> v_z;
    MathUtils<double>::CrossProduct(v_z, v_x, v_y);
    const double norm_z = norm_2(v_z);

    v_z[0] *= 1.0 / norm_z;
    v_z[1] *= 1.0 / norm_z;
    v_z[2] *= 1.0 / norm_z;

    // Unitary vector in local y direction
    MathUtils<double>::CrossProduct(v_y, v_z, v_x);

    // Rotation Matrix
    rRotationMatrix(0, 0) = v_x[0];
    rRotationMatrix(0, 1) = v_x[1];
    rRotationMatrix(0, 2) = v_x[2];

    rRotationMatrix(1, 0) = v_y[0];
    rRotationMatrix(1, 1) = v_y[1];
    rRotationMatrix(1, 2) = v_y[2];

    rRotationMatrix(2, 0) = v_z[0];
    rRotationMatrix(2, 1) = v_z[1];
    rRotationMatrix(2, 2) = v_z[2];
}

template <unsigned int TDim, unsigned int TNumNodes>
void UPwLysmerAbsorbingCondition<TDim, TNumNodes>::CalculateRotationMatrix(
    BoundedMatrix<double, TDim, TDim>& rRotationMatrix, const Element::GeometryType& rGeom)
{
    const auto geometry_family = this->GetGeometry().GetGeometryFamily();

    if constexpr (TDim == 2) {
        if (geometry_family == GeometryData::KratosGeometryFamily::Kratos_Linear) {
            CalculateRotationMatrix2DLine(rRotationMatrix, rGeom);
        } else {
            KRATOS_ERROR << "Rotation matrix for geometry type: " << rGeom.Name()
                         << " is not implemented." << std::endl;
        }
    } else {
        if (geometry_family == GeometryData::KratosGeometryFamily::Kratos_Triangle) {
            CalculateRotationMatrix3DTriangle(rRotationMatrix, rGeom);
        } else if (geometry_family == GeometryData::KratosGeometryFamily::Kratos_Quadrilateral) {
            CalculateRotationMatrix3DQuad(rRotationMatrix, rGeom);
        } else {
            KRATOS_ERROR << "Rotation matrix for geometry type: " << rGeom.Name()
                         << " is not implemented." << std::endl;
        }
    }
}

template <unsigned int TDim, unsigned int TNumNodes>
void UPwLysmerAbsorbingCondition<TDim, TNumNodes>::CalculateRotationMatrix3DQuad(DimensionMatrixType& rRotationMatrix,
                                                                                 const Element::GeometryType& rGeom)
{
    // Quadrilateral_3d_4
    array_1d<double, 3>        p_mid_0;
    array_1d<double, 3>        p_mid_1;
    const array_1d<double, 3>& r_p_2 = rGeom.GetPoint(2);
    noalias(p_mid_0)                 = 0.5 * (rGeom.GetPoint(0) + rGeom.GetPoint(3));
    noalias(p_mid_1)                 = 0.5 * (rGeom.GetPoint(1) + r_p_2);

    // Unitary vector in local x direction
    array_1d<double, 3> v_x;
    noalias(v_x)            = p_mid_1 - p_mid_0;
    const double inv_norm_x = 1.0 / norm_2(v_x);
    v_x[0] *= inv_norm_x;
    v_x[1] *= inv_norm_x;
    v_x[2] *= inv_norm_x;

    // Unitary vector in local z direction
    array_1d<double, 3> v_y;
    noalias(v_y) = r_p_2 - p_mid_0;
    array_1d<double, 3> v_z;
    MathUtils<double>::CrossProduct(v_z, v_x, v_y);
    const double norm_z = norm_2(v_z);

    v_z[0] *= 1.0 / norm_z;
    v_z[1] *= 1.0 / norm_z;
    v_z[2] *= 1.0 / norm_z;

    // Unitary vector in local y direction
    MathUtils<double>::CrossProduct(v_y, v_z, v_x);

    // Rotation Matrix
    rRotationMatrix(0, 0) = v_x[0];
    rRotationMatrix(0, 1) = v_x[1];
    rRotationMatrix(0, 2) = v_x[2];

    rRotationMatrix(1, 0) = v_y[0];
    rRotationMatrix(1, 1) = v_y[1];
    rRotationMatrix(1, 2) = v_y[2];

    rRotationMatrix(2, 0) = v_z[0];
    rRotationMatrix(2, 1) = v_z[1];
    rRotationMatrix(2, 2) = v_z[2];
}

template <unsigned int TDim, unsigned int TNumNodes>
std::string UPwLysmerAbsorbingCondition<TDim, TNumNodes>::Info() const
{
    return "UPwLysmerAbsorbingCondition";
}

// 2 noded line
template class UPwLysmerAbsorbingCondition<2, 2>;

// 3 noded line
template class UPwLysmerAbsorbingCondition<2, 3>;

// first order triangle, quad
template class UPwLysmerAbsorbingCondition<3, 3>;
template class UPwLysmerAbsorbingCondition<3, 4>;

// second order triangle, quad
template class UPwLysmerAbsorbingCondition<3, 6>;
template class UPwLysmerAbsorbingCondition<3, 8>;

} // Namespace Kratos.
