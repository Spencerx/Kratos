//
// Authors:
// Miguel Angel Celigueta maceli@cimne.upc.edu
// Salvador Latorre latorre@cimne.upc.edu
// Miquel Santasusana msantasusana@cimne.upc.edu
// Guillermo Casas gcasas@cimne.upc.edu
// Chengshun Shang cshang@cimne.upc.edu
//

// System includes
#include <string>
#include <iostream>
#include <cmath>
#include <map>

#include <fstream>

// External includes

// Project includes
#include "spheric_particle.h"
#include "custom_utilities/GeometryFunctions.h"
#include "custom_utilities/AuxiliaryFunctions.h"
#include "custom_utilities/discrete_particle_configure.h"
#include "custom_strategies/schemes/glued_to_wall_scheme.h"


namespace Kratos
{
// using namespace GeometryFunctions;

SphericParticle::SphericParticle()
    : DiscreteElement(), mRealMass(0)
{
    mRadius = 0;
    mRealMass = 0;
    mStressTensor = NULL;
    mStrainTensor = NULL;
    mDifferentialStrainTensor = NULL;
    mSymmStressTensor = NULL;
    mpTranslationalIntegrationScheme = NULL;
    mpRotationalIntegrationScheme = NULL;
    mpInlet = NULL;
}

SphericParticle::SphericParticle(IndexType NewId, GeometryType::Pointer pGeometry)
    : DiscreteElement(NewId, pGeometry), mRealMass(0){
    mRadius = 0;
    mRealMass = 0;
    mStressTensor = NULL;
    mStrainTensor = NULL;
    mDifferentialStrainTensor = NULL;
    mSymmStressTensor = NULL;
    mpTranslationalIntegrationScheme = NULL;
    mpRotationalIntegrationScheme = NULL;
    mpInlet = NULL;
}

SphericParticle::SphericParticle(IndexType NewId, GeometryType::Pointer pGeometry,  PropertiesType::Pointer pProperties)
    : DiscreteElement(NewId, pGeometry, pProperties), mRealMass(0)
{
    mRadius = 0.0;
    mRealMass = 0.0;
    mStressTensor = NULL;
    mStrainTensor = NULL;
    mDifferentialStrainTensor = NULL;
    mSymmStressTensor = NULL;
    mpTranslationalIntegrationScheme = NULL;
    mpRotationalIntegrationScheme = NULL;
    mpInlet = NULL;
}

SphericParticle::SphericParticle(IndexType NewId, NodesArrayType const& ThisNodes)
    : DiscreteElement(NewId, ThisNodes), mRealMass(0)
{
    mRadius = 0;
    mRealMass = 0;
    mStressTensor = NULL;
    mStrainTensor = NULL;
    mDifferentialStrainTensor = NULL;
    mSymmStressTensor = NULL;
    mpTranslationalIntegrationScheme = NULL;
    mpRotationalIntegrationScheme = NULL;
    mpInlet = NULL;
}

Element::Pointer SphericParticle::Create(IndexType NewId, NodesArrayType const& ThisNodes, PropertiesType::Pointer pProperties) const
{
    return Element::Pointer(new SphericParticle(NewId, GetGeometry().Create(ThisNodes), pProperties));
}

/// Destructor.
SphericParticle::~SphericParticle(){
    if (mStressTensor!=NULL) {
        delete mStressTensor;
        mStressTensor = NULL;
        delete mSymmStressTensor;
        mSymmStressTensor = NULL;
    }
    if (mStrainTensor) {
        delete mStrainTensor;
        mStrainTensor = NULL;
    }
    if (mDifferentialStrainTensor) {
        delete mDifferentialStrainTensor;
        mDifferentialStrainTensor = NULL;
    }
    if (mpTranslationalIntegrationScheme!=NULL) {
        if(mpTranslationalIntegrationScheme != mpRotationalIntegrationScheme) delete mpTranslationalIntegrationScheme;
        mpTranslationalIntegrationScheme = NULL;
    }
    if (mpRotationalIntegrationScheme!=NULL) {
        delete mpRotationalIntegrationScheme;
        mpRotationalIntegrationScheme = NULL;
    }
}

SphericParticle& SphericParticle::operator=(const SphericParticle& rOther) {
    DiscreteElement::operator=(rOther);
    mElasticEnergy = rOther.mElasticEnergy;
    mMaxNormalBallToBallForceTimesRadius = rOther.mMaxNormalBallToBallForceTimesRadius;
    mInelasticFrictionalEnergy = rOther.mInelasticFrictionalEnergy;
    mInelasticViscodampingEnergy = rOther.mInelasticViscodampingEnergy;
    mInelasticRollingResistanceEnergy = rOther.mInelasticRollingResistanceEnergy;
    mNeighbourElements = rOther.mNeighbourElements;
    mContactingNeighbourIds = rOther.mContactingNeighbourIds;
    mContactingFaceNeighbourIds = rOther.mContactingFaceNeighbourIds;
    mNeighbourRigidFaces = rOther.mNeighbourRigidFaces;
    mNeighbourNonContactRigidFaces = rOther.mNeighbourNonContactRigidFaces;
    mNeighbourPotentialRigidFaces = rOther.mNeighbourPotentialRigidFaces;
    mContactConditionWeights = rOther.mContactConditionWeights;
    mContactConditionContactTypes = rOther.mContactConditionContactTypes;
    mConditionContactPoints = rOther.mConditionContactPoints;
    mNeighbourRigidFacesTotalContactForce = rOther.mNeighbourRigidFacesTotalContactForce;
    mNeighbourRigidFacesElasticContactForce = rOther.mNeighbourRigidFacesElasticContactForce;
    mNeighbourElasticContactForces = rOther.mNeighbourElasticContactForces;
    mNeighbourElasticExtraContactForces = rOther.mNeighbourElasticExtraContactForces;
    mContactMoment = rOther.mContactMoment;
    mPartialRepresentativeVolume = rOther.mPartialRepresentativeVolume; //TODO: to continuum!
    mFemOldNeighbourIds = rOther.mFemOldNeighbourIds;
    mRadius = rOther.mRadius;
    mSearchRadius = rOther.mSearchRadius;
    mRealMass = rOther.mRealMass;
    mClusterId = rOther.mClusterId;
    mDiscontinuumConstitutiveLaw = rOther.mDiscontinuumConstitutiveLaw->CloneUnique();
    mRollingFrictionModel = rOther.mRollingFrictionModel->CloneUnique();
    mGlobalDampingModel = rOther.mGlobalDampingModel->CloneUnique();

    if (rOther.mStressTensor != NULL) {
        mStressTensor  = new BoundedMatrix<double, 3, 3>(3,3);
        *mStressTensor = *rOther.mStressTensor;

        mSymmStressTensor  = new BoundedMatrix<double, 3, 3>(3,3);
        *mSymmStressTensor = *rOther.mSymmStressTensor;
    }
    else {
        mStressTensor     = NULL;
        mSymmStressTensor = NULL;
    }
    if (rOther.mStrainTensor) {
        mStrainTensor  = new BoundedMatrix<double, 3, 3>(3,3);
        *mStrainTensor = *rOther.mStrainTensor;
    }
    else {
        mStrainTensor = NULL;
    }
    if (rOther.mDifferentialStrainTensor) {
        mDifferentialStrainTensor  = new BoundedMatrix<double, 3, 3>(3,3);
        *mDifferentialStrainTensor = *rOther.mDifferentialStrainTensor;
    }
    else {
        mDifferentialStrainTensor = NULL;
    }

    mFastProperties = rOther.mFastProperties; //This might be unsafe

    DEMIntegrationScheme::Pointer& translational_integration_scheme = GetProperties()[DEM_TRANSLATIONAL_INTEGRATION_SCHEME_POINTER];
    DEMIntegrationScheme::Pointer& rotational_integration_scheme = GetProperties()[DEM_ROTATIONAL_INTEGRATION_SCHEME_POINTER];
    SetIntegrationScheme(translational_integration_scheme, rotational_integration_scheme);
    SetValue(WALL_POINT_CONDITION_POINTERS, std::vector<Condition*>());
    SetValue(WALL_POINT_CONDITION_ELASTIC_FORCES, std::vector<array_1d<double, 3> >());
    SetValue(WALL_POINT_CONDITION_TOTAL_FORCES, std::vector<array_1d<double, 3> >());

    return *this;
}

void SphericParticle::Initialize(const ProcessInfo& r_process_info)
{
    KRATOS_TRY

    this->mInitializationTime = r_process_info[TIME];
    this->mIndentationInitialOption = r_process_info[CLEAN_INDENT_V2_OPTION];

    SetValue(NEIGHBOUR_IDS, DenseVector<int>());

    MemberDeclarationFirstStep(r_process_info);

    NodeType& node = GetGeometry()[0];

    SetRadius(node.GetSolutionStepValue(RADIUS));
    mInitialRadius = GetRadius();
    SetMass(GetDensity() * CalculateVolume());

    if (this->IsNot(BLOCKED)) node.GetSolutionStepValue(PARTICLE_MATERIAL) = GetParticleMaterial();

    mClusterId = -1;

    if (this->Is(DEMFlags::HAS_ROTATION)) {
        node.GetSolutionStepValue(PARTICLE_MOMENT_OF_INERTIA) = CalculateMomentOfInertia();

        Quaternion<double> Orientation = Quaternion<double>::Identity();
        node.GetSolutionStepValue(ORIENTATION) = Orientation;

        array_1d<double, 3> angular_momentum;
        CalculateLocalAngularMomentum(angular_momentum);
        noalias(node.GetSolutionStepValue(ANGULAR_MOMENTUM)) = angular_momentum;

        array_1d<double, 3>& delta_rotation = node.GetSolutionStepValue(DELTA_ROTATION);
        delta_rotation = ZeroVector(3);

        array_1d<double, 3>& rotation_angle = node.GetSolutionStepValue(PARTICLE_ROTATION_ANGLE);
        rotation_angle = ZeroVector(3);

        if (this->Is(DEMFlags::HAS_ROLLING_FRICTION)) {
            mRollingFrictionModel = pCloneRollingFrictionModel(this);
        }
    }
    else {
        array_1d<double, 3>& angular_velocity = node.GetSolutionStepValue(ANGULAR_VELOCITY); //TODO: do we need this when there is no rotation??
        angular_velocity = ZeroVector(3);
    }

    if (node.GetDof(VELOCITY_X).IsFixed())         {node.Set(DEMFlags::FIXED_VEL_X,true);}
    else                                           {node.Set(DEMFlags::FIXED_VEL_X,false);}
    if (node.GetDof(VELOCITY_Y).IsFixed())         {node.Set(DEMFlags::FIXED_VEL_Y,true);}
    else                                           {node.Set(DEMFlags::FIXED_VEL_Y,false);}
    if (node.GetDof(VELOCITY_Z).IsFixed())         {node.Set(DEMFlags::FIXED_VEL_Z,true);}
    else                                           {node.Set(DEMFlags::FIXED_VEL_Z,false);}
    if (node.GetDof(ANGULAR_VELOCITY_X).IsFixed()) {node.Set(DEMFlags::FIXED_ANG_VEL_X,true);}
    else                                           {node.Set(DEMFlags::FIXED_ANG_VEL_X,false);}
    if (node.GetDof(ANGULAR_VELOCITY_Y).IsFixed()) {node.Set(DEMFlags::FIXED_ANG_VEL_Y,true);}
    else                                           {node.Set(DEMFlags::FIXED_ANG_VEL_Y,false);}
    if (node.GetDof(ANGULAR_VELOCITY_Z).IsFixed()) {node.Set(DEMFlags::FIXED_ANG_VEL_Z,true);}
    else                                           {node.Set(DEMFlags::FIXED_ANG_VEL_Z,false);}

    double& elastic_energy = this->GetElasticEnergy();
    elastic_energy = 0.0;
    double& inelastic_frictional_energy = this->GetInelasticFrictionalEnergy();
    inelastic_frictional_energy = 0.0;
    double& inelastic_viscodamping_energy = this->GetInelasticViscodampingEnergy();
    inelastic_viscodamping_energy = 0.0;
    double& inelastic_rollingresistance_energy = this->GetInelasticRollingResistanceEnergy();
    inelastic_rollingresistance_energy = 0.0;
    double& max_normal_ball_to_ball_force_times_radius = this->GetMaxNormalBallToBallForceTimesRadius();
    max_normal_ball_to_ball_force_times_radius = 0.0;

    if (this->Is(DEMFlags::HAS_GLOBAL_DAMPING)) {
        mGlobalDampingModel = pCloneGlobalDampingModel(this);
        mGlobalDampingModel->mGlobalDamping = r_process_info[GLOBAL_DAMPING];
    }

    DEMIntegrationScheme::Pointer& translational_integration_scheme = GetProperties()[DEM_TRANSLATIONAL_INTEGRATION_SCHEME_POINTER];
    DEMIntegrationScheme::Pointer& rotational_integration_scheme = GetProperties()[DEM_ROTATIONAL_INTEGRATION_SCHEME_POINTER];
    SetIntegrationScheme(translational_integration_scheme, rotational_integration_scheme);

    SetValue(WALL_POINT_CONDITION_POINTERS, std::vector<Condition*>());
    SetValue(WALL_POINT_CONDITION_ELASTIC_FORCES, std::vector<array_1d<double, 3>>());
    SetValue(WALL_POINT_CONDITION_TOTAL_FORCES, std::vector<array_1d<double, 3>>());

    KRATOS_CATCH( "" )
}

void SphericParticle::SetIntegrationScheme(DEMIntegrationScheme::Pointer& translational_integration_scheme, DEMIntegrationScheme::Pointer& rotational_integration_scheme) {
    mpTranslationalIntegrationScheme = translational_integration_scheme->CloneRaw();
    mpRotationalIntegrationScheme = rotational_integration_scheme->CloneRaw();
}

void SphericParticle::CalculateRightHandSide(const ProcessInfo& r_process_info, double dt, const array_1d<double,3>& gravity)
{
    KRATOS_TRY

    // Creating a data buffer to store those variables that we want to reuse so that we can keep function parameter lists short

    SphericParticle::BufferPointerType p_buffer = CreateParticleDataBuffer(this); // all memory will be freed once this shared pointer goes out of scope
    ParticleDataBuffer& data_buffer = *p_buffer;
    data_buffer.SetBoundingBox(r_process_info[DOMAIN_IS_PERIODIC], r_process_info[DOMAIN_MIN_CORNER], r_process_info[DOMAIN_MAX_CORNER]);

    NodeType& this_node = GetGeometry()[0];

    data_buffer.mDt = dt;
    data_buffer.mTime = r_process_info[TIME];
    data_buffer.mMultiStageRHS = false;

    array_1d<double, 3> additional_forces = ZeroVector(3);
    array_1d<double, 3> additionally_applied_moment = ZeroVector(3);
    array_1d<double, 3>& elastic_force       = this_node.FastGetSolutionStepValue(ELASTIC_FORCES);
    array_1d<double, 3>& contact_force       = this_node.FastGetSolutionStepValue(CONTACT_FORCES);
    array_1d<double, 3>& rigid_element_force = this_node.FastGetSolutionStepValue(RIGID_ELEMENT_FORCE);

    mContactMoment.clear();
    elastic_force.clear();
    contact_force.clear();
    rigid_element_force.clear();
    //mBondedScalingFactor[0] = 0.0;
    //mBondedScalingFactor[1] = 0.0;
    //mBondedScalingFactor[2] = 0.0;

    InitializeForceComputation(r_process_info);

    ComputeBallToBallContactForceAndMoment(data_buffer, r_process_info, elastic_force, contact_force);

    ComputeBallToRigidFaceContactForceAndMoment(data_buffer, elastic_force, contact_force, rigid_element_force, r_process_info);    

    if (this->IsNot(DEMFlags::BELONGS_TO_A_CLUSTER)){
        ComputeAdditionalForces(additional_forces, additionally_applied_moment, r_process_info, gravity);
        #ifdef KRATOS_DEBUG
        DemDebugFunctions::CheckIfNan(additional_forces, "NAN in Additional Force in RHS of Ball");
        DemDebugFunctions::CheckIfNan(additionally_applied_moment, "NAN in Additional Torque in RHS of Ball");
        #endif
    }

    if (this->Is(DEMFlags::HAS_ROTATION) && !data_buffer.mMultiStageRHS) {
        if (this->Is(DEMFlags::HAS_ROLLING_FRICTION) && !data_buffer.mMultiStageRHS) {
            mRollingFrictionModel->DoFinalOperations(this, dt, mContactMoment);
        }
    }

    array_1d<double,3>& total_forces = this_node.FastGetSolutionStepValue(TOTAL_FORCES);
    array_1d<double,3>& total_moment = this_node.FastGetSolutionStepValue(PARTICLE_MOMENT);

    total_forces[0] = contact_force[0] + additional_forces[0];
    total_forces[1] = contact_force[1] + additional_forces[1];
    total_forces[2] = contact_force[2] + additional_forces[2];

    total_moment[0] = mContactMoment[0] + additionally_applied_moment[0];
    total_moment[1] = mContactMoment[1] + additionally_applied_moment[1];
    total_moment[2] = mContactMoment[2] + additionally_applied_moment[2];

    ApplyGlobalDampingToContactForcesAndMoments(total_forces, total_moment);

    #ifdef KRATOS_DEBUG
    DemDebugFunctions::CheckIfNan(total_forces, "NAN in Total Forces in RHS of Ball");
    DemDebugFunctions::CheckIfNan(total_moment, "NAN in Total Torque in RHS of Ball");
    #endif

    FinalizeForceComputation(data_buffer);
    KRATOS_CATCH("")
}

void SphericParticle::InitializeForceComputation(const ProcessInfo& r_process_info){}

void SphericParticle::FirstCalculateRightHandSide(const ProcessInfo& r_process_info, double dt){}

void SphericParticle::CollectCalculateRightHandSide(const ProcessInfo& r_process_info){}

void SphericParticle::FinalCalculateRightHandSide(const ProcessInfo& r_process_info, double dt, const array_1d<double,3>& gravity){}

void SphericParticle::CalculateMaxBallToBallIndentation(double& r_current_max_indentation, const ProcessInfo& r_process_info)
{
    r_current_max_indentation = - std::numeric_limits<double>::max();

    for (unsigned int i = 0; i < mNeighbourElements.size(); i++){
        SphericParticle* ineighbour = mNeighbourElements[i];
        auto& central_node = GetGeometry()[0];
        auto& neighbour_node = ineighbour->GetGeometry()[0];

        array_1d<double, 3> other_to_me_vect;
        if (!r_process_info[DOMAIN_IS_PERIODIC]){ // default infinite-domain case
            noalias(other_to_me_vect) = central_node.Coordinates() - neighbour_node.Coordinates();
        }

        else { // periodic domain
            double my_coors[3] = {central_node[0], central_node[1], central_node[2]};
            double other_coors[3] = {neighbour_node[0], neighbour_node[1], neighbour_node[2]};

            TransformNeighbourCoorsToClosestInPeriodicDomain(r_process_info, my_coors, other_coors);
            other_to_me_vect[0] = my_coors[0] - other_coors[0];
            other_to_me_vect[1] = my_coors[1] - other_coors[1];
            other_to_me_vect[2] = my_coors[2] - other_coors[2];
        }

        double other_radius                  = ineighbour->GetInteractionRadius();
        double distance                      = DEM_MODULUS_3(other_to_me_vect);
        double radius_sum                    = GetInteractionRadius() + other_radius;
        double indentation                   = radius_sum - distance;

        r_current_max_indentation = (indentation > r_current_max_indentation) ? indentation : r_current_max_indentation;
    }
}

void SphericParticle::CalculateMaxBallToFaceIndentation(double& r_current_max_indentation)
{
    r_current_max_indentation = - std::numeric_limits<double>::max();

    std::vector<DEMWall*>& rNeighbours   = this->mNeighbourRigidFaces;

    for (unsigned int i = 0; i < rNeighbours.size(); i++) {

        double LocalCoordSystem[3][3]            = {{0.0}, {0.0}, {0.0}};
        array_1d<double, 3> wall_delta_disp_at_contact_point = ZeroVector(3);
        array_1d<double, 3> wall_velocity_at_contact_point = ZeroVector(3);
        double DistPToB = 0.0;
        int ContactType = -1;
        array_1d<double, 4>& Weight = this->mContactConditionWeights[i];

        //ComputeConditionRelativeData(i,rNeighbours[i], LocalCoordSystem, DistPToB, Weight, wall_delta_disp_at_contact_point, wall_velocity_at_contact_point, ContactType);

        rNeighbours[i]->ComputeConditionRelativeData(i, this, LocalCoordSystem, DistPToB, Weight, wall_delta_disp_at_contact_point, wall_velocity_at_contact_point, ContactType);

        if(ContactType > 0){
            double indentation = GetInteractionRadius() - DistPToB;
            r_current_max_indentation = (indentation > r_current_max_indentation) ? indentation : r_current_max_indentation;

        }

    } //for every rigidface neighbor
}

void SphericParticle::CalculateMomentum(array_1d<double, 3>& r_momentum)
{
    const array_1d<double, 3>& vel = this->GetGeometry()[0].FastGetSolutionStepValue(VELOCITY);
    noalias(r_momentum) = GetMass() * vel;
}

void SphericParticle::CalculateLocalAngularMomentum(array_1d<double, 3>& r_angular_momentum)
{
    const array_1d<double, 3> ang_vel  = this->GetGeometry()[0].FastGetSolutionStepValue(ANGULAR_VELOCITY);
    const double moment_of_inertia     = this->GetGeometry()[0].FastGetSolutionStepValue(PARTICLE_MOMENT_OF_INERTIA);
    noalias(r_angular_momentum) = moment_of_inertia * ang_vel;
}

void SphericParticle::ComputeNewNeighboursHistoricalData(DenseVector<int>& temp_neighbours_ids,
                                                         std::vector<array_1d<double, 3> >& temp_neighbour_elastic_contact_forces)
{
    std::vector<array_1d<double, 3> > temp_neighbour_elastic_extra_contact_forces;
    unsigned int new_size = mNeighbourElements.size();
    array_1d<double, 3> vector_of_zeros = ZeroVector(3);
    temp_neighbours_ids.resize(new_size, false);
    temp_neighbour_elastic_contact_forces.resize(new_size);
    temp_neighbour_elastic_extra_contact_forces.resize(new_size);

    DenseVector<int>& vector_of_ids_of_neighbours = GetValue(NEIGHBOUR_IDS);

    for (unsigned int i = 0; i < new_size; i++) {
        noalias(temp_neighbour_elastic_contact_forces[i]) = vector_of_zeros;
        noalias(temp_neighbour_elastic_extra_contact_forces[i]) = vector_of_zeros;

        if (mNeighbourElements[i] == NULL) { // This is required by the continuum sphere which reorders the neighbors
            temp_neighbours_ids[i] = -1;
            continue;
        }

        temp_neighbours_ids[i] = mNeighbourElements[i]->Id();

        for (unsigned int j = 0; j < vector_of_ids_of_neighbours.size(); j++) {
            if (int(temp_neighbours_ids[i]) == vector_of_ids_of_neighbours[j] && vector_of_ids_of_neighbours[j] != -1) {
                noalias(temp_neighbour_elastic_contact_forces[i]) = mNeighbourElasticContactForces[j];
                noalias(temp_neighbour_elastic_extra_contact_forces[i]) = mNeighbourElasticExtraContactForces[j]; //TODO: remove this from discontinuum!!
                break;
            }
        }
    }

    vector_of_ids_of_neighbours.swap(temp_neighbours_ids);
    mNeighbourElasticContactForces.swap(temp_neighbour_elastic_contact_forces);
    mNeighbourElasticExtraContactForces.swap(temp_neighbour_elastic_extra_contact_forces);
}

void SphericParticle::ComputeNewRigidFaceNeighboursHistoricalData()
{
    array_1d<double, 3> vector_of_zeros = ZeroVector(3);
    std::vector<DEMWall*>& rNeighbours = this->mNeighbourRigidFaces;
    unsigned int new_size              = rNeighbours.size();
    std::vector<int> temp_neighbours_ids(new_size); //these two temporal vectors are very small, saving them as a member of the particle loses time (usually they consist on 1 member).
    std::vector<array_1d<double, 3> > temp_neighbours_elastic_contact_forces(new_size);
    std::vector<array_1d<double, 3> > temp_neighbours_contact_forces(new_size);

    for (unsigned int i = 0; i<rNeighbours.size(); i++){

        noalias(temp_neighbours_elastic_contact_forces[i]) = vector_of_zeros;
        noalias(temp_neighbours_contact_forces[i]) = vector_of_zeros;

        if (rNeighbours[i] == NULL) { // This is required by the continuum sphere which reorders the neighbors
            temp_neighbours_ids[i] = -1;
            continue;
        }

        temp_neighbours_ids[i] = static_cast<int>(rNeighbours[i]->Id());

        for (unsigned int j = 0; j != mFemOldNeighbourIds.size(); j++) {
            if (static_cast<int>(temp_neighbours_ids[i]) == mFemOldNeighbourIds[j] && mFemOldNeighbourIds[j] != -1) {
                noalias(temp_neighbours_elastic_contact_forces[i]) = mNeighbourRigidFacesElasticContactForce[j];
                noalias(temp_neighbours_contact_forces[i]) = mNeighbourRigidFacesTotalContactForce[j];
                break;
            }
        }
    }

    mFemOldNeighbourIds.swap(temp_neighbours_ids);
    mNeighbourRigidFacesElasticContactForce.swap(temp_neighbours_elastic_contact_forces);
    mNeighbourRigidFacesTotalContactForce.swap(temp_neighbours_contact_forces);
}

void SphericParticle::EquationIdVector(EquationIdVectorType& rResult, const ProcessInfo& r_process_info) const {}

void SphericParticle::CalculateMassMatrix(MatrixType& rMassMatrix, const ProcessInfo& r_process_info)
{
    rMassMatrix(0,0) = GetMass();
}

void SphericParticle::EvaluateDeltaDisplacement(ParticleDataBuffer & data_buffer,
                                                double RelDeltDisp[3],
                                                double RelVel[3],
                                                double LocalCoordSystem[3][3],
                                                double OldLocalCoordSystem[3][3],
                                                const array_1d<double, 3>& vel,
                                                const array_1d<double, 3>& delta_displ)
{
    // FORMING LOCAL COORDINATES

    // Notes: Since we will normally inherit the mesh from GiD, we respect the global system X,Y,Z [0],[1],[2]
    // In the local coordinates we will define the normal direction of the contact as the [2] component.
    // Compression is positive.
    GeometryFunctions::ComputeContactLocalCoordSystem(data_buffer.mOtherToMeVector, data_buffer.mDistance, LocalCoordSystem); //new Local Coordinate System (normalizes data_buffer.mOtherToMeVector)

    // FORMING OLD LOCAL COORDINATES
    array_1d<double, 3> old_coord_target;
    noalias(old_coord_target) = this->GetGeometry()[0].Coordinates() - delta_displ;

    const array_1d<double, 3>& other_delta_displ = data_buffer.mpOtherParticleNode->FastGetSolutionStepValue(DELTA_DISPLACEMENT);
    array_1d<double, 3> old_coord_neigh;
    noalias(old_coord_neigh) = data_buffer.mpOtherParticleNode->Coordinates() - other_delta_displ;

    if (data_buffer.mDomainIsPeriodic){ // the domain is periodic
        TransformNeighbourCoorsToClosestInPeriodicDomain(data_buffer, old_coord_target, old_coord_neigh);
    }

    array_1d<double, 3> old_other_to_me_vect;
    noalias(old_other_to_me_vect) = old_coord_target - old_coord_neigh;

    const double old_distance = DEM_MODULUS_3(old_other_to_me_vect);

    GeometryFunctions::ComputeContactLocalCoordSystem(old_other_to_me_vect, old_distance, OldLocalCoordSystem); //Old Local Coordinate System

    // VELOCITIES AND DISPLACEMENTS
    const array_1d<double, 3 >& other_vel = data_buffer.mpOtherParticleNode->FastGetSolutionStepValue(VELOCITY);

    RelVel[0] = (vel[0] - other_vel[0]);
    RelVel[1] = (vel[1] - other_vel[1]);
    RelVel[2] = (vel[2] - other_vel[2]);

    // DeltDisp in global coordinates
    RelDeltDisp[0] = (delta_displ[0] - other_delta_displ[0]);
    RelDeltDisp[1] = (delta_displ[1] - other_delta_displ[1]);
    RelDeltDisp[2] = (delta_displ[2] - other_delta_displ[2]);

}

void SphericParticle::RelativeDisplacementAndVelocityOfContactPointDueToRotation(const double indentation,
                                                double RelDeltDisp[3],
                                                double RelVel[3],
                                                const double LocalCoordSystem[3][3],
                                                const double& other_radius,
                                                const double& dt,
                                                const array_1d<double, 3>& my_ang_vel,
                                                SphericParticle* p_neighbour)
{
    const array_1d<double, 3>& my_delta_rotation = GetGeometry()[0].FastGetSolutionStepValue(DELTA_ROTATION);
    const auto& neighbour_node = p_neighbour->GetGeometry()[0];
    const array_1d<double, 3>& other_ang_vel = neighbour_node.FastGetSolutionStepValue(ANGULAR_VELOCITY);
    const array_1d<double, 3>& other_delta_rotation = neighbour_node.FastGetSolutionStepValue(DELTA_ROTATION);
    array_1d<double, 3> my_arm_vector;
    array_1d<double, 3> other_arm_vector;
    array_1d<double, 3> my_vel_at_contact_point_due_to_rotation;
    array_1d<double, 3> other_vel_at_contact_point_due_to_rotation;
    array_1d<double, 3> my_delta_disp_at_contact_point_due_to_rotation;
    array_1d<double, 3> other_delta_disp_at_contact_point_due_to_rotation;
    const double other_young = p_neighbour->GetYoung();
    const double my_young = GetYoung();

    const double inverse_of_sum_of_youngs = 1.0 / (other_young + my_young);

    const double my_arm_length = GetInteractionRadius() - indentation * other_young * inverse_of_sum_of_youngs;
    const double other_arm_length  = other_radius - indentation * my_young * inverse_of_sum_of_youngs;

    my_arm_vector[0] = -LocalCoordSystem[2][0] * my_arm_length;
    my_arm_vector[1] = -LocalCoordSystem[2][1] * my_arm_length;
    my_arm_vector[2] = -LocalCoordSystem[2][2] * my_arm_length;

    GeometryFunctions::CrossProduct(my_ang_vel, my_arm_vector, my_vel_at_contact_point_due_to_rotation);

    other_arm_vector[0] = LocalCoordSystem[2][0] * other_arm_length;
    other_arm_vector[1] = LocalCoordSystem[2][1] * other_arm_length;
    other_arm_vector[2] = LocalCoordSystem[2][2] * other_arm_length;

    GeometryFunctions::CrossProduct(other_ang_vel, other_arm_vector, other_vel_at_contact_point_due_to_rotation);

    RelVel[0] += my_vel_at_contact_point_due_to_rotation[0] - other_vel_at_contact_point_due_to_rotation[0];
    RelVel[1] += my_vel_at_contact_point_due_to_rotation[1] - other_vel_at_contact_point_due_to_rotation[1];
    RelVel[2] += my_vel_at_contact_point_due_to_rotation[2] - other_vel_at_contact_point_due_to_rotation[2];

    GeometryFunctions::CrossProduct(my_delta_rotation,    my_arm_vector,    my_delta_disp_at_contact_point_due_to_rotation);
    GeometryFunctions::CrossProduct(other_delta_rotation, other_arm_vector, other_delta_disp_at_contact_point_due_to_rotation);


    // Contribution of the rotation
    RelDeltDisp[0] += my_delta_disp_at_contact_point_due_to_rotation[0] - other_delta_disp_at_contact_point_due_to_rotation[0];
    RelDeltDisp[1] += my_delta_disp_at_contact_point_due_to_rotation[1] - other_delta_disp_at_contact_point_due_to_rotation[1];
    RelDeltDisp[2] += my_delta_disp_at_contact_point_due_to_rotation[2] - other_delta_disp_at_contact_point_due_to_rotation[2];

}


void SphericParticle::RelativeDisplacementAndVelocityOfContactPointDueToRotationMatrix(double DeltDisp[3],
                                                double RelVel[3],
                                                const double OldLocalCoordSystem[3][3],
                                                const double& other_radius,
                                                const double& dt,
                                                const array_1d<double, 3>& angular_vel,
                                                SphericParticle* p_neighbour)
{
        const auto& central_node = GetGeometry()[0];
        const array_1d<double, 3>& my_delta_rotation = central_node.FastGetSolutionStepValue(DELTA_ROTATION);
        const auto& neighbour_node = p_neighbour->GetGeometry()[0];
        const array_1d<double, 3>& neigh_angular_vel = neighbour_node.FastGetSolutionStepValue(ANGULAR_VELOCITY);
        const array_1d<double, 3>& other_delta_rotation = neighbour_node.FastGetSolutionStepValue(DELTA_ROTATION);
        const double other_young = p_neighbour->GetYoung();
        const double my_young = GetYoung();

        const double my_rotated_angle = DEM_MODULUS_3(my_delta_rotation);
        const double other_rotated_angle = DEM_MODULUS_3(other_delta_rotation);

        const array_1d<double, 3>& coors = central_node.Coordinates();
        array_1d<double, 3> neigh_coors = neighbour_node.Coordinates();

        const array_1d<double, 3> other_to_me_vect = coors - neigh_coors;
        const double distance            = DEM_MODULUS_3(other_to_me_vect);
        const double radius_sum          = GetInteractionRadius() + other_radius;
        const double indentation         = radius_sum - distance;

        const double arm = GetInteractionRadius() - indentation * other_young / (other_young + my_young);
        array_1d<double, 3> e1;
        DEM_COPY_SECOND_TO_FIRST_3(e1, OldLocalCoordSystem[2]);
        DEM_MULTIPLY_BY_SCALAR_3(e1, -arm);
        array_1d<double, 3> new_axes1 = e1;

        const double other_arm = other_radius - indentation * my_young / (other_young + my_young);
        array_1d<double, 3> e2;
        DEM_COPY_SECOND_TO_FIRST_3(e2, OldLocalCoordSystem[2]);
        DEM_MULTIPLY_BY_SCALAR_3(e2, other_arm);
        array_1d<double, 3> new_axes2 = e2;

        if (my_rotated_angle) {
            array_1d<double, 3> axis_1;
            axis_1[0] = my_delta_rotation[0] / my_rotated_angle;
            axis_1[1] = my_delta_rotation[1] / my_rotated_angle;
            axis_1[2] = my_delta_rotation[2] / my_rotated_angle;

            GeometryFunctions::RotateAVectorAGivenAngleAroundAUnitaryVector(e1, axis_1, my_rotated_angle, new_axes1);

        } //if my_rotated_angle

        if (other_rotated_angle) {
            array_1d<double, 3> axis_2;
            axis_2[0] = other_delta_rotation[0] / other_rotated_angle;
            axis_2[1] = other_delta_rotation[1] / other_rotated_angle;
            axis_2[2] = other_delta_rotation[2] / other_rotated_angle;

            GeometryFunctions::RotateAVectorAGivenAngleAroundAUnitaryVector(e2, axis_2, other_rotated_angle, new_axes2);

        } //if other_rotated_angle

        array_1d<double, 3> radial_vector = - other_to_me_vect;
        array_1d<double, 3> other_radial_vector = other_to_me_vect;

        GeometryFunctions::normalize(radial_vector);
        GeometryFunctions::normalize(other_radial_vector);

        radial_vector *= arm;
        other_radial_vector *= other_arm;

        array_1d<double, 3> vel = ZeroVector(3);
        array_1d<double, 3> other_vel = ZeroVector(3);

        GeometryFunctions::CrossProduct(angular_vel, radial_vector, vel);
        GeometryFunctions::CrossProduct(neigh_angular_vel, other_radial_vector, other_vel);

        RelVel[0] += vel[0] - other_vel[0];
        RelVel[1] += vel[1] - other_vel[1];
        RelVel[2] += vel[2] - other_vel[2];

        // Contribution of the rotation velocity


        DeltDisp[0] += (new_axes1[0] - new_axes2[0]) + (e2[0] - e1[0]);
        DeltDisp[1] += (new_axes1[1] - new_axes2[1]) + (e2[1] - e1[1]);
        DeltDisp[2] += (new_axes1[2] - new_axes2[2]) + (e2[2] - e1[2]);
}

void SphericParticle::RelativeDisplacementAndVelocityOfContactPointDueToRotationQuaternion(double RelDeltDisp[3],
                                                double RelVel[3],
                                                const double OldLocalCoordSystem[3][3],
                                                const double& other_radius,
                                                const double& dt,
                                                const array_1d<double, 3>& my_ang_vel,
                                                SphericParticle* p_neighbour,
                                                ParticleDataBuffer & data_buffer)
{
    const auto& central_node = GetGeometry()[0];
    const auto& neighbour_node = p_neighbour->GetGeometry()[0];
    const array_1d<double, 3>& other_ang_vel = neighbour_node.FastGetSolutionStepValue(ANGULAR_VELOCITY);
    const array_1d<double, 3>& my_delta_rotation = central_node.FastGetSolutionStepValue(DELTA_ROTATION);
    const array_1d<double, 3>& other_delta_rotation = neighbour_node.FastGetSolutionStepValue(DELTA_ROTATION);
    array_1d<double, 3> my_arm_vector;
    array_1d<double, 3> other_arm_vector;
    array_1d<double, 3> my_new_arm_vector;
    array_1d<double, 3> other_new_arm_vector;
    array_1d<double, 3> my_vel_at_contact_point_due_to_rotation;
    array_1d<double, 3> other_vel_at_contact_point_due_to_rotation;
    const double other_young = p_neighbour->GetYoung();
    const double my_young = GetYoung();

    const array_1d<double, 3>& coors = central_node.Coordinates();
    array_1d<double, 3> neigh_coors = neighbour_node.Coordinates();

    if (data_buffer.mDomainIsPeriodic){
        TransformNeighbourCoorsToClosestInPeriodicDomain(data_buffer, coors, neigh_coors);
    }

    const array_1d<double, 3> other_to_me_vect = coors - neigh_coors;
    const double distance            = DEM_MODULUS_3(other_to_me_vect);
    const double radius_sum          = GetInteractionRadius() + other_radius;
    const double indentation         = radius_sum - distance;

    const double my_arm_length = GetInteractionRadius() - indentation * other_young / (other_young + my_young);
    const double other_arm_length  = other_radius - indentation * my_young / (other_young + my_young);

    my_arm_vector[0] = -OldLocalCoordSystem[2][0] * my_arm_length;
    my_arm_vector[1] = -OldLocalCoordSystem[2][1] * my_arm_length;
    my_arm_vector[2] = -OldLocalCoordSystem[2][2] * my_arm_length;

    GeometryFunctions::CrossProduct(my_ang_vel, my_arm_vector, my_vel_at_contact_point_due_to_rotation);

    other_arm_vector[0] = OldLocalCoordSystem[2][0] * other_arm_length;
    other_arm_vector[1] = OldLocalCoordSystem[2][1] * other_arm_length;
    other_arm_vector[2] = OldLocalCoordSystem[2][2] * other_arm_length;

    GeometryFunctions::CrossProduct(other_ang_vel, other_arm_vector, other_vel_at_contact_point_due_to_rotation);

    RelVel[0] += my_vel_at_contact_point_due_to_rotation[0] - other_vel_at_contact_point_due_to_rotation[0];
    RelVel[1] += my_vel_at_contact_point_due_to_rotation[1] - other_vel_at_contact_point_due_to_rotation[1];
    RelVel[2] += my_vel_at_contact_point_due_to_rotation[2] - other_vel_at_contact_point_due_to_rotation[2];

    Quaternion<double> MyDeltaOrientation = Quaternion<double>::Identity();
    Quaternion<double> OtherDeltaOrientation = Quaternion<double>::Identity();

    GeometryFunctions::OrientationFromRotationAngle(MyDeltaOrientation, my_delta_rotation);
    GeometryFunctions::OrientationFromRotationAngle(OtherDeltaOrientation, other_delta_rotation);

    MyDeltaOrientation.RotateVector3(my_arm_vector, my_new_arm_vector);
    OtherDeltaOrientation.RotateVector3(other_arm_vector, other_new_arm_vector);

    // Contribution of the rotation
    RelDeltDisp[0] += (my_new_arm_vector[0] - other_new_arm_vector[0]) + (other_arm_vector[0] - my_arm_vector[0]);
    RelDeltDisp[1] += (my_new_arm_vector[1] - other_new_arm_vector[1]) + (other_arm_vector[1] - my_arm_vector[1]);
    RelDeltDisp[2] += (my_new_arm_vector[2] - other_new_arm_vector[2]) + (other_arm_vector[2] - my_arm_vector[2]);

}

void SphericParticle::ComputeMoments(double NormalLocalContactForce,
                                     double Force[3],
                                     double LocalCoordSystem2[3],
                                     SphericParticle* p_neighbour,
                                     double indentation,
                                     unsigned int i)
{
    const double other_young = p_neighbour->GetYoung();
    const double arm_length  = GetInteractionRadius() - indentation * other_young / (other_young + GetYoung());

    array_1d<double, 3> arm_vector;
    arm_vector[0] = -LocalCoordSystem2[0] * arm_length;
    arm_vector[1] = -LocalCoordSystem2[1] * arm_length;
    arm_vector[2] = -LocalCoordSystem2[2] * arm_length;

    array_1d<double, 3> moment_of_this_neighbour;
    GeometryFunctions::CrossProduct(arm_vector, Force, moment_of_this_neighbour);
    noalias(mContactMoment) += moment_of_this_neighbour;

}

void SphericParticle::ComputeMomentsWithWalls(double NormalLocalContactForce,
                                     double Force[3],
                                     double LocalCoordSystem2[3],
                                     Condition* wall,
                                     double indentation,
                                     unsigned int i)
{
    double arm_length = GetInteractionRadius() - indentation;

    array_1d<double, 3> arm_vector;
    arm_vector[0] = -LocalCoordSystem2[0] * arm_length;
    arm_vector[1] = -LocalCoordSystem2[1] * arm_length;
    arm_vector[2] = -LocalCoordSystem2[2] * arm_length;

    array_1d<double, 3> moment_of_this_neighbour;
    GeometryFunctions::CrossProduct(arm_vector, Force, moment_of_this_neighbour);
    noalias(mContactMoment) += moment_of_this_neighbour;
}

void SphericParticle::ComputeBallToBallContactForceAndMoment(SphericParticle::ParticleDataBuffer & data_buffer,
                                                    const ProcessInfo& r_process_info,
                                                    array_1d<double, 3>& r_elastic_force,
                                                    array_1d<double, 3>& r_contact_force)
{
    KRATOS_TRY

    NodeType& this_node = GetGeometry()[0];
    DEM_COPY_SECOND_TO_FIRST_3(data_buffer.mMyCoors, this_node)

    //LOOP OVER NEIGHBORS:
    for (int i = 0; data_buffer.SetNextNeighbourOrExit(i); ++i){

        if (CalculateRelativePositionsOrSkipContact(data_buffer)) {
            DEM_SET_COMPONENTS_TO_ZERO_3x3(data_buffer.mLocalCoordSystem)
            DEM_SET_COMPONENTS_TO_ZERO_3x3(data_buffer.mOldLocalCoordSystem)
            double DeltDisp[3]                       = {0.0};
            double LocalDeltDisp[3]                  = {0.0};
            double RelVel[3]                         = {0.0};
            double LocalContactForce[3]              = {0.0};
            double GlobalContactForce[3]             = {0.0};
            double LocalElasticContactForce[3]       = {0.0};
            double LocalElasticExtraContactForce[3]  = {0.0};
            double GlobalElasticContactForce[3]      = {0.0};
            double GlobalElasticExtraContactForce[3] = {0.0};
            double TotalGlobalElasticContactForce[3] = {0.0};
            double ViscoDampingLocalContactForce[3]  = {0.0};
            double cohesive_force                    =  0.0;
            bool sliding = false;

            const array_1d<double, 3>& velocity     = this_node.FastGetSolutionStepValue(VELOCITY);
            const array_1d<double, 3>& delta_displ  = this_node.FastGetSolutionStepValue(DELTA_DISPLACEMENT);
            const array_1d<double, 3>& ang_velocity = this_node.FastGetSolutionStepValue(ANGULAR_VELOCITY);

            EvaluateDeltaDisplacement(data_buffer, DeltDisp, RelVel, data_buffer.mLocalCoordSystem, data_buffer.mOldLocalCoordSystem, velocity, delta_displ);

            if (this->Is(DEMFlags::HAS_ROTATION)) {
                RelativeDisplacementAndVelocityOfContactPointDueToRotationQuaternion(DeltDisp, RelVel, data_buffer.mLocalCoordSystem, data_buffer.mOtherRadius, data_buffer.mDt, ang_velocity, data_buffer.mpOtherParticle, data_buffer);
            }

            RelativeDisplacementAndVelocityOfContactPointDueToOtherReasons(r_process_info, DeltDisp, RelVel, data_buffer.mOldLocalCoordSystem, data_buffer.mLocalCoordSystem, data_buffer.mpOtherParticle);


            EvaluateBallToBallForcesForPositiveIndentiations(data_buffer,
                                                            r_process_info,
                                                            LocalElasticContactForce,
                                                            DeltDisp,
                                                            LocalDeltDisp,
                                                            RelVel,
                                                            data_buffer.mIndentation,
                                                            ViscoDampingLocalContactForce,
                                                            cohesive_force,
                                                            data_buffer.mpOtherParticle,
                                                            sliding,
                                                            data_buffer.mLocalCoordSystem,
                                                            data_buffer.mOldLocalCoordSystem,
                                                            mNeighbourElasticContactForces[i]);


            array_1d<double, 3> other_ball_to_ball_forces = ZeroVector(3);
            ComputeOtherBallToBallForces(other_ball_to_ball_forces); //These forces can exist even with no indentation.

            // Transforming to global forces and adding up
            AddUpForcesAndProject(data_buffer.mOldLocalCoordSystem, data_buffer.mLocalCoordSystem, LocalContactForce, LocalElasticContactForce, LocalElasticExtraContactForce, GlobalContactForce,
                                  GlobalElasticContactForce, GlobalElasticExtraContactForce, TotalGlobalElasticContactForce, ViscoDampingLocalContactForce, cohesive_force, other_ball_to_ball_forces, r_elastic_force, r_contact_force, i, r_process_info);
            //TODO: make different AddUpForces for continuum and discontinuum (different arguments, different operations!)

            // ROTATION FORCES AND ROLLING FRICTION
            if (this->Is(DEMFlags::HAS_ROTATION) && !data_buffer.mMultiStageRHS) {
                ComputeMoments(LocalContactForce[2], GlobalContactForce, data_buffer.mLocalCoordSystem[2], data_buffer.mpOtherParticle, data_buffer.mIndentation, i);
                    
              if (this->Is(DEMFlags::HAS_ROLLING_FRICTION) && !data_buffer.mMultiStageRHS) {
                if (mRollingFrictionModel->CheckIfThisModelRequiresRecloningForEachNeighbour()){
                  mRollingFrictionModel = pCloneRollingFrictionModelWithNeighbour(data_buffer.mpOtherParticle);
                  mRollingFrictionModel->ComputeRollingFriction(this, data_buffer.mpOtherParticle, r_process_info, LocalContactForce, data_buffer.mIndentation, mContactMoment);
                }
                else {
                  mRollingFrictionModel->ComputeRollingResistance(this, data_buffer.mpOtherParticle, LocalContactForce);
                }      
              }
            }

            if (this->Is(DEMFlags::HAS_STRESS_TENSOR)) {
                AddNeighbourContributionToStressTensor(r_process_info, GlobalContactForce, data_buffer.mLocalCoordSystem[2], data_buffer.mDistance, data_buffer.mRadiusSum, this);
            }

            if (r_process_info[IS_TIME_TO_PRINT] && r_process_info[CONTACT_MESH_OPTION] == 1) { //TODO: we should avoid calling a processinfo for each neighbour. We can put it once per time step in the buffer??
                unsigned int neighbour_iterator_id = data_buffer.mpOtherParticle->Id();
                if ((i < (int)mNeighbourElements.size()) && this->Id() < neighbour_iterator_id) {
                    CalculateOnContactElements(i, LocalContactForce, GlobalContactForce);
                }
            } else if (r_process_info[IS_TIME_TO_UPDATE_CONTACT_ELEMENT] && r_process_info[CONTACT_MESH_OPTION] == 1) {
                unsigned int neighbour_iterator_id = data_buffer.mpOtherParticle->Id();
                if ((i < (int)mNeighbourElements.size()) && this->Id() < neighbour_iterator_id) {
                    CalculateOnContactElements(i, LocalContactForce, GlobalContactForce);
                }
            }

            if (r_process_info[ENERGY_CALCULATION_OPTION]){
                double NormalBallToBallForceTimesRadius = this->GetRadius() * LocalContactForce[2];
                if ( NormalBallToBallForceTimesRadius > mMaxNormalBallToBallForceTimesRadius) {
                    mMaxNormalBallToBallForceTimesRadius = NormalBallToBallForceTimesRadius;
                }
            }

            // Store contact information needed for later processes
            StoreBallToBallContactInfo(r_process_info, data_buffer, GlobalContactForce, LocalContactForce, ViscoDampingLocalContactForce, sliding);

            DEM_SET_COMPONENTS_TO_ZERO_3(DeltDisp)
            DEM_SET_COMPONENTS_TO_ZERO_3(LocalDeltDisp)
            DEM_SET_COMPONENTS_TO_ZERO_3(RelVel)
            DEM_SET_COMPONENTS_TO_ZERO_3x3(data_buffer.mLocalCoordSystem)
            DEM_SET_COMPONENTS_TO_ZERO_3x3(data_buffer.mOldLocalCoordSystem)

            #ifdef KRATOS_DEBUG
                DemDebugFunctions::CheckIfNan(GlobalContactForce, "NAN in Force in Ball to Ball contact");
                DemDebugFunctions::CheckIfNan(mContactMoment, "NAN in Torque in Ball to Ball contact");
            #endif
        }
    }// for each neighbor

    KRATOS_CATCH("")
}// ComputeBallToBallContactForceAndMoment

void SphericParticle::EvaluateBallToBallForcesForPositiveIndentiations(SphericParticle::ParticleDataBuffer & data_buffer,
                                                                       const ProcessInfo& r_process_info,
                                                                       double LocalElasticContactForce[3],
                                                                       double DeltDisp[3],
                                                                       double LocalDeltDisp[3],
                                                                       double RelVel[3],
                                                                       const double indentation,
                                                                       double ViscoDampingLocalContactForce[3],
                                                                       double& cohesive_force,
                                                                       SphericParticle* p_neighbour_element,
                                                                       bool& sliding,
                                                                       double LocalCoordSystem[3][3],
                                                                       double OldLocalCoordSystem[3][3],
                                                                       array_1d<double, 3>& neighbour_elastic_contact_force)
{
    double OldLocalElasticContactForce[3] = {0.0};
    RotateOldContactForces(OldLocalCoordSystem, LocalCoordSystem, neighbour_elastic_contact_force);// still in global coordinates
    // Here we recover the old local forces projected in the new coordinates in the way they were in the old ones; Now they will be increased if necessary
    GeometryFunctions::VectorGlobal2Local(LocalCoordSystem, neighbour_elastic_contact_force, OldLocalElasticContactForce);
    GeometryFunctions::VectorGlobal2Local(LocalCoordSystem, DeltDisp, LocalDeltDisp);
    const double previous_indentation = indentation + LocalDeltDisp[2];
    data_buffer.mLocalRelVel[0] = 0.0;
    data_buffer.mLocalRelVel[1] = 0.0;
    data_buffer.mLocalRelVel[2] = 0.0;
    GeometryFunctions::VectorGlobal2Local(LocalCoordSystem, RelVel, data_buffer.mLocalRelVel);
    
    mDiscontinuumConstitutiveLaw = pCloneDiscontinuumConstitutiveLawWithNeighbour(p_neighbour_element);
    mDiscontinuumConstitutiveLaw->CalculateForces(r_process_info, OldLocalElasticContactForce,
            LocalElasticContactForce, LocalDeltDisp, data_buffer.mLocalRelVel, indentation, previous_indentation,
            ViscoDampingLocalContactForce, cohesive_force, this, p_neighbour_element, sliding, LocalCoordSystem);
}

void SphericParticle::ComputeBallToRigidFaceContactForceAndMoment(SphericParticle::ParticleDataBuffer & data_buffer,
                                                        array_1d<double, 3>& r_elastic_force,
                                                        array_1d<double, 3>& r_contact_force,
                                                        array_1d<double, 3>& rigid_element_force,
                                                        const ProcessInfo& r_process_info)
{
    KRATOS_TRY

    RenewData();
    const auto& central_node = GetGeometry()[0];

    std::vector<DEMWall*>& rNeighbours   = this->mNeighbourRigidFaces;
    const array_1d<double, 3>& velocity  = GetGeometry()[0].FastGetSolutionStepValue(VELOCITY);
    const array_1d<double,3>& AngularVel = GetGeometry()[0].FastGetSolutionStepValue(ANGULAR_VELOCITY);

    for (unsigned int i = 0; i < rNeighbours.size(); i++) {
        DEMWall* wall = rNeighbours[i];
        if (wall == NULL)
          continue;
        else
          data_buffer.mpOtherRigidFace = wall;

        if(this->Is(DEMFlags::STICKY)) {
            DEMIntegrationScheme& dem_scheme = this->GetTranslationalIntegrationScheme();
            GluedToWallScheme* p_glued_scheme = dynamic_cast<GluedToWallScheme*>(&dem_scheme);
            Condition* p_condition = p_glued_scheme->pGetCondition();
            if(p_condition == wall) continue;
        }
        if(wall->IsPhantom()){
            wall->CheckSide(this);
            continue;
        }

        double LocalElasticContactForce[3]       = {0.0};
        double GlobalElasticContactForce[3]      = {0.0};
        double ViscoDampingLocalContactForce[3]  = {0.0};
        double cohesive_force                    =  0.0;
        DEM_SET_COMPONENTS_TO_ZERO_3x3(data_buffer.mLocalCoordSystem)
        DEM_SET_COMPONENTS_TO_ZERO_3x3(data_buffer.mOldLocalCoordSystem)
        array_1d<double, 3> wall_delta_disp_at_contact_point = ZeroVector(3);
        array_1d<double, 3> wall_velocity_at_contact_point = ZeroVector(3);
        bool sliding = false;

        double ini_delta = GetInitialDeltaWithFEM(i);
        double DistPToB = 0.0;

        int ContactType = -1;
        array_1d<double, 4>& Weight = this->mContactConditionWeights[i];

        rNeighbours[i]->ComputeConditionRelativeData(i, this, data_buffer.mLocalCoordSystem, DistPToB, Weight, wall_delta_disp_at_contact_point, wall_velocity_at_contact_point, ContactType);

        if (ContactType == 1 || ContactType == 2 || ContactType == 3) {

            double indentation = -(DistPToB - GetInteractionRadius()) - ini_delta;

            if (this->mIndentationInitialOption) {

                if (data_buffer.mTime < (0.0 + 2.0 * data_buffer.mDt)){
                    if (indentation > 0.0) {
                        this->mIndentationInitialWall[static_cast<int>(rNeighbours[i]->Id())] = indentation;
                    } 
                }

                if (this->mIndentationInitialWall.find(static_cast<int>(rNeighbours[i]->Id())) != this->mIndentationInitialWall.end()){
                   indentation -= this->mIndentationInitialWall[static_cast<int>(rNeighbours[i]->Id())];
                } 
            }

            double DeltDisp[3] = {0.0};
            double DeltVel [3] = {0.0};

            DeltVel[0] = velocity[0] - wall_velocity_at_contact_point[0];
            DeltVel[1] = velocity[1] - wall_velocity_at_contact_point[1];
            DeltVel[2] = velocity[2] - wall_velocity_at_contact_point[2];

            // For translation movement delta displacement
            const array_1d<double, 3>& delta_displ  = central_node.FastGetSolutionStepValue(DELTA_DISPLACEMENT);
            DeltDisp[0] = delta_displ[0] - wall_delta_disp_at_contact_point[0];
            DeltDisp[1] = delta_displ[1] - wall_delta_disp_at_contact_point[1];
            DeltDisp[2] = delta_displ[2] - wall_delta_disp_at_contact_point[2];

            if (this->Is(DEMFlags::HAS_ROTATION)) {
                const array_1d<double,3> negative_delta_rotation = -1.0 * central_node.FastGetSolutionStepValue(DELTA_ROTATION);
                array_1d<double, 3> actual_arm_vector, old_arm_vector;
                actual_arm_vector[0] = -data_buffer.mLocalCoordSystem[2][0] * DistPToB;
                actual_arm_vector[1] = -data_buffer.mLocalCoordSystem[2][1] * DistPToB;
                actual_arm_vector[2] = -data_buffer.mLocalCoordSystem[2][2] * DistPToB;

                double tangential_vel[3]           = {0.0};
                double tangential_displacement_due_to_rotation[3]  = {0.0};
                GeometryFunctions::CrossProduct(AngularVel, actual_arm_vector, tangential_vel);

                Quaternion<double> NegativeDeltaOrientation = Quaternion<double>::Identity();
                GeometryFunctions::OrientationFromRotationAngle(NegativeDeltaOrientation, negative_delta_rotation);

                NegativeDeltaOrientation.RotateVector3(actual_arm_vector, old_arm_vector);

                // Contribution of the rotation
                tangential_displacement_due_to_rotation[0] = (actual_arm_vector[0] - old_arm_vector[0]);
                tangential_displacement_due_to_rotation[1] = (actual_arm_vector[1] - old_arm_vector[1]);
                tangential_displacement_due_to_rotation[2] = (actual_arm_vector[2] - old_arm_vector[2]);

                DEM_ADD_SECOND_TO_FIRST(DeltVel, tangential_vel)
                DEM_ADD_SECOND_TO_FIRST(DeltDisp, tangential_displacement_due_to_rotation)
            }

            double LocalDeltDisp[3] = {0.0};
            GeometryFunctions::VectorGlobal2Local(data_buffer.mLocalCoordSystem, DeltDisp, LocalDeltDisp);

            double OldLocalElasticContactForce[3] = {0.0};

            GeometryFunctions::VectorGlobal2Local(data_buffer.mLocalCoordSystem, mNeighbourRigidFacesElasticContactForce[i], OldLocalElasticContactForce);
            const double previous_indentation = indentation + LocalDeltDisp[2];
            data_buffer.mLocalRelVel[0] = 0.0;
            data_buffer.mLocalRelVel[1] = 0.0;
            data_buffer.mLocalRelVel[2] = 0.0;

            if (indentation > 0.0) {

                GeometryFunctions::VectorGlobal2Local(data_buffer.mLocalCoordSystem, DeltVel, data_buffer.mLocalRelVel);
                mDiscontinuumConstitutiveLaw = pCloneDiscontinuumConstitutiveLawWithFEMNeighbour(wall);
                mDiscontinuumConstitutiveLaw->CalculateForcesWithFEM(r_process_info,
                                                                    OldLocalElasticContactForce,
                                                                    LocalElasticContactForce,
                                                                    LocalDeltDisp,
                                                                    data_buffer.mLocalRelVel,
                                                                    indentation,
                                                                    previous_indentation,
                                                                    ViscoDampingLocalContactForce,
                                                                    cohesive_force,
                                                                    this,
                                                                    wall,
                                                                    sliding);
            }

            double LocalContactForce[3]  = {0.0};
            double GlobalContactForce[3] = {0.0};

            AddUpFEMForcesAndProject(data_buffer.mLocalCoordSystem, LocalContactForce, LocalElasticContactForce, GlobalContactForce,
                                     GlobalElasticContactForce, ViscoDampingLocalContactForce, cohesive_force, r_elastic_force,
                                     r_contact_force, mNeighbourRigidFacesElasticContactForce[i], mNeighbourRigidFacesTotalContactForce[i]);

            rigid_element_force[0] -= GlobalContactForce[0];
            rigid_element_force[1] -= GlobalContactForce[1];
            rigid_element_force[2] -= GlobalContactForce[2];

            // ROTATION FORCES AND ROLLING FRICTION
            if (this->Is(DEMFlags::HAS_ROTATION)) {
              ComputeMomentsWithWalls(LocalContactForce[2], GlobalContactForce, data_buffer.mLocalCoordSystem[2], wall, indentation, i); //WARNING: sending itself as the neighbor!!
              
              if (this->Is(DEMFlags::HAS_ROLLING_FRICTION) && !data_buffer.mMultiStageRHS) {
                if (mRollingFrictionModel->CheckIfThisModelRequiresRecloningForEachNeighbour()){
                  mRollingFrictionModel = pCloneRollingFrictionModelWithFEMNeighbour(wall);                
                  mRollingFrictionModel->ComputeRollingFrictionWithWall(this, wall, r_process_info, LocalContactForce, indentation, mContactMoment);
                }
                else {
                  mRollingFrictionModel->ComputeRollingResistanceWithWall(this, wall, LocalContactForce);
                }      
              }
            }

            //WEAR
            if (wall->GetProperties()[COMPUTE_WEAR]) {
                const double area              = Globals::Pi * GetInteractionRadius() * GetInteractionRadius();
                const double inverse_of_volume = 1.0 / (4.0 * 0.333333333333333 * area * GetInteractionRadius());
                ComputeWear(data_buffer.mLocalRelVel,
                            data_buffer.mDt,
                            sliding,
                            inverse_of_volume,
                            LocalElasticContactForce[2],
                            wall);
            } //wall->GetProperties()[COMPUTE_WEAR] if

            if (this->Is(DEMFlags::HAS_STRESS_TENSOR)) {
                AddWallContributionToStressTensor(GlobalContactForce, data_buffer.mLocalCoordSystem[2], DistPToB, 0.0);
            }

            // Store contact information needed for later processes
            StoreBallToRigidFaceContactInfo(r_process_info, data_buffer, GlobalContactForce, LocalContactForce, ViscoDampingLocalContactForce, sliding);

        } //ContactType if
    } //rNeighbours.size loop

    auto& list_of_point_condition_pointers = this->GetValue(WALL_POINT_CONDITION_POINTERS);
    auto& neighbour_point_faces_elastic_contact_force = this->GetValue(WALL_POINT_CONDITION_ELASTIC_FORCES);
    auto& neighbour_point_faces_total_contact_force = this->GetValue(WALL_POINT_CONDITION_TOTAL_FORCES);

    for (unsigned int i = 0; i < list_of_point_condition_pointers.size(); i++) {
        Condition* wall = list_of_point_condition_pointers[i];

        array_1d<double, 3> wall_coordinates = wall->GetGeometry().Center();

        double RelVel[3] = { 0.0 };
        double LocalElasticContactForce[3] = { 0.0 };
        double GlobalElasticContactForce[3] = { 0.0 };
        double ViscoDampingLocalContactForce[3] = { 0.0 };
        double cohesive_force = 0.0;

        DEM_SET_COMPONENTS_TO_ZERO_3x3(data_buffer.mLocalCoordSystem)
        DEM_SET_COMPONENTS_TO_ZERO_3x3(data_buffer.mOldLocalCoordSystem)

        const Matrix& r_N = wall->GetGeometry().ShapeFunctionsValues();

        array_1d<double, 3> wall_delta_disp_at_contact_point = ZeroVector(3);
        for (IndexType i = 0; i < wall->GetGeometry().size(); ++i) {
            wall_delta_disp_at_contact_point -= r_N(0, i)*wall->GetGeometry()[i].GetSolutionStepValue(DELTA_DISPLACEMENT, 0);
        }

        array_1d<double, 3> wall_velocity_at_contact_point = ZeroVector(3);
        for (IndexType i = 0; i < wall->GetGeometry().size(); ++i) {
            wall_velocity_at_contact_point += r_N(0, i) * wall->GetGeometry()[i].GetSolutionStepValue(VELOCITY, 0);
        }

        bool sliding = false;
        double ini_delta = GetInitialDeltaWithFEM(i);

        array_1d<double, 3> cond_to_me_vect;
        noalias(cond_to_me_vect) = central_node.Coordinates() - wall_coordinates;

        double DistPToB = DEM_MODULUS_3(cond_to_me_vect);

        double indentation = -(DistPToB - GetInteractionRadius()) - ini_delta;

        double DeltDisp[3] = { 0.0 };
        double DeltVel[3] = { 0.0 };

        DeltVel[0] = velocity[0] - wall_velocity_at_contact_point[0];
        DeltVel[1] = velocity[1] - wall_velocity_at_contact_point[1];
        DeltVel[2] = velocity[2] - wall_velocity_at_contact_point[2];

        // For translation movement delta displacement
        const array_1d<double, 3>& delta_displ = central_node.FastGetSolutionStepValue(DELTA_DISPLACEMENT);
        DeltDisp[0] = delta_displ[0] - wall_delta_disp_at_contact_point[0];
        DeltDisp[1] = delta_displ[1] - wall_delta_disp_at_contact_point[1];
        DeltDisp[2] = delta_displ[2] - wall_delta_disp_at_contact_point[2];

        Node::Pointer other_particle_node = this->GetGeometry()[0].Clone();
        other_particle_node->GetSolutionStepValue(DELTA_DISPLACEMENT) = wall_delta_disp_at_contact_point;
        data_buffer.mpOtherParticleNode = &*other_particle_node;
        DEM_COPY_SECOND_TO_FIRST_3(data_buffer.mOtherToMeVector, cond_to_me_vect)
        data_buffer.mDistance = DistPToB;
        data_buffer.mDomainIsPeriodic = false;
        EvaluateDeltaDisplacement(data_buffer, DeltDisp, RelVel, data_buffer.mLocalCoordSystem, data_buffer.mOldLocalCoordSystem, velocity, delta_displ);

        if (this->Is(DEMFlags::HAS_ROTATION)) {
            const array_1d<double, 3>& delta_rotation = central_node.FastGetSolutionStepValue(DELTA_ROTATION);

            array_1d<double, 3> actual_arm_vector;
            actual_arm_vector[0] = -data_buffer.mLocalCoordSystem[2][0] * DistPToB;
            actual_arm_vector[1] = -data_buffer.mLocalCoordSystem[2][1] * DistPToB;
            actual_arm_vector[2] = -data_buffer.mLocalCoordSystem[2][2] * DistPToB;

            double tangential_vel[3] = { 0.0 };
            double tangential_displacement_due_to_rotation[3] = { 0.0 };
            GeometryFunctions::CrossProduct(AngularVel, actual_arm_vector, tangential_vel);
            GeometryFunctions::CrossProduct(delta_rotation, actual_arm_vector, tangential_displacement_due_to_rotation);

            DEM_ADD_SECOND_TO_FIRST(DeltVel, tangential_vel)
            DEM_ADD_SECOND_TO_FIRST(DeltDisp, tangential_displacement_due_to_rotation)
        }

        double LocalDeltDisp[3] = { 0.0 };
        GeometryFunctions::VectorGlobal2Local(data_buffer.mLocalCoordSystem, DeltDisp, LocalDeltDisp);

        double OldLocalElasticContactForce[3] = { 0.0 };

        GeometryFunctions::VectorGlobal2Local(data_buffer.mLocalCoordSystem, neighbour_point_faces_elastic_contact_force[i], OldLocalElasticContactForce);
        const double previous_indentation = indentation + LocalDeltDisp[2];
        data_buffer.mLocalRelVel[0] = 0.0;
        data_buffer.mLocalRelVel[1] = 0.0;
        data_buffer.mLocalRelVel[2] = 0.0;

        if (indentation > 0.0)
        {
            GeometryFunctions::VectorGlobal2Local(data_buffer.mLocalCoordSystem, DeltVel, data_buffer.mLocalRelVel);
            mDiscontinuumConstitutiveLaw = pCloneDiscontinuumConstitutiveLawWithFEMNeighbour(wall);
            mDiscontinuumConstitutiveLaw->CalculateForcesWithFEM(
                r_process_info,
                OldLocalElasticContactForce,
                LocalElasticContactForce,
                LocalDeltDisp,
                data_buffer.mLocalRelVel,
                indentation,
                previous_indentation,
                ViscoDampingLocalContactForce,
                cohesive_force,
                this,
                wall,
                sliding);
        }

        double LocalContactForce[3] = { 0.0 };
        double GlobalContactForce[3] = { 0.0 };

        AddUpFEMForcesAndProject(
            data_buffer.mLocalCoordSystem,
            LocalContactForce,
            LocalElasticContactForce,
            GlobalContactForce,
            GlobalElasticContactForce,
            ViscoDampingLocalContactForce,
            cohesive_force,
            r_elastic_force,
            r_contact_force,
            neighbour_point_faces_elastic_contact_force[i],
            neighbour_point_faces_total_contact_force[i]);

        rigid_element_force[0] -= GlobalContactForce[0];
        rigid_element_force[1] -= GlobalContactForce[1];
        rigid_element_force[2] -= GlobalContactForce[2];

        Vector GlobalContactForceVector(3);
        DEM_COPY_SECOND_TO_FIRST_3(GlobalContactForceVector, GlobalContactForce)
        wall->SetValue(EXTERNAL_FORCES_VECTOR, GlobalContactForceVector);

        if (this->Is(DEMFlags::HAS_ROTATION)) {
            ComputeMomentsWithWalls(LocalContactForce[2], GlobalContactForce, data_buffer.mLocalCoordSystem[2], wall, indentation, i); //WARNING: sending itself as the neighbor!!
        }

        if (this->Is(DEMFlags::HAS_STRESS_TENSOR)) {
            AddWallContributionToStressTensor(GlobalContactForce, data_buffer.mLocalCoordSystem[2], DistPToB, 0.0);
        }
    }

    KRATOS_CATCH("")
}// ComputeBallToRigidFaceContactForceAndMoment

void SphericParticle::RenewData()
{
  //To be redefined
}
void SphericParticle::ComputeConditionRelativeData(int rigid_neighbour_index,   // check for delete
                                                    DEMWall* const wall,
                                                    double LocalCoordSystem[3][3],
                                                    double& DistPToB,
                                                    array_1d<double, 4>& Weight,
                                                    array_1d<double, 3>& wall_delta_disp_at_contact_point,
                                                    array_1d<double, 3>& wall_velocity_at_contact_point,
                                                    int& ContactType)
{
    size_t FE_size = wall->GetGeometry().size();

    std::vector<double> TempWeight;
    TempWeight.resize(FE_size);

    double total_weight = 0.0;
    int points = 0;
    int inode1 = 0, inode2 = 0;

    for (unsigned int inode = 0; inode < FE_size; inode++) {

        if (Weight[inode] > 1.0e-12) {
            total_weight = total_weight + Weight[inode];
            points++;
            if (points == 1) {inode1 = inode;}
            if (points == 2) {inode2 = inode;}
        }

        if (fabs(total_weight - 1.0) < 1.0e-12) {
            break;
        }
    }

    bool contact_exists = true;
    array_1d<double, 3>& node_coordinates = this->GetGeometry()[0].Coordinates();

    const double radius = this->GetInteractionRadius();

    if (points == 3 || points == 4)
    {
        unsigned int dummy_current_edge_index;
        bool is_inside;
        contact_exists = GeometryFunctions::FacetCheck(wall->GetGeometry(), node_coordinates, radius, LocalCoordSystem, DistPToB, TempWeight, dummy_current_edge_index, is_inside);
        ContactType = 1;
        Weight[0]=TempWeight[0];
        Weight[1]=TempWeight[1];
        Weight[2]=TempWeight[2];
        if (points == 4)
        {
            Weight[3] = TempWeight[3];
        }
        else
        {
            Weight[3] = 0.0;
        }
    }

    else if (points == 2) {

        double eta = 0.0;
        contact_exists = GeometryFunctions::EdgeCheck(wall->GetGeometry()[inode1], wall->GetGeometry()[inode2], node_coordinates, radius, LocalCoordSystem, DistPToB, eta);

        Weight[inode1] = 1-eta;
        Weight[inode2] = eta;
        ContactType = 2;

    }

    else if (points == 1) {
        contact_exists = GeometryFunctions::VertexCheck(wall->GetGeometry()[inode1], node_coordinates, radius, LocalCoordSystem, DistPToB);
        Weight[inode1] = 1.0;
        ContactType = 3;
    }

    if (contact_exists == false) {ContactType = -1;}

    for (std::size_t inode = 0; inode < FE_size; inode++) {
        noalias(wall_velocity_at_contact_point) += wall->GetGeometry()[inode].FastGetSolutionStepValue(VELOCITY) * Weight[inode];

        array_1d<double, 3>  wall_delta_displacement = ZeroVector(3);
        wall->GetDeltaDisplacement(wall_delta_displacement, inode);
        noalias(wall_delta_disp_at_contact_point) += wall_delta_displacement* Weight[inode];

    }
} //ComputeConditionRelativeData

void SphericParticle::ComputeWear(double LocalRelVel[3],
                                  double mTimeStep,
                                  bool sliding,
                                  double inverse_of_volume,
                                  double LocalElasticContactForce,
                                  DEMWall* wall) {

    array_1d<double, 3>& sphere_center = this->GetGeometry()[0].Coordinates();
    double volume_wear = 0.0;

    Properties& properties_of_this_contact = GetProperties().GetSubProperties(wall->GetProperties().Id());
    const double WallSeverityOfWear       = properties_of_this_contact[SEVERITY_OF_WEAR];
    const double WallImpactSeverityOfWear = properties_of_this_contact[IMPACT_WEAR_SEVERITY];
    const double WallBrinellHardness      = properties_of_this_contact[BRINELL_HARDNESS];
    double InverseOfWallBrinellHardness;

    if (WallBrinellHardness) InverseOfWallBrinellHardness = 1.0 / WallBrinellHardness;
    else KRATOS_ERROR << "Brinell hardness cannot be zero!";

    const double Sliding_0 = LocalRelVel[0] * mTimeStep;
    const double Sliding_1 = LocalRelVel[1] * mTimeStep;

    double impact_wear = WallImpactSeverityOfWear * InverseOfWallBrinellHardness * GetDensity() * mRadius * std::abs(LocalRelVel[2]);
    if (sliding) volume_wear = WallSeverityOfWear * InverseOfWallBrinellHardness * std::abs(LocalElasticContactForce) * sqrt(Sliding_0 * Sliding_0 + Sliding_1 * Sliding_1);

    const double element_area = wall->GetGeometry().Area();

    if (element_area) {
        impact_wear /= element_area;
        volume_wear /= element_area;
    } else KRATOS_ERROR << "A wall element with zero area was found!";

    //Computing the projected point
    array_1d<double, 3> inner_point = ZeroVector(3);
    array_1d<double, 3> relative_vector = wall->GetGeometry()[0].Coordinates() - sphere_center; //We could have chosen also [1] or [2].
    const unsigned int line_dimension = 2;

    if (wall->GetGeometry().size() > line_dimension) {

        array_1d<double, 3> normal_to_wall;
        wall->CalculateNormal(normal_to_wall);
        const double dot_product = DEM_INNER_PRODUCT_3(relative_vector, normal_to_wall);
        DEM_MULTIPLY_BY_SCALAR_3(normal_to_wall, dot_product);

        inner_point = sphere_center + normal_to_wall;

    } else {
        // Projection on a line element
        const double numerical_limit = std::numeric_limits<double>::epsilon();
        array_1d<double, 3> line_vector = wall->GetGeometry()[1].Coordinates()-wall->GetGeometry()[0].Coordinates();
        KRATOS_ERROR_IF(wall->GetGeometry().Length()<=numerical_limit) << "Line element has zero length" << std::endl;
        line_vector/=wall->GetGeometry().Length();

        DEM_COPY_SECOND_TO_FIRST_3(inner_point,line_vector);
        const double dot_product = DEM_INNER_PRODUCT_3(relative_vector, line_vector);
        DEM_MULTIPLY_BY_SCALAR_3(inner_point, dot_product);
        DEM_ADD_SECOND_TO_FIRST(inner_point,wall->GetGeometry()[0].Coordinates());
    }

    array_1d<double, 3> point_local_coordinates;
    Vector shape_functions_coefs(3);
    wall->GetGeometry().PointLocalCoordinates(point_local_coordinates, inner_point);
    wall->GetGeometry().ShapeFunctionsValues(shape_functions_coefs, point_local_coordinates);

    if ((shape_functions_coefs[0] >= 0.0) && (shape_functions_coefs[1] >= 0.0) && (shape_functions_coefs[2] >= 0.0)) { // Which means the ball is inside this element
                                                                                                                       // This avoids negative shape functions that would give
                                                                                                                       // negative wear values
        auto& n0 = wall->GetGeometry()[0];
        n0.SetLock();
        n0.FastGetSolutionStepValue(NON_DIMENSIONAL_VOLUME_WEAR) += shape_functions_coefs[0] * volume_wear;
        n0.FastGetSolutionStepValue(IMPACT_WEAR) += shape_functions_coefs[0] * impact_wear;
        n0.UnSetLock();

        auto& n1 = wall->GetGeometry()[1];
        n1.SetLock();
        n1.FastGetSolutionStepValue(NON_DIMENSIONAL_VOLUME_WEAR) += shape_functions_coefs[1] * volume_wear;
        n1.FastGetSolutionStepValue(IMPACT_WEAR) += shape_functions_coefs[1] * impact_wear;
        n1.UnSetLock();

        auto& n2 = wall->GetGeometry()[2];
        n2.SetLock();
        n2.FastGetSolutionStepValue(NON_DIMENSIONAL_VOLUME_WEAR) += shape_functions_coefs[2] * volume_wear;
        n2.FastGetSolutionStepValue(IMPACT_WEAR) += shape_functions_coefs[2] * impact_wear;
        n2.UnSetLock();
    }
} //ComputeWear

void SphericParticle::CalculateDampingMatrix(MatrixType& rDampingMatrix, const ProcessInfo& r_process_info){}

void SphericParticle::GetDofList(DofsVectorType& ElementalDofList, const ProcessInfo& r_process_info) const {
    KRATOS_TRY

    ElementalDofList.resize(0);

    for (unsigned int i = 0; i < GetGeometry().size(); i++) {
        ElementalDofList.push_back(GetGeometry()[i].pGetDof(VELOCITY_X));
        ElementalDofList.push_back(GetGeometry()[i].pGetDof(VELOCITY_Y));

        if (GetGeometry().WorkingSpaceDimension() == 3){
            ElementalDofList.push_back(GetGeometry()[i].pGetDof(VELOCITY_Z));
        }

        ElementalDofList.push_back(GetGeometry()[i].pGetDof(ANGULAR_VELOCITY_X));
        ElementalDofList.push_back(GetGeometry()[i].pGetDof(ANGULAR_VELOCITY_Y));

        if (GetGeometry().WorkingSpaceDimension() == 3){
            ElementalDofList.push_back(GetGeometry()[i].pGetDof(ANGULAR_VELOCITY_Z));
        }
    }

    KRATOS_CATCH("")
}

void SphericParticle::InitializeSolutionStep(const ProcessInfo& r_process_info)
{
    KRATOS_TRY

    auto& central_node = GetGeometry()[0];
    mRadius = central_node.FastGetSolutionStepValue(RADIUS); //Just in case someone is overwriting the radius in Python
    mPartialRepresentativeVolume = 0.0;
    central_node.FastGetSolutionStepValue(REPRESENTATIVE_VOLUME) = CalculateVolume();
    double& elastic_energy = this->GetElasticEnergy();
    elastic_energy = 0.0;
    double& max_normal_ball_to_ball_force_times_radius = this->GetMaxNormalBallToBallForceTimesRadius();
    max_normal_ball_to_ball_force_times_radius = 0.0;
    if (this->Is(DEMFlags::HAS_STRESS_TENSOR)) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                (*mStressTensor)(i,j) = 0.0;
            }
        }
    }

    if (this->Is(DEMFlags::HAS_ROTATION)) {
        if (this->Is(DEMFlags::HAS_ROLLING_FRICTION)) {
            if (mRollingFrictionModel != nullptr){
                mRollingFrictionModel->InitializeSolutionStep();
            }
        }
    }

    KRATOS_CATCH("")
}

