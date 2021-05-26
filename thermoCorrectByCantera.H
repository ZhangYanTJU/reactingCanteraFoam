{
    //thermo.correct():
    //{
    //    this->psi_.oldTime();
    //    calculate();
    //}

    volScalarField& psi = const_cast<volScalarField&>(thermo.psi());
    volScalarField& mu = const_cast<volScalarField&>(thermo.mu()());
    volScalarField& alpha = const_cast<volScalarField&>(thermo.alpha());


    psi.oldTime();


    //volScalarField& he = thermo.he(); // [J/kg]
    hc = thermo.hc(); // [J/kg]
    ha = hc + thermo.he(); // [J/kg]

    const scalarField& hCells = ha;
    const scalarField& pCells = p;
    scalarField& TCells = T.primitiveFieldRef();
    scalarField& psiCells = psi.primitiveFieldRef();
    scalarField& muCells = mu.primitiveFieldRef();
    scalarField& alphaCells = alpha.primitiveFieldRef();

    forAll(TCells, celli)
    {
        scalarList yTemp(Y.size());
        forAll(Y, i)
        {
            yTemp[i] = Y[i][celli];
        }
        gas->setState_PY(pCells[celli], yTemp.begin());
        gas->setState_HP(hCells[celli], pCells[celli]); // setState_HP needs (J/kg)

        TCells[celli] = gas->temperature();

        psiCells[celli] = gas->meanMolecularWeight()/gas->RT(); // meanMolecularWeight() kg/kmol    RT() Joules/kmol

        muCells[celli] = trans->viscosity(); // Pa-s

        alphaCells[celli] = trans->thermalConductivity()/(gas->cp_mass()); // thermalConductivity() W/m/K   cp_mass()   J/kg/K

        scalarList diffTemp(gas->nSpecies());
        trans->getMixDiffCoeffs(&diffTemp[0]); // m2/s
        forAll(Y, i)
        {
            diff[i][celli] = diffTemp[i];
        }
    }

    volScalarField::Boundary& pBf =
        p.boundaryFieldRef();

    volScalarField::Boundary& TBf =
        T.boundaryFieldRef();

    volScalarField::Boundary& psiBf =
        psi.boundaryFieldRef();

    volScalarField::Boundary& hBf =
        ha.boundaryFieldRef();

    volScalarField::Boundary& muBf =
        mu.boundaryFieldRef();

    volScalarField::Boundary& alphaBf =
        alpha.boundaryFieldRef();

    forAll(T.boundaryField(), patchi)
    {
        fvPatchScalarField& pp = pBf[patchi];
        fvPatchScalarField& pT = TBf[patchi];
        fvPatchScalarField& ppsi = psiBf[patchi];
        fvPatchScalarField& ph = hBf[patchi];
        fvPatchScalarField& pmu = muBf[patchi];
        fvPatchScalarField& palpha = alphaBf[patchi];

        if (pT.fixesValue())
        {
            forAll(pT, facei)
            {
                scalarList yTemp(Y.size());
                forAll(Y, i)
                {
                    yTemp[i] = Y[i].boundaryField()[patchi][facei];
                }
                gas->setState_TPY(pT[facei], pp[facei], yTemp.begin());

                ph[facei] = gas->enthalpy_mass();

                ppsi[facei] = gas->meanMolecularWeight()/gas->RT();

                pmu[facei] = trans->viscosity();

                palpha[facei] = trans->thermalConductivity()/(gas->cp_mass());

                scalarList diffTemp(gas->nSpecies());
                trans->getMixDiffCoeffs(&diffTemp[0]);
                forAll(Y, i)
                {
                    diff[i].boundaryFieldRef()[patchi][facei] = diffTemp[i];
                }
            }
        }
        else
        {
            forAll(pT, facei)
            {
                scalarList yTemp(Y.size());
                forAll(Y, i)
                {
                    yTemp[i] = Y[i].boundaryField()[patchi][facei];
                }
                gas->setState_PY(pp[facei], yTemp.begin());
                gas->setState_HP(ph[facei], pp[facei]);

                pT[facei] = gas->temperature();

                ppsi[facei] = gas->meanMolecularWeight()/gas->RT();

                pmu[facei] = trans->viscosity();

                palpha[facei] = trans->thermalConductivity()/(gas->cp_mass());

                scalarList diffTemp(gas->nSpecies());
                trans->getMixDiffCoeffs(&diffTemp[0]);
                forAll(Y, i)
                {
                    diff[i].boundaryFieldRef()[patchi][facei] = diffTemp[i];
                }
            }
        }
    }
}