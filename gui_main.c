#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "include/csv_utils.h"
#include "include/doctor_list.h"
#include "include/scheduler.h"

// Enum untuk kolom-kolom tabel
enum { COL_ID = 0, COL_NAME, COL_MAX_SHIFTS, COL_SHIFTS_PER_WEEK, COL_UNAVAILABILITY, NUM_DOCTOR_COLS };
enum { COL_DAY_INFO = 0, COL_SHIFT_MORNING, COL_SHIFT_AFTERNOON, COL_SHIFT_NIGHT, NUM_SCHEDULE_COLS };

// --- Deklarasi Global Widget ---
GtkWidget *main_window, *stack, *doctor_tree_view, *schedule_tree_view, *conflict_text_view, *doctor_schedule_text_view;

// --- Prototipe Fungsi ---
void populate_doctor_table();
void populate_schedule_table_range(int start_day, int num_days);
void show_info_dialog(const gchar *message);
void show_error_dialog(const gchar *message);
GtkWidget* create_doctor_view();
GtkWidget* create_schedule_view();
GtkWidget* create_text_display_view(GtkWidget **text_view_widget);
void on_schedule_option_toggled(GtkToggleButton *togglebutton, gpointer user_data);
void recalculate_all_doctor_shift_counts(); // BARU: Untuk sinkronisasi data

// Prototipe untuk semua fungsi callback
static void on_display_doctors_clicked(GtkButton *button, gpointer user_data);
static void on_add_doctor_clicked(GtkButton *button, gpointer user_data);
static void on_edit_doctor_clicked(GtkButton *button, gpointer user_data);
static void on_remove_doctor_clicked(GtkButton *button, gpointer user_data);
static void on_display_overall_schedule_clicked(GtkButton *button, gpointer user_data);
static void on_display_doctor_schedule_clicked(GtkButton *button, gpointer user_data);
static void on_generate_schedule_clicked(GtkButton *button, gpointer user_data);
static void on_view_conflicts_clicked(GtkButton *button, gpointer user_data);
static void on_save_data_clicked(GtkButton *button, gpointer user_data);
static void on_load_data_clicked(GtkButton *button, gpointer user_data);


// --- Implementasi Lengkap Semua Fungsi ---

// BARU: Fungsi untuk menghitung ulang shift dokter berdasarkan jadwal di memori
void recalculate_all_doctor_shift_counts() {
    if (!head) return;

    // 1. Reset semua hitungan shift ke 0
    for (Doctor* doc = head; doc != NULL; doc = doc->next) {
        for (int i = 0; i < NUM_WEEKS; i++) {
            doc->shifts_scheduled_per_week[i] = 0;
        }
    }

    // 2. Iterasi melalui seluruh jadwal dan increment hitungan yang sesuai
    for (int day = 0; day < NUM_DAYS; day++) {
        for (int shift = 0; shift < NUM_SHIFTS_PER_DAY; shift++) {
            int doc_id = schedule[day][shift];
            if (doc_id > 0) {
                Doctor* doc = findDoctorById(doc_id);
                if (doc) {
                    int week_index = day / NUM_DAYS_PER_WEEK;
                    doc->shifts_scheduled_per_week[week_index]++;
                }
            }
        }
    }
}

// DIUBAH: on_load_data_clicked sekarang melakukan sinkronisasi dan refresh total
// GANTIKAN FUNGSI LAMA DENGAN VERSI DEBUG INI
static void on_load_data_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new("Pilih Folder Data", GTK_WINDOW(main_window), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, "_Batal", GTK_RESPONSE_CANCEL, "_Buka", GTK_RESPONSE_ACCEPT, NULL);
    
    // PERBAIKAN: Memeriksa GTK_RESPONSE_ACCEPT, bukan GTK_RESPONSE_OK
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *folder_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        char doc_path[512], sched_path[512];
        snprintf(doc_path, sizeof(doc_path), "%s/doctors.csv", folder_path);
        snprintf(sched_path, sizeof(sched_path), "%s/schedule.csv", folder_path);

        bool doc_loaded = load_doctors_from_csv(doc_path);
        bool sched_loaded = load_schedule_from_csv(sched_path);

        if (doc_loaded) {
            if (sched_loaded) {
                recalculate_all_doctor_shift_counts();
            }
            populate_doctor_table();
            populate_schedule_table_range(0, NUM_DAYS);
            gtk_stack_set_visible_child_name(GTK_STACK(stack), "doctors_view");
            show_info_dialog("Data dokter dan jadwal berhasil dimuat dan disinkronkan.");
        } else { 
            show_error_dialog("Gagal memuat data dokter. Pastikan 'doctors.csv' ada di folder yang dipilih."); 
        }
        g_free(folder_path);
    }
    
    gtk_widget_destroy(dialog);
}


