#include<iostream>
#include<fstream>
#include<string>

#define KY_start 0
#define OH_start 120
#define PA_start 120+88
#define VA_start 120+88+67
#define WV_start 120+88+67+131
#define INF (1<<20)

//#define OUTPUT_DRUG_COUNTS
#define OUTPUT_REAL_COUNT
#define OUTPUT_CA_COUNT

using namespace std;

string s[500];  						// store the name of counties
int county_count[10][500];	// store the number of drug reports of different counties in 2010-2017
int result[30][500];		// store the results simulated by CA.
int State_count[10][5];		// strore the number of drug reports of different state in 2010-2017
int CA_State_count[40][5];
int name_counter;			// store the number of counties
double factor[10][500][20];		//factor[year][county][VA]
double coef[20];
double coef2[2] = { 0.999, 0.186 };
double error[10];
double coef_constant;
const string State_name[] = {"KY", "OH", "PA", "VA", "WV"  };
struct{ 
	int rel_name;		// the number of adjacent counties
	int adj[20];		
} relations[500] ;

/* following are file operator  */
ifstream names;
ifstream count_year;
ifstream county_relation;
ifstream factor_file,coef_file;
ofstream ca_count;
ofstream real_count;

/* following are functions declaration*/
void get_name(void); 
void get_count(void);
void get_relation(void);
void get_data(void);
void get_max_min(void);
void CA(int year);
int update(int year, int county_id);
void output(void);
double cal_error(int year);
int back(int year, int county_id);

void get_max_min(void);    int count_max,count_min;
void ite(int year);
void init(void);
int main() {
	names.open("COUNTY_NAMES.txt");
	count_year.open("DRUG_COUNTS.txt");
	county_relation.open("NEW_COUNTY_RELATION.txt");
	ca_count.open("CA_RESULTS.txt");
	real_count.open("REAL_RESULTS.txt");
	factor_file.open("DATA.txt");
	coef_file.open("COEF.txt");



	get_name();
	get_count();
	get_relation();
//	get_data();
//	get_max_min();

	init();

	ite(2016);
//	CA(2015);
	//cal_error(2011);
//	output();

	names.close();
	count_year.close();
	county_relation.close();
	ca_count.close();
	real_count.close();

	factor_file.close();
	coef_file.close();

}

void ite(int year) {

	double ult_error = double(INF);
	double ult_cof1=0.0;
	double ult_cof2=0.0;
	double eps=0.001;

	double cof1=0.0;
	double cof2=0.0;
	double tmp;
	while(cof1<1.0 ) {
		while(cof2<1.0) {
			coef2[0]=cof1;
			coef2[1]=cof2;
			CA(year);
			tmp=cal_error(year);
			if( tmp < ult_error ) {
				ult_error=tmp;
				//`cout<<ult_error<<endl;
				ult_cof1=cof1;
				ult_cof2=cof2;
			}
			cof2=cof2+eps;
		}
		//cout<<cof1<<endl;
		cof1=cof1+eps;
		cof2=eps;
	}

	cout<<ult_cof1<<endl<<ult_cof2<<endl<<ult_error<<endl;


}




void get_name(void) {
	name_counter=0;
	while(names>>s[name_counter]) {
		name_counter++;
	}
	//cout<<"In get_name(), get "<<name_counter<<" county names"<<endl;
	return ;
}

void get_count(void) {
	memset(county_count,0,sizeof(county_count));
	int Inyear,Incounty_count;
	string InState, Incounty;
	int herestart;
	while( count_year >> Inyear >> InState >> Incounty >> Incounty_count  ) {
		
	//	cout<<Inyear<<InState<<Incounty<<Incounty_count<<endl;
		switch(InState[0]) {
			case 'K' : herestart=KY_start; break;
			case 'O' : herestart=OH_start; break;
			case 'P' : herestart=PA_start; break;
			case 'V' : herestart=VA_start; break;
			case 'W' : herestart=WV_start; break;				default : cout<<"Error\n"<<endl; break;	
		}
		for(int i=herestart; i<name_counter;i++ ) {
			if(s[i]==Incounty) {
				county_count[Inyear%2010][i]+= Incounty_count;
				break;
			
			}
			if(i==name_counter-1)
				cout<<"In get_count, Error\n";
		
		}
	}
	memset(State_count,0,sizeof(State_count));
	for(int i=2010;i<=2017;i++) {
		for(int j=KY_start;j<OH_start;j++) 
			State_count[i%2010][0]+=county_count[i%2010][j];
		for(int j=OH_start;j<PA_start;j++) 
			State_count[i%2010][1]+=county_count[i%2010][j];
		for(int j=PA_start;j<VA_start;j++) 
			State_count[i%2010][2]+=county_count[i%2010][j];
		for(int j=VA_start;j<WV_start;j++) 
			State_count[i%2010][3]+=county_count[i%2010][j];
		for(int j=WV_start;j<name_counter;j++) 
			State_count[i%2010][4]+=county_count[i%2010][j];
	}
}

