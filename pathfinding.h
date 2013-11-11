



void find_adjacent_squares(Node *node_start, PF_List *current_list, PF_List *closed_list, PF_List *invalid_list);
int is_diag(Node* node1, Node* node2);
int calc_G(Node* parent_node, Node* node);
int calc_H(Node* node_start, Node* node_end);
void calc_nodes_GHF(Node* selected_node, Node *node_end, PF_List *lst);
void check_g_score(Node* selected_node, PF_List *current_list, PF_List *open_list);
void get_path(Node* node_start, PF_List *closed_list, PF_List *result);
void find_shortest_path(Node *node_start, Node *node_end, PF_List *invalid_list, PF_List *result);
