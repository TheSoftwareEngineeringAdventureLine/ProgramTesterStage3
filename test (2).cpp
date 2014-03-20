/*************************************************************************//**
 * @file
 *
 * @mainpage Automated Grading System
 *
 * @section course_section CSC 470
 *
 * @author Julian Brackins, Jon Dixon, and Hafiza Farzami
 *
 * @date February 9, 2014
 *
 * @par Professor:
 *         Dr. Logar
 *
 * @par Course:
 *         CSC 470
 *
 * @par Location:
 *         McLaury - 313
 *
 * @section program_section Program Information
 *
 * @details
 * test.cpp is a testing suite designed for 150/250/300 level c++ file
 * testing. The application is meant to compile and run c++ files, using
 * test cases supplied in the same directory as the .cpp file, or nested in
 * subdirectories under the same directory where the .cpp file is located.
 * The test application will run the compiled program, using all available test
 * cases found in subdirectories, and will keep record of the success or failure
 * of each test case. An overall grade will be assigned to the program based on
 * how many test cases are passed.
 *
 * @section compile_section Compiling and Usage
 *
 * @par Compiling Instructions:
 *      g++ -o test test.cpp -g or make
 *
 * @par Usage:
   @verbatim
   ./test <file>

   NOTE that the <file> should be in a folder with the same name as the .cpp
   file you wish to compile and run. the folder should be in the same directory
   as the test application.
   Example
   in /home directory:
      test example
   in /home/example directory:
      example.cpp
   @endverbatim
 *
 * @section todo_bugs_modification_section Todo, Bugs, and Modifications
 *
 * @bug none
 *
 * @todo Sprint 1 complete
 *
 * @par Modifications and Development Timeline:
   @verbatim
   Date          Modification
   ------------  --------------------------------------------------------------
   Feb 09, 2014  Implemented compile_file(), started run_file(). also created
                 add_extension() to handle removing extensions from .cpp files
   Feb 11, 2014  Set up GitHub. http clone url:
                 https://github.com/jbrackins/CSC470.git
   Feb 12, 2014  printed usage, revamping program structure to handle commands
                 within the software rather than via command line
                 made routine to count the number of .tst files in directory
   Feb 13, 2014  run_file() now redirects input such that case_x.tst is read
                 in as the commands for the test and case_x.out is the result
                 from forementioned test.
                 run_file() now returns integer, 1 if it passes a test case,
                 0 if it fails
                 test_loop() runs through each test case run and tallies how
                 many were successes
                 is_dir() detects if object in directory is a directory
                 vector_directories() recursively traverses a folder system and
                 utilizes is_dir() to find all subdirectories. Each of these
                 subdirectories found is pushed into a vector.
                 Subdirectory traversal ((((should)))) be complete.
   Feb 14, 2014  Code Review session with team at 3:00 PM.
   Feb 18, 2014  Completed log file creation in test_loop()
                 Fixed piping in run_file() to prevent output to terminal.
                 Renamed file to test.cpp (orignally grade.cpp)
                 Implemented error handing and modified usage statement
                 User Interface from event_loop() removed, now simply takes in
                 file to be tested via command line.
   Feb 19, 2014  Revamped function order for a more logical grouping in file.
                 Final documentation in preparation for submission.
   @endverbatim
 *
 *****************************************************************************/

/*************************************************************************//**
*********************************INCLUDES*************************************
******************************************************************************/

#define CURR_VER "1.0"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>     //cout cin
#include <fstream>      //file i/o
#include <string>       //basic string handling. cstyle also used for directory

#include <dirent.h>     //
#include <cstring>      //
#include<sys/stat.h>    //Handling directory traversal
#include<sys/types.h>   //
#include <vector>        //

#include <ctime>        //Handling Timestamps

/*************************************************************************//**
*********************************NAMESPACE************************************
******************************************************************************/

using namespace std;

/*************************************************************************//**
********************************FUNCTION PROTOTYPES***************************
******************************************************************************/

/*Compiling and Running File*/
int compile_file(string cpp_file);
int run_file(string cpp_file, string test_case);

/*Testing Code*/
bool test_loop(string cpp_file);
int count_case();
int result_compare(string test_file);
bool isGolden(string& golden_name, string path, string home);
bool test_code(string cpp_file, vector<string> test_cases, int &total, ofstream &fout);

