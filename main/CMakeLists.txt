idf_component_register(
    SRCS
        "main.c"
        "wifi.c"
        "fetch.c"
    INCLUDE_DIRS
        "."
    # REQUIRES
    #     nvs_flash
    #     esp_wifi
    #     esp_http_client
    #     esp-tls
    EMBED_TXTFILES 
        google_com_root_cert.pem
)
