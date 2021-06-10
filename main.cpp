
#include "Rinex.h"
#include "Ephemeris.h"
#include "Receiver.h"
using namespace std;
int numiterator = 0;
int debug_stop = 170;

//int main(int argc, char ** argv) {
//	int n = 18;
//	int m = 2;
//	Eigen::MatrixXd Q = Eigen::MatrixXd::Zero(n,n);
//	Eigen::VectorXd a = Eigen::VectorXd::Zero(n);
//	Eigen::MatrixXd b  = Eigen::MatrixXd::Zero(n, m);
//	filetovector(a,n,"a");
//	filetomatrix(Q,n,n,"Q");
//	//matrixtofile(Q,"Qtest",'F');
//	int info = 0;
//	vector<double> s(2,0);
//	info = lambda(n, m, a, Q, b, s);
//	int aaaa = 1;
//
//}

int main(int argc,char ** argv){
	string navfile = "D:\\SHMILY\\340_RTK\\RTK_CPP\\data\\base.nav";
	string Bobsfile = "D:\\SHMILY\\340_RTK\\RTK_CPP\\data\\base.obs";
	//string Robsfile = "D:\\SHMILY\\340_RTK\\RTK_CPP\\data\\base1.obs";
	string Robsfile = "D:\\SHMILY\\340_RTK\\RTK_CPP\\data\\rover.obs";
	NAV_RINEX nav(navfile);
	nav.readfile();
	//All_Sats_EPH allsateph(nav); 
	//allsateph.satephsolve(nav);
	OBS_RINEX obs(Bobsfile, Robsfile);
	receiver rcv(nav,1,1,"1");//dynamics_on  arfilter
	gtime_t headtime = obs.readheader();
	string buffB,buffR;
	/*ifstream obsfileB(Bobsfile);
	ifstream obsfileR(Robsfile);
	obs.findfirstepoch(obsfileB, obsfileR, buffB, buffR, headtime);
	do {
		obs.readepoch(obsfileB, obsfileR, buffB, buffR, headtime);
		obs.getdoppler();
		
	} while (getline(obsfileB, buffB) && getline(obsfileR, buffR));
	obs.printfdoppler();*/
	/*Ô­Ê¼³ÌÐò*/
	ifstream obsfileB(Bobsfile);
	ifstream obsfileR(Robsfile);
	//double nba;
	//cout << sizeof(&nba) << endl;
	obs.findfirstepoch(obsfileB, obsfileR,buffB,buffR,headtime);
	do {	
		obs.readepoch(obsfileB, obsfileR,buffB,buffR, headtime);
		rcv.updateobs(obs);
		rcv.updateSatinfo();
		/*if (obs.GPStime_R.time == 1614221810) {
			int a = 1;
		}*/
		numiterator++;
		if (rcv.LS_pos('N') == 1) {
			rcv.LS_vel();
			/*if (numiterator == debug_stop) {
				int aaaa = 1;
			}*/
			rcv.RTK_pos("PL");
			//rcv.displaysatpos(3);
		}	
		/*if (numiterator == debug_stop) {
			int aaaa = 1;
		}*/
		
		rcv.soltofile("enu");
		int aaa = 1;
			
			
			
		
		
		
	} while (getline(obsfileB, buffB) && getline(obsfileR, buffR));
	//obsfileB.close();
	//obsfileR.close();



	int a = 1;
}