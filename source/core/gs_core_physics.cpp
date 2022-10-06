
/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: gs_core_physics.cpp

    All Rights Reserved

    BSD 3-Clause License

    Copyright (c) 2022 John Jackson

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its contributors may be used to 
    endorse or promote products derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIEDi
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=================================================================================================================*/ 

// Core includes
#include "gs_core_physics.h"

// Bullet includes
#include "bullet/btBulletCollisionCommon.h"
#include "bullet/btBulletDynamicsCommon.h"

static gs_core_physics_t* g_physics = nullptr;

#define GS_CORE_BULLET()     ((gs_core_bullet_t*)g_physics->user_data)
#define GS_CORE_BODY(HNDL)   (gs_slot_array_getp(GS_CORE_BULLET()->bodies, (HNDL)))
#define GS_CORE_SHAPE(HNDL)  (gs_slot_array_getp(GS_CORE_BULLET()->shapes, (HNDL)))

// Utils
static btVector3 
gv2bv(const gs_vec3* v)
{
    return btVector3(v->x, v->y, v->z);
}

static gs_vec3 
bv_to_gv(const btVector3* v) 
{
    return gs_v3(v->getX(), v->getY(), v->getZ());
}

static gs_quat 
bq2gq(const btQuaternion* v)
{ 
    gs_quat q = {}; 
    q.x = v->getX();
    q.y = v->getY();
    q.z = v->getZ();
    q.w = v->getW();
    return q;
}

static btQuaternion 
gq2bq(const gs_quat* v)
{
    btQuaternion q = {};
    q.setX(v->x);
    q.setY(v->y);
    q.setZ(v->z);
    q.setW(v->w);
    return q;
}

static gs_vqs 
bxform2gxform(const btTransform* v)
{
    gs_vqs xform = {};
    xform.translation = bv_to_gv(&v->getOrigin());
    xform.scale = gs_v3s(1.f);
    xform.rotation = bq2gq(&v->getRotation());
    return xform;
} 

static btTransform 
gxform2bxform(const gs_vqs* v)
{
    btTransform xform = {};
    xform.setIdentity();
    xform.setOrigin(gv2bv(&v->translation));
    xform.setRotation(gq2bq(&v->rotation));
    return xform;
} 

struct gs_core_collision_shape_t
{
    gs_core_physics_collision_shape_desc_t desc;
    btCollisionShape* shape = nullptr;
}; 

struct gs_core_rigid_body_t
{
    gs_core_physics_body_desc_t desc;
    btRigidBody* body = nullptr;
    btMotionState* motion_state = nullptr;
}; 

struct gs_core_bullet_t 
{
    btDiscreteDynamicsWorld* world = nullptr; 
    btDefaultCollisionConfiguration* collision_config = nullptr;
    btCollisionDispatcher* dispatcher = nullptr;
    btBroadphaseInterface* broadphase = nullptr;
    btSequentialImpulseConstraintSolver* solver = nullptr; 
    gs_slot_array(gs_core_rigid_body_t) bodies = nullptr;
    gs_slot_array(gs_core_collision_shape_t) shapes = nullptr;
}; 

// Decls.
GS_API_DECL void gs_core_bullet_check_collisions(); 

//==== [ Implementation ] ====/

GS_API_DECL gs_core_physics_t* 
gs_core_physics_new()
{
    if (g_physics) return g_physics;

    // Construct physics system, bullet data, set instance
    gs_core_physics_t* physics = gs_malloc_init(gs_core_physics_t); 
    g_physics = physics;
    physics->user_data = (gs_core_bullet_t*)gs_malloc_init(gs_core_bullet_t);

    // Grab bullet data
    gs_core_bullet_t* bullet = GS_CORE_BULLET();

    // Set up default config, single thread
    bullet->collision_config = new btDefaultCollisionConfiguration();

    // Default collision dispatch, single thread
    bullet->dispatcher = new btCollisionDispatcher(bullet->collision_config);

    // General purpose broad phase
    bullet->broadphase = new btDbvtBroadphase();

    // Set up solver
    bullet->solver = new btSequentialImpulseConstraintSolver();

    // Set up physics world
    bullet->world = new btDiscreteDynamicsWorld(bullet->dispatcher, bullet->broadphase, 
            bullet->solver, bullet->collision_config); 

    // Set up gravity for world
    bullet->world->setGravity(btVector3(0.f, -10.f, 0.f));

    return physics;
}

