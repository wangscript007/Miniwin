project(irdetocca C CXX)

set(SRCS_IRDETO
        ./cca_drv/src/Spi_Version.c
        ./cca_drv/src/UniversalClient_Stdlib.c
        ./cca_drv/src/UniversalClient_SPI.c
        ./cca_drv/src/UniversalClient_Core_SPI.c
        ./cca_drv/UniversalClient.cc
        ./cca_mw/src/CCA_UCMsgProc.cc
        ./cca_mw/src/CCA_StatusCodeProc.cc
        ./cca_gui/MApp_ZUI_ACTCCACasCmd.cc
        ./cca_mw/src/decoder_msg/appCCADecoderMsg.cc
        ./cca_mw/src/decoder_msg/appCCADecoderMsgTextMsg.cc
        ./cca_mw/src/decoder_msg/appCCADecoderMsgProfDec.cc
)
set(SRCS_IRDETOGUI
        cca_gui/irdccamenu.cc
        cca_gui/irdccaLoaderStatus.cc
        cca_gui/Irdccaserviceliststatus.cc
	cca_gui/irdccastatuscom.cc
)
add_definitions(-DDVB_CLIENT)
include_directories(
   cca_gui
   cca_drv/inc
   cca_gui/include
   cca_mw/inc
   cca_mw/inc/decoder_msg
   $ENV{HOME}/gx3213/irdeto_cca/dvt/platform/library/arm-linux/include
   $ENV{HOME}/gx3213/irdeto_cca/dvt/platform/library/arm-linux/include/av
   $ENV{HOME}/gx3213/irdeto_cca/dvt/platform/library/arm-linux/include/dvbhal
   $ENV{HOME}/gx3213/irdeto_cca/dvt/platform/library/arm-linux/include/devapi
   $ENV{HOME}/gx3213/irdeto_cca/dvt/platform/library/arm-linux/include/downloader
   $ENV{HOME}/gx3213/irdeto_cca/dvt/platform/library/arm-linux/include/downloader/ExportAPI
   $ENV{HOME}/gx3213/irdeto_cca/dvt/platform/library/arm-linux/include/bus
   $ENV{HOME}/gx3213/irdeto_cca/dvt/platform/library/arm-linux/include/bus/module/player
   $ENV{HOME}/gx3213/irdeto_cca/dvt/platform/library/arm-linux/include/frontend
   $ENV{HOME}/gx3213/irdeto_cca/dvt/platform/library/arm-linux/include/bus/module/frontend
)
link_directories($ENV{HOME}/gx3213/irdeto_cca/dvt/platform/library/arm-linux/lib cca_drv/lib)
add_library(irdetocca STATIC ${SRCS_IRDETO} ${SRCS_IRDETOGUI})
message("======HAL_EXTRA_LIBS=${HAL_EXTRA_LIBS}")
set(NGL_CALIBDIR $ENV{HOME}/gx3213/irdeto_cca/dvt/platform/library/arm-linux/lib ${PROJECT_SOURCE_DIR}/cca_drv/lib PARENT_SCOPE)
set(NGL_CALIBS 
   tvhal_static 
   -Wl,--start-group 
      irdetocca 
      CloakedCAAgent 
   -Wl,--end-group 
   downloader 
   ${HAL_EXTRA_LIBS} PARENT_SCOPE
)
