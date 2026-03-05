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
using std::filesystem::canonical;
using std::filesystem::is_directory;
using std::filesystem::is_regular_file;
using std::filesystem::is_symlink;
using std::filesystem::read_symlink;
using std::filesystem::symlink_status;
using std::ifstream;
using std::filesystem::filesystem_error;
using std::vector;




struct ScanOptions{

    bool recursive = false;
    bool verbose = false;
    bool follow_symlinks = false;
    bool absolute_paths = false;


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

bool is_valid_file(const path &file_path, const bool &follow_symlinks){

    if(is_symlink(file_path) && follow_symlinks)
        return is_valid_file(read_symlink(file_path), follow_symlinks);
    
    

        
    else if(is_regular_file(symlink_status(file_path)) || is_directory(symlink_status(file_path)))  
        return true;

    return false;

}




void check_type(const path &directory_name, const FileInfo &fileinfo, const ScanOptions &scan_options){


    try{
        directory_iterator it(directory_name);
    
    

    for(const auto &file : it){
        try{
            if(!is_valid_file(file.path().string(), scan_options.follow_symlinks))
                continue;
                
            if(scan_options.recursive && is_directory(file.path()))
                check_type(file.path(), fileinfo, scan_options);
            ifstream ifile{file.path().string(), std::ios::binary};
            if(ifile.is_open()){
                long long magic_number {0};
                ifile.read(reinterpret_cast<char*>(&magic_number), fileinfo.signature_length);

            if(magic_number == fileinfo.filetype){
                if(scan_options.absolute_paths)
                    cout<<canonical(file.path()).string()<<endl;
                else
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
    cout<<"Usage: file_type_tester [file type] [directory path] [options]"<<endl;
    cout<<"-v       verbose mode(displays error messages)"<<endl;
    cout<<"-r       recurive mode(descends into subdirectories)"<<endl;
    cout<<"-l       follow symlinks"<<endl;
    cout<<"-a       always display absolute paths(will resolve symlinks)"<<endl;
    cout<<"Currently supported file types: "<<endl;
    cout<<"ELF(use -e in file type)"<<endl;
    cout<<"PNG(use -p in the file type)"<<endl;

}

void check_args(const vector<string> &args){

    ScanOptions scan_options;

    if(args.size() == 2 && args.at(1) == "-h"){
        print_help();
        return;
    }

    if(args.size() < 3){
        cout<<"Too little arguments provided, use -h for help"<<endl;
        return;}

    for(int i{0}; const auto &arg : args){

        if(i >= 3){
            if(arg == "-r"){
                if(scan_options.recursive == true){
                    cout<<"Invalid argument provieded, use -h for help"<<endl;
                    return;
                }
                scan_options.recursive = true;
            }
            if(arg == "-v"){
                if(scan_options.verbose == true){
                    cout<<"Invalid argument provided, use -h for help"<<endl;
                    return;
                }
                scan_options.verbose = true;
            }
            if(arg == "-l"){
                if(scan_options.follow_symlinks == true){
                    cout<<"Invalid argument provided, use -h for help"<<endl;
                    return;
                }
                scan_options.follow_symlinks = true;
            }
            if(arg == "-a"){
                if(scan_options.absolute_paths == true){
                    cout<<"Invalid argument provided, use -h for help"<<endl;
                    return;
                }
                scan_options.absolute_paths = true;
            }
            
            if(arg != "-r" && arg != "-v" && arg != "-l" && arg != "-a"){
                cout<<"Invalid argument provided, use -h for help"<<endl;
                return;
            }
        }
        i++;
    }
     

    if(args.size() <= 7){
        if(args.at(1) == "-e")
            check_type(args.at(2), FileType::ELF, scan_options);
        
        else if(args.at(1) == "-p")
            check_type(args.at(2), FileType::PNG, scan_options);

        else{
            cout<<"Invalid argument provided, use -h for help";
        }

            
        
        }
    
    else{
        cout<<"Too many arguments provided, use -h for help"<<endl;
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
