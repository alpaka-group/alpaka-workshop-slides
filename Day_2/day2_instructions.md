```bash

# Connect to LUMI using SSH, or using the https://www.lumi.csc.fi/public/ web interface
# Use your own ssh-key filename and your LUMI account name
ssh -i ~/pathTo/key username@lumi.csc.fi

# Source the environment profile for the LUMI supercomputer
# This command sets up environment variables and tools.
# Loads the required modules on LUMI that are needed to build and run alpaka
source /project/project_465001310/workshop_software/env.sh

# navigate to the location where we cloned our workshop repository
cd ~/src/alpaka-workshop-slides

# Pull the latest changes from the remote repository for the alpaka workshop slides
# This ensures your local repository is up to date with any recent updates or fixes.
git pull

