import json
import os
import errno
import argparse

# ==============================================================================================
# generate_particles.py
# ----------------------------------------------------------------------------------------------
# Angela Gross
# ----------------------------------------------------------------------------------------------
# Generates cs particle JSON files that are clones of Base.csparticle except either the particles
# per emission (PPE) or total maximum particles (TMP) property is changed.
# ==============================================================================================

# //////////////////////////////////////////////////////////////////////////////////////////////

# ---------------------------------------------------------------------------------------------
# GLOBAL VARIABLES
# ---------------------------------------------------------------------------------------------

# Default particle names and base particle data holder
k_fileName = "%d_particles.csparticle"
k_mainParticleFileName = "Base.csparticle"
k_mainParticleData = ""

# Read with arguments- these are filler values
k_generatedParticlesDirectory = "Generated"
k_isPPEChanging = True
k_isTMPChanging = False
k_minParticles = 0
k_maxParticles = 1000
k_particlesStep = 100
k_ppeStep = 1.0
k_tmpStep = 1.0
k_constantParticles = 10000

# //////////////////////////////////////////////////////////////////////////////////////////////

# ---------------------------------------------------------------------------------------------
# HELPER FUNCTIONS
# ---------------------------------------------------------------------------------------------

def clearParticlesDirectory():
	# change to generated particles directory
	cwd = os.getcwd()
	os.chdir(k_generatedParticlesDirectory)

	# remove all .csparticle files in the directory
	filelist = [ f for f in os.listdir(".") if f.endswith(".csparticle") ]
	for f in filelist:
		os.remove(f)
		
	# go back to the current working directory
	os.chdir(cwd)

def createParticlesDirectory():
	# try to create the particles directory
	try:
		os.makedirs(k_generatedParticlesDirectory)
	except OSError as exception:
		if exception.errno != errno.EEXIST:
			raise

def createParticleFile(in_changingParticles):
	# copy the data
	newParticleData = k_mainParticleData;
	
	# format the file name and change the changing particle attribute
	newParticleFileName = k_generatedParticlesDirectory + "/" + ( k_fileName % (in_changingParticles) )
	if k_isPPEChanging:
		newParticleData["Emitter"]["ParticlesPerEmissionProperty"] = "%s" % (in_changingParticles * k_ppeStep)
	if k_isTMPChanging:
		if in_changingParticles == 0:
			# this value is used to allocate an array, so it needs to be at least 1
			newParticleData["MaxParticles"] = "1"
		else:
			newParticleData["MaxParticles"] = "%s" % (in_changingParticles * k_tmpStep)

	# write file
	with open(newParticleFileName, 'w') as file:
		json.dump(newParticleData, file, indent=4, sort_keys=True)

# //////////////////////////////////////////////////////////////////////////////////////////////

# ---------------------------------------------------------------------------------------------
# ARGUMENT PARSER HELPERS
# ---------------------------------------------------------------------------------------------

def restricted_float(x):
    x = float(x)
    if x < 0.0 or x > 1.0:
        raise argparse.ArgumentTypeError("%r not in range [0.0, 1.0]"%(x,))
    return x

# //////////////////////////////////////////////////////////////////////////////////////////////

# ---------------------------------------------------------------------------------------------
# MAIN
# ---------------------------------------------------------------------------------------------

if __name__ == "__main__":

	# Setup argument parsing
	argParser = argparse.ArgumentParser(description='Generate Particle JSON Files')
	argParser.add_argument('-changing', required=True, choices=['PPE', 'TMP', 'Both'], help='Either the particles per emission (PPE) or total max particles (TMP) will be changing from min to max.')
	argParser.add_argument('-min', default=0, type=int, help='The minimum particles- either PPE or TMP. (non-negative number, default: %(default)s)')
	argParser.add_argument('-max', default=1000, type=int, help='The maximum particles- either PPE or TMP. (non-negative number, default: %(default)s)')
	argParser.add_argument('-step', default=100, type=int, help='The step between min and max particles- either PPE or TMP. (default: %(default)s)')
	argParser.add_argument('-constant', default=10000, type=int, help='The constant number of particles, either PPE or TMP but it will be opposite of arg "changing". If both are changing, then this has no effect. (default: %(default)s)')
	argParser.add_argument('-tmpStep', default=1.0, type=restricted_float, help='The percent that TMP will step by if it is changing. (default: %(default)s)')
	argParser.add_argument('-ppeStep', default=1.0, type=restricted_float, help='The percent that PPE will step by if it is changing. (default: %(default)s)')
	argParser.add_argument('-dir', default='Generated', help='The output directory name (default is "%(default)s" and it will create it for you)')
	args = vars(argParser.parse_args())

	# Parse arguments
	k_minParticles = args['min']
	k_maxParticles = args['max']
	k_particlesStep = args['step']
	k_constantParticles = args['constant']
	k_generatedParticlesDirectory = args['dir']
	k_isTMPChanging = args['changing'] == 'TMP' or args['changing'] == 'Both'
	k_isPPEChanging = args['changing'] == 'PPE' or args['changing'] == 'Both'
	k_tmpStep = args['tmpStep']
	k_ppeStep = args['ppeStep']

	# Get copy of base particle file
	with open(k_mainParticleFileName, 'r') as file:
		k_mainParticleData = json.load(file)
	
	# Update constant particles attribute
	if not k_isTMPChanging:
		k_mainParticleData["MaxParticles"] = "%s" % k_constantParticles
	elif not k_isPPEChanging:
		k_mainParticleData["Emitter"]["ParticlesPerEmissionProperty"] = "%s" % k_constantParticles
	
	# Write the base file
	with open(k_mainParticleFileName, 'w') as file:
		json.dump(k_mainParticleData, file, indent=4, sort_keys=True)

	# Created and clear generated particles directory
	createParticlesDirectory()
	clearParticlesDirectory()

	# Generate particles 
	for i in range(k_minParticles, k_maxParticles + 1, k_particlesStep):
		createParticleFile(i)

# //////////////////////////////////////////////////////////////////////////////////////////////