// (Sisa fungsi-fungsi lain tidak ada yang berubah, salin dari jawaban sebelumnya)
static void on_display_doctors_clicked(GtkButton *button, gpointer user_data) {
    populate_doctor_table();
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "doctors_view");
}

static void on_add_doctor_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog, *content_area, *grid, *pref_grid;
    GtkWidget *name_label, *name_entry, *shifts_label, *shifts_spin_button, *pref_frame;
    GtkCheckButton *pref_checkboxes[NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY];
    const char *day_labels[] = {"Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu", "Minggu"};
    const char *shift_labels[] = {"Pagi", "Siang", "Malam"};

    dialog = gtk_dialog_new_with_buttons("Tambah Dokter Baru", GTK_WINDOW(main_window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "_Batal", GTK_RESPONSE_CANCEL, "_Tambah", GTK_RESPONSE_ACCEPT, NULL);
    gtk_window_set_default_size(GTK_WINDOW(dialog), 450, 500);
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 10);
    gtk_container_add(GTK_CONTAINER(content_area), grid);

    name_label = gtk_label_new("Nama Dokter:");
    name_entry = gtk_entry_new();
    shifts_label = gtk_label_new("Maks Shift/Minggu:");
    GtkAdjustment *adj = gtk_adjustment_new(5.0, 1.0, 21.0, 1.0, 5.0, 0.0);
    shifts_spin_button = gtk_spin_button_new(adj, 1.0, 0);
    gtk_grid_attach(GTK_GRID(grid), name_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), name_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), shifts_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), shifts_spin_button, 1, 1, 1, 1);

    pref_frame = gtk_frame_new("Preferensi Ketersediaan (Centang jika BERSESIA)");
    gtk_grid_attach(GTK_GRID(grid), pref_frame, 0, 2, 2, 1);
    pref_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(pref_grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(pref_grid), 15);
    gtk_container_set_border_width(GTK_CONTAINER(pref_grid), 10);
    gtk_container_add(GTK_CONTAINER(pref_frame), pref_grid);

    for(int s=0; s < NUM_SHIFTS_PER_DAY; s++) { gtk_grid_attach(GTK_GRID(pref_grid), gtk_label_new(shift_labels[s]), s + 1, 0, 1, 1); }
    for(int d=0; d < NUM_DAYS_PER_WEEK; d++) {
        gtk_grid_attach(GTK_GRID(pref_grid), gtk_label_new(day_labels[d]), 0, d + 1, 1, 1);
        for(int s=0; s < NUM_SHIFTS_PER_DAY; s++) {
            pref_checkboxes[d][s] = GTK_CHECK_BUTTON(gtk_check_button_new());
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pref_checkboxes[d][s]), TRUE);
            gtk_grid_attach(GTK_GRID(pref_grid), GTK_WIDGET(pref_checkboxes[d][s]), s + 1, d + 1, 1, 1);
        }
    }

    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        const char *name = gtk_entry_get_text(GTK_ENTRY(name_entry));
        int max_shifts = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(shifts_spin_button));
        if (strlen(name) > 0 && max_shifts > 0) {
            addDoctor(name, max_shifts);
            Doctor* new_doc = findDoctorByName(name);
            if (new_doc) {
                for(int d=0; d < NUM_DAYS_PER_WEEK; d++) {
                    for(int s=0; s < NUM_SHIFTS_PER_DAY; s++) {
                        setDoctorPreference(new_doc->id, d, s, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pref_checkboxes[d][s])));
                    }
                }
            }
            populate_doctor_table();
            gtk_stack_set_visible_child_name(GTK_STACK(stack), "doctors_view");
            show_info_dialog("Dokter berhasil ditambahkan!");
        } else { show_error_dialog("Input tidak valid."); }
    }
    gtk_widget_destroy(dialog);
}

