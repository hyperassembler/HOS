
# preprocess files, add all target object files to ${target} variable
function(PP_GEN file target)
    # Note that file should be full path
    get_filename_component(base ${file} NAME)
    set(output ${CMAKE_CURRENT_BINARY_DIR}/${base})
    add_custom_command(
            OUTPUT ${output}
            DEPENDS ${file}
            COMMAND ${CC} ${PP_FLAGS} -o ${output} ${file}
            IMPLICIT_DEPENDS C ${file}
    )
    set(${target} ${${target}} ${output} PARENT_SCOPE)
endfunction()

# cc file compile, add all target object files to ${target} variable
function(CC_GEN file target)
    # Note that file should be full path
    get_filename_component(base ${file} NAME)
    set(output ${CMAKE_CURRENT_BINARY_DIR}/${base}.o)
    add_custom_command(
            OUTPUT ${output}
            DEPENDS ${file}
            COMMAND ${CC} ${CC_FLAGS} -o ${output} ${file}
            IMPLICIT_DEPENDS C ${file}
    )
    set(${target} ${${target}} ${output} PARENT_SCOPE)
endfunction()

# asm file compile, add all target object files to ${target} variable
function(AS_GEN file target)
    # Note that file should be full path
    get_filename_component(base ${file} NAME)
    set(output ${CMAKE_CURRENT_BINARY_DIR}/${base}.o)
    add_custom_command(
            OUTPUT ${output}
            DEPENDS ${file}
            COMMAND ${AS} ${AS_FLAGS} -o ${output} ${file}
    )
    set(${target} ${${target}} ${output} PARENT_SCOPE)
endfunction()