GS_API_DECL gs_core_physics_t* 
gs_core_physics_instance()
{
    return g_physics;
}

GS_API_DECL void 
gs_core_physics_update()
{
    if (gs_core_physics_instance()->paused) return;

    gs_core_bullet_t* bullet = GS_CORE_BULLET(); 
    const gs_platform_time_t* time = gs_platform_time();

    // Simulate physics world
    // NOTE(john): Not sure what to set this as just yet, so for now we'll cap to 60HZ, 10 iterations
    bullet->world->stepSimulation(1.f / 60.f, 10.f);

    // Check collisions, process callbacks
    gs_core_bullet_check_collisions();
}

GS_API_DECL void 
gs_core_bullet_check_collisions()
{
    gs_core_bullet_t* bullet = GS_CORE_BULLET(); 

    // Browse all collision pairs
    int32_t num_manifolds = bullet->world->getDispatcher()->getNumManifolds();
    for (int32_t i = 0; i < num_manifolds; ++i)
    {
        btPersistentManifold* contact_manifold = bullet->world->getDispatcher()->getManifoldByIndexInternal(i);

        // Get user pointers as comps...

        // Do other stuff here with callbacks...
    }
}

GS_API_DECL void 
gs_core_physics_shutdown()
{
    gs_core_bullet_t* bullet = GS_CORE_BULLET(); 

    // Set up default config, single thread
    delete bullet->collision_config; 
    delete bullet->dispatcher; 
    delete bullet->broadphase; 
    delete bullet->solver; 
    delete bullet->world;
} 

GS_API_DECL void 
gs_core_physics_set_gravity(const gs_vec3* v)
{
    gs_core_bullet_t* bullet = GS_CORE_BULLET();
    bullet->world->setGravity(gv2bv(v));
}

GS_API_DECL gs_core_physics_collision_shape_handle_t 
gs_core_physics_add_shape(const gs_core_physics_collision_shape_desc_t* desc)
{
    gs_core_bullet_t* bullet = GS_CORE_BULLET(); 

    // Construct new shape based on type
    gs_core_collision_shape_t shape = {};
    shape.desc = *desc;

    switch (desc->type)
    {
        default:
        {
            shape.desc.type = GS_CORE_COLLISION_SHAPE_EMPTY; 
            shape.shape = new btEmptyShape();
        } break;

        case GS_CORE_COLLISION_SHAPE_BOX:
        {
            shape.shape = new btBoxShape(gv2bv(&desc->box.half_extents));
        } break;

        case GS_CORE_COLLISION_SHAPE_SPHERE:
        {
            shape.shape = new btSphereShape(desc->sphere.radius);
        } break;

        case GS_CORE_COLLISION_SHAPE_CYLINDER:
        {
            shape.shape = new btCylinderShape(gv2bv(&desc->cylinder.half_extents));
        } break;

        case GS_CORE_COLLISION_SHAPE_CONE:
        {
            shape.shape = new btConeShape(desc->cone.radius, desc->cone.height);
        } break;

        case GS_CORE_COLLISION_SHAPE_CAPSULE:
        {
            shape.shape = new btCapsuleShape(desc->capsule.radius, desc->capsule.height);
        } break;
    }

    // Add shape into slot array
    gs_core_physics_collision_shape_handle_t hndl = gs_slot_array_insert(bullet->shapes, shape);

    return hndl;
}

