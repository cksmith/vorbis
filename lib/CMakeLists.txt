set(VORBIS_PUBLIC_HEADERS
    ../include/vorbis/codec.h
    ../include/vorbis/vorbisenc.h
    ../include/vorbis/vorbisfile.h
)

set(VORBIS_HEADERS
    envelope.h
    lpc.h
    lsp.h
    codebook.h
    misc.h
    psy.h
    masking.h
    os.h
    mdct.h
    smallft.h
    highlevel.h
    registry.h
    scales.h
    window.h
    lookup.h
    lookup_data.h
    codec_internal.h
    backends.h
    bitrate.h
)

set(VORBIS_SOURCES
    mdct.c
    smallft.c
    block.c
    envelope.c
    window.c
    lsp.c
    lpc.c
    analysis.c
    synthesis.c
    psy.c
    info.c
    floor1.c
    floor0.c
    res0.c
    mapping0.c
    registry.c
    codebook.c
    sharedbook.c
    lookup.c
    bitrate.c
)

set(VORBISFILE_SOURCES
    vorbisfile.c
)

set(VORBISENC_SOURCES
    vorbisenc.c
)

if(WIN32)
    list(APPEND VORBIS_SOURCES vorbisenc.c)
endif()

if(MSVC)
    list(APPEND VORBIS_SOURCES ../win32/vorbis.def)
    list(APPEND VORBISENC_SOURCES ../win32/vorbisenc.def)
    list(APPEND VORBISFILE_SOURCES ../win32/vorbisfile.def)
endif()

include_directories(../include)
include_directories(.)
if(DEFINED OGG_INCLUDE_DIRS)
	include_directories(${OGG_INCLUDE_DIRS})
endif()

if (NOT BUILD_FRAMEWORK)
    add_library(vorbis ${VORBIS_HEADERS} ${VORBIS_SOURCES})
    add_library(vorbisenc ${VORBISENC_SOURCES})
    add_library(vorbisfile ${VORBISFILE_SOURCES})

    get_version_info(VORBIS_VERSION_INFO "V_LIB_CURRENT" "V_LIB_AGE" "V_LIB_REVISION")
    set_target_properties(vorbis PROPERTIES SOVERSION ${VORBIS_VERSION_INFO})
    get_version_info(VORBISENC_VERSION_INFO "VE_LIB_CURRENT" "VE_LIB_AGE" "VE_LIB_REVISION")
    set_target_properties(vorbisenc PROPERTIES SOVERSION ${VORBISENC_VERSION_INFO})
    get_version_info(VORBISFILE_VERSION_INFO "VF_LIB_CURRENT" "VF_LIB_AGE" "VF_LIB_REVISION")
    set_target_properties(vorbisfile PROPERTIES SOVERSION ${VORBISFILE_VERSION_INFO})

	if(DEFINED OGG_LIBRARIES)
		target_link_libraries(vorbis ${OGG_LIBRARIES})
		target_link_libraries(vorbisenc ${OGG_LIBRARIES} vorbis)
		target_link_libraries(vorbisfile ${OGG_LIBRARIES} vorbis)
	else()
		target_link_libraries(vorbis libogg::ogg)
		target_link_libraries(vorbisenc vorbis libogg::ogg)
		target_link_libraries(vorbisfile vorbis libogg::ogg)
	endif()
	
	set_target_properties(
		vorbis PROPERTIES
		PUBLIC_HEADER "${VORBIS_PUBLIC_HEADERS}"
	}

    install(TARGETS vorbis vorbisenc vorbisfile
		EXPORT "${target_export_name}"
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} 
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} 
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} 
		FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
		PUBLIC_HEADER DESTINATION "${include_install_dir}/ogg")
		
else()
    add_library(vorbis ${VORBIS_PUBLIC_HEADERS} ${VORBIS_HEADERS} ${VORBIS_SOURCES} ${VORBISFILE_SOURCES} ${VORBISENC_SOURCES})
    set_target_properties(vorbis PROPERTIES
        FRAMEWORK TRUE
        FRAMEWORK_VERSION ${PROJECT_VERSION}
        MACOSX_FRAMEWORK_IDENTIFIER org.xiph.vorbis
        MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${PROJECT_VERSION}
        MACOSX_FRAMEWORK_BUNDLE_VERSION ${PROJECT_VERSION}
        XCODE_ATTRIBUTE_INSTALL_PATH "@rpath"
        PUBLIC_HEADER "${VORBIS_PUBLIC_HEADERS}"
        OUTPUT_NAME Vorbis
    )
    target_link_libraries(vorbis ${OGG_LIBRARIES})
endif()
