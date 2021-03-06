/*************************************************************************//**
 * @file
 *
 * @mainpage Automated Grading System
 *
 * @section course_section CSC 470
 *
 * @author Andrew Koc, Erik Hattervig, Jonathan Tomes
 *
 * @date March 23,2014
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
 * test.cpp
 *
 * @section compile_section Compiling and Usage
 *
 * @par Compiling Instructions:
 *      g++ -o test test.cpp -g or make
 *
 * @par Usage:
   @verbatim
   ./test <directory>

	NOTE: <directory> should be a directory containing student source codes
    that are in their own folder and
	.tst and .ans files.

   @endverbatim
 *
 * @section todo_bugs_modification_section Todo, Bugs, and Modifications
 * @todo This program still does not have gcov, or gprof support
 * to allow for more profiling
 * @bug This program doesn't have infinite loop stopping for the files
 * to be tested, it wasn't pipeing the output, and would also kill this
 * program rather than just the program being tested
 * @bug This code doesn't do string generation because when selected the
 * code freezes up, a reson for this has not been found
 *
 * @par Modifications and Development Timeline:
   @verbatim
   Date          Modification
   ------------  --------------------------------------------------------------

   @endverbatim
 *
 *****************************************************************************/

/*************************************************************************//**
*********************************INCLUDES*************************************
******************************************************************************/

#define CURR_VER "2.0"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>     //cout cin
#include <fstream>      //file i/o
#include <string>       //basic string handling. cstyle also used for directory
#include <string.h>     //cstring handeling.
#include <dirent.h>     //
#include <cstring>      //
#include<sys/stat.h>    //Handling directory traversal
#include<sys/types.h>   //
#include <vector>        //
#include <sstream>
#include <ctime>        //Handling Timestamps

#include <sys/wait.h>   //handling wait function.

#include <ctype.h>
#include <algorithm>
#include <cmath>

#include <cstdio>

#include <fcntl.h>

/*************************************************************************//**
*********************************NAMESPACE************************************
******************************************************************************/

using namespace std;

/*************************************************************************//**
********************************FUNCTION PROTOTYPES***************************
******************************************************************************/
/* Menu Functions */
void mainMenu( string root);


/*Compiling and Running File*/
int compile_file(string cpp_file);
int run_file(string cpp_file, string test_case);

/*Testing Code*/
bool test_loop(string cpp_file, bool generate);
int count_case();
int result_compare(string test_file);
bool isGolden(string& golden_name, string &path, string home);
bool test_code(string cpp_file, vector<string> test_cases, int &total, ofstream &fout);
void generateFiles(string testPath, string goldenName);
void generate_ans(string cpp_file, string test_case);


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
string student_log_file(string source);
string student_name(string source);
string format_argv(char *argv);

/*Not used in Sprint 1*/
//bool event_loop();
void dir_list();

/* Menu Driven Testing */
bool specFileCheck( string &specName );
void generateMenuTestCases( string goldenName, string specName );

/*Checking For Presentation Errors*/
bool isNumber (string s);
double stringToFloat(string s);
void stringtolower(string &s);
bool cmpString(string s1, string s2);
double roundToPlace(double val, int n);
bool cmpNum(string s1, string s2);
bool cmpFiles(string s1, string s2);

/**************************************************************************//**
 * @authors Julian Brackins, Benjamin Sherman, James Tillma, Anthony Morast,
 *              Jonathan Tomes
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

    //Commented out the old code for now.
    // we can get rid of it if we don't need it.
    /*
    usage();
    bool generate;
    // user only entered a directory argument
    if(argc == 2)
    {
        cout << "Testing " << argv[1] << "...\n" ;
        generate = false;
    }
    // user entered a directory and generate tests flag
    else if(argc == 3 && strcmp(argv[2], "-g") == 0)
    {
        cout << "**************GENERATING TEST CASES***************" << endl;
        generate = true;
    }

    else
    {
        err_usage();
        return -1;
    }
    test_loop(format_argv(argv[1]), generate);
     * */
    
    //user entered too many arguments
    
    string root;
    
    if(argc > 2 )
    {
        cout << "Error in usage,  correct usage: " << argv[0] 
                << " <optional: path to directory>" << endl;
        return 0;
    }
    
    char cCurrentPath[FILENAME_MAX];
    
    getcwd(cCurrentPath, sizeof(cCurrentPath) );
    string current = cCurrentPath;
    
    if( argc == 2 )
    {
        //get the directory
        string directory = argv[1];
        
        //check for a full path. Full Paths should start with a "/"
        if( directory[0]=='/')
        {
            root = directory;
        }
        else //the path specified is a relative path.
        {
            root = current + "/" + argv[1];
        }
        
    }
    else if( argc == 1) //Should use the current directory if one is not
                        //provided.
    {
        root = current;
    }
    char * temp = strcpy( cCurrentPath, root.c_str() );
    root = format_argv( cCurrentPath );
    mainMenu(root);
    

    return 0;
}

/*******************************************************************************
 *@author Jonathan Tomes
 *@par Description:
 *A basic main menu function. Handles user input and interaction. Ask the
 * user if they want to run tests, generate tests, or exit.                                                                         
 ******************************************************************************/
