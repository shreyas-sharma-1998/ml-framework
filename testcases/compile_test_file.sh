rm *.out

gcc $1 -I ../include -I ../private/include -L ../lib -lss -lm -o $2 -Wall
