

main_process(){
difpid=`pgrep smalldb`  # process's pid(s)
declare -a array_main_process  #array with all the main process
for pid in $difpid
do
ret=`pgrep -P $pid | wc -l`            # ret is number of child processes using a wc
if [ "$ret" -ne 0 ];  #if children !=0
    then  
    array_main_process+=("$pid")
fi
done
echo ${array_main_process[@]}
}


list(){
declare -a list_main_process="$(main_process)"
for pid in $list_main_process
do
kill -SIGUSR2
done
}

sync(){
kill -SIGUSR1 ???? pid ?????
}

stop(){
kill -SIGINT ???? pid ?????
}