void get_relation() {
	string In_county;
	int rel;
	for(int i=0;i<name_counter;i++) {
		relations[i].rel_name=0;
		county_relation>>In_county;
		while(county_relation>>rel) {
			if(rel==-1)
				break;
			relations[i].adj[relations[i].rel_name]=rel-1;
			relations[i].rel_name++;
		}
	}
}


void get_data() {
	int In_year;
	for(int i=2010;i<=2016;i++) {
		for(int j=0;j<name_counter;j++) {
			for(int k=0;k<7;k++) {
				factor_file>>In_year>>factor[i%2010][j][k];
			}
		}
	}

	for(int i=0;i<7;i++) {
		coef_file>>coef[i];
	}
	coef_file>>coef_constant;
}


void init(void) {
	for(int i=0;i<name_counter;i++)
		result[2010%2000][i] = county_count[2010%2010][i];

}

void CA(int year) {
	
	for(int i=2010;i<2020;i++)  {
		for(int j=0;j<name_counter;j++) {
			result[ (i+1)%2000][j] =  update(i,j);
		}

	}
	for(int i=2010; i>2000;i--) {
		for(int j=0;j<name_counter;j++) {
			result[(i-1)%2000  ][j] = back(i,j);	
		
		}
	}
}

int update(int year,  int county_id) {	
	int val=0;
	double tmp1=0;
	double tmp2=0;
	tmp1 += coef2[0] * result[year%2000][county_id];
	for(int i=0;i<relations[county_id].rel_name;i++) {
		tmp2 +=  coef2[1] *  double(result[year%2000][relations[county_id].adj[i]]);
	}
	tmp2=tmp2 / double (relations[county_id].rel_name);
	val= (tmp1+tmp2*0.8);
	return val;
}
	
int back(int year, int county_id) {
	int val=0;
	double tmp1=0;
	double tmp2=0;
	tmp1=result[year%2000][county_id];
	for(int i=0;i<relations[county_id].rel_name;i++) {
		tmp2+= coef2[1] * double(result[year%2000][relations[county_id].adj[i]]);
	}
	tmp2 = tmp2 / double(relations[county_id].rel_name);
	val = (tmp1-tmp2)/ double(coef2[0]);
	if(val<0)
		val=0;
	return val;


}

