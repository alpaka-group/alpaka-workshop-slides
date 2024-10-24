# PIConGPU usage hands-on

Please refer to the slides. This is just for quick copy past of commands.

## LUMI access

Project accounting:
https://my.lumi-supercomputer.eu/

SSH-keys:
```bash
ssh-keygen -t ed25519 -f /home/username/.ssh/id_juwels_ed25519
```
https://mms.myaccessid.org/profile/

login via ssh:
````bash
ssh -i /.ssh/id_lumi_ed25519 your_username@lumi.csc.fi
```

web access:
http://www.lumi.csc.fi/

## workshop project on LUMI
`project_465001310`

create:
```bash
cd /projappl/project_465001310
mkdir $(whoami)

cd /scratch/project_465001310
mkdir $(whoami)
```

## PIConGPU first example

our main documentation:
https://picongpu.readthedocs.io/

# get code:
````bash
cd $HOME/
mkdir src
cd src
git clone https://github.com/ComputationalRadiationPhysics/picongpu.git
```

copy a LUMI profile:
```bash
cp picongpu/etc/picongpu/lumi-eurohpc/lumi-G_hipcc_picongpu.profile.example /projappl/project_465001310/$(whoami)/lumi-G_hipcc_picongpu.profile
```

adjust:
        - [line 8] `export MY_MAILNOTIFY="ALL"`
        - [line 9] `export MY\_MAIL="your.mail@server.gov"`
        - [line 16] `export PROJID=project_465001310`
        - [line 17] `export PROJECT_DIR=/projappl/$PROJID/`
        - [line 18] `export SCRATCH=/scratch/$PROJID/`
        - [line 27] `export EDITOR="emacs -nw"`
        - [line 72] `export PIC_LIBS=$PROJECT_DIR/workshop_software/local`
        - [line 81] `export PICSRC=$HOME/src/picongpu`

```bash
`source /projappl/project_465001310/$(whoami)/lumi-G_hipcc_picongpu.profile
```

copy default example:
```bash
mkdir -p picInputs
cd picInputs
pic-create LaserWakefield LWFA
```

adjust laser parameter
````bash
cd LWFA/include/picongpu/param
ls -l
```

edit: `incidentField.param`

        - [line 66] change $a_0$ from `8.0` to `5.0`

adjust run time setting:
```bash
cd -
cd LWFA/etc/picongpu/
ls -l
```

edit ``4.cfg`:

        - [line 68] remove checkpointing and add mapped memory strategy

```
TBG_openPMD="--openPMD.period 100 \\
             --openPMD.file simData \\
             --openPMD.ext bp \\
             --openPMD.dataPreparationStrategy mappedMemory"
```

compiling PIConGPU

```bash
cd -
getDevice
pic-build -f 2> err.txt
```

use today's reservation `ENCCS_day3`:
edit `etc/picongpu/lumi-eurohpc/standard-g.tpl`:

        - [line 25] remove line and add the two following 2 lines:

```
#SBATCH --reservation=ENCCS_day3
#SBATCH --exclusive
```

submit job to cluster:
```bash
tbg -s -t -c etc/picongpu/4.cfg $SCRATCH/$(whoami)/01_LWFA
```

FYI: to check your jobs in SLURM:
```bash
squeue -u $USER
scontrol show jobid <Your job’s id>
```

## ipython data analysis

```bash
cp /projappl/project_465001310/workshop_software/notebooks/LWFA_analysis.ipynb $HOME/
```

