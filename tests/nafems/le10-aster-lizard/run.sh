#!/bin/bash

# check for needed tools
for i in gmsh jq /usr/bin/time; do
 if [ -z "$(which $i)" ]; then
  echo "error: $i not installed"
  exit 1
 fi
done

if [ ! -z "$(which feenox)" ]; then
  has_feenox="yes"
  rm -f feenox.dat
  $(feenox -V | grep mumps > /dev/null)
  if [ $? -eq 0 ]; then
    has_feenox_mumps="yes"
    rm -f feenox-mumps.dat
  fi
fi

if [ -e "sparselizard/sparselizard" ]; then
  has_sparselizard="yes"
  rm -f sparselizard.dat
fi

if [ ! -z "$(which as_run)" ]; then
  has_aster="yes"
fi

if [ ! -z "$(which reflex)" ]; then
  has_reflex="yes"
  rm -f reflex.dat
fi

# TODO: --check

# for c in 3 2 1.8 1.4 1.2 1.2 1.1 1.05 1.025 1 0.975 0.95 0.925 0.9 0.89 0.88; do
# for c in 3 2.5 2 1.5 1.25 1.1 1 0.9 0.8 0.75 0.7 0.65 0.6 0.55 0.5; do
for c in 3 2; do

  if [ ! -e le10-${c}.msh ]; then
    gmsh -3 le10.geo -clscale ${c} -o le10-${c}.msh || exit 1
  fi

  # count nodes
  gmsh - -v 0 le10-${c}.msh nodes.geo || exit 1
  mv nodes.txt le10-${c}.nodes
  
  # ---- FeenoX (GAMG) -----------------------------------------
  if [ ! -z "${has_feenox}" ]; then
    if [ ! -e feenox-${c}.sigmay ]; then
      echo "running FeenoX GAMG c = ${c}"
      /usr/bin/time -f "%e\t%S\t%U\t%M " -o feenox-${c}.time feenox le10.fee ${c} > feenox-${c}.sigmay || exit 1
    fi
    echo -ne "${c}\t" >> feenox.dat
    cat le10-${c}.nodes    | tr "\n" "\t" >> feenox.dat
    cat feenox-${c}.sigmay | tr "\n" "\t" >> feenox.dat
    cat feenox-${c}.time   | tr "\n" "\t" >> feenox.dat
    echo >> feenox.dat
  fi

  # ---- FeenoX (MUMPS) -----------------------------------------
  if [ ! -z "${has_feenox_mumps}" ]; then
    if [ ! -e feenox-mumps-${c}.sigmay ]; then
      echo "running FeenoX MUMPS c = ${c}"
      /usr/bin/time -f "%e\t%S\t%U\t%M " -o feenox-mumps-${c}.time feenox le10.fee ${c} --mumps > feenox-mumps-${c}.sigmay || exit 1
    fi
    echo -ne "${c}\t" >> feenox-mumps.dat
    cat le10-${c}.nodes    | tr "\n" "\t" >> feenox-mumps.dat
    cat feenox-mumps-${c}.sigmay | tr "\n" "\t" >> feenox-mumps.dat
    cat feenox-mumps-${c}.time   | tr "\n" "\t" >> feenox-mumps.dat
    echo >> feenox-mumps.dat
  fi
  
  # ---- Sparselizard ----------------------------------
  if [ ! -z "${has_sparselizard}" ]; then
    if [ ! -e sparselizard-${c}.sigmay ]; then
      echo "Running Sparselizard c = ${c}"
      cd sparselizard
      /usr/bin/time -f "%e\t%S\t%U\t%M " -o ../sparselizard-${c}.time ./run_sparselizard.sh ${c} > ../sparselizard-${c}.sigmay || exit 1
      cd ..
    fi
    echo -ne "${c}\t" >> sparselizard.dat
    cat le10-${c}.nodes    | tr "\n" "\t" >> sparselizard.dat
    cat sparselizard-${c}.sigmay | tr "\n" "\t" >> sparselizard.dat
    cat sparselizard-${c}.time   | tr "\n" "\t" >> sparselizard.dat
    echo >> sparselizard.dat
  fi
  
  if [ ! -z "${has_aster}" ]; then
    if [ ! -e le10-${c}.unv ]; then
      gmsh -3 le10-${c}.msh -o le10-${c}.unv || exit 1
    fi
    if [ ! -e aster-${c}.sigmay ]; then
      echo "running Aster c = ${c}"
      cp le10-${c}.unv le10.unv
      /usr/bin/time -f "%e\t%S\t%U\t%M " -o aster-${c}.time as_run le10.export || exit 1
      tail -n 2 DD.dat | head -n1 | awk '{print $3}' > aster-${c}.sigmay
    fi
    echo -ne "${c}\t" >> aster.dat
    cat le10-${c}.nodes    | tr "\n" "\t" >> aster.dat
    cat aster-${c}.sigmay | tr "\n" "\t" >> aster.dat
    cat aster-${c}.time   | tr "\n" "\t" >> aster.dat
    echo >> aster.dat


  fi

  # ---- Reflex ----------------------------------
  if [ ! -z "${has_reflex}" ]; then
    if [ ! -e reflex-${c}.sigmay ]; then
      echo "running Reflex c = ${c}"
      /usr/bin/time -f "%e\t%S\t%U\t%M " -o reflex-${c}.time reflex-develop -i le10.json -s "c=${c}" || exit 1
      jq .outputs.kpi_data[0].symm_tensor.data[0].yy output/le10_result_kpi.json > reflex-${c}.sigmay
    fi
    echo -ne "${c}\t" >> reflex.dat
    cat le10-${c}.nodes    | tr "\n" "\t" >> reflex.dat
    cat reflex-${c}.sigmay | tr "\n" "\t" >> reflex.dat
    cat reflex-${c}.time   | tr "\n" "\t" >> reflex.dat
    echo >> reflex.dat
    
  fi
  
done