void output() {
	
#ifdef OUTPUT_DRUG_COUNTS
	/*
	for(int i=2010;i<=2017;i++) {
		cout<<i <<"\t"<<"KY\t"<<State_count[i%2010][0]<<endl;
		cout<<i <<"\t"<<"OH\t"<<State_count[i%2010][1]<<endl;
		cout<<i <<"\t"<<"PA\t"<<State_count[i%2010][2]<<endl;
		cout<<i <<"\t"<<"VA\t"<<State_count[i%2010][3]<<endl;
		cout<<i <<"\t"<<"WV\t"<<State_count[i%2010][4]<<endl;

	}
	*/
	for(int i=2010;i<=2017;i++) {
		for(int j=0;j<name_counter;j++) {
			//cout<< i<< "\t" ;
			/*	
			if(j<OH_start) 
				cout<<"KY"<<"\t";
			else if(j<PA_start)
				cout<<"OH"<<"\t";
			else if(j<VA_start)
				cout<<"PA"<<"\t";
			else if(j<WV_start)
				cout<<"VA"<<"\t";
			else
				cout<<"WV"<<"\t";
			*/
			cout<</* s[j]<< "\t"<< */county_count[i%2010][j];
			
			cout<<endl;
		} 
	}
#endif 

#ifdef OUTPUT_REAL_COUNT
	for(int i=2010;i<=2017;i++) {
		cout<<i<<"\t"<<"KY\t"<<State_count[i%2010][0]<<endl;
		cout<<i<<"\t"<<"OH\t"<<State_count[i%2010][1]<<endl;
		cout<<i<<"\t"<<"PA\t"<<State_count[i%2010][2]<<endl;
		cout<<i<<"\t"<<"VA\t"<<State_count[i%2010][3]<<endl;
		cout<<i<<"\t"<<"WV\t"<<State_count[i%2010][4]<<endl;

	}

	for(int i=2011;i<=2017;i++) {
		for(int j=0;j<name_counter;j++) {
			real_count << i <<"\t"<<s[j]<<"\t\t"<<county_count[i%2010][j]<<endl;		
		}
	} 

#endif


#ifdef OUTPUT_CA_COUNT
	for(int i=2000;i<=2020;i++) {
		for(int j=KY_start;j<OH_start;j++) 
			CA_State_count[i%2000][0]+=result[i%2000][j];
		for(int j=OH_start;j<PA_start;j++) 
			CA_State_count[i%2000][1]+=result[i%2000][j];
		for(int j=PA_start;j<VA_start;j++) 
			CA_State_count[i%2000][2]+=result[i%2000][j];
		for(int j=VA_start;j<WV_start;j++) 
			CA_State_count[i%2000][3]+=result[i%2000][j];
		for(int j=WV_start;j<name_counter;j++) 
			CA_State_count[i%2000][4]+=result[i%2000][j];
	}
	for(int i=2000;i<=2020;i++) {
//		cout<<i<<"\t"<<"KY\t"<<CA_State_count[i%2000][0]<<endl;
//		cout<<i<<"\t"<<"OH\t"<<CA_State_count[i%2000][1]<<endl;
//		cout<<i<<"\t"<<"PA\t"<<CA_State_count[i%2000][2]<<endl;
//		cout<<i<<"\t"<<"VA\t"<<CA_State_count[i%2000][3]<<endl;
//		cout<<i<<"\t"<<"WV\t"<<CA_State_count[i%2000][4]<<endl;

	}
	for(int i=2000;i<=2020;i++) {
		for(int j=0;j<name_counter;j++) {
			ca_count<<i<<"\t";
			if(j<OH_start) 
				ca_count<<"KY"<<"\t";
			else if(j<PA_start)
				ca_count<<"OH"<<"\t";
			else if(j<VA_start)
				ca_count<<"PA"<<"\t";
			else if(j<WV_start)
				ca_count<<"VA"<<"\t";
			else
				ca_count<<"WV"<<"\t";

			ca_count <<s[j]<<"\t\t"<<result[i%2000][j]<<endl;		
		}
	} 

#endif


}


void get_max_min(void) {
	count_max=-INF;
	count_min=INF;
//	State_max=-INF;
//	State_min=INF;
	for(int i=2010;i<=2016;i++) {
		for(int j=0;j<name_counter;j++) {
			count_max=(county_count[i%2010][j]>count_max)?county_count[i%2010][j]:count_max;
			count_min=(county_count[i%2010][j]<count_min)?county_count[i%2010][j]:count_min;
		}
	}
	/*
	for(int i=2010;i<=2016;i++) {
		for(int j=0;j<5;j++) {
			State_max=(State_count[i%2010][j]>State_max)?State_count[i%2010][j]:State_max;
			State_min=(State_count[i%2010][j]<State_min)?State_count[i%2010][j]:State_min;
		}
	}
	*/


	cout<<count_max<<endl<<count_min<<endl;
}



double cal_error(int year) {
	double val;
	double tmp;
	double tmp1;
	double tmp2;
	for(int year=2011;year<=2017;year++)
	for(int i=0;i<name_counter;i++) {
		/*
		if(county_count[year%2010][i]==0)
			continue;
		
		tmp1 = (double)(abs(result[year%2010][i] - county_count[year%2010][i])) ;
		tmp2 =	( double(county_count[year%2010][i]) );
		tmp=tmp1/tmp2;
		//cout<<tmp1<<"\t"<<tmp2<<"\t"<< tmp<<endl;
		val += (tmp);
		*/
		
		tmp1 = (double)(abs(result[year%2000][i] - county_count[year%2010][i])) ;
 		val += ( (tmp1*tmp1)/name_counter   );
	}
	//cout<<tmp<<endl;
//	cout<<"val="<<val<<endl;
	val = val / 7; 
	return val;


}





