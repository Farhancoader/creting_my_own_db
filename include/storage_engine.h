#ifndef STORAGE_ENGINE_H
#define STORAGE_ENGINE_H

#include <map>
#include <string>
#include <fstream>
#include <cstdint>

using namespace std;


class StorageEngine{
private:
    map<std::string, std::string> data;
    string db_file;
public:
    StorageEngine(const string& filename) : db_file(filename){
        load_from_disk();
    }
    ~StorageEngine(){
        flush_to_disk();
    }
    void put(const string& key,const string& value){
        data[key]=value;
    }

    string* get(const string& key){
        auto id = data.find(key);
        if(id!=data.end())return  &id->second;
        return nullptr;
    }

    void flush_to_disk(){
        ofstream file(db_file, std::ios::binary);
        uint32_t num_entries = data.size();
        file.write((const char*)& num_entries,sizeof(num_entries));

        for(const auto [key,value] : data){
            uint32_t key_len = key.length();
            file.write((const char*)&key_len, sizeof(key_len));
            file.write(key.data(), key_len);

            uint32_t val_len = value.length();
            file.write((const char*)&val_len, sizeof(val_len));
            file.write(value.data(), val_len);
        }
        file.close();
    }
    void load_from_disk(){
        ifstream file(db_file, std::ios::binary);
        if (!file.is_open()) {
            return;  // File doesn't exist yet, start fresh
        }
        
        // Read number of entries
        uint32_t num_entries;
        file.read((char*)&num_entries, sizeof(num_entries));
        
        // Read each key-value pair
        for (uint32_t i = 0; i < num_entries; ++i) {
            // Read key
            uint32_t key_len;
            file.read((char*)&key_len, sizeof(key_len));
            std::string key(key_len, '\0');  // Create empty string of length key_len
            file.read(&key[0], key_len);
            
            // Read value
            uint32_t val_len;
            file.read((char*)&val_len, sizeof(val_len));
            std::string value(val_len, '\0');
            file.read(&value[0], val_len);
            
            data[key] = value;
        }
        
        file.close();
    }



};

#endif