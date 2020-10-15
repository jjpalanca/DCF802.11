#include <bits/stdc++.h>

using namespace std;

const int DATA_FRAME_SIZE = 50;
const double SLOT_DURATION = 0.00001;
const int SIFS = 1;
const int CW_MIN = 4;
const int CW_MAX = 1024;
const int ACK= 2;
const int RTS = 2;
const int CTS = 2;
const int DIFS = 2;
const int XMISSION_RATE = 24000000;
const int SIMULATION_TIME = 10;
const vector<int> LAMBDA {200, 300, 500, 1000, 2000};

vector<double> generateUDM(int, int);
vector<double> generateXDM(int, vector<double>);
vector<int> generateSlots(double, vector<double>);
vector<int> generateArrivalT(vector<int>);
int generateBackoff(int);
void simulate_SCD_CSMACA(vector<int>, vector<int>);
unordered_map<string, int> simulate_HT_CSMACA(vector<int>, vector<int>);
unordered_map<string, int> simulate_SCD_CSMACA_VCS(vector<int>, vector<int>);
void simulate_HT_CSMACA_VCS(vector<int>, vector<int>);

int main()
{
    srand((unsigned)time(0));
    vector<double> udm1 = generateUDM(200, 10);
    vector<double> xdm1 = generateXDM(200, udm1);
    vector<int> slots1 = generateSlots(0.00001, xdm1);
    vector<int> A = generateArrivalT(slots1);
//    srand((unsigned)time(1));
    vector<double> udm2 = generateUDM(200, 10);
    vector<double> xdm2 = generateXDM(200, udm2);
    vector<int> slots2 = generateSlots(0.00001, xdm2);
    vector<int> C = generateArrivalT(slots2);

//    cout << "This is the UDM:" << endl;
//    for (double num: udm){
//        cout << num << endl;
//    }
//    cout << "\nThis is the XDM:" << endl;
//    for(double num: xdm){
//        cout << num << endl;
//    }
//
//    cout << "\nThis is the generated slots:" << endl;
//    for(int num: slots)
//        cout << num << endl;
//
//
//    cout << "\nThese are the arrival times: " << endl;
//    for(int num: arrivalT){
//        cout << num << endl;
//    }

//    vector<int>A{100, 200};
//    vector<int>C{200, 250};

    simulate_SCD_CSMACA(A, C);


    return 0;
}

vector<double> generateUDM(int lambda, int simulT){
    vector<double> udm;
    int count = lambda * simulT;
//    srand((unsigned)time(0));
    double randNum;
    for (int i = 0; i < count; i++){
        double temp = (double)rand() / RAND_MAX;
        randNum = temp * 1.0;
        udm.push_back(randNum);
    }

    sort(udm.begin(), udm.end());

    return udm;
}

vector<double> generateXDM(int lambda, vector<double> udm){
    vector<double> xdm;

    for(double num: udm){
        double Xn = log(1-num)*(1/double(lambda)) * -1;
        xdm.push_back(Xn);
    }

    return xdm;
}

vector<int> generateSlots(double slotDur, vector<double> xdm){
    vector<int> timeSlots;
    int slot;
    for(double num: xdm){
        slot = ceil(num / slotDur);
        timeSlots.push_back(slot);
    }
    return timeSlots;
}

vector<int> generateArrivalT(vector<int> slots){
    vector<int> arrivalTimes;

    int temp = slots[0];
    arrivalTimes.push_back(temp);

    for(int i = 1; i < slots.size(); i++){
        temp += slots[i];
        arrivalTimes.push_back(temp);
    }
    return arrivalTimes;
}

