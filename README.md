# docker-rng-repro

Each rng folder have in common a Makefile, Dockerfile and a run.c
The Dockerfile already build the run binary using make all.
For all run case scenarios the run binary can be executed without
any parameter. For the sake of the reproducibility experiment we 
make the choice of hardly setting the parameters under which each
use case is run.

There is no rule to where the results for each run has to be put. I will
assume attaching a volume to each with an associated name and dumping the
run result into that volume will suffice.