/*Directory Traversal Code*/
bool change_dir(string dir_name);
bool is_dir(string dir);
void vector_directories(string base_dir, vector<string>& vector);
void vector_test_cases(vector<string>& case_vector, vector<string> &crit_vector);
void get_source(string& source_file);
void student_source (vector<string>& source, string new_dir,string home,
                     vector<string>& source_path);
void get_golden(string& source_file);

/*Log files and Grade calculations*/
string log_filename(string cpp_file);
double grade_percent(int right, int total);
string grade_letter(double grade_percent);

/*Usage Statements*/
void usage();
void err_usage();

/*Misc String Manipulations*/
string add_extension(string input);
string get_extension(string input);
string get_pathname();
string case_name(string test_case, string ext);
string timestamp();
string str_replace(string str, char a, char b);
string student_log_file(string str);
string student_name(string source);

/*Not used in Sprint 1*/
bool event_loop();
void dir_list();

/**************************************************************************//**
 * @author Julian Brackins
 *
 * @par Description:
 * Main function.
 * reads argument from command line into test_loop().
 *
 * @param[in] argc - # of arguments.
 * @param[in] argv - argument vector.
 *
 * @returns 0 - successfully exit program.
 *
 *****************************************************************************/
int main(int argc, char ** argv)
{
    usage();
    if(argc > 1)
    {
        cout << "testing " << argv[1] << "...\n" ;
        test_loop(argv[1]);
    }
    else
        err_usage();
    return 0;
}

/**************************************************************************//**
 * @author Julian Brackins
 *
 * @par Description:
 * Using C++ String manipulation, a g++ compile command is sent to the
 * terminal in order to compile the file brought in by the argument
 * cpp_file.
 *
 * The compile line is as follows
 * compile_file(example.cpp);
 * "g++ -o example example.cpp"
 *
 * @param[in] cpp_file - name of .cpp file to be compiled by program
 *
 * @returns system(buffer.c_str()) - 0 if compile worked, 1024 if failed
 *
 *****************************************************************************/
int compile_file(string cpp_file)
{
    string buffer("g++ -o");
    string out_name = cpp_file;
    cpp_file.erase(cpp_file.length()-4);
    buffer += " " + cpp_file + " " + out_name;
    return system(buffer.c_str());
}

/**************************************************************************//**
 * @author Julian Brackins
 *
 * @par Description:
 * Using C++ String manipulation, a command is sent to the terminal in
 * order to run the file brought in by the argument cpp_file
 * String buffers are used to handle piping both for inputting
 * An integer value is returned from this function.
 * A 0 indicates the program failed the test case.
 * A 1 indicates the program has identical results to the test case.
 *
 * The run line sent to system() is as follows
 * run_file(example, case_x.tst);
 * <full_path>/./example < case_x.tst > case_x.out
 *
 * @param[in] cpp_file - name of program file to be run
 * @param[in] test_case - string with test case file name
 *
 * @returns result_compare(test_case) - 0 if test fails, 1 if test succeeds
 *
 *****************************************************************************/
int run_file(string cpp_file, string test_case) //case_num
{
    string run_cmd("./");
    cpp_file = student_name(cpp_file);
    run_cmd += cpp_file;
    //create .out file name
    string case_out(case_name(test_case, "out"));

    //set up piping buffers
    string buffer1("");
    string buffer2(" &> /dev/null < ");
    string buffer3(" > ");

    // "try using | "
    //construct run command, then send to system
    //./<filename> &> /dev/null  < case_x.tst > case_x.out
    buffer1 += run_cmd + buffer2 + test_case + buffer3 + case_out;
    system(buffer1.c_str());

    //0 = Fail, 1 = Pass
    return result_compare(test_case);
}