void SphericParticle::AddNeighbourContributionToStressTensor(const ProcessInfo& r_process_info,
                                                            const double Force[3],
                                                            const double other_to_me_vect[3],
                                                            const double distance,
                                                            const double radius_sum,
                                                            SphericParticle* element) {
    KRATOS_TRY

    double gap = distance - radius_sum;
    double real_distance = GetInteractionRadius() + 0.5 * gap;

    array_1d<double, 3> normal_vector_on_contact;
    normal_vector_on_contact[0] = - other_to_me_vect[0]; //outwards
    normal_vector_on_contact[1] = - other_to_me_vect[1]; //outwards
    normal_vector_on_contact[2] = - other_to_me_vect[2]; //outwards

    array_1d<double, 3> x_centroid = real_distance * normal_vector_on_contact;

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            (*mStressTensor)(i,j) += x_centroid[j] * Force[i]; //ref: Katalin Bagi 1995 Mean stress tensor
        }
    }
    KRATOS_CATCH("")
}

void SphericParticle::AddWallContributionToStressTensor(const double Force[3],
                                                        const double other_to_me_vect[3],
                                                        const double distance,
                                                        const double contact_area) {

    KRATOS_TRY

    double& rRepresentative_Volume = this->GetGeometry()[0].FastGetSolutionStepValue(REPRESENTATIVE_VOLUME);
    rRepresentative_Volume += 0.33333333333333 * (distance * contact_area);

    array_1d<double, 3> normal_vector_on_contact;
    normal_vector_on_contact[0] = -1 * other_to_me_vect[0]; //outwards
    normal_vector_on_contact[1] = -1 * other_to_me_vect[1]; //outwards
    normal_vector_on_contact[2] = -1 * other_to_me_vect[2]; //outwards

    array_1d<double, 3> x_centroid = distance * normal_vector_on_contact;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            (*mStressTensor)(i,j) += (x_centroid[j]) * Force[i]; //ref: Katalin Bagi 1995 Mean stress tensor
        }
    }

    KRATOS_CATCH("")
}

