#include "dir_traversal.h"

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