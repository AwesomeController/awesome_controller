
#====================================================================#
#  Settings                                                          #
#====================================================================#
set(FIRMWARE_NAME awesome_controller)

set(${FIRMWARE_NAME}_BOARD uno)               # Arduino Target board
set(${FIRMWARE_NAME}_SRCS awesome_controller.cpp)

set(${FIRMWARE_NAME}_PORT /dev/ttyACM0)            # Serial upload port
set(${FIRMWARE_NAME}_SERIAL picocom @INPUT_PORT@ -b 9600 -l )  # Serial terminal cmd


#====================================================================#
#  Target generation                                                 #
#====================================================================#
generate_arduino_firmware(${FIRMWARE_NAME})
