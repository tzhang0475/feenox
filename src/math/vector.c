/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX vector routines
 *
 *  Copyright (C) 2015--2017 jeremy theler
 *
 *  This file is part of FeenoX.
 *
 *  FeenoX is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  FeenoX is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FeenoX.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"
extern feenox_t feenox;


double feenox_vector_get(vector_t *this, const size_t i) {

  if (this->initialized == 0) {
    if (feenox_vector_init(this, 0) != FEENOX_OK) {
      feenox_push_error_message("initialization of vector '%s' failed", this->name);
      feenox_runtime_error();
    }
  }
  
  return gsl_vector_get(feenox_value_ptr(this), i);
}

double feenox_vector_get_initial_static(vector_t *this, const size_t i) {
  
  if (this->initialized == 0) {
    if (feenox_vector_init(this, 0) != FEENOX_OK) {
      feenox_push_error_message("initialization of vector '%s' failed", this->name);
      feenox_runtime_error();
    }
  }
  
  return gsl_vector_get(this->initial_static, i);
}

double feenox_vector_get_initial_transient(vector_t *this, const size_t i) {
  
  if (this->initialized == 0) {
    if (feenox_vector_init(this, 0) != FEENOX_OK) {
      feenox_push_error_message("initialization of vector '%s' failed", this->name);
      feenox_runtime_error();
    }
  }
  
  return gsl_vector_get(this->initial_transient, i);
}

int feenox_vector_set(vector_t *this, const size_t i, double value) {
  
  if (this->initialized == 0) {
    if (feenox_vector_init(this, 0) != FEENOX_OK) {
      feenox_push_error_message("initialization of vector '%s' failed", this->name);
      return FEENOX_ERROR;
    }
  }
  
  gsl_vector_set(feenox_value_ptr(this), i, value);
  
  return FEENOX_OK;
}

int feenox_vector_set_size(vector_t *this, size_t size) {
  if (this->initialized) {
    feenox_push_error_message("cannot set vector '%s' size, it is already initialized", this->name);
    return FEENOX_ERROR;
  }
  
  this->size = size;
  
  return FEENOX_OK;
  
}


int feenox_vector_init(vector_t *this, int no_initial) {

  int size;
  int i;
  expr_t *data;

  if (this->initialized) {
    return FEENOX_OK;
  }

  if ((size = this->size) == 0 && (size = (int)(round(feenox_expression_eval(&this->size_expr)))) == 0) {
    feenox_push_error_message("vector '%s' has zero size at initialization", this->name);
    return FEENOX_ERROR;
  } else if (size < 0) {
    feenox_push_error_message("vector '%s' has negative size %d at initialization", this->name, size);
    return FEENOX_ERROR;
  }
  
  this->size = size;
  feenox_value_ptr(this) = gsl_vector_calloc(size);
  if (no_initial == 0) {
    this->initial_static = gsl_vector_calloc(size);
    this->initial_transient = gsl_vector_calloc(size);
  }  

  if (this->datas != NULL) {
    i = 0;
    LL_FOREACH(this->datas, data) {
      gsl_vector_set(feenox_value_ptr(this), i++, feenox_expression_eval(data));
    }
  }

/*  
  if (this->function != NULL) {
    feenox_realloc_vector_ptr(this, this->function->data_value, 0);
  }
*/  
  this->initialized = 1;
  
  return FEENOX_OK;

}

int feenox_instruction_sort_vector(void *arg) {
  
  sort_vector_t *sort_vector = (sort_vector_t *)arg;
  
  if (sort_vector->v2 == NULL)
    gsl_sort_vector(sort_vector->v1->value);
  else
    gsl_sort_vector2(sort_vector->v1->value, sort_vector->v2->value);
  
  if (sort_vector->descending) {
    gsl_vector_reverse(sort_vector->v1->value);
    
    if (sort_vector->v2 != NULL)
      gsl_vector_reverse(sort_vector->v2->value);
  }
  
  return FEENOX_OK;
}
