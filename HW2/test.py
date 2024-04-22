import random
import math
from copy import deepcopy
random.seed(11451401)
def read_input_file(filename):
    with open(filename, 'r') as file:
        n = int(file.readline().strip())
        tau = float(file.readline().strip())
        messages = []
        for _ in range(n):
            line = file.readline().strip().split()
            priority, transmission_time, period = map(float, line)
            messages.append({'priority': priority, 'transmission_time': transmission_time, 'period': period})
    return n, tau, messages

def calculate_bi(messages):
    n = len(messages)
    bi_values = []
    for i in range(n):
        max_transmission_time = 0
        for j in range(n):
            if messages[j]['priority'] >= messages[i]['priority']:
                max_transmission_time = max(max_transmission_time, messages[j]['transmission_time'])
        bi_values.append(max_transmission_time)
    return bi_values

def simulated_annealing(messages, initial_temp, cooling_rate, stopping_temp):
    def calculate_total_response_time(messages):
        n = len(messages)
        B = calculate_bi(messages)
        total_response_time = 0
        for i in range(n):
            Qi = B[i]
            while True:
                RHS = B[i]
                for j in range(n):
                    if messages[j]['priority'] < messages[i]['priority']:
                        RHS += math.ceil((Qi+tau)/messages[j]['period']) * messages[j]['transmission_time']
                if RHS + messages[i]['transmission_time'] > messages[i]['period']:
                    return float('inf')  # Not schedulable, return inf
                elif Qi == RHS:
                    Ri = Qi + messages[i]['transmission_time']
                    total_response_time += Ri
                    break
                else:
                    Qi = RHS
        return total_response_time

    def get_neighbor(state):
        neighbor = deepcopy(state)
        i, j = random.sample(range(len(neighbor)), 2)
        neighbor[i]['priority'], neighbor[j]['priority'] = neighbor[j]['priority'], neighbor[i]['priority']
        return neighbor
    current_state = deepcopy(messages)
    current_temp = initial_temp
    best_state = deepcopy(current_state)
    best_response_time = calculate_total_response_time(current_state)
    while current_temp > stopping_temp:
        neighbor = get_neighbor(current_state)
        current_response_time = calculate_total_response_time(current_state)
        neighbor_response_time = calculate_total_response_time(neighbor)
        # If the new state is better, accept it, otherwise accept it with a certain probability that decreases with temperature
        if (current_response_time == float('inf') or (neighbor_response_time < current_response_time or
                random.uniform(0, 1) < math.exp((current_response_time - neighbor_response_time) / current_temp))):
            current_state = neighbor

        if neighbor_response_time < best_response_time:
            best_state = neighbor
            best_response_time = neighbor_response_time

        # Cool down the temperature
        current_temp *= (1 - cooling_rate)

    return best_state, best_response_time

filename = 'input.dat' # you must change this to your target input file!
n, tau, messages = read_input_file(filename)

# you can change the following value in case you found the algorithm is too slow
initial_temp = 100000
cooling_rate = 0.002
stopping_temp = 1
best_response_time = float('inf')
best_state, best_response_time = simulated_annealing(messages, initial_temp, cooling_rate, stopping_temp)

# Output the result
for item in best_state:
    print(int(item['priority']))
print(f"{round(best_response_time, 2)}")