void SphericParticle::CorrectRepresentativeVolume(double& rRepresentative_Volume/*, bool& is_smaller_than_sphere*/) {

    KRATOS_TRY

    const double sphere_volume = CalculateVolume();

    if ((rRepresentative_Volume <= sphere_volume)) { //In case it gets 0.0 (discontinuum). Also sometimes the error can be too big. This puts some bound to the error for continuum.
        rRepresentative_Volume = sphere_volume;
        //is_smaller_than_sphere = true;
    }

    KRATOS_CATCH("")
}

void SphericParticle::FinalizeSolutionStep(const ProcessInfo& r_process_info){

    KRATOS_TRY

    ComputeReactions();

    auto& central_node = GetGeometry()[0];

    central_node.FastGetSolutionStepValue(REPRESENTATIVE_VOLUME) = mPartialRepresentativeVolume;
    double& rRepresentative_Volume = central_node.FastGetSolutionStepValue(REPRESENTATIVE_VOLUME);

    //bool is_smaller_than_sphere = false;
    CorrectRepresentativeVolume(rRepresentative_Volume/*, is_smaller_than_sphere*/);

    if (this->Is(DEMFlags::HAS_STRESS_TENSOR)) {

        //Divide Stress Tensor by the total volume:
        //const array_1d<double, 3>& reaction_force=central_node.FastGetSolutionStepValue(FORCE_REACTION);

        //make a copy
        if (this->Is(DEMFlags::PRINT_STRESS_TENSOR)) {
            this->GetGeometry()[0].FastGetSolutionStepValue(DEM_STRESS_TENSOR_RAW) = (*mStressTensor);
        }

        //KRATOS_WATCH(this->GetGeometry()[0].FastGetSolutionStepValue(DEM_STRESS_TENSOR_RAW))

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                (*mStressTensor)(i,j) /= rRepresentative_Volume;
            }
            //(*mStressTensor)(i,i) += GeometryFunctions::sign( (*mStressTensor)(i,i) ) * GetRadius() * fabs(reaction_force[i]) / rRepresentative_Volume;
        }
        /*if( this->Is(DEMFlags::HAS_ROTATION) ) { //THIS IS GIVING STABILITY PROBLEMS WHEN USING THE EXTRA TERMS FOR CONTINUUM
            const array_1d<double, 3>& reaction_moment=central_node.FastGetSolutionStepValue(MOMENT_REACTION);
            const double fabs_reaction_moment_modulus = fabs( DEM_MODULUS_3(reaction_moment) );
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if(i!=j){
                        (*mStressTensor)(i,j) += GeometryFunctions::sign( (*mStressTensor)(i,j) ) * fabs_reaction_moment_modulus / rRepresentative_Volume;
                    }
                }
            }
        }*/

        ComputeDifferentialStrainTensor(r_process_info);
        SymmetrizeDifferentialStrainTensor();
        ComputeStrainTensor(r_process_info);

        FinalizeStressTensor(r_process_info, rRepresentative_Volume);
        SymmetrizeStressTensor();
    }
    KRATOS_CATCH("")
}

