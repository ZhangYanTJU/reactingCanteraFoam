# reactingCanteraFoam

This solver calls Cantera to update T psi mu alpha D in OpenFOAM

You can build your flame in OpenFOAM by these steps:
- copy one case from [this repository](https://github.com/ZSHtju/reactingDNS_OpenFOAM), remove the 0 folder.
- python adiabatic_flame.py, to generate a premixed flame in file: adiabatic_flame.csv
- matlab Ctr2OF.m, which will convert adiabatic_flame.csv to OpenFOAM 0 folder.
- rebuild mesh according to the output of matlab (domain length and grid number, uniform mesh)
- run reactingCanteraFoam with following settup in constant/thermophysicalProperties
```
Sct 0.7;// ignore it if laminar
mechanismFile "TRF.cti"; // put TRF.cti in $FOAM_CASE, needed by Cantera
transportModel "Mix"; // you can also try other transport models from Cantera: Multi, UnityLewis, Ion, water, HighP
```

I have run test cases in Cantera-2.4 Cantera 2.5.1 with OpenFOAM. And the flame structures are better than that from reactingFoam.
Since I have no time to test the performance thoroughly, I hope someone can do this if interested.

The tool to convert Cantera results to OpenFOAM fields is from [JSqueo299 in GitHub](https://github.com/JSqueo299/Python/blob/main/Cantera/premixedFlames/Matlab2OF.m).

The correct of transport equation is copied from [ZSHtju in GitHub](https://github.com/ZSHtju/reactingDNS_OpenFOAM).
