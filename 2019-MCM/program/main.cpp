#include<iostream>
#include<fstream>
#include<string>
using namespace std;
string s[500];
int county_count[10][500];
int result[10][500];
int State_count[10][5];
int name_counter;
string State_name[] = {"KY", "OH", "PA", "VA", "WV"  };
struct {
	int rel_name;
	int adj[20];
} relations[500] ;

const int KY_start=0;
const int OH_start=120;
const int PA_start=120+88;
const int VA_start=120+88+67;
const int WV_start=120+88+67+131;

ifstream names;
ifstream count_year;
ifstream county_relation;
ofstream outfile;

void get_name(void) {
	name_counter=0;
	while(names>>s[name_counter]) {
		name_counter++;
	}
	cout<<"In get_name(), get "<<name_counter<<" county names"<<endl;
	return ;
}

void get_count(void) {
	int Inyear,Incounty_count,InState_count;
	string InState, Incounty;
	int herestart;
	while( count_year >> Inyear >> InState >> Incounty >> Incounty_count >> InState_count ) {
		
		switch(InState[0]) {
			case 'K' : herestart=KY_start; break;
			case 'O' : herestart=OH_start; break;
			case 'P' : herestart=PA_start; break;
			case 'V' : herestart=VA_start; break;
			case 'W' : herestart=WV_start; break;				default : cout<<"Error\n"<<endl; break;	
		
		}
		for(int i=herestart; i<name_counter;i++ ) {
			if(s[i]==Incounty) {
				county_count[Inyear%2010][i]= Incounty_count;
				break;
			
			}
			if(i==name_counter-1)
				cout<<"In get_count, Error\n";
		
		}
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

void output() {
	for(int i=0;i<name_counter;i++)
		results[2010%2010][i]=county_count[2010%2010][i];


	for(int k=2010;k<2017;k++) {
		for(int i=0;i<name_counter;i++) {
			cout<<s[i]<<": ";
			for(int j=0;j<relations[i].rel_name;j++) {
				cout<<s[relations[i].adj[j] ]<<", ";
			}
			cout<<endl;
	
		}
	}
}



void CA() {
	
	for(int i=0;i<name_counter;i++)
		results[2010%2010][i] = county_count[2010%2010][i];

	for(int i=2010;i<2017;i++)  {
		
		for(int j=0;j<name_counter;j++) {
			results[ (i+1)%2010][j] =  update(results[i%2010][j]);
		
		}



		


	}
	
}



int main() {
	names.open("COUNTY_NAMES.txt");
	count_year.open("DRUG_COUNTS.txt");
	county_relation.open("NEW_COUNTY_RELATION.txt");
	outfile.open("CA_RESULTS.txt");


	get_name();
	get_count();
	get_relation();
	CA();
	//output();


	names.close();
	count_year.close();
	county_relation.close();
	outfile.close();
}



