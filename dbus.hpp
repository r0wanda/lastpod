#pragma once
#include <iostream>
#include <gio/gio.hpp>

static void on_interface_added(GDBusObjectManager *manager,
                               GDBusObject *object,
                               GDBusInterface *interface,
                               gpointer user_data) {
    const gchar *object_path = g_dbus_object_get_object_path(object);
    std::cout << "Device connected at path: " << object_path << std::endl;
    
    // Your mount logic goes here
}

int main() {
    // Initialize GLib type system
    g_type_init(); 

    GError *error = nullptr;
    // Connect to the UDisks2 D-Bus Object Manager
    GDBusObjectManager *manager = g_dbus_object_manager_client_new_for_bus_sync(
        G_BUS_TYPE_SYSTEM,
        G_DBUS_OBJECT_MANAGER_CLIENT_FLAGS_NONE,
        "org.freedesktop.UDisks2",
        "/org/freedesktop/UDisks2",
        nullptr, nullptr, nullptr, &error);

    if (!manager) {
        std::cerr << "Failed to connect to UDisks2: " << error->message << std::endl;
        g_error_free(error);
        return 1;
    }

    // Listen for new interfaces (device connections)
    g_signal_connect(manager, "interface-added", G_CALLBACK(on_interface_added), nullptr);

    std::cout << "Waiting for device connection..." << std::endl;

    // Start the event loop to keep waiting
    GMainLoop *loop = g_main_loop_new(nullptr, FALSE);
    g_main_loop_run(loop);

    g_object_unref(manager);
    return 0;
}
