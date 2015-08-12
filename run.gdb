set pagination off
set logging file gdb.log
set logging on
set $i = 0
file cybootload_linux
break main
set args ./CY8CKIT-049-41XX_GPIO_Example.cyacd
break WriteData 
# break CyBtldr_StartBootloadOperation
commands 2
  set $i=1000
  print "commands 2 : %d",$i
end
run
while ( $i < 1000 )
  step
  # next
  # continue
  set $i = $i + 1
  # print "loop : %d",$i
end
# print "at %d\n",$i  
quit
