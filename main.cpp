#include <bits/stdc++.h>

using namespace std;

const int DATA_FRAME_SIZE = 50;
const double SLOT_DURATION = 0.00001;
const int SIFS = 1;
const int CW_MIN = 4;
const int CW_MAX = 1024;
const int ACK = 2;
const int RTS = 2;
const int CTS = 2;
const int DIFS = 2;
const int XMISSION_RATE = 24000000;
const int SIMULATION_TIME = 10;
const vector<int> LAMBDA{ 200, 300, 500, 1000, 2000 };

vector<double> generateUDM(int, int);
vector<double> generateXDM(int, vector<double>);
vector<int> generateSlots(double, vector<double>);
vector<int> generateArrivalT(vector<int>);
int generateBackoff(int);
unordered_map<string, double> simulate_SCD_CSMACA(vector<int>, vector<int>);
unordered_map<string, double> simulate_HT_CSMACA(vector<int>, vector<int>);
unordered_map<string, double> simulate_SCD_CSMACA_VCS(vector<int>, vector<int>);
unordered_map<string, double> simulate_HT_CSMACA_VCS(vector<int>, vector<int>);

int main()
{
	srand((unsigned)time(0));

	for(int x :LAMBDA){
		// generate arrival time for A
		vector<double> udm1 = generateUDM(x, SIMULATION_TIME);
		vector<double> xdm1 = generateXDM(x, udm1);
		vector<int> slots1 = generateSlots(SLOT_DURATION, xdm1);
		vector<int> A = generateArrivalT(slots1);

		// generate arrival time for C
		vector<double> udm2 = generateUDM(x, SIMULATION_TIME);
		vector<double> xdm2 = generateXDM(x, udm2);
		vector<int> slots2 = generateSlots(SLOT_DURATION, xdm2);
		vector<int> C = generateArrivalT(slots2);

		unordered_map<string, double> SCD_CSMACA = simulate_SCD_CSMACA(A, C);
		cout << "Output for SCD_CSMACA: " << endl;
		for (auto  x : SCD_CSMACA) {
			if (x.first == "ThroughputA" or x.first == "ThroughputC") {
				cout << x.first << ": " << (int)x.second << endl;
			}
			else {
				cout << x.first << ": " << x.second << endl;
			}
		}

		cout << endl;

		unordered_map<string, double> HT_CSMACA_VCS = simulate_HT_CSMACA_VCS(A, C);
		cout << "Output for HT_CSMACA_VCS: " << endl;
		for (auto x : HT_CSMACA_VCS) {
			if (x.first == "ThroughputA" or x.first == "ThroughputC") {
				cout << x.first << ": " << (int)x.second << endl;
			}
			else {
				cout << x.first << ": " << x.second << endl;
			}
		}
		cout << endl;

		unordered_map<string, double> HT_CSMACA = simulate_HT_CSMACA(A, C);
		cout << "Output for HT_CSMACA: " << endl;
		for (auto x : HT_CSMACA) {
			if (x.first == "ThroughputA" or x.first == "ThroughputC") {
				cout << x.first << ": " << (int)x.second << endl;
			}
			else {
				cout << x.first << ": " << x.second << endl;
			}
		}
		cout << endl;

		unordered_map<string, double> SCD_CSMACA_VCS = simulate_SCD_CSMACA_VCS(A, C);
		cout << "Output for SCD_CSMACA_VCS: " << endl;
		for (auto x : SCD_CSMACA_VCS) {
			if (x.first == "ThroughputA" or x.first == "ThroughputC") {
				cout << x.first << ": " << (int)x.second << endl;
			}
			else {
				cout << x.first << ": " << x.second << endl;
			}
		}
		cout << endl;

	}
	
	return 0;
}

vector<double> generateUDM(int lambda, int simulT) {
	vector<double> udm;
	int count = lambda * simulT;
	//    srand((unsigned)time(0));
	double randNum;
	for (int i = 0; i < count; i++) {
		double temp = (double)rand() / RAND_MAX;
		randNum = temp * 1.0;
		udm.push_back(randNum);
	}

	sort(udm.begin(), udm.end());

	return udm;
}

vector<double> generateXDM(int lambda, vector<double> udm) {
	vector<double> xdm;

	for (double num : udm) {
		double Xn = log(1 - num)*(1 / double(lambda)) * -1;
		xdm.push_back(Xn);
	}

	return xdm;
}