void SphericParticle::ComputeStrainTensor(const ProcessInfo& r_process_info) {

    const int Dim = r_process_info[DOMAIN_SIZE];
    for (int i = 0; i < Dim; i++) {
        for (int j = 0; j < Dim; j++) {
            (*mStrainTensor)(i,j) += (*mDifferentialStrainTensor)(i,j);
        }
    }
}

void SphericParticle::ComputeDifferentialStrainTensor(const ProcessInfo& r_process_info) {

    // Following Bagi, Katalin. "Analysis of microstructural strain tensors for granular assemblies". 2005. Section 2.3.1.
    const int Dim = r_process_info[DOMAIN_SIZE];
    array_1d<double, 3> assembly_centroid = this->GetGeometry()[0].Coordinates();
    array_1d<double, 3> assembly_average_delta_displacement = this->GetGeometry()[0].FastGetSolutionStepValue(DELTA_DISPLACEMENT);
    int total_number_of_neighbours = 0;
    for (unsigned int i = 0; i < mNeighbourElements.size(); i++) {
        if (!mNeighbourElements[i]) continue;
        SphericParticle* neighbour_iterator = dynamic_cast<SphericParticle*>(mNeighbourElements[i]);
        array_1d<double, 3> node_coordinates = neighbour_iterator->GetGeometry()[0].Coordinates();
        assembly_centroid += node_coordinates;
        array_1d<double, 3> node_delta_displacement = neighbour_iterator->GetGeometry()[0].FastGetSolutionStepValue(DELTA_DISPLACEMENT);
        assembly_average_delta_displacement += node_delta_displacement;
        total_number_of_neighbours++;
    }
    
    if (total_number_of_neighbours < Dim) {
        *mDifferentialStrainTensor = ZeroMatrix(3,3);
    } else {
        BoundedMatrix<double, 3, 3> CoefficientsMatrix = ZeroMatrix(3, 3);
        BoundedMatrix<double, 3, 3> RightHandSide = ZeroMatrix(3, 3);
        assembly_centroid /= (1.0 + total_number_of_neighbours);  
        assembly_average_delta_displacement /= (1.0 + total_number_of_neighbours);

        array_1d<double, 3> relative_position = this->GetGeometry()[0].Coordinates() - assembly_centroid;
        array_1d<double, 3> relative_delta_displacement = this->GetGeometry()[0].FastGetSolutionStepValue(DELTA_DISPLACEMENT) - assembly_average_delta_displacement;
        for (int i = 0; i < Dim; i++) {
            for (int j = 0; j < Dim; j++) {
                CoefficientsMatrix(j,i) += relative_position[j] * relative_position[i];
                RightHandSide(j,i) += relative_delta_displacement[i] * relative_position[j];
            }
        }

        for (unsigned int i = 0; i < mNeighbourElements.size(); i++) {
            if (!mNeighbourElements[i]) continue;
            relative_position = mNeighbourElements[i]->GetGeometry()[0].Coordinates() - assembly_centroid;
            relative_delta_displacement = mNeighbourElements[i]->GetGeometry()[0].FastGetSolutionStepValue(DELTA_DISPLACEMENT) - assembly_average_delta_displacement;
            for (int i = 0; i < Dim; i++) {
                for (int j = 0; j < Dim; j++) {
                    CoefficientsMatrix(j,i) += relative_position[j] * relative_position[i];
                    RightHandSide(j,i) += relative_delta_displacement[i] * relative_position[j];
                }
            }
        }

        double det = 0.0;
        BoundedMatrix<double, 3, 3> InvertedCoefficientsMatrix = ZeroMatrix(3, 3);
        if (Dim == 2) {
            CoefficientsMatrix(2,2) = 1.0;
            RightHandSide(2,2) = 1.0;
        }
        MathUtils<double>::InvertMatrix3(CoefficientsMatrix, InvertedCoefficientsMatrix, det);
        *mDifferentialStrainTensor = prod(InvertedCoefficientsMatrix, RightHandSide);
        if (Dim == 2) {
            (*mDifferentialStrainTensor)(2,2) = (*mDifferentialStrainTensor)(0,2) = (*mDifferentialStrainTensor)(1,2) = (*mDifferentialStrainTensor)(2,1) = (*mDifferentialStrainTensor)(2,0) = 0.0;
        }
    }
}