GS_API_DECL gs_core_physics_body_handle_t 
gs_core_physics_add_body(const gs_core_physics_body_desc_t* desc) 
{
    gs_core_bullet_t* bullet = GS_CORE_BULLET(); 

    // Construct body to fill out
    gs_core_rigid_body_t rb = {};
    rb.desc = *desc;

    // Get collision shape object
    gs_core_collision_shape_t* cs = gs_slot_array_getp(bullet->shapes, desc->shape);

    // Get local inertia using shape
    btVector3 local_inertia = gv2bv(&gs_core_physics_collision_shape_calculate_local_intertia(desc->shape, desc->mass));

    // Transform
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(0.f, 0.f, 0.f));

    // Construct motion state and body 
    rb.motion_state = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo rb_info(desc->mass, rb.motion_state, cs->shape, local_inertia);
    rb.body = new btRigidBody(rb_info);
    rb.body->setRestitution(desc->restitution);
    rb.body->setFriction(desc->friction);
    rb.body->setDamping(desc->linear_damping, desc->angular_damping);
    rb.body->setGravity(gv2bv(&desc->gravity));
    rb.body->setLinearFactor(gv2bv(&desc->linear_factor));
    rb.body->setAngularFactor(gv2bv(&desc->angular_factor));

    // Set mass properties of bullet rigid body
    rb.body->setMassProps(desc->mass, local_inertia);

    // Insert into body slot array
    gs_core_physics_body_handle_t hndl = gs_slot_array_insert(bullet->bodies, rb);

    // Set flags
    gs_core_physics_body_set_is_trigger(hndl, desc->flags & GS_CORE_PHYSICS_BODY_FLAG_IS_TRIGGER);
    gs_core_physics_body_set_is_kinematic(hndl, desc->flags & GS_CORE_PHYSICS_BODY_FLAG_IS_KINEMATIC);
    gs_core_physics_body_set_ccd_enabled(hndl, desc->flags & GS_CORE_PHYSICS_BODY_FLAG_CCD_ENABLED);

    rb.body->activate(true);

    // Add body to world
    bullet->world->addRigidBody(rb.body);

    return hndl;
}

GS_API_DECL void 
gs_core_physics_remove_body(gs_core_physics_body_handle_t hndl)
{
    gs_core_bullet_t* bullet = GS_CORE_BULLET();
}

GS_API_DECL void 
gs_core_physics_pause_system(bool enabled)
{
    gs_core_physics_instance()->paused = enabled;
}

GS_API_DECL bool 
gs_core_physics_is_paused()
{
    return gs_core_physics_instance()->paused;
}

GS_API_DECL void 
gs_core_physics_clear_all_forces()
{
}

GS_API_DECL void 
gs_core_physics_cast_ray(gs_core_physics_ray_cast_result_t* ray)
{
    gs_core_bullet_t* bullet = GS_CORE_BULLET();

    btVector3 start = gv2bv(&ray->start);
    btVector3 end = gv2bv(&ray->end);
    btCollisionWorld::ClosestRayResultCallback cb(start, end); 
    bullet->world->rayTest(start, end, cb);

    if (cb.hasHit())
    {
        void* user_data = cb.m_collisionObject->getUserPointer();
        ray->hit = (uint16_t)true;
        ray->hndl = user_data ? *(uint32_t*)user_data : 0;
        ray->impact_point = bv_to_gv(&cb.m_hitPointWorld);
        ray->impact_normal = bv_to_gv(&cb.m_hitNormalWorld);
    } 
} 

//========= [ Core_PhysicsBody ] ========//

GS_API_DECL void 
gs_core_physics_body_set_world_transform(gs_core_physics_body_handle_t hndl, const gs_vqs* transform)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;
    gs_core_collision_shape_t* cs = GS_CORE_SHAPE(rb->desc.shape);

    btTransform xform;
    xform.setIdentity();

    gs_vec3 elp = transform->translation;
    gs_vec3 offset = cs->desc.offset;

    xform.setOrigin(btVector3(elp.x + offset.x, elp.y + offset.y, elp.z + offset.z));
    xform.setRotation(gq2bq(&transform->rotation));

    // Set local scaling of shape
    gs_core_physics_collision_shape_set_local_scaling(rb->desc.shape, &transform->scale); 

    body->setWorldTransform(xform);
    body->getMotionState()->setWorldTransform(xform);
}

GS_API_DECL gs_vqs 
gs_core_physics_body_get_world_transform(gs_core_physics_body_handle_t hndl)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;
    gs_core_collision_shape_t* cs = GS_CORE_SHAPE(rb->desc.shape);

    gs_vqs xform = gs_vqs_default();
    if (body->getMotionState())
    {
        btTransform btxform; 
        body->getMotionState()->getWorldTransform(btxform);
        
        xform.translation = gs_vec3_sub(bv_to_gv(&btxform.getOrigin()), cs->desc.offset);
        xform.rotation = bq2gq(&btxform.getRotation());
        xform.scale = gs_core_physics_collision_shape_get_local_scaling(rb->desc.shape);
    }

    return xform;
}

