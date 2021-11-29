# the build target executable:
# Compiles all files and creates executables
all:
	gcc -o central central.c
	gcc -o clientA clientA.c
	gcc -o clientB clientB.c
	gcc -o serverT serverT.c
	gcc -o serverS serverS.c
	gcc -o serverP serverP.c
# remove all executables
clean: 
	-rm central
	-rm clientA
	-rm clientB
	-rm serverT
	-rm serverS
	-rm serverP