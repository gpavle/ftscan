#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <map>
#include <array>
#include <optional>
#include <sstream>
#include <ranges>

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
using std::filesystem::exists;
using std::filesystem::absolute;
using std::ifstream;
using std::filesystem::filesystem_error;
using std::vector;
using std::map;
using std::array;
using std::optional;
using std::stringstream;
using std::views::enumerate;

enum class FileType : long;
struct ScanOptions;
struct ScanContext;
struct FileInfo;
bool is_valid_file(const path&, const bool&);
void check_type(const path&, const FileInfo&, ScanContext);
void print_help();
bool check_options(const vector<string>&, ScanOptions&);
optional<FileType> check_file_type_option(const string&);
void check_args(const vector<string>&);


struct ScanOptions{

    unsigned int recursion_depth = 0;
    bool verbose = false;
    bool follow_symlinks = false;
    bool absolute_paths = false;


};

struct ScanContext{
    const ScanOptions &rules;
    unsigned int current_depth;

    ScanContext(const ScanOptions &context_rules): rules{context_rules},current_depth{context_rules.recursion_depth}{}
    ScanContext(ScanContext ctx, const unsigned int depth): rules{ctx.rules}, current_depth{depth}{}
    

};

enum class FileType : long{

    ELF = 1179403647,
    PNG = 727905341920923785,
    JPG = 16767231,
    ISO = 211261867075,
    TAR = 491261293429,
    XZ = 388031461373,
    GZ = 35615


};

struct FileInfo{
    private:
        static const map<FileType, array<unsigned int, 2>> sig_info;
    public:
        FileType filetype;
        unsigned int signature_length;
        unsigned int offset;
    

        FileInfo(const FileType &input_filetype){
            filetype = input_filetype;
            signature_length = sig_info.at(filetype).at(0);
            offset = sig_info.at(filetype).at(1);

    }

};
const map<FileType, array<unsigned int, 2>> FileInfo::sig_info {{FileType::ELF, {4,0}},{FileType::JPG, {3,0}}, {FileType::PNG, {8,0}}, {FileType::ISO, {5,32769}}, {FileType::TAR, {5,257}}, {FileType::XZ, {6,0}}, {FileType::GZ, {2,0}}};

bool is_valid_file(const path &file_path, const ScanOptions &scan_options){

    if(!exists(file_path) && is_symlink(file_path) && scan_options.follow_symlinks){
        if(scan_options.verbose){
            if(scan_options.absolute_paths)
                cerr<<"Error: broken symlink:"<<absolute(file_path).lexically_normal()<< " to:"<<read_symlink(file_path)<<" File doesn't exist"<<endl;
            else{
                cerr<<"Error: broken symlink:"<<file_path<< " to:"<<read_symlink(file_path)<<" File doesn't exist"<<endl;
                
            }

        }
        return false;
    }

    if(is_symlink(file_path) && scan_options.follow_symlinks)
        return is_valid_file(read_symlink(file_path), scan_options);
    
    

        
    else if(is_regular_file(symlink_status(file_path)) || is_directory(symlink_status(file_path)))  
        return true;

    return false;

}




void check_type(const path &directory_name, const FileInfo &fileinfo, ScanContext scan_context){


    try{
    for(const auto &file : directory_iterator(directory_name)){
            if(!is_valid_file(file.path().generic_string(), scan_context.rules))
                continue;
                
            if(scan_context.current_depth > 0 && is_directory(file.path()))
                check_type(file.path(), fileinfo, ScanContext{scan_context, scan_context.current_depth - 1});
            ifstream ifile{file.path().generic_string(), std::ios::binary};
            if(ifile.is_open()){
                long magic_number {0};
                ifile.seekg(fileinfo.offset, std::ios::beg);
                ifile.read(reinterpret_cast<char*>(&magic_number), fileinfo.signature_length);

            if(magic_number == static_cast<long>(fileinfo.filetype)){
                if(scan_context.rules.absolute_paths)
                    cout<<canonical(file.path()).generic_string()<<endl;
                else
                    cout<<file.path().generic_string()<<endl;
            }

            ifile.close();
                
        }
            else if(scan_context.rules.verbose && !is_directory(file.path())){
                cerr<<"Failed to open file: "<<file.path().generic_string()<<endl;
        }

    }
}catch(const filesystem_error &fse){
     if(scan_context.rules.verbose){
            if(scan_context.rules.absolute_paths)
                cerr<<"Failed to open directory:" <<absolute(fse.path1()).lexically_normal().generic_string()<<": "<<fse.code().message()<<endl;
            
            else
                cerr<<"Failed to open directory:" <<fse.path1().generic_string()<<": "<<fse.code().message()<<endl;
            }
   
}
}