static void on_edit_doctor_clicked(GtkButton *button, gpointer user_data) {
    if (!head) {
        show_error_dialog("Tidak ada dokter untuk diedit.");
        return;
    }
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Pilih Dokter untuk Diedit", GTK_WINDOW(main_window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "_Batal", GTK_RESPONSE_CANCEL, "_Pilih", GTK_RESPONSE_OK, NULL);
    GtkWidget* content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget* combo = gtk_combo_box_text_new();
    for (Doctor *doc = head; doc != NULL; doc = doc->next) {
        char label[MAX_NAME_LENGTH + 10];
        sprintf(label, "%d: %s", doc->id, doc->name);
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo), g_strdup_printf("%d", doc->id), label);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
    gtk_container_add(GTK_CONTAINER(content_area), combo);
    gtk_widget_show_all(dialog);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) != GTK_RESPONSE_OK) {
        gtk_widget_destroy(dialog);
        return;
    }
    const char* doc_id_str = gtk_combo_box_get_active_id(GTK_COMBO_BOX(combo));
    if (!doc_id_str) { gtk_widget_destroy(dialog); return; }
    int doc_id = atoi(doc_id_str);
    Doctor* doc_to_edit = findDoctorById(doc_id);
    gtk_widget_destroy(dialog);
    GtkWidget *edit_dialog, *grid, *pref_grid, *pref_frame;
    GtkCheckButton *pref_checkboxes[NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY];
    const char *day_labels[] = {"Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu", "Minggu"};
    const char *shift_labels[] = {"Pagi", "Siang", "Malam"};
    char dialog_title[100];
    sprintf(dialog_title, "Edit Dokter: %s (ID: %d)", doc_to_edit->name, doc_to_edit->id);
    edit_dialog = gtk_dialog_new_with_buttons(dialog_title, GTK_WINDOW(main_window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "_Batal", GTK_RESPONSE_CANCEL, "_Simpan", GTK_RESPONSE_ACCEPT, NULL);
    gtk_window_set_default_size(GTK_WINDOW(edit_dialog), 450, 500);
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(edit_dialog));
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 10);
    gtk_container_add(GTK_CONTAINER(content_area), grid);
    GtkWidget *name_label = gtk_label_new("Nama Baru:");
    GtkWidget *name_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(name_entry), doc_to_edit->name);
    GtkWidget *shifts_label = gtk_label_new("Maks Shift/Minggu:");
    GtkAdjustment *adj = gtk_adjustment_new(doc_to_edit->max_shifts_per_week, 1.0, 21.0, 1.0, 5.0, 0.0);
    GtkWidget *shifts_spin_button = gtk_spin_button_new(adj, 1.0, 0);
    gtk_grid_attach(GTK_GRID(grid), name_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), name_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), shifts_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), shifts_spin_button, 1, 1, 1, 1);
    pref_frame = gtk_frame_new("Preferensi Ketersediaan (Centang jika BERSESIA)");
    gtk_grid_attach(GTK_GRID(grid), pref_frame, 0, 2, 2, 1);
    pref_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(pref_grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(pref_grid), 15);
    gtk_container_set_border_width(GTK_CONTAINER(pref_grid), 10);
    gtk_container_add(GTK_CONTAINER(pref_frame), pref_grid);
    for(int s=0; s < NUM_SHIFTS_PER_DAY; s++) { gtk_grid_attach(GTK_GRID(pref_grid), gtk_label_new(shift_labels[s]), s + 1, 0, 1, 1); }
    for(int d=0; d < NUM_DAYS_PER_WEEK; d++) {
        gtk_grid_attach(GTK_GRID(pref_grid), gtk_label_new(day_labels[d]), 0, d + 1, 1, 1);
        for(int s=0; s < NUM_SHIFTS_PER_DAY; s++) {
            pref_checkboxes[d][s] = GTK_CHECK_BUTTON(gtk_check_button_new());
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pref_checkboxes[d][s]), doc_to_edit->preference[d][s]);
            gtk_grid_attach(GTK_GRID(pref_grid), GTK_WIDGET(pref_checkboxes[d][s]), s + 1, d + 1, 1, 1);
        }
    }
    gtk_widget_show_all(edit_dialog);
    if (gtk_dialog_run(GTK_DIALOG(edit_dialog)) == GTK_RESPONSE_ACCEPT) {
        const char *new_name = gtk_entry_get_text(GTK_ENTRY(name_entry));
        int new_max_shifts = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(shifts_spin_button));
        updateDoctor(doc_id, new_name, new_max_shifts);
        for(int d=0; d < NUM_DAYS_PER_WEEK; d++) {
            for(int s=0; s < NUM_SHIFTS_PER_DAY; s++) {
                setDoctorPreference(doc_id, d, s, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pref_checkboxes[d][s])));
            }
        }
        populate_doctor_table();
        show_info_dialog("Data dokter berhasil diperbarui.");
    }
    gtk_widget_destroy(edit_dialog);
}