GS_API_DECL void 
gs_core_physics_body_refresh_transform(gs_core_physics_body_handle_t hndl)
{
    gs_core_physics_body_set_world_transform(hndl, &gs_core_physics_body_get_world_transform(hndl));
} 

GS_API_DECL void 
gs_core_physics_body_set_awake(gs_core_physics_body_handle_t hndl, bool enabled)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;
    body->activate(enabled);
}

GS_API_DECL void gs_core_physics_body_set_shape(gs_core_physics_body_handle_t hndl, gs_core_physics_shape_type_t type)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;
}

GS_API_DECL void 
gs_core_physics_body_set_mass(gs_core_physics_body_handle_t hndl, float value)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;
    rb->desc.mass = value;
    btVector3 local_inertia = gv2bv(&gs_core_physics_collision_shape_calculate_local_intertia(rb->desc.shape, rb->desc.mass));
    body->setMassProps(rb->desc.mass, local_inertia);
}

GS_API_DECL float 
gs_core_physics_body_get_mass(gs_core_physics_body_handle_t hndl)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;
    return rb->desc.mass;
}

GS_API_DECL void 
gs_core_physics_body_set_restitution(gs_core_physics_body_handle_t hndl, float value)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;
    rb->desc.restitution = value;
    body->setRestitution(value); 
}

GS_API_DECL float 
gs_core_physics_body_get_restitution(gs_core_physics_body_handle_t hndl)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;
    return rb->desc.restitution;
}

GS_API_DECL void 
gs_core_physics_body_set_linear_damping(gs_core_physics_body_handle_t hndl, float value)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;
    rb->desc.linear_damping = value;
    body->setDamping(rb->desc.linear_damping, rb->desc.angular_damping); 
}

GS_API_DECL float 
gs_core_physics_body_get_linear_damping(gs_core_physics_body_handle_t hndl)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;
    return rb->desc.linear_damping; 
}

GS_API_DECL void 
gs_core_physics_body_set_angular_damping(gs_core_physics_body_handle_t hndl, float value)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;
    rb->desc.angular_damping = value;
    body->setDamping(rb->desc.linear_damping, rb->desc.angular_damping); 
}

GS_API_DECL float 
gs_core_physics_body_get_angular_damping(gs_core_physics_body_handle_t hndl)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;
    return rb->desc.angular_damping;
}

GS_API_DECL void 
gs_core_physics_body_set_friction(gs_core_physics_body_handle_t hndl, float value)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;
    rb->desc.friction = value;
    body->setFriction(rb->desc.friction);
}

GS_API_DECL float 
gs_core_physics_body_get_friction(gs_core_physics_body_handle_t hndl)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;
    return rb->desc.friction;
}

GS_API_DECL void 
gs_core_physics_body_set_gravity(gs_core_physics_body_handle_t hndl, const gs_vec3* value)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;
    rb->desc.gravity = *value;
    body->setGravity(gv2bv(&rb->desc.gravity));
}

GS_API_DECL gs_vec3 
gs_core_physics_body_get_gravity(gs_core_physics_body_handle_t hndl)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;
    return rb->desc.gravity;
}

GS_API_DECL void 
gs_core_physics_body_set_linear_velocity(gs_core_physics_body_handle_t hndl, const gs_vec3* value)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;
    body->setLinearVelocity(gv2bv(value));
}

GS_API_DECL void 
gs_core_physics_body_set_angular_velocity(gs_core_physics_body_handle_t hndl, const gs_vec3* value)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;
    body->setAngularVelocity(gv2bv(value));
}

GS_API_DECL void 
gs_core_physics_body_set_linear_factor(gs_core_physics_body_handle_t hndl, const gs_vec3* value)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;
    rb->desc.linear_factor = *value;
    body->setLinearFactor(gv2bv(&rb->desc.linear_factor));
}