void mainMenu( string root )
{
    int choice = 0;
    bool generate;
    
    while(choice != 3 )
    {
        cout << "Welcome to the Program Testing Application!"<<endl;
        cout << "Testing in: " << root << endl;
        cout << "Please chose a number: " << endl;
        cout << "1) Run Tests" << endl;
        cout << "2) Generate Test Cases" << endl;
        cout << "3) Exit" << endl;
        cin >> choice;
        
        if( choice == 1)
        {
            cout << "Running Tests..." << endl;
            generate = false;
            test_loop( root, generate );
        }
        else if( choice == 2 )
        {
            cout << "Running test generation..." << endl;
            generate = true;
            test_loop( root, generate );
        }
        else if( choice == 3 )
        {
            cout << "Exiting..." << endl;
            
        }
        else
        {
            cout << "Not a valid option. please enter 1, 2, or 3" << endl;
            choice = 0;
        }
    }
    
    return;
}

/**************************************************************************//**
 * @author Julian Brackins
 *
 * @par Description:
 * Function Edited by Andrew Koc
 * Using C++ String manipulation, a g++ compile command is sent to the
 * terminal in order to compile the file brought in by the argument
 * cpp_file.
 *
 * The compile line is as follows
 * compile_file(example.cpp);
 * "g++ -pg -fprofile-arcs -ftest-coverage example.cpp example"
 *
 * @param[in] cpp_file - name of .cpp file to be compiled by program
 *
 * @returns system(buffer.c_str()) - 0 if compile worked, 1024 if failed
 *
 *****************************************************************************/
int compile_file(string cpp_file)
{
    //g++ -pg -fprofile-arcs -ftest-coverage file.cpp -o file
    string buffer("g++ -pg -fprofile-arcs -ftest-coverage");
    string out_name = cpp_file;
    out_name.erase(out_name.length()-4);
    buffer += " " + cpp_file + " -o " + out_name;
    return system(buffer.c_str());
}

/**************************************************************************//**
 * @author Julian Brackins, Jonathan Tomes
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
 * @param[in] test_case - string with test case file name path
 *
 * @returns result_compare(test_case) - 0 if test fails, 1 if test succeeds
 *****************************************************************************/
int run_file(string cpp_file, string test_case)
{
    //child process id
    int childPid = 0;
    //Process id for wait
    int wait_pid = 0;
    //child exit status.
    int status;
    //timer to wait
    int timer;
    const int MAX_TIME = 10;
    bool timeLimit = false;
    
    //file pointers
    int fpt1, fpt2;
    
    
    
    string run_cmd("./");
    int result;
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
    
    //Commenting out because no longer needed. The program
    // needs to be ran using fork and execv calls to catch
    // infinite loops.
 

    buffer1 += run_cmd + buffer2 + test_case + buffer3 + case_out;
    system(buffer1.c_str());
    
    
    //fork from the parent process.

    //Infinite Loop test NOT WORKING.
    //commented out so still viewable... but going back to original code.
    



    /*
    childPid = fork();
    
    
    if( childPid = 0 )
    {
        //construct the arguments for the command execv;
        buffer1 += run_cmd;
        char * args[4];
        strcpy(args[0], buffer1.c_str() );
        
         
       
        if ((fpt1 = open(test_case.c_str(), O_RDONLY)) == -1)
        {
                cerr << "Unable to open '" << test_case 
                        << "' for reading." << endl;
                exit(-1);
        }
        close(0);       // close child standard input
        dup(fpt1);      // redirect the child input
        close(fpt1);    // close unnecessary file descriptor
        
        if ((fpt2 = creat(case_out.c_str(), 0644)) == -1)
          {
                cerr << "Unable to open '" << case_out
                        << "' for writing." << endl;
                exit(-1);
          }
        close(1);       // close child standard output 
        dup(fpt2);      // redirect the child output 
        close(fpt2);    // close unnecessary file descriptor
        
       
        execl( args[0], args[0], 0);
        //if the execv fails.. this line will be displayed.
        cout << "Execv command failed to execute!" << endl;
        exit(-1);
    }
    
    timeLimit = false;
    timer = 0;
    while(true)
    {
        sleep(1);
        timer++;
        wait_pid= waitpid(childPid, &status, WNOHANG );
        
        //child exited normally
        if( wait_pid != 0 )
        {
            break;
        }
        
        //Time limit exceeded, set result to fail
        //kill the child process
        if( timer >= MAX_TIME )
        {
            timeLimit = true;
            kill(childPid, 9 );
        }
    }
    */
    

    //0 = Fail, 1 = Pass, 2 = Pass with presentation error
    //timeLimit will always be false for now, while
    //the infinite loop code is commented out.
    //couts were for debugging purposes.
    if( !timeLimit )
    {
        //cout << "Did not run over time limit" << endl;
        result =  result_compare(case_out);
        //cout << result << endl;
    }
    else
    {
        //cout << "Ran over time limit." << endl;
        result = 0;
        //cout << result << endl;
    }
    string remove("rm " + case_out);
    system(remove.c_str());

    

    return result;
}

/**************************************************************************//**
 * @authors Julian Brackins, James Tillma, Anthony Morast & Benjamin Sherman
 *
 * @par Description:
 * Using C++ String manipulation, a command is sent to the terminal in
 * order to run the file brought in by the argument cpp_file
 * String buffers are used to handle piping both for inputting
 * and outputting
 *
 * The run line sent to system() is as follows
 * run_file(example, case_x.tst);
 * <full_path>/./example < case_x.tst > case_x.out
 *
 * @param[in] cpp_file - name of program file to be run
 * @param[in] test_case - string with test case file name path
 *****************************************************************************/
