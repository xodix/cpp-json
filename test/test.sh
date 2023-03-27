filename=${1};
executable_name="${filename%.*}";

clang++ $filename -std=c++20 -g -o $executable_name;

# Pass arguments that are not file name to executable
args="";

i=1;
j=$#;

shift 1;

while [ $i -le $j ] 
do
    args+=$1;
    args+=" ";
    i=$((i + 1));
    shift 1;
done

gdb $executable_name $args;
