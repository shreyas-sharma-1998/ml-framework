rm *.o

gcc -c ../src/error/*.c -I ../include -I ../private/include -lm -Wall

gcc -c ../src/matrix/*.c -I ../include -I ../private/include -lm -Wall

gcc -c ../src/vector/*.c -I ../include -I ../private/include -lm -Wall

gcc -c ../src/math/*.c -I ../include -I ../private/include -lm -Wall

gcc -c ../src/ml_math/*.c -I ../include -I ../private/include -lm -Wall

gcc -c ../src/linear_regression/*.c -I ../include -I ../private/include -lm -Wall

gcc -c ../src/data_provider/*.c -I ../include -I ../private/include -lm -Wall

rm ../lib/libss.a

ar rcs ../lib/libss.a *.o