void generate_ans(string cpp_file, string test_case)
{
    string run_cmd("./");
    cpp_file = student_name(cpp_file);
    run_cmd += cpp_file;
    //create .ans file name
    string case_out(case_name(test_case, "ans"));

    //set up piping buffers
    string buffer1("");
    string buffer2(" &> /dev/null < ");
    string buffer3(" > ");

    // "try using | "
    //construct run command, then send to system
    //./<filename> &> /dev/null  < case_x.tst > case_x.ans
    buffer1 += run_cmd + buffer2 + test_case + buffer3 + case_out;

    system(buffer1.c_str());
}
/**************************************************************************//**
 * @author Benjamin Sherman
 *
 * @par Description:
 * This function is given the commandline argument and removes any appending
 * slash. It returns the argument as a c++ string.
 *
 * @param[in] argv - commandline argument
 *
 * @returns class_folder - properly formatted commandline argument
 *
 *****************************************************************************/
string format_argv(char *argv)
{
    string class_folder = argv;
    char last_char = class_folder[class_folder.length() - 1];
    if(last_char == '/')
        class_folder = class_folder.substr(0, class_folder.length() - 1);

    return class_folder;


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
 * -change to directories where test cases are located and store path
 *    to test cases and critical test cases
 * -sequentially test each each students source code against the test cases
 *      -test code against critical test cases
 *      -if student code passed all critical test cases
 *          -finish testing on normal test cases
 *          -record what percentage of test cases student PASSED
 *      -else
 *          -record that student FAILED, no percentage grade
 *

 *
 * @param[in] class_folder - name of directory that contains the source
 * code for each student in a CSC_150 class
 * @param[in] generate - flag that tells this function whether to
 * generate test cases or not
 *
 * @returns true - testing completed
 * @returns false - student source code failed to compile during testing
 *
 *****************************************************************************/
bool test_loop(string class_folder, bool generate)
{
    int test_cases_temp = 0;
    int test_cases_total = 0;
    int i;
    int total = 0;
    bool passed;

    ofstream fout, student_fout;
    string log_name(log_filename(class_folder.substr(class_folder.find_last_of('/'))));
    string golden_name;  						//if there is a .cpp, golden exists
    string golden_dir;	//if golden.cpp exists it exists in golden_dir
    string student_logname;
    
    string specName;

    vector<string> sub_dir;                  //vector of all the subdirectories
    vector<string> test_cases;               //vector of test cases in current directory
    vector<string> _source;					//student source code in the directory
    vector<string> source_paths;				//path to student source code
    vector<string> crit_cases;

    string homepath = class_folder + "/";  //create string with home path name

    if(generate)
    {
        golden_dir = homepath;	//create path to find golden.cpp
        //golden = true if golden.cpp exists, golden.cpp filename will be golden_name
        bool golden = isGolden(golden_name, golden_dir, homepath);

        //call James' function here!
        if (golden)
		{
            // test for if there is a .spec file
            if ( specFileCheck( specName ) == true )
            {
                generateMenuTestCases( golden_name , specName );
            }
            else
            {
                generateFiles(class_folder, golden_name);
            }
		}
        else
            cout << "\nNo golden source code found. \nSkipping test case generation . . ." << endl;
    }

    vector_directories(class_folder, sub_dir);   //place all subdirectory names in vector
    //Open summary log file
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
            change_dir(sub_dir.back()); //change to next subdirectory in vector

            vector_test_cases(test_cases, crit_cases); //vector .tst files in current directory

            test_cases_temp = count_case();         //count the number of .tst files... MIGHT REMOVE
            test_cases_total += test_cases_temp;

            if(test_cases.size() != 0)
            {
                student_fout << "In directory " << sub_dir.back() << ":\n";

            }

            change_dir (homepath);
        }
        sub_dir.pop_back();                          //remove sub directory from vector
    }

    //for each source file in _source, do the following
    i = _source.size();
    for (int j = 0; j < i; j++)
    {
        cout << "\nTesting " << student_name(_source.back()) << endl;
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
            passed = test_code(_source.back(), crit_cases, total, student_fout);

        if(passed)
            if(test_cases.size() > 0)
                test_code(_source.back(), test_cases, total, student_fout);

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

        //close student logfile
        student_fout.close();
        cout << "Done.\n";

        _source.pop_back();
        source_paths.pop_back();


    }
    //close summary logfile
    fout.close();
    return true;
}

/**************************************************************************//**
 * @author Benjamin Sherman
 *
 * @par Description:
 * The algorithm, given a students source code name, returns the name of the
 * file without the extension.
 *
 * @param[in] source - name of a student source code file
 *
 * @returns name of student source code file without extension
 *
 *****************************************************************************/
string student_name(string source)
{
    return source.substr(0, source.find_last_of("."));
}

/**************************************************************************//**
 * @author Benjamin Sherman
 *
 * @par Description:
 * Function Edited by Andrew Koc
 *
 * The algorithm, given a student source code file name and a vector of test
 * cases will test the students code on every test case in the vector.
 *
 * The file output stream "fout" is passed by reference to write to a student
 * logfile. Total is passed by reference to keep track of the number of tests
 * a students code has passed. "passed" is returned and stores whether a
 * student failed any test cases in the vector "test_cases".
 *
 * @param[in] cpp_file - name of student source code file to be tested
 * @param[in] total - used to keep track of the total number of test cases a
 * student's source code has undergone
 * @param[in] four - student's logfile with detailed information on whether
 * a student passed or failed
 *
 * @returns true - student source code passed all test cases
 * @returns false - student source code failed at least one test case
 *
 *****************************************************************************/