static void on_remove_doctor_clicked(GtkButton *button, gpointer user_data) {
    if (!head) {
        show_error_dialog("Tidak ada dokter untuk dihapus.");
        return;
    }
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Pilih Dokter untuk Dihapus", GTK_WINDOW(main_window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "_Batal", GTK_RESPONSE_CANCEL, "_Pilih", GTK_RESPONSE_OK, NULL);
    GtkWidget* content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget* combo = gtk_combo_box_text_new();
    for (Doctor *doc = head; doc != NULL; doc = doc->next) {
        char label[MAX_NAME_LENGTH + 10];
        sprintf(label, "%d: %s", doc->id, doc->name);
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo), g_strdup_printf("%d", doc->id), label);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
    gtk_container_add(GTK_CONTAINER(content_area), combo);
    gtk_widget_show_all(dialog);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) != GTK_RESPONSE_OK) {
        gtk_widget_destroy(dialog);
        return;
    }
    const char* doc_id_str = gtk_combo_box_get_active_id(GTK_COMBO_BOX(combo));
    if (!doc_id_str) { gtk_widget_destroy(dialog); return; }
    int doc_id = atoi(doc_id_str);
    Doctor* doc_to_remove = findDoctorById(doc_id);
    gtk_widget_destroy(dialog);
    char confirmation_text[200];
    sprintf(confirmation_text, "Apakah Anda yakin ingin menghapus Dr. %s (ID: %d)?", doc_to_remove->name, doc_id);
    GtkWidget* confirm_dialog = gtk_message_dialog_new(GTK_WINDOW(main_window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "%s", confirmation_text);
    if (gtk_dialog_run(GTK_DIALOG(confirm_dialog)) == GTK_RESPONSE_YES) {
        removeDoctor(doc_id);
        populate_doctor_table();
        show_info_dialog("Dokter telah dihapus.");
    }
    gtk_widget_destroy(confirm_dialog);
}

static void on_display_overall_schedule_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog, *content_area, *box, *radio_month, *radio_week, *radio_day, *week_spin, *day_spin;
    dialog = gtk_dialog_new_with_buttons("Pilih Jangkauan Jadwal Keseluruhan", GTK_WINDOW(main_window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "_Batal", GTK_RESPONSE_CANCEL, "_Tampilkan", GTK_RESPONSE_OK, NULL);
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(box), 15);
    gtk_container_add(GTK_CONTAINER(content_area), box);
    radio_month = gtk_radio_button_new_with_label(NULL, "Tampilkan Jadwal 30 Hari");
    radio_week = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio_month), "Tampilkan Per Minggu (1-5)");
    radio_day = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio_month), "Tampilkan Per Hari (0=Senin, 6=Minggu)");
    GtkAdjustment *adj_week = gtk_adjustment_new(1, 1, NUM_WEEKS, 1, 0, 0);
    week_spin = gtk_spin_button_new(adj_week, 1, 0);
    GtkAdjustment *adj_day = gtk_adjustment_new(0, 0, NUM_DAYS_PER_WEEK - 1, 1, 0, 0);
    day_spin = gtk_spin_button_new(adj_day, 1, 0);
    gtk_box_pack_start(GTK_BOX(box), radio_month, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), radio_week, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), week_spin, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), radio_day, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), day_spin, FALSE, FALSE, 0);
    GtkWidget *widgets_for_overall[] = {radio_week, radio_day, week_spin, day_spin};
    g_signal_connect(radio_month, "toggled", G_CALLBACK(on_schedule_option_toggled), widgets_for_overall);
    g_signal_connect(radio_week, "toggled", G_CALLBACK(on_schedule_option_toggled), widgets_for_overall);
    g_signal_connect(radio_day, "toggled", G_CALLBACK(on_schedule_option_toggled), widgets_for_overall);
    on_schedule_option_toggled(NULL, widgets_for_overall);
    gtk_widget_show_all(dialog);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_month))) {
            populate_schedule_table_range(0, NUM_DAYS);
        } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_week))) {
            populate_schedule_table_range((gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(week_spin)) - 1) * 7, 7);
        } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_day))) {
            populate_schedule_table_range((gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(week_spin)) - 1) * 7 + gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(day_spin)), 1);
        }
        gtk_stack_set_visible_child_name(GTK_STACK(stack), "schedule_view");
    }
    gtk_widget_destroy(dialog);
}

