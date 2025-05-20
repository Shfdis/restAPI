add_library(
        messenger
        add_requests.h
        add_requests.cpp
        login_register.cpp
        login_register.h
        settings.h
        settings.cpp
        messages.cpp
        messages.h
)
target_link_libraries(messenger network_handler db crypto)