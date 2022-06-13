#include <iostream>
#include <cmath>

using namespace std;

#define graph_t unsigned long long int

void lcd_graph_gen(
        graph_t vertex_number,
        bool is_oriented,
        graph_t &edge_number,
        graph_t *&src_ids,
        graph_t *&dest_ids
) {
    graph_t *chord_array = new graph_t[2 * vertex_number + 1];

    graph_t free_vertices_number = 2 * vertex_number;
    graph_t *free_vertices = new graph_t[2 * vertex_number + 1];

    for (graph_t i = 1; i <= 2 * vertex_number; i++) {
        free_vertices[i] = i;
        chord_array[i] = -1;
    }

    srand(time(NULL));

    /*
     * Generating chords
     */

    for (graph_t i = 1; i <= vertex_number; i++) {
        graph_t vertex1 = rand() % free_vertices_number + 1;
        graph_t vertex2 = rand() % free_vertices_number + 1;

        while (free_vertices[vertex1] == free_vertices[vertex2]) {
            vertex1 = rand() % free_vertices_number + 1;
            vertex2 = rand() % free_vertices_number + 1;
        }

        chord_array[free_vertices[vertex1]] = free_vertices[vertex2];
        chord_array[free_vertices[vertex2]] = free_vertices[vertex1];
        free_vertices[max(vertex1, vertex2)] = free_vertices[free_vertices_number];
        free_vertices[min(vertex1, vertex2)] = free_vertices[free_vertices_number - 1];
        free_vertices_number -= 2;
    }

    /*
     * Mapping vertices from chords to actual vertices
     */

    graph_t *vertex_mapping = new graph_t[2 * vertex_number + 1];
    graph_t vertex_counter = 1;
    for (graph_t i = 1; i <= 2 * vertex_number; i++) {
        vertex_mapping[i] = vertex_counter;
        if (i > chord_array[i]) {
            vertex_counter++;
        }
    }

    /*
     * Counting edges number
     */

    edge_number = 0;
    for (graph_t i = 1; i <= 2 * vertex_number; i++) {
        if (vertex_mapping[i] != vertex_mapping[chord_array[i]] && (i < chord_array[i] || !is_oriented)) {
            edge_number++;
        }
    }

    /*
     * Writing edges to given arrays
     */

    src_ids = new graph_t[edge_number];
    dest_ids = new graph_t[edge_number];

    graph_t edge_counter = 0;

    for (graph_t i = 1; i <= 2 * vertex_number; i++) {
        if (vertex_mapping[i] != vertex_mapping[chord_array[i]] && (i < chord_array[i] || !is_oriented)) {
            src_ids[edge_counter] = vertex_mapping[i];
            dest_ids[edge_counter] = vertex_mapping[chord_array[i]];
            edge_counter++;
        }
    }
}

void print_graph_edges(graph_t edge_number, graph_t *src_ids, graph_t *dest_ids) {
    for (int i = 0; i < edge_number; i++) {
        cout << src_ids[i] << " -> " << dest_ids[i] << endl;
    }
}

void test_lcd_graph_gen() {
    graph_t vertex_number;
    bool is_oriented;

    cout << "Vertex number: ";
    cin >> vertex_number;
    cout << "Is graph oriented(0 - false, 1 - true): ";
    cin >> is_oriented;

    graph_t edge_number;
    graph_t *src_ids;
    graph_t *dest_ids;

    lcd_graph_gen(vertex_number, is_oriented, edge_number, src_ids, dest_ids);

//    print_graph_edges(edge_number, src_ids, dest_ids);

    delete[] src_ids;
    delete[] dest_ids;
}

int main() {
    /*
     * Remove comment to test with vertex number input
     */
    test_lcd_graph_gen();

    return 0;
}