void SphericParticle::SymmetrizeDifferentialStrainTensor() {
    
    for (int i = 0; i < 3; i++) {
        for (int j = i; j < 3; j++) {
            (*mDifferentialStrainTensor)(i,j) = 0.5 * ((*mDifferentialStrainTensor)(i,j) + (*mDifferentialStrainTensor)(j,i));
        }
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < i; j++) {
            (*mDifferentialStrainTensor)(i,j) = (*mDifferentialStrainTensor)(j,i);
        }
    }
}

void SphericParticle::SymmetrizeStressTensor(){
    //The following operation symmetrizes the tensor. We will work with the symmetric stress tensor always, because the non-symmetric one is being filled while forces are being calculated
    for (int i = 0; i < 3; i++) {
        for (int j = i; j < 3; j++) {
            if(fabs((*mStressTensor)(i,j)) > fabs((*mStressTensor)(j,i))) {
                (*mSymmStressTensor)(i,j) = (*mSymmStressTensor)(j,i) = (*mStressTensor)(i,j);
            }
            else {
                (*mSymmStressTensor)(i,j) = (*mSymmStressTensor)(j,i) = (*mStressTensor)(j,i);
            }
        }
    }

    /*for (int i = 0; i < 3; i++) {
        for (int j = i; j < 3; j++) {
            (*mSymmStressTensor)(i,j) = (*mSymmStressTensor)(j,i) = 0.5 * ((*mStressTensor)(i,j) + (*mStressTensor)(j,i));
        }
    }*/
}

