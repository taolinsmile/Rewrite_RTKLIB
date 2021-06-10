#include "Rinex.h"


void NAV_RINEX::display() {}

void NAV_RINEX::readfile() {
	ifstream navfile(filename);
	if (!navfile.is_open()) {
		cout << "navfile û��" << endl;
		return;
	}
	string buff;
	int header_flag = 1;
	stringstream ss;
	while (getline(navfile, buff)) {
		if (header_flag == 1) {
			if (buff.substr(60, 13) == "END OF HEADER") {
				header_flag = 0;
				continue;
			}
			if (buff.substr(60, 12) == "LEAP SECONDS") {
				ss << buff.substr(0, 6);
				ss >> LEAP_SECONDS;
				ss.clear();
				ss.str("");
				continue;
			}
			if(buff.substr(60, 16) == "IONOSPHERIC CORR") {
				CORR_TYPE.push_back(buff.substr(0, 4));
				int i = 0;
				int j = 5;
				for (i = 0; i < 4; i++, j += 12) {
					double temp;
					string temps = buff.substr(j, 13);
					string::iterator it = find(temps.begin(), temps.end(), 'D');
					if (it != temps.end())
						*it = 'E';
					ss << temps;
					ss >> temp;
					ION_CORR.push_back(temp);
					ss.clear();
					ss.str("");
				}				
			}
			if (buff.substr(60, 16) == "TIME SYSTEM CORR") {
				CORR_TYPE.push_back(buff.substr(0, 4));
				double temp;
				string temps;
				temps = buff.substr(5, 17);
				string::iterator it = find(temps.begin(), temps.end(), 'D');
				if (it != temps.end())
					*it = 'E';
				ss << temps;
				ss >> temp;
				TIME_CORR.push_back(temp);
				ss.clear();
				ss.str("");
				temps = buff.substr(22, 16);
				it = find(temps.begin(), temps.end(), 'D');
				if (it != temps.end())
					*it = 'E';
				ss << temps;
				ss >> temp;
				TIME_CORR.push_back(temp);
				ss.clear();
				ss.str("");
				ss << buff.substr(38, 7);
				ss >> temp;
				TIME_CORR.push_back(temp);
				ss.clear();
				ss.str("");
				ss << buff.substr(45, 5);
				ss >> temp;
				TIME_CORR.push_back(temp);
				ss.clear();
				ss.str("");	
			}
		}
		// ��body
		else {
			if (buff[0] == 'G') {
				solve_epoch(buff, navfile, GPS_ID, GPS_NAV_DATA, GPS_TOC);
			}
			else {
				solve_epoch(buff, navfile, BDS_ID, BDS_NAV_DATA, BDS_TOC);
			}
		}
	}
	navfile.close();
}
void NAV_RINEX::solve_epoch(string buff,ifstream &navfile,vector<int> & GNSS_ID, vector<double> & GNSS_NAV_DATA, vector<int> & GNSS_TOC) {
	int i = 1;
	int temp;
	double tempd;
	stringstream ss;
	ss << buff.substr(1, 2);
	ss >> temp;
	ss.clear();
	ss.str("");
	GNSS_ID.push_back(temp);
	gtime_t toc = string2time(buff.substr(4, 19));
	GNSS_TOC.push_back(toc.time);
	for (int k = 0, j = 23; k < 3; k++, j += 19) {
		ss << buff.substr(j, 19);
		ss >> tempd;
		GNSS_NAV_DATA.push_back(tempd);
		ss.clear();
		ss.str("");
	}
	for (i = 1; i < 8;i++) {
		getline(navfile, buff);
		if (i < 7) {
			for (int k = 0, j = 4; k < 4; k++, j += 19) {
				ss << buff.substr(j, 19);
				ss >> tempd;
				GNSS_NAV_DATA.push_back(tempd);
				ss.clear();
				ss.str("");
			}
		}
		else {
			for (int k = 0, j = 4; k < 2; k++, j += 19) {
				ss << buff.substr(j, 19);
				ss >> tempd;
				GNSS_NAV_DATA.push_back(tempd);
				ss.clear();
				ss.str("");
			}
		}
	}
}
void NAV_RINEX::NAV_RINEX_CLEAR() {
	vector<int>().swap(GPS_ID);
	vector<int>().swap(BDS_ID);
	vector<int>().swap(GPS_TOC);
	vector<int>().swap(BDS_TOC);
	vector<string >().swap(CORR_TYPE);
	vector<double >().swap(ION_CORR);
	vector<double >().swap(TIME_CORR);
	vector<double>().swap(GPS_NAV_DATA);
	vector<double>().swap(BDS_NAV_DATA);
}
gtime_t  OBS_RINEX::readheader() {
	//ifstream obsfileB(filenameBase);
	//ifstream obsfileR(filenameRover);
	gtime_t baseheadtime = obtainheadparas(filenameBase,'B');
	gtime_t roverheadtime = obtainheadparas(filenameRover,'R');
	return timediff(baseheadtime ,roverheadtime)<0 ? roverheadtime : baseheadtime;
}
gtime_t OBS_RINEX::obtainheadparas(string file, char mode) {
	ifstream obsfile(file);
	if (!obsfile.is_open()) {
		cout << "obsfile û��" << endl;
		return gtime_t(0,0.0);
	}
	int GPS_Fnum = 0;
	int BDS_Fnum = 0;
	vector<double > GPS_fre;
	vector<double > BDS_fre;
	string buff;
	stringstream ss;
	int tempi;
	double tempd;
	gtime_t headtime;
	while (getline(obsfile, buff)) {
		if (buff[0] == '>') {
			headtime = string2time(buff.substr(2, 27));
			break;
		}
		if (buff.substr(60, 19) == "SYS / # / OBS TYPES") {
			ss << buff.substr(4, 2);
			ss >> tempi;
			ss.clear();
			ss.str("");
			if (buff[0] == 'G') {
				GPS_Fnum = tempi / 4;
				for (int i = 0; i < GPS_Fnum; i++) {
					if (buff.substr(0 + i * 16 + 7, 3) == "C1C")
						GPS_fre.push_back(1575.42);
					else if (buff.substr(0 + i * 16 + 7, 3) == "C2P" || buff.substr(0 + i * 16 + 7, 3) == "C2W")
						GPS_fre.push_back(1227.60);
					else if (buff.substr(0 + i * 16 + 7, 3) == "C5Q" || buff.substr(0 + i * 16 + 7, 3) == "C5X")
						GPS_fre.push_back(1176.45);
				}
			}
			else if (buff[0] == 'C') {
				BDS_Fnum = tempi / 4;
				for (int i = 0; i < BDS_Fnum; i++) {
					if (buff.substr(0 + i * 16 + 7, 3) == "C2I" || buff.substr(0 + i * 16 + 7, 3) == "C1I")
						BDS_fre.push_back(1561.098);
					else if (buff.substr(0 + i * 16 + 7, 3) == "C7I")
						BDS_fre.push_back(1207.14);
				}
			}
		}
	}
	if (mode == 'B') {
		GPS_Fnum_B = GPS_Fnum;
		BDS_Fnum_B = BDS_Fnum;
		GPS_fre_B = GPS_fre;
		BDS_fre_B = BDS_fre;
	}
	else if (mode == 'R') {
		GPS_Fnum_R = GPS_Fnum;
		BDS_Fnum_R = BDS_Fnum;
		GPS_fre_R = GPS_fre;
		BDS_fre_R = BDS_fre;
	}
	else
		cout << "mode is wrong" << endl;
	
	obsfile.close();
	return headtime;
}

