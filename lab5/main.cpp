#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>

using namespace std;

class SuffixTree {
private:
    struct Node {
        int start;
        int end;
        Node* suffix_link;
        std::unordered_map<char, Node*> children;
        
        Node(int start = -1, int end = -1) 
            : start(start), end(end), suffix_link(nullptr) {}
    };
    
    struct ActivePoint {
        Node* node;
        int edge;
        int length;
        
        ActivePoint() : node(nullptr), edge(-1), length(0) {}
    };

    Node* root;
    ActivePoint active;
    int remaining;
    int global_end;
    std::string text;
    
    int edge_length(Node* node) {
        return std::min(node->end, global_end + 1) - node->start;
    }
    
    bool walk_down(Node* node) {
        int edge_len = edge_length(node);
        
        if (active.length >= edge_len) {
            active.edge += edge_len;
            active.length -= edge_len;
            active.node = node;
            return true;
        }
        return false;
    }
    
    void extend_tree(int pos) {
        global_end = pos;
        remaining++;
        Node* last_created_internal_node = nullptr;
        
        while (remaining > 0) {
            if (active.length == 0) {
                active.edge = pos;
            }
            
            char current_char = text[pos];
            auto it = active.node->children.find(text[active.edge]);
            
            if (it == active.node->children.end()) {
                Node* new_node = new Node(pos, text.length());
                active.node->children[text[active.edge]] = new_node;
                
                if (last_created_internal_node != nullptr) {
                    last_created_internal_node->suffix_link = active.node;
                    last_created_internal_node = nullptr;
                }
            } else {
                Node* next_node = it->second;

                if (walk_down(next_node)) {
                    continue;
                }

                if (text[next_node->start + active.length] == current_char) {
                    if (last_created_internal_node != nullptr && active.node != root) {
                        last_created_internal_node->suffix_link = active.node;
                    }
                    active.length++;
                    break;
                }

                Node* split_node = new Node(next_node->start, next_node->start + active.length);
                Node* new_leaf = new Node(pos, text.length());
                
                next_node->start += active.length;

                split_node->children[text[next_node->start]] = next_node;
                split_node->children[current_char] = new_leaf;

                active.node->children[text[active.edge]] = split_node;

                if (last_created_internal_node != nullptr) {
                    last_created_internal_node->suffix_link = split_node;
                }
                last_created_internal_node = split_node;
            }
            
            remaining--;
            
            if (active.node == root && active.length > 0) {
                active.length--;
                active.edge = pos - remaining + 1;
            } else if (active.node != root) {
                active.node = (active.node->suffix_link != nullptr) ? 
                             active.node->suffix_link : root;
            }
        }
    }
    
    void delete_subtree(Node* node) {
        if (node == nullptr) return;
        
        for (auto& child : node->children) {
            delete_subtree(child.second);
        }
        delete node;
    }
    
    void print_tree(Node* node, int depth = 0) {
        if (node == nullptr) return;
        
        for (int i = 0; i < depth; i++) {
            std::cout << "  ";
        }
        
        std::cout << "Node(" << node->start << ", " 
                  << (node->end == text.length() ? "END" : std::to_string(node->end)) 
                  << ")";
        
        if (node->suffix_link != nullptr) {
            std::cout << " [suffix_link]";
        }
        std::cout << std::endl;
        
        for (auto& child : node->children) {
            for (int i = 0; i < depth + 1; i++) {
                std::cout << "  ";
            }
            std::cout << "Edge '" << child.first << "' -> ";
            print_tree(child.second, depth + 2);
        }
    }

public:
    SuffixTree() : root(new Node()), remaining(0), global_end(-1) {
        active.node = root;
    }
    
    ~SuffixTree() {
        delete_subtree(root);
    }
   
    void build(const std::string& input_text) {
        text = input_text + "$";
        global_end = -1;
        remaining = 0;
        active = ActivePoint();
        active.node = root;
        
        for (int i = 0; i < text.length(); i++) {
            extend_tree(i);
        }
    }

    void print() {
        std::cout << "Suffix Tree for: " << text << std::endl;
        print_tree(root);
    }
private:
    struct staff
    {
        int counter = 0;
        Node* prev_node;
        Node* current_node;
        int edge_start = -1;
        int edge_pos = 0;
        int j = 0;
    };

    staff traversal;
    void start_traversal() {
        traversal.counter = 0;
        traversal.prev_node = root;
        traversal.current_node = root;
        traversal.edge_start = -1;
        traversal.edge_pos = 0;
    }

public:
    int global_counter = 0;
    int downgrade(int a){
        a -= 1;
        if (a < 0) return 0;
        return a;
    }

