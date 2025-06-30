idf_component_register(
    SRCS "queue_manager.cpp" "vcp_usb_manager.cpp" "main.cpp"
    INCLUDE_DIRS "."
    PRIV_REQUIRES usb
    )
