deps_config := \
	/home/osk/esp/esp-idf/components/app_trace/Kconfig \
	/home/osk/esp/esp-idf/components/aws_iot/Kconfig \
	/home/osk/esp/esp-idf/components/bt/Kconfig \
	/home/osk/esp/esp-idf/components/esp32/Kconfig \
	/home/osk/esp/esp-idf/components/ethernet/Kconfig \
	/home/osk/esp/esp-idf/components/fatfs/Kconfig \
	/home/osk/esp/esp-idf/components/freertos/Kconfig \
	/home/osk/esp/esp-idf/components/heap/Kconfig \
	/home/osk/esp/esp-idf/components/libsodium/Kconfig \
	/home/osk/esp/esp-idf/components/log/Kconfig \
	/home/osk/esp/esp-idf/components/lwip/Kconfig \
	/home/osk/esp/esp-idf/components/mbedtls/Kconfig \
	/home/osk/esp/esp-idf/components/openssl/Kconfig \
	/home/osk/esp/esp-idf/components/pthread/Kconfig \
	/home/osk/esp/esp-idf/components/spi_flash/Kconfig \
	/home/osk/esp/esp-idf/components/spiffs/Kconfig \
	/home/osk/esp/esp-idf/components/tcpip_adapter/Kconfig \
	/home/osk/esp/esp-idf/components/wear_levelling/Kconfig \
	/home/osk/esp/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/osk/esp/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/osk/esp/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/osk/esp/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
