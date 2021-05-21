vk = Zero;
diffsh = Zero;
DiffError = Zero;


tmp<fv::convectionScheme<scalar>> mvConvection
(
    fv::convectionScheme<scalar>::New
    (
        mesh,
        fields,
        phi,
        mesh.divScheme("div(phi,Yi_h)")
    )
);

const volScalarField MW = thermo.W(); // Molecular weight [kg/kmol]
forAll(Y,i)
{
    forAll(T, cellI)
    {
        X[i][cellI] = Y[i][cellI]*MW[cellI]/composition.Wi(i);
    }
}


forAll(Y, i)
{
    const volScalarField& Yi = Y[i];
    const volScalarField& diffi = diff[i];
    DiffError += (diffi*fvc::grad(Yi) + Yi*diffi*fvc::grad(MW)/MW);
}
const surfaceScalarField phiUc = linearInterpolate(DiffError) & mesh.Sf();


{
    reaction->correct();
    volScalarField Yt(0.0*Y[0]);

    forAll(Y, i)
    {
        volScalarField& Yi = Y[i];
        volScalarField& Xi = X[i];
        const volScalarField& diffi = diff[i];


        forAll(T, icell)
        {
            Hsi[icell] = composition.Hs(i, p[icell], T[icell]);
        }

        volScalarField::Boundary& HsiBf = Hsi.boundaryFieldRef();

        forAll(HsiBf, patchi)
        {
            scalarField& HsiPatch = HsiBf[patchi];
            const scalarField& pp = p.boundaryField()[patchi];
            const scalarField& Tp = T.boundaryField()[patchi];

            forAll(HsiPatch, facei)
            {
                HsiPatch[facei] = composition.Hs(i, pp[facei], Tp[facei]);
            }
        }
        vk += Hsi*(diffi*fvc::grad(Yi) + Yi*diffi*fvc::grad(MW)/MW - Yi*DiffError);
        diffsh += fvc::laplacian(thermo.alpha()*Hsi, Yi);

        if (i != inertIndex && composition.active(i))
        {
            volScalarField DEff = diff[i] + turbulence->mut()/Sct;
            fvScalarMatrix YiEqn
            (
                fvm::ddt(rho, Yi)
              + mvConvection->fvmDiv(phi, Yi)
              //- fvm::laplacian(turbulence->muEff(), Yi) // original reactingFoam solver
              - fvm::laplacian(DEff, Yi)
              + mvConvection->fvmDiv(phiUc, Yi)
              - fvc::laplacian(diffi*Yi/MW, MW)
              // soret effect not included yet
             ==
                reaction->R(Yi)
              + fvOptions(rho, Yi)
            );

            YiEqn.relax();

            fvOptions.constrain(YiEqn);

            YiEqn.solve("Yi");

            fvOptions.correct(Yi);

            Yi.max(0.0);
            Yt += Yi;
        }
    }

    Y[inertIndex] = scalar(1) - Yt;
    Y[inertIndex].max(0.0);
}
