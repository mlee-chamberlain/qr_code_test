#  This function is only for IDE library.   Separate so we don't get ones we cannot fix easily.

function(custom_ide_remove_compile_warnings target)
    set_target_properties( ${target}
        PROPERTIES COMPILE_FLAGS 
    " \
    -Wno-pedantic \
    -Wno-cast-align \
    -Wno-sign-compare \
    "
    )
endfunction()

# Explanations.......
# cast-align    alignment on 1, 2, or 4 byte boundary violation

#  these were declared, but none are found, so copy here for safekeeping.
#    -Wno-missing-prototypes     -Wno-strict-prototypes     -Wno-c++-compat 

#  misc. previous options included that did not affect nor change binary.
#   "-mthumb 
#    -mcpu=cortex-m4 
#    -mfloat-abi=hard 
#    -mfpu=fpv4-sp-d16    
#   these are set in the main project file.
#  -Wno-switch    -Wno-unused-parameter      -Wno-missing-declarations     -Wno-cast-qual     -Wno-shadow -Wno-unused-but-set-variable