static void on_display_doctor_schedule_clicked(GtkButton *button, gpointer user_data) {
    if (!head) { show_error_dialog("Tidak ada dokter dalam daftar."); return; }
    GtkWidget *dialog, *content_area, *box, *doctor_combo, *radio_doc_month, *radio_doc_week, *doc_week_spin;
    dialog = gtk_dialog_new_with_buttons("Pilih Jadwal Dokter Spesifik", GTK_WINDOW(main_window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "_Batal", GTK_RESPONSE_CANCEL, "_Tampilkan", GTK_RESPONSE_OK, NULL);
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(box), 15);
    gtk_container_add(GTK_CONTAINER(content_area), box);
    doctor_combo = gtk_combo_box_text_new();
    for (Doctor *doc = head; doc != NULL; doc = doc->next) {
        char label[MAX_NAME_LENGTH + 10];
        sprintf(label, "%d: %s", doc->id, doc->name);
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(doctor_combo), g_strdup_printf("%d", doc->id), label);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(doctor_combo), 0);
    radio_doc_month = gtk_radio_button_new_with_label(NULL, "Tampilkan Jadwal 30 Hari");
    radio_doc_week = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio_doc_month), "Tampilkan Per Minggu");
    GtkAdjustment *adj_doc_week = gtk_adjustment_new(1, 1, NUM_WEEKS, 1, 0, 0);
    doc_week_spin = gtk_spin_button_new(adj_doc_week, 1, 0);
    gtk_widget_set_sensitive(doc_week_spin, FALSE);
    g_signal_connect_swapped(radio_doc_week, "toggled", G_CALLBACK(gtk_widget_set_sensitive), doc_week_spin);
    gtk_box_pack_start(GTK_BOX(box), gtk_label_new("Pilih Dokter:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), doctor_combo, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), radio_doc_month, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), radio_doc_week, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), doc_week_spin, FALSE, FALSE, 0);
    gtk_widget_show_all(dialog);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        const char* doc_id_str = gtk_combo_box_get_active_id(GTK_COMBO_BOX(doctor_combo));
        if (!doc_id_str) { show_error_dialog("Silakan pilih dokter."); gtk_widget_destroy(dialog); return; }
        int doc_id = atoi(doc_id_str);
        Doctor* doc = findDoctorById(doc_id);
        GString* result_str = g_string_new("");
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_doc_month))) {
            g_string_printf(result_str, "--- Jadwal 30 Hari untuk Dr. %s ---\n\n", doc->name);
            int monthly_shifts[NUM_WEEKS][NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY];
            int monthly_count[NUM_WEEKS][NUM_DAYS_PER_WEEK];
            get_doctor_schedule_month(doc_id, monthly_shifts, monthly_count);
            const char *d_names[] = {"Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu", "Minggu"};
            const char *s_names[] = {"Pagi", "Siang", "Malam"};
            for (int w = 0; w < NUM_WEEKS; w++) {
                int shifts_this_week = 0;
                for(int d=0; d<NUM_DAYS_PER_WEEK; d++) if(monthly_count[w][d] > 0) shifts_this_week++;
                if (shifts_this_week > 0) {
                    g_string_append_printf(result_str, "--- Minggu %d ---\n", w + 1);
                    for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
                        if (monthly_count[w][d] > 0) {
                            g_string_append_printf(result_str, "%-10s: ", d_names[d]);
                            for (int s = 0; s < monthly_count[w][d]; s++) g_string_append_printf(result_str, "%s ", s_names[monthly_shifts[w][d][s]]);
                            g_string_append(result_str, "\n");
                        }
                    }
                }
            }
            int total_s = totalShift(doc_id);
            if (total_s == 0) g_string_append(result_str, "\nTidak ada shift.\n"); else g_string_append_printf(result_str, "\nTotal Shift: %d\n", total_s);
        } else {
            int week = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(doc_week_spin));
            g_string_printf(result_str, "--- Jadwal Minggu %d untuk Dr. %s ---\n\n", week, doc->name);
            int weekly_shifts[NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY];
            int weekly_count[NUM_DAYS_PER_WEEK];
            getDoctorScheduleWeek(week, doc_id, weekly_shifts, weekly_count);
            const char *d_names[] = {"Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu", "Minggu"};
            const char *s_names[] = {"Pagi", "Siang", "Malam"};
            int total_ws = totalShiftWeek(week, doc_id);
            if (total_ws > 0) {
                for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
                    if (weekly_count[d] > 0) {
                        g_string_append_printf(result_str, "%-10s: ", d_names[d]);
                        for (int s = 0; s < weekly_count[d]; s++) g_string_append_printf(result_str, "%s ", s_names[weekly_shifts[d][s]]);
                        g_string_append(result_str, "\n");
                    }
                }
            } else { g_string_append(result_str, "Tidak ada shift minggu ini.\n"); }
        }
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(doctor_schedule_text_view));
        gtk_text_buffer_set_text(buffer, result_str->str, -1);
        g_string_free(result_str, TRUE);
        gtk_stack_set_visible_child_name(GTK_STACK(stack), "doctor_schedule_view");
    }
    gtk_widget_destroy(dialog);
}