/**************************************************************************//**
 * @author Julian Brackins
 *
 * @par Description:
 * The algorithm for traversing through each directory, running the program
 * and checking the results from the test cases present in each subdirectory.
 *
 * The algorithm is as follows:
 * -create a vector of every subdirectory in program folder
 * -change to directory where program is located
 * -compile program
 * -while subdirectory vector is not empty:
 *   -de-vector first subdirectory in vector
 *     -change into that subdirectory
 *   -create a vector of every .tst file in current directory
 *   -while test case vector is not empty:
 *     -de-vector first test case in vector
 *     -run program using that test case
 *       -count whether the program passed or failed test case
 * -change back to home directory (where program is located)
 * -create a vector of every .tst file in home directory
 *   -while test case vector is not empty:
 *     -de-vector first test case in vector
 *     -run program using that test case
 *       -count whether the program passed or failed test case
 * -write log file containing percentage of tests passed and final grade
 *
 * @param[in] cpp_file - program name
 *
 * @returns none
 *
 *****************************************************************************/
bool test_loop(string cpp_file)
{
    int test_cases_temp = 0;
    int test_cases_total = 0;
    int i;
    int total = 0;
    bool passed;

    ofstream fout, student_fout;
    string log_name(log_filename(cpp_file));
    string golden_name;  						//if there is a .cpp, golden exists
    string golden_dir;	//if golden.cpp exists it exists in golden_dir
    string student_logname;

    vector<string> sub_dir;                  //vector of all the subdirectories
    vector<string> test_cases;               //vector of test cases in current directory
    vector<string> _source;					//student source code in the directory
    vector<string> source_paths;				//path to student source code
    vector<string> crit_cases;

    string homepath(get_pathname() + "/");  //create string with home path name

    string subpath(get_pathname() + "/");   //create a string with current directory path

    string progpath(get_pathname() + "/" + cpp_file + "/"); //string with path to prog file

    golden_dir = homepath + cpp_file;	//create path to find golden.cpp
    //golden = true if golden.cpp exists, golden.cpp filename will be golden_name
    bool golden = isGolden(golden_name, golden_dir, get_pathname());

    //call James' function here!

    vector_directories(cpp_file, sub_dir);   //place all subdirectory names in vector

    //Open file
    fout.open((homepath + log_name).c_str());    //open file

    //use this to find all the students source code, the path to the source code,
    // and the test files.. run the tests elsewhere.
    while(sub_dir.size() != 0)
    {
        vector<string> used ;  //implement this to determine if the students source was found.
        bool isDone = false;  //  "" "" "" "" "" """ "" "

        //check if the sub directory is a students
        if (sub_dir.back().find("test") == -1)
        {
            //look in the directory for the .cpp file and put in vector
            student_source(_source, sub_dir.back(),homepath,source_paths);
        }

        else
        {
            change_dir(homepath + sub_dir.back()); //change to next subdirectory in vector

            vector_test_cases(test_cases, crit_cases);           //vector .tst files in current directory

            test_cases_temp = count_case();         //count the number of .tst files... MIGHT REMOVE
            test_cases_total += test_cases_temp;

            if(test_cases.size() != 0)
            {
                student_fout << "In directory " << sub_dir.back() << ":\n";
                cout << "In directory " << sub_dir.back() << ":\n";
            }

            change_dir (homepath);
        }
        sub_dir.pop_back();                          //remove sub directory from vector
    }

    //for each source file in _source, do the following
    i = _source.size();
    for (int j = 0; j < i; j++)
    {   
        cout << _source.back() << endl;
        passed = true;
        total = 0;
        //cout << student_logfile(_source[j]) << endl;
        change_dir(source_paths.back());       //change directory to where prog file is located

        student_logname = student_log_file(_source.back());

        student_fout.open(student_logname.c_str());
        if(!student_fout)
        {
            cout << _source.back() << " failed to open, testing skipped." << endl;
            continue;
        }

        //change into source code file -- compile it -- test it

        if(compile_file(_source.back()) != 0 )     //compile the prog file
        {
            err_usage();
            return false;
        }
        if(crit_cases.size() > 0)
        {
            passed = test_code(_source.back(), crit_cases, total, student_fout);
            
        }  

        if(passed)
        {
            if(test_cases.size() > 0)
            {
                test_code(_source.back(), test_cases, total, student_fout);
            }     
        }

        //output grade to log file
        if(passed)
        {
            student_fout << "\n" << total << "/" << test_cases_total << " test cases passed\n";
            int grade = grade_percent(total, test_cases_total);
            student_fout << "percentage: " << grade << "%\n";
            student_fout << "     grade: " << grade_letter(grade) << "\n";
            fout << student_name(_source.back())<< "\t\t%" << grade << endl;
        }
        else
        {   
            int grade = 0;
            student_fout << "FAILED" << endl;
            student_fout << "percentage: " << grade << "%\n";
            student_fout << "     grade: " << grade_letter(grade) << "\n";
            fout << student_name(_source.back()) << "\t\tFAILED" << endl;
        }
        //return to the homepath
        change_dir(homepath);

        student_fout.close();
        cout << "Done.\n";

        _source.pop_back();
        source_paths.pop_back();


    }
    fout.close();
    return true;
}

