const volScalarField& psi = thermo.psi();
volScalarField& T = thermo.T();
const label inertIndex(composition.species()[inertSpecie]);

word mechanismFile(thermo.lookup("mechanismFile"));
Cantera::IdealGasMix gas(mechanismFile, "");
word transportModel(thermo.lookup("transportModel")); // Mix, Multi, UnityLewis, Ion, water, HighP
autoPtr<Cantera::Transport> trans(newTransportMgr(transportModel, &gas, 0));

if (gas.nSpecies() != composition.species().size())
{
    FatalErrorIn("CanteraChemistryModel::solve")
        << "nSpecies in Cantera is not consistent with that in OpenFOAM" << endl
        << abort(FatalError) ;
}
else
{
    forAll(Y, i)
    {
        if (gas.speciesName(i) != Y[i].name())
        {
            FatalErrorIn("CanteraChemistryModel::solve")
                << "species name in Cantera is not consistent with that in OpenFOAM" << endl
                << "in Cantera:" << gas.speciesName(i) << endl
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
            dimensionedScalar(dimDynamicViscosity, 0) // kg/(m s)
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