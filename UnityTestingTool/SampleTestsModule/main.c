#include <stdio.h>
#include "FunctionsToTest.h"
#include "unity.h"

void goodTest_averageNumbersInArray(void)
{
    printf("\nTesting averaging function - Should Pass\n");
    double testArray1[4] = {1.0, 3.0, 4.0, 5.0};
    double testArray2[5] = {2.0, 4.0, 6.0, 8.0, 2.0};
    TEST_ASSERT_EQUAL_FLOAT(3.25, averageNumbersInArray(testArray1, 4)); //Should pass
    TEST_ASSERT_FLOAT_WITHIN(0.05, 4.40, averageNumbersInArray(testArray2, 5)); //Should pass
}


void badTest_averageNumbersInArray(void)
{
    printf("\nTesting averaging function - Should Fail\n");
    double testArray1[4] = {1.0, 3.0, 4.0, 5.0};
    double testArray2[5] = {2.0, 4.0, 6.0, 8.0, 2.0};
    TEST_ASSERT_EQUAL_FLOAT(3.24, averageNumbersInArray(testArray1, 4)); //Should fail
    TEST_ASSERT_FLOAT_WITHIN(0.05, 5.00, averageNumbersInArray(testArray2, 5)); //Should fails
    
}

void goodTest_sum2Arrays(void)
{
    printf("\nTesting sum2Arrays function - Should Pass\n");
    double array1[5] = {1.0, 1.5, 3.25, 0, -3};
    double array2[5] = {2.7, 1.5, 3.25, -1, -3};  
    double expVal[5] = {3.7, 3.0, 6.5, -1, -6};
   
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expVal, sum2Arrays(array1, array2, 5), 5); //Should Pass   
}

void badTest_sum2Arrays(void)
{
    printf("\nTesting sum2Arrays function - Should Fail\n");
    double array1[5] = {1.0, 1.5, 3.25, 0, -3};
    double array2[5] = {2.7, 1.5, 3.25, -1, -3};  
    double expectedVal[5] = {3.6, 3.0, 6.5, -1, -0};
    
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expectedVal, sum2Arrays(array1, array2, 5), 5); //Should fail
}

int main(void)
{
    UNITY_BEGIN();
    
    RUN_TEST(goodTest_averageNumbersInArray);
    RUN_TEST(badTest_averageNumbersInArray);
    RUN_TEST(goodTest_sum2Arrays);
    RUN_TEST(badTest_sum2Arrays);

    return UNITY_END();
       
}


