idf_component_register(SRCS "esp_dexcom.c"
    INCLUDE_DIRS "include" 
    REQUIRES 
        esp_http_client 
        esp-tls
        cjson
    EMBED_TXTFILES 
        dexcom_share_root_cert.pem
)