void SphericParticle::ComputeReactions() {

    KRATOS_TRY

    Node& node = GetGeometry()[0];
    array_1d<double, 3>& reaction_force = node.FastGetSolutionStepValue(FORCE_REACTION);
    array_1d<double, 3>& r_total_forces = node.FastGetSolutionStepValue(TOTAL_FORCES);
    reaction_force[0] = node.Is(DEMFlags::FIXED_VEL_X) * (-r_total_forces[0]);
    reaction_force[1] = node.Is(DEMFlags::FIXED_VEL_Y) * (-r_total_forces[1]);
    reaction_force[2] = node.Is(DEMFlags::FIXED_VEL_Z) * (-r_total_forces[2]);

    if (this->Is(DEMFlags::HAS_ROTATION)) {
        array_1d<double, 3>& reaction_moment = node.FastGetSolutionStepValue(MOMENT_REACTION);
        array_1d<double, 3>& r_total_moment = node.FastGetSolutionStepValue(PARTICLE_MOMENT);
        reaction_moment[0] = node.Is(DEMFlags::FIXED_ANG_VEL_X) * (-r_total_moment[0]);
        reaction_moment[1] = node.Is(DEMFlags::FIXED_ANG_VEL_Y) * (-r_total_moment[1]);
        reaction_moment[2] = node.Is(DEMFlags::FIXED_ANG_VEL_Z) * (-r_total_moment[2]);
    }

    KRATOS_CATCH("")
}