GS_API_DECL gs_vec3 
gs_core_physics_body_get_linear_factor(gs_core_physics_body_handle_t hndl)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;
    return rb->desc.linear_factor;
}

GS_API_DECL void 
gs_core_physics_body_set_angular_factor(gs_core_physics_body_handle_t hndl, const gs_vec3* value)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;
    rb->desc.angular_factor = *value;
    body->setAngularFactor(gv2bv(&rb->desc.angular_factor));
}

GS_API_DECL gs_vec3 
gs_core_physics_body_get_angular_factor(gs_core_physics_body_handle_t hndl)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;
    return rb->desc.angular_factor;
} 

GS_API_DECL void 
gs_core_physics_body_set_is_kinematic(gs_core_physics_body_handle_t hndl, bool enabled)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;
    
    if (enabled)
    {
        body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
        body->setActivationState(DISABLE_DEACTIVATION);
    } 
    else
    {
        if (body->getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT)
        {
            body->setCollisionFlags(body->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
            body->activate(true);
        } 
    }
}

GS_API_DECL bool 
gs_core_physics_body_get_is_kinematic(gs_core_physics_body_handle_t hndl)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    return (rb->desc.flags & GS_CORE_PHYSICS_BODY_FLAG_IS_KINEMATIC);
} 

GS_API_DECL void 
gs_core_physics_body_set_ccd_enabled(gs_core_physics_body_handle_t hndl, bool enabled)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;

    body->setCcdMotionThreshold((float)enabled);
    if (enabled)
    {
        body->setCcdSweptSphereRadius(0.2f);        
    } 
}

GS_API_DECL bool 
gs_core_physics_body_get_ccd_enabled(gs_core_physics_body_handle_t hndl)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    return (rb->desc.flags & GS_CORE_PHYSICS_BODY_FLAG_CCD_ENABLED);
} 

GS_API_DECL void 
gs_core_physics_body_set_is_trigger(gs_core_physics_body_handle_t hndl, bool enabled)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;

    if (enabled)
    {
        body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    }
    else
    {
        if (body->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE)
        {
            body->setCollisionFlags(body->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }
    }
}

GS_API_DECL bool 
gs_core_physics_body_get_is_trigger(gs_core_physics_body_handle_t hndl)
{ 
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    return (rb->desc.flags & GS_CORE_PHYSICS_BODY_FLAG_IS_TRIGGER);
}

GS_API_DECL void 
gs_core_physics_body_translate(gs_core_physics_body_handle_t hndl, const gs_vec3* value)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;
    btTransform wt = body->getCenterOfMassTransform();

    // Apply translation
    wt.getOrigin() += gv2bv(value);
    body->setCenterOfMassTransform(wt);
    if (body->getMotionState())
    {
        body->getMotionState()->setWorldTransform(wt);
    }
}

GS_API_DECL void 
gs_core_physics_body_clear_forces(gs_core_physics_body_handle_t hndl)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    gs_core_collision_shape_t* cs = GS_CORE_SHAPE(rb->desc.shape);
    btRigidBody* body = rb->body;
    body->clearForces();
    btTransform initial_transform;
    btVector3 local_inertia = gv2bv(&gs_core_physics_collision_shape_calculate_local_intertia(rb->desc.shape, rb->desc.mass));
    body->setMassProps(rb->desc.mass, local_inertia);
}

GS_API_DECL void 
gs_core_physics_body_force_awake(gs_core_physics_body_handle_t hndl)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;
    body->activate(true);
}

GS_API_DECL void 
gs_core_physics_body_apply_central_force(gs_core_physics_body_handle_t hndl, const gs_vec3* value)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;

    if (!body->isActive())
    {
        gs_core_physics_body_force_awake(hndl);
    }

    body->applyCentralForce(gv2bv(value));
}

GS_API_DECL void 
gs_core_physics_body_apply_relative_force(gs_core_physics_body_handle_t hndl, 
    const gs_vec3* value, const gs_vec3* relative_position)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;

    if (!body->isActive())
    {
        gs_core_physics_body_force_awake(hndl);
    }

    body->applyForce(gv2bv(value), gv2bv(relative_position));
}

