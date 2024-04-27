file(GLOB SRC_FILES "*.c")

# Create one target
add_avr_executable(FinalProject  ${SRC_FILES} )# ${MEG540_C_LIB_SRC_FILES} )

# LINK Agains LUFA Libarary
avr_target_link_libraries(FinalProject LUFA_USB MEGN540)