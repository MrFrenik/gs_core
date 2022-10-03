/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: sm_physics.h

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

#ifndef GS_CORE_PHYSICS_H
#define GS_CORE_PHYSICS_H 

#include <gs/gs.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    GS_CORE_COLLISION_SHAPE_EMPTY = 0x00,
    GS_CORE_COLLISION_SHAPE_BOX, 
    GS_CORE_COLLISION_SHAPE_SPHERE, 
    GS_CORE_COLLISION_SHAPE_CYLINDER, 
    GS_CORE_COLLISION_SHAPE_CONE, 
    GS_CORE_COLLISION_SHAPE_CAPSULE, 
    GS_CORE_COLLISION_SHAPE_POLY
} gs_core_physics_shape_type_t;

enum
{
    GS_CORE_PHYSICS_BODY_FLAG_IS_TRIGGER = (1 << 0),
    GS_CORE_PHYSICS_BODY_FLAG_IS_KINEMATIC = (1 << 1),
    GS_CORE_PHYSICS_BODY_FLAG_CCD_ENABLED = (1 << 2)
};

typedef uint32_t gs_core_physics_body_handle_t;
typedef uint32_t gs_core_physics_collision_shape_handle_t;

typedef struct
{
    float mass;
    float restitution;
    float friction;
    float linear_damping;
    float angular_damping;
    gs_vec3 gravity;
    uint16_t flags;
    gs_vec3 linear_factor;
    gs_vec3 angular_factor;
    gs_core_physics_collision_shape_handle_t shape;
} gs_core_physics_body_desc_t;

typedef struct
{
    gs_vec3 local_scaling;
    gs_vec3 offset;
    gs_core_physics_shape_type_t type;
    union
    {
        struct {gs_vec3 half_extents;}       box;
        struct {float radius;}               sphere;
        struct {gs_vec3 half_extents;}       cylinder;
        struct {float height; float radius;} cone;
        struct {float height; float radius;} capsule;
    };
} gs_core_physics_collision_shape_desc_t;

typedef struct
{
    gs_vec3 start;                   // Starting point of ray
    gs_vec3 end;                     // Ending point of ray
    gs_vec3 impact_point;            // World position of impact
    gs_vec3 impact_normal;           // World normal of impact
    gs_core_physics_body_handle_t hndl; // Hit physics body handle
    uint16_t hit;                    // Whether or not result was hit
} gs_core_physics_ray_cast_result_t;

typedef struct gs_core_physics_s
{
    void* user_data;
    bool32 paused;
} gs_core_physics_t;

// Core_Physics
GS_API_DECL gs_core_physics_t* 
gs_core_physics_new();

GS_API_DECL void 
gs_core_physics_update();

GS_API_DECL void 
gs_core_physics_shutdown();

GS_API_DECL gs_core_physics_t* 
gs_core_physics_instance();

GS_API_DECL void 
gs_core_physics_set_gravity(const gs_vec3* v);

GS_API_DECL gs_core_physics_collision_shape_handle_t 
gs_core_physics_add_shape(const gs_core_physics_collision_shape_desc_t* desc);

GS_API_DECL gs_core_physics_body_handle_t 
gs_core_physics_add_body(const gs_core_physics_body_desc_t* desc);

GS_API_DECL void 
gs_core_physics_remove_body(gs_core_physics_body_handle_t hndl);

GS_API_DECL void 
gs_core_physics_pause_system(bool enabled);

GS_API_DECL bool 
gs_core_physics_is_paused();

GS_API_DECL void 
gs_core_physics_clear_all_forces();

GS_API_DECL void 
gs_core_physics_cast_ray(gs_core_physics_ray_cast_result_t* ray);

//======[ Physics Collision Shape ]======//

GS_API_DECL void 
gs_core_physics_collision_shape_set_body(gs_core_physics_collision_shape_handle_t chndl, 
    gs_core_physics_body_handle_t bhndl);

GS_API_DECL gs_vec3 
gs_core_physics_collision_shape_calculate_local_intertia(gs_core_physics_collision_shape_handle_t hndl, float mass);

GS_API_DECL void
gs_core_physics_collision_shape_set_local_scaling(gs_core_physics_collision_shape_handle_t hndl, const gs_vec3* scale);

GS_API_DECL gs_vec3 
gs_core_physics_collision_shape_get_local_scaling(gs_core_physics_collision_shape_handle_t hndl);

GS_API_DECL gs_core_physics_shape_type_t 
gs_core_physics_collision_shape_get_shape_type(gs_core_physics_collision_shape_handle_t hndl);

GS_API_DECL void 
gs_core_physics_collision_shape_set_offset(gs_core_physics_collision_shape_handle_t hndl, gs_vec3 offset);

GS_API_DECL gs_vec3 
gs_core_physics_collision_shape_get_offset(gs_core_physics_collision_shape_handle_t hndl);

//=====[ Physics Body ]=====// 

GS_API_DECL void 
gs_core_physics_body_refresh_transform(gs_core_physics_body_handle_t hndl);

GS_API_DECL void 
gs_core_physics_body_set_awake(gs_core_physics_body_handle_t hndl, bool enabled);