vector<int> generateSlots(double slotDur, vector<double> xdm) {
	vector<int> timeSlots;
	int slot;
	for (double num : xdm) {
		slot = ceil(num / slotDur);
		timeSlots.push_back(slot);
	}
	return timeSlots;
}

vector<int> generateArrivalT(vector<int> slots) {
	vector<int> arrivalTimes;

	int temp = slots[0];
	arrivalTimes.push_back(temp);

	for (int i = 1; i < slots.size(); i++) {
		temp += slots[i];
		arrivalTimes.push_back(temp);
	}
	return arrivalTimes;
}

unordered_map<string, double> simulate_SCD_CSMACA(vector<int> A, vector<int> C) {
	unordered_map<string, double> result;
	int SLOTTED_TIME = 0;
	int tp_A;
	int tp_C;
	int coll_A_ctr = 0;
	int coll_C_ctr = 0;
	double fair_index;
	int bA;
	int bC;
	int cw = CW_MIN;
	//    int slots_taken_A = 0;
	//    int slots_taken_C = 0;
	int a = 0;
	int c = 0;
	int simul_slots = SIMULATION_TIME / SLOT_DURATION;

	srand((unsigned)time(0));

	while (SLOTTED_TIME < simul_slots) {
		if ((A[a] > simul_slots) and (C[c] > simul_slots)) {
			break;
		}

		bA = generateBackoff(cw);
		bC = generateBackoff(cw);
		//        cout << "bA: " << bA << endl;
		//        cout << "bC: " << bC << endl;
		if (SLOTTED_TIME == 0) {
			//            cout << "A[0]: " << A[0] << endl;
			//            cout << "C[0]: " << C[0] << endl;
			if (A[0] < C[0]) {
				//                cout << "A has been transmitted....\n";
				SLOTTED_TIME = A[0] + DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
				//                slots_taken_A += SLOTTED_TIME;
				//                cout << "Slotted time: " << SLOTTED_TIME<< endl;
				a++;
			}
			else if (A[0] > C[0]) {
				//                cout << "C has been transmitted....\n";
				SLOTTED_TIME = C[0] + DIFS + bC + DATA_FRAME_SIZE + SIFS + ACK;
				//                slots_taken_C += SLOTTED_TIME;
				//                cout << "Slotted time: " << SLOTTED_TIME<< endl;
				c++;
			}
			else {
				coll_A_ctr++;
				coll_C_ctr++;
				int temp_cw = cw;
				int itr = 0;
				bool retransmit = false;
				while (bA == bC && cw < CW_MAX) {
					retransmit = true;
					temp_cw = 2 * temp_cw;

					if (itr == 0) {
						SLOTTED_TIME += A[0] + DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
					}
					else {
						coll_A_ctr++;
						coll_C_ctr++;
						SLOTTED_TIME += DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
					}
					//                    cout << "Collision detected. New slotted time: " << SLOTTED_TIME << endl;
					bA = generateBackoff(temp_cw);
					bC = generateBackoff(temp_cw);
					//                    cout << "New bA: " << bA << endl;
					//                    cout << "New bC: " << bC << endl;
					itr++;
				}
				if (bA < bC) {
					// transmit A
					if (retransmit) {
						SLOTTED_TIME += DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
						//                        slots_taken_A += DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
					}
					else {
						SLOTTED_TIME += A[0] + DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
						//                        slots_taken_A += SLOTTED_TIME;
					}

					//                    cout << "Collision detected. Transmitting A first. New slotted time: " << SLOTTED_TIME << endl;
					a++;
					bC = bC - bA;

					// then transmit C
					SLOTTED_TIME += DIFS + bC + DATA_FRAME_SIZE + SIFS + ACK;
					//                    slots_taken_C += DIFS + bC + DATA_FRAME_SIZE + SIFS + ACK;
					//                    cout << "Transmit C after collision. New slotted time: " << SLOTTED_TIME<< endl;
					c++;

				}
				else if (bA > bC) {
					// transmit C
					if (retransmit) {
						SLOTTED_TIME += DIFS + bC + DATA_FRAME_SIZE + SIFS + ACK;
						//                        slots_taken_C += DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
					}
					else {
						SLOTTED_TIME += C[0] + DIFS + bC + DATA_FRAME_SIZE + SIFS + ACK;
						//                        slots_taken_C += SLOTTED_TIME;
					}
					//                    cout << "Collision detected. Transmitting C first. New slotted time: " << SLOTTED_TIME << endl;
					c++;
					bA = bA - bC;

					// then transmit A
					SLOTTED_TIME += DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
					//                    slots_taken_A += DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
					//                    cout << "Transmit A after collision. New slotted time: " << SLOTTED_TIME<< endl;
					a++;
				}

			}
			continue;
		}
		//        cout << "A[" << a << "]: " << A[a] << endl;
		//        cout << "C[" << c << "]: " << C[c] << endl;
		if (A[a] < C[c]) {
			//            cout << "A has been transmitted...." << A[a] << endl;
			if (SLOTTED_TIME < A[a]) {
				SLOTTED_TIME = A[a];
				SLOTTED_TIME += DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
				//                cout << "Slotted time: " << SLOTTED_TIME<< endl;
				a++;
			}
			else {
				SLOTTED_TIME += DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
				//                cout << "Slotted time: " << SLOTTED_TIME<< endl;
				a++;
			}
			//            slots_taken_A += DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;

		}
		else if (A[a] > C[c]) {
			//            cout << "C has been transmitted...." << C[c] << endl;
			if (SLOTTED_TIME < C[c]) {
				SLOTTED_TIME = C[c];
				SLOTTED_TIME += DIFS + bC + DATA_FRAME_SIZE + SIFS + ACK;
				//                cout << "Slotted time: " << SLOTTED_TIME<< endl;
				c++;
			}
			else {
				SLOTTED_TIME += DIFS + bC + DATA_FRAME_SIZE + SIFS + ACK;
				//                cout << "Slotted time: " << SLOTTED_TIME<< endl;
				c++;
			}
			//            slots_taken_C += DIFS + bC + DATA_FRAME_SIZE + SIFS + ACK;

		}
		else {
			coll_A_ctr++;
			coll_C_ctr++;
			int temp_cw = cw;
			int itr = 0;
			while (bA == bC && cw < CW_MAX) {
				temp_cw = 2 * temp_cw;
				if (itr != 0) {
					coll_A_ctr++;
					coll_C_ctr++;
				}
				SLOTTED_TIME += DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
				bA = generateBackoff(temp_cw);
				bC = generateBackoff(temp_cw);
				itr++;
			}
			if (bA < bC) {
				// transmit A
				SLOTTED_TIME += DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
				//                slots_taken_A += DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
				//                cout << "Collision detected. Transmitting A first. New slotted time: " << SLOTTED_TIME << endl;
				a++;
				bC = bC - bA;

				// then transmit C
				if (C[c] <= simul_slots) {
					SLOTTED_TIME += DIFS + bC + DATA_FRAME_SIZE + SIFS + ACK;
					//                    slots_taken_C += DIFS + bC + DATA_FRAME_SIZE + SIFS + ACK;
					//                    cout << "Transmit C after collision. New slotted time: " << SLOTTED_TIME<< endl;
					c++;
				}


			}
			else if (bA > bC) {
				// transmit C
				SLOTTED_TIME += DIFS + bC + DATA_FRAME_SIZE + SIFS + ACK;
				//                slots_taken_C += DIFS + bC + DATA_FRAME_SIZE + SIFS + ACK;
				//                cout << "Collision detected. Transmitting C first. New slotted time: " << SLOTTED_TIME << endl;
				c++;
				bA = bA - bC;

				// then transmit A
				if (A[a] <= simul_slots) {
					SLOTTED_TIME += DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
					//                    slots_taken_A += DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
					//                    cout << "Transmit A after collision. New slotted time: " << SLOTTED_TIME<< endl;
					a++;
				}

			}
		}
	}

	if (a >= A.size()) {
		a = A.size();
	}
	if (c >= A.size()) {
		c = C.size();
	}

	tp_A = a * 12000 / 10;
	tp_C = c * 12000 / 10;
	fair_index = (double)a / c;

	result["ThroughputA"] = (double)tp_A;
	result["ThroughputC"] = (double)tp_C;
	result["CollisionA"] = (double)coll_A_ctr;
	result["CollisionC"] = (double)coll_C_ctr;
	result["FairIndex"] = fair_index;

	return result;
}


