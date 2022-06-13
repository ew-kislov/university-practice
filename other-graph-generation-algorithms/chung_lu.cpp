#include <iostream>
#include <cmath>
#include "omp.h"

using namespace std;

#define graph_t unsigned long long int

enum Connectivity_Degree { One_Component = 0, Big_Component, Many_Small_Components };

void chung_lu_graph_gen(
        double alpha,
        double beta,
        bool is_oriented,
        graph_t &edge_number,
        graph_t *&src_ids,
        graph_t *&dest_ids
) {
    graph_t max_degree = round(exp(alpha / beta));
    graph_t *degree_array = new graph_t[max_degree + 1];
    graph_t vertex_number = 0;

    edge_number = 0;

    /*
     * Initializing degree array
     * Counting vertex/edge number
     */

    graph_t degree;

    #pragma parallel omp for  num_threads(8) schedule(dynamic, 256) private(degree) shared(degree_array, max_degree, vertex_number, edge_number)
    for (degree = 1; degree <= max_degree; degree++) {
        degree_array[degree] = round(exp(alpha) / pow(degree, beta));

        vertex_number += degree_array[degree];
        edge_number += degree * degree_array[degree];
    }

    edge_number /= 2;

    graph_t *vertex_array = new graph_t[edge_number * 2 + 1];
    graph_t vertex_index = 1;
    graph_t vertex_array_index = 1;

    /*
     * Initializing vertex_array - array which contains each vertex with number of degree
     */

    graph_t local_vertex_index;
    graph_t vertex_edge;

    #pragma parallel omp for  num_threads(8) schedule(dynamic, 256) private(degree) shared(vertex_array, degree_array, max_degree, vertex_array_index)
    for (degree = 1; degree <= max_degree; degree++) {
        for (local_vertex_index = 1; local_vertex_index <= degree_array[degree]; local_vertex_index++) {
            for (vertex_edge = 1; vertex_edge <= degree; vertex_edge++) {
                vertex_array[vertex_array_index] = vertex_index;
                vertex_array_index++;
            }
            vertex_index++;
        }
    }

    if (is_oriented) {
        src_ids = new graph_t[edge_number];
        dest_ids = new graph_t[edge_number];
    } else {
        src_ids = new graph_t[2 * edge_number];
        dest_ids = new graph_t[2 * edge_number];
    }

    /*
     * Randomly generating edges
     */

    long long int edge_iterator;
    graph_t vertex1;
    graph_t vertex2;

    #pragma omp parallel num_threads(8) private(edge_iterator) shared(vertex1, vertex2, vertex_array, src_ids, dest_ids, edge_number, is_oriented)
    {
        graph_t seed = int(time(NULL)) * omp_get_thread_num();
        srand(seed);

        #pragma omp for schedule(dynamic, 256)
        for (edge_iterator = edge_number; edge_iterator > 0; edge_iterator--) {
            vertex1 = rand() % edge_iterator * 2 + 1;
            vertex2 = rand() % edge_iterator * 2 + 1;

            if (edge_iterator == 1 && vertex_array[vertex1] == vertex_array[vertex2]) {
                /*
                 * If two left vertexes are the same - finish the loop
                 */

                #pragma omp critical
                {
                    edge_number--;
                }
            } else {
                while (vertex_array[vertex1] == vertex_array[vertex2]) {
                    vertex1 = rand() % edge_iterator + 1;
                    vertex2 = rand() % edge_iterator + edge_iterator + 1;
                }

                src_ids[edge_number - edge_iterator] = vertex_array[vertex1];
                dest_ids[edge_number - edge_iterator] = vertex_array[vertex2];

                if (!is_oriented) {
                    src_ids[2 * edge_number - edge_iterator - 1] = vertex_array[vertex2];
                    dest_ids[2 * edge_number - edge_iterator - 1] = vertex_array[vertex1];
                }

                /*
                 * Replacing chosen vertexes with two last vertexes
                 */

                vertex_array[vertex1] = vertex_array[edge_iterator * 2];
                vertex_array[vertex2] = vertex_array[edge_iterator * 2 - 1];
            }
        }
    }

    if (!is_oriented) {
        edge_number *= 2;
    }
}


