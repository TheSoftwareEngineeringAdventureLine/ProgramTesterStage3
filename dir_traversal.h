#ifndef DIR_TRAVERSAL_H
#define DIR_TRAVERSAL_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <vector>
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
#include "string_ops.h"

using namespace std;

/*Directory Traversal Code*/
bool change_dir(string dir_name);
bool is_dir(string dir);
void vector_directories(string base_dir, vector<string>& vector);
void vector_test_cases(vector<string>& case_vector, vector<string> &crit_vector);
void get_source(string& source_file);
void student_source (vector<string>& source, string new_dir,string home,
                     vector<string>& source_path);
void get_golden(string& source_file);

#endif