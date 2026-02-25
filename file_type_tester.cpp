#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::filesystem::directory_iterator;
using std::filesystem::path;
using std::filesystem::is_directory;
using std::ifstream;
using std::error_code;
using std::vector;

enum FileType{

    ELF = 282584257676671,
    PNG = 727905341920923785

};


bool recursive {false};
bool verbose{false};

void check_type(const path &directory_name, const FileType &filetype){

    error_code ec;
    static string error_dir;

    for(const auto &file : directory_iterator(directory_name, ec)){
        error_dir = file.path().string();
        if(recursive && is_directory(file.path()))
            check_type(file.path(), filetype);
        ifstream ifile{file.path().string(), std::ios::binary};
        if(ifile.is_open()){
            long long magic_number {0};
            ifile.read(reinterpret_cast<char*>(&magic_number), 8);

             if(magic_number == filetype){
                cout<<file.path().string()<<endl;
            }

            ifile.close();
                
        }
        else if(verbose && !is_directory(file.path())){
            cout<<"Failed to open file: "<<file.path().generic_string()<<endl;
        }

    }
     if(ec && verbose){
            cout<<"Failed to open directory:"<<error_dir<<" "<<ec.message()<<endl;
            
        }
}

void print_help(){

    cout<<endl;
    cout<<"Usage: file_type_tester (-r/n) [file type] [directory path]  (-v/n)"<<endl;
    cout<<"Currently supported file types: "<<endl;
    cout<<"Elf(use -e in file type)"<<endl;

}

void check_args(const vector<string> &args){

    if(args.size() == 2 && args.at(1) == "-h"){
        print_help();
        return;
    }

     if(args.size() == 5){
        if(args.at(4) == "-v")
            verbose = true;
        if(args.at(1) == "-r")
            recursive = true;
        
        if(args.at(2) == "-e")
            check_type(args.at(3), FileType::ELF);
        
        else if(args.at(2) == "-p")
            check_type(args.at(3), FileType::PNG);
        

    }
    else{
        cout<<"Invalid number of arguments";
    }

}



int main(int argc, char* argv[]){

    vector<string> args;

    for(int i {0}; i < argc;i++){
        args.push_back(argv[i]);
    }

    check_args(args);

    


    return 0;
}