void simulate_SCD_CSMACA(vector<int> A, vector<int> C){
//    unordered_map<string, int> result;
    int SLOTTED_TIME = 0;
    int tp_A;
    int tp_C;
    int coll_A_ctr = 0;
    int coll_C_ctr = 0;
    int fair_index;
    int bA;
    int bC;
    int cw = CW_MIN;

    int a = 0;
    int c = 0;
    int simul_slots = SIMULATION_TIME / SLOT_DURATION;
//    int z = 0;
    srand((unsigned)time(0));

    while (SLOTTED_TIME < simul_slots) {
        if((A[a] > simul_slots) and (C[c] > simul_slots)){
            break;
        }

        bA = generateBackoff(cw);
        bC = generateBackoff(cw);
        cout << "bA: " << bA << endl;
        cout << "bC: " << bC << endl;
        if(SLOTTED_TIME == 0){
            cout << "A[0]: " << A[0] << endl;
            cout << "C[0]: " << C[0] << endl;
            if (A[0] < C[0]){
                cout << "A has been transmitted....\n";
                SLOTTED_TIME = A[0] + DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
                cout << "Slotted time: " << SLOTTED_TIME<< endl;
                a++;
                coll_C_ctr++;
            } else if (A[0] > C[0]){
                cout << "C has been transmitted....\n";
                SLOTTED_TIME = C[0] + DIFS + bC + DATA_FRAME_SIZE + SIFS + ACK;
                cout << "Slotted time: " << SLOTTED_TIME<< endl;
                c++;
                coll_A_ctr;
            } else {
                int temp_cw = cw;
                int itr = 0;
                bool retransmit = false;
                while(bA == bC && cw < CW_MAX) {
                    retransmit = true;
                    temp_cw = 2 * temp_cw;
                    coll_A_ctr++;
                    coll_C_ctr++;
                    if (itr == 0){
                        SLOTTED_TIME += A[0] + DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
                    } else {
                        SLOTTED_TIME += DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
                    }
                    cout << "Collision detected. New slotted time: " << SLOTTED_TIME << endl;
                    bA = generateBackoff(temp_cw);
                    bC = generateBackoff(temp_cw);
                    cout << "New bA: " << bA << endl;
                    cout << "New bC: " << bC << endl;
                    itr++;
                }
                if (bA < bC) {
                    // transmit A
                    if (retransmit){
                        SLOTTED_TIME += DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
                    } else {
                        SLOTTED_TIME += A[0] + DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
                    }

                    cout << "Collision detected. Transmitting A first. New slotted time: " << SLOTTED_TIME << endl;
                    a++;
                    bC = bC - bA;
                    coll_C_ctr++;
                    //A= 159 BA=4, transmit, st=100+59 = 159
                    // C = 100, 300 bc= 6, bc = 2, transmit c,
                    // then transmit C
                    SLOTTED_TIME += DIFS + bC + DATA_FRAME_SIZE + SIFS + ACK;
                    cout << "Transmit C after collision. New slotted time: " << SLOTTED_TIME<< endl;
                    c++;

                } else if (bA > bC) {
                    // transmit C
                    if (retransmit){
                        SLOTTED_TIME += DIFS + bC + DATA_FRAME_SIZE + SIFS + ACK;
                    } else {
                        SLOTTED_TIME += C[0] + DIFS + bC + DATA_FRAME_SIZE + SIFS + ACK;
                    }
                    cout << "Collision detected. Transmitting C first. New slotted time: " << SLOTTED_TIME << endl;
                    c++;
                    bA = bA - bC;
                    coll_A_ctr++;

                    // then transmit A
                    SLOTTED_TIME += DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
                    cout << "Transmit A after collision. New slotted time: " << SLOTTED_TIME<< endl;
                    a++;
                }

            }
            //z++;
            continue;
        }
        cout << "A[" << a << "]: " << A[a] << endl;
        cout << "C[" << c << "]: " << C[c] << endl;
        if (A[a] < C[c]){
            cout << "A has been transmitted...." << A[a] << endl;
            if(SLOTTED_TIME < A[a]){ // 200 , A=250
                SLOTTED_TIME = A[a];
                SLOTTED_TIME += DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
                cout << "Slotted time: " << SLOTTED_TIME<< endl;
                a++;
            } else{
                SLOTTED_TIME += DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
                cout << "Slotted time: " << SLOTTED_TIME<< endl;
                a++;
                coll_A_ctr++;
            }

        } else if(A[a] > C[c]) {
            cout << "C has been transmitted...." << C[c] << endl;
            if(SLOTTED_TIME < C[c]){
                SLOTTED_TIME = C[c];
                SLOTTED_TIME += DIFS + bC + DATA_FRAME_SIZE + SIFS + ACK;
                cout << "Slotted time: " << SLOTTED_TIME<< endl;
                c++;
            } else {
                SLOTTED_TIME += DIFS + bC + DATA_FRAME_SIZE + SIFS + ACK;
                cout << "Slotted time: " << SLOTTED_TIME<< endl;
                c++;
                coll_C_ctr++;
            }

        } else {
            int temp_cw = cw;
            while(bA == bC && cw < CW_MAX) {
                temp_cw = 2 * temp_cw;
                coll_A_ctr++;
                coll_C_ctr++;
                SLOTTED_TIME += DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
                bA = generateBackoff(temp_cw);
                bC = generateBackoff(temp_cw);
            }
            if (bA < bC) {
                // transmit A
                SLOTTED_TIME += DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
                cout << "Collision detected. Transmitting A first. New slotted time: " << SLOTTED_TIME << endl;
                a++;
                bC = bC - bA;
                coll_C_ctr++;

                // then transmit C
                SLOTTED_TIME += DIFS + bC + DATA_FRAME_SIZE + SIFS + ACK;
                cout << "Transmit C after collision. New slotted time: " << SLOTTED_TIME<< endl;
                c++;

            } else if (bA > bC) {
                // transmit C
                SLOTTED_TIME += DIFS + bC + DATA_FRAME_SIZE + SIFS + ACK;
                cout << "Collision detected. Transmitting C first. New slotted time: " << SLOTTED_TIME << endl;
                c++;
                bA = bA - bC;
                coll_A_ctr++;

                // then transmit A
                SLOTTED_TIME += DIFS + bA + DATA_FRAME_SIZE + SIFS + ACK;
                cout << "Transmit A after collision. New slotted time: " << SLOTTED_TIME<< endl;
                a++;
            }
        }
        //z++;
    }

    if (a >= A.size()){
        a = A.size();
    }
    if (c >= A.size()){
        c = C.size();
    }

    tp_A = a * 12000 / 10;
    tp_C = c * 12000 / 10;

//    fair_index = (a / c) )
    cout << "A: " << a << endl;
    cout << "C: " << c << endl;
    cout << "Throughput A: " << tp_A << endl;
    cout << "Throughput C: " << tp_C << endl;

    cout << "Collision A: " << coll_A_ctr << endl;
    cout << "Collision C: " << coll_C_ctr << endl;

    // [tp_A = 1000, tp_B = 200, .......]
    //
//    return result;
}