static void on_generate_schedule_clicked(GtkButton *button, gpointer user_data) {
    if (!head) { show_error_dialog("Tidak ada dokter untuk membuat jadwal."); return; }
    generate_schedule();
    populate_doctor_table();
    populate_schedule_table_range(0, NUM_DAYS);
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "schedule_view");
    show_info_dialog("Jadwal baru telah berhasil dibuat!");
}

static void on_view_conflicts_clicked(GtkButton *button, gpointer user_data) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(conflict_text_view));
    if (chead == NULL) {
        gtk_text_buffer_set_text(buffer, "\nTidak ada konflik jadwal yang ditemukan.\n", -1);
    } else {
        GString *conflict_str = g_string_new("--- Konflik Jadwal yang Tidak Terselesaikan ---\n\n");
        const char *shift_names[] = {"Pagi", "Siang", "Malam"};
        for (Conflict *temp = chead; temp != NULL; temp = temp->next) {
            g_string_append_printf(conflict_str, "Konflik: Dokter %s (ID: %d) terjadwal di Hari ke-%d, Shift %s padahal tidak bersedia.\n",
                                   temp->name, temp->id, temp->hari, shift_names[temp->shift -1]);
        }
        g_string_append(conflict_str, "\n---------------------------------------------\n");
        gtk_text_buffer_set_text(buffer, conflict_str->str, -1);
        g_string_free(conflict_str, TRUE);
    }
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "conflicts_view");
}

static void on_save_data_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new("Simpan Data ke Folder", GTK_WINDOW(main_window), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, "_Batal", GTK_RESPONSE_CANCEL, "_Simpan", GTK_RESPONSE_ACCEPT, NULL);
    
    // PERBAIKAN: Memeriksa GTK_RESPONSE_ACCEPT, bukan GTK_RESPONSE_OK
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *folder_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        char doc_path[512], sched_path[512];
        snprintf(doc_path, sizeof(doc_path), "%s/doctors.csv", folder_path);
        snprintf(sched_path, sizeof(sched_path), "%s/schedule.csv", folder_path);
        
        if (save_doctors_to_csv(doc_path) && save_schedule_to_csv(sched_path)) {
            show_info_dialog("Data dokter dan jadwal berhasil disimpan.");
        } else { 
            show_error_dialog("Gagal menyimpan data."); 
        }
        g_free(folder_path);
    }
    
    gtk_widget_destroy(dialog);
}