unordered_map<string, double> simulate_HT_CSMACA(vector<int> A, vector<int> C) {
	unordered_map<string, double> result;
	int SLOTTED_TIME;
	int tp_A;
	int tp_C;
	int coll_A_ctr;
	int coll_C_ctr;
	double fair_index;

	int backOff_A;
	int backOff_C;
	double k = 1;
	const int w0 = 4;

	//set up
	tp_A = tp_C = coll_A_ctr = coll_C_ctr = 0;
	SLOTTED_TIME = 0;

	int simul_slots = SIMULATION_TIME / SLOT_DURATION;


START:while (SLOTTED_TIME < simul_slots) {
	//backoff generation
	backOff_A = rand() % w0;
	backOff_C = rand() % w0;
	//check if slotted_time is less than tran

	//A transmits first
	if (A[tp_A] < C[tp_C]) {
		//check if slotted_time is less than transmition time
		if (SLOTTED_TIME < A[tp_A]) {
			SLOTTED_TIME = A[tp_A];
		}
		//check if c will transmit during A
		if (A[tp_A] + DIFS + backOff_A + DATA_FRAME_SIZE + SIFS + ACK < C[tp_C] + DIFS) {
			SLOTTED_TIME += DIFS + backOff_A + DATA_FRAME_SIZE + SIFS + ACK;
			tp_A++;
			//goto START;
		}
		else {
			//initial collision time used
			SLOTTED_TIME += (C[tp_C] + DIFS + backOff_C + DATA_FRAME_SIZE + SIFS + ACK) - A[tp_A];
			//increment C collision
			coll_C_ctr++;
			//begin retrans loop
			k = 1;
			while (abs((A[tp_A] + backOff_A) - (C[tp_C] + backOff_C)) <= (DIFS + DATA_FRAME_SIZE + SIFS + ACK)) {
				//new backoff's
				backOff_A = (rand() % ((static_cast<int> (pow(2, k)))*(w0 - 1))) % 1024;
				backOff_C = (rand() % ((static_cast<int> (pow(2, k)))*(w0 - 1))) % 1024;
				k++;
				//check for new collision
				if (abs((A[tp_A] + backOff_A) - (C[tp_C] + backOff_C)) <= (DIFS + DATA_FRAME_SIZE + SIFS + ACK)) {
					if (A[tp_A] + backOff_A < C[tp_C] + backOff_C) {
						SLOTTED_TIME += (C[tp_C] + DIFS + backOff_C + DATA_FRAME_SIZE + SIFS + ACK) - A[tp_A];
						coll_C_ctr++;

					}
					if (C[tp_C] + backOff_C < A[tp_A] + backOff_A) {
						SLOTTED_TIME += (A[tp_A] + DIFS + backOff_A + DATA_FRAME_SIZE + SIFS + ACK) - C[tp_C];
						coll_A_ctr++;
					}
					if (A[tp_A] + backOff_A == C[tp_C] + backOff_C) {
						SLOTTED_TIME += DIFS + backOff_A + DATA_FRAME_SIZE + SIFS + ACK;
						coll_A_ctr++;
						coll_C_ctr++;

					}
				}
				else {
					if (backOff_A < backOff_C) {
						//A transmits
						SLOTTED_TIME += DIFS + backOff_A + DATA_FRAME_SIZE + SIFS + ACK;
						tp_A++;
						goto START;

					}
					if (backOff_C < backOff_A) {
						SLOTTED_TIME += DIFS + backOff_C + DATA_FRAME_SIZE + SIFS + ACK;
						tp_C++;
						goto START;
					}
				}
			}
		}
	}

	//C transmits first
	if (C[tp_C] < A[tp_A]) {
		//check if slotted_time is less than transmition time
		if (SLOTTED_TIME < C[tp_C]) {
			SLOTTED_TIME = C[tp_C];
		}
		//check if A will transmit during C
		if (C[tp_C] + DIFS + backOff_A + DATA_FRAME_SIZE + SIFS + ACK < A[tp_A] + DIFS) {
			SLOTTED_TIME += DIFS + backOff_C + DATA_FRAME_SIZE + SIFS + ACK;
			tp_C++;
		}
		else {
			//initial collision time used
			SLOTTED_TIME += (A[tp_A] + DIFS + backOff_A + DATA_FRAME_SIZE + SIFS + ACK) - C[tp_C];
			//increment C collision
			coll_A_ctr++;
			//begin retrans loop
			k = 1;
			while (abs((C[tp_C] + backOff_C) - (A[tp_A] + backOff_A)) <= (DIFS + DATA_FRAME_SIZE + SIFS + ACK)) {
				//new backoff's
				backOff_A = (rand() % ((static_cast<int> (pow(2, k)))*(w0 - 1))) % 1024;
				backOff_C = (rand() % ((static_cast<int> (pow(2, k)))*(w0 - 1))) % 1024;
				k++;
				//check for new collision
				if (abs((A[tp_A] + backOff_A) - (C[tp_C] + backOff_C)) <= (DIFS + DATA_FRAME_SIZE + SIFS + ACK)) {
					if (A[tp_A] + backOff_A < C[tp_C] + backOff_C) {
						SLOTTED_TIME += (C[tp_C] + DIFS + backOff_C + DATA_FRAME_SIZE + SIFS + ACK) - A[tp_A];
						coll_C_ctr++;

					}
					if (C[tp_C] + backOff_C < A[tp_A] + backOff_A) {
						SLOTTED_TIME += (A[tp_A] + DIFS + backOff_A + DATA_FRAME_SIZE + SIFS + ACK) - C[tp_C];
						coll_A_ctr++;
					}
					if (A[tp_A] + backOff_A == C[tp_C] + backOff_C) {
						SLOTTED_TIME += DIFS + backOff_A + DATA_FRAME_SIZE + SIFS + ACK;
						coll_A_ctr++;
						coll_C_ctr++;

					}
				}
				else {
					if (backOff_A < backOff_C) {
						//A transmits
						SLOTTED_TIME += DIFS + backOff_A + DATA_FRAME_SIZE + SIFS + ACK;
						tp_A++;
						goto START;

					}
					if (backOff_C < backOff_A) {
						SLOTTED_TIME += DIFS + backOff_C + DATA_FRAME_SIZE + SIFS + ACK;
						tp_C++;
						goto START;
					}
				}
			}
		}
	}





	//simoultaneous transmit
	if (A[tp_A] == C[tp_C]) {
		//check sloted time
		if (SLOTTED_TIME < A[tp_A]) {
			SLOTTED_TIME = A[tp_A];
		}
		//collision
		SLOTTED_TIME += DIFS + backOff_A + DATA_FRAME_SIZE + SIFS + ACK;
		coll_A_ctr++;
		coll_C_ctr++;

		//retrans until no collision
		k = 1;
		while (abs((C[tp_C] + backOff_C) - (A[tp_A] + backOff_A)) <= (DIFS + DATA_FRAME_SIZE + SIFS + ACK)) {
			//new backoff's
			backOff_A = (rand() % ((static_cast<int> (pow(2, k)))*(w0 - 1))) % 1024;
			backOff_C = (rand() % ((static_cast<int> (pow(2, k)))*(w0 - 1))) % 1024;
			k++;
			// check collision
			if (abs((A[tp_A] + backOff_A) - (C[tp_C] + backOff_C)) <= (DIFS + DATA_FRAME_SIZE + SIFS + ACK)) {
				if (A[tp_A] + backOff_A < C[tp_C] + backOff_C) {
					SLOTTED_TIME += (C[tp_C] + DIFS + backOff_C + DATA_FRAME_SIZE + SIFS + ACK) - A[tp_A];
					coll_C_ctr++;

				}
				if (C[tp_C] + backOff_C < A[tp_A] + backOff_A) {
					SLOTTED_TIME += (A[tp_A] + DIFS + backOff_A + DATA_FRAME_SIZE + SIFS + ACK) - C[tp_C];
					coll_A_ctr++;
				}
				if (A[tp_A] + backOff_A == C[tp_C] + backOff_C) {
					SLOTTED_TIME += DIFS + backOff_A + DATA_FRAME_SIZE + SIFS + ACK;
					coll_A_ctr++;
					coll_C_ctr++;

				}
			}
			else {
				if (backOff_A < backOff_C) {
					//A transmits
					SLOTTED_TIME += DIFS + backOff_A + DATA_FRAME_SIZE + SIFS + ACK;
					tp_A++;
					goto START;

				}
				if (backOff_C < backOff_A) {
					//C transmits
					SLOTTED_TIME += DIFS + backOff_C + DATA_FRAME_SIZE + SIFS + ACK;
					tp_C++;
					goto START;
				}
			}


		}
	}

}

fair_index = (double)tp_A / tp_C;
tp_A = tp_A * 12000 / 10;
tp_C = tp_C * 12000 / 10;

result["ThroughputA"] = (double)tp_A;
result["ThroughputC"] = (double)tp_C;
result["CollisionA"] = (double)coll_A_ctr;
result["CollisionC"] = (double)coll_C_ctr;
result["FairIndex"] = fair_index;

	  return result;
}

