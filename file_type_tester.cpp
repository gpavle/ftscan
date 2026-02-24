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
using std::ifstream;
using std::error_code;
using std::vector;

enum FileType{

    ELF = 282584257676671,
    PNG = 727905341920923785

};


bool verbose {false};

void check_type(const path &directory_name, const FileType &filetype){

    error_code ec;

    for(const auto &file : directory_iterator(directory_name, ec)){
        ifstream ifile{file.path().string(), std::ios::binary};
        if(ifile.is_open()){
            long long magic_number {0};
            ifile.read(reinterpret_cast<char*>(&magic_number), 8);

            if(magic_number == filetype){
                cout<<file.path().string()<<endl;
            }

            ifile.close();
                
        }
        else if(verbose){
            cout<<"Failed to open file: "<<file.path().filename().generic_string();
        }

    }
     if(ec){
            cout<<ec.message()<<endl;
            return;
        }
}

void print_help(){

    cout<<endl;
    cout<<"Usage: file_type_tester [directory path] [file type] (-v)"<<endl;
    cout<<"Currently supported file types: "<<endl;
    cout<<"Elf(use -e in file type)"<<endl;

}

void check_args(const vector<string> &args){

    if(args.size() == 4){
        if(args.at(3) == "-v"){
            verbose = true;
        }
        else{
            cout<<"Invalid argument";
            return;
        }}
    

    if(args.size() == 1){
        cout<<"Too little arguments, type file_type_tester -h for help";
    }


    else if(args.size() == 2){
        if(args.at(1) == "-h")
            print_help();
        else{
            cout<<"Invalid argument";
            return;
        }
    }

    else if(args.size() == 3 || args.size() == 4){
        if(args.at(1) == "-e")
            check_type(args.at(2), FileType::ELF);
        else if(args.at(1) == "-p")
            check_type(args.at(2), FileType::PNG);

        else{
            cout<<"Invalid argument";
        }
        
    }
    else{
        cout<<"Too many arguments, use -h for help";
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