string student_name(string source)
{
        return source.substr(0, source.find_last_of("."));
}

bool test_code(string cpp_file, vector<string> test_cases, int &total, ofstream &fout)
{
    string subpath;
    bool passed = true;
    int j = test_cases.size();
    for(int i = 0; i < j; i++) //test_cases is empty if done testing home directory
    {
        fout << test_cases[i] << ": ";
        subpath = get_pathname() + "/";
        if ( run_file(cpp_file, test_cases[i]) == 1) // run_file(criticaltests[i], subpath+test_cases[i])
        {
            total += 1;
            fout << "PASSED\n";  
        }
        else
        {
            fout << "FAILED\n";
            passed = false;
        }
    }
    return passed;
}

/******************************************************************************
* Author: Anthony Morast, James Thilma, Ben Sherman
*
* This function changes into the student directory and finds the .cpp source
* code for that student. The source code file name is pushed onto the source
* vector and the path to the source code is pushed onto the source_paths vector.
* The function switches back into the home directory once the source code is
* found.
*
******************************************************************************/
void student_source (vector<string>& source, string new_dir,string home,
                     vector<string>& source_paths)
{
    //if the sub_dir belongs to a student, switch into that directory
    string source_file;
    change_dir(new_dir);

    //gets the current path
    string path = get_pathname();

    get_source(source_file);	//find the .cpp source file
    if ( source_file.find(".cpp") != -1 )
    {
        source.push_back(source_file);	//vector the source file
        source_paths.push_back(path);	//vector the path
    }

    //switch to home directory
    change_dir(home);
}

/******************************************************************************
*
*
*
******************************************************************************/
bool isGolden(string& golden_name, string path, string home)
{
    string name;

    change_dir(path); //change into directory

    get_golden(name); //determine if there's a .cpp file

    if (name.length() > 0)	//if there is a .cpp file
    {
        golden_name = name;		//set golden name to the .cpp file name
        change_dir(home);	//change back to home directory
        return true;
    }

    change_dir(home);	//change back to home dir

    return false;  //otherwise return false
}

/**************************************************************************//**
 * @author Julian Brackins
 *
 * @par Description:
 * Find number of test cases in directory.
 * Using DIR, current directory is traversed. The name of each file and directory
 * is read in one by one to the DIR pointer. Every file with a tst extension is
 * tallied to count the total number of test cases in the directory.
 *
 * @returns count - number of .tst files in current directory
 *
 *****************************************************************************/
int count_case()
{
    DIR *dp;
    int count = 0;
    char* name;
    struct dirent *ep;
    dp = opendir ("./");

    //cout << get_pathname() << endl;
    if (dp != NULL)
    {
        while (ep = readdir (dp))
        {
            name = ep->d_name;
            string str_name(get_extension(name));
            if (str_name.compare("tst") == 0)
                count++;
        }
        (void) closedir (dp);
    }

    return count;
}

/**************************************************************************//**
 * @author Julian Brackins
 *
 * @par Description:
 * The result_compare() function is designed to determine whether or not the
 * program passes or fails a test case. in the run_file() command, the results
 * of the test are piped into a .out file with the same name as the .tst file
 * used for testing input. A file with the same name but a .ans extension will
 * contain the expected result from the given test case.
 * By this logic, a successful test case run will result in a .out file that is
 * identical to the .ans file. This function runs the diff command on the two
 * files to determine if the files are identical. If the two files match, the
 * program passed the test case, and a 1 is returned. If the files do not match,
 * a 0 is returned.
 * The results of the diff command are actually piped into a .tmp file. This
 * temporary file will be empty if the files are identical, and will contain
 * the differences between the two files if they are not identical. The file
 * is opened to determine if it has any contents, and if not, the .out and .ans
 * files can be confirmed to be identical.
 *
 * @param[in] test_file - test file name
 *
 * @returns 1 - Empty .tmp file, test passed.
 * @returns 0 - diff command yielded results, test failed.
 *
 *****************************************************************************/
