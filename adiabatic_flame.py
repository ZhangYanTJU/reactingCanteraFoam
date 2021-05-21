"""
A freely-propagating, premixed hydrogen flat flame with multicomponent
transport properties.

Requires: cantera >= 2.5.0
"""

import cantera as ct
import numpy as np
import csv

# Simulation parameters
p = 10*ct.one_atm  # pressure
Tin = 900.0  # unburned gas temperature [K]  multicomponent flamespeed = 1.382267 m/s
#Tin = 300.0  # unburned gas temperature [K]  multicomponent flamespeed = 0.130404 m/s
fuel = "IC8H18:0.7, C7H16:0.3" # mole fraction
oxidizer = "O2:0.21,N2:0.79" # mole fraction
phi=1

width = 0.03  # m
loglevel = 1  # amount of diagnostic output (0 to 8)


mech='TRF.cti'
# Solution object used to compute mixture properties, set to the state of the
# upstream fuel-air mixture
gas = ct.Solution(mech)
#gas.TPX = Tin, p, reactants
gas.TP = Tin, p
gas.set_equivalence_ratio(phi, fuel, oxidizer)


# Set up flame object
f = ct.FreeFlame(gas, width=width)
f.set_refine_criteria(ratio=3, slope=0.06, curve=0.12)
#f.show_solution()

# Solve with mixture-averaged transport model
f.transport_model = 'Mix'
f.solve(loglevel=loglevel, auto=True)

#f.show_solution()
print('mixture-averaged flamespeed = {0:7f} m/s'.format(f.velocity[0]))

# Solve with multi-component transport properties
f.transport_model = 'Multi'
f.solve(loglevel)  # don't use 'auto' on subsequent solves
#f.show_solution()
print('multicomponent flamespeed = {0:7f} m/s'.format(f.velocity[0]))


# Save to .csv file
csv_file = 'adiabatic_flame.csv'
with open(csv_file, 'w') as outfile:
    writer = csv.writer(outfile)
    writer.writerow(['z (m)']  +  ['p (Pa)'] +  ['U (m/s)'] + ['T (K)'] + gas.species_names)
    for i in range(len(f.grid)):
        writer.writerow([f.grid[i]] + [p] + [f.velocity[i]] + [f.T[i]] + list(f.Y[:,i]))

print('Output written to adiabatic_flame.csv')


maxSlope = ( f.T[1] - f.T[0] ) / ( f.grid[1] - f.grid[0] )
for i in range(2,f.flame.n_points):
    slope=( f.T[i] - f.T[i-1] ) / ( f.grid[i] - f.grid[i-1] )
    if slope > maxSlope :
        maxSlope = slope
flameThickness=  (max(f.T)-min(f.T))/maxSlope
print('flameThickness =', flameThickness)
