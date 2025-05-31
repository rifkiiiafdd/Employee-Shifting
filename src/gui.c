
#include "gui.h"         // Include its own header
#include "csv_utils.h"   // For CSV file handling functions
#include "doctor_list.h" // For doctor data management functions

#include <glib.h> // For GLib data types and utilities (like g_free, g_strdup_printf)
#include <stdio.h>  // For printf (for terminal output)
#include <stdlib.h> // For atoi
#include <string.h> // For strlen

// Enums for GtkListStore columns (defined here as they are specific to the
// GUI's model)
enum {
    COL_ID,                  // ID first
    COL_NAME,                // Name second
    COL_MAX_SHIFTS,          // Max Shifts third
    COL_PREFERENCES_SUMMARY, // Summary of blacklisted preferences
    NUM_COLS                 // Keep this last, it's the total count of columns
};

// Global UI widget pointers (static to this file, accessible only within gui.c)
static GtkTreeView *doctor_list_tree_view = NULL;
static GtkListStore *doctor_list_store = NULL; // Model for the TreeView

// Add/Remove section entries
static GtkEntry *add_name_entry = NULL;
static GtkEntry *add_max_shifts_entry = NULL;
static GtkEntry *remove_id_entry = NULL;

// Edit section entries and widgets
static GtkEntry *edit_id_entry = NULL;
static GtkEntry *edit_name_entry = NULL;
static GtkEntry *edit_max_shifts_entry = NULL;
// 2D array of GtkCheckButton for preferences (Day x Shift)
static GtkCheckButton *pref_checkboxes[NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY];
// Pointer to the currently selected doctor for editing
static Doctor *selected_doctor_for_edit = NULL;

// Forward declarations for internal callback functions and helpers
static void on_load_display_clicked(GtkButton *button, gpointer user_data);
static void on_add_doctor_clicked(GtkButton *button, gpointer user_data);
static void on_remove_doctor_clicked(GtkButton *button, gpointer user_data);
static void on_save_doctors_clicked(GtkButton *button, gpointer user_data);
static void on_load_for_edit_clicked(GtkButton *button, gpointer user_data);
static void on_apply_edit_clicked(GtkButton *button, gpointer user_data);
static void setup_tree_view(GtkTreeView *tree_view);
static void show_message_dialog(GtkWindow *parent, GtkMessageType type,
                                const char *title, const char *message);

// Function to initialize and create the main application window and its UI
// elements. This function will be connected to the GtkApplication's "activate"
// signal.
void create_main_window(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *main_box;          // Main vertical container
    GtkWidget *input_grid_add;    // Grid for add inputs
    GtkWidget *input_grid_remove; // Grid for remove inputs
    GtkWidget *input_grid_edit;   // Grid for edit inputs
    // Removed: GtkWidget *button_box_main_actions; // Box for Load/Save buttons
    // - redeclared below
    GtkWidget *scrolled_window; // For the tree view

    // Labels for input fields (re-used to avoid re-declaration)
    GtkWidget *label_name, *label_max_shifts, *label_id, *label_edit_id,
        *label_edit_name, *label_edit_max_shifts;

    // 1. Create a new application window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window),
                         "Doctor Scheduler Application (GTK 3)");
    gtk_window_set_default_size(GTK_WINDOW(window), 1000,
                                750); // Increased default size for more UI

    // 2. Create a main vertical box container
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10); // 10 pixels spacing
    gtk_container_set_border_width(GTK_CONTAINER(main_box), 20);
    gtk_container_add(GTK_CONTAINER(window), main_box);

    // --- Add Doctor Section ---
    GtkWidget *add_frame = gtk_frame_new("Add New Doctor");
    gtk_box_pack_start(GTK_BOX(main_box), add_frame, FALSE, FALSE, 0);

    input_grid_add = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(input_grid_add), 5);
    gtk_grid_set_column_spacing(GTK_GRID(input_grid_add), 10);
    gtk_container_set_border_width(GTK_CONTAINER(input_grid_add), 10);
    gtk_container_add(GTK_CONTAINER(add_frame), input_grid_add);

    label_name = gtk_label_new("Name:");
    gtk_grid_attach(GTK_GRID(input_grid_add), label_name, 0, 0, 1, 1);
    add_name_entry = GTK_ENTRY(gtk_entry_new());
    gtk_grid_attach(GTK_GRID(input_grid_add), GTK_WIDGET(add_name_entry), 1, 0,
                    1, 1);

    label_max_shifts = gtk_label_new("Max Shifts/Week:");
    gtk_grid_attach(GTK_GRID(input_grid_add), label_max_shifts, 0, 1, 1, 1);
    add_max_shifts_entry = GTK_ENTRY(gtk_entry_new());
    gtk_grid_attach(GTK_GRID(input_grid_add), GTK_WIDGET(add_max_shifts_entry),
                    1, 1, 1, 1);

    GtkWidget *add_button = gtk_button_new_with_label("Add Doctor");
    g_signal_connect(add_button, "clicked", G_CALLBACK(on_add_doctor_clicked),
                     NULL);
    gtk_grid_attach(GTK_GRID(input_grid_add), add_button, 2, 0, 1, 2);

    // --- Remove Doctor Section ---
    GtkWidget *remove_frame = gtk_frame_new("Remove Doctor by ID");
    gtk_box_pack_start(GTK_BOX(main_box), remove_frame, FALSE, FALSE, 0);

    input_grid_remove = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(input_grid_remove), 5);
    gtk_grid_set_column_spacing(GTK_GRID(input_grid_remove), 10);
    gtk_container_set_border_width(GTK_CONTAINER(input_grid_remove), 10);
    gtk_container_add(GTK_CONTAINER(remove_frame), input_grid_remove);

    label_id = gtk_label_new("Doctor ID:");
    gtk_grid_attach(GTK_GRID(input_grid_remove), label_id, 0, 0, 1, 1);
    remove_id_entry = GTK_ENTRY(gtk_entry_new());
    gtk_grid_attach(GTK_GRID(input_grid_remove), GTK_WIDGET(remove_id_entry), 1,
                    0, 1, 1);

    GtkWidget *remove_button = gtk_button_new_with_label("Remove Doctor");
    g_signal_connect(remove_button, "clicked",
                     G_CALLBACK(on_remove_doctor_clicked), NULL);
    gtk_grid_attach(GTK_GRID(input_grid_remove), remove_button, 2, 0, 1, 1);

    // --- Edit Doctor Section ---
    GtkWidget *edit_frame = gtk_frame_new("Edit Doctor Data & Preferences");
    gtk_box_pack_start(GTK_BOX(main_box), edit_frame, FALSE, FALSE, 0);

    input_grid_edit = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(input_grid_edit), 5);
    gtk_grid_set_column_spacing(GTK_GRID(input_grid_edit), 10);
    gtk_container_set_border_width(GTK_CONTAINER(input_grid_edit), 10);
    gtk_container_add(GTK_CONTAINER(edit_frame), input_grid_edit);

    // Edit ID input and Load button
    label_edit_id = gtk_label_new("Edit Doctor ID:");
    gtk_grid_attach(GTK_GRID(input_grid_edit), label_edit_id, 0, 0, 1, 1);
    edit_id_entry = GTK_ENTRY(gtk_entry_new());
    gtk_grid_attach(GTK_GRID(input_grid_edit), GTK_WIDGET(edit_id_entry), 1, 0,
                    1, 1);
    GtkWidget *load_for_edit_button =
        gtk_button_new_with_label("Load for Edit");
    g_signal_connect(load_for_edit_button, "clicked",
                     G_CALLBACK(on_load_for_edit_clicked), NULL);
    gtk_grid_attach(GTK_GRID(input_grid_edit), load_for_edit_button, 2, 0, 1,
                    1);

    // Editable Name and Max Shifts
    label_edit_name = gtk_label_new("Name:");
    gtk_grid_attach(GTK_GRID(input_grid_edit), label_edit_name, 0, 1, 1, 1);
    edit_name_entry = GTK_ENTRY(gtk_entry_new());
    gtk_grid_attach(GTK_GRID(input_grid_edit), GTK_WIDGET(edit_name_entry), 1,
                    1, 1, 1);

    label_edit_max_shifts = gtk_label_new("Max Shifts/Week:");
    gtk_grid_attach(GTK_GRID(input_grid_edit), label_edit_max_shifts, 0, 2, 1,
                    1);
    edit_max_shifts_entry = GTK_ENTRY(gtk_entry_new());
    gtk_grid_attach(GTK_GRID(input_grid_edit),
                    GTK_WIDGET(edit_max_shifts_entry), 1, 2, 1, 1);

    // Preferences Grid (7 Days x 3 Shifts)
    GtkWidget *pref_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(pref_grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(pref_grid), 5);
    gtk_grid_attach(GTK_GRID(input_grid_edit), pref_grid, 0, 3, 3,
                    1); // Attach pref_grid spanning 3 columns

    const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    const char *shifts[] = {"Morning", "Afternoon", "Night"};

    // Add shift labels (column headers for preferences)
    gtk_grid_attach(GTK_GRID(pref_grid), gtk_label_new(""), 0, 0, 1,
                    1); // Empty corner
    for (int j = 0; j < NUM_SHIFTS_PER_DAY; j++) {
        gtk_grid_attach(GTK_GRID(pref_grid), gtk_label_new(shifts[j]), j + 1, 0,
                        1, 1);
    }

    // Add day labels and preference checkboxes
    for (int i = 0; i < NUM_DAYS_PER_WEEK; i++) {
        gtk_grid_attach(GTK_GRID(pref_grid), gtk_label_new(days[i]), 0, i + 1,
                        1, 1);
        for (int j = 0; j < NUM_SHIFTS_PER_DAY; j++) {
            pref_checkboxes[i][j] = GTK_CHECK_BUTTON(gtk_check_button_new());
            gtk_grid_attach(GTK_GRID(pref_grid),
                            GTK_WIDGET(pref_checkboxes[i][j]), j + 1, i + 1, 1,
                            1);
        }
    }

    // Apply Changes button for edit section
    GtkWidget *apply_edit_button = gtk_button_new_with_label("Apply Changes");
    g_signal_connect(apply_edit_button, "clicked",
                     G_CALLBACK(on_apply_edit_clicked), NULL);
    gtk_grid_attach(GTK_GRID(input_grid_edit), apply_edit_button, 0,
                    NUM_DAYS_PER_WEEK + 4, 3, 1); // Position below preferences

    // --- Main Action Buttons (Load/Save) ---
    GtkWidget *button_box_main_actions =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(main_box), button_box_main_actions, FALSE, FALSE,
                       0);
    gtk_box_set_homogeneous(GTK_BOX(button_box_main_actions), TRUE);
    gtk_box_set_spacing(GTK_BOX(button_box_main_actions), 10);

    GtkWidget *load_display_button =
        gtk_button_new_with_label("Load Doctors from CSV");
    g_signal_connect(load_display_button, "clicked",
                     G_CALLBACK(on_load_display_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box_main_actions), load_display_button,
                       TRUE, TRUE, 0);

    GtkWidget *save_button = gtk_button_new_with_label("Save Doctors to CSV");
    g_signal_connect(save_button, "clicked",
                     G_CALLBACK(on_save_doctors_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box_main_actions), save_button, TRUE,
                       TRUE, 0);

    // --- Doctor List Display Area (GtkTreeView) ---
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(main_box), scrolled_window, TRUE, TRUE, 0);

    // Create the GtkListStore model for the tree view
    doctor_list_store = gtk_list_store_new(NUM_COLS, G_TYPE_INT, G_TYPE_STRING,
                                           G_TYPE_INT, G_TYPE_STRING);

    // Create the GtkTreeView and set its model
    doctor_list_tree_view = GTK_TREE_VIEW(
        gtk_tree_view_new_with_model(GTK_TREE_MODEL(doctor_list_store)));
    gtk_tree_view_set_headers_visible(doctor_list_tree_view,
                                      TRUE); // Show column headers
    gtk_container_add(GTK_CONTAINER(scrolled_window),
                      GTK_WIDGET(doctor_list_tree_view));

    // Setup columns in the tree view
    setup_tree_view(doctor_list_tree_view);

    // Connect row-activated signal for editing
    g_signal_connect(doctor_list_tree_view, "row-activated",
                     G_CALLBACK(on_load_for_edit_clicked), NULL);

    // Initial display of doctors (if any are loaded or added)
    update_doctor_list_display();

    // 5. Present (show) the window to the user
    gtk_widget_show_all(window);
}

