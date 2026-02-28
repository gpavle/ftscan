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

struct ScanOptions{

    bool recursive;
    bool verbose;


};

enum FileType{

    ELF = 282584257676671,
    PNG = 727905341920923785

};




void check_type(const path &directory_name, const FileType &filetype, const ScanOptions &scan_options){

    error_code ec;
    static string error_dir{directory_name};

    for(const auto &file : directory_iterator(directory_name, ec)){
        error_dir = file.path().string();
        if(scan_options.recursive && is_directory(file.path()))
            check_type(file.path(), filetype, scan_options);
        ifstream ifile{file.path().string(), std::ios::binary};
        if(ifile.is_open()){
            long long magic_number {0};
            ifile.read(reinterpret_cast<char*>(&magic_number), 8);

             if(magic_number == filetype){
                cout<<file.path().string()<<endl;
            }

            ifile.close();
                
        }
        else if(scan_options.verbose && !is_directory(file.path())){
            cout<<"Failed to open file: "<<file.path().generic_string()<<endl;
        }

    }
     if(ec && scan_options.verbose){
            cout<<"Failed to open directory:"<<error_dir<<" "<<ec.message()<<endl;
            
        }
}

void print_help(){

    cout<<endl;
    cout<<"Usage: file_type_tester [file type] [directory path] (-r) (-v)"<<endl;
    cout<<"Currently supported file types: "<<endl;
    cout<<"Elf(use -e in file type)"<<endl;

}

void check_args(const vector<string> &args){

    ScanOptions scan_options;

    if(args.size() == 2 && args.at(1) == "-h"){
        print_help();
        return;
    }

    if(args.size() < 3)
        cout<<"Too little arguments provided, use -h for help"<<endl;

    if(args.size() == 5){
        if(args.at(3) == "-r")
            scan_options.recursive = true;
        else{
            cout<<"Invalid argument provided, use -h for help"<<endl;
            return;}

        if(args.at(4) == "-v")
            scan_options.verbose = true;}

    if(args.size() == 4){
        if(args.at(3) == "-r")
            scan_options.recursive = true;
        else if(args.at(3) == "-v")
            scan_options.verbose = true;
        else{
            cout<<"Invalid argument provided, use -h for help";
        }
    }
     

        
    
    if(args.size() >= 3 && args.size() <= 5){
        if(args.at(1) == "-e")
            check_type(args.at(2), FileType::ELF, scan_options);
        
        else if(args.at(1) == "-p")
            check_type(args.at(2), FileType::PNG, scan_options);

            
        
        }
        

    
    else{
        cout<<"Too little arguments provided, use -h for help"<<endl;
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
