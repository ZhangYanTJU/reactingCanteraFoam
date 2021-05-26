const volScalarField& psi = thermo.psi();
volScalarField& T = thermo.T();
const label inertIndex(composition.species()[inertSpecie]);

//for cantera 2.4
//Cantera::IdealGasMix gas(mechanismFile, "");
//autoPtr<Cantera::Transport> trans(newTransportMgr(transportModel, &gas, 0));

//for cantera 2.5
word mechanismFile(thermo.lookup("mechanismFile"));
auto sol = Cantera::newSolution(mechanismFile, "");
auto gas = sol->thermo();
word transportModel(thermo.lookup("transportModel")); // Mix, Multi, UnityLewis, Ion, water, HighP
autoPtr<Cantera::Transport> trans(newTransportMgr(transportModel, sol->thermo().get()));

const long unsigned int nSpecies_OF = composition.species().size();
if (gas->nSpecies() != nSpecies_OF)
{
    FatalErrorIn("CanteraChemistryModel::solve")
        << "nSpecies in Cantera is not consistent with that in OpenFOAM" << endl
        << abort(FatalError) ;
}
else
{
    forAll(Y, i)
    {
        if (gas->speciesName(i) != Y[i].name())
        {
            FatalErrorIn("CanteraChemistryModel::solve")
                << "species name in Cantera is not consistent with that in OpenFOAM" << endl
                << "in Cantera:" << gas->speciesName(i) << endl
                << "in OpenFOAM:" << Y[i].name() << endl
                << abort(FatalError) ;
        }
    }
}


PtrList<volScalarField> diff(Y.size());
forAll(Y, i)
{
    diff.set
    (
        i,
        new volScalarField
        (
            IOobject
            (
                "diff_" + Y[i].name(),
                runTime.timeName(),
                mesh,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh,
            dimensionedScalar(dimViscosity, 0) // m2/s
        )
    );
}

scalar Sct = readScalar(thermo.lookup("Sct"));

volScalarField hc
(
    IOobject
    (
        "hc",
        runTime.timeName(),
        mesh,
        IOobject::READ_IF_PRESENT,
        IOobject::AUTO_WRITE
    ),
    thermo.hc()
);
volScalarField ha
(
    IOobject
    (
        "ha",
        runTime.timeName(),
        mesh,
        IOobject::READ_IF_PRESENT,
        IOobject::AUTO_WRITE
    ),
    thermo.he()+thermo.hc()
);



volScalarField diffsh
(
    IOobject
    (
        "diffsh",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::NO_WRITE
    ),
    mesh,
    dimensionedScalar(dimEnergy/dimTime/dimVolume, 0)
);
volVectorField vk
(
    IOobject
    (
        "vk",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::NO_WRITE
    ),
    mesh,
    dimensionedVector(dimensionSet(1,0,-3,0,0,0,0), Zero)
);
volScalarField Hsi
(
    IOobject
    (
        "Hsi",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::NO_WRITE
    ),
    mesh,
    dimensionedScalar(dimEnergy/dimMass, 0.),
    zeroGradientFvPatchScalarField::typeName
);
volVectorField DiffError
(
    IOobject
    (
        "DiffError",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::NO_WRITE
    ),
    mesh,
    dimensionedVector("DiffError", dimDynamicViscosity/dimLength, Zero)
);
