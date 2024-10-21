```bash

# General overview of variables and directories to be used/created
#
# - Environment variables 
# $USER   :your LUMI user-id
# $PROJID :LUMI project name and folder name we will be using (actually it is "project_465001310") 
# 
# - Code Directories to be created 
# /users/$USER/src : the directory to be created for alpaka source and training codes
#
# - alpaka Installation Directories to be created for alpaka install
# /scratch/$PROJID/$USER/build : The build directory for alpaka installation   
# /scratch/$PROJID/$USER/local : The install directory for alpaka

# Connect to LUMI using SSH
# Use your own ssh-key filename and your LUMI account name
ssh -i ~/pathTo/key username@lumi.csc.fi

# Source the environment profile for the LUMI supercomputer
# This command sets up environment variables and tools.
# Loads the required modules on LUMI that are needed to build and run alpaka
source /project/project_465001310/workshop_software/env.sh

# Create and navigate to a source directory to store your project
mkdir src && cd src

# Clone the alpaka repository from the `1.2.0` release using HTTPS
git clone -b 1.2.0 --single-branch https://github.com/alpaka-group/alpaka.git

# Navigate to the project directory using the environment variable `$PROJID`
cd /scratch/$PROJID

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

# Allocate resources by getDevice function
# To see what getDevice script function is doing use "declare -f getDevice"
getDevice

# The `local` directory will hold installed files
# Run CMake to generate build files from the source directory of alpaka
cmake -DCMAKE_INSTALL_PREFIX=/scratch/$PROJID/$USER/local ~/src/alpaka

# After configuration, exit the CMake interface and proceed to build the project.
# Use the `-j` flag to parallelize the build for faster compilation.
make -j

# Once the build is complete, install the compiled files to the `local` directory.
make install

# Add custom path where alpaka was installed to CMake search paths
export CMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH:/scratch/$PROJID/$USER/local"

# Run the vector addition example from alpaka to test installation and performance.
# Discuss how memory movement between devices may impact performance.

# Navigate back to your src in your home directory
cd ~/src

# Check the current working directory to confirm you are in the right place
# Expected output: `/users/<YOUR_USERNAME>/src`
pwd

# Copy the vectorAdd example from the alpaka source to it's own folder in src
cp -r alpaka/example/vectorAdd/ .

cd vectorAdd

mkdir build && cd build

# Open the CMake configuration interface to customize the build process
cmake ..

# Inside the CMake configration interface called "ccmake", activate the accelerators of interest 
# For example, enable HIP support by turning on `alpaka_ACC_GPU_HIP_ENABLE`.
# If mdspan is used in the project set alpaka_USE_MDSPAN to FETCH
ccmake .

# As an alternative to setting cmake param at "ccmake" cmake command can be used as well: 
#    cmake -DCMAKE_BUILD_TYPE=Release \
#          -Dalpaka_ACC_CPU_B_SEQ_T_SEQ_ENABLE=ON \
#          -Dalpaka_ACC_GPU_HIP_ENABLE=ON
#          -Dalpaka_USE_MDSPAN=FETCH ..

# After configuration, exit the CMake interface and proceed to build the project.
# Use the `-j` flag to parallelize the build for faster compilation.
make -j

# Execute the vector add example
./vectorAdd

cd ~/src

# Clone the workshop repository from the `oct2024_workshop` branch 
git clone -b oct2024_workshop --single-branch https://github.com/alpaka-group/alpaka-workshop-slides.git

# File Transfer and Sharing methods if needed

# 1. Using SFTP for File Transfer
# From GNU/Linux terminal or Windows PowerCell
sftp your_lumi_username@lumi.csc.fi
# See which remote directory you are in
sftp> pwd
# Copy your local file to a LUMI directory /users/your_lumi_username or any other directory
sftp> put local_file_name.txt /users/your_lumi_username
# Do "ls" to see remote directory. Check if your local file is transferred 
sftp> ls
# Copy your remote file to local computer
sftp> get remote_file_name.txt local_directory
# Create a remote directory if needed
sftp> mkdir remote_directory

# 2. Mounting, in case a local mounting point for remote file system is preferred
#
# Mount the remote filesystem using SSHFS to make it accessible locally (for example at your laptop).
# The `sshfs` command mounts the remote directory to a local mount point (for example /home/your_local_username/mnt/)
# This can make file transfers and edits simpler by working directly with remote files.

sshfs -o follow_symlinks -o IdentityFile=~/.ssh/your_key_for_lumi your_lumi_username@lumi.csc.fi:/users/your_lumi_username/ /home/your_local_username/mnt/ -o auto_cache

# A working example command for name_example LUMI user, local user directory is /home/username/mnt/ to mount the lumi filesystem
# sshfs -o follow_symlinks -o IdentityFile=~/pathTo/key name_example@lumi.csc.fi:/users/name_example/ /home/username/mnt/ -o auto_cache
# NOTE for macOS: macOS needs macFUSE and sshfs for mounting remote directories.
# NOTE for Windows: Windows requires WinFsp and SSHFS-Win to achieve the same functionality.

