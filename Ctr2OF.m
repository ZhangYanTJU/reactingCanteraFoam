%% Script to convert Cantera results to OpenFOAM fields

% code from: https://github.com/JSqueo299/Python/blob/main/Cantera/premixedFlames/Matlab2OF.m



% read variable names from .csv file
fname = 'adiabatic_flame.csv';
fid = fopen(fname);
vars = textscan(fid,' %s ','Delimiter',',','MultipleDelimsAsOne',1);
fclose(fid);


% read data from Cantera .csv generated file
fid = fopen(fname);
hdr = fgetl(fid);
num = numel(regexp(hdr,',','split'));
Cantera = textscan(fid,repmat('%f ',1,num),'Delimiter',',','headerlines',1,'EmptyValue',0);
fclose(fid);        % Close the opened file
N_data = length(Cantera{1});
N_vars = length(Cantera) - 1;
x_Cantera = Cantera{1};
T_Cantera = Cantera{4};
Tu = T_Cantera(1); % unburnt gas T (K)
Tb = T_Cantera(end); % burnt gas T (K)
L = x_Cantera(end); % length of domain (m)
Sl = Cantera{3}(1);% laminar flame speed from Cantera (f.u[0])

% check laminar flame thickness calculation in Python with Matlab
dx_Cantera = diff(x_Cantera);
dT_Cantera = diff(T_Cantera);
Lf = (Tb - Tu) ./ max(dT_Cantera./dx_Cantera) ;
min_dx = Lf ./ 30;
min_cells = L ./ min_dx;
fprintf('Laminar flame speed: Sl = %.15g m/s \n',Sl)
fprintf('Laminar flame thickenss: Lf = %.15g m \n',Lf)
fprintf('Minimum grid spacing required: dx_min = %.15g m \n',min_dx)
fprintf('Minimum # of cells  required: %.2f cells \n',min_cells)

varname = string(vars{1}(2:N_vars+1));
varname(1) = 'p';
varname(2) = 'U';
varname(3) = 'T';


% Interpolation
xStart = 0;
xEnd = L;
OF_gridSize = ceil(min_cells);
OF_grid = linspace(xStart,xEnd,OF_gridSize)';
fprintf('OF_gridSize: %.0f \n',OF_gridSize)

for i = 1:N_vars
    F = griddedInterpolant(x_Cantera,Cantera{i+1},'spline') ;
    data = F(OF_grid);
    OF_data{i} = data;
end




% Write data into OpenFOAM format, 0/ folder
tFolder = 0;
mkdir(num2str(tFolder));
cd(num2str(tFolder));

for i = 1:N_vars
    fid = fopen(varname(i),'w'); % write permission
    fprintf(fid,'/*--------------------------------*- C++ -*----------------------------------*\\\n');
    fprintf(fid,'| =========                 |                                                 |\n');
    fprintf(fid,'| \\\\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox           |\n');
    fprintf(fid,'|  \\\\    /   O peration     | Version:  5.x                                   |\n');
    fprintf(fid,'|   \\\\  /    A nd           | Web:      www.OpenFOAM.org                      |\n');
    fprintf(fid,'|    \\\\/     M anipulation  |                                                 |\n');
    fprintf(fid,'\\*---------------------------------------------------------------------------*/\n');
    fprintf(fid,'FoamFile\n');
    fprintf(fid,'{\n');
    fprintf(fid,'    version     2.0;\n');
    fprintf(fid,'    format      ascii;\n');
    if strcmp(varname(i),'U')
        fprintf(fid,'    class       volVectorField;\n');
    else
        fprintf(fid,'    class       volScalarField;\n');
    end
    fprintf(fid,'    location    "%.15g";\n',tFolder);
    fprintf(fid,'    object       %s;\n',varname(i));
    fprintf(fid,'}\n');
    fprintf(fid,'// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //\n');
    fprintf(fid,'\n');

    if strcmp(varname(i),'T')
        fprintf(fid,'dimensions      [0 0 0 1 0 0 0];\n');
        fprintf(fid,'\n');
        fprintf(fid,'internalField   nonuniform List<scalar>\n');
        fprintf(fid,'%d\n',OF_gridSize);
        fprintf(fid,'(\n');
        fprintf(fid,'%.15f\n',OF_data{i});
        fprintf(fid,')\n;\n');
    elseif strcmp(varname(i),'U')
        fprintf(fid,'dimensions      [0 1 -1 0 0 0 0];\n');
        fprintf(fid,'\n');
        fprintf(fid,'internalField   nonuniform List<vector>\n');
        fprintf(fid,'%d\n',OF_gridSize);
        fprintf(fid,'(\n');
        fprintf(fid,'(%.15f 0.0 0.0)\n',OF_data{i});
        fprintf(fid,')\n;\n');
    elseif strcmp(varname(i),'p')
        fprintf(fid,'dimensions      [1 -1 -2 0 0 0 0];\n');
        fprintf(fid,'\n');
        fprintf(fid,'internalField   nonuniform List<scalar>\n');
        fprintf(fid,'%d\n',OF_gridSize);
        fprintf(fid,'(\n');
        fprintf(fid,'%.15e\n',OF_data{i});
        fprintf(fid,')\n;\n');
    else
        fprintf(fid,'dimensions      [0 0 0 0 0 0 0];\n');
        fprintf(fid,'\n');
        fprintf(fid,'internalField   nonuniform List<scalar>\n');
        fprintf(fid,'%d\n',OF_gridSize);
        fprintf(fid,'(\n');
        fprintf(fid,'%.15e\n',OF_data{i});
        fprintf(fid,')\n;\n');
    end


    fprintf(fid,'\nboundaryField\n');

    fprintf(fid,'{\n');
    fprintf(fid,'    inlet\n');
    fprintf(fid,'    {\n');
    if strcmp(varname(i),'T')
        fprintf(fid,'        type            fixedValue;\n');
        %fprintf(fid,'        value           uniform 300;\n');
        fprintf(fid,'        value           uniform ');
        fprintf(fid,'%.8f',Tu);
        fprintf(fid,';\n');
    elseif strcmp(varname(i),'U')
        fprintf(fid,'        type            fixedValue;\n');
        fprintf(fid,'        value           uniform ( ');
        fprintf(fid,'%.8f',Sl);
        fprintf(fid,' 0.000000 0.000000 );\n');
    elseif strcmp(varname(i),'p')
        fprintf(fid,'        type            zeroGradient;\n');
    elseif Cantera{i+1}(1) > 1e-7
        fprintf(fid,'        type            fixedValue;\n');
        fprintf(fid,'        value           uniform ');
        fprintf(fid,'%.8f',Cantera{i+1}(1));
        fprintf(fid,';\n');
    else
        fprintf(fid,'        type            fixedValue;\n');
        fprintf(fid,'        value           uniform 0;\n');
    end
    fprintf(fid,'    }\n');




    fprintf(fid,'    outlet\n');
    fprintf(fid,'    {\n');
    if strcmp(varname(i),'p')
        fprintf(fid,'        type            waveTransmissive;\n');
        fprintf(fid,'        gamma            1.4;\n');
    else
        fprintf(fid,'        type            zeroGradient;\n');
    end
    fprintf(fid,'    }\n');


    fprintf(fid,'    wall\n');
    fprintf(fid,'    {\n');
        fprintf(fid,'        type            empty;\n');
    fprintf(fid,'    }\n');


    fprintf(fid,'}\n');
    fprintf(fid,'\n');
    fprintf(fid,'\n');
    fprintf(fid,'// ************************************************************************* //\n');

    fclose(fid);
end

cd ..;
