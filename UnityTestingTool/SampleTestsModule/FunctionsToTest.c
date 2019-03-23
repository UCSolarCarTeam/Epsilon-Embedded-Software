#include "FunctionsToTest.h"

double averageNumbersInArray(double arr[], int length)
{
    int i;
    double sum = 0;

    for(i = 0; i < length; i++)
    {
        sum += arr[i];
    }
    return sum/(double)length;
}

double* sum2Arrays(double array1[], double array2[], const int sz)
{
    int i;
    for (i = 0; i < sz; i++)
    {
        array1[i] = array1[i] + array2[i];
    }
    
    return array1;
}
