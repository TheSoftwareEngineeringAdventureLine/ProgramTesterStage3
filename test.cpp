/*************************************************************************//**
 * @file
 *
 * @mainpage Automated Grading System
 *
 * @section course_section CSC 470
 *
 * @author Ben Sherman, James Tillma, Anthony Morast
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
 *
 * @bug none
 *
 * @todo Sprint 1 complete
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

#include <stdio.h>
#include <math.h>
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
#include <sstream>
#include <ctime>        //Handling Timestamps
#include "string_ops.h"
#include "dir_traversal.h"

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
bool test_loop(string cpp_file, bool generate);
int count_case();
int result_compare(string test_file);
bool isGolden(string& golden_name, string &path, string home);
bool test_code(string cpp_file, vector<string> test_cases, int &total, ofstream &fout);
void generateFiles(string testPath, string goldenName);
void generate_ans(string cpp_file, string test_case);

/*Grade calculation*/
double grade_percent(int right, int total);

/**************************************************************************//**
 * @authors Julian Brackins, Benjamin Sherman, James Tillma, & Anthony Morast
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
 * @param[in] test_case - string with test case file name path
 *
 * @returns result_compare(test_case) - 0 if test fails, 1 if test succeeds
 *****************************************************************************/
int run_file(string cpp_file, string test_case)
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
    string log_name(log_filename(class_folder));
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
    string progpath(get_pathname() + "/" + class_folder + "/"); //string with path to prog file

    if(generate)
    {
        golden_dir = homepath + class_folder + "/";	//create path to find golden.cpp
        //golden = true if golden.cpp exists, golden.cpp filename will be golden_name
        bool golden = isGolden(golden_name, golden_dir, get_pathname());

        //call James' function here!
        if (golden)
            generateFiles(homepath + class_folder, golden_name);
        else
        {
            cout << "\n\nNo golden.cpp found in class directory.\n\n";
            cout << "Skipping test case generation . . .\n" << endl;
        }
    }

    vector_directories(class_folder, sub_dir);   //place all subdirectory names in vector
    //Open summary log file
    fout.open((homepath + class_folder + "/" + log_name).c_str());    //open file

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
    for(int i = 0; i < j; i++) //test_cases is empty if done testing home directory
    {
        fout << test_cases[i] << ": ";
        if ( run_file(cpp_file, test_cases[i]) == 1)
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
 * @author James Tillma
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
    string type, tempPath;
    string generatedNameBase = "/GENERATED_TEST_FILE_";
    string tst = ".tst";
    string ans = ".ans";
    string toTest;
    float min, max;
    int numCases, numFiles;
    char temp[6];
    ofstream testFout;
    ifstream openTest;
    float tempRand;
    ostringstream convert;

    compile_file(goldenName);
    cout << "Choose a type of test case, \"int\" or \"float\":" << endl;
    do
    {
        cin.getline(temp, 6);
        type = temp;
        if( type != integer && type != floating )
        {
            cout << "Enter either \"int\" or \"float\" to use as the type for test cases: " << endl;
        }
    } while (type != integer && type != floating );

    cout<< "Enter a number of test cases to use per file: " << endl;
    cin >> numCases;

    cout<< "Enter the minimum value to use in test-case creation: " << endl;
    cin >> min;

    cout << "Enter the maximum value to use in test-case creation: " << endl;
    do
    {
        cin >> max;
        if( max <= min )
        {
            cout << "The maximum must be greater than the minimum, re-enter the maximum: " << endl;
        }
    } while ( max <= min );

    cout << "Enter the number of test case files to create: " << endl;
    cin >> numFiles;

    srand(time(NULL));

    for( int i = 0; i < numFiles; i++)
    {
        convert << i;
        tempPath = testPath + generatedNameBase + convert.str();
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
        for( int j = 0; j < numCases; j++ )
        {
            while ( type == floating && tempRand == 0)
            {
                tempRand = float(rand()) / RAND_MAX;
                if ( tempRand == 1)
                    tempRand--;
            }
            if(type == floating)
                testFout << tempRand + fmod((float)rand(), ((min - max) + min)) << endl;
            else
                testFout << rand() % (int)((min - max) + min) << endl;
        }
        convert.str("");
        generate_ans(goldenName, toTest);
        testFout.close();
    }

    change_dir(homepath);
}