// Helper function to setup columns for the GtkTreeView
static void setup_tree_view(GtkTreeView *tree_view) {
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    // Column 1: ID
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text",
                                                      COL_ID, NULL);
    gtk_tree_view_append_column(tree_view, column);

    // Column 2: Name
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Doctor Name", renderer,
                                                      "text", COL_NAME, NULL);
    gtk_tree_view_append_column(tree_view, column);

    // Column 3: Max Shifts/Week
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(
        "Max Shifts/Week", renderer, "text", COL_MAX_SHIFTS, NULL);
    gtk_tree_view_append_column(tree_view, column);

    // Column 4: Preferences Summary
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(
        "Blacklisted Prefs", renderer, "text", COL_PREFERENCES_SUMMARY, NULL);
    gtk_tree_view_append_column(tree_view, column);
}

// Helper function to show a message dialog
static void show_message_dialog(GtkWindow *parent, GtkMessageType type,
                                const char *title, const char *message) {
    GtkWidget *dialog = gtk_message_dialog_new(parent, GTK_DIALOG_MODAL, type,
                                               GTK_BUTTONS_OK, "%s", message);
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_dialog_run(GTK_DIALOG(dialog)); // Run the dialog as blocking
    gtk_widget_destroy(dialog);         // Destroy it after it's closed
}

// Callback function for when the "Load & Display Doctors" button is clicked
static void on_load_display_clicked(GtkButton *button, gpointer user_data) {
    const char *data_file = "data/daftar_dokter.csv"; // Updated path

    if (loadDoctorsFromCSV(data_file)) {
        show_message_dialog(
            GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
            GTK_MESSAGE_INFO, "Load Successful",
            "Doctors loaded successfully from CSV.");
    } else {
        show_message_dialog(
            GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
            GTK_MESSAGE_ERROR, "Load Failed",
            "Failed to load doctors from 'data/daftar_dokter.csv'. Please "
            "ensure the file exists and is accessible.");
    }
    update_doctor_list_display(); // Always update display after load attempt
}

// Callback function for when the "Add Doctor" button is clicked
static void on_add_doctor_clicked(GtkButton *button, gpointer user_data) {
    const char *name = gtk_entry_get_text(add_name_entry);
    const char *max_shifts_str = gtk_entry_get_text(add_max_shifts_entry);
    int max_shifts;

    if (strlen(name) == 0) {
        show_message_dialog(
            GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
            GTK_MESSAGE_WARNING, "Input Error", "Doctor Name cannot be empty.");
        return;
    }

    if (sscanf(max_shifts_str, "%d", &max_shifts) != 1 || max_shifts <= 0) {
        show_message_dialog(
            GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
            GTK_MESSAGE_WARNING, "Input Error",
            "Max Shifts per Week must be a positive number.");
        return;
    }

    addDoctor(name, max_shifts);
    update_doctor_list_display();

    // Clear input fields after adding
    gtk_entry_set_text(add_name_entry, "");
    gtk_entry_set_text(add_max_shifts_entry, "");

    show_message_dialog(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
                        GTK_MESSAGE_INFO, "Doctor Added",
                        "New doctor added successfully!");
}

// Callback function for when the "Remove Doctor" button is clicked
static void on_remove_doctor_clicked(GtkButton *button, gpointer user_data) {
    const char *id_str = gtk_entry_get_text(remove_id_entry);
    int doctor_id;

    if (sscanf(id_str, "%d", &doctor_id) != 1 || doctor_id <= 0) {
        show_message_dialog(
            GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
            GTK_MESSAGE_WARNING, "Input Error",
            "Please enter a valid Doctor ID (positive number).");
        return;
    }

    removeDoctor(
        doctor_id); // This function already prints to console if not found
    update_doctor_list_display();

    // Clear input field after removing
    gtk_entry_set_text(remove_id_entry, "");

    show_message_dialog(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
                        GTK_MESSAGE_INFO, "Doctor Removed",
                        "Attempted to remove doctor. Check list for changes.");
}

// Callback function for saving doctors to CSV
static void on_save_doctors_clicked(GtkButton *button, gpointer user_data) {
    const char *data_file = "data/daftar_dokter.csv"; // Updated path

    if (saveDoctorsToCSV(data_file)) {
        show_message_dialog(
            GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
            GTK_MESSAGE_INFO, "Save Successful",
            "Doctors saved successfully to 'data/daftar_dokter.csv'.");
    } else {
        show_message_dialog(
            GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
            GTK_MESSAGE_ERROR, "Save Failed",
            "Failed to save doctors to 'data/daftar_dokter.csv'. Check "
            "permissions or file path.");
    }
}

// Callback function for loading a doctor's data into the edit fields
static void on_load_for_edit_clicked(GtkButton *button, gpointer user_data) {
    int doctor_id = -1;
    // If called from button, get ID from entry
    if (GTK_IS_BUTTON(button)) {
        const char *id_str = gtk_entry_get_text(edit_id_entry);
        if (sscanf(id_str, "%d", &doctor_id) != 1 || doctor_id <= 0) {
            show_message_dialog(
                GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
                GTK_MESSAGE_WARNING, "Input Error",
                "Please enter a valid Doctor ID to load for editing.");
            return;
        }
    } else if (GTK_IS_TREE_VIEW(button)) { // If called from row-activated
                                           // signal (double-click)
        GtkTreeSelection *selection =
            gtk_tree_view_get_selection(GTK_TREE_VIEW(button));
        GtkTreeModel *model;
        GtkTreeIter iter;

        if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
            gtk_tree_model_get(model, &iter, COL_ID, &doctor_id, -1);
        } else {
            // This case should ideally not happen for row-activated, but good
            // for robustness
            show_message_dialog(
                GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
                GTK_MESSAGE_INFO, "Selection Error",
                "No doctor selected in the list to load for editing.");
            return;
        }
    }

    selected_doctor_for_edit = findDoctorById(doctor_id);

    if (selected_doctor_for_edit) {
        // Update edit ID entry with the actual ID (in case it was loaded from
        // TreeView)
        gtk_entry_set_text(edit_id_entry,
                           g_strdup_printf("%d", selected_doctor_for_edit->id));
        gtk_entry_set_text(edit_name_entry, selected_doctor_for_edit->name);
        gtk_entry_set_text(
            edit_max_shifts_entry,
            g_strdup_printf("%d",
                            selected_doctor_for_edit->max_shifts_per_week));

        // Populate preference checkboxes
        for (int i = 0; i < NUM_DAYS_PER_WEEK; i++) {
            for (int j = 0; j < NUM_SHIFTS_PER_DAY; j++) {
                // Set active (TRUE) if preference is 1 (can do), FALSE if 0
                // (cannot do)
                gtk_toggle_button_set_active(
                    GTK_TOGGLE_BUTTON(pref_checkboxes[i][j]),
                    selected_doctor_for_edit->preference[i][j] == 1);
            }
        }
        show_message_dialog(
            GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
            GTK_MESSAGE_INFO, "Doctor Loaded",
            g_strdup_printf("Doctor '%s' (ID: %d) loaded for editing.",
                            selected_doctor_for_edit->name,
                            selected_doctor_for_edit->id));
    } else {
        show_message_dialog(
            GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
            GTK_MESSAGE_ERROR, "Doctor Not Found",
            g_strdup_printf("Doctor with ID %d not found.", doctor_id));
        // Clear edit fields if doctor not found
        gtk_entry_set_text(edit_id_entry, "");
        gtk_entry_set_text(edit_name_entry, "");
        gtk_entry_set_text(edit_max_shifts_entry, "");
        for (int i = 0; i < NUM_DAYS_PER_WEEK; i++) {
            for (int j = 0; j < NUM_SHIFTS_PER_DAY; j++) {
                gtk_toggle_button_set_active(
                    GTK_TOGGLE_BUTTON(pref_checkboxes[i][j]), FALSE);
            }
        }
    }
}

// Callback function for applying edits to a doctor's data
static void on_apply_edit_clicked(GtkButton *button, gpointer user_data) {
    if (!selected_doctor_for_edit) {
        show_message_dialog(
            GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
            GTK_MESSAGE_WARNING, "No Doctor Selected",
            "Please load a doctor for editing first.");
        return;
    }

    const char *name = gtk_entry_get_text(edit_name_entry);
    const char *max_shifts_str = gtk_entry_get_text(edit_max_shifts_entry);
    int max_shifts;

    if (strlen(name) == 0) {
        show_message_dialog(
            GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
            GTK_MESSAGE_WARNING, "Input Error", "Doctor Name cannot be empty.");
        return;
    }

    if (sscanf(max_shifts_str, "%d", &max_shifts) != 1 || max_shifts <= 0) {
        show_message_dialog(
            GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
            GTK_MESSAGE_WARNING, "Input Error",
            "Max Shifts per Week must be a positive number.");
        return;
    }

    // Update doctor's name and max shifts
    strncpy(selected_doctor_for_edit->name, name, MAX_NAME_LENGTH - 1);
    selected_doctor_for_edit->name[MAX_NAME_LENGTH - 1] = '\0';
    selected_doctor_for_edit->max_shifts_per_week = max_shifts;

    // Update preferences from checkboxes
    for (int i = 0; i < NUM_DAYS_PER_WEEK; i++) {
        for (int j = 0; j < NUM_SHIFTS_PER_DAY; j++) {
            // If checkbox is active (TRUE), preference is 1 (can do). If not
            // active (FALSE), preference is 0 (cannot do).
            selected_doctor_for_edit->preference[i][j] =
                gtk_toggle_button_get_active(
                    GTK_TOGGLE_BUTTON(pref_checkboxes[i][j]))
                    ? 1
                    : 0;
        }
    }

    update_doctor_list_display(); // Refresh the TreeView to show changes
    show_message_dialog(
        GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
        GTK_MESSAGE_INFO, "Changes Applied",
        g_strdup_printf("Changes to Doctor '%s' (ID: %d) applied successfully.",
                        selected_doctor_for_edit->name,
                        selected_doctor_for_edit->id));
}

// Function to update the GtkTreeView with the current doctor list
void update_doctor_list_display() { // Made public (no static)
    GtkTreeIter iter;               // Iterator for appending rows
    Doctor *current = head; // Start from the beginning of your linked list

    // Clear the existing content in the GtkListStore
    gtk_list_store_clear(doctor_list_store);

    if (current == NULL) {
        return;
    }

    // Iterate through your linked list and add each doctor as a row in the
    // GtkListStore
    while (current != NULL) {
        gtk_list_store_append(doctor_list_store, &iter); // Append a new row
        char *prefs_summary = get_blacklisted_preferences_string(
            current); // Get blacklisted prefs string

        gtk_list_store_set(
            doctor_list_store, &iter, COL_ID, current->id, COL_NAME,
            current->name, COL_MAX_SHIFTS, current->max_shifts_per_week,
            COL_PREFERENCES_SUMMARY, prefs_summary, // Set preferences summary
            -1); // -1 marks the end of arguments for gtk_list_store_set

        g_free(prefs_summary); // Free the string returned by
                               // get_blacklisted_preferences_string
        current = current->next;
    }
}