void on_schedule_option_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
    GtkWidget **widgets = (GtkWidget**)user_data;
    gtk_widget_set_sensitive(widgets[2], gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets[0])) || gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets[1])));
    gtk_widget_set_sensitive(widgets[3], gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets[1])));
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    initialize_schedule();
    gtk_init(&argc, &argv);
    g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", TRUE, NULL);
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), "Sistem Penjadwalan Dokter (Final v3)");
    gtk_window_set_default_size(GTK_WINDOW(main_window), 1200, 750);
    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 15);
    gtk_container_add(GTK_CONTAINER(main_window), grid);
    GtkWidget *action_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_grid_attach(GTK_GRID(grid), action_box, 0, 0, 1, 1);
    
    GtkWidget *btn_display_docs = gtk_button_new_with_label("Tampilkan Dokter");
    GtkWidget *btn_overall_sched = gtk_button_new_with_label("Jadwal Keseluruhan");
    GtkWidget *btn_doctor_sched = gtk_button_new_with_label("Jadwal Dokter");
    GtkWidget *btn_add = gtk_button_new_with_label("Tambah Dokter");
    GtkWidget *btn_edit = gtk_button_new_with_label("Edit Dokter");
    GtkWidget *btn_remove = gtk_button_new_with_label("Hapus Dokter");
    GtkWidget *btn_generate = gtk_button_new_with_label("Buat Jadwal Otomatis");
    GtkWidget *btn_conflict = gtk_button_new_with_label("Tampilkan Konflik");
    GtkWidget *btn_save = gtk_button_new_with_label("Simpan Data");
    GtkWidget *btn_load = gtk_button_new_with_label("Muat Data");
    
    gtk_box_pack_start(GTK_BOX(action_box), btn_display_docs, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(action_box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(action_box), btn_overall_sched, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(action_box), btn_doctor_sched, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(action_box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(action_box), btn_add, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(action_box), btn_edit, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(action_box), btn_remove, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(action_box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(action_box), btn_generate, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(action_box), btn_conflict, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(action_box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(action_box), btn_save, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(action_box), btn_load, FALSE, FALSE, 0);
    
    stack = gtk_stack_new();
    gtk_widget_set_hexpand(stack, TRUE);
    gtk_widget_set_vexpand(stack, TRUE);
    gtk_grid_attach(GTK_GRID(grid), stack, 1, 0, 1, 1);
    
    GtkWidget *welcome_label = gtk_label_new("Selamat Datang!\n\nPilih aksi dari menu di sebelah kiri.");
    gtk_stack_add_named(GTK_STACK(stack), welcome_label, "welcome_view");
    gtk_stack_add_named(GTK_STACK(stack), create_doctor_view(), "doctors_view");
    gtk_stack_add_named(GTK_STACK(stack), create_schedule_view(), "schedule_view");
    gtk_stack_add_named(GTK_STACK(stack), create_text_display_view(&conflict_text_view), "conflicts_view");
    gtk_stack_add_named(GTK_STACK(stack), create_text_display_view(&doctor_schedule_text_view), "doctor_schedule_view");

    g_signal_connect(btn_display_docs, "clicked", G_CALLBACK(on_display_doctors_clicked), NULL);
    g_signal_connect(btn_overall_sched, "clicked", G_CALLBACK(on_display_overall_schedule_clicked), NULL);
    g_signal_connect(btn_doctor_sched, "clicked", G_CALLBACK(on_display_doctor_schedule_clicked), NULL);
    g_signal_connect(btn_add, "clicked", G_CALLBACK(on_add_doctor_clicked), NULL);
    g_signal_connect(btn_edit, "clicked", G_CALLBACK(on_edit_doctor_clicked), NULL);
    g_signal_connect(btn_remove, "clicked", G_CALLBACK(on_remove_doctor_clicked), NULL);
    g_signal_connect(btn_generate, "clicked", G_CALLBACK(on_generate_schedule_clicked), NULL);
    g_signal_connect(btn_conflict, "clicked", G_CALLBACK(on_view_conflicts_clicked), NULL);
    g_signal_connect(btn_save, "clicked", G_CALLBACK(on_save_data_clicked), NULL);
    g_signal_connect(btn_load, "clicked", G_CALLBACK(on_load_data_clicked), NULL);

    gtk_widget_show_all(main_window);
    gtk_main();

    freeDoctorList();
    free_conflict_list();
    printf("Aplikasi ditutup, memori telah dibebaskan.\n");
    return 0;
}

GtkWidget* create_doctor_view() {
    GtkListStore *store = gtk_list_store_new(NUM_DOCTOR_COLS, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING);
    doctor_tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(doctor_tree_view), GTK_TREE_VIEW_GRID_LINES_BOTH);
    const char *titles[] = {"ID", "Nama", "Maks/Mgg", "Shift/Mgg", "Jadwal Tidak Tersedia"};
    for (int i = 0; i < NUM_DOCTOR_COLS; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(titles[i], renderer, "text", i, NULL);
        gtk_tree_view_column_set_sort_column_id(column, i);
        gtk_tree_view_append_column(GTK_TREE_VIEW(doctor_tree_view), column);
    }
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), doctor_tree_view);
    return scrolled_window;
}

