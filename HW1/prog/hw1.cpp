#include <iostream>
#include <fstream>
#include <vector>
#include <cmath> // Use cmath for standard math functions
using namespace std;

// Function to compute the blocking time, B_i, for task i
double computeBi(const vector<vector<double> >& data, int num, int i){
    double max = 0; // Assuming blocking times are non-negative
    int p_i = data[i][0];
    for(int j = 0; j < num; j++){
        if(data[j][0] >= p_i){ // Tasks of lower or same priority
            if(data[j][1] > max) max = data[j][1]; // Find the longest blocking time
        }
    }
    return max;
}

// Function to compute the RHS of the recurrence relation for task i
double computeRHS(const vector<vector<double> >& data, int num, int i, double Q_i, double B_i, double tau){
    int p_i = data[i][0];
    double RHS = B_i;
    for(int j = 0; j < num; j++){
        if(data[j][0] < p_i){ // Tasks of higher priority
            RHS += ceil((Q_i + tau) / data[j][2]) * data[j][1];
        }
    }
    return RHS;
}

int main(int argc, char* argv[]){
    
    // Check for correct command-line argument usage
    if(argc != 2) {
        cout << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1; // Return a non-zero value to indicate error
    }
    
    ifstream fin(argv[1]);
    if (!fin) {
        cout << "Error opening file: " << argv[1] << endl;
        return 1; // Return a non-zero value to indicate error
    }
    
    int num;
    double tau;
    fin >> num >> tau;

    vector<vector<double> > data(num, vector<double>(3));
    
    for(int i = 0; i < num; i++){
        fin >> data[i][0] >> data[i][1] >> data[i][2];
    }

    vector<double> B(num); 
    for(int i = 0; i < num; i++){
        B[i] = computeBi(data, num, i);
    }
    double Q_i, RHS, R_i;
    for(int i = 0; i < num; i++){
        Q_i = B[i]; // Initially assume Q_i = B_i
        while (true){
            RHS = computeRHS(data, num, i, Q_i, B[i], tau);
            if (RHS + data[i][1] > data[i][2]) { // Check if the task is unschedulable
                cout << "Task " << i << " is unschedulable." << endl;
                break;
            }
            if (RHS != Q_i) {
                Q_i = RHS; // Update Q_i for the next iteration
            } else {
                R_i = RHS + data[i][1];
                cout << "Response time of Task " << i << ": " << R_i << endl; 
                break;
            }
        } 
    }
    return 0;
}
