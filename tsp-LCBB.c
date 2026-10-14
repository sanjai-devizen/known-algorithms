#include <stdio.h>
#include <stdlib.h>

#define INF 999
#define N 4 // Number of cities

int global_upper_bound = INF; // Stores the cost of the best complete tour found

typedef struct {
    int vertex;
    int level;
    int bound;
    int matrix[N][N];
    int path[N + 1];
} Node;

// Reduces the matrix and returns the reduction cost
int reduce_matrix(int matrix[N][N]) {
    int reduction_cost = 0;

    // 1. Row Reduction
    for (int i = 0; i < N; i++) {
        int min_val = INF;
        for (int j = 0; j < N; j++) {
            if (matrix[i][j] < min_val) min_val = matrix[i][j];
        }
        if (min_val != INF && min_val > 0) {
            reduction_cost += min_val;
            for (int j = 0; j < N; j++) {
                if (matrix[i][j] != INF) matrix[i][j] -= min_val;
            }
        }
    }

    // 2. Column Reduction
    for (int j = 0; j < N; j++) {
        int min_val = INF;
        for (int i = 0; i < N; i++) {
            if (matrix[i][j] < min_val) min_val = matrix[i][j];
        }
        if (min_val != INF && min_val > 0) {
            reduction_cost += min_val;
            for (int i = 0; i < N; i++) {
                if (matrix[i][j] != INF) matrix[i][j] -= min_val;
            }
        }
    }

    return reduction_cost;
}

// Emulates a true LC priority queue extraction (finds node with lowest bound)
int get_lc_node(Node queue[], int queue_size) {
    int min_idx = 0;
    for (int i = 1; i < queue_size; i++) {
        if (queue[i].bound < queue[min_idx].bound) {
            min_idx = i;
        }
    }
    return min_idx;
}

void solve_tsp_lcbb(int cost_matrix[N][N]) {
    Node queue[100];
    int queue_size = 0;

    // Create Root Node (Starting at City 0 / A)
    Node root;
    root.vertex = 0;
    root.level = 0;
    root.path[0] = 0;
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) root.matrix[i][j] = cost_matrix[i][j];
    }
    
    root.bound = reduce_matrix(root.matrix);
    queue[queue_size++] = root;

    while (queue_size > 0) {
        // Pop the Least Cost node
        int lc_idx = get_lc_node(queue, queue_size);
        Node curr = queue[lc_idx];
        
        // Remove from queue
        for (int i = lc_idx; i < queue_size - 1; i++) queue[i] = queue[i + 1];
        queue_size--;

        // If current bound is worse than best solution found, prune it
        if (curr.bound >= global_upper_bound) continue;

        // Leaf Node reached (All cities visited)
        // curr.bound already includes the return edge cost because it wasn't blocked!
        if (curr.level == N - 1) {
            curr.path[N] = 0; // Complete the cycle back to start
            int final_cost = curr.bound; 
            
            if (final_cost < global_upper_bound) {
                global_upper_bound = final_cost;
                printf("New best tour found with cost %d: ", global_upper_bound);
                for (int i = 0; i <= N; i++) printf("%c ", curr.path[i] + 'A');
                printf("\n");
            }
            continue;
        }

        // Branch out to all other unvisited cities
        for (int next_vert = 0; next_vert < N; next_vert++) {
            if (curr.matrix[curr.vertex][next_vert] != INF) {
                
                Node child;
                child.vertex = next_vert;
                child.level = curr.level + 1;
                
                // Copy path history and add new city
                for (int i = 0; i <= curr.level; i++) child.path[i] = curr.path[i];
                child.path[child.level] = next_vert;

                // Copy matrix state
                for (int i = 0; i < N; i++) {
                    for (int j = 0; j < N; j++) child.matrix[i][j] = curr.matrix[i][j];
                }

                // Apply constraints for the chosen edge (curr.vertex -> next_vert)
                int edge_cost = child.matrix[curr.vertex][next_vert];
                
                for (int k = 0; k < N; k++) {
                    child.matrix[curr.vertex][k] = INF; // Can't leave current city again
                    child.matrix[k][next_vert] = INF;  // Can't enter next city again
                }
                
                // Only block immediate return if we are NOT visiting the final city
                if (child.level < N - 1) {
                    child.matrix[next_vert][0] = INF; 
                }

                // Compute child bound
                int reduction_cost = reduce_matrix(child.matrix);

                child.bound = curr.bound + edge_cost + reduction_cost;

                // Add to queue if it's promising
                if (child.bound < global_upper_bound) {
                    queue[queue_size++] = child;
                }
            }
        }
    }
}

int main() {
    int cost_matrix[N][N] = {
        { INF,  10,  15,  20 },
        {   5, INF,   9,  10 },
        {   6,  13, INF,  12 },
        {   8,   8,   9, INF }
    };

    printf("Starting Traveling Salesman LCBB Optimization...\n");
    solve_tsp_lcbb(cost_matrix);
    
    printf("\nAbsolute Min Cost Tour Value: %d\n", global_upper_bound);
    return 0;
}