GS_API_DECL void 
gs_core_physics_body_apply_central_impulse(gs_core_physics_body_handle_t hndl, const gs_vec3* value)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;

    if (!body->isActive())
    {
        gs_core_physics_body_force_awake(hndl);
    }

    body->applyCentralImpulse(gv2bv(value));
}

GS_API_DECL void 
gs_core_physics_body_apply_impulse(gs_core_physics_body_handle_t hndl, const gs_vec3* value, const gs_vec3* relative_impulse)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    btRigidBody* body = rb->body;

    if (!body->isActive())
    {
        gs_core_physics_body_force_awake(hndl);
    }

    body->applyImpulse(gv2bv(value), gv2bv(relative_impulse));
}

GS_API_DECL void 
gs_core_physics_body_set_local_scaling(gs_core_physics_body_handle_t hndl, const gs_vec3* scale)
{
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl);
    gs_core_physics_collision_shape_set_local_scaling(rb->desc.shape, scale);
}

GS_API_DECL void 
gs_core_physics_body_set_user_data(gs_core_physics_body_handle_t hndl, void* data)
{ 
    gs_core_rigid_body_t* rb = GS_CORE_BODY(hndl); 
    btRigidBody* body = rb->body;
    body->setUserPointer(data);
}

GS_API_DECL gs_core_physics_shape_type_t
gs_core_physics_body_get_shape_type(gs_core_physics_body_handle_t hndl)
{
    gs_core_collision_shape_t* cs = GS_CORE_SHAPE(hndl);
    return cs->desc.type;
}

//========= [ Core_PhysicsCollisionShape ] ==========//

GS_API_DECL gs_vec3 
gs_core_physics_collision_shape_get_local_scaling(gs_core_physics_collision_shape_handle_t hndl)
{ 
    gs_core_bullet_t* bullet = GS_CORE_BULLET(); 
    gs_core_collision_shape_t* cs = gs_slot_array_getp(bullet->shapes, hndl);
    return cs->desc.local_scaling;
}

GS_API_DECL gs_vec3 
gs_core_physics_collision_shape_calculate_local_intertia(gs_core_physics_collision_shape_handle_t hndl, float mass)
{
    gs_core_bullet_t* bullet = GS_CORE_BULLET(); 
    gs_core_collision_shape_t* cs = gs_slot_array_getp(bullet->shapes, hndl);
    btVector3 local_inertia(0.f, 0.f, 0.f);
    if (mass != 0.f && cs->desc.type != GS_CORE_COLLISION_SHAPE_EMPTY)
    {
        cs->shape->calculateLocalInertia(mass, local_inertia);
    }
    return bv_to_gv(&local_inertia);
}

GS_API_DECL void 
gs_core_physics_collision_shape_set_local_scaling(gs_core_physics_collision_shape_handle_t hndl, const gs_vec3* scale)
{ 
    gs_core_bullet_t* bullet = GS_CORE_BULLET(); 
    gs_core_collision_shape_t* cs = gs_slot_array_getp(bullet->shapes, hndl);
    cs->desc.local_scaling = *scale;
    cs->shape->setLocalScaling(gv2bv(scale));
} 

GS_API_DECL gs_core_physics_shape_type_t 
gs_core_physics_collision_shape_get_shape_type(gs_core_physics_collision_shape_handle_t hndl)
{
    gs_core_bullet_t* bullet = GS_CORE_BULLET(); 
    gs_core_collision_shape_t* cs = gs_slot_array_getp(bullet->shapes, hndl);
    return cs->desc.type; 
}

GS_API_DECL void 
gs_core_physics_collision_shape_set_offset(gs_core_physics_collision_shape_handle_t hndl, gs_vec3 offset)
{
    gs_core_bullet_t* bullet = GS_CORE_BULLET(); 
    gs_core_collision_shape_t* cs = gs_slot_array_getp(bullet->shapes, hndl);
    cs->desc.offset = offset;
}

GS_API_DECL gs_vec3 
gs_core_physics_collision_shape_get_offset(gs_core_physics_collision_shape_handle_t hndl)
{
    gs_core_bullet_t* bullet = GS_CORE_BULLET(); 
    gs_core_collision_shape_t* cs = gs_slot_array_getp(bullet->shapes, hndl);
    return cs->desc.offset;
}



















