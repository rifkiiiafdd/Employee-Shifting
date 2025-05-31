#include "doctor_list.h" // For freeDoctorList (cleanup)
#include "gui.h"         // Include the GUI module header
#include <gio/gio.h>     // For GApplication
#include <gtk/gtk.h>     // For GtkApplication

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    // 1. Create a new GtkApplication instance
    app =
        gtk_application_new("org.pmp.doctorscheduler", 0); // Using 0 for flags

    // 2. Connect the "activate" signal to our GUI's main window creation
    // function. The 'activate' signal is emitted when the application is
    // launched.
    g_signal_connect(app, "activate", G_CALLBACK(create_main_window), NULL);

    // 3. Run the application. This enters the GTK main loop.
    status = g_application_run(G_APPLICATION(app), argc, argv);

    // 4. Clean up: Release the application object when done.
    g_object_unref(app);

    // 5. Free any dynamically allocated doctor list data before exiting
    freeDoctorList();

    return status;
}