void chung_lu_graph_gen(
        graph_t  desired_vertex_number,
        Connectivity_Degree connectivity_degree,
        bool is_oriented,
        graph_t  &edge_number,
        graph_t *&src_ids,
        graph_t *&dest_ids
) {
    /*
     * Randomly generating beta
     */

    long double min_beta;
    long double max_beta;

    if (connectivity_degree == One_Component) {
        min_beta = 0.5;
        max_beta = 1;
    } else if (connectivity_degree == Big_Component) {
        min_beta = 1;
        max_beta = 3;
    } else if (connectivity_degree == Many_Small_Components) {
        min_beta = 3;
        max_beta = 6;
    } else {
        cout << "Wrong Connectivity parameter" << endl;
        exit;
    }

    srand(time(NULL));

    long double beta = ((double) rand() / RAND_MAX) * (max_beta - min_beta) + min_beta;

    /*
     * Finding alpha with binary search
     */

    long double left_alpha = 0;
    long double right_alpha = 20;
    long double alpha = (left_alpha + right_alpha) / 2.0;

    graph_t max_degree;
    graph_t vertex_number = 0;

    while (desired_vertex_number != vertex_number) {
        vertex_number = 0;
        max_degree = round(exp(alpha / beta));

        for (graph_t degree = 1; degree <= max_degree; degree++) {
            vertex_number += round(exp(alpha) / pow(degree, beta));
        }

        long long int vertex_diff = vertex_number - desired_vertex_number;
        if (vertex_diff == 0) {
            break;
        } else if (vertex_diff > 0) {
            right_alpha = (left_alpha + right_alpha) / 2.0;
            alpha = (left_alpha + right_alpha) / 2.0;
        } else if (vertex_diff < 0) {
            left_alpha = (left_alpha + right_alpha) / 2.0;
            alpha = (left_alpha + right_alpha) / 2.0;
        }
    }

    /*
     * Running Chung-Lu generator for found alpha/beta parameters
     */

    chung_lu_graph_gen(alpha, beta, is_oriented, edge_number, src_ids, dest_ids);
}

void print_graph_edges(
        graph_t edge_number,
        graph_t *src_ids,
        graph_t *dest_ids
) {
    for (graph_t i = 0; i < edge_number; i++) {
        cout << src_ids[i] << " -> " << dest_ids[i] << endl;
    }
}


void test_with_alpha_beta_input() {
    double alpha, beta;
    bool is_oriented;

    graph_t edge_number;
    graph_t *src_ids;
    graph_t *dest_ids;

    cout << "alpha: ";
    cin >> alpha;
    cout << "beta: ";
    cin >> beta;

    chung_lu_graph_gen(alpha, beta, is_oriented, edge_number, src_ids, dest_ids);

//    print_graph_edges(edge_number, src_ids, dest_ids);

    delete[] src_ids;
    delete[] dest_ids;
}

void test_with_vertex_number_input() {
    int connectivity_degree_ord;
    Connectivity_Degree connectivity_degree;
    bool is_oriented;
    graph_t vertex_number;
    graph_t edge_number;
    graph_t *src_ids;
    graph_t *dest_ids;

    cout << "Vertex number: ";
    cin >> vertex_number;
    cout << "Connectivity degree(one component - 0, big component - 1, many small-components - 2): ";
    cin >> connectivity_degree_ord;
    cout << "Is graph oriented(0 - false, 1 - true): ";
    cin >> is_oriented;

    connectivity_degree = static_cast<Connectivity_Degree>(connectivity_degree_ord);

    chung_lu_graph_gen(vertex_number, connectivity_degree, is_oriented, edge_number, src_ids, dest_ids);

//    print_graph_edges(edge_number, src_ids, dest_ids);

    delete[] src_ids;
    delete[] dest_ids;
}

int main() {

    /*
     * Remove comment to test with alpha/beta input
     */
//    test_with_alpha_beta_input();

    /*
     * Remove comment to test with vertex number input
     */
//    test_with_vertex_number_input();
    return 0;
}