unordered_map<string, int> simulate_HT_CSMACA(vector<int> A, vector<int> C){
    unordered_map<string, int> result;
    int SLOTTED_TIME;
    int tp_A;
    int tp_C;
    int coll_A_ctr;
    int coll_C_ctr;
    int fair_index;

    return result;
}

unordered_map<string, int> simulate_SCD_CSMACA_VCS(vector<int> A, vector<int> C){
    unordered_map<string, int> result;
    int SLOTTED_TIME;
    int tp_A;
    int tp_C;
    int coll_A_ctr;
    int coll_C_ctr;
    int fair_index;

    return result;
}

void simulate_HT_CSMACA_VCS(vector<int> A, vector<int> C){
//    unordered_map<string, int> result;
    int SLOTTED_TIME = 0;
    int tp_A;
    int tp_C;
    int coll_A_ctr = 0;
    int coll_C_ctr = 0;
    int fair_index;
    int bA;
    int bC;
    int cw = CW_MIN;

    int a = 0;
    int c = 0;
    int simul_slots = SIMULATION_TIME / SLOT_DURATION;
//    int z = 0;
    srand((unsigned)time(0));

    while (SLOTTED_TIME < simul_slots) {
        if((A[a] > simul_slots) and (C[c] > simul_slots)){
            break;
        }

        bA = generateBackoff(cw);
        bC = generateBackoff(cw);
        cout << "bA: " << bA << endl;
        cout << "bC: " << bC << endl;
    }
//    return result;
}

int generateBackoff(int cw){

    return rand() % cw;
}
