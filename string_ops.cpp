#include "string_ops.h"


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
    cout << "./test <class_directory> | -g\n";
    cout << "                           -g will generate test cases.\n";
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