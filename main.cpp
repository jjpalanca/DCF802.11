#include <bits/stdc++.h>

using namespace std;

vector<double> generateUDM(int, int);
vector<double> generateXDM(int, vector<double>);
vector<int> generateSlots(double, vector<double>);
vector<int> generateArrivalT(vector<int>);
unordered_map<string, auto> simulate_SCD_CSMACA(vector<int>, vector<int>);
unordered_map<string, auto> simulate_HT_CSMACA(vector<int>, vector<int>);
unordered_map<string, auto> simulate_SCD_CSMACA_VCS(vector<int>, vector<int>);
unordered_map<string, auto> simulate_HT_CSMACA_VCS(vector<int>, vector<int>);

int main()
{
    vector<double> udm = generateUDM(2, 5);
    vector<double> xdm = generateXDM(2, udm);
    vector<int> slots = generateSlots(0.00001, xdm);
    vector<int> arrivalT = generateArrivalT(slots);

    cout << "This is the UDM:" << endl;
    for (double num: udm){
        cout << num << endl;
    }
    cout << "\nThis is the XDM:" << endl;
    for(double num: xdm){
        cout << num << endl;
    }

    cout << "\nThis is the generated slots:" << endl;
    for(int num: slots)
        cout << num << endl;


    cout << "\nThese are the arrival times: " << endl;
    for(int num: arrivalT){
        cout << num << endl;
    }

    return 0;
}

vector<double> generateUDM(int lambda, int simulT){
    vector<double> udm;
    int count = lambda * simulT;
    srand((unsigned)time(0));
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

unordered_map<string, auto> simulate_SCD_CSMACA(vector<int> A, vector<int> C){
    unordered_map<string, auto> result;
    int SLOTTED_TIME;

}

unordered_map<string, auto> simulate_HT_CSMACA(vector<int A, vector<int> C>){
    unordered_map<string, auto> result;
    int SLOTTED_TIME;
}

unordered_map<string, auto> simulate_SCD_CSMACA_VCS(vector<int> A, vector<int> C){
    unordered_map<string, auto> result;
    int SLOTTED_TIME;
}

unordered_map<string, auto> simulate_HT_CSMACA_VCS(vector<int> A, vector<int> C){
    unordered_map<string, auto> result;
    int SLOTTED_TIME;
}