bool test_code(string cpp_file, vector<string> test_cases, int &total, ofstream &fout)
{
    bool passed = true;
    int j = test_cases.size();
    int pass_value;
    for(int i = 0; i < j; i++) //test_cases is empty if done testing home directory
    {
        fout << test_cases[i] << ": ";
        pass_value = run_file(cpp_file, test_cases[i]);
        if ( pass_value == 1)
        {
            total += 1;
            fout << "PASSED\n";
        }
        else if ( pass_value == 2 )
        {
            total += 1;
            fout << "PASSED*\n";
        }
        else
        {
            fout << "FAILED\n";
            passed = false;
        }
    }
    return passed;
}

/**************************************************************************//**
 * @author Anthony Morast
 *
 * @par Description
 * This function fills a vector with the source code for each student in the
 * grading directory. Another vector is used to store the paths to each students
 * source code. This is obviously set up in such a way that the position in
 * both vectors correspond to the same student.
 *
 * @param[in] vector source: stores name of the students source code
 * @param[in] string new_dir: directory to move into to look for student code
 * @param[in] string home: home directory, switch back at the end
 * @param[in] vector source_paths: path to each students source code
 *
 * @returns None
 *****************************************************************************/
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

/**************************************************************************//**
 * @author Anthony Morast
 *
 * @par Description
 * This function determines whether or not a .cpp file exists in the home
 * directory. If there is a .cpp file in the home directory this is deemed the
 * golden cpp. The name of the golden.cpp file, the path to the file, and a
 * boolean to determine whether or not the .cpp is there are "returned" from
 * this function.
 *
 * @param[in] golden_name: holds the name of the golden.cpp file
 * @param[in] path: path to change into, should be directory with student
 *					directories,test directories, etc.
 * @param[in] home: home directory, switch back to at end.
 *
 * @returns true: there is a golden.cpp file
 * @returns			false: there is not a golden.cpp file
 *****************************************************************************/