int result_compare(string test_file)
{
    int length;
    ifstream fin;

    string case_out(case_name(test_file, "out"));
    string case_ans(case_name(test_file, "ans"));
    string case_tmp(case_name(test_file, "tmp"));   //create temp file

    //perform diff command
    string buffer("diff ");
    buffer += case_out + " " + case_ans + " > " + case_tmp;
    system(buffer.c_str());

    fin.open(case_tmp.c_str(), ios::binary);    //open file
    fin.seekg(0, ios::end);                     //cursor at EOF
    length = fin.tellg();                       //find cursor position
    fin.close();

    //remove tmp file
    buffer = "rm " + case_tmp;
    system(buffer.c_str());
    if ( length == 0 ) //File is empty, no diff between .ans and .tmp
        return 1;
    else
        return 0;
}

/**************************************************************************//**
 * @author Julian Brackins
 *
 * @par Description:
 * Directory traversal is quintessential in this program in order to find all
 * available test cases for a given program. change_dir() is the heart of the
 * traversal functions designed for this project.
 * It should be noted that most path names read in here should be the full
 * path name to avoid getting stuck in deep-nested directories. Using the full
 * path will allow the program to change to a specific directory, rather than
 * be restricted to the sub directories present in the current path.
 * Regardless, the function returns a boolean value to indicate whether or not
 * the directory change was successful.
 *
 * @param[in] dir_name - full path of a directory
 *
 * @returns true - successful directory change
 * @returns false - failed to change directories
 *
 *****************************************************************************/
bool change_dir(string dir_name)
{
    string path;
    if(chdir(dir_name.c_str()) == 0)
    {
        path = get_pathname();
        //cout << "In " << path << "\n";
        return true;
    }
    return false;
}

/**************************************************************************//**
 * @author Julian Brackins
 *
 * @par Description:
 * Small function to determine if an object in a directory is a directory
 * itself.
 *
 * @returns true - is a dir
 * @returns false - is not a dir (probably a file)
 *
 *****************************************************************************/
bool is_dir(string dir)
{
    struct stat file_info;
    stat(dir.c_str(), &file_info);
    if ( S_ISDIR(file_info.st_mode) )
        return true;
    else
        return false;
}

/******************************************************************************
*
*
*
*
*
******************************************************************************/
void get_golden(string& source_file)
{
    DIR *dp;
    struct dirent *dirp;
    string path(get_pathname());
    string slash = "/";
    path += slash;
    string cpp = ".cpp";
    string file_name;
    if ((dp = opendir(path.c_str())) == NULL)
    {
        cout << "Error opening directory...\n";
        return;
    }
    else
    {
        //cout << "files in: " << path << "\n";
        while ((dirp = readdir(dp)) != NULL)
        {
            if (dirp->d_name != string(".") && dirp->d_name != string(".."))
            {
                if (is_dir(path + dirp->d_name) == false)       //NOT a directory
                {
                    file_name =  dirp->d_name;
                    string ext (file_name.end()-4, file_name.end());    //get ext
                    if(ext.compare(".cpp") == 0)                         //check if it's a .cpp
                    {
                        //cout << file_name << "\n";
                        source_file = file_name;                  //add .cpp file name to vector
                    }
                }
            }
        }
        closedir(dp);
    }
}
/******************************************************************************
*
*
*
*
*
******************************************************************************/
void get_source(string& source_file)
{
    DIR *dp;
    struct dirent *dirp;
    string path(get_pathname());
    string slash = "/";
    string cpp = ".cpp";
    string file_name;
    string student_name = "test";

    int index = path.find_last_of(slash);
    student_name = path.substr(index+1,path.length()-1);
    student_name += cpp;

    path += slash;

    //cout << "looking for " << student_name <<" in " <<get_pathname();

    if ((dp = opendir(path.c_str())) == NULL)
    {
        cout << "Error opening directory...\n";
        return;
    }
    else
    {
        //cout << "files in: " << path << "\n";
        while ((dirp = readdir(dp)) != NULL)
        {
            if (dirp->d_name != string(".") && dirp->d_name != string(".."))
            {
                if (is_dir(path + dirp->d_name) == false)       //NOT a directory
                {
                    file_name =  dirp->d_name;
                    //string ext (file_name.end()-4, file_name.end());    //get ext
                    if(file_name == student_name)                         //check if it's a .cpp
                    {
                        //cout << " found " << file_name;
                        //cout << file_name << "\n";
                        source_file = file_name;                  //add .cpp file name to vector
                    }
                }
            }
        }
        closedir(dp);
    }
}
/**************************************************************************//**
 * @author Julian Brackins
 *
 * @par Description:
 * Recursively traverse the directory structure, adding the name of each
 * subdirectory to a vector. These subdirectories are the test suites, which
 * will later be referenced to run the corresponding program against each
 * test case in the subdirectory.
 *
 * @param[in] test_file - the base directory
 * @param[in,out] vector - subdirectory vector.
 *
 * @returns none
 *
 *****************************************************************************/
