#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstdlib> // Required for rand() and srand()
#include <ctime> // Required for time()

using namespace std;

double computeBi(const vector<vector<double> >& data, int num, int i) {
    double max = 0;
    for (int j = 0; j < num; j++) {
        if (data[j][0] >= data[i][0]) { // Adjusted for clarity
            max = std::max(max, data[j][1]);
        }
    }
    return max;
}

double computeRHS(const vector<vector<double> >& data, int num, int i, double Q_i, double B_i, double tau) {
    double RHS = B_i;
    for (int j = 0; j < num; j++) {
        if (data[j][0] < data[i][0]) { // Adjusted for clarity
            RHS += ceil((Q_i + tau) / data[j][2]) * data[j][1];
        }
    }
    return RHS;
}

void swap_p(vector<vector<double> >& data, int i, int j) {
    swap(data[i][0], data[j][0]); // Utilizing std::swap for priorities
}

int main(int argc, char* argv[]) {
    
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }
    
    ifstream fin(argv[1]);
    if (!fin) {
        cout << "Error opening file: " << argv[1] << endl;
        return 1;
    }
    
    int num;
    double tau;
    fin >> num >> tau;

    vector<vector<double> > data(num, vector<double>(3));
    for (int i = 0; i < num; i++) {
        fin >> data[i][0] >> data[i][1] >> data[i][2];
    }

    // Simulated Annealing
    double T = 100000; // Initial temperature
    double T_min = 1; // Minimum temperature
    double coolingRate = 0.99; // Adjusted for more practical cooling
    srand(static_cast<unsigned int>(time(nullptr))); // Seed the random number generator

    vector<double> bestConfiguration; // For storing the best configuration
    double bestCost = numeric_limits<double>::max();

    while (T > T_min) {
        for (int i = 0; i < num; i++) {
            int m1 = rand() % num;
            int m2 = rand() % num;
            while (m1 == m2) m2 = rand() % num; // Ensure m1 and m2 are different

            swap_p(data, m1, m2); // Try swapping priorities

            double currentCost = 0; // Recalculate cost based on your criteria
            for (int j = 0; j < num; j++) {
                double B_i = computeBi(data, num, j);
                double Q_i = B_i; // Start with B_i
                double RHS = computeRHS(data, num, j, Q_i, B_i, tau);
                if (RHS + data[j][1] > data[j][2]) { // Unfeasible solution
                    currentCost += numeric_limits<double>::max(); // Penalize heavily
                    break;
                }
                currentCost += RHS + data[j][1]; // Update cost with response time
            }

            if (currentCost < bestCost) {
                bestCost = currentCost;
                bestConfiguration = vector<double>(num); // Storing best priorities
                for (int k = 0; k < num; k++) {
                    bestConfiguration[k] = data[k][0];
                }
            } else {
                swap_p(data, m1, m2); // Revert if not better
            }
        }

        T *= coolingRate; // Cooling down
    }

    // Output best configuration and its cost
    cout << "Best configuration priorities:" << endl;
    for (double priority : bestConfiguration) {
        cout << priority << endl;
    }
    cout << "Best cost: " << bestCost << endl;

    return 0;
}
