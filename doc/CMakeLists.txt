# Copyright (C) 2009 Stelios Bounanos, M0GLD (m0gld AT enotty DOT net)
# License: GPLv3+: GNU GPL version 3 or later.

set(ASCIIDOC_CONF asciidoc.conf)
set(FLDIGI_MAN_SRC fldigi.1.txt fldigi-shell.1.txt)
set(FLARQ_MAN_SRC flarq.1.txt)

if(WANT_FLDIGI)
    list(APPEND MAN_SRC ${FLDIGI_MAN_SRC})
    set(FLDIGI_HTML_SRC guide.txt)
endif()

if(WANT_FLARQ)
    list(APPEND MAN_SRC ${FLARQ_MAN_SRC})
endif()

if(NOT ASCIIDOC_ICONS_DIR)
    set(ASCIIDOC_ICONS_DIR /usr/share/asciidoc/images/icons)
endif()
if(NOT EXISTS ${ASCIIDOC_ICONS_DIR})
    message(ERROR "*** The default or specified asciidoc icons directory,
$(ASCIIDOC_ICONS_DIR), does not exist.")
endif()

# Additional attributes are defined in guide.conf
set(ASCIIDOC_ARGS "--unsafe" "-a toc" "-a data-uri" "-a badges" "-a icons" "-a iconsdir=${ASCIIDOC_ICONS_DIR}")
set(A2X_ARGS "--format=manpage" "--destination-dir=${CMAKE_CURRENT_BINARY_DIR}" "--no-xmllint")

# Build man pages.
foreach(_infile ${MAN_SRC})
    get_filename_component(_outname ${_infile} NAME)
    string(REPLACE ".txt" "" _outfile ${_outname})
    message(STATUS "Adding manpage target: ${_outfile}")
    add_custom_command(OUTPUT ${_outfile}
        COMMAND ${A2X} ${A2X_ARGS} ${CMAKE_CURRENT_SOURCE_DIR}/${_infile}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Building man page ${_outfile}"
    )
    list(APPEND MANFILES ${CMAKE_CURRENT_BINARY_DIR}/${_outfile})
endforeach()

# Build html guide.
foreach(_infile ${FLDIGI_HTML_SRC})
    get_filename_component(_outname ${_infile} NAME)
    string(REPLACE ".txt" ".html" _outfile ${_outname})
    message(STATUS "Adding doc target: ${_outfile}")
    add_custom_command(OUTPUT ${_outfile}
        COMMAND ${ASCIIDOC} ${ASCIIDOC_ARGS} -o ${_outfile} ${CMAKE_CURRENT_SOURCE_DIR}/${_infile}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Building html guide ${_outfile}"
        VERBATIM
    )
    list(APPEND HTMLFILES ${CMAKE_CURRENT_BINARY_DIR}/${_outfile})
endforeach()

if(NOT TARGET doc)
    add_custom_target(doc DEPENDS ${MANFILES} ${HTMLFILES})
endif()

install(FILES ${MANFILES}
    DESTINATION ${CMAKE_INSTALL_MANDIR}/man1
)
install(FILES ${HTMLFILES}
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
)



