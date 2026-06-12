include ../common/Makefile

CC=mpicc
XC=mpic++

INCLUDE += -I../common/mpi_wrapper/include
FLAG += -fopenmp
