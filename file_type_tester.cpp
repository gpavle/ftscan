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
using std::filesystem::is_regular_file;
using std::filesystem::is_symlink;
using std::filesystem::read_symlink;
using std::ifstream;
using std::filesystem::filesystem_error;
using std::vector;




struct ScanOptions{

    bool recursive;
    bool verbose;


};

enum FileType{

    ELF = 1179403647,
    PNG = 727905341920923785

};

struct FileInfo{

    FileType filetype;
    size_t signature_length;

    FileInfo(const FileType &input_filetype){
        filetype = input_filetype;
        if(filetype == ELF)
            signature_length = 4;

    }

};

bool is_valid_file(const string &file_path){

    if(is_symlink(file_path))
        return is_valid_file(read_symlink(file_path));
    
    

    else if(is_regular_file(file_path) || is_directory(file_path))   
        return true;

    return false;

}




void check_type(const path &directory_name, const FileInfo &fileinfo, const ScanOptions &scan_options){


    try{
        directory_iterator it(directory_name);
    
    

    for(const auto &file : it){
        try{
            if(!is_valid_file(file.path().string()))
                continue;
                
            if(scan_options.recursive && is_directory(file.path()))
                check_type(file.path(), fileinfo, scan_options);
            ifstream ifile{file.path().string(), std::ios::binary};
            if(ifile.is_open()){
                long long magic_number {0};
                ifile.read(reinterpret_cast<char*>(&magic_number), fileinfo.signature_length);

            if(magic_number == fileinfo.filetype){
                cout<<file.path().string()<<endl;
            }

            ifile.close();
                
        }
            else if(scan_options.verbose && !is_directory(file.path())){
                cout<<"Failed to open file: "<<file.path().generic_string()<<endl;
        }}

    catch(const filesystem_error &fse){
        if(scan_options.verbose)
            cout<<fse.what()<<endl;
    }}
}catch(const filesystem_error &fse){
    if(scan_options.verbose)
        cout<<fse.what()<<endl;
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

        else{
            cout<<"Invalid argument provided, use -h for help";
        }

            
        
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
