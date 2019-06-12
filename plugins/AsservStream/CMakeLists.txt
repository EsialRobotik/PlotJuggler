include_directories( ./ ../  ../../include  ../../common)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)


QT5_WRAP_UI ( UI_SRC  )

SET( SRC
    AsservStream_uartDecoder.cpp
    AsservStream.cpp
    ../../include/PlotJuggler/datastreamer_base.h
    )
    
    
add_library(AsservStream SHARED ${SRC} ${UI_SRC} )
target_link_libraries(AsservStream  ${Qt5Widgets_LIBRARIES} ${Qt5Xml_LIBRARIES})
target_compile_definitions(AsservStream PRIVATE PLUGIN_NAME="Esial Robotik asserv stream")

if(COMPILING_WITH_CATKIN)
    install(TARGETS AsservStream
        ARCHIVE DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
        LIBRARY DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
        RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION} )
else()
    install(TARGETS AsservStream DESTINATION bin  )
    
endif()
