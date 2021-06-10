#ifndef RINEX_H_
#define RINEX_H_



#include <stdio.h>
#include <fstream>
//#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <math.h>
#include <map>
#include "Time_utility.h"

typedef std::pair<int, vector<double>> SDpair;
typedef std::map <int, vector<double> > SDmp;//���ĳ����ID �� ��Ӧ�Ķ�����



class NAV_RINEX {
public:
	NAV_RINEX(string file) { filename = file; }
	~NAV_RINEX() {};
	void NAV_RINEX_CLEAR();
	void display();
	void readfile();
	void solve_epoch(string buff, ifstream &navfile, vector<int> & GNSS_ID, vector<double> & GNSS_NAV_DATA, vector<int> & GNSS_TOC);
	string filename;
	int LEAP_SECONDS;
	vector<string > CORR_TYPE;
	vector<double > ION_CORR;
	vector<double > TIME_CORR; 
	vector<int > GPS_ID;
	vector<int > BDS_ID;
	vector<int > GPS_TOC;
	vector<int > BDS_TOC;
	vector<double> GPS_NAV_DATA;
	vector<double> BDS_NAV_DATA;
	
};

class OBS_RINEX {
public:
	OBS_RINEX(string fileB, string fileR) { filenameBase = fileB; filenameRover = fileR; }
	gtime_t readheader();
	gtime_t obtainheadparas(string file,char mode);
	void findfirstepoch(ifstream &fileB, ifstream &fileR,string &buffB,string &buffR,gtime_t headtime);
	void getepochdata(int,ifstream &fileB,char mode);
	void getepochdatabody(int Frequency_num,string buff, vector<int>&, vector<int>&, vector<double>&);
	void readepoch(ifstream &fileB, ifstream &fileR,string buffB,string buffR, gtime_t headtime);
	/*���ù۲����йصĺ���*/
	void getdoppler();
	void printfdoppler();
	string filenameBase,filenameRover;
	int GPS_Fnum_B;
	int BDS_Fnum_B;
	int GPS_Fnum_R;
	int BDS_Fnum_R;
	vector<double> GPS_fre_B;
	vector<double> BDS_fre_B;
	vector<double> GPS_fre_R;
	vector<double> BDS_fre_R;
	int epochflag_B;
	int epochflag_R;
	int cur_sat_num_B;
	int cur_sat_num_R;
	vector<int > GPS_satID_B;
	vector<int > BDS_satID_B;
	vector<int > GPS_flags_B;
	vector<int > BDS_flags_B;
	vector<double > GPS_measurements_B;
	vector<double > BDS_measurements_B;
	gtime_t GPStime_B;
	vector<int > GPS_satID_R;
	vector<int > BDS_satID_R;
	vector<int > GPS_flags_R;
	vector<int > BDS_flags_R;
	vector<double > GPS_measurements_R;
	vector<double > BDS_measurements_R;
	gtime_t GPStime_R;
	/*����ȡ�۲����йصĽṹ*/
	SDmp sat_doppler;
};

#endif // !RINEX_H_
#pragma once
