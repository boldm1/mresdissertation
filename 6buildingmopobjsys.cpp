
//only considers two buildings for which operations are fixed by PP14

#include <vector>
#include <fstream>
#include <algorithm>
#include <ilcplex/ilocplex.h>

ILOSTLBEGIN	

class Building {
	
	public:
	
	int PP14_s; 		//operations start time
	int PP14_e; 		//operations end time
	int POCO; 		//POCO length
	vector<int> p; 		//task processing times. 3 tasks: ID1, ID2, FD
	double SM;		//monthly S&M costs
	double CM; 		//monthly C&M costs
	vector<int> b;		//monthly task waste package production
	vector<int> a; 		//monthly task costs
	vector<int> H;		//monthly task hazard measures
};

//needed because comparing doubles is subject to rounding errors
bool double_equals(double a, double b, double epsilon = 0.0001){
	return abs(a - b) < epsilon;
}

void effsolntreesearch(IloModel model, double x1, double y1, double x2, double y2, IloNumVar z, IloNumVar C, IloNumVar haz, IloArray<IloNumVarArray> s, int numbuildings){

	int i;

	cout << x1 << "," <<  y1 << "," << x2 << "," << y2 << endl;

	IloEnv env = model.getEnv();

	double lambda = -(x2-x1)/((y2-y1)-(x2-x1));
	IloObjective obj = IloMinimize(env, lambda*haz + (1-lambda)*C);
	model.add(obj);
	IloRange xnewconst(x2 <= C <= x1);
	model.add(xnewconst);
	IloRange ynewconst(y1 <= haz <= y2);
	model.add(ynewconst);
	IloCplex cplex(model);
	cplex.setOut(env.getNullStream());
	cplex.solve();
	
	double xnew = cplex.getValue(C);
	double ynew = cplex.getValue(haz);

	cout << xnew << "," << ynew << endl;

	if((double_equals(xnew, x1) && double_equals(ynew, y1)) || (double_equals(xnew, x2) && double_equals(ynew, y2))){
		cout << "I've returned" << endl;
		model.remove(obj);
		model.remove(xnewconst);
		model.remove(ynewconst);
		return;
	}
	else{
//		ofstream solfile1;
//		solfile1.open ("4btsolfile1.csv", ios::ate | ios::app);
//		solfile1 << "efficient point" << "," << "lamdba = " << lambda << "," << cplex.getStatus() << "," << cplex.getObjValue() << "," << cplex.getValue(z) << "," << cplex.getValue(C) << "," << cplex.getValue(haz) << endl;
//		solfile1.close();

		ofstream solfile1;
		solfile1.open ("4btsolfile1.csv", ios::ate | ios::app);
		solfile1 << cplex.getValue(C) << "," << cplex.getValue(haz) << "," << lambda << "," << cplex.getObjValue() << endl;
		solfile1.close();

		IloNumArray bs(env);
		ofstream solfile2;
		solfile2.open ("4btsolfile2.csv", ios::ate | ios::app);
		for(i=0; i<numbuildings; i++){
			cplex.getValues(bs,s[i]);
			solfile2 << bs << endl; 
		}
		solfile2 << "//" << endl;
		solfile2.close();
		

		model.remove(obj);
		model.remove(xnewconst);
		model.remove(ynewconst);

		cout << "I'm entering the first recursion" << endl;

		effsolntreesearch(model, x1, y1, xnew, ynew, z, C, haz, s, numbuildings); //search to the right
	
		cout << "I'm entering the second recursion" << endl;

		effsolntreesearch(model, xnew, ynew, x2, y2, z, C, haz, s, numbuildings); //search to the left
	
		return;
	}
	
	env.end();

}


