/*
 * algorithm: preorder list
 * author: maros vasilisin
 */

#include <mpi.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>
#include <array>
#include <string>
#include <climits>
#include <list>

using namespace std;

#define TAG 0
#define MAX_NODE_NUMBER 50

typedef struct t_edge {
    char src;
    char dest;
    bool has_next;
} t_edge;

typedef struct t_adj_list {
    char src;
    char dest[MAX_NODE_NUMBER];
    int dest_count;
} t_adj_list;

typedef struct t_tree {
    char vertices[MAX_NODE_NUMBER];
    int vertices_count;
    t_adj_list adj_lists[MAX_NODE_NUMBER];
    int adj_lists_count;
    t_edge edges[MAX_NODE_NUMBER];
    int edges_count;
} t_tree;

t_tree tree;

void add_edge(char src, char dest) {

    for (int i = 0; i < tree.vertices_count; i++) {
        if (src == tree.vertices[i]) {
            for (int j = 0; j < tree.adj_lists_count; j++) {
                if (src == tree.adj_lists[j].src) {
                    tree.adj_lists[j].dest[tree.adj_lists[j].dest_count] = dest;
                    tree.adj_lists[j].dest_count++;
                }
                if (dest == tree.adj_lists[j].src) {
                    tree.adj_lists[j].dest[tree.adj_lists[j].dest_count] = src;
                    tree.adj_lists[j].dest_count++;
                }
            }
        }
    }

}

void create_edges() {

    for (int i = 0; i < tree.adj_lists_count; i++) {
        for (int j = 0; j < tree.adj_lists[i].dest_count; j++) {
            t_edge edge;
            edge.has_next = (j+1 == tree.adj_lists[i].dest_count) ? false : true;
            edge.src = tree.adj_lists[i].src;
            edge.dest = tree.adj_lists[i].dest[j];
            tree.edges[tree.edges_count] = edge;
            tree.edges_count++;
        }
    }
}

void init_tree(char* input) {
    
    tree.vertices_count = 0;
    tree.adj_lists_count = 0;
    for (int i = 1; i <= strlen(input); i++) {
        tree.vertices[i-1] = input[i-1];
        tree.vertices_count++;
        t_adj_list adj_list;
        adj_list.src = input[i-1];
        adj_list.dest_count = 0;
        tree.adj_lists[i-1] = adj_list;
        tree.adj_lists_count++;
        tree.edges_count = 0;
    }
    for (int i = 1; i <= strlen(input); i++) {
        if (2*i <= strlen(input)) {
            add_edge(input[i-1], input[2*i-1]);
        }
        if (2*i+1 <= strlen(input)) {
            add_edge(input[i-1], input[2*i]);            
        }
    }
    create_edges();
}

t_edge find_reversed_edge(t_edge edge) {

    for (int i = 0; i < tree.edges_count; i++) {
        if (tree.edges[i].src == edge.dest && tree.edges[i].dest == edge.src) {
            cout << "reversed edge for: " << edge.src << "->" << edge.dest << " is " << tree.edges[i].src << "->" << tree.edges[i].dest << endl;
            return tree.edges[i];
        }
    }
    t_edge clean;
    return clean;
}

int get_edge_number(t_edge edge) {

    for (int i = 0; i < tree.edges_count; i++) {
        if (tree.edges[i].src == edge.src && tree.edges[i].dest == edge.dest) {
            return i;
        }
    }
    return 0; 
}

t_edge get_adj_list_first_element(char src) {

    for (int i = 0; i < tree.adj_lists_count; i++) {
        if (tree.adj_lists[i].src == src) {
            t_edge edge;
            edge.src = src;
            edge.dest = tree.adj_lists[i].dest[0]; 
            return edge;
        }
    }
    t_edge clean;
    return clean;
}

t_edge get_next(t_edge edge) {

    for (int i = 0; i < tree.adj_lists_count; i++) {
        if (tree.adj_lists[i].src == edge.src) {
            for (int j = 0; j < tree.adj_lists[i].dest_count; j++) {
                if (edge.dest == tree.adj_lists[i].dest[j]) {
                    t_edge next_edge;
                    next_edge.src = edge.src;
                    next_edge.dest = tree.adj_lists[i].dest[j+1]; 
                    return next_edge;
                }
            }
        }
    }
    t_edge clean;
    return clean;
}

int main(int argc, char* argv[]) {
    
    int processor_count;
    int my_process_id;
    MPI_Status stat; 

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &processor_count); 
    MPI_Comm_rank(MPI_COMM_WORLD, &my_process_id);

    int my_edge_number;

    char* input = argv[1];

    if (my_process_id == 0) {
        // cout << "AHOJ" << endl;

        init_tree(input);

        for (int i = 0; i < tree.edges_count; i++) {
            cout << tree.edges[i].src << "->" << tree.edges[i].dest << endl;
        }
        cout << "LIST: " << tree.adj_lists[0].dest << endl;
        cout << "LIST: " << tree.adj_lists[1].dest << endl;
        cout << "LIST: " << tree.adj_lists[2].dest << endl;
        cout << "LIST: " << tree.adj_lists[3].dest << endl;
        cout << "LIST: " << tree.adj_lists[4].dest << endl;
        cout << "LIST: " << tree.adj_lists[5].dest << endl;
        cout << "LIST: " << tree.adj_lists[6].dest << endl;

        int proc_number = 0;

        for (int i = 0; i < tree.edges_count; i++) {

            int edge_number = i;
            MPI_Send(&edge_number, 1, MPI_INT, proc_number, TAG, MPI_COMM_WORLD);
            proc_number++;
        }
    }

    MPI_Recv(&my_edge_number, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);

    t_edge my_edge = tree.edges[my_edge_number];
    cout << "Processor: " << my_process_id << " has edge number: " << my_edge_number << endl;
    t_edge my_reversed_edge = find_reversed_edge(my_edge);
    cout << "Processor: " << my_process_id << " has edge: " << my_edge.src << "->" << my_edge.dest << endl;

    bool is_next = my_reversed_edge.has_next;

    int my_e_tour_number = 0;
    if (!is_next) {
        t_edge my_e_tour = get_adj_list_first_element(my_reversed_edge.src);
        my_e_tour_number = get_edge_number(my_e_tour);
    }
    else {
        t_edge my_e_tour = get_next(my_reversed_edge);
        my_e_tour_number = get_edge_number(my_e_tour);
    }

    t_edge e_tour[MAX_NODE_NUMBER];

    for (int i = 1; i < processor_count; i++) {
        if (my_process_id == i) {
            MPI_Send(&my_e_tour_number, 1, MPI_INT, 0, TAG,  MPI_COMM_WORLD);
        }
        if (my_process_id == 0) {
            int received_e_tour_number;
            MPI_Recv(&received_e_tour_number, 1, MPI_INT, i, TAG, MPI_COMM_WORLD, &stat);
            e_tour[i] = tree.edges[received_e_tour_number];
        }
    }

    if (my_process_id == 0) {
        e_tour[0] = tree.edges[my_e_tour_number];

        cout << "*******************" << endl;
        for (int i = 0; i < processor_count; i++) {
            cout << e_tour[i].src << "->" << e_tour[i].dest << endl;
        }
    }

    MPI_Finalize(); 
    return 0;

}

