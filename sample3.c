define fizz;
define buzz;
define fizzbuzz;
define others;
define i;

fizz = 0; 
buzz = 0;
fizzbuzz = 0; 
others = 0;
i = 1;
while(i < 31){
    if ((i / 15) * 15 == i){
        fizzbuzz = fizzbuzz + 1;
    }
    else if ((i / 3) * 3 == i){
        fizz = fizz + 1;
    }
    else if ((i / 5) * 5 == i){
        buzz = buzz + 1;
    }else{
        others = others + 1;
    }    
    i = i + 1;
}
