if(enable-json-sample)

set(json_src Backend.cpp MainWindow.cpp)
set(json_moc_hdr Backend.hpp MainWindow.hpp)

include_directories(${MathGL_BINARY_DIR}/json)
if(enable-qt5)
	include(../cmake-qt5.txt)
	qt5_wrap_ui(json_ui_src MainWindow.ui)
else(enable-qt5)
	include(../cmake-qt4.txt)
	qt4_wrap_ui(json_ui_src MainWindow.ui)
endif(enable-qt5)
add_executable(MglForJsTestBench ${json_src} ${json_moc_hdr} ${json_ui_src})
if(enable-qt5)
	target_compile_definitions(MglForJsTestBench PUBLIC MGL_USE_QT5)
	target_link_libraries(MglForJsTestBench mgl-qt5 ${MGL_QT5_LIBS})
else(enable-qt5)
	target_link_libraries(MglForJsTestBench mgl-qt4 ${MGL_QT4_LIBS})
endif(enable-qt5)

endif(enable-json-sample)