GS_API_DECL void 
gs_core_physics_body_set_shape(gs_core_physics_body_handle_t hndl, gs_core_physics_shape_type_t type);

GS_API_DECL void 
gs_core_physics_body_set_mass(gs_core_physics_body_handle_t hndl, float value);

GS_API_DECL float 
gs_core_physics_body_get_mass(gs_core_physics_body_handle_t hndl);

GS_API_DECL void 
gs_core_physics_body_set_is_trigger(gs_core_physics_body_handle_t hndl, bool enabled);

GS_API_DECL bool 
gs_core_physics_body_get_is_trigger(gs_core_physics_body_handle_t hndl);

GS_API_DECL void 
gs_core_physics_body_set_restitution(gs_core_physics_body_handle_t hndl, float value);

GS_API_DECL float 
gs_core_physics_body_get_restitution(gs_core_physics_body_handle_t hndl);

GS_API_DECL void 
gs_core_physics_body_set_linear_damping(gs_core_physics_body_handle_t hndl, float value);

GS_API_DECL float 
gs_core_physics_body_get_linear_damping(gs_core_physics_body_handle_t hndl);

GS_API_DECL void 
gs_core_physics_body_set_angular_damping(gs_core_physics_body_handle_t hndl, float value);

GS_API_DECL float 
gs_core_physics_body_get_angular_damping(gs_core_physics_body_handle_t hndl);

GS_API_DECL void 
gs_core_physics_body_set_friction(gs_core_physics_body_handle_t hndl, float value);

GS_API_DECL float 
gs_core_physics_body_get_friction(gs_core_physics_body_handle_t hndl);

GS_API_DECL void 
gs_core_physics_body_set_gravity(gs_core_physics_body_handle_t hndl, const gs_vec3* value);

GS_API_DECL gs_vec3 
gs_core_physics_body_get_gravity(gs_core_physics_body_handle_t hndl);

GS_API_DECL void 
gs_core_physics_body_set_linear_velocity(gs_core_physics_body_handle_t hndl, const gs_vec3* value);

GS_API_DECL void 
gs_core_physics_body_set_angular_velocity(gs_core_physics_body_handle_t hndl, const gs_vec3* value);

GS_API_DECL void 
gs_core_physics_body_set_linear_factor(gs_core_physics_body_handle_t hndl, const gs_vec3* value);

GS_API_DECL gs_vec3 
gs_core_physics_body_get_linear_factor(gs_core_physics_body_handle_t hndl);

GS_API_DECL void 
gs_core_physics_body_set_angular_factor(gs_core_physics_body_handle_t hndl, const gs_vec3* value);

GS_API_DECL gs_vec3 
gs_core_physics_body_get_angular_factor(gs_core_physics_body_handle_t hndl);

GS_API_DECL void 
gs_core_physics_body_set_is_kinematic(gs_core_physics_body_handle_t hndl, bool enable);

GS_API_DECL bool 
gs_core_physics_body_get_is_kinematic(gs_core_physics_body_handle_t hndl);

GS_API_DECL void 
gs_core_physics_body_translate(gs_core_physics_body_handle_t hndl, const gs_vec3* value);

GS_API_DECL void 
gs_core_physics_body_clear_forces(gs_core_physics_body_handle_t hndl);

GS_API_DECL void 
gs_core_physics_body_force_awake(gs_core_physics_body_handle_t hndl);

GS_API_DECL void 
gs_core_physics_body_apply_central_force(gs_core_physics_body_handle_t hndl, const gs_vec3* value);

GS_API_DECL void 
gs_core_physics_body_apply_relative_force(gs_core_physics_body_handle_t hndl, 
    const gs_vec3* value, const gs_vec3* relative_position);

GS_API_DECL void 
gs_core_physics_body_apply_central_impulse(gs_core_physics_body_handle_t hndl, const gs_vec3* value);

GS_API_DECL void 
gs_core_physics_body_apply_impulse(gs_core_physics_body_handle_t hndl, 
    const gs_vec3* value, const gs_vec3* relative_impulse);

GS_API_DECL void 
gs_core_physics_body_set_world_transform(gs_core_physics_body_handle_t hndl, const gs_vqs* transform);

GS_API_DECL gs_vqs 
gs_core_physics_body_get_world_transform(gs_core_physics_body_handle_t hndl); 

GS_API_DECL void 
gs_core_physics_body_refresh_transform(gs_core_physics_body_handle_t hndl); 

GS_API_DECL void 
gs_core_physics_body_set_local_scaling(gs_core_physics_body_handle_t hndl, const gs_vec3* scale); 

GS_API_DECL void 
gs_core_physics_body_set_ccd_enabled(gs_core_physics_body_handle_t hndl, bool enabled); 

GS_API_DECL bool 
gs_core_physics_body_get_ccd_enabled(gs_core_physics_body_handle_t hndl); 

GS_API_DECL void 
gs_core_physics_body_set_user_data(gs_core_physics_body_handle_t hndl, void* data); 

GS_API_DECL gs_core_physics_shape_type_t 
gs_core_physics_body_get_shape_type(gs_core_physics_body_handle_t hndl);

#ifdef __cplusplus
}
#endif 

#endif // GS_CORE_PHYSICS_H



