GtkWidget* create_schedule_view() {
    GtkListStore *store = gtk_list_store_new(NUM_SCHEDULE_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    schedule_tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(schedule_tree_view), GTK_TREE_VIEW_GRID_LINES_BOTH);
    const char *titles[] = {"Hari", "Shift Pagi", "Shift Siang", "Shift Malam"};
    for (int i = 0; i < NUM_SCHEDULE_COLS; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(titles[i], renderer, "text", i, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(schedule_tree_view), column);
    }
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), schedule_tree_view);
    return scrolled_window;
}

GtkWidget* create_text_display_view(GtkWidget **text_view_widget) {
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    *text_view_widget = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(*text_view_widget), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(*text_view_widget), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(*text_view_widget), 10);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(*text_view_widget), 10);
    gtk_container_add(GTK_CONTAINER(scrolled_window), *text_view_widget);
    return scrolled_window;
}

void populate_doctor_table() {
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(doctor_tree_view)));
    gtk_list_store_clear(store);
    if (!head) return;
    const char *day_names[] = {"Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu", "Minggu"};
    const char *shift_names_short[] = {"Pagi", "Siang", "Malam"};
    GtkTreeIter iter;
    for (Doctor *current = head; current != NULL; current = current->next) {
        GString *unavailability_str = g_string_new("");
        bool first_unav = TRUE;
        for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
            for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
                if (current->preference[d][s] == 0) {
                    if (!first_unav) g_string_append(unavailability_str, ", ");
                    g_string_append_printf(unavailability_str, "%s %s", day_names[d], shift_names_short[s]);
                    first_unav = FALSE;
                }
            }
        }
        GString *shifts_per_week_str = g_string_new("");
        for (int w = 0; w < NUM_WEEKS; w++) {
            g_string_append_printf(shifts_per_week_str, "%d", current->shifts_scheduled_per_week[w]);
            if (w < NUM_WEEKS - 1) g_string_append(shifts_per_week_str, " / ");
        }
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                           COL_ID, current->id,
                           COL_NAME, current->name,
                           COL_MAX_SHIFTS, current->max_shifts_per_week,
                           COL_SHIFTS_PER_WEEK, shifts_per_week_str->str,
                           COL_UNAVAILABILITY, (unavailability_str->len > 0 ? unavailability_str->str : "Selalu Tersedia"),
                           -1);
        g_string_free(unavailability_str, TRUE);
        g_string_free(shifts_per_week_str, TRUE);
    }
}

void populate_schedule_table_range(int start_day, int num_days_to_show) {
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(schedule_tree_view)));
    gtk_list_store_clear(store);
    int schedule_ids[NUM_DAYS][NUM_SHIFTS_PER_DAY];
    get_all_doctors_schedule_month(schedule_ids);
    const char *day_names[] = {"Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu", "Minggu"};
    GtkTreeIter iter;
    int end_day = start_day + num_days_to_show;
    if (end_day > NUM_DAYS) end_day = NUM_DAYS;
    for (int day = start_day; day < end_day; day++) {
        char day_info[50];
        sprintf(day_info, "Minggu %d - %s (Hari ke-%d)", (day / 7) + 1, day_names[day % 7], day + 1);
        const char* names[NUM_SHIFTS_PER_DAY];
        for(int shift=0; shift < NUM_SHIFTS_PER_DAY; shift++) {
            Doctor *doc = findDoctorById(schedule_ids[day][shift]);
            names[shift] = doc ? doc->name : "Kosong";
        }
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, COL_DAY_INFO, day_info, COL_SHIFT_MORNING, names[0],
                           COL_SHIFT_AFTERNOON, names[1], COL_SHIFT_NIGHT, names[2], -1);
    }
}

void show_info_dialog(const gchar *message) {
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(main_window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void show_error_dialog(const gchar *message) {
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(main_window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}