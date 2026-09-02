#ifndef BPLUS_TREE_H
#define BPLUS_TREE_H

#include <vector>
#include <string>
#include <memory>
#include <iostream>

using namespace std;

template <typename Key,typename Value>
class BPlusTree;

template <typename Key,typename Value>
struct BPlusNode
{
    vector<Key>keys;
    bool is_leaf;
    shared_ptr<BPlusNode> parent;
    /* data */
    BPlusNode(bool leaf = true) : is_leaf(leaf), parent(nullptr) {}
    virtual ~BPlusNode() {}
};

template <typename Key,typename Value>
struct LeafNode: BPlusNode<Key,Value>{
    vector<Value> data;
    shared_ptr<LeafNode> next;

    LeafNode() : BPlusNode<Key, Value>(true), next(nullptr) {}
};

template <typename Key, typename Value>
struct InternalNode : BPlusNode<Key, Value> {
    vector<shared_ptr<BPlusNode<Key, Value>>> children;  // Pointers to children
    
    InternalNode() : BPlusNode<Key, Value>(false) {}
};

template<typename Key,typename Value>
class BPlusTree{
    private:
        static const int ORDER = 3;
        shared_ptr<BPlusNode<Key, Value>> root;
        shared_ptr<LeafNode<Key, Value>> leftmost_leaf;

    public:
        BPlusTree(){
            auto leaf = make_shared<LeafNode<Key,Value>>();
            root = leaf;
            leftmost_leaf = leaf;
        }
        void insert(const Key &key, const Value &value){
            if(root->is_leaf){
                auto leaf = dynamic_pointer_cast<LeafNode<Key, Value>>(root);
                insert_into_leaf(leaf,key,value);

                if(leaf->keys.size()>=ORDER){
                    split_leaf(leaf);
                }
            }
            else{
                auto leaf = find_leaf(root,key);
                insert_into_leaf(leaf,key,value);

                if (leaf->keys.size() >= ORDER) {
                split_leaf(leaf);
                }
            }
        }
        Value* search(const Key &key){
            auto leaf = find_leaf(root,key);
            for(size_t i =0;i<leaf->keys.size();i++){
                if(leaf->keys[i]==key){
                    return &leaf->data[i];
                }
            }
            return nullptr;
        }
        vector<Value> range_search(const Key & min_key,const Key &max_key){
            vector<Value> result;
            auto current_leaf = find_leaf(root,min_key);
            while(current_leaf){
                for(size_t i =0;i<current_leaf->keys.size();i++){
                    if(current_leaf->keys[i] >= min_key && current_leaf->keys[i] <= max_key){
                        result.push_back(current_leaf->data[i]);
                    }
                    if(current_leaf->keys[i]>max_key)return result;
                }
                current_leaf = current_leaf->next;
            }
            return result;
        }
        void print(){
            print_node(root,0);
        }

    private:
        shared_ptr<LeafNode<Key, Value>> find_leaf(shared_ptr<BPlusNode<Key, Value>> node, const Key &key) {
            if (node->is_leaf) {
                return dynamic_pointer_cast<LeafNode<Key, Value>>(node);
            } else {
                auto internal_node = dynamic_pointer_cast<InternalNode<Key, Value>>(node);
                size_t i = 0;
                while (i < internal_node->keys.size() && key >= internal_node->keys[i]) {
                    i++;
                }
                return find_leaf(internal_node->children[i], key);
            }
        }

        void insert_into_leaf(shared_ptr<LeafNode<Key, Value>> leaf, const Key &key, const Value &value){
            int pos = 0;
            while(pos<leaf->keys.size() && leaf->keys[pos]<key){
                pos++;
        }
         
            leaf->keys.insert(leaf->keys.begin()+pos,key);
            leaf->data.insert(leaf->data.begin()+pos,value);
        }

        void split_leaf(shared_ptr<LeafNode<Key,Value>> leaf){
             int mid = leaf->keys.size()/2;
             auto right_leaf = make_shared<LeafNode<Key,Value>>();
             for(int i =mid;i<leaf->keys.size();i++){
                right_leaf->keys.push_back(leaf->keys[i]);
                right_leaf->data.push_back(leaf->data[i]);
             }
            leaf->keys.erase(leaf->keys.begin() + mid, leaf->keys.end());
            leaf->data.erase(leaf->data.begin() + mid, leaf->data.end());

            right_leaf->next = leaf->next;
            leaf->next = right_leaf;

            Key promoted_key = right_leaf->keys[0];  // First key of right
            insert_into_parent(leaf, promoted_key, right_leaf);
        }

        void insert_into_parent(shared_ptr<BPlusNode<Key,Value>> left,const Key &key,shared_ptr<BPlusNode<Key,Value>> right){
             if (!left->parent) {
                auto new_root = make_shared<InternalNode<Key, Value>>();
                new_root->keys.push_back(key);
            
                auto left_internal = dynamic_pointer_cast<InternalNode<Key, Value>>(new_root);
                left_internal->children.push_back(left);
                left_internal->children.push_back(right);
                
                left->parent = new_root;
                right->parent = new_root;
                
                root = new_root;
                return;
            }
            auto parent = dynamic_pointer_cast<InternalNode<Key, Value>>(left->parent);
        
            int pos = 0;
            while (pos < parent->keys.size() && key > parent->keys[pos]) {
                pos++;
            }
            parent->keys.insert(parent->keys.begin() + pos, key);
            parent->children.insert(parent->children.begin() + pos + 1, right);
        
            right->parent = parent;
        
        if (parent->keys.size() >= ORDER) {
            split_internal(parent);
        }
    }

    void split_internal(shared_ptr<InternalNode<Key, Value>> node){
        int mid = node->keys.size() / 2;
        auto right_internal = make_shared<InternalNode<Key, Value>>();
        for(int i =mid+1;i<node->keys.size();i++){
            right_internal->keys.push_back(node->keys[i]);
        }
        for(int i=mid+1;i<node->children.size();i++){
            right_internal->children.push_back(node->children[i]);
            node->children[i]->parent = right_internal;
        }

        Key promoted_key = node->keys[mid];

        node->keys.erase(node->keys.begin()+mid , node->keys.end());
        node->children.erase(node->children.begin()+mid+1 , node->children.end());

        insert_into_parent(node, promoted_key, right_internal);
    }
    void print_node(shared_ptr<BPlusNode<Key, Value>> node, int depth) {
        if (!node) return;
        
        // Print indentation
        for (int i = 0; i < depth; i++) cout << "  ";
        
        // Print keys
        cout << "[";
        for (size_t i = 0; i < node->keys.size(); i++) {
            if (i > 0) cout << ", ";
            cout << node->keys[i];
        }
        cout << "]";
        
        if (node->is_leaf) {
            cout << " (LEAF)";
        }
        cout << endl;
        
        // Print children if internal node
        if (!node->is_leaf) {
            auto internal = dynamic_pointer_cast<InternalNode<Key, Value>>(node);
            for (auto& child : internal->children) {
                print_node(child, depth + 1);
            }
        }
    }
    
};


#endif