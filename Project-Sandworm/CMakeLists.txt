file(GLOB LAB_SRC_FILES "*.c")

# Create one target
add_avr_executable(Project  ${LAB_SRC_FILES} )# ${MEG540_C_LIB_SRC_FILES} )

# LINK Again LUFA Library
avr_target_link_libraries(Project LUFA_USB MEGN540)