void vector_directories(string base_dir, vector<string>& vector)
{
    DIR *dp;
    struct dirent *dirp;
    string path;

    string dir_name(base_dir);

    base_dir += "/";
    if ((dp = opendir(base_dir.c_str())) == NULL)
    {
        cout << "\nError opening subdirectories...\n";
        return;
    }
    else
    {
        while ((dirp = readdir(dp)) != NULL)
        {
            if (dirp->d_name != string(".") && dirp->d_name != string(".."))
            {
                if (is_dir(base_dir + dirp->d_name) == true)            //it's a directory
                {
                    path = base_dir + dirp->d_name + "/";
                    //cout << path << endl;
                    vector.push_back(path);                                   //push string into vector
                    vector_directories(base_dir + dirp->d_name, vector);  //recursion!!
                }
            }
        }
        closedir(dp);
    }
}

/**************************************************************************//**
 * @author Julian Brackins
 *
 * @par Description:
 * Traverse the directory, adding the name of each test case to a vector.
 * These vectord test cases will later be referenced to run the corresponding
 * program against each test case in the directory.
 * A modified version of vector_directories(), this version points to each
 * object in the directory and determines if each one is a file, rather than a
 * directory. From there, if the file contains the .tst extension, it is added
 * to the vector. No recursion in this one, since and individual test case vector
 * is built for each sub directory.
 *
 * @param[in,out] vector - test case vector.
 *
 * @returns none
 *
 *****************************************************************************/
void vector_test_cases(vector<string>& case_vector, vector<string> &crit_vector)
{
    DIR *dp;
    struct dirent *dirp;
    string path(get_pathname());
    string slash = "/";
    path += slash;
    string new_path = path;
    string file_name, crit;
    int under_score, dot;

    if ((dp = opendir(path.c_str())) == NULL)
    {
        cout << "Error opening directory...\n";
        return;
    }
    else
    {
        //cout << "files in: " << path << "\n";
        while ((dirp = readdir(dp)) != NULL)
        {
            new_path = path;
            if (dirp->d_name != string(".") && dirp->d_name != string(".."))
            {
                if (is_dir(path + dirp->d_name) == false)       //NOT a directory
                {
                    file_name =  dirp->d_name;
                    string ext (file_name.end()-4, file_name.end());    //get ext
                    if(ext.compare(".tst") == 0)                         //check if it's a .tst
                    {
                        new_path += file_name;
                        // get position fo last dot and alst underscore int eh filename path
                        under_score = file_name.find_last_of( '_' );
                        dot = file_name.find_last_of( '.' );

                        // if either the dot or underscore is not found, then the test case file
                        // is not a critical test case
                        if( under_score >= 0 && dot >= 0 )
                            crit = file_name.substr(under_score, 6);

                        else
                            crit = "";

                        if(crit.compare("_crit.") == 0)
                            crit_vector.push_back(new_path);
                        else
                            //cout << file_name << "\n";
                            case_vector.push_back(new_path);  //add .tst file name to vector

                    }
                }
            }
        }
        closedir(dp);
    }
    cout << "crit size: " << crit_vector.size() << endl;
}