int main(){

	IloEnv env;

	try{
	
	int numbuildings = 6;
	int numphases = 3;
	
	//Max yearly package production
	vector<int> W = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30};

	//Yearly budget
	vector<double> B = {160, 160, 160, 160, 160, 160, 160, 160, 160, 160};

	//building 1
	Building building1;
	
	building1.PP14_s = 0;		
	building1.PP14_e = 3;		
	building1.POCO = 3;
	//p
	building1.p.push_back(3);	//ID1
	building1.p.push_back(3);	//ID2
	building1.p.push_back(3);	//FD
	building1.SM = 5;
	building1.CM = 1;
	//b
	building1.b.push_back(2);	//ID1
	building1.b.push_back(2);	//ID2
	building1.b.push_back(2);	//FD
	//a
	building1.a.push_back(2);	//ID1
	building1.a.push_back(2);	//ID2
	building1.a.push_back(2);	//FD
	//H
	building1.H.push_back(14);	//ID1
	building1.H.push_back(14);	//ID2
	building1.H.push_back(14);	//FD

	//building 2
	Building building2;
	
	building2.PP14_s = 0;
	building2.PP14_e = 3;
	building2.POCO = 3;
	//p
	building2.p.push_back(3);
	building2.p.push_back(3);
	building2.p.push_back(3);
	building2.SM = 2;
	building2.CM = 3;
	//b
	building2.b.push_back(2);	//ID1
	building2.b.push_back(2);	//ID2
	building2.b.push_back(2);	//FD
	//a
	building2.a.push_back(2);	//ID1
	building2.a.push_back(2);	//ID2
	building2.a.push_back(2);	//FD
	//H
	building2.H.push_back(3);	//ID1
	building2.H.push_back(3);	//ID2
	building2.H.push_back(3);	//FD

	//building 3
	Building building3;
	
	building3.PP14_s = 0;
	building3.PP14_e = 3;
	building3.POCO = 3;
	//p
	building3.p.push_back(3);
	building3.p.push_back(3);
	building3.p.push_back(3);
	building3.SM = 3;
	building3.CM = 2;
	//b
	building3.b.push_back(2);	//ID1
	building3.b.push_back(2);	//ID2
	building3.b.push_back(2);	//FD
	//b
	building3.a.push_back(2);	//ID1
	building3.a.push_back(2);	//ID2
	building3.a.push_back(2);	//FD
	//H
	building3.H.push_back(2);	//ID1
	building3.H.push_back(2);	//ID2
	building3.H.push_back(2);	//FD

	//building 4
	Building building4;
	
	building4.PP14_s = 0;
	building4.PP14_e = 3;
	building4.POCO = 3;
	//p
	building4.p.push_back(3);
	building4.p.push_back(3);
	building4.p.push_back(3);
	building4.SM = 4;
	building4.CM = 1;
	//b
	building4.b.push_back(2);	//ID1
	building4.b.push_back(2);	//ID2
	building4.b.push_back(2);	//FD
	//a
	building4.a.push_back(2);	//ID1
	building4.a.push_back(2);	//ID2
	building4.a.push_back(2);	//FD
	//H
	building4.H.push_back(1);	//ID1
	building4.H.push_back(1);	//ID2
	building4.H.push_back(1);	//FD

	//building 5
	Building building5;
	
	building5.PP14_s = 0;		
	building5.PP14_e = 3;		
	building5.POCO = 3;
	//p
	building5.p.push_back(3);	//ID1
	building5.p.push_back(3);	//ID2
	building5.p.push_back(3);	//FD
	building5.SM = 1;
	building5.CM = 1;
	//b
	building5.b.push_back(2);	//ID1
	building5.b.push_back(2);	//ID2
	building5.b.push_back(2);	//FD
	//a
	building5.a.push_back(2);	//ID1
	building5.a.push_back(2);	//ID2
	building5.a.push_back(2);	//FD
	//H
	building5.H.push_back(13);	//ID1
	building5.H.push_back(13);	//ID2
	building5.H.push_back(13);	//FD
	
	//building 6
	Building building6;
	
	building6.PP14_s = 0;		
	building6.PP14_e = 3;		
	building6.POCO = 3;
	//p
	building6.p.push_back(3);	//ID1
	building6.p.push_back(3);	//ID2
	building6.p.push_back(3);	//FD
	building6.SM = 1;
	building6.CM = 1;
	//b
	building6.b.push_back(2);	//ID1
	building6.b.push_back(2);	//ID2
	building6.b.push_back(2);	//FD
	//a
	building6.a.push_back(2);	//ID1
	building6.a.push_back(2);	//ID2
	building6.a.push_back(2);	//FD
	//H
	building6.H.push_back(12);	//ID1
	building6.H.push_back(12);	//ID2
	building6.H.push_back(12);	//FD

	vector<Building> buildings;
	buildings.push_back(building1);
	buildings.push_back(building2);
	buildings.push_back(building3);
	buildings.push_back(building4);
	buildings.push_back(building5);
	buildings.push_back(building6);

	IloModel model(env);

	IloInt T = 10*12;
	int i=0, j=0, k=0, l=0;

	typedef IloArray<IloNumVarArray> IntVarMat; 
	typedef IloArray<IntVarMat> IntVar3Mat;	

	typedef IloArray<IloIntArray> IntMat;

	//Variable definitions
	//////////////////////////////////////////////////////

	//binary activity variables x
	IntVar3Mat x(env, numbuildings);
	for(i=0; i<numbuildings; i++){
		x[i] = IntVarMat(env, numphases);
		for(j=0; j<numphases; j++){
			x[i][j] = IloNumVarArray(env, T, 0, 1);
			for(k=0; k<numphases; k++){
				x[i][j][k] = IloNumVar(env, 0, 1, ILOINT);
			}
		}
	}

	//binary start-up variables su
	IntVar3Mat su(env, numbuildings);
	for(i=0; i<numbuildings; i++) {
		su[i] = IntVarMat(env, numphases);	
		for(j=0; j<numphases; j++){
			su[i][j] = IloNumVarArray(env, T, 0, 1);
			for(k=0; k<T; k++){
				su[i][j][k] = IloNumVar(env, 0, 1, ILOINT);
			}
		}
	}

	//binary shut-down variables sd
	IntVar3Mat sd(env, numbuildings);
	for(i=0; i<numbuildings; i++) {
		sd[i] = IntVarMat(env, numphases);	
		for(j=0; j<numphases; j++){
			sd[i][j] = IloNumVarArray(env, T, 0, 1);
			for(k=0; k<T; k++){
				sd[i][j][k] = IloNumVar(env, 0, 1, ILOINT);
			}
		}
	}

	//start time and end time variables
	IntVarMat s(env, numbuildings);
	for(i=0; i<numbuildings; i++){
		s[i] = IloNumVarArray(env, numphases, 0, T, ILOINT);
	}

	IntVarMat e(env, numbuildings);
	for(i=0; i<numbuildings; i++){
		e[i] = IloNumVarArray(env, numphases, 0, T, ILOINT);
	}

	//operation start and end time
	IloNumVarArray s_o(env, numbuildings, 0, T, ILOINT);
	IloNumVarArray e_o(env, numbuildings, 0, T, ILOINT);

	//POCO start and end time
	IloNumVarArray s_POCO(env, numbuildings, 0, T, ILOINT);
	IloNumVarArray e_POCO(env, numbuildings, 0, T, ILOINT);

	//POCO binary shut-down variables
	IntVarMat sdp(env, numbuildings);
	for(i=0; i<numbuildings; i++){
		sdp[i] = IloNumVarArray(env, T, 0, 1, ILOINT);
	}

	//number of waste packages produced by task in an active month
	//IloNumVarArray b(env, numphases, 0, 200, ILOINT); //200 should be max(W) //moved to data	

	//number of waste packages produced per month
	IntVar3Mat w(env, numbuildings);
	for(i=0; i<numbuildings; i++) {
		w[i] = IntVarMat(env, numphases);	
		for(j=0; j<numphases; j++){
			w[i][j] = IloNumVarArray(env, T, 0, 1);
			for(k=0; k<T; k++){
				w[i][j][k] = IloNumVar(env, 0, *max_element(W.begin(),W.end()), ILOINT);
			}
		}
	}

	//cost per month of a task in an active month
	//IloNumVarArray a(env, numphases, 0, 100, ILOINT); //100 is an arbitrary placeholder //moved to data

	//cost per month
	IntVar3Mat c(env, numbuildings);
	for(i=0; i<numbuildings; i++) {
		c[i] = IntVarMat(env, numphases);	
		for(j=0; j<numphases; j++){
			c[i][j] = IloNumVarArray(env, T, 0, 1);
			for(k=0; k<T; k++){
				c[i][j][k] = IloNumVar(env, 0, *max_element(B.begin(),B.end()), ILOFLOAT);
			}
		}
	}

	//safety & maintenance variables
	IntVarMat sm(env, numbuildings);
	for(i=0; i<numbuildings; i++){
		sm[i] = IloNumVarArray(env, T, 0, 1, ILOINT);
	}

	//care & maintenance variables 
	IntVarMat cm(env, numbuildings);
	for(i=0; i<numbuildings; i++){
		cm[i] = IloNumVarArray(env, T, 0, 1, ILOINT);
	}

	//Constraints
	///////////////////////////////////////////////////////
	
	//main variables

	//constraint 5: relation between activity variables and start-up, shut-down variables. check indexing.
	for(i=0; i<numbuildings; i++){
		for(j=0; j<numphases; j++){
			for(k=0; k<T; k++){
				IloExpr sum_su(env), sum_sd(env);
				for(l=0; l<=k; l++){
					sum_su += su[i][j][l];
				}
				for(l=0; l<k; l++){
					sum_sd += sd[i][j][l];
				}
				model.add(x[i][j][k] == sum_su - sum_sd);
				sum_su.end(), sum_sd.end();
			}
		}
	}

	//constraints 6 & 7: there can be at most one start-up, and one shut-down for each job
	for(i=0; i<numbuildings; i++){
		for(j=0; j<numphases; j++){
			model.add(IloSum(su[i][j]) == 1);
			model.add(IloSum(su[i][j]) == 1);
		}
	}

	//constraints 8 & 9: start and end times
	for(i=0; i<numbuildings; i++){
		for(j=0; j<numphases; j++){
			IloExpr st(env), et(env);
			for(k=0; k<T; k++){
				st += k*su[i][j][k];
				et += k*sd[i][j][k];
			}
			model.add(s[i][j] == st);
			model.add(e[i][j] == et);
			st.end(), et.end();
		}
	}

	//contraint 10: ensuring correct number of non-zero activity variables	
	for(i=0; i<numbuildings; i++){
		for(j=0; j<numphases; j++){
			model.add(s[i][j] + buildings[i].p[j] - 1 == e[i][j]);
		}
	}	

	//constraint 11: operation start and end times fixed by PP14
	for(i=0; i<numbuildings; i++){
		model.add(s_o[i] == buildings[i].PP14_s);
		model.add(e_o[i] == buildings[i].PP14_e);
	}

	//constraint 14: start and end of poco
	for(i=0; i<numbuildings; i++){
		model.add(s_POCO[i] + buildings[i].POCO - 1 == e_POCO[i]);
	}	

	//constraint 15: POCO starts immediately after operations end
	for(i=0; i<numbuildings; i++){
		model.add(s_POCO[i] == e_o[i] + 1);
	}

	//constraint 16: At most one POCO shut-down
	for(i=0; i<numbuildings; i++){
		model.add(IloSum(sdp[i]) == 1);
	}	

	//constraint 17: POCO shut-down time
	for(i=0; i<numbuildings; i++){
		IloExpr POCOet(env);
		for(j=0; j<T; j++){
			POCOet += j*sdp[i][j];
		}	
		model.add(POCOet == e_POCO[i]);
		POCOet.end();
	}

	//precedence constraints

	//constraints 22-25: phase ordering
	for(i=0; i<numbuildings; i++){
		model.add(e_o[i] + 1 <= s_POCO[i]);
		model.add(e_POCO[i] + 1 <= s[i][0]);
		model.add(e[i][0] + 1 <= s[i][1]);
		model.add(e[i][1] + 1 <= s[i][2]);
	}	

	//package constraints
	
	//constraint 27 & 28: number of packages produces by task in a month. //review LB and UB		
	for(i=0; i<numbuildings; i++){
		for(j=0; j<numphases; j++){
			for(k=0; k<T; k++){
				model.add(w[i][j][k] >= *(min_element(buildings[i].b.begin(),buildings[i].b.end()))*x[i][j][k]);
				model.add(w[i][j][k] <= *(max_element(buildings[i].b.begin(),buildings[i].b.end()))*x[i][j][k]); //LB*x[i][j][k] <= w[i][j][k] <= UB*x[i][j][k]
				model.add(buildings[i].b[j] - w[i][j][k] >= *(min_element(buildings[i].b.begin(),buildings[i].b.end()))*(1-x[i][j][k]));
				model.add(buildings[i].b[j] - w[i][j][k] <= *(max_element(buildings[i].b.begin(),buildings[i].b.end()))*(1-x[i][j][k])); //LB*(1-x[i][j][k]) <= b[i][j] - w[i][j][k] <= UB*(1-x[i][j][k])			
			}	
		}
	}	


	//constraint 29: max number of packages for each year
	for(l=0; l<T/12; l++){
		IloExpr yps(env); //yearly package sum
		for(i=0; i<numbuildings; i++){
			for(j=0; j<numphases; j++){
				for(k=l*12; k<l*12+12; k++){
					yps += w[i][j][k];
				}
			}
		}
		model.add(yps <= W[l]);
		yps.end();
	}

	//budget constraints

	//constraint 30 & 31: costs of a task in a month
	for(i=0; i<numbuildings; i++){
		for(j=0; j<numphases; j++){
			for(k=0; k<T; k++){
				model.add(c[i][j][k] >= *(min_element(buildings[i].a.begin(),buildings[i].a.end()))*x[i][j][k]);
				model.add(c[i][j][k] <= *(max_element(buildings[i].a.begin(),buildings[i].a.end()))*x[i][j][k]); //LB*x[i][j][k] <= c[i][j][k] <= UB*x[i][j][k]
				model.add(buildings[i].a[j] - c[i][j][k] >= *(min_element(buildings[i].a.begin(),buildings[i].a.end()))*(1-x[i][j][k]));
				model.add(buildings[i].a[j] - c[i][j][k] <= *(max_element(buildings[i].a.begin(),buildings[i].a.end()))*(1-x[i][j][k])); //LB*(1-x[i][j][k]) <= a[i][j] - c[i][j][k] <= UB*(1-x[i][j][k])			
			}	
		}
	}

	//constraint 32: safety and maintenance
	for(i=0; i<numbuildings; i++){
		for(j=0; j<T; j++){
			IloExpr sum_sdp(env), sum_sdID2(env);
			for(k=0; k<j; k++){
				sum_sdp += sdp[i][k];
				sum_sdID2 += sd[i][1][k];   
			}		
			model.add(sm[i][j] == sum_sdp - sum_sdID2);
			sum_sdp.end(), sum_sdID2.end();
		}
	}

	//constraint 35: care and maintenance
	for(i=0; i<numbuildings; i++){
		for(j=0; j<T; j++){
			IloExpr sum_sdID2(env), sum_suFD(env);
			for(k=0; k<j; k++){
				sum_sdID2 += sd[i][1][k];
			}	
			for(k=0; k<=j; k++){
				sum_suFD += su[i][2][k];   
			}
			model.add(cm[i][j] == sum_sdID2 - sum_suFD);
			sum_sdID2.end(), sum_suFD.end();
		}
	}

      	//constraint 38: yearly budget constraint
	for(l=0; l<T/12; l++){
		IloExpr yc(env); //yearly cost
		IloExpr ysmcm(env); //yearly s&m and c&m
		for(i=0; i<numbuildings; i++){
			for(k=l*12; k<l*12+12; k++){
				ysmcm += buildings[i].SM*sm[i][k] + buildings[i].CM*cm[i][k];
				for(j=0; j<numphases; j++){
					yc += c[i][j][k];
				}
			}
		}
		model.add(yc + ysmcm <= B[l]);
		yc.end(), ysmcm.end();
	}


	//objective function
	//////////////////////////////////////////////////////////////////////////////////	

	//makespan
	/////////////////////////////////////////////////////////////

	//makespan variable
	IloNumVar z(env, 0, T, ILOINT);	

	//makespan constraint 
	for(i=0; i<numbuildings; i++){
		model.add(z >= e[i][2]);
	}

	//makespan objective
	//model.add(IloMinimize(env, z));


	//s&m and c&m costs
	/////////////////////////////////////////////////////////////

	//s&m and c&m costs variable
	IloNumVar C(env, 0, accumulate(B.begin(),B.end(),0.0), ILOFLOAT, "C");

        //s&m and c&m costs constraint
	IloExpr costsum(env);
	for(i=0; i<numbuildings; i++){
		for(j=0; j<T; j++){
			costsum += (buildings[i].SM*sm[i][j] + buildings[i].CM*cm[i][j]);
		}
	
	}
	model.add(C >= costsum);
	costsum.end();

	//s&m and c&m cos objective
	//model.add(IloMinimize(env, C));

	//hazard
	/////////////////////////////////////////////////////////////

	//hazard variable
	IloNumVar haz(env, 0, 15*T, ILOINT, "haz");

	//hazard constraint
	IloExpr hazsum(env);
	for(i=0; i<numbuildings; i++){
		hazsum += (e[i][0] - e_POCO[i])*buildings[i].H[0] + (e[i][1] - e[i][0])*buildings[i].H[1] + (e[i][2] - e[i][1])*buildings[i].H[2];
	}
	model.add(haz >= hazsum);
	hazsum.end();

	//hazard objective
	//model.add(IloMinimize(env, haz));

	///////////////////////////////////////////////////////////

	//Creating files to store solutions
	
	ofstream solfile1;
	solfile1.open ("4btsolfile1.csv");
	solfile1 << "# Cost" << "," << "Hazard" << "," << "Lambda" << "," << "Obj. value" << endl;
	solfile1.close();

	//systematic weighted sum method

	//extreme point 1. Minimise haz.
	IloObjective obj1 = IloMinimize(env, haz);
	model.add(obj1);

	IloCplex cplex(model);
	cplex.setOut(env.getNullStream());
	cplex.solve();
	
	//lexographic re-optimisation
	IloRange consthaz(haz == cplex.getValue(haz));
	model.add(consthaz); //haz should not worsen
	model.remove(obj1);
	IloObjective reoptobj1 = IloMinimize(env, C);
	model.add(reoptobj1);
	cplex.solve();

	//storing solution
	IloIntArray ends(env); 
	for(i=0; i<numbuildings; i++){
		ends.add(cplex.getValue(e[i][2]));
	}

//	solfile1.open ("4btsolfile1.csv", ios::ate | ios::app);
//	solfile1 << "extreme point 1" << "," << cplex.getStatus() << "," << cplex.getObjValue() << "," << IloMax(ends) << "," << cplex.getValue(C) << "," << cplex.getValue(haz) << endl;
	ends.clear();
//	solfile1.close();
	
	solfile1.open ("4btsolfile1.csv", ios::ate | ios::app);
	solfile1 << cplex.getValue(C) << "," << cplex.getValue(haz) << "," << 1 << "," << cplex.getObjValue() << endl;
	solfile1.close();

	IloNumArray bs(env);
	ofstream solfile2;
	solfile2.open ("4btsolfile2.csv");
	for(i=0; i<numbuildings; i++){
		cplex.getValues(bs,s[i]);
		solfile2 << bs << endl; 
	}
	solfile2 << "//" << endl;
	solfile2.close();

	double x1 = cplex.getValue(C);
	double y1 = cplex.getValue(haz);
	
	model.remove(consthaz);
	model.remove(reoptobj1);
	
	//extreme point 2. Minimise C.
	IloObjective obj2 = IloMinimize(env, C);
	model.add(obj2);
	cplex.solve();

	//lexographic re-optimisation
	IloRange constC(C == cplex.getValue(C));
	model.add(constC); //C should not worsen
	model.remove(obj2);
	IloObjective reoptobj2 = IloMinimize(env, haz);
	model.add(reoptobj2);
	cplex.solve();

	//storing solution
	for(i=0; i<numbuildings; i++){
		ends.add(cplex.getValue(e[i][2]));
	}

//	solfile1.open ("4btsolfile1.csv", ios::ate | ios::app);
//	solfile1 << "extreme point 2" << "," << cplex.getStatus() << "," << cplex.getObjValue() << "," << IloMax(ends) << "," << cplex.getValue(C) << "," << cplex.getValue(haz) << endl;
	ends.clear();
//	solfile1.close();

	solfile1.open ("4btsolfile1.csv", ios::ate | ios::app);
	solfile1 << cplex.getValue(C) << "," << cplex.getValue(haz) << "," << 0 << "," << cplex.getObjValue() << endl;
	solfile1.close();


	solfile2.open ("4btsolfile2.csv", ios::ate | ios::app);
	for(i=0; i<numbuildings; i++){
		cplex.getValues(bs,s[i]);
		solfile2 << bs << endl; 
	}
	solfile2 << "//" << endl;
	solfile2.close();

	double x2 = cplex.getValue(C);
	double y2 = cplex.getValue(haz);
	
	model.remove(constC);
	model.remove(reoptobj2);
	
	if(x1 != x2 || y1 != y2){
		effsolntreesearch(model, x1, y1, x2, y2, z, C, haz, s, numbuildings);
	}

	///////////////////////////////////////////////////////////

	}

	catch(IloException& e){
		cerr << "Concert Exception: " << e << endl;
	}

	catch(...){
		cerr << "Unknown Exception" << endl;
	}

        env.end();
	return 0;
}



