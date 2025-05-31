#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h> // Include GTK for GtkApplication and other types

// Function to initialize and create the main application window and its UI
// elements. This function will be connected to the GtkApplication's "activate"
// signal.
void create_main_window(GtkApplication *app, gpointer user_data);

// Function to update the GtkTreeView with the current doctor list.
// This is made public so other modules (like doctor_list.c or csv_utils.c)
// can trigger a UI refresh after data changes.
void update_doctor_list_display();

#endif // GUI_H