void print_help(){

    cout<<endl;
    cout<<"Usage: file_type_tester [file type] [directory path] [options]"<<endl;
    cout<<"-v       verbose mode(displays error messages)"<<endl;
    cout<<"-r N     recurive mode(descends into subdirectories N levels(omitting N gives maximum recursion))"<<endl;
    cout<<"-l       follow symlinks"<<endl;
    cout<<"-a       always display absolute paths(will resolve symlinks)"<<endl;
    cout<<"Currently supported file types: "<<endl;
    cout<<"ELF(use -e in file type)"<<endl;
    cout<<"PNG(use -p in the file type)"<<endl;
    cout<<"JPG(use -j in the file type)"<<endl;
    cout<<"ISO(use -i in the file type)"<<endl;
    cout<<"TAR(use -t in the file type)"<<endl;
    cout<<"XZ(use -x in the file type)"<<endl;
    cout<<"GZ(use -g in the file type)"<<endl;

}

bool check_options(const vector<string> &scan_options_args, ScanOptions &scan_options){
     for(const auto [index, arg] : enumerate(scan_options_args)){

            if(arg == "-r"){
                if(scan_options.recursion_depth != 0){
                    cerr<<"Invalid argument provided, use -h for help"<<endl;
                    return false;}
                
                if(scan_options_args.size() > static_cast<size_t>(index + 1) && stringstream{scan_options_args.at(index+1)} >> scan_options.recursion_depth);
                        
                else
                    scan_options.recursion_depth = 999999999;
                
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
                if(!(stringstream{arg} >> scan_options.recursion_depth && scan_options_args.at(index-1) == "-r")){
                    cerr<<"Invalid argument provided, use -h for help"<<endl;
                    return false;}
            }
        
        
    } 
    return true;

}

optional<FileType> check_file_type_option(const string &file_type_option){

    const map<string, FileType> options_filetypes{{"-e", FileType::ELF},{"-p", FileType::PNG},{"-j", FileType::JPG},{"-i", FileType::ISO,},{"-t", FileType::TAR},{"-x" , FileType::XZ},{"-g" ,FileType::GZ}};

    if(options_filetypes.find(file_type_option) == options_filetypes.end()){
        cerr<<"Invalid argument provided, use -h for help"<<endl;
        return std::nullopt;
        }

    return options_filetypes.at(file_type_option);
    
}

void check_args(const vector<string> &args){


    if(args.size() == 2 && args.at(1) == "-h"){
        print_help();
        return;
    }

    else if(args.size() < 3){
        cerr<<"Too little arguments provided, use -h for help"<<endl;
        return;}

   
    
     

    else if(args.size() <= 8){
        ScanOptions scan_options;
        if(check_options(vector<string>{args.begin()+3, args.end()}, scan_options)){
        string directory_name{args.at(2)};
        string file_type_option{args.at(1)};
        
       
       auto chosen_type {check_file_type_option(file_type_option)};
       if(!chosen_type){
            return;
       }
       if(args.at(2) == "-"){
        while(getline(cin, directory_name)){
            check_type(directory_name, chosen_type.value(), scan_options);
        }
        
       }
       else
            check_type(directory_name,chosen_type.value(), scan_options);
        
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
