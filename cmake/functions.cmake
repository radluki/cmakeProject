
function(print_list_simple FILE_LIST)
	foreach(f ${FILE_LIST})
		message(STATUS "${f}")
	endforeach()
endfunction()

print_list_simple("${INCLUDES}")

function(print_list FILE_LIST)
    list(LENGTH FILE_LIST len)
    math(EXPR range_end "${len} - 1")
    foreach(i RANGE ${range_end})
    	list(GET FILE_LIST ${i} f)
    	math(EXPR i "${i}+1")
    	message(STATUS "${i}) ${f}")
    endforeach()
endfunction()