bool isGolden(string& golden_name, string& path, string home)
{
    string name;

    change_dir(path); //change into directory

    get_golden(name); //determine if there's a .cpp file

    if (name.length() > 0)	//if there is a .cpp file
    {
        golden_name = name;		//set golden name to the .cpp file name
        path = get_pathname();  //pat to golden.cpp..........
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
 * Function Slightly edited by Andrew Koc.
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
 * @returns 2 - Test passed with presentation errors
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
    {
        if (cmpFiles(case_ans, case_out))
            return 2;
        return 0;
    }
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

/**************************************************************************//**
 * @author Anthony Morast
 *
 * @par Description
 * This function walks through the root directory and determines if there is
 * a .cpp file in the directory. If there is it is returned to the isGolden
 * function via source_file.
 *
 * @param[in] source_file - stores the name of the cpp source file found
 *
 * @returns None
 *****************************************************************************/
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
                if (is_dir(path + dirp->d_name) == false)    //NOT a directory
                {
                    file_name =  dirp->d_name;
                    string ext (file_name.end()-4, file_name.end());//get ext
                    if(ext.compare(".cpp") == 0)        //check if it's a .cpp
                    {
                        //cout << file_name << "\n";
                        source_file = file_name; //add .cpp file name to vector
                    }
                }
            }
        }
        closedir(dp);
    }
}
/**************************************************************************//**
 * @author Anthony Morast
 *
 * @par Description
 * This function walks through each directory and determines if there is a .cpp
 * file named after the name of the directoty. If so this is a student's
 * directory. The cpp file is set to source_file and returned to get source
 * to be queued.
 *
 * @param[in] source_file - stores the name of the cpp source file found
 *
 * @returns None
 *****************************************************************************/
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
                if (is_dir(path + dirp->d_name) == false)  //NOT a directory
                {
                    file_name =  dirp->d_name;
                    if(file_name == student_name)     //check if it's a .cpp
                        source_file = file_name; //add .cpp file name to queue
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
                //it's a directory
                if (is_dir(base_dir + dirp->d_name) == true)
                {
                    path = base_dir + dirp->d_name + "/";
                    vector.push_back(path);     //push string into vector
                    //recursion!!
                    vector_directories(base_dir + dirp->d_name, vector);
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
        while ((dirp = readdir(dp)) != NULL)
        {
            new_path = path;
            if (dirp->d_name != string(".") && dirp->d_name != string(".."))
            {
                if (is_dir(path + dirp->d_name) == false)   //NOT a directory
                {
                    file_name =  dirp->d_name;
                    string ext (file_name.end()-4, file_name.end());  //get ext
                    if(ext.compare(".tst") == 0)    //check if it's a .tst
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
                            case_vector.push_back(new_path);  //add .tst file name to vector

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
    cout << "\n*************AUTOMATED GRADING SYSTEM*************\n";
    cout << "*********************ver  ";
    cout << CURR_VER << "*********************\n";
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
    cout << "./test <class_directory> | -g";
    cout << "                           -g will generate test cases.\n";
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
 * @author Benjamin Sherman
 *
 * @par Description:
 * The algorithm given a student source code, removes the ".cpp" extension and
 * appends ".log" extenstion. The function is used to generate a log file name
 * for each student.
 *
 * @param[in] source - name of a student source code file
 *
 * @returns name of student source code file without extension
 *
 *****************************************************************************/
string student_log_file(string source)
{
    source = source.substr(0, source.find_last_of("."));
    source = log_filename(source);
    return source;
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
}

/**************************************************************************//**
 * @author James Tillma, Jonathan Tomes
 *
 * @par Description
 * This function handles the generating of a number of test cases. It handles
 * everything associated with this including: getting user options, creating
 * the test output files, compiling the "golden.cpp" file and creating the
 * solutions files associated with each test case. Not that the number,
 * type, and a range for the values to be generated are all scpecified by user
 * console input.
 *
 * @param[in] testPath - contains the path to store the test and solution files
 * @param[in] goldenName - contains the name (including extension) for the
 *						   valid cpp file
 *
 * @returns None
 *****************************************************************************/
void generateFiles(string testPath, string goldenName)
{
    string homepath = get_pathname();
    change_dir(testPath);
    const string integer = "int";
    const string floating = "float";
    string type, tempPath, toTest;
    string generatedNameBase = "/GENERATED_TEST_FILE_";
    string tst = ".tst";
    string ans = ".ans";
    float min, max, tempRand = 0;
    int numCases, numFiles;
    char temp[20];
    ofstream testFout;
    ifstream openTest;
    ostringstream convert;
    
    //added by Jonathan Tomes
    int choice = 0;

	system("mkdir test &>/dev/null"); //make the directory to store test files if it doesnt exist


    compile_file(goldenName);
    
    //Jonathan Tomes going to comment out this first chunk
    // to make it easier to modify and add different types.
    //get the type of test case to use (used in random number generation)
    /*
    cout << "Choose a type of test case, \"int\" or \"float\":" << endl;
    do
    {
        cin.getline(temp, 100);
        type = temp;
        if( type != integer && type != floating )
        {
            cout << "Enter either \"int\" or \"float\" to use as the type for test cases: " << endl;
        }
    } while (type != integer && type != floating );
    */
    while( choice == 0)
    {
        cout << "Please choose a type of test: " << endl;
        cout << " 1) integer" << endl;
        cout << " 2) floating point" << endl;
        cout << " 3) character string" << endl;
        cin >> choice;
        cin.clear();
        if ( choice != 1 && choice != 2 && choice != 3)
        {
            cout << "Please enter 1, 2, or 3." << endl;
            choice = 0;
        }
        
    }
    
    if( choice == 1 || choice == 2 )
    {
            if( choice == 1 )
                type = integer;
            if( choice == 2 )
                type = floating;
        //get the number of test cases to generate per file
        cout<< "Enter a number of test cases to use per file: " << endl;
        do
        {
            cin.getline(temp, 100);
            numCases = atoi(temp);
            if ( numCases <= 0 )
            {
                cout << "The number of cases must be positive, please re-enter: " << endl;
            }
        } while ( numCases <= 0);
        //get the minimum value to use in test-case creation
        cout<< "Enter the minimum value to use in test-case creation: " << endl;
        cin.getline(temp, 100);
        min = atof(temp);
        //get the theoretical maximum value
        cout << "Enter the maximum value to use in test-case creation: " << endl;
        do
        {
            cin.getline(temp, 100);
            max = atof(temp);
            if( max <= min )
            {
                cout << "The maximum must be greater than the minimum, re-enter the maximum: " << endl;
            }
        } while ( max <= min );
        //get the number of files to create
        cout << "Enter the number of test case files to create: " << endl;
        do
        {
            cin.getline(temp, 100);
            numFiles = atoi(temp);
            if ( numFiles <= 0)
            {
                cout << "The number of files to generate must be positive, re-enter the number: " << endl;
            }
        } while ( numFiles <= 0 );
        srand(time(NULL));


        //added to the path name to put .tst files in the test directory created above
        string testDir = "/test";

        for( int i = 0; i < numFiles; i++)
        {
            convert << i;
            tempPath = testPath + testDir + generatedNameBase + convert.str();
            toTest = tempPath;
            toTest += tst;
            openTest.open( toTest.c_str() );
            // this handles naming conflicts
            if(openTest)
            {
                numFiles++;
                openTest.close();
                convert.str("");
                continue;
            }
            openTest.close();
            testFout.open( toTest.c_str() );
            //generate random values and output to test file
            for( int j = 0; j < numCases; j++ )
            {
                while ( type == floating && tempRand == 0)
                {
                    tempRand = float (rand()) / float (RAND_MAX);
                    if ( tempRand == 1)
                        tempRand--;
                }
                if(type == floating)
                {
                    testFout << tempRand + fmod(rand(), (max - min)) + min << endl;
                    tempRand = 0;
                }
                else
                {
                    testFout << rand() % (int)(max - min) + (int)min << endl;
                }
            }
            convert.str("");
            generate_ans(goldenName, toTest);
            testFout.close();
        }
    }
    
    else if( choice == 3)
    {
       
        cout << "This function is nonfunctional" << endl;
        /*
        //Generate string tests here.
        choice = 0;
        int maxLength = 0;
        
        //variableLeng stores weather or not the strings should have
        // random lengtsh;
        bool variableLeng = false;
       
        // get the length and store it.
        while( choice < 1 || choice > 100);
        {
            cout<< "How long would you like the strings to be?" << endl;
            cout << "Please enter a number between 1 and 100: ";
            cin >> choice;
            cin.clear();
            if( choice < 1 || choice > 100 )
                cout << "Please choice a number between 1 and 100" << endl;
        }
        maxLength = choice;
        
        //Ask the user if they want the length to be absolute, or
        // up to the number specified.
        
        choice = 0;
        while( choice == 0 )
        {
            cout <<"Do you want the length to be EXACTLY "
                    << maxLength << "?" << endl;
            cout << "1) Yes I want my strings to be exactly " << maxLength 
                    << endl;
            cout << "2) No I want my strings to at most " << maxLength << endl;
            cin >> choice;
            cin.clear();
            if( choice == 1 )
            {
                variableLeng = false;
            }
            else if ( choice == 2)
            {
                variableLeng = true;
            }
            else
            {
                cout << "Please enter 1 or 2" << endl;
                choice = 0;
            }
        }
        
        choice = 0;
       //get the number of files to generate.
        while( choice == 0 )
        {
            cout << "How many test files do you want?" << endl;
            cout << "Please enter a number between 1 and 100: ";
            cin >> choice;
            cin.clear();
            if( choice < 1 || choice > 100 )
            {
                cout << "Please enter a number between 1 and 100!" << endl;
                choice = 0;
            }
        }
        
        numFiles = choice;
        
        //Start generating tests.
        srand(time(NULL));
        
        //stole a chunk of this from above... 
        // wouldn't need to if it was a separate function.
        //added to the path name to put .tst files 
        //in the test directory created above
	string testDir = "/test";
        
        for( int i = 0; i < numFiles; i++)
        {
                convert << i;
                tempPath = testPath + testDir + generatedNameBase + 
                        convert.str();
                toTest = tempPath;
                toTest += tst;
                openTest.open( toTest.c_str() );
                // this handles naming conflicts
                if(openTest)
                {
                        numFiles++;
                        openTest.close();
                        convert.str("");
                        continue;
                }
                openTest.close();
                testFout.open( toTest.c_str() );
                //End of stealing stuff from above.
                
                //Starting to generate random characters to a string.
                
                string testString = "";
                int length;
                
                if(variableLeng)//if the strings can be off different sizes
                {
                    length = rand() % maxLength +1; //make the length up to max
                }
                
                for( int j = 0; j < length; j++)
                {
                    //generate  random character.
                    char temp = (char) (rand() % 26 + 97);
                    testString += temp;
                }
                //write the string to the test file
                testFout << testString;
                
                //generate an answer file.
                convert.str("");
                generate_ans(goldenName, toTest);
                testFout.close();
        
        }
         * */
    }

    change_dir(homepath);
}

/**************************************************************************//**
 * @author Erik Hattervig
 * Modified from get_golden
 *
 * @par Description:
 * Checks for the presents of a .spec file to promped the program to generate
 * tests based on the spec file, if the .spec file is found it is returned to 
 * the parameter specName.
 * 
 * @param[out] specName - The name of the .spec file, returned if a .spec file
 * is found in the directory
 *
 * @return true - A .spec file was found in the current directory
******************************************************************************/
bool specFileCheck(string &specName )
{
    DIR *dp;
    struct dirent *dirp;
    string path(get_pathname());
    string slash = "/";
    path += slash;
    string file_name;

    if ((dp = opendir(path.c_str())) == NULL)
    {
        cout << "Error opening directory...\n";
        return false;
    }
    else
    {
        //cout << "files in: " << path << "\n";
        while ((dirp = readdir(dp)) != NULL)
        {
            if (dirp->d_name != string(".") && dirp->d_name != string(".."))
            {
                if (is_dir(path + dirp->d_name) == false)    //NOT a directory
                {
                    file_name =  dirp->d_name;
                    string ext (file_name.end()-5, file_name.end());//get ext
                    if(ext.compare(".spec") == 0)        //check if it's a .spec
                    {
                        specName = file_name;
                        closedir(dp);
                        return true;
                    }
                }
            }
        }
        closedir(dp);
    }
    return false;
}

/**************************************************************************//**
 * @author Erik Hattervig
 *
 * @par Description:
 * This function generates test cases for menu driven programs, this function
 * expects the name of the name of the golden .cpp file and the name of the 
 * .spec file ans input. This function should only be called if a .spec file 
 * is present. The function will prompt the user for the number of test cases
 * to generate, the the min and max of numbers that the user wants, and the 
 * length of strings that will be generated. The function then reads the input
 * from the .spec file and generates appropriate test cases based on the 
 * contents of the file.
 *
 * @param[in] goldenName - the name of the golden cpp file in the directory
 * @param[in] specName - the name of the .spec file in the directory
 *
 * @returns - none
******************************************************************************/
void generateMenuTestCases( string goldenName , string specName )
{
    int numTestCases;   // the number of test cases to generate, the user will
                        //   the number
    float temp;         // used for input
    ifstream specFile;  // file stream of the .spec file
    ofstream outFile;   // file stream of the current .tst file being writen to
    string pathName;    // path to the directory containing the generated .tst
                        //   files
    string homePath = get_pathname();   // the path to the current directory
    ostringstream convert;  // used in converting ints to strings for creating
                            //   .tst files
    string command;     // string used to create command line arguments
    string testString;  // string used in the creation of random strings
    int max;            // the maximum range of generated digits, user will 
                        //   input this
    int min;            // the minimum range of generated digits, user will
                        //   input this
    float tempRand;     // used in the generation of random floating point
                        //   numbers
    int stringSize;     // the length of random strings, user will input this
    int i, j;
    
    tempRand = 0;
    
    // print out message saying that spec file was detected
    cout << "Spec file detected, creating menu test cases" << endl;
    
    //open the spec file
    specFile.open( specName.c_str() );
    
    if ( specFile.is_open() == false )
    {
        cout << "Unable to open spec file" << endl;
        return;
    }
    
    // prompt for number of test cases to be used and get it
    cout << "Please enter the number of test cases to be generated: ";
    do
    {
        cin >> numTestCases;
        cout << endl;
        if( numTestCases <= 0 )
        {
            cout << "Enter a number greater then 0: ";
        }
        
    }while( numTestCases <= 0 );
    
    cout << "Enter the max for ints, floats, and doubles: ";
    cin >> max;
    cout << endl;
    
    cout << "Enter the min for ints, floats, and doubles: ";
    cin >> min;
    cout << endl;
    
    cout << "Enter the length of the strings: ";
    cin >> stringSize;
    cout << endl;
    
    
    for( i = 0 ; i < numTestCases ; i++ )
    {
        // open up output file
        convert << i;
        pathName = homePath + "/test/GENERATED_TEST_FILE_" + convert.str() + ".tst";
        outFile.open( pathName.c_str() );
        
        // set specFile to beginning 
        specFile.seekg( 0 , ios::beg );
        
        // output a test case of 0 for first menu option
        outFile << "0" << endl;
        
        //specFile >> command;
        while( specFile >> command )
        {
            if ( command == "int" )
            {
                // output a random int
                outFile << rand() % (max - min) + min << endl;
            }
            else if ( command == "float" || command == "double" )
            {
                // output a random float
                while ( tempRand == 0)
                {
                    tempRand = float (rand()) / float (RAND_MAX);
                    if ( tempRand == 1)
                        tempRand--;
                }
                    outFile << tempRand + fmod(rand(), (float)(max - min)) + (float)min << endl;
                    tempRand = 0;
            }
            else if ( command == "string" )
            {
                // ouput a random string
                for( int j = 0; j < stringSize; j++)
                {
                    //generate  random character.
                    char temp = (char) (rand() % 26 + 97);
                    testString += temp;
                }
                //write the string to the test file
                outFile << testString << endl;
            }
            else
            {
                // output the menu option to the tst
                outFile << command << endl;
            }
            
            
            //specFile >> command;
        }
        outFile.close();
        generate_ans( goldenName, pathName);

    }
    
    
    return;
}

/**************************************************************************//**
 * @author Andrew Koc
 *
 * @par Description:
 * Checks if a string passed in is a valid expression for a number.  Meaning it
 * consists of digits 0-9 one or fewer '.' and possibly starting with a '-'
 * denoting a negative number.
 *
 * @param[in] s - string to be tested
 *
 * @returns true - the string is a valid number
 * @returns false - the string is not a valid number
 *
 *****************************************************************************/
bool isNumber(string s)
{
    int i = 0;

    //boolean value to keep track of if a decimal has been found
    bool found_decimal = false;
    bool found_digit = false;

    //check if possible negative number
    if ( s[0] == '-' )
        i = 1;


    //for each letter in the string check if it is a number, or the first
    //decimal, else return false
    for ( i = i; i < s.length(); i++ )
    {
        //check if letter is a number 0-9
        if (isdigit(s[i]))
        {
            found_digit = true;
        }
        else
        {
            //check if letter is '.'
            if ( s[i] == '.' )
            {
                //if we have already found a decimal
                if (found_decimal)
                    return false;
                //We found a decimal set found_decimal to true;
                found_decimal = true;
            }
            //we found something that isn't 0-9 or a '.'
            else
                return false;
        }
    }

    if ( found_digit )
    {
        //String is a number return true;
        return true;
    }
    else
    {
        //string is '.' or "-." not actual numbers
        return false;
    }
}

/**************************************************************************//**
 * @author Andrew Koc
 *
 * @par Description:
 * This function takes a string and changes it into a double precision floating
 * point number.  It does this by reading each digit in from left to right
 * multiplying the current value by ten then adding the correct value for the
 * specified digit, until there are no more digits, or a '.' is hit.
 *
 * If a '.' is hit then it builds the decimal part of the number, starting
 * right to left adding the value of the digit then dividing by ten.  Then the
 * integer part and decimal part are added together and used as the return
 * value.
 *
 * @param[in] s - the string to be converted
 *
 * @returns double value of string s
 *
 *****************************************************************************/
double stringToFloat(string s)
{
    int i = 0;
    double val = 0;
    double dec = 0;
    bool negative = false;
    bool decimal_found = false;

    //check for negative numbers
    if ( s[0] == '-' )
    {
        negative = true;
        i = 1;
    }

    //Get integer value of the number
    for ( i = i; i < s.length(); i++ )
    {
        if ( s[i] == '.' )
        {
            decimal_found = true;
            i = s.length();
        }
        else
        {
            val = val * 10;
            val += s[i] - '0';
        }
    }

    //if you found a decimal work backwards to get the decimal part
    if ( decimal_found )
    {
        for ( i = s.length() - 1; s[i] != '.'; i-- )
        {
            dec += s[i] - '0';
            dec = dec / 10;
        }
    }

    //if the number should be negative, return the negative
    //of the absolute value, else return the absolute value
    val += dec;
    if ( negative == true )
        return -1 * val;
    else
        return val;
}

/**************************************************************************//**
 * @author Andrew Koc
 *
 * @par Description:
 * Using the tolower function this stringtolower takes a string and makes it
 * all lowercase.
 *
 * @param[in,out] s - string to be transformed
 *
 * @returns nothing
 *
 *****************************************************************************/
void stringtolower(string &s)
{
    int i;
    for ( i = 0; i < s.length(); i++ )
        s[i] = tolower(s[i]);
}

/**************************************************************************//**
 * @author Andrew Koc
 *
 * @par Description:
 * This function takes two strings and checks if they match, allowing for
 * presentation errors.  These errors include: Different capitalization,
 * If the first and last letters match they count as the same word, and if
 * both words contain the same letters they count as the same word.
 *
 * This is done by calling stringtolower on both strings then checking if they
 * have the same first and last letters, if they do the function returns true,
 * else it continues to the next test.  Then sort is called on both strings,
 * this will quickly tell if they have the same letters but in different orders
 * by then comparing the sorted strings.  If the sorted strings are the same
 * it returns true, else it returns false.
 *
 * @param[in] s1 - first string to be compared.
 * @param[in] s2 - second string to be compared.
 *
 * @returns true - strings match with given presentation errors
 * @returns false - strings do not match with given presentation errors
 *
 *****************************************************************************/
bool cmpString(string s1, string s2)
{
    stringtolower(s1);
    stringtolower(s2);

    //check if first and last letters are the same
    if ( s1[0] == s2[0] && s1[s1.length() - 1] == s2[s2.length() - 1] )
        return true;

    //check if they have the same letters, in the wrong order
    //sort the strings and then check if they are the same
    sort(s1.begin(), s1.end());
    sort(s2.begin(), s2.end());

    if ( s1 == s2 )
        return true;

    return false;
}

/**************************************************************************//**
 * @author Andrew Koc
 *
 * @par Description:
 * This function returns a rounded value of a double to a given decimal place.
 *
 * @param[in] val - the double value to be rounded
 * @param[in] n - the number of decimal places to be rounded to
 *
 * @returns the rounded value of the double passed in
 *
 *****************************************************************************/
double roundToPlace(double val, int n)
{
    double round = 5.0/10.0;
    if ( val < 0.0 )
        return ceil((val * pow(10, n)) - round) / pow(10, n);
    else
        return floor((val * pow(10, n)) + round) / pow(10, n);
}

/**************************************************************************//**
 * @author Andrew Koc
 *
 * @par Description:
 * This function takes two strings assumed to be valid number values and checks
 * if the second value rounds to the first value.  This is done by determining
 * the number of digits after the decimal exists in the first number, then
 * getting the double values of both strings and seeing if the second rounded
 * to the specified decimal place, is equal to the first number.
 *
 * @param[in] s1 - the string of the first number, this is used to determine
 *             the decimal place to round to
 * @param[in] s2 - the string of the second number, this gets rounded.
 *
 * @returns true - the second number rounds to the first number
 * @returns false - the second number does not round to the first number
 *
 *****************************************************************************/
bool cmpNum(string s1, string s2)
{
    int pos;
    double val1, val2;

    //look for decimal place
    size_t found = s1.find('.');

    //decimal place to round to will be the number of
    //letters left after the decimal
    //if there is no decimal then round to 0 decimal places
    if ( found == string::npos )
        pos = 0;
    else
        pos = s1.length() - found - 1;

    //get actual values to manipulate
    val1 = stringToFloat(s1);
    val2 = stringToFloat(s2);

    //if the second number rounds to the first
    //return true, else return false
    if( roundToPlace(val2,pos) == val1 )
        return true;
    else
        return false;
}

/**************************************************************************//**
 * @author Andrew Koc
 *
 * @par Description:
 * This function reads in two files word by word to see if they match, allowing
 * for presentation errors such as, misspelled words, covered by the cmpString
 * function, and rounding errors, covered by the cmpNum function.  If any set
 * of words don't match, by being the wrong word/number or being of different
 * types, one is a word the other is a number, or by having to little or
 * to much information in a file this function returns false.
 *
 * @param[in] s1 - name of the first file to be compared
 * @param[in] s2 - name of the second file to be compared
 *
 * @returns true - files match with presentation errors
 * @returns false - files do not match
 *
 *****************************************************************************/
bool cmpFiles(string s1, string s2)
{
    ifstream file1, file2;
    string in1, in2;

    file1.open(s1.c_str());
    file2.open(s2.c_str());
    if( !file1 || !file2 )
    {
        cout << "Files to be compared could not be opened" << endl;
        file1.close();
        file2.close();
        return false;
    }

    while (file1 >> in1)
    {
        if (file2 >> in2)
        {
            if(in1 != in2)
            {
                if(isNumber(in1))
                {
                    if(isNumber(in2))
                    {
                        //both inputs are numbers
                        //if in2 doesn't round to in1 file is wrong
                        if (!cmpNum(in1,in2))
                        {
                            //file failed
                            file1.close();
                            file2.close();
                            return false;
                        }
                    }
                    else
                    {
                        //in1 is number, in2 is not, file is wrong
                        //file failed
                        file1.close();
                        file2.close();
                        return false;
                    }
                }
                else
                {
                    if(isNumber(in2))
                    {
                        //in1 is a string in2 is a number, file is wrong
                        //file failed
                        file1.close();
                        file2.close();
                        return false;
                    }
                    else
                    {
                        //in1 and in2 are strings
                        //if in1 and in2 don't match with presentation errors
                        if (!cmpString(in1,in2))
                        {
                            //file failed
                            file1.close();
                            file2.close();
                            return false;
                        }
                    }
                }
            }
        }
        else
        {
            //file is missing information
            //file failed
            file1.close();
            file2.close();
            return false;
        }
    }

    //if the second file has additonal information
    if (file2 >> in2)
    {
        //file failed
        file1.close();
        file2.close();
        return false;
    }
    file1.close();
    file2.close();
    return true;
}
