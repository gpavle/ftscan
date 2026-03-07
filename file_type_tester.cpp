#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>

using std::cin;
using std::cerr;
using std::getline;
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

enum class FileType : long;
struct ScanOptions;
struct FileInfo;
bool is_valid_file(const path&, const bool&);
void check_type(const path&, const FileInfo&, const ScanOptions&);
void print_help();
bool check_options(const vector<string>&, ScanOptions&);
FileType check_file_type_option(const string&);
void check_args(const vector<string>&);


struct ScanOptions{

    bool recursive = false;
    bool verbose = false;
    bool follow_symlinks = false;
    bool absolute_paths = false;


};

enum class FileType : long{

    ELF = 1179403647,
    PNG = 727905341920923785,
    JPG = 16767231,
    PE = 9460301


};

struct FileInfo{

    FileType filetype;
    size_t signature_length;

    FileInfo(const FileType &input_filetype){
        filetype = input_filetype;
        if(filetype == FileType::ELF || filetype == FileType::PE)
            signature_length = 4;
        else if(filetype == FileType::JPG)
            signature_length = 3;
        else if(filetype == FileType::PNG)
            signature_length = 8;

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
        
    
    for(const auto &file : directory_iterator(directory_name)){
        try{
            if(!is_valid_file(file.path().generic_string(), scan_options.follow_symlinks))
                continue;
                
            if(scan_options.recursive && is_directory(file.path()))
                check_type(file.path(), fileinfo, scan_options);
            ifstream ifile{file.path().generic_string(), std::ios::binary};
            if(ifile.is_open()){
                long long magic_number {0};
                ifile.read(reinterpret_cast<char*>(&magic_number), fileinfo.signature_length);

            if(magic_number == static_cast<long>(fileinfo.filetype)){
                if(scan_options.absolute_paths)
                    cout<<canonical(file.path()).generic_string()<<endl;
                else
                    cout<<file.path().generic_string()<<endl;
            }

            ifile.close();
                
        }
            else if(scan_options.verbose && !is_directory(file.path())){
                cerr<<"Failed to open file: "<<file.path().generic_string()<<endl;
        }}

    catch(const filesystem_error &fse){
        if(scan_options.verbose)
            cerr<<fse.what()<<endl;
    }}
}catch(const filesystem_error &fse){
    if(scan_options.verbose)
        cerr<<fse.what()<<endl;
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
    cout<<"JPG(use -j in the file type)"<<endl;

}

bool check_options(const vector<string> &args, ScanOptions &scan_options){
     for(int i{0}; const auto &arg : args){

        if(i >= 3){
            if(arg == "-r"){
                if(scan_options.recursive == true){
                    cerr<<"Invalid argument provided, use -h for help"<<endl;
                    return false;
                }
                scan_options.recursive = true;
            }
            if(arg == "-v"){
                if(scan_options.verbose == true){
                    cerr<<"Invalid argument provided, use -h for help"<<endl;
                    return false;
                }
                scan_options.verbose = true;
            }
            if(arg == "-l"){
                if(scan_options.follow_symlinks == true){
                    cerr<<"Invalid argument provided, use -h for help"<<endl;
                    return false;
                }
                scan_options.follow_symlinks = true;
            }
            if(arg == "-a"){
                if(scan_options.absolute_paths == true){
                    cerr<<"Invalid argument provided, use -h for help"<<endl;
                    return false;
                }
                scan_options.absolute_paths = true;
            }
            
            if(arg != "-r" && arg != "-v" && arg != "-l" && arg != "-a"){
                cerr<<"Invalid argument provided, use -h for help"<<endl;
                return false;
            }
        }
        i++;
    } 
    return true;

}

FileType check_file_type_option(const string &file_type_option){

     if(file_type_option == "-e")
        return FileType::ELF;

    else if(file_type_option == "-j")   
        return FileType::JPG;

        
    else if(file_type_option == "-p")
        return FileType::PNG;
    
    else if(file_type_option == "-pe"){
        return FileType::PE;
    }
        

    else{
        cerr<<"Invalid argument provided, use -h for help"<<endl;
        return static_cast<FileType>(0);
        }

}

void check_args(const vector<string> &args){


    if(args.size() == 2 && args.at(1) == "-h"){
        print_help();
        return;
    }

    else if(args.size() < 3){
        cerr<<"Too little arguments provided, use -h for help"<<endl;
        return;}

   
    
     

    else if(args.size() <= 7){
        ScanOptions scan_options;
        if(check_options(args, scan_options)){
        string directory_name{args.at(2)};
        string file_type_option{args.at(1)};
        
       
       FileType chosen_type {check_file_type_option(file_type_option)};
       if(static_cast<long>(chosen_type) == 0){
            return;
       }
       if(args.at(2) == "-"){
        while(getline(cin, directory_name)){
            check_type(directory_name, chosen_type, scan_options);
        }
        
       }
       else
            check_type(directory_name,chosen_type, scan_options);
        
        }}
    

        
    else{
        cerr<<"Too many arguments provided, use -h for help"<<endl;
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