/**************************************************************************//**
 * @author Julian Brackins
 *
 * @par Description:
 * A function that creates a string containing the log file name with the
 * appropriate timestamp extension.
 *
 * @param[in] cpp_file - file name
 *
 * @returns log_str += "_" + timestamp() + ".log" - .log file with timestamp
 *
 *****************************************************************************/
string log_filename(string cpp_file)
{
    string log_str(cpp_file);
    return log_str += "_" + timestamp() + ".log";
}

/**************************************************************************//**
 * @author Julian Brackins
 *
 * @par Description:
 * A small function to calculate the percentage of test cases passed.
 *
 * @param[in] right - number of test cases passed
 * @param[in] total - total number of test cases
 *
 * @returns float( ( float(right) / float(total) ) * 100 ) - % of passed test
 *
 *****************************************************************************/
double grade_percent(int right, int total)
{
    return float( ( float(right) / float(total) ) * 100 );
}

/**************************************************************************//**
 * @author Julian Brackins
 *
 * @par Description:
 * A double containing the percentage of test cases correct is brought into
 * this function and a grade letter is assigned.
 *
 * @param[in] grade_percent - percent of test cases passed
 *
 * @returns letter - Letter grade
 *
 *****************************************************************************/
string grade_letter(double grade_percent)
{
    string letter;

    if(grade_percent >= 90.0)
        letter = "A";
    else if(grade_percent >= 80.0)
        letter = "B";
    else if(grade_percent >= 70.0)
        letter = "C";
    else if(grade_percent >= 60.0)
        letter = "D";
    else
        letter = "F";
    return letter;
}

/**************************************************************************//**
 * @author Julian Brackins
 *
 * @par Description:
 * Prints Software Info
 *
 * @returns none
 *
 *****************************************************************************/
void usage()
{
    cout << "\n\n*************AUTOMATED GRADING SYSTEM*************\n";
    cout << "*********************ver  ";
    cout << CURR_VER << "*********************\n\n";
}

/**************************************************************************//**
 * @author Julian Brackins
 *
 * @par Description:
 * Prints error message
 *
 * @returns none
 *
 *****************************************************************************/
void err_usage()
{
    cout << "\nAn error occurred.....\n";
    cout << "USAGE:\n";
    cout << "./test <filename> : compile <filename> and test it\n";
    cout << "                    against available test suites\n";
}

/**************************************************************************//**
 * @author Julian Brackins
 *
 * @par Description:
 * This function is needed to handle the addition of the .cpp extension on file
 * names. This is important, for example, when compiling the file, as you need
 * the full name of the file (example.cpp) as well as the name of the file sans
 * extension (example)
 *
 * @param[in] input - string containing file name
 *
 * @returns newstring - string similar to parameter input with .cpp extension
 *
 *****************************************************************************/
string add_extension(string input)
{
    input.append(".cpp");
    return input;
}

/**************************************************************************//**
 * @author Julian Brackins
 *
 * @par Description:
 * This function is needed to detect the extension on a given file. The
 * extension on the given file is returned as a string.
 * Used for detecting whether files in a directory contain a .tst extension,
 * therefore indicating that the file is a test case.
 *
 * @param[in] input - string containing file name and extension
 *
 * @returns extension - file extension
 *
 *****************************************************************************/
string get_extension(string input)
{
    unsigned found = input.find_last_of(".");
    string extension( input.substr(found+1) );
    return extension;
}

/**************************************************************************//**
 * @author Julian Brackins
 *
 * @par Description:
 * Returns a string with the current working directory. Useful not only in
 * testing, as the directory traversal can be confusing, but is also needed to
 * pass in pathnames as parameters for other functions in the program.
 *
 *
 * @returns path - string containing the current working directory
 *
 *****************************************************************************/
string get_pathname()
{
    char buffer[1024];
    string path;

    getcwd(buffer, sizeof(buffer));
    //printf("In %s\n", buffer);
    path = buffer;

    return path;
}

/**************************************************************************//**
 * @author Julian Brackins
 *
 * @par Description:
 * String mutation to create a file of the same name, but with a different
 * extension.
 *
 * @param[in] test_case - string containing file name and extension
 * @param[in] ext - new file name extension
 *
 * @returns temp - new string with extension the same as the ext param.
 *
 *****************************************************************************/
