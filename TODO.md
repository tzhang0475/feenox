# General things to do

 * implicit call to `SOLVE_PROBLEM`?
 * use `MatPreallocator` to efficiently pre-allocate the matrices? use a `DMPlex`? 
 * use `spack`
 * https://joss.theoj.org/
 * FIT to mesh-based functions
 * FIT taking into account uncertainties
 * extended integration (as in reduced, full, extended)
 
## Tests

 * move all `doc/examples` to `tests`
 * automated nightly builds and tests (Laminar CI?)
 * write a `README` and an explanation of each test
 * make an index of the keywords used in each test and link them from the doc as examples
 * cell-based mesh writes and integrations
 * `FIT` ranges

## Wasora keywords

 * `SEMAPHORE` + `SHARED_MEMORY`
 * `READ` / `WRITE`
 * `M4` (MUSTACHE? MACRO?)
 * `SHELL`
 * `HISTORY`
 * `PRINT_FUNCTION PHYSICAL_ENTITY`
 * `CALL` user routines (check hash of feenox.h)
 
## Wasora-like stuff

 * vector & matrix assignements & DAEs
 * debug mode, line-by-line
 * trap signit & sigterm

## API

 * Python?
 * Julia?
 * come up with an example in C

## Nice to have

 * logaritmic ranges for `PRINT_FUNCTION`
 * default separator after `TEXT` should be space, after numerical should be tab
 * `PRINTF` instruction
 * `PRINT_FUNCTION` with `%f` in between arguments (like `PRINT_VECTOR`)
 * `BLAS` 
   - `BLAS a * b = s   # scalar dot`
   - `BLAS a x b = c   # vector cross`
   - `BLAS A * b = c   # matrix-vector`
   - `BLAS a + b = c   # vector`
   - `BLAS A + B = C   # matrix`
   - `BLAS a - b = c`
   - `BLAS A - B = C`
   
 
## Command-line arguments

 * list symbols with `-l` (is there any clash with petsc?)
 * `$1` = `${1}`
 * `$0` should have the main input file basename (without the extension)
 
## Error handling 
 
 * GSL error handling
 * `feenox_nan_error()`
 * `feenox_gsl_handler()`

## Postprocessing output

 * formats
   - vtu?
   - xdmf? (needs DMView() and VecView())
   - frd? (to be able to use ccx's post processor... is it worth it?)
   - binary msh and vtk
   - msh v4.1
   - cgns?
   - med?
 
 * full tensors
 * problem-aware `WRITE_POST`
   - `full | displacements | stresses | strains | von_mises | tresca | principal`
   - `full | temperature | heat_flux`
   - `full | mode_1 | mode_n1 | modes`
   - `full | flux_1 | fluxes | currents`
 
## Extensions
 
 * mustache instead of m4
 * gmsh as a library? maybe not, just import both gmsh and feenox in Python and use them together
 * use dmplex? for real or just for allocating the matrices?
 
# PDEs

 * check when the matrices do not need to be rebuilt -- use constant flag
 * allow different material model for each volume!
   - so far we have a global material model, i.e. all the materials have to be orthotropic
   - this is wrong!
 * remove the need of needing an explicit `SOLVE_PROBLEM`
   - detect which variables/functions are PDE outputs and call `solve_problem()` the first time they are needed
 * benchmark MPI (read `.msh` with partitioning info first)
 
## Laplace/Poisson/Helmholtz

 * poisson f = 1, eta = 0
 * helmholtz f = 1, eta = 1
 * see 2D example in Gmsh list
 
## Heat

 * parallel runs
 * convection with a separate matrix? think!
 * jacobians for non-linear problems
 * explain that radiation is better off as an expression
 * same for total heat sources/bcs
 * orthotropic conductivity
 
## Mechanical

 * non-linear
 * stresses: count negative jacobians and worst jacobians
 * stress linearization (per-problem parser)
 * strain energy (global and local)

## Modal

 * partially-unconstrained DOFs
 
## Neutron

 * transport
 * FVM
 * benchmarks
 
## V & V

 * MMS

