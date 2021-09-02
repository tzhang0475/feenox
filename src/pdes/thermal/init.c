/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for the heat equation: initialization
 *
 *  Copyright (C) 2021 jeremy theler
 *
 *  This file is part of FeenoX <https://www.seamplex.com/feenox>.
 *
 *  feenox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  feenox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"
#include "thermal.h"
extern feenox_t feenox;
thermal_t thermal;

///pb_thermal+NONE+description Laplace's equation does not need any extra keyword to `PROBLEM`.
int feenox_problem_parse_problem_thermal(const char *token) {

  // no need to parse anything;
  if (token != NULL) {
    feenox_push_error_message("undefined keyword '%s'", token);
    return FEENOX_ERROR;
  }
 
  return FEENOX_OK;
}


int feenox_problem_init_parser_thermal(void) {

#ifdef HAVE_PETSC
  feenox.pde.problem_init_runtime_particular = feenox_problem_init_runtime_thermal;
  feenox.pde.bc_parse = feenox_problem_bc_parse_thermal;
  feenox.pde.setup_ksp = feenox_problem_setup_ksp_thermal;
  feenox.pde.setup_pc = feenox_problem_setup_pc_thermal;
  feenox.pde.bc_set_dirichlet = feenox_problem_bc_set_thermal_temperature;
  feenox.pde.build_element_volumetric_gauss_point = feenox_problem_build_volumetric_gauss_point_thermal;
  feenox.pde.solve_post = feenox_problem_solve_post_thermal;
  feenox.pde.feenox_problem_gradient_fill = feenox_problem_gradient_fill_thermal;
  feenox.pde.feenox_problem_gradient_properties_at_element_nodes = feenox_problem_gradient_properties_at_element_nodes_thermal;
  feenox.pde.feenox_problem_gradient_fluxes_at_node_alloc = feenox_problem_gradient_fluxes_at_node_alloc_thermal;
  feenox.pde.feenox_problem_gradient_add_elemental_contribution_to_node = feenox_problem_gradient_add_elemental_contribution_to_node_thermal;
  feenox.pde.feenox_problem_gradient_fill_fluxes = feenox_problem_gradient_fill_fluxes_thermal;
  
  // thermal is a scalar problem
  feenox.pde.dofs = 1;
///re_thermal+T+description The temperature field\ $T(\vec{x})$. This is the primary unknown of the problem.  
  feenox_check_alloc(feenox.pde.unknown_name = calloc(feenox.pde.dofs, sizeof(char *)));
  feenox_check_alloc(feenox.pde.unknown_name[0] = strdup("T"));
  feenox_call(feenox_problem_define_solutions());
  feenox.mesh.default_field_location = field_location_nodes;
    
  // heat fluxes
///re_thermal+qx+description The heat flux field\ $q_x(\vec{x}) = -k(\vec{x}) \cdot \frac{\partial T}{\partial x}$ in the\ $x$ direction. This is a secondary unknown of the problem.  
///re_thermal+qy+description The heat flux field\ $q_y(\vec{x}) = -k(\vec{x}) \cdot \frac{\partial T}{\partial y}$ in the\ $x$ direction. This is a secondary unknown of the problem.
///re_thermal+qy+description Only available for two and three-dimensional problems.  
///re_thermal+qz+description The heat flux field\ $q_z(\vec{x}) = -k(\vec{x}) \cdot \frac{\partial T}{\partial z}$ in the\ $x$ direction. This is a secondary unknown of the problem.  
///re_thermal+qz+description Only available for three-dimensional problems.
  feenox_call(feenox_problem_define_solution_function("qx", &thermal.qx, 1));
  if (feenox.pde.dim > 1) {
    feenox_call(feenox_problem_define_solution_function("qy", &thermal.qy, 1));
    if (feenox.pde.dim > 2) {
      feenox_call(feenox_problem_define_solution_function("qz", &thermal.qz, 1));
    }
  }
  
  
///va_thermal+T_max+detail The maximum temperature\ $T_\text{max}$.
  feenox_check_null(thermal.T_max = feenox_define_variable_get_ptr("T_max"));

///va_thermal+T_min+detail The minimum temperature\ $T_\text{min}$.
  feenox_check_null(thermal.T_min = feenox_define_variable_get_ptr("T_min"));
  
#endif
  return FEENOX_OK;
}