void SphericParticle::PrepareForPrinting(const ProcessInfo& r_process_info){

    if (this->GetGeometry()[0].SolutionStepsDataHas(IS_STICKY)) {
        this->GetGeometry()[0].FastGetSolutionStepValue(IS_STICKY) = this->Is(DEMFlags::STICKY);
    }

    if (this->Is(DEMFlags::PRINT_STRESS_TENSOR)) {
        this->GetGeometry()[0].FastGetSolutionStepValue(DEM_STRESS_TENSOR) = (*mSymmStressTensor);
        this->GetGeometry()[0].FastGetSolutionStepValue(DEM_STRAIN_TENSOR) = (*mStrainTensor);
        this->GetGeometry()[0].FastGetSolutionStepValue(DEM_DIFFERENTIAL_STRAIN_TENSOR) = (*mDifferentialStrainTensor);
    }
}

void SphericParticle::ComputeAdditionalForces(array_1d<double, 3>& externally_applied_force,
                                            array_1d<double, 3>& externally_applied_moment,
                                            const ProcessInfo& r_process_info,
                                            const array_1d<double,3>& gravity)
{
    KRATOS_TRY

    if (this->Is(DEMFlags::CUMULATIVE_ZONE)) {
        const array_1d<double,3> gravity_force = ComputeWeight(gravity, r_process_info);
        const double gravity_force_magnitude = DEM_MODULUS_3(gravity_force);
        const array_1d<double, 3>& vel = this->GetGeometry()[0].FastGetSolutionStepValue(VELOCITY);
        const double vel_magnitude = DEM_MODULUS_3(vel);
        if (vel_magnitude != 0.0){
            const array_1d<double, 3> unitary_vel =  vel/vel_magnitude;
            const double inlet_damping_coefficient = 1e3;
            const array_1d<double, 3> damping_force = - inlet_damping_coefficient * GetMass() * vel_magnitude  * vel_magnitude * unitary_vel;
            const array_1d<double, 3> counter_force  = -5.0 * gravity_force_magnitude * unitary_vel;
            noalias(externally_applied_force)  += damping_force;
            noalias(externally_applied_force)  += counter_force;}
    } else {
        noalias(externally_applied_force)  += ComputeWeight(gravity, r_process_info);
        noalias(externally_applied_force)  += this->GetGeometry()[0].FastGetSolutionStepValue(EXTERNAL_APPLIED_FORCE);
        noalias(externally_applied_moment) += this->GetGeometry()[0].FastGetSolutionStepValue(EXTERNAL_APPLIED_MOMENT);
    }
    KRATOS_CATCH("")
}

array_1d<double,3> SphericParticle::ComputeWeight(const array_1d<double,3>& gravity, const ProcessInfo& r_process_info) {

    KRATOS_TRY

    return GetMass() * gravity;

    KRATOS_CATCH("")
}

void SphericParticle::AddUpForcesAndProject(double OldCoordSystem[3][3],
                                            double LocalCoordSystem[3][3],
                                            double LocalContactForce[3],
                                            double LocalElasticContactForce[3],
                                            double LocalElasticExtraContactForce[3],
                                            double GlobalContactForce[3],
                                            double GlobalElasticContactForce[3],
                                            double GlobalElasticExtraContactForce[3],
                                            double TotalGlobalElasticContactForce[3],
                                            double ViscoDampingLocalContactForce[3],
                                            const double cohesive_force,
                                            array_1d<double, 3>& other_ball_to_ball_forces,
                                            array_1d<double, 3>& r_elastic_force,
                                            array_1d<double, 3>& r_contact_force,
                                            const unsigned int i_neighbour_count,
                                            const ProcessInfo& r_process_info)
{

    for (unsigned int index = 0; index < 3; index++) {
        LocalContactForce[index] = LocalElasticContactForce[index] + ViscoDampingLocalContactForce[index] + other_ball_to_ball_forces[index];
    }
    LocalContactForce[2] -= cohesive_force;
  
    DEM_ADD_SECOND_TO_FIRST(LocalElasticContactForce, other_ball_to_ball_forces);

    GeometryFunctions::VectorLocal2Global(LocalCoordSystem, LocalElasticContactForce, GlobalElasticContactForce);
    GeometryFunctions::VectorLocal2Global(LocalCoordSystem, LocalContactForce, GlobalContactForce);
    GeometryFunctions::VectorLocal2Global(LocalCoordSystem, LocalElasticExtraContactForce, GlobalElasticExtraContactForce);

    // Saving contact forces (We need to, since tangential elastic force is history-dependent)
    DEM_COPY_SECOND_TO_FIRST_3(mNeighbourElasticContactForces[i_neighbour_count], GlobalElasticContactForce)
    DEM_COPY_SECOND_TO_FIRST_3(mNeighbourElasticExtraContactForces[i_neighbour_count], GlobalElasticExtraContactForce)

    TotalGlobalElasticContactForce[0] = GlobalElasticContactForce[0] + GlobalElasticExtraContactForce[0];
    TotalGlobalElasticContactForce[1] = GlobalElasticContactForce[1] + GlobalElasticExtraContactForce[1];
    TotalGlobalElasticContactForce[2] = GlobalElasticContactForce[2] + GlobalElasticExtraContactForce[2];
    DEM_ADD_SECOND_TO_FIRST(r_elastic_force, TotalGlobalElasticContactForce)

    double TotalGlobalContactForce[3];
    TotalGlobalContactForce[0] = GlobalContactForce[0] + GlobalElasticExtraContactForce[0];
    TotalGlobalContactForce[1] = GlobalContactForce[1] + GlobalElasticExtraContactForce[1];
    TotalGlobalContactForce[2] = GlobalContactForce[2] + GlobalElasticExtraContactForce[2];
    DEM_ADD_SECOND_TO_FIRST(r_contact_force, TotalGlobalContactForce )
}

void SphericParticle::AddUpMomentsAndProject(double LocalCoordSystem[3][3],
                                             double LocalElasticRotationalMoment[3],
                                             double LocalViscoRotationalMoment[3]) {

    double LocalContactRotationalMoment[3] = {0.0};
    double GlobalContactRotationalMoment[3] = {0.0};

    for (unsigned int index = 0; index < 3; index++) {
        LocalContactRotationalMoment[index] = LocalElasticRotationalMoment[index] + LocalViscoRotationalMoment[index];
    }

    GeometryFunctions::VectorLocal2Global(LocalCoordSystem, LocalContactRotationalMoment, GlobalContactRotationalMoment);
    
    DEM_ADD_SECOND_TO_FIRST(mContactMoment, GlobalContactRotationalMoment)

}

void SphericParticle::AddUpFEMForcesAndProject(double LocalCoordSystem[3][3],
                                               double LocalContactForce[3],
                                               double LocalElasticContactForce[3],
                                               double GlobalContactForce[3],
                                               double GlobalElasticContactForce[3],
                                               double ViscoDampingLocalContactForce[3],
                                               const double cohesive_force,
                                               array_1d<double, 3>& r_elastic_force,
                                               array_1d<double, 3>& r_contact_force,
                                               array_1d<double, 3>& elastic_force_backup,
                                               array_1d<double, 3>& total_force_backup) {
    for (unsigned int index = 0; index < 3; index++) {
        LocalContactForce[index] = LocalElasticContactForce[index] + ViscoDampingLocalContactForce[index];
    }
    LocalContactForce[2] -= cohesive_force;

    GeometryFunctions::VectorLocal2Global(LocalCoordSystem, LocalElasticContactForce, GlobalElasticContactForce);
    GeometryFunctions::VectorLocal2Global(LocalCoordSystem, LocalContactForce, GlobalContactForce);
    // Saving contact forces (We need to, since tangential elastic force is history-dependent)
    DEM_COPY_SECOND_TO_FIRST_3(elastic_force_backup,GlobalElasticContactForce)
    DEM_COPY_SECOND_TO_FIRST_3(total_force_backup,GlobalContactForce)
    DEM_ADD_SECOND_TO_FIRST(r_elastic_force, GlobalElasticContactForce)
    DEM_ADD_SECOND_TO_FIRST(r_contact_force, GlobalContactForce)

}

void SphericParticle::MemberDeclarationFirstStep(const ProcessInfo& r_process_info) {
    // Passing the element id to the node upon initialization
    if (r_process_info[PRINT_EXPORT_ID] == 1) {
        this->GetGeometry()[0].FastGetSolutionStepValue(EXPORT_ID) = double(this->Id());
    }

    if (r_process_info[ROTATION_OPTION]) this->Set(DEMFlags::HAS_ROTATION, true);
    else                                 this->Set(DEMFlags::HAS_ROTATION, false);

    if (r_process_info[ROLLING_FRICTION_OPTION]) this->Set(DEMFlags::HAS_ROLLING_FRICTION, true);
    else                                         this->Set(DEMFlags::HAS_ROLLING_FRICTION, false);

    if (r_process_info[GLOBAL_DAMPING_OPTION]) this->Set(DEMFlags::HAS_GLOBAL_DAMPING, true);
    else                                       this->Set(DEMFlags::HAS_GLOBAL_DAMPING, false);

    if (r_process_info[COMPUTE_STRESS_TENSOR_OPTION]) this->Set(DEMFlags::HAS_STRESS_TENSOR, true);
    else                                              this->Set(DEMFlags::HAS_STRESS_TENSOR, false);

    if (r_process_info[PRINT_STRESS_TENSOR_OPTION]) this->Set(DEMFlags::PRINT_STRESS_TENSOR, true);
    else                                            this->Set(DEMFlags::PRINT_STRESS_TENSOR, false);

    if (this->Is(DEMFlags::HAS_STRESS_TENSOR)) {

        mStressTensor  = new BoundedMatrix<double, 3, 3>(3,3);
        *mStressTensor = ZeroMatrix(3,3);

        mSymmStressTensor  = new BoundedMatrix<double, 3, 3>(3,3);
        *mSymmStressTensor = ZeroMatrix(3,3);

        mStrainTensor  = new BoundedMatrix<double, 3, 3>(3,3);
        *mStrainTensor = ZeroMatrix(3,3);

        mDifferentialStrainTensor  = new BoundedMatrix<double, 3, 3>(3,3);
        *mDifferentialStrainTensor = ZeroMatrix(3,3);
    }
    else {

        mStressTensor             = NULL;
        mSymmStressTensor         = NULL;
        mStrainTensor             = NULL;
        mDifferentialStrainTensor = NULL;
    }
}

std::unique_ptr<DEMDiscontinuumConstitutiveLaw> SphericParticle::pCloneDiscontinuumConstitutiveLawWithNeighbour(SphericParticle* neighbour) {
    Properties& properties_of_this_contact = GetProperties().GetSubProperties(neighbour->GetProperties().Id());
    return properties_of_this_contact[DEM_DISCONTINUUM_CONSTITUTIVE_LAW_POINTER]->CloneUnique();
}

std::unique_ptr<DEMDiscontinuumConstitutiveLaw> SphericParticle::pCloneDiscontinuumConstitutiveLawWithFEMNeighbour(Condition* neighbour) {
    Properties& properties_of_this_contact = GetProperties().GetSubProperties(neighbour->GetProperties().Id());
    return properties_of_this_contact[DEM_DISCONTINUUM_CONSTITUTIVE_LAW_POINTER]->CloneUnique();
}

std::unique_ptr<DEMRollingFrictionModel> SphericParticle::pCloneRollingFrictionModel(SphericParticle* element){
    Properties& properties_of_this_particle = element->GetProperties();
    return properties_of_this_particle[DEM_ROLLING_FRICTION_MODEL_POINTER]->CloneUnique();
}

std::unique_ptr<DEMRollingFrictionModel> SphericParticle::pCloneRollingFrictionModelWithNeighbour(SphericParticle* neighbour){
    Properties& properties_of_this_contact = GetProperties().GetSubProperties(neighbour->GetProperties().Id());
    return properties_of_this_contact[DEM_ROLLING_FRICTION_MODEL_POINTER]->CloneUnique();
}

std::unique_ptr<DEMRollingFrictionModel> SphericParticle::pCloneRollingFrictionModelWithFEMNeighbour(Condition* neighbour){
    Properties& properties_of_this_contact = GetProperties().GetSubProperties(neighbour->GetProperties().Id());
    return properties_of_this_contact[DEM_ROLLING_FRICTION_MODEL_POINTER]->CloneUnique();
}

std::unique_ptr<DEMGlobalDampingModel> SphericParticle::pCloneGlobalDampingModel(SphericParticle* element){
    Properties& properties_of_this_particle = element->GetProperties();
    return properties_of_this_particle[DEM_GLOBAL_DAMPING_MODEL_POINTER]->CloneUnique();
}

void SphericParticle::ComputeOtherBallToBallForces(array_1d<double, 3>& other_ball_to_ball_forces) {}

void SphericParticle::StoreBallToBallContactInfo(const ProcessInfo& r_process_info, SphericParticle::ParticleDataBuffer& data_buffer, double GlobalContactForceTotal[3], double LocalContactForceTotal[3], double LocalContactForceDamping[3], bool sliding) {}

void SphericParticle::StoreBallToRigidFaceContactInfo(const ProcessInfo& r_process_info, SphericParticle::ParticleDataBuffer& data_buffer, double GlobalContactForceTotal[3], double LocalContactForceTotal[3], double LocalContactForceDamping[3], bool sliding) {}

double SphericParticle::GetInitialDeltaWithFEM(int index) {//only available in continuum_particle
    return 0.0;
}

void SphericParticle::Calculate(const Variable<double>& rVariable, double& Output, const ProcessInfo& r_process_info) {
    KRATOS_TRY

    if (rVariable == PARTICLE_TRANSLATIONAL_KINEMATIC_ENERGY) {

      const array_1d<double, 3>& vel = this->GetGeometry()[0].FastGetSolutionStepValue(VELOCITY);
      double square_of_celerity      = vel[0] * vel[0] + vel[1] * vel[1] + vel[2] * vel[2];

      Output = 0.5 * (GetMass() * square_of_celerity);

      return;
    }

    if (rVariable == PARTICLE_ROTATIONAL_KINEMATIC_ENERGY) {

        const array_1d<double, 3> ang_vel = this->GetGeometry()[0].FastGetSolutionStepValue(ANGULAR_VELOCITY);
        const double moment_of_inertia    = this->GetGeometry()[0].FastGetSolutionStepValue(PARTICLE_MOMENT_OF_INERTIA);
        double square_of_angular_celerity = ang_vel[0] * ang_vel[0] + ang_vel[1] * ang_vel[1] + ang_vel[2] * ang_vel[2];
        Output = 0.5 * moment_of_inertia * square_of_angular_celerity;

        return;
    }

    if (rVariable == PARTICLE_GRAVITATIONAL_ENERGY) {

       // Energy relative to the origin [0,0,0]
       const double coord[3] = { this->GetGeometry()[0][0], this->GetGeometry()[0][1], this->GetGeometry()[0][2] };
       Output = - this->GetMass() * DEM_INNER_PRODUCT_3(r_process_info[GRAVITY], coord);

        return;
    }

    if (rVariable == PARTICLE_ELASTIC_ENERGY) {

        Output = GetElasticEnergy();

    }

    if (rVariable == PARTICLE_MAX_NORMAL_BALL_TO_BALL_FORCE_TIMES_RADIUS) {

        Output = GetMaxNormalBallToBallForceTimesRadius();

    }

    if (rVariable == PARTICLE_INELASTIC_FRICTIONAL_ENERGY) {

        Output = GetInelasticFrictionalEnergy();

    }

    if (rVariable == PARTICLE_INELASTIC_VISCODAMPING_ENERGY) {

        Output = GetInelasticViscodampingEnergy();

    }

    if (rVariable == PARTICLE_INELASTIC_ROLLING_RESISTANCE_ENERGY) {

      Output = GetInelasticRollingResistanceEnergy();

    }

    AdditionalCalculate(rVariable, Output, r_process_info);

    KRATOS_CATCH("")

} //Calculate