string case_name(string test_case, string ext)
{
    char buffer [20];
    int n;
    string temp(test_case.begin(), test_case.end()-4);

    //get a new extension (brought in by second parameter)
    if ( ext.compare("tst") == 0)
        temp += ".tst";
    else if ( ext.compare("ans") == 0)
        temp += ".ans";
    else if ( ext.compare("out") == 0)
        temp += ".out";
    else if ( ext.compare("tmp") == 0)
        temp += ".tmp";
    else if ( ext.compare("log") == 0)
    {
        //HANDLE TIMESTAMP
        temp += ".log";
    }
    else
        cout << "Please indicate an extension in second parameter...\n";
    return temp;
}

/**************************************************************************//**
 * @author Julian Brackins
 *
 * @par Description:
 * Construct a string with the current date and time.
 *
 * @returns ymdt - date string with year_month_date_time format
 *
 *****************************************************************************/
string timestamp()
{
    time_t now = time(0);
    struct tm tstruct;
    char buffer[80];
    tstruct = *localtime(&now);

    strftime(buffer, sizeof(buffer), "%Y_%m_%d_%X", &tstruct);

    string ymdt( str_replace(buffer, ':', '_') );
    return ymdt;
}

/**************************************************************************//**
 * @author Julian Brackins
 *
 * @par Description:
 * Replaces every character in a string that matches the first parameter (a)
 * with the second parameter character (b)
 *
 * @param[in] a - character to be replaced
 * @param[in] b - character to replace a
 *
 * @returns str - new string with replaced characters
 *
 *****************************************************************************/
string str_replace(string str, char a, char b)
{
    for (int i = 0; i < str.length(); ++i)
    {
        if (str[i] == a)
            str[i] = b;
    }
    return str;
}

/**************************************************************************//**
 * @author Julian Brackins
 *
 * @par Description:
 * Event loop to handle a user interface for the testing suite rather than
 * command line usage. not used in sprint 1.
 *
 * @returns true  - basically anything EXCEPT "exit" was sent to the console
 * @returns false - "exit" was sent to console
 *
 *****************************************************************************/
bool event_loop()
{
    /*EVENT LOOP IS NOT
      USED FOR SPRINT 1*/

    string input;
    string arg;
    char* command;
    char* filename;
    char buffer[100];

    cout << ">> ";             //prompt

    //read in commands, break up arguments into tokens
    fgets(buffer,100, stdin);
    command = strtok(buffer," \n");
    filename = strtok(NULL, " \n");

    //Check to see if NULL command was sent from console
    if(command != NULL)
    {
        input = command;

        //test <filename>
        //handle improper commands here as well
        if(input.compare("test") == 0)
        {
            if(filename != NULL)
            {
                arg = filename;
                //cout << "testing " << arg << "\n";

                test_loop(arg);
            }
            else
                cout << "Invalid Input!!\n";
        }
        //Print list of "program" folders
        if(input.compare("dir") == 0)
            dir_list();
        //Exit
        if(input.compare("exit") == 0)
        {
            cout << "Exiting Program...\n";
            return false;
        }
    }
    return true;
}

string student_log_file(string str)
{
    str = str.substr(0, str.find_last_of("."));
    str = log_filename(str);
    return str;
}

/**************************************************************************//**
 * @author Julian Brackins
 *
 * @par Description:
 * Prints a list of all folders in the current directory. This is useful for
 * the ./grade user, as it lists all directories that could contain programs
 * to test.
 * A modified version of vector_directories(), sans the recursion.
 *
 * @returns none
 *
 *****************************************************************************/
void dir_list()
{
    /*dir_list is not used
      in sprint 1*/

    DIR *dp;
    struct dirent *dirp;
    string path(get_pathname());
    path += "/";
    string file_name;

    if ((dp = opendir(path.c_str())) == NULL)
    {
        cout << "Error opening directory...\n";
        return;
    }
    else
    {
        cout << "Directory List:\n\n";
        while ((dirp = readdir(dp)) != NULL)
        {
            if (dirp->d_name != string(".") && dirp->d_name != string(".."))
            {
                if (is_dir(path + dirp->d_name) == true)
                {
                    file_name =  dirp->d_name;
                    cout << file_name << "\n";
                }
            }
        }
        closedir(dp);
    }
    cout << "\n";
}


