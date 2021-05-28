# reactingCanteraFoam

[![OpenFOAM version](https://img.shields.io/badge/OpenFOAM-7-brightgreen)](https://github.com/OpenFOAM/OpenFOAM-7)
[![Cantera version](https://img.shields.io/badge/Cantera-2.4%202.5-red)](https://github.com/Cantera/cantera)

This solver calls Cantera to update T psi mu alpha D in OpenFOAM

I have run test cases in Cantera-2.4 Cantera 2.5.1 with OpenFOAM. And the flame structures are better than that from reactingFoam.
Since I have no time to test the performance thoroughly, I hope someone can do this if interested.

The tool to convert Cantera results to OpenFOAM fields is from [JSqueo299 in GitHub](https://github.com/JSqueo299/Python/blob/main/Cantera/premixedFlames/Matlab2OF.m).

The correct of transport equation is copied from [ZSHtju in GitHub](https://github.com/ZSHtju/reactingDNS_OpenFOAM).

## How to compile

You can compile Cantera source code, to generate c++ library and headers, which is used in this OpenFOAM solver.


Or using libcantera installed by conda:
```
conda install -c cantera libcantera-devel
```
in this way, you should change Make/options to :
```
-I/path_to_anaconda3/envs/yourEnvName/include

/path_to_anaconda3/envs/yourEnvName/lib/libcantera_shared.so
```
PS: if you want to use Cantera Python module, you have to install it individually:
```
conda install -c cantera cantera
```

If you just want to run this solver for a try, you can use my pre-compiled Cantera-2.5.2 library and headers in `cantera_build`:
```
cd cantera_build/lib
tar -zxvf libcantera_shared2.5.2.so.tar.gz
ln -s libcantera_shared2.5.2.so libcantera_shared.so.2
cd ../..
wmake
```

## How to use

You can run the `testCase`:
```
cd testCase
export LD_LIBRARY_PATH=../cantera_build/lib:$LD_LIBRARY_PATH
export CANTERA_DATA=../cantera_build/data
reactingCanteraFoam
```

Or you can build your flame in OpenFOAM by these steps:
- `python adiabatic_flame.py`, to generate a premixed flame, you will get adiabatic_flame.csv
- `matlab Ctr2OF.m`, which will convert adiabatic_flame.csv to OpenFOAM `0` folder
- rebuild mesh according to the output of MATLAB (domain length and grid number, uniform mesh)
- run `reactingCanteraFoam` with following settup in constant/thermophysicalProperties

```
Sct 0.7;// ignore it if laminar
mechanismFile "h2_konnov_2008.xml"; // put the cantera mech file (*.cti or *.xml) in $FOAM_CASE or $CANTERA_DATA
transportModel "Mix"; // you can also try other transport models from Cantera: Multi, UnityLewis, Ion, water, HighP
```