unordered_map<string, double> simulate_SCD_CSMACA_VCS(vector<int> A, vector<int> C) {
	unordered_map<string, double> result;
	int SLOTTED_TIME;
	int tp_A;
	int tp_C;
	int coll_A_ctr;
	int coll_C_ctr;
	double fair_index;

	int nav_RTS = 0;
	int nav_CTS = 0;

	int backOff_A;
	int backOff_C;

	const int w0 = 4;

	int k = 1;
	int simul_slots = SIMULATION_TIME / SLOT_DURATION;


	//set up
	tp_A = tp_C = coll_A_ctr = coll_C_ctr = 0;
	SLOTTED_TIME = 0;

	srand((unsigned)time(0));

START:while (SLOTTED_TIME < simul_slots) {

	//generate backOff's
	backOff_A = rand() % w0;
	backOff_C = rand() % w0;

	//if A fires
	if (A[tp_A] + backOff_A < C[tp_C] + backOff_C) {
		//check if SLOTTED_TIME is less than transmission time and update
		if (SLOTTED_TIME < A[tp_A]) {
			SLOTTED_TIME = A[tp_A];
		}
		//if(A[tp_A] + DIFS + 1 < C[tp_C]){

		//transmit A
		SLOTTED_TIME += (DIFS + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK + backOff_A);
		//tp_A++;
		//goto START;
		if ((C[tp_C] + backOff_C) - (A[tp_A] + backOff_A) < (DIFS + backOff_A)) {
			backOff_C = (C[tp_C] + backOff_C) - (A[tp_A] + backOff_A);
			tp_A++;

		}
		else {
			tp_A++;
		}
	}


	//if C fires
	if (C[tp_C] + backOff_C < A[tp_A] + backOff_A) {
		//check if SLOTTED_TIME is less than transmission time and update
		if (SLOTTED_TIME < C[tp_C]) {
			SLOTTED_TIME = C[tp_C];
		}
		//transmit A
		SLOTTED_TIME += (DIFS + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK + backOff_C);
		//tp_C++;
		//goto START;
		if ((A[tp_A] + backOff_A) - (C[tp_C] + backOff_C) < (DIFS + backOff_C)) {
			backOff_A = (C[tp_A] + backOff_A) - (C[tp_C] + backOff_C);
			tp_C++;

		}
		else {
			tp_C++;
		}
	}


	//collision
	if (A[tp_A] + backOff_A == C[tp_C] + backOff_C) {
		//check if SLOTTED_TIME is less than transmission time and update
		if (SLOTTED_TIME < C[tp_C]) {
			SLOTTED_TIME = C[tp_C];
		}
		//add to collision count
		coll_A_ctr++;
		coll_C_ctr++;
		//add collision frames to SLOTTED_TIME
		SLOTTED_TIME += (backOff_A + DIFS + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK);
		k = 1;
		while (abs(A[tp_A] + backOff_A - (C[tp_C] + backOff_C) < (DIFS + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK))) {
			backOff_A = (rand() % ((static_cast<int> (pow(2, k)))*(w0 - 1))) % 1024;
			backOff_C = (rand() % ((static_cast<int> (pow(2, k)))*(w0 - 1))) % 1024;
			k++;

			//same time
			if (A[tp_A] + backOff_A == C[tp_C] + backOff_C) {
				coll_A_ctr++;
				coll_C_ctr++;
				//add collision frames to SLOTTED_TIME
				SLOTTED_TIME += (backOff_A + DIFS + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK);
			}
			//A first
			if (A[tp_A] + backOff_A < C[tp_C] + backOff_C) {
				//transmit A
				SLOTTED_TIME += DIFS + backOff_A + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
				//adjust backoff_C
				backOff_C = (C[tp_C] + backOff_C) - (A[tp_A] + backOff_A);
				//transmit C
				SLOTTED_TIME += DIFS + backOff_C + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
				//increment tp_A, tp_C
				tp_A++;
				tp_C++;
				goto START;
			}
			//C first
			if (A[tp_A] + backOff_A > C[tp_C] + backOff_C) {
				//transmit A
				SLOTTED_TIME += DIFS + backOff_C + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
				//adjust backoff_C
				backOff_A = (A[tp_C] + backOff_A) - (C[tp_C] + backOff_C);
				//transmit C
				SLOTTED_TIME += DIFS + backOff_A + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
				//increment tp_A, tp_C
				tp_A++;
				tp_C++;
				goto START;
			}
		}

	}



}
	 
fair_index = (double)tp_A / tp_C;
tp_A = tp_A * 12000 / 10;
tp_C = tp_C * 12000 / 10;

result["ThroughputA"] = (double)tp_A;
result["ThroughputC"] = (double)tp_C;
result["CollisionA"] = (double)coll_A_ctr;
result["CollisionC"] = (double)coll_C_ctr;
result["FairIndex"] = fair_index;


	  return result;
}

