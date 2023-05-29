find_program(iwyu_path NAMES include-what-you-use iwyu)
if(iwyu_path)
  set_property(TARGET ${lib}
    PROPERTY "${iwyu_path}" -Xiwyu
      --mapping_file "${CMAKE_CURRENT_SOURCE_DIR}/iwyu.imp"
  )
endif()
