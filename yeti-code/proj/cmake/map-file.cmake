function(custom_target_map_file target)
    target_link_options(${target}
        PRIVATE
            "LINKER:-Map,$<TARGET_FILE:${target}>.map"
            "LINKER:--cref"
    )
endfunction()
