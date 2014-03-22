#ifndef STRING_OPS_H
#define STRING_OPS_H

#define CURR_VER "2.0"
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
#include <sstream>
#include <ctime>        //Handling Timestamps
#include "dir_traversal.h"


using namespace std;

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
string log_filename(string cpp_file);

/*Usage Statements*/
void usage();
void err_usage(); 

/*Grade Letter*/
string grade_letter(double grade_percent);

#endif