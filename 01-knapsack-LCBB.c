#include <stdio.h>
#include <stdlib.h>

#define MAX_NODES 1000

int global_min_cost = 0; // Stores the minimum cost (negative maximum profit)

typedef struct {
    int weight;
    int value; // Treated as negative for cost minimization
    float ratio;
} item;

typedef struct {
    int level;
    int profit;
    int weight;
    float bound;
} Node;

// Bounding function: Calculates the lowest possible cost (best potential profit)
float calculate_bound(Node u, int n, int W, item items[]) {
    if (u.weight >= W) return 0; // Over capacity, invalid

    float profit_bound = u.profit;
    int j = u.level + 1;
    int total_weight = u.weight;

    while ((j < n) && (total_weight + items[j].weight <= W)) {
        total_weight += items[j].weight;
        profit_bound += items[j].value;
        j++;
    }

    // Fractional part if items remain
    if (j < n) {
        profit_bound += (W - total_weight) * items[j].ratio;
    }

    return profit_bound;
}

// Emulates a true LC BB priority queue extraction (Finds node with least bound)
int get_lc_node(Node queue[], int queue_size) {
    int min_idx = 0;
    for (int i = 1; i < queue_size; i++) {
        if (queue[i].bound < queue[min_idx].bound) {
            min_idx = i;
        }
    }
    return min_idx;
}

void knapsack_lc(item items[], int n, int capacity) {
    Node queue[MAX_NODES];
    int queue_size = 0;

    // Initialize root node
    Node u, v;
    u.level = -1;
    u.profit = 0;
    u.weight = 0;
    u.bound = calculate_bound(u, n, capacity, items);
    
    queue[queue_size++] = u;

    while (queue_size > 0) {
        // Extract Least Cost node
        int lc_idx = get_lc_node(queue, queue_size);
        v = queue[lc_idx];
        
        // Remove from queue
        for (int i = lc_idx; i < queue_size - 1; i++) queue[i] = queue[i + 1];
        queue_size--;

        // If the current node's bound is worse than the best found solution, kill it
        if (v.bound > global_min_cost) continue;

        u.level = v.level + 1;
        if (u.level == n) continue;

        // Branch 1: Include Next Item 
        u.weight = v.weight + items[u.level].weight;
        u.profit = v.profit + items[u.level].value;

        if (u.weight <= capacity) {
            // If it's a leaf node or a better solution, update global minimum cost
            if (u.profit < global_min_cost) {
                global_min_cost = u.profit;
            }
            
            u.bound = calculate_bound(u, n, capacity, items);
            if (u.bound < global_min_cost && queue_size < MAX_NODES) {
                queue[queue_size++] = u;
            }
        }

        // Branch 2: Exclude Next Item 
        u.weight = v.weight;
        u.profit = v.profit;
        u.bound = calculate_bound(u, n, capacity, items);

        if (u.bound < global_min_cost && queue_size < MAX_NODES) {
            queue[queue_size++] = u;
        }
    }
}

int main() {
    int n = 4;
    int capacity = 10;
    
    // Values are negative to convert maximization into LC (minimization)
    item items[] = {
        {2, -10, -5.0}, 
        {3, -18, -6.0}, 
        {4, -28, -7.0}, 
        {5, -40, -8.0}
    };

    knapsack_lc(items, n, capacity);

    printf("Max profit calculated: %d\n", -1 * global_min_cost);
    return 0;
}