int feenox_problem_init_runtime_thermal(void) {
  
#ifdef HAVE_PETSC
  // we are FEM not FVM
  feenox.pde.mesh->data_type = data_type_node;
  feenox.pde.spatial_unknowns = feenox.pde.mesh->n_nodes;
  feenox.pde.global_size = feenox.pde.spatial_unknowns * feenox.pde.dofs;
  
  // check if we were given an initial solution
  if ((feenox.pde.initial_condition = feenox_get_function_ptr("T_0")) != NULL) {
    if (feenox.pde.initial_condition->n_arguments != feenox.pde.dim) {
      feenox_push_error_message("initial condition function T_0 ought to have %d arguments instead of %d", feenox.pde.dim, feenox.pde.initial_condition->n_arguments);
      return FEENOX_ERROR;
    }
  }

  // initialize distributions
  // here we just initialize everything, during build we know which
  // of them are mandatory and which are optional

///pr_thermal+k+usage k
///pr_thermal+k+description The thermal conductivity in units of power per length per degree of temperature.
  feenox_distribution_define_mandatory(thermal, k, "k", "thermal conductivity");
  thermal.k.space_dependent = feenox_expression_depends_on_space(thermal.k.dependency_variables);
  thermal.k.non_linear = feenox_expression_depends_on_function(thermal.k.dependency_functions, feenox.pde.solution[0]);  
  
///pr_thermal+q'''+usage q'''
///pr_thermal+q'''+description The volumetric power dissipated in the material in units of power per unit of volume.
///pr_thermal+q'''+description Default is zero (i.e. no power).
  
  feenox_call(feenox_distribution_init(&thermal.q, "q'''"));
  if (thermal.q.defined == 0) {
///pr_thermal+q+usage q
///pr_thermal+q+description Alias for `q'''`
    feenox_call(feenox_distribution_init(&thermal.q, "q"));
  }
  thermal.q.space_dependent = feenox_expression_depends_on_space(thermal.q.dependency_variables);
  thermal.q.non_linear = feenox_expression_depends_on_function(thermal.q.dependency_functions, feenox.pde.solution[0]);  
  
  feenox.pde.has_mass = (feenox_var_value(feenox_special_var(end_time)) > 0) ? PETSC_TRUE : PETSC_FALSE;
  if (feenox.pde.has_mass) {
    feenox_call(feenox_distribution_init(&thermal.kappa, "kappa"));
    if (thermal.kappa.defined == 0) {
      feenox_call(feenox_distribution_init(&thermal.rhocp, "rhocp"));
      if (thermal.rhocp.defined == 0) {
        feenox_call(feenox_distribution_init(&thermal.rho, "rho"));
        feenox_call(feenox_distribution_init(&thermal.cp, "cp"));

        if (thermal.rho.defined == 0 || thermal.cp.defined == 0) {
          feenox_push_error_message("either 'kappa', 'rhocp' or both 'rho' and 'cp' are needed for transient");
          return FEENOX_ERROR;
        }
        if (thermal.rho.full == 0 || thermal.cp.full == 0) {
          feenox_push_error_message("either 'rho' or 'cp' is not defined over all volumes");
          return FEENOX_ERROR;
        }
      } else if (thermal.rhocp.full == 0) {
        feenox_push_error_message("product 'rhocp' is not defined over all volumes");
        return FEENOX_ERROR;
      }
    } else if (thermal.kappa.full == 0) {
      feenox_push_error_message("thermal diffusivity 'kappa' is not defined over all volumes");
      return FEENOX_ERROR;
    }
  }

  thermal.kappa.space_dependent = feenox_expression_depends_on_space(thermal.kappa.dependency_variables);
  thermal.rho.space_dependent   = feenox_expression_depends_on_space(thermal.rho.dependency_variables);
  thermal.cp.space_dependent    = feenox_expression_depends_on_space(thermal.cp.dependency_variables);
  thermal.rhocp.space_dependent = feenox_expression_depends_on_space(thermal.rhocp.dependency_variables);

  thermal.kappa.non_linear      = feenox_expression_depends_on_function(thermal.kappa.dependency_functions,  feenox.pde.solution[0]);
  thermal.rho.non_linear        = feenox_expression_depends_on_function(thermal.rho.dependency_functions,    feenox.pde.solution[0]);
  thermal.cp.non_linear         = feenox_expression_depends_on_function(thermal.cp.dependency_functions,     feenox.pde.solution[0]);
  thermal.rhocp.non_linear      = feenox_expression_depends_on_function(thermal.rhocp.dependency_functions,  feenox.pde.solution[0]);
  
  thermal.space_stiffness = thermal.k.space_dependent ||
                            thermal.kx.space_dependent ||
                            thermal.ky.space_dependent ||
                            thermal.kz.space_dependent;
  thermal.space_source = thermal.q.space_dependent;
  thermal.space_mass = thermal.kappa.space_dependent ||
                       thermal.rho.space_dependent   ||
                       thermal.cp.space_dependent    ||
                       thermal.rhocp.space_dependent;
  
  thermal.temperature_stiffness = feenox_expression_depends_on_function(thermal.k.dependency_functions,     feenox.pde.solution[0]) ||
                                  feenox_expression_depends_on_function(thermal.kx.dependency_functions,    feenox.pde.solution[0]) ||
                                  feenox_expression_depends_on_function(thermal.ky.dependency_functions,    feenox.pde.solution[0]) ||
                                  feenox_expression_depends_on_function(thermal.kz.dependency_functions,    feenox.pde.solution[0]);
  thermal.temperature_mass      = feenox_expression_depends_on_function(thermal.kappa.dependency_functions, feenox.pde.solution[0]) ||
                                  feenox_expression_depends_on_function(thermal.rho.dependency_functions,   feenox.pde.solution[0]) ||
                                  feenox_expression_depends_on_function(thermal.cp.dependency_functions,    feenox.pde.solution[0]) ||
                                  feenox_expression_depends_on_function(thermal.rhocp.dependency_functions, feenox.pde.solution[0]);
  
  // check BCs are consistent
  bc_t *bc = NULL;
  bc_t *bc_tmp = NULL;
  HASH_ITER(hh, feenox.mesh.bcs, bc, bc_tmp) {
    
    int first_type_math = bc_type_math_undefined;
    bc_data_t *bc_data = NULL;
    bc_data_t *bc_data_tmp = NULL;
    DL_FOREACH_SAFE(bc->bc_datums, bc_data, bc_data_tmp) {

      // this is general, maybe we can put it in a non-virtual method      
      if (first_type_math == bc_type_math_undefined) {
        first_type_math = bc_data->type_math;
      } else if (first_type_math != bc_data->type_math) {
        feenox_push_error_message("BCs of different types cannot be mixed '%s' and '%s'", bc->bc_datums->string, bc_data->string);
        return FEENOX_ERROR;
      }
      
      // convection (i.e. Robin) BCs have two arguments h and Tref,
      // we need to check they both are given (and only these two and nothing more)
      if (bc_data->type_math == bc_type_math_robin) {
        // first, check that there are only two bc_datums
        // utlist's doubly-linked lists work like this, sorry!
        if (bc_data == bc->bc_datums && bc_data->next == NULL) {
          feenox_push_error_message("both 'h' and 'Tref' have to be given in the same BC line");
          return FEENOX_ERROR;
        } else if (!(bc_data->prev == bc_data->next || bc_data->next == NULL)) {
          feenox_push_error_message("only 'h' and 'Tref' have to be given in the same BC line");
          return FEENOX_ERROR;
        }
        
        // check we have one h and one Tref
        if (bc_data == bc->bc_datums) {
          if ((bc_data->type_phys == BC_TYPE_THERMAL_CONVECTION_COEFFICIENT && bc_data->next->type_phys == BC_TYPE_THERMAL_CONVECTION_COEFFICIENT) ||
              (bc_data->type_phys == BC_TYPE_THERMAL_CONVECTION_TEMPERATURE && bc_data->next->type_phys == BC_TYPE_THERMAL_CONVECTION_TEMPERATURE)) {
            feenox_push_error_message("convection BC needs one 'h' and one 'Tref' in the same BC line");
            return FEENOX_ERROR;
          }
        } else if (bc_data->next == NULL) {
          // mark the second as disabled so only the first one is processed
          bc_data->disabled = 1;
        } else {
          feenox_push_error_message("internal mismatch in convection BC");
          return FEENOX_ERROR;
        }
        
      }
      
      bc_data->space_dependent = feenox_expression_depends_on_space(bc_data->expr.variables);
      thermal.space_bc |= bc_data->space_dependent;
      
      bc_data->nonlinear = feenox_expression_depends_on_function(bc_data->expr.functions, feenox.pde.solution[0]);
      thermal.temperature_bc |= bc_data->nonlinear;
      
    }
  }
  
  if (feenox.pde.math_type == math_type_automatic) {
    feenox.pde.math_type = (thermal.temperature_stiffness == PETSC_FALSE &&
                            thermal.temperature_mass      == PETSC_FALSE &&
                            thermal.temperature_source    == PETSC_FALSE &&
                            thermal.temperature_bc        == PETSC_FALSE) ? math_type_linear : math_type_nonlinear;
  }
  
  // TODO: check for transient_type
  feenox.pde.solve = (feenox_special_var_value(end_time) > 0) ? feenox_problem_solve_petsc_transient :
                         ((feenox.pde.math_type == math_type_linear) ? feenox_problem_solve_petsc_linear :
                                                                       feenox_problem_solve_petsc_nonlinear);
  
  feenox.pde.has_stiffness = PETSC_TRUE;
  feenox.pde.has_rhs = PETSC_TRUE;
  // has_mass is above
  
  feenox.pde.has_jacobian_K = thermal.temperature_stiffness;
  feenox.pde.has_jacobian_M = thermal.temperature_mass;
  feenox.pde.has_jacobian_b = (thermal.temperature_source || thermal.temperature_bc);
  feenox.pde.has_jacobian = feenox.pde.has_jacobian_K || feenox.pde.has_jacobian_M || feenox.pde.has_jacobian_b;
  
  feenox.pde.symmetric_K = PETSC_TRUE;
  feenox.pde.symmetric_M = PETSC_TRUE;
  
#endif  
  return FEENOX_OK;
}


#ifdef HAVE_PETSC
int feenox_problem_setup_pc_thermal(PC pc) {

  PCType pc_type = NULL;
  petsc_call(PCGetType(pc, &pc_type));
  if (pc_type == NULL) {
    petsc_call(PCSetType(pc, PCGAMG));
  }
  
  return FEENOX_OK;
}

int feenox_problem_setup_ksp_thermal(KSP ksp ) {

  KSPType ksp_type = NULL;
  petsc_call(KSPGetType(ksp, &ksp_type));
  if (ksp_type == NULL) {
    petsc_call(KSPSetType(ksp, KSPCG));
  }  

  return FEENOX_OK;
}
#endif