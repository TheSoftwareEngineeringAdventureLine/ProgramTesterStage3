#include <iostream>
using namespace std;

//---------------------------------
// This program finds the smallest 
// of three numbers.  Use 3 different
// numbers for this discussion
//---------------------------------

int main()
  {
  
  float x;
  float y;
  float z;
  float max;

  // get the three numbers
  cerr << "Enter first number : ";
  cin >> x;
  cerr << "Enter second number : ";
  cin >>  y;
  cerr << "Enter third number " ;
  cin >> z;


  if ( x > y)
     max = x;
  else if ( z > x)
     max = z;
  else
     max = y;
 
 cout << max << endl;

  } // end main  