    void go_back(char c, vector<int>& stat){
        if (traversal.edge_start != -1){
            Node* suffix_target = (traversal.prev_node->suffix_link != nullptr) 
                                    ? traversal.prev_node->suffix_link 
                                    : root;
            traversal.current_node = traversal.prev_node;
            if (traversal.current_node == root){
                if (global_end == (traversal.edge_start + 1)){
                    traversal.prev_node = root;
                    traversal.current_node = root;
                    traversal.edge_pos = 0;
                    traversal.edge_start = -1;
                    feed(c, stat);
                } else {
                    traversal.edge_pos -= 1;
                    if (traversal.edge_pos == 0){
                        traversal.prev_node = root;
                        traversal.current_node = root;
                        traversal.edge_pos = 0;
                        traversal.edge_start = -1;
                        feed(c, stat);
                    } else {
                        traversal.prev_node = traversal.current_node;
                        traversal.current_node = traversal.current_node->children[text[traversal.edge_start + 1]];
                        traversal.edge_start += 1;
                        feed(c, stat);
                    }
                }
            } else {
                traversal.current_node = suffix_target;
                traversal.prev_node = suffix_target;
                traversal.current_node = traversal.current_node->children[text[traversal.edge_start]];
                feed(c, stat);
            }
        } else {
            Node* suffix_target = (traversal.current_node->suffix_link != nullptr) 
                                    ? traversal.current_node->suffix_link 
                                    : root;
            traversal.current_node = suffix_target;
            feed(c, stat);
        }
    }

    void feed(char c, vector<int>& stat){
        global_counter += 1;
        if ((traversal.edge_start == -1) || (traversal.edge_pos == 0)) {
            traversal.edge_pos = 0;
            traversal.edge_start = -1;
            if (traversal.current_node->children.find(c) != traversal.current_node->children.end()){
                traversal.prev_node = traversal.current_node;
                traversal.edge_start = traversal.current_node->children[c]->start;
                traversal.current_node = traversal.current_node->children[c];
                traversal.edge_pos = 1;
                traversal.counter += 1;
            } else {
                stat[traversal.j] = traversal.counter;
                traversal.j += 1;
                traversal.counter = downgrade(traversal.counter);
                if (traversal.current_node == root){
                    if (root->children.find(c) == root->children.end()) {
                        traversal.edge_start = -1;
                        traversal.edge_pos = 0;
                        traversal.prev_node = root;
                        traversal.current_node = root;
                    } else {
                        feed(c, stat);
                    }
                } else {
                    go_back(c, stat);
                }
            }
        } else {
            bool stop = false;
            while (!stop){
                int edge_len = edge_length(traversal.current_node);
                if (traversal.edge_pos == edge_len) {
                    bool stop = true;
                    traversal.edge_start = -1;
                    traversal.edge_pos = 0;
                    feed(c, stat);
                    break;
                } else if (traversal.edge_pos > edge_len) {
                    traversal.edge_pos -= edge_len;
                    traversal.prev_node = traversal.current_node;
                    traversal.current_node = traversal.current_node->children[text[traversal.edge_start + edge_len]];
                    traversal.edge_start += edge_len;
                }  else {
                    bool stop = true;
                    if (text[traversal.edge_start + traversal.edge_pos] == c){
                        traversal.edge_pos += 1;
                        traversal.counter += 1;
                    } else {
                        stat[traversal.j] = traversal.counter;
                        traversal.j += 1;
                        traversal.counter = downgrade(traversal.counter);
                        go_back(c, stat);
                        
                    }
                    break;
                }
            }
        }
    }

    vector<int> search(string T){
        vector<int> T_stat(T.length(), 0); 

        int fin_pos = T.length();
        //fin_pos = 5;
        start_traversal();
        for (size_t i = 0; i < fin_pos; i++)
        {
            char c = T[i];
            feed(c, T_stat);
        }
        for (size_t i = 0; i < traversal.counter; i++)
        {
            T_stat[T.length() - (traversal.counter - i)] = (traversal.counter - i);
        }
        vector<int> finale;
        for (size_t i = 0; i < T_stat.size(); i++)
        {
            if (T_stat[i] == (text.length() - 1)) {
                finale.push_back(i + 1);
            }
        }
        return finale;
    };
};

std::vector<int> findPatternPositions(const std::string& image, const std::string& text) {
    if ((image == "") && (text == "")){
        vector<int> a(1, 1);
        return  a;
    }
    vector<int> stat(text.length(), 0);
    SuffixTree tree;
    tree.build(image);
    return tree.search(text);
}

int main(){
    string image = "", text = "";
    cin >> image >> text;
    vector<int> stat(text.length(), 0);
    stat = findPatternPositions(image, text);
    for (size_t i = 0; i < stat.size(); i++)
    {
        cout << stat[i] << endl;
    }
}