void SphericParticle::Calculate(const Variable<array_1d<double, 3> >& rVariable, array_1d<double, 3>& Output,
                                const ProcessInfo& r_process_info)
{
    if (rVariable == MOMENTUM) {
        CalculateMomentum(Output);
    }

    else if (rVariable == ANGULAR_MOMENTUM) {
        CalculateLocalAngularMomentum(Output);
    }
}

void SphericParticle::RotateOldContactForces(const double OldLocalCoordSystem[3][3], const double LocalCoordSystem[3][3], array_1d<double, 3>& mNeighbourElasticContactForces) {

    array_1d<double, 3> v1;
    array_1d<double, 3> v2;
    array_1d<double, 3> v3;
    array_1d<double, 3> rotated_neighbour_elastic_contact_forces;

    v1[0] = OldLocalCoordSystem[2][0]; v1[1] = OldLocalCoordSystem[2][1]; v1[2] = OldLocalCoordSystem[2][2];
    v2[0] = LocalCoordSystem[2][0];    v2[1] = LocalCoordSystem[2][1];    v2[2] = LocalCoordSystem[2][2];

    GeometryFunctions::CrossProduct(v1, v2, v3);

    double v1_mod = GeometryFunctions::module(v1);
    double v2_mod = GeometryFunctions::module(v2);
    double v3_mod = GeometryFunctions::module(v3);

    double alpha = asin(v3_mod / (v1_mod * v2_mod));

    GeometryFunctions::normalize(v3);

    GeometryFunctions::RotateAVectorAGivenAngleAroundAUnitaryVector(mNeighbourElasticContactForces, v3, alpha, rotated_neighbour_elastic_contact_forces);

    DEM_COPY_SECOND_TO_FIRST_3(mNeighbourElasticContactForces, rotated_neighbour_elastic_contact_forces)
}

bool SphericParticle::CalculateRelativePositionsOrSkipContact(ParticleDataBuffer & data_buffer)
{
    const bool other_is_injecting_me = this->Is(NEW_ENTITY) && data_buffer.mpOtherParticle->Is(BLOCKED);
    const bool i_am_injecting_other = this->Is(BLOCKED) && data_buffer.mpOtherParticle->Is(NEW_ENTITY);
    const bool multistage_condition = data_buffer.mMultiStageRHS  &&  this->Id() > data_buffer.mpOtherParticle->Id();

    bool must_skip_contact_calculation = other_is_injecting_me || i_am_injecting_other || multistage_condition;

    if (must_skip_contact_calculation){
        return false;
    }

    NodeType& other_node = data_buffer.mpOtherParticle->GetGeometry()[0];
    DEM_COPY_SECOND_TO_FIRST_3(data_buffer.mOtherCoors, other_node)

    if (data_buffer.mDomainIsPeriodic){
        TransformNeighbourCoorsToClosestInPeriodicDomain(data_buffer);
    }

    data_buffer.mOtherToMeVector[0] = data_buffer.mMyCoors[0] - data_buffer.mOtherCoors[0];
    data_buffer.mOtherToMeVector[1] = data_buffer.mMyCoors[1] - data_buffer.mOtherCoors[1];
    data_buffer.mOtherToMeVector[2] = data_buffer.mMyCoors[2] - data_buffer.mOtherCoors[2];

    data_buffer.mDistance = DEM_MODULUS_3(data_buffer.mOtherToMeVector);

    must_skip_contact_calculation = data_buffer.mDistance < std::numeric_limits<double>::epsilon();

    if (must_skip_contact_calculation){
        return false;
    }

    data_buffer.mOtherRadius = data_buffer.mpOtherParticle->GetInteractionRadius();
    data_buffer.mRadiusSum   = this->GetInteractionRadius() + data_buffer.mOtherRadius;
    data_buffer.mIndentation = data_buffer.mRadiusSum - data_buffer.mDistance;

    if (this->mIndentationInitialOption) {

        if (data_buffer.mTime < (0.0 + 2.0 * data_buffer.mDt)){
            if (data_buffer.mIndentation > 0.0) {
                this->mIndentationInitial[data_buffer.mpOtherParticle->Id()] = data_buffer.mIndentation;
            } 
        }

        if (this->mIndentationInitial.find(data_buffer.mpOtherParticle->Id()) != this->mIndentationInitial.end()){
            data_buffer.mIndentation -= this->mIndentationInitial[data_buffer.mpOtherParticle->Id()];
        } 
    }

    return data_buffer.mIndentation > 0.0;
}

void SphericParticle::RelativeDisplacementAndVelocityOfContactPointDueToOtherReasons(const ProcessInfo& r_process_info,
                                                                                    double DeltDisp[3], //IN GLOBAL AXES
                                                                                    double RelVel[3], //IN GLOBAL AXES
                                                                                    double OldLocalCoordSystem[3][3],
                                                                                    double LocalCoordSystem[3][3],
                                                                                    SphericParticle* neighbour_iterator) {}

void SphericParticle::SendForcesToFEM(){}

void SphericParticle::TransformNeighbourCoorsToClosestInPeriodicDomain(ParticleDataBuffer & data_buffer)
{
    const double periods[3] = {data_buffer.mDomainMax[0] - data_buffer.mDomainMin[0],
                               data_buffer.mDomainMax[1] - data_buffer.mDomainMin[1],
                               data_buffer.mDomainMax[2] - data_buffer.mDomainMin[2]};

    DiscreteParticleConfigure<3>::TransformToClosestPeriodicCoordinates(data_buffer.mMyCoors, data_buffer.mOtherCoors, periods);
}

void SphericParticle::TransformNeighbourCoorsToClosestInPeriodicDomain(ParticleDataBuffer & data_buffer,
                                                                       const array_1d<double, 3>& coors,
                                                                       array_1d<double, 3>& neighbour_coors)
{
    const double periods[3] = {data_buffer.mDomainMax[0] - data_buffer.mDomainMin[0],
                               data_buffer.mDomainMax[1] - data_buffer.mDomainMin[1],
                               data_buffer.mDomainMax[2] - data_buffer.mDomainMin[2]};

    DiscreteParticleConfigure<3>::TransformToClosestPeriodicCoordinates(coors, neighbour_coors, periods);
}

void SphericParticle::TransformNeighbourCoorsToClosestInPeriodicDomain(const ProcessInfo& r_process_info,
                                                                       const double coors[3],
                                                                       double neighbour_coors[3])
{
    const array_1d<double,3>& domain_min = r_process_info[DOMAIN_MIN_CORNER];
    const array_1d<double,3>& domain_max = r_process_info[DOMAIN_MAX_CORNER];

    const double periods[3] = {domain_max[0] - domain_min[0],
                               domain_max[1] - domain_min[1],
                               domain_max[2] - domain_min[2]};

    DiscreteParticleConfigure<3>::TransformToClosestPeriodicCoordinates(coors, neighbour_coors, periods);
}


void SphericParticle::Move(const double delta_t, const bool rotation_option, const double force_reduction_factor, const int StepFlag) {
    GetTranslationalIntegrationScheme().Move(GetGeometry()[0], delta_t, force_reduction_factor, StepFlag);
    if (rotation_option) {
        GetRotationalIntegrationScheme().Rotate(GetGeometry()[0], delta_t, force_reduction_factor, StepFlag);
    }
}

bool SphericParticle::SwapIntegrationSchemeToGluedToWall(Condition* p_wall) {
    if(mpTranslationalIntegrationScheme != mpRotationalIntegrationScheme) {
        delete mpTranslationalIntegrationScheme;
    }
    bool is_inside = false;
    mpTranslationalIntegrationScheme = new GluedToWallScheme(p_wall, this, is_inside);
    delete mpRotationalIntegrationScheme;
    mpRotationalIntegrationScheme = mpTranslationalIntegrationScheme;
    return is_inside;
}

void SphericParticle::Calculate(const Variable<Vector >& rVariable, Vector& Output, const ProcessInfo& r_process_info){}
void SphericParticle::Calculate(const Variable<Matrix >& rVariable, Matrix& Output, const ProcessInfo& r_process_info){}
void SphericParticle::AdditionalCalculate(const Variable<double>& rVariable, double& Output, const ProcessInfo& r_process_info){}

void SphericParticle::ApplyGlobalDampingToContactForcesAndMoments(array_1d<double,3>& total_forces, array_1d<double,3>& total_moment) {
    if (this->Is(DEMFlags::HAS_GLOBAL_DAMPING) && mGlobalDampingModel != nullptr) {
        mGlobalDampingModel->AddGlobalDampingForceAndMoment(this, total_forces, total_moment);
    }
}

void SphericParticle::CalculateOnContactElements(size_t i, double LocalContactForce[3], double GlobalContactForce[3]) {
    KRATOS_TRY

    if (!mBondElements.size()) return; // we skip this function if the vector of bonds hasn't been filled yet.
    ParticleContactElement* bond = mBondElements[i];
    if (bond == NULL) return; //This bond was never created (happens in some MPI cases, see CreateContactElements() in explicit_solve_continumm.h)

    bond->mLocalContactForce[0] = LocalContactForce[0];
    bond->mLocalContactForce[1] = LocalContactForce[1];
    bond->mLocalContactForce[2] = LocalContactForce[2];

    bond->mGlobalContactForce[0] = GlobalContactForce[0];
    bond->mGlobalContactForce[1] = GlobalContactForce[1];
    bond->mGlobalContactForce[2] = GlobalContactForce[2];

    bond->GetValue(GLOBAL_CONTACT_FORCE)[0] = GlobalContactForce[0];
    bond->GetValue(GLOBAL_CONTACT_FORCE)[1] = GlobalContactForce[1];
    bond->GetValue(GLOBAL_CONTACT_FORCE)[2] = GlobalContactForce[2];
        
    KRATOS_CATCH("")
}

int    SphericParticle::GetClusterId()                                                    { return mClusterId;      }
void   SphericParticle::SetClusterId(int givenId)                                         { mClusterId = givenId;   }
double SphericParticle::GetInitializationTime() const                                     { return mInitializationTime;}
double SphericParticle::GetProgrammedDestructionTime() const {return mProgrammedDestructionTime;}
void SphericParticle::SetProgrammedDestructionTime(const double destruction_time){mProgrammedDestructionTime = destruction_time;}
double SphericParticle::GetRadius()                                                       { return mRadius;         }
double SphericParticle::CalculateVolume()                                                 { return 4.0 * Globals::Pi / 3.0 * mRadius * mRadius * mRadius;     }
void   SphericParticle::SetRadius(double radius)                                          { mRadius = radius;       }
void   SphericParticle::SetRadius()                                                       { mRadius = GetGeometry()[0].FastGetSolutionStepValue(RADIUS);       }
void   SphericParticle::SetRadius(bool is_radius_expansion, double radius_expansion_rate, double radius_multiplier_max, double radius_multiplier, double radius_multiplier_old)                                                       
{ 
    if (is_radius_expansion){
        if (radius_multiplier_old >= 1.0) {
            mRadius = (GetGeometry()[0].FastGetSolutionStepValue(RADIUS) / radius_multiplier_old) * radius_multiplier;
            GetGeometry()[0].FastGetSolutionStepValue(RADIUS) = mRadius;
            
            if (this->Is(DEMFlags::HAS_ROTATION)) {

                NodeType& node = GetGeometry()[0];
                
                node.GetSolutionStepValue(PARTICLE_MOMENT_OF_INERTIA) = CalculateMomentOfInertia();
                
                array_1d<double, 3> angular_momentum;
                CalculateLocalAngularMomentum(angular_momentum);
                noalias(node.GetSolutionStepValue(ANGULAR_MOMENTUM)) = angular_momentum;
            }

        } else {
            mRadius = GetGeometry()[0].FastGetSolutionStepValue(RADIUS);
        }
    } else {
        mRadius = GetGeometry()[0].FastGetSolutionStepValue(RADIUS);
    }
}
double SphericParticle::GetInteractionRadius(const int radius_index)                      { return mRadius;         }
void   SphericParticle::SetInteractionRadius(const double radius, const int radius_index) { mRadius = radius; GetGeometry()[0].FastGetSolutionStepValue(RADIUS) = radius;}
double SphericParticle::GetSearchRadius()                                                 { return mSearchRadius;   }
void   SphericParticle::SetSearchRadius(const double radius)                              { mSearchRadius = radius; }
void SphericParticle::SetDefaultRadiiHierarchy(const double radius)
{
    SetRadius(radius);
    SetSearchRadius(radius);
}

double SphericParticle::GetMass()                                                         { return mRealMass;       }
void   SphericParticle::SetMass(double real_mass)                                         { mRealMass = real_mass;  GetGeometry()[0].FastGetSolutionStepValue(NODAL_MASS) = real_mass;}
double SphericParticle::CalculateMomentOfInertia()                                        { return 0.4 * GetMass() * GetRadius() * GetRadius(); }
double SphericParticle::GetYoung()                                                        { return GetFastProperties()->GetYoung();                    }
double SphericParticle::GetPoisson()                                                      { return GetFastProperties()->GetPoisson();                  }
double SphericParticle::GetDensity()                                                      { return GetFastProperties()->GetDensity();                  }
int    SphericParticle::GetParticleMaterial()                                             { return GetFastProperties()->GetParticleMaterial();         }

array_1d<double, 3>& SphericParticle::GetForce()                                          { return GetGeometry()[0].FastGetSolutionStepValue(TOTAL_FORCES);}
double&              SphericParticle::GetElasticEnergy()                                  { return mElasticEnergy; }
double&              SphericParticle::GetInelasticFrictionalEnergy()                      { return mInelasticFrictionalEnergy; }
double&              SphericParticle::GetInelasticViscodampingEnergy()                    { return mInelasticViscodampingEnergy; }
double&              SphericParticle::GetInelasticRollingResistanceEnergy()               { return mInelasticRollingResistanceEnergy; }
double&              SphericParticle::GetMaxNormalBallToBallForceTimesRadius()            { return mMaxNormalBallToBallForceTimesRadius; }

void   SphericParticle::SetYoungFromProperties(double* young)                                          { GetFastProperties()->SetYoungFromProperties( young);                                            }
void   SphericParticle::SetPoissonFromProperties(double* poisson)                                      { GetFastProperties()->SetPoissonFromProperties( poisson);                                        }
void   SphericParticle::SetDensityFromProperties(double* density)                                      { GetFastProperties()->SetDensityFromProperties( density);                                        }
void   SphericParticle::SetParticleMaterialFromProperties(int* particle_material)                      { GetFastProperties()->SetParticleMaterialFromProperties( particle_material);                     }

PropertiesProxy* SphericParticle::GetFastProperties()                                     { return mFastProperties;   }
void   SphericParticle::SetFastProperties(PropertiesProxy* pProps)                        { mFastProperties = pProps; }
void   SphericParticle::SetFastProperties(std::vector<PropertiesProxy>& list_of_proxies)  {
    for (unsigned int j = 0; j < list_of_proxies.size(); j++){
        if (list_of_proxies[j].GetId() == GetProperties().Id()) {
            SetFastProperties(&list_of_proxies[j]);
            return;
        }
    }
}

double SphericParticle::SlowGetYoung() const                    { return GetProperties()[YOUNG_MODULUS];               }
double SphericParticle::SlowGetPoisson() const                  { return GetProperties()[POISSON_RATIO];               }
double SphericParticle::SlowGetDensity() const                  { return GetProperties()[PARTICLE_DENSITY];            }
int    SphericParticle::SlowGetParticleMaterial() const         { return GetProperties()[PARTICLE_MATERIAL];           }

}  // namespace Kratos.
