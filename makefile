
all:	
	c++ 6buildingmopobjsys.cpp -o 4btmopobjsys -I/opt/ibm/ILOG/CPLEX_Studio127/cplex/include/ -I/opt/ibm/ILOG/CPLEX_Studio127/concert/include/ -L/opt/ibm/ILOG/CPLEX_Studio127/cplex/lib/x86-64_linux/static_pic/ -lilocplex -lcplex -L/opt/ibm/ILOG/CPLEX_Studio127/concert/lib/x86-64_linux/static_pic/ -lconcert -lpthread -O3 -DIL_STD -std=c++0x


