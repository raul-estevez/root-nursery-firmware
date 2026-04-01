idf_component_register(
    SRCS
        "root-nursery-firmware.c"
        "wifi_driver.c"
        "mqtt_driver.c"
        "aht20_driver.c"
        "as7341_driver.c"
        "led_controller.c"
        "control_loop.c"
        "config.c"
    INCLUDE_DIRS "."
)
