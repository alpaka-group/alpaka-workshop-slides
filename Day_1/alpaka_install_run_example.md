```bash

# Connect to the remote server using SSH
ssh -i ~/.ssh/thinkpad narwalta@lumi.csc.fi

# Source the environment profile for the LUMI supercomputer
# This command sets up environment variables and tools.
# Loads the required modules on LUMI that are needed to build and run alpaka
source /project/project_465001310/workshop_software/env.sh

# Create and navigate to a source directory to store your project
mkdir src && cd src

# Clone the alpaka repository from the `develop` branch using SSH
git clone -b develop --single-branch git@github.com:alpaka-group/alpaka.git

# If SSH cloning fails, try using the HTTPS option:
# git clone -b develop --single-branch https://github.com/alpaka-group/alpaka.git

# Navigate to the project directory using the environment variable `$PROJID`
cd /project/$PROJID

# Check your username to verify the environment is set up correctly
echo $USER

# Create a personal directory for your project under the `$USER` directory
mkdir $USER

# Set the directory permissions to `700` (owner read/write/execute only)
chmod 700 $USER/

# Navigate into the newly created user directory
cd $USER

# Create directory for building
# The `build` directory will store build files 
mkdir build

# Enter the `build` directory
cd build

# Create a subdirectory for alpaka build files
mkdir alpaka && cd alpaka

getDevice

# The `local` directory will hold installed files
# Run CMake to generate build files from the source directory of alpaka
cmake -DCMAKE_INSTALL_PREFIX=/project/$PROJID/$USER/local ~/src/alpaka

# After configuration, exit the CMake interface and proceed to build the project.
# Use the `-j` flag to parallelize the build for faster compilation.
make -j

# Once the build is complete, install the compiled files to the `local` directory.
make install

# Add custom path where alpaka was installed to CMake search paths
export CMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH:/project/$PROJID/$USER/local"

# Run the vector addition example from alpaka to test installation and performance.
# Discuss how memory movement between devices may impact performance.

# Navigate back to your src in your home directory
cd ~/src

# Check the current working directory to confirm you are in the right place
# Expected output: `/users/<YOUR_USERNAME>/src`
pwd

# Copy the vectorAdd example from the alpaka source to it own folder in src
cp -r alpaka/example/vectorAdd/ .

cd vectorAdd

mkdir build && cd build

# Open the CMake configuration interface to customize the build process
cmake ..

# Inside the CMake configration interface activate the accelerators of interest 
# For example, enable HIP support by turning on `ALPAKA_HIP_ENABLE`.
ccmake .

# After configuration, exit the CMake interface and proceed to build the project.
# Use the `-j` flag to parallelize the build for faster compilation.
make -j

# Execute the vector add example
./vectorAdd

cd ~/src

# Clone the workshop repository from the `oct2024_workshop` branch using SSH
git clone -b oct2024_workshop --single-branch git@github.com:alpaka-group/alpaka-workshop-slides.git

# If SSH cloning fails, try using the HTTPS option:
# git clone -b oct2024_workshop --single-branch https://github.com/alpaka-group/alpaka-workshop-slides.git

# Mount the remote filesystem using SSHFS to make it accessible locally.
# The `sshfs` command mounts the remote directory to a local mount point.
# This can make file transfers and edits simpler by working directly with remote files.
sshfs -o follow_symlinks -o IdentityFile=~/.ssh/thinkpad narwalta@lumi.csc.fi:/users/narwalta/ /home/ikbuibui/mnt/ -o auto_cache
# THIS FOR PC and MAC?