unordered_map<string, double> simulate_HT_CSMACA_VCS(vector<int> A, vector<int> C) {
	unordered_map<string, double> result;
	int SLOTTED_TIME = 0;
	int tp_A;
	int tp_C;
	int coll_A_ctr = 0;
	int coll_C_ctr = 0;
	double fair_index;
	int bA;
	int bC;
	int cw = CW_MIN;
	//    int slots_taken_A = 0;
	//    int slots_taken_C = 0;

	int a = 0;
	int c = 0;
	int simul_slots = SIMULATION_TIME / SLOT_DURATION;
	srand((unsigned)time(0));

	while (SLOTTED_TIME < simul_slots) {
		if ((A[a] > simul_slots) and (C[c] > simul_slots)) {
			break;
		}

		bA = generateBackoff(cw);
		bC = generateBackoff(cw);
		//        cout << "bA: " << bA << endl;
		//        cout << "bC: " << bC << endl;
		if (SLOTTED_TIME == 0) {
			//            cout << "A[0]: " << A[0] << endl;
			//            cout << "C[0]: " << C[0] << endl;
			if (A[0] < C[0]) {
				//                cout << "A[0] has been transmitted....\n";
				SLOTTED_TIME = A[0] + DIFS + bA + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
				//                slots_taken_A += SLOTTED_TIME;
				//                cout << "Slotted time: " << SLOTTED_TIME<< endl;
				a++;
				if (C[0] <= A[0] + DIFS + bA + RTS + SIFS + CTS) {
					coll_C_ctr++;
				}
			}
			else if (A[0] > C[0]) {
				//                cout << "C[0] has been transmitted....\n";
				SLOTTED_TIME = C[0] + DIFS + bC + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
				//                slots_taken_C += SLOTTED_TIME;
				//                cout << "Slotted time: " << SLOTTED_TIME<< endl;
				c++;
				if (A[0] <= C[0] + DIFS + bA + RTS + SIFS + CTS) {
					coll_A_ctr++;
				}
			}
			else {
				coll_A_ctr++;
				coll_C_ctr++;
				int temp_cw = cw;
				int itr = 0;
				bool retransmit = false;
				while (bA == bC && cw < CW_MAX) {
					retransmit = true;
					temp_cw = 2 * temp_cw;

					if (itr == 0) {
						SLOTTED_TIME += A[0] + DIFS + bA + RTS + SIFS + CTS;
					}
					else {
						coll_A_ctr++;
						coll_C_ctr++;
						SLOTTED_TIME += DIFS + bA + RTS + SIFS + CTS;
					}
					//                    cout << "Collision detected. bA and bC are equal. Generating new bA and bC on the next round. New slotted time: " << SLOTTED_TIME << endl;
					bA = generateBackoff(temp_cw);
					bC = generateBackoff(temp_cw);
					//                    cout << "New bA: " << bA << endl;
					//                    cout << "New bC: " << bC << endl;
					itr++;
				}
				if (bA < bC) {
					// transmit A
					if (retransmit) {
						SLOTTED_TIME += DIFS + bA + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
						//                        slots_taken_A += DIFS + bA  + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
					}
					else {
						SLOTTED_TIME += A[0] + DIFS + bA + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
						//                        slots_taken_A += SLOTTED_TIME;
					}

					//                    cout << "Collision detected. Transmitting A first. New slotted time after transmission A: " << SLOTTED_TIME << endl;
					a++;
					bC = bC - bA;
					coll_C_ctr++;

					// then transmit C
					SLOTTED_TIME += DIFS + bC + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
					//                    slots_taken_C += DIFS + bC  + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
					//                    cout << "Transmit C after collision. New slotted time after transmission C: " << SLOTTED_TIME<< endl;
					c++;

				}
				else if (bA > bC) {
					// transmit C
					if (retransmit) {
						SLOTTED_TIME += DIFS + bC + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
						//                        slots_taken_C += DIFS + bC  + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
					}
					else {
						SLOTTED_TIME += C[0] + DIFS + bC + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
						//                        slots_taken_C += SLOTTED_TIME;
					}
					//                    cout << "Collision detected. Transmitting C first. New slotted time after transmission C: " << SLOTTED_TIME << endl;
					c++;
					bA = bA - bC;
					coll_A_ctr++;

					// then transmit A
					SLOTTED_TIME += DIFS + bA + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
					//                    slots_taken_A += DIFS + bA  + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
					//                    cout << "Transmit A after collision. New slotted time after transmission A: " << SLOTTED_TIME<< endl;
					a++;
				}
			}

		}
		else {
			if (A[a] < C[c]) {
				//                cout << "A has been transmitted...." << A[a] << endl;
				if (SLOTTED_TIME < A[a]) {
					SLOTTED_TIME = A[a];
					SLOTTED_TIME += DIFS + bA + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
					//                    cout << "Slotted time: " << SLOTTED_TIME<< endl;
					a++;
				}
				else {
					SLOTTED_TIME += DIFS + bA + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
					//                    cout << "Slotted time: " << SLOTTED_TIME<< endl;
					a++;
				}
				//                slots_taken_A += DIFS + bA  + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
				if (C[c] <= A[a] + DIFS + bA + RTS + SIFS + CTS) {
					coll_C_ctr++;
				}

			}
			else if (A[a] > C[c]) {
				//                cout << "C has been transmitted...." << C[c] << endl;
				if (SLOTTED_TIME < C[c]) {
					SLOTTED_TIME = C[c];
					SLOTTED_TIME += DIFS + bC + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
					//                    cout << "Slotted time: " << SLOTTED_TIME<< endl;
					c++;
				}
				else {
					SLOTTED_TIME += DIFS + bC + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
					//                    cout << "Slotted time: " << SLOTTED_TIME<< endl;
					c++;
				}
				//                slots_taken_C += DIFS + bC  + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
				if (A[a] <= C[c] + DIFS + bA + RTS + SIFS + CTS) {
					coll_A_ctr++;
				}

			}
			else {
				coll_A_ctr++;
				coll_C_ctr++;
				int temp_cw = cw;
				int itr = 0;
				while (bA == bC && cw < CW_MAX) {
					temp_cw = 2 * temp_cw;
					if (itr != 0) {
						coll_A_ctr++;
						coll_C_ctr++;
					}
					SLOTTED_TIME += DIFS + bA + RTS + SIFS + CTS;
					bA = generateBackoff(temp_cw);
					bC = generateBackoff(temp_cw);
					itr++;
				}
				if (bA < bC) {
					// transmit A
					SLOTTED_TIME += DIFS + bA + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
					//                    slots_taken_A += DIFS + bA  + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
					//                    cout << "Collision detected. Transmitting A first. New slotted time: " << SLOTTED_TIME << endl;
					a++;
					bC = bC - bA;
					coll_C_ctr++;

					// then transmit C
					if (C[c] <= simul_slots) {
						SLOTTED_TIME += DIFS + bC + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
						//                        slots_taken_C += DIFS + bC  + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
						//                        cout << "Transmit C after collision. New slotted time: " << SLOTTED_TIME<< endl;
						c++;
					}


				}
				else if (bA > bC) {
					// transmit C
					SLOTTED_TIME += DIFS + bC + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
					//                    slots_taken_C += DIFS + bC + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
					//                    cout << "Collision detected. Transmitting C first. New slotted time: " << SLOTTED_TIME << endl;
					c++;
					bA = bA - bC;
					coll_A_ctr++;

					// then transmit A
					if (A[a] <= simul_slots) {
						SLOTTED_TIME += DIFS + bA + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
						//                        slots_taken_A += DIFS + bA  + RTS + SIFS + CTS + SIFS + DATA_FRAME_SIZE + SIFS + ACK;
						//                        cout << "Transmit A after collision. New slotted time: " << SLOTTED_TIME<< endl;
						a++;
					}

				}
			}
		}
	}

	if (a >= A.size()) {
		a = A.size();
	}
	if (c >= A.size()) {
		c = C.size();
	}

	tp_A = a * 12000 / 10;
	tp_C = c * 12000 / 10;

	fair_index = (double)a / c;

	//    cout << "Fair index: " << fair_index << endl;

	result["ThroughputA"] = (double)tp_A;
	result["ThroughputC"] = (double)tp_C;
	result["CollisionA"] = (double)coll_A_ctr;
	result["CollisionC"] = (double)coll_C_ctr;
	result["FairIndex"] = fair_index;

	return result;
}

int generateBackoff(int cw) {

	return rand() % cw;
}