void OBS_RINEX::findfirstepoch(ifstream &fileB, ifstream &fileR,string & buffB,string &buffR,gtime_t headtime) {
	while (getline(fileB, buffB)) {
		if (buffB.substr(60, 13) == "END OF HEADER") {
			break;
		}
	}
	while (getline(fileB, buffB)) {
		if (buffB[0] == '>')
			if (fabs(timediff(headtime,string2time(buffB.substr(2, 27)))) > 0.3)
				continue;
			else
				break;
	}
	while (getline(fileR, buffR)) {
		if (buffR.substr(60, 13) == "END OF HEADER") {
			break;
		}
	}
	while (getline(fileR, buffR)) {
		if (buffR[0] == '>')
			if (fabs(timediff(headtime, string2time(buffB.substr(2, 27)))) > 0.3)
				continue;
			else
				break;
	}
	return;
}

void OBS_RINEX::readepoch(ifstream &fileB, ifstream &fileR, string buffB, string buffR, gtime_t headtime) {
	stringstream ss;
	string temps;
	int flagB = 0;
	int flagR = 0;
	int numB = 0;
	int numR = 0;
	gtime_t timestampB, timestampR;
	/*while (getline(fileB, buffB)) {
		if (buffB[0] == '>')
			if (abs(headtime - string2time(buffB.substr(2, 27))) > 0.2) 
				continue;
			else
				break;
	}
	while (getline(fileR, buffR)) {
		if (buffR[0] == '>')
			if (abs(headtime - string2time(buffB.substr(2, 27))) > 0.2)
				continue;
			else
				break;
	}*/

		timestampB = string2time(buffB.substr(2, 27));
		timestampR = string2time(buffR.substr(2, 27));
		flagB = buffB[31]-'0';
		flagR = buffR[31]-'0';
		ss << buffB.substr(32, 3);
		ss >> numB;
		ss.clear();
		ss.str("");
		ss << buffR.substr(32, 3);
		ss >> numR;
		ss.clear();
		ss.str("");
		if (fabs(timediff(timestampB, timestampR)) <= 0.1) {
			vector<int> ().swap(GPS_satID_B);
			vector<int>().swap(GPS_satID_R);
			vector<int>().swap(BDS_satID_B);
			vector<int>().swap(BDS_satID_R);
			vector<int>().swap(GPS_flags_B);
			vector<int>().swap(GPS_flags_R);
			vector<int>().swap(BDS_flags_B);
			vector<int>().swap(BDS_flags_R);
			vector<double > ().swap(GPS_measurements_B);
			vector<double >().swap(GPS_measurements_R);
			vector<double >().swap(BDS_measurements_B);
			vector<double >().swap(BDS_measurements_R);
			getepochdata(numB,fileB, 'B');
			getepochdata(numR,fileR, 'R');
			cur_sat_num_B = numB;
			cur_sat_num_R = numR;
			epochflag_B = flagB;
			epochflag_R = flagR;
			GPStime_B = timestampB;
			GPStime_R = timestampR;
		}
		else {
			while (numB > 0) {
				getline(fileB, buffB);
				numB--;
			}
			while (numR > 0) {
				getline(fileR, buffR);
				numR--;
			}
		}
		return;

	



}
void OBS_RINEX::getepochdata(int num,ifstream &file, char mode) {
	vector<int> GPS_satID;
	vector<int> BDS_satID;
	vector<int> GPS_flags;
	vector<int> BDS_flags;
	vector<double > GPS_measurements;
	vector<double > BDS_measurements;
	int GPS_Fnum, BDS_Fnum;
	string buff;
	if (mode == 'B') {
		GPS_Fnum = GPS_Fnum_B;
		BDS_Fnum = BDS_Fnum_B;
	}
	else if (mode == 'R') {
		GPS_Fnum = GPS_Fnum_R;
		BDS_Fnum = BDS_Fnum_R;
	}
	else
		cout << "no such mode" << endl;
	while (num > 0) {
		getline(file, buff);
		num--;
		if (buff[0] == 'G') {
			getepochdatabody(GPS_Fnum, buff,GPS_satID,GPS_flags,GPS_measurements);
		}
		else {
			getepochdatabody(BDS_Fnum, buff, BDS_satID, BDS_flags, BDS_measurements);
		}	
	}
	if (mode == 'B') {
		GPS_satID_B = GPS_satID;
		BDS_satID_B = BDS_satID;
		GPS_flags_B = GPS_flags;
		BDS_flags_B = BDS_flags;
		GPS_measurements_B = GPS_measurements;
		BDS_measurements_B = BDS_measurements;
	}
	else if (mode == 'R') {
		GPS_satID_R = GPS_satID;
		BDS_satID_R = BDS_satID;
		GPS_flags_R = GPS_flags;
		BDS_flags_R = BDS_flags;
		GPS_measurements_R = GPS_measurements;
		BDS_measurements_R = BDS_measurements;
	}
	else
		cout << "no such mode" << endl;
	return;
}
void OBS_RINEX::getepochdatabody(int Frequency_num, string buff, vector<int>&GNSS_ID, vector<int>&GNSS_flags, vector<double>&GNSS_measuremenst) {
	stringstream ss;
	string temps;
	string subbuff;
	int tempint;
	double tempdouble;
	ss << buff.substr(1, 2);
	ss >> tempint;
	GNSS_ID.push_back(tempint);
	ss.clear();
	ss.str("");
	for (int i = 0; i < Frequency_num; i++) {
		if (5 + i * 64 > buff.size()) {
			GNSS_measuremenst.push_back(0);
			GNSS_measuremenst.push_back(0);
			GNSS_measuremenst.push_back(0);
			GNSS_measuremenst.push_back(0);
			GNSS_flags.push_back(0);
			GNSS_flags.push_back(0);
			GNSS_flags.push_back(0);
		}
		else {
			subbuff = buff.substr(5 + i * 64, 12);
			if (subbuff.substr(0, 4) == "    ") {
				GNSS_measuremenst.push_back(0);
				
			}
			else {
				ss << subbuff;
				ss >> tempdouble;
				GNSS_measuremenst.push_back(tempdouble);
				ss.clear(); ss.str("");
			}
			subbuff = buff.substr(20 + i * 64, 13);
			if (subbuff.substr(0, 4) == "    ") {
				GNSS_measuremenst.push_back(0);
			}
			else {
				ss << subbuff;
				ss >> tempdouble;
				GNSS_measuremenst.push_back(tempdouble);
				ss.clear(); ss.str("");
			}
			subbuff = buff.substr(40 + i * 64, 9);
			if (subbuff.substr(0, 4) == "    ") {
				GNSS_measuremenst.push_back(0);
			}
			else {
				ss << subbuff;
				ss >> tempdouble;
				GNSS_measuremenst.push_back(tempdouble);
				ss.clear(); ss.str("");
			}
			subbuff = buff.substr(59 + i * 64, 6);
			if (subbuff.substr(0, 4) == "    ") {
				GNSS_measuremenst.push_back(0);
			}
			else {
				ss << subbuff;
				ss >> tempdouble;
				GNSS_measuremenst.push_back(tempdouble);
				ss.clear(); ss.str("");
			}
			if (buff[18 + i * 64] != ' ')
				GNSS_flags.push_back(buff[18 + i * 64] - '0');
			else
				GNSS_flags.push_back(0);
			if (buff[34 + i * 64] != ' ')
				GNSS_flags.push_back(buff[34 + i * 64] - '0');
			else
				GNSS_flags.push_back(0);
			if (buff[33 + i * 64] != ' ')
				GNSS_flags.push_back(buff[33 + i * 64] - '0');
			else
				GNSS_flags.push_back(0);	
		}
	}
	return;
}
void OBS_RINEX::getdoppler() {
	for (int i = 0; i < GPS_satID_R.size(); i++) {
		int ID = GPS_satID_R[i];
		double doppler = GPS_measurements_R[i*GPS_Fnum_R * 4 + 2];
		if (sat_doppler.empty() || sat_doppler.find(ID) == sat_doppler.end()) {
			sat_doppler.insert(SDpair(ID, vector<double >(1, 0)));
			sat_doppler[ID][0] = doppler;
		}
		else {
			sat_doppler[ID].push_back(doppler);
		}
	
	}
	for (int i = 0; i < BDS_satID_R.size(); i++) {
		int ID = BDS_satID_R[i]+40;
		double doppler = BDS_measurements_R[i*BDS_Fnum_R * 4 + 2];
		if (sat_doppler.empty() || sat_doppler.find(ID) == sat_doppler.end()) {
			sat_doppler.insert(SDpair(ID, vector<double >(1, 0)));
			sat_doppler[ID][0] = doppler;
		}
		else {
			sat_doppler[ID].push_back(doppler);
		}
	}
}
void OBS_RINEX::printfdoppler() {
	SDmp::iterator it = sat_doppler.begin();
	string RootDir = "D:\\SHMILY\\340_RTK\\RTK_CPP\\doppler\\a";
	for (; it != sat_doppler.end(); it++) {
		int sat = it->first;
		if (sat > 40) {
			RootDir[RootDir.size()-1]='C';
			sat -= 40;
		}
		else
			RootDir[RootDir.size() - 1] = 'G';
		vector<double > doppler_v = it->second;
		string sats = to_string(sat)+".txt";
		string filepath = RootDir + sats;
		ofstream of;
		of.open(filepath, ios::out | ios::app);
		if (!of) {
			cout << "cannot open" << endl;
			return;
		}
		for (int i = 0; i < doppler_v.size(); i++) {
			of << setprecision(7)<<doppler_v[i] << endl;
		}
		//break;
	}
	return;



}