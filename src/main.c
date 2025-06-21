
/*
 * File: main.c
 * Deskripsi: File utama program yang berisi logika untuk User Interface (UI)
 * menggunakan library GTK. Menangani semua interaksi pengguna
 * seperti klik tombol, input data, dan penampilan tabel.
 */

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "csv_utils.h"
#include "doctor_list.h"
#include "scheduler.h"

// Enum untuk mempermudah pengelolaan kolom di tabel (GtkTreeView)
// Ini lebih aman dan mudah dibaca daripada menggunakan angka langsung (0, 1, 2,
// ...).

// Kolom untuk tabel daftar dokter
enum {
    COL_ID = 0,
    COL_NAME,
    COL_MAX_SHIFTS,
    COL_SHIFTS_PER_WEEK,
    COL_TOTAL_SHIFTS,
    COL_UNAVAILABILITY,
    NUM_DOCTOR_COLS // Jumlah total kolom
};

// Kolom untuk tabel jadwal keseluruhan
enum {
    COL_DAY_INFO = 0,
    COL_SHIFT_MORNING,
    COL_SHIFT_AFTERNOON,
    COL_SHIFT_NIGHT,
    NUM_SCHEDULE_COLS
};

// Kolom untuk tabel daftar konflik (DIPERBARUI)
enum {
    COL_CONFLICT_ID = 0,
    COL_CONFLICT_NAME,
    COL_CONFLICT_DETAILS, // Menggabungkan Hari dan Shift
    COL_CONFLICT_REASON,
    NUM_CONFLICT_COLS // Jumlah total kolom sekarang 4
};

// ----- DEKLARASI WIDGET GLOBAL -----
// Widget yang perlu diakses dari berbagai fungsi dideklarasikan di sini.
GtkWidget *main_window, *stack;
GtkWidget *doctor_tree_view, *schedule_tree_view, *conflict_tree_view;
GtkWidget *doctor_schedule_tree_view, *doctor_schedule_info_label;

// ----- PROTOTYPE FUNGSI (DEKLARASI DI AWAL) -----
// Ini memberitahu compiler bahwa fungsi-fungsi ini ada,
// meskipun definisinya ada di bagian bawah file.

// Fungsi untuk membuat elemen-elemen UI
void create_main_window();
GtkWidget *create_doctor_view();
GtkWidget *create_schedule_view();
GtkWidget *create_doctor_schedule_view();
GtkWidget *create_conflict_view();

// Fungsi untuk mengisi data ke dalam tabel UI
void populate_doctor_table();
void populate_schedule_table_range(int start_day, int num_days);
void populate_doctor_schedule_table(int doc_id, int display_mode, int week_num,
                                    int day_num);
void populate_conflict_table();

// Fungsi "Callback" atau "Event Handler"
// Fungsi-fungsi ini akan dijalankan sebagai respon dari aksi user (misal: klik
// tombol)
static void on_display_doctors_clicked(GtkButton *button, gpointer user_data);
static void on_add_doctor_clicked(GtkButton *button, gpointer user_data);
static void on_edit_doctor_clicked(GtkButton *button, gpointer user_data);
static void on_remove_doctor_clicked(GtkButton *button, gpointer user_data);
static void on_display_overall_schedule_clicked(GtkButton *button,
                                                gpointer user_data);
static void on_display_doctor_schedule_clicked(GtkButton *button,
                                               gpointer user_data);
static void on_generate_schedule_clicked(GtkButton *button, gpointer user_data);
static void on_view_conflicts_clicked(GtkButton *button, gpointer user_data);
static void on_save_data_clicked(GtkButton *button, gpointer user_data);
static void on_load_data_clicked(GtkButton *button, gpointer user_data);
void on_schedule_option_toggled(GtkToggleButton *togglebutton,
                                gpointer user_data);
void on_doctor_schedule_option_toggled(GtkToggleButton *togglebutton,
                                       gpointer user_data);

// Fungsi utilitas
void show_info_dialog(const gchar *message);
void show_error_dialog(const gchar *message);
void recalculate_all_doctor_shift_counts();

// ----- FUNGSI UTAMA -----
int main(int argc, char *argv[]) {
    // Inisialisasi awal
    srand(time(NULL));      // Inisialisasi generator angka acak
    initialize_schedule();  // Pastikan jadwal kosong saat program mulai
    gtk_init(&argc, &argv); // Inisialisasi library GTK

    // Atur tema gelap (opsional, biar keren)
    g_object_set(gtk_settings_get_default(),
                 "gtk-application-prefer-dark-theme", TRUE, NULL);

    // Buat window utama dan semua isinya
    create_main_window();

    // Tampilkan window dan semua widget di dalamnya
    gtk_widget_show_all(main_window);
    // Jalankan loop utama GTK, program akan menunggu event dari user di sini
    gtk_main();

    // Bagian ini akan dijalankan setelah window ditutup
    freeDoctorList();
    free_conflict_list();
    printf("Aplikasi ditutup, memori telah dibebaskan.\n");
    return 0;
}

// ----- IMPLEMENTASI FUNGSI PEMBUATAN UI -----

// Membuat window utama aplikasi
void create_main_window() {
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), "Sistem Penjadwalan Dokter");
    gtk_window_set_default_size(GTK_WINDOW(main_window), 1200, 750);
    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 15);
    gtk_container_add(GTK_CONTAINER(main_window), grid);

    // Box untuk menampung tombol-tombol aksi di sebelah kiri
    GtkWidget *action_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_grid_attach(GTK_GRID(grid), action_box, 0, 0, 1, 1);

    // Buat semua tombol
    GtkWidget *btn_display_docs = gtk_button_new_with_label("Tampilkan Dokter");
    GtkWidget *btn_overall_sched =
        gtk_button_new_with_label("Jadwal Keseluruhan");
    GtkWidget *btn_doctor_sched = gtk_button_new_with_label("Jadwal Dokter");
    GtkWidget *btn_add = gtk_button_new_with_label("Tambah Dokter");
    GtkWidget *btn_edit = gtk_button_new_with_label("Edit Dokter");
    GtkWidget *btn_remove = gtk_button_new_with_label("Hapus Dokter");
    GtkWidget *btn_generate = gtk_button_new_with_label("Buat Jadwal Otomatis");
    GtkWidget *btn_conflict = gtk_button_new_with_label("Tampilkan Konflik");
    GtkWidget *btn_save = gtk_button_new_with_label("Simpan Data");
    GtkWidget *btn_load = gtk_button_new_with_label("Muat Data");

    // Masukkan tombol-tombol ke dalam action_box
    gtk_box_pack_start(GTK_BOX(action_box), btn_display_docs, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(action_box),
                       gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE,
                       FALSE, 10);
    gtk_box_pack_start(GTK_BOX(action_box), btn_overall_sched, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(action_box), btn_doctor_sched, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(action_box),
                       gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE,
                       FALSE, 10);
    gtk_box_pack_start(GTK_BOX(action_box), btn_add, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(action_box), btn_edit, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(action_box), btn_remove, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(action_box),
                       gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE,
                       FALSE, 10);
    gtk_box_pack_start(GTK_BOX(action_box), btn_generate, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(action_box), btn_conflict, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(action_box),
                       gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE,
                       FALSE, 10);
    gtk_box_pack_start(GTK_BOX(action_box), btn_save, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(action_box), btn_load, FALSE, FALSE, 0);

    // GtkStack adalah widget yang bisa menumpuk beberapa halaman (view) dan
    // hanya menampilkan satu per satu.
    stack = gtk_stack_new();
    gtk_widget_set_hexpand(stack, TRUE);
    gtk_widget_set_vexpand(stack, TRUE);
    gtk_grid_attach(GTK_GRID(grid), stack, 1, 0, 1, 1);

    // Buat halaman-halaman untuk dimasukkan ke stack
    GtkWidget *welcome_label = gtk_label_new(
        "Selamat Datang!\n\nPilih aksi dari menu di sebelah kiri.");
    gtk_stack_add_named(GTK_STACK(stack), welcome_label, "welcome_view");
    gtk_stack_add_named(GTK_STACK(stack), create_doctor_view(), "doctors_view");
    gtk_stack_add_named(GTK_STACK(stack), create_schedule_view(),
                        "schedule_view");
    gtk_stack_add_named(GTK_STACK(stack), create_conflict_view(),
                        "conflicts_view");
    gtk_stack_add_named(GTK_STACK(stack), create_doctor_schedule_view(),
                        "doctor_schedule_view");

    // Hubungkan sinyal "clicked" dari setiap tombol ke fungsi callback-nya
    // masing-masing
    g_signal_connect(btn_display_docs, "clicked",
                     G_CALLBACK(on_display_doctors_clicked), NULL);
    g_signal_connect(btn_overall_sched, "clicked",
                     G_CALLBACK(on_display_overall_schedule_clicked), NULL);
    g_signal_connect(btn_doctor_sched, "clicked",
                     G_CALLBACK(on_display_doctor_schedule_clicked), NULL);
    g_signal_connect(btn_add, "clicked", G_CALLBACK(on_add_doctor_clicked),
                     NULL);
    g_signal_connect(btn_edit, "clicked", G_CALLBACK(on_edit_doctor_clicked),
                     NULL);
    g_signal_connect(btn_remove, "clicked",
                     G_CALLBACK(on_remove_doctor_clicked), NULL);
    g_signal_connect(btn_generate, "clicked",
                     G_CALLBACK(on_generate_schedule_clicked), NULL);
    g_signal_connect(btn_conflict, "clicked",
                     G_CALLBACK(on_view_conflicts_clicked), NULL);
    g_signal_connect(btn_save, "clicked", G_CALLBACK(on_save_data_clicked),
                     NULL);
    g_signal_connect(btn_load, "clicked", G_CALLBACK(on_load_data_clicked),
                     NULL);
}

// Membuat view (tabel) untuk menampilkan daftar dokter
GtkWidget *create_doctor_view() {
    GtkListStore *store = gtk_list_store_new(
        NUM_DOCTOR_COLS, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING,
        G_TYPE_INT, G_TYPE_STRING);
    doctor_tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(doctor_tree_view),
                                 GTK_TREE_VIEW_GRID_LINES_BOTH);

    const char *titles[] = {"ID",          "Nama",
                            "Maks/Mgg",    "Shift/Mgg",
                            "Total Shift", "Jadwal Tidak Tersedia"};
    for (int i = 0; i < NUM_DOCTOR_COLS; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
            titles[i], renderer, "text", i, NULL);
        gtk_tree_view_column_set_sort_column_id(column, i);
        gtk_tree_view_append_column(GTK_TREE_VIEW(doctor_tree_view), column);
    }
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), doctor_tree_view);
    return scrolled_window;
}

// Membuat view (tabel) untuk menampilkan jadwal keseluruhan
GtkWidget *create_schedule_view() {
    GtkListStore *store =
        gtk_list_store_new(NUM_SCHEDULE_COLS, G_TYPE_STRING, G_TYPE_STRING,
                           G_TYPE_STRING, G_TYPE_STRING);
    schedule_tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(schedule_tree_view),
                                 GTK_TREE_VIEW_GRID_LINES_BOTH);

    const char *titles[] = {"Hari", "Shift Pagi", "Shift Siang", "Shift Malam"};
    for (int i = 0; i < NUM_SCHEDULE_COLS; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
            titles[i], renderer, "text", i, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(schedule_tree_view), column);
    }
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), schedule_tree_view);
    return scrolled_window;
}

// Membuat view untuk menampilkan jadwal spesifik per dokter
GtkWidget *create_doctor_schedule_view() {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(box), 10);

    doctor_schedule_info_label =
        gtk_label_new("Pilih dokter dan jangkauan jadwal untuk ditampilkan.");
    gtk_widget_set_halign(doctor_schedule_info_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(box), doctor_schedule_info_label, FALSE, FALSE,
                       5);

    GtkListStore *store =
        gtk_list_store_new(NUM_SCHEDULE_COLS, G_TYPE_STRING, G_TYPE_STRING,
                           G_TYPE_STRING, G_TYPE_STRING);
    doctor_schedule_tree_view =
        gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(doctor_schedule_tree_view),
                                 GTK_TREE_VIEW_GRID_LINES_BOTH);

    const char *titles[] = {"Hari", "Pagi", "Siang", "Malam"};
    for (int i = 0; i < NUM_SCHEDULE_COLS; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
            titles[i], renderer, "text", i, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(doctor_schedule_tree_view),
                                    column);
    }

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    gtk_container_add(GTK_CONTAINER(scrolled_window),
                      doctor_schedule_tree_view);
    gtk_box_pack_start(GTK_BOX(box), scrolled_window, TRUE, TRUE, 0);

    return box;
}

// Membuat view (tabel) untuk menampilkan konflik jadwal
GtkWidget *create_conflict_view() {
    // Model data (store) disesuaikan dengan jumlah kolom baru (4)
    GtkListStore *store =
        gtk_list_store_new(NUM_CONFLICT_COLS,
                           G_TYPE_INT,    // COL_CONFLICT_ID
                           G_TYPE_STRING, // COL_CONFLICT_NAME
                           G_TYPE_STRING, // COL_CONFLICT_DETAILS
                           G_TYPE_STRING  // COL_CONFLICT_REASON
        );
    conflict_tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(conflict_tree_view),
                                 GTK_TREE_VIEW_GRID_LINES_BOTH);

    // Judul kolom disesuaikan
    const char *titles[] = {"ID Dokter", "Nama Dokter", "Detail Konflik",
                            "Keterangan"};

    for (int i = 0; i < NUM_CONFLICT_COLS; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
            titles[i], renderer, "text", i, NULL);
        gtk_tree_view_column_set_sort_column_id(column, i);
        gtk_tree_view_append_column(GTK_TREE_VIEW(conflict_tree_view), column);
    }
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), conflict_tree_view);
    return scrolled_window;
}

// ----- IMPLEMENTASI FUNGSI PENGISIAN DATA (POPULATE) -----

// Mengisi tabel dokter dengan data dari linked list
void populate_doctor_table() {
    GtkListStore *store = GTK_LIST_STORE(
        gtk_tree_view_get_model(GTK_TREE_VIEW(doctor_tree_view)));
    gtk_list_store_clear(store);
    if (!head)
        return;

    const char *day_names[] = {"Senin", "Selasa", "Rabu",  "Kamis",
                               "Jumat", "Sabtu",  "Minggu"};
    const char *shift_names_short[] = {"Pagi", "Siang", "Malam"};
    GtkTreeIter iter;

    for (Doctor *current = head; current != NULL; current = current->next) {
        GString *unavailability_str = g_string_new("");
        bool first_unav = TRUE;
        for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
            for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
                if (current->preference[d][s] == 0) {
                    if (!first_unav)
                        g_string_append(unavailability_str, ", ");
                    g_string_append_printf(unavailability_str, "%s %s",
                                           day_names[d], shift_names_short[s]);
                    first_unav = FALSE;
                }
            }
        }

        GString *shifts_per_week_str = g_string_new("");
        for (int w = 0; w < NUM_WEEKS; w++) {
            g_string_append_printf(shifts_per_week_str, "%d",
                                   totalShiftWeek(w + 1, current->id));
            if (w < NUM_WEEKS - 1)
                g_string_append(shifts_per_week_str, " / ");
        }

        int total_shifts = totalShift(current->id);
        char full_name[MAX_NAME_LENGTH + 4];
        sprintf(full_name, "dr. %s", current->name);

        gtk_list_store_append(store, &iter);
        gtk_list_store_set(
            store, &iter, COL_ID, current->id, COL_NAME, full_name,
            COL_MAX_SHIFTS, current->max_shifts_per_week, COL_SHIFTS_PER_WEEK,
            shifts_per_week_str->str, COL_TOTAL_SHIFTS, total_shifts,
            COL_UNAVAILABILITY,
            (unavailability_str->len > 0 ? unavailability_str->str
                                         : "Selalu Tersedia"),
            -1);

        g_string_free(unavailability_str, TRUE);
        g_string_free(shifts_per_week_str, TRUE);
    }
}

// Mengisi tabel jadwal keseluruhan untuk rentang hari
void populate_schedule_table_range(int start_day, int num_days_to_show) {
    GtkListStore *store = GTK_LIST_STORE(
        gtk_tree_view_get_model(GTK_TREE_VIEW(schedule_tree_view)));
    gtk_list_store_clear(store);

    int schedule_ids[NUM_DAYS][NUM_SHIFTS_PER_DAY];
    get_all_doctors_schedule_month(schedule_ids);

    const char *day_names[] = {"Senin", "Selasa", "Rabu",  "Kamis",
                               "Jumat", "Sabtu",  "Minggu"};
    GtkTreeIter iter;
    int end_day = start_day + num_days_to_show;
    if (end_day > NUM_DAYS)
        end_day = NUM_DAYS;

    int current_week = -1;

    for (int day = start_day; day < end_day; day++) {
        int week_of_day = day / NUM_DAYS_PER_WEEK;
        if (week_of_day != current_week) {
            current_week = week_of_day;
            char week_header[50];
            sprintf(week_header, "--- Minggu %d ---", current_week + 1);
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter, COL_DAY_INFO, week_header,
                               COL_SHIFT_MORNING, "", COL_SHIFT_AFTERNOON, "",
                               COL_SHIFT_NIGHT, "", -1);
        }

        char name_morning[MAX_NAME_LENGTH + 4],
            name_afternoon[MAX_NAME_LENGTH + 4],
            name_night[MAX_NAME_LENGTH + 4];

        Doctor *doc_m = findDoctorById(schedule_ids[day][0]);
        if (doc_m)
            sprintf(name_morning, "dr. %s", doc_m->name);
        else
            strcpy(name_morning, "Kosong");

        Doctor *doc_a = findDoctorById(schedule_ids[day][1]);
        if (doc_a)
            sprintf(name_afternoon, "dr. %s", doc_a->name);
        else
            strcpy(name_afternoon, "Kosong");

        Doctor *doc_n = findDoctorById(schedule_ids[day][2]);
        if (doc_n)
            sprintf(name_night, "dr. %s", doc_n->name);
        else
            strcpy(name_night, "Kosong");

        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, COL_DAY_INFO, day_names[day % 7],
                           COL_SHIFT_MORNING, name_morning, COL_SHIFT_AFTERNOON,
                           name_afternoon, COL_SHIFT_NIGHT, name_night, -1);
    }
}

// Mengisi tabel jadwal untuk dokter spesifik
void populate_doctor_schedule_table(int doc_id, int display_mode, int week_num,
                                    int day_num) {
    Doctor *doc = findDoctorById(doc_id);
    if (!doc)
        return;

    GtkListStore *store = GTK_LIST_STORE(
        gtk_tree_view_get_model(GTK_TREE_VIEW(doctor_schedule_tree_view)));
    gtk_list_store_clear(store);

    const char *day_names[] = {"Senin", "Selasa", "Rabu",  "Kamis",
                               "Jumat", "Sabtu",  "Minggu"};
    GtkTreeIter iter;
    int total_shifts = 0;
    char title_text[150], total_text[50], final_markup[256];
    int day_idx = day_num - 1; // Konversi dari 1-based ke 0-based

    // Mode 0: Bulanan, 1: Mingguan, 2: Harian
    if (display_mode == 0) { // Tampilan Bulanan
        sprintf(title_text, "Jadwal 30 Hari untuk dr. %s", doc->name);
        int monthly_shifts_data[NUM_WEEKS][NUM_DAYS_PER_WEEK]
                               [NUM_SHIFTS_PER_DAY];
        int monthly_count[NUM_WEEKS][NUM_DAYS_PER_WEEK];
        get_doctor_schedule_month(doc_id, monthly_shifts_data, monthly_count);

        for (int w = 0; w < NUM_WEEKS; w++) {
            char week_header[50];
            sprintf(week_header, "--- Minggu %d ---", w + 1);
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter, COL_DAY_INFO, week_header,
                               COL_SHIFT_MORNING, "", COL_SHIFT_AFTERNOON, "",
                               COL_SHIFT_NIGHT, "", -1);

            for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
                bool on_morning = false, on_afternoon = false, on_night = false;
                for (int s = 0; s < monthly_count[w][d]; s++) {
                    if (monthly_shifts_data[w][d][s] == 0)
                        on_morning = true;
                    if (monthly_shifts_data[w][d][s] == 1)
                        on_afternoon = true;
                    if (monthly_shifts_data[w][d][s] == 2)
                        on_night = true;
                }
                gtk_list_store_append(store, &iter);
                gtk_list_store_set(
                    store, &iter, COL_DAY_INFO, day_names[d], COL_SHIFT_MORNING,
                    on_morning ? "Jaga" : "---", COL_SHIFT_AFTERNOON,
                    on_afternoon ? "Jaga" : "---", COL_SHIFT_NIGHT,
                    on_night ? "Jaga" : "---", -1);
            }
        }
        total_shifts = totalShift(doc_id);
        sprintf(total_text, "Total Shift Keseluruhan: %d", total_shifts);

    } else if (display_mode == 1) { // Tampilan Mingguan
        sprintf(title_text, "Jadwal Minggu %d untuk dr. %s", week_num,
                doc->name);
        int weekly_shifts_data[NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY];
        int weekly_count[NUM_DAYS_PER_WEEK];
        getDoctorScheduleWeek(week_num, doc_id, weekly_shifts_data,
                              weekly_count);

        for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
            bool on_morning = false, on_afternoon = false, on_night = false;
            for (int s = 0; s < weekly_count[d]; s++) {
                if (weekly_shifts_data[d][s] == 0)
                    on_morning = true;
                if (weekly_shifts_data[d][s] == 1)
                    on_afternoon = true;
                if (weekly_shifts_data[d][s] == 2)
                    on_night = true;
            }
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter, COL_DAY_INFO, day_names[d],
                               COL_SHIFT_MORNING, on_morning ? "Jaga" : "---",
                               COL_SHIFT_AFTERNOON,
                               on_afternoon ? "Jaga" : "---", COL_SHIFT_NIGHT,
                               on_night ? "Jaga" : "---", -1);
        }
        total_shifts = totalShiftWeek(week_num, doc_id);
        sprintf(total_text, "Total Shift Minggu Ini: %d", total_shifts);

    } else { // Tampilan Harian
        if (day_idx < 0 || day_idx >= NUM_DAYS_PER_WEEK)
            return;
        sprintf(title_text, "Jadwal Hari %s, Minggu %d untuk dr. %s",
                day_names[day_idx], week_num, doc->name);

        int daily_shifts[NUM_SHIFTS_PER_DAY];
        int shift_count_for_day;
        getDoctorScheduleDay(week_num, day_idx, doc_id, daily_shifts,
                             &shift_count_for_day);

        bool on_morning = false, on_afternoon = false, on_night = false;
        for (int s = 0; s < shift_count_for_day; s++) {
            if (daily_shifts[s] == 0)
                on_morning = true;
            if (daily_shifts[s] == 1)
                on_afternoon = true;
            if (daily_shifts[s] == 2)
                on_night = true;
        }

        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, COL_DAY_INFO, day_names[day_idx],
                           COL_SHIFT_MORNING, on_morning ? "Jaga" : "---",
                           COL_SHIFT_AFTERNOON, on_afternoon ? "Jaga" : "---",
                           COL_SHIFT_NIGHT, on_night ? "Jaga" : "---", -1);

        total_shifts = totalShiftDay(week_num, day_idx, doc_id);
        sprintf(total_text, "Total Shift Hari Ini: %d", total_shifts);
    }

    sprintf(final_markup, "<b>%s</b>\n%s", title_text, total_text);
    gtk_label_set_markup(GTK_LABEL(doctor_schedule_info_label), final_markup);
}

// Isi tabel view konflik
void populate_conflict_table() {
    GtkListStore *store = GTK_LIST_STORE(
        gtk_tree_view_get_model(GTK_TREE_VIEW(conflict_tree_view)));
    gtk_list_store_clear(store);

    if (chead == NULL)
        return;

    GtkTreeIter iter;
    const char *shift_names[] = {"Pagi", "Siang", "Malam"};
    for (Conflict *temp = chead; temp != NULL; temp = temp->next) {
        if (temp->shift >= 0 && temp->shift < NUM_SHIFTS_PER_DAY) {
            char reason[256];
            char full_name[MAX_NAME_LENGTH + 4];
            char conflict_details[100];

            sprintf(conflict_details, "Hari ke-%d, Shift %s", temp->hari + 1,
                    shift_names[temp->shift]);

            sprintf(reason, "Terjadwal pada hari/shift yang tidak tersedia");
            sprintf(full_name, "dr. %s", temp->name);

            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter, COL_CONFLICT_ID, temp->id,
                               COL_CONFLICT_NAME, full_name,
                               COL_CONFLICT_DETAILS, conflict_details,
                               COL_CONFLICT_REASON, reason, -1);
        }
    }
}

// ----- IMPLEMENTASI EVENT HANDLER / CALLBACK -----

// Callback untuk toggle button pada dialog jadwal dokter spesifik
void on_doctor_schedule_option_toggled(GtkToggleButton *togglebutton,
                                       gpointer user_data) {
    GtkWidget **widgets = (GtkWidget **)user_data;
    gboolean week_active =
        gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets[0]));
    gboolean day_active =
        gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets[1]));
    gtk_widget_set_sensitive(widgets[2], week_active || day_active);
    gtk_widget_set_sensitive(widgets[3], day_active);
}

// Callback untuk toggle button pada dialog jadwal keseluruhan
void on_schedule_option_toggled(GtkToggleButton *togglebutton,
                                gpointer user_data) {
    GtkWidget **widgets = (GtkWidget **)user_data;
    gboolean week_active =
        gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets[0]));
    gboolean day_active =
        gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets[1]));
    gtk_widget_set_sensitive(widgets[2], week_active || day_active);
    gtk_widget_set_sensitive(widgets[3], day_active);
}

// Callback saat tombol "Tampilkan Dokter" diklik
static void on_display_doctors_clicked(GtkButton *button, gpointer user_data) {
    populate_doctor_table();
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "doctors_view");
}

// Callback saat tombol "Tambah Dokter" diklik
static void on_add_doctor_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog, *content_area, *grid, *pref_grid;
    GtkWidget *name_label, *name_entry, *shifts_label, *shifts_spin_button,
        *pref_frame;
    GtkCheckButton *pref_checkboxes[NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY];
    const char *day_labels[] = {"Senin", "Selasa", "Rabu",  "Kamis",
                                "Jumat", "Sabtu",  "Minggu"};
    const char *shift_labels[] = {"Pagi", "Siang", "Malam"};

    dialog = gtk_dialog_new_with_buttons(
        "Tambah Dokter Baru", GTK_WINDOW(main_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "_Batal",
        GTK_RESPONSE_CANCEL, "_Tambah", GTK_RESPONSE_ACCEPT, NULL);
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

    pref_frame =
        gtk_frame_new("Preferensi Ketersediaan (Centang jika BERSEDIA)");
    gtk_grid_attach(GTK_GRID(grid), pref_frame, 0, 2, 2, 1);
    pref_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(pref_grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(pref_grid), 15);
    gtk_container_set_border_width(GTK_CONTAINER(pref_grid), 10);
    gtk_container_add(GTK_CONTAINER(pref_frame), pref_grid);

    for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
        gtk_grid_attach(GTK_GRID(pref_grid), gtk_label_new(shift_labels[s]),
                        s + 1, 0, 1, 1);
    }
    for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
        gtk_grid_attach(GTK_GRID(pref_grid), gtk_label_new(day_labels[d]), 0,
                        d + 1, 1, 1);
        for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
            pref_checkboxes[d][s] = GTK_CHECK_BUTTON(gtk_check_button_new());
            gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(pref_checkboxes[d][s]), TRUE);
            gtk_grid_attach(GTK_GRID(pref_grid),
                            GTK_WIDGET(pref_checkboxes[d][s]), s + 1, d + 1, 1,
                            1);
        }
    }

    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        const char *name = gtk_entry_get_text(GTK_ENTRY(name_entry));
        int max_shifts = gtk_spin_button_get_value_as_int(
            GTK_SPIN_BUTTON(shifts_spin_button));
        if (strlen(name) > 0 && max_shifts > 0) {
            addDoctor(name, max_shifts);
            Doctor *new_doc = findDoctorByName(name);
            if (new_doc) {
                for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
                    for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
                        setDoctorPreference(
                            new_doc->id, d, s,
                            gtk_toggle_button_get_active(
                                GTK_TOGGLE_BUTTON(pref_checkboxes[d][s])));
                    }
                }
            }
            populate_doctor_table();
            gtk_stack_set_visible_child_name(GTK_STACK(stack), "doctors_view");
            char info_msg[100];
            sprintf(info_msg, "dr. %s berhasil ditambahkan!", name);
            show_info_dialog(info_msg);
        } else {
            show_error_dialog("Input tidak valid. Nama tidak boleh kosong dan "
                              "maks shift > 0.");
        }
    }
    gtk_widget_destroy(dialog);
}

// Callback saat tombol "Edit Dokter" diklik
static void on_edit_doctor_clicked(GtkButton *button, gpointer user_data) {
    if (!head) {
        show_error_dialog("Tidak ada dokter untuk diedit.");
        return;
    }
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "Pilih Dokter untuk Diedit", GTK_WINDOW(main_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "_Batal",
        GTK_RESPONSE_CANCEL, "_Pilih", GTK_RESPONSE_OK, NULL);
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *combo = gtk_combo_box_text_new();
    for (Doctor *doc = head; doc != NULL; doc = doc->next) {
        char label[MAX_NAME_LENGTH + 15];
        sprintf(label, "%d: dr. %s", doc->id, doc->name);
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo),
                                  g_strdup_printf("%d", doc->id), label);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
    gtk_container_add(GTK_CONTAINER(content_area), combo);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) != GTK_RESPONSE_OK) {
        gtk_widget_destroy(dialog);
        return;
    }

    const char *doc_id_str = gtk_combo_box_get_active_id(GTK_COMBO_BOX(combo));
    if (!doc_id_str) {
        gtk_widget_destroy(dialog);
        return;
    }
    int doc_id = atoi(doc_id_str);
    Doctor *doc_to_edit = findDoctorById(doc_id);
    gtk_widget_destroy(dialog);

    GtkWidget *edit_dialog, *grid, *pref_grid, *pref_frame;
    GtkCheckButton *pref_checkboxes[NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY];
    const char *day_labels[] = {"Senin", "Selasa", "Rabu",  "Kamis",
                                "Jumat", "Sabtu",  "Minggu"};
    const char *shift_labels[] = {"Pagi", "Siang", "Malam"};

    char dialog_title[100];
    sprintf(dialog_title, "Edit Dokter: dr. %s (ID: %d)", doc_to_edit->name,
            doc_to_edit->id);
    edit_dialog = gtk_dialog_new_with_buttons(
        dialog_title, GTK_WINDOW(main_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "_Batal",
        GTK_RESPONSE_CANCEL, "_Simpan", GTK_RESPONSE_ACCEPT, NULL);
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
    GtkAdjustment *adj = gtk_adjustment_new(doc_to_edit->max_shifts_per_week,
                                            1.0, 21.0, 1.0, 5.0, 0.0);
    GtkWidget *shifts_spin_button = gtk_spin_button_new(adj, 1.0, 0);

    gtk_grid_attach(GTK_GRID(grid), name_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), name_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), shifts_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), shifts_spin_button, 1, 1, 1, 1);

    pref_frame =
        gtk_frame_new("Preferensi Ketersediaan (Centang jika BERSEDIA)");
    gtk_grid_attach(GTK_GRID(grid), pref_frame, 0, 2, 2, 1);
    pref_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(pref_grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(pref_grid), 15);
    gtk_container_set_border_width(GTK_CONTAINER(pref_grid), 10);
    gtk_container_add(GTK_CONTAINER(pref_frame), pref_grid);

    for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
        gtk_grid_attach(GTK_GRID(pref_grid), gtk_label_new(shift_labels[s]),
                        s + 1, 0, 1, 1);
    }
    for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
        gtk_grid_attach(GTK_GRID(pref_grid), gtk_label_new(day_labels[d]), 0,
                        d + 1, 1, 1);
        for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
            pref_checkboxes[d][s] = GTK_CHECK_BUTTON(gtk_check_button_new());
            gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(pref_checkboxes[d][s]),
                doc_to_edit->preference[d][s]);
            gtk_grid_attach(GTK_GRID(pref_grid),
                            GTK_WIDGET(pref_checkboxes[d][s]), s + 1, d + 1, 1,
                            1);
        }
    }

    gtk_widget_show_all(edit_dialog);

    if (gtk_dialog_run(GTK_DIALOG(edit_dialog)) == GTK_RESPONSE_ACCEPT) {
        const char *new_name = gtk_entry_get_text(GTK_ENTRY(name_entry));
        int new_max_shifts = gtk_spin_button_get_value_as_int(
            GTK_SPIN_BUTTON(shifts_spin_button));
        updateDoctor(doc_id, new_name, new_max_shifts);
        for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
            for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
                setDoctorPreference(
                    doc_id, d, s,
                    gtk_toggle_button_get_active(
                        GTK_TOGGLE_BUTTON(pref_checkboxes[d][s])));
            }
        }
        populate_doctor_table();
        show_info_dialog("Data dokter berhasil diperbarui.");
    }
    gtk_widget_destroy(edit_dialog);
}

// Callback saat tombol "Hapus Dokter" diklik
static void on_remove_doctor_clicked(GtkButton *button, gpointer user_data) {
    if (!head) {
        show_error_dialog("Tidak ada dokter untuk dihapus.");
        return;
    }
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "Pilih Dokter untuk Dihapus", GTK_WINDOW(main_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "_Batal",
        GTK_RESPONSE_CANCEL, "_Pilih", GTK_RESPONSE_OK, NULL);
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *combo = gtk_combo_box_text_new();
    for (Doctor *doc = head; doc != NULL; doc = doc->next) {
        char label[MAX_NAME_LENGTH + 15];
        sprintf(label, "%d: dr. %s", doc->id, doc->name);
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo),
                                  g_strdup_printf("%d", doc->id), label);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
    gtk_container_add(GTK_CONTAINER(content_area), combo);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) != GTK_RESPONSE_OK) {
        gtk_widget_destroy(dialog);
        return;
    }
    const char *doc_id_str = gtk_combo_box_get_active_id(GTK_COMBO_BOX(combo));
    if (!doc_id_str) {
        gtk_widget_destroy(dialog);
        return;
    }
    int doc_id = atoi(doc_id_str);
    Doctor *doc_to_remove = findDoctorById(doc_id);
    gtk_widget_destroy(dialog);

    char confirmation_text[200];
    sprintf(confirmation_text,
            "Apakah Anda yakin ingin menghapus dr. %s (ID: %d)?",
            doc_to_remove->name, doc_id);
    GtkWidget *confirm_dialog = gtk_message_dialog_new(
        GTK_WINDOW(main_window), GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "%s", confirmation_text);
    if (gtk_dialog_run(GTK_DIALOG(confirm_dialog)) == GTK_RESPONSE_YES) {
        removeDoctor(doc_id);
        populate_doctor_table();
        show_info_dialog("Dokter telah dihapus.\nID telah diperbarui, harap "
                         "membuat jadwal otomatis baru!");
    }
    gtk_widget_destroy(confirm_dialog);
}

// Callback saat tombol "Jadwal Keseluruhan" diklik
static void on_display_overall_schedule_clicked(GtkButton *button,
                                                gpointer user_data) {
    GtkWidget *dialog, *content_area, *box, *radio_month, *radio_week,
        *radio_day, *week_spin, *day_spin;
    dialog = gtk_dialog_new_with_buttons(
        "Pilih Jangkauan Jadwal Keseluruhan", GTK_WINDOW(main_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "_Batal",
        GTK_RESPONSE_CANCEL, "_Tampilkan", GTK_RESPONSE_OK, NULL);
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(box), 15);
    gtk_container_add(GTK_CONTAINER(content_area), box);

    radio_month =
        gtk_radio_button_new_with_label(NULL, "Tampilkan Jadwal 30 Hari");
    radio_week = gtk_radio_button_new_with_label_from_widget(
        GTK_RADIO_BUTTON(radio_month), "Tampilkan Per Minggu");
    radio_day = gtk_radio_button_new_with_label_from_widget(
        GTK_RADIO_BUTTON(radio_month), "Tampilkan Per Hari");

    GtkAdjustment *adj_week = gtk_adjustment_new(1, 1, NUM_WEEKS, 1, 0, 0);
    week_spin = gtk_spin_button_new(adj_week, 1, 0);
    GtkAdjustment *adj_day =
        gtk_adjustment_new(1, 1, NUM_DAYS_PER_WEEK, 1, 0, 0);
    day_spin = gtk_spin_button_new(adj_day, 1, 0);

    gtk_box_pack_start(GTK_BOX(box), radio_month, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), radio_week, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), radio_day, FALSE, FALSE, 0);

    GtkWidget *spin_grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(spin_grid), 10);
    gtk_grid_attach(GTK_GRID(spin_grid), gtk_label_new("Minggu:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(spin_grid), week_spin, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(spin_grid), gtk_label_new("Hari (1=Sen):"), 2, 0,
                    1, 1);
    gtk_grid_attach(GTK_GRID(spin_grid), day_spin, 3, 0, 1, 1);
    gtk_box_pack_start(GTK_BOX(box), spin_grid, FALSE, FALSE, 0);

    GtkWidget *widgets_for_overall[] = {radio_week, radio_day, week_spin,
                                        day_spin};
    g_signal_connect(radio_month, "toggled",
                     G_CALLBACK(on_schedule_option_toggled),
                     widgets_for_overall);
    g_signal_connect(radio_week, "toggled",
                     G_CALLBACK(on_schedule_option_toggled),
                     widgets_for_overall);
    g_signal_connect(radio_day, "toggled",
                     G_CALLBACK(on_schedule_option_toggled),
                     widgets_for_overall);
    on_schedule_option_toggled(NULL, widgets_for_overall);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_month))) {
            populate_schedule_table_range(0, NUM_DAYS);
        } else if (gtk_toggle_button_get_active(
                       GTK_TOGGLE_BUTTON(radio_week))) {
            populate_schedule_table_range(
                (gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(week_spin)) -
                 1) *
                    NUM_DAYS_PER_WEEK,
                NUM_DAYS_PER_WEEK);
        } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_day))) {
            int week_val =
                gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(week_spin));
            int day_val =
                gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(day_spin));
            populate_schedule_table_range(
                (week_val - 1) * NUM_DAYS_PER_WEEK + (day_val - 1), 1);
        }
        gtk_stack_set_visible_child_name(GTK_STACK(stack), "schedule_view");
    }
    gtk_widget_destroy(dialog);
}

// Callback saat tombol "Jadwal Dokter" diklik
static void on_display_doctor_schedule_clicked(GtkButton *button,
                                               gpointer user_data) {
    if (!head) {
        show_error_dialog("Tidak ada dokter dalam daftar.");
        return;
    }

    GtkWidget *dialog, *content_area, *box, *doctor_combo, *radio_doc_month,
        *radio_doc_week, *radio_doc_day, *doc_week_spin, *doc_day_spin;
    dialog = gtk_dialog_new_with_buttons(
        "Pilih Jangkauan Jadwal Dokter Spesifik", GTK_WINDOW(main_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "_Batal",
        GTK_RESPONSE_CANCEL, "_Tampilkan", GTK_RESPONSE_OK, NULL);
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(box), 15);
    gtk_container_add(GTK_CONTAINER(content_area), box);

    doctor_combo = gtk_combo_box_text_new();
    for (Doctor *doc = head; doc != NULL; doc = doc->next) {
        char label[MAX_NAME_LENGTH + 15];
        sprintf(label, "%d: dr. %s", doc->id, doc->name);
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(doctor_combo),
                                  g_strdup_printf("%d", doc->id), label);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(doctor_combo), 0);

    radio_doc_month =
        gtk_radio_button_new_with_label(NULL, "Tampilkan Jadwal 30 Hari");
    radio_doc_week = gtk_radio_button_new_with_label_from_widget(
        GTK_RADIO_BUTTON(radio_doc_month), "Tampilkan Per Minggu");
    radio_doc_day = gtk_radio_button_new_with_label_from_widget(
        GTK_RADIO_BUTTON(radio_doc_month), "Tampilkan Per Hari");

    GtkAdjustment *adj_doc_week = gtk_adjustment_new(1, 1, NUM_WEEKS, 1, 0, 0);
    doc_week_spin = gtk_spin_button_new(adj_doc_week, 1, 0);
    GtkAdjustment *adj_doc_day =
        gtk_adjustment_new(1, 1, NUM_DAYS_PER_WEEK, 1, 0, 0);
    doc_day_spin = gtk_spin_button_new(adj_doc_day, 1, 0);

    gtk_box_pack_start(GTK_BOX(box), gtk_label_new("Pilih Dokter:"), FALSE,
                       FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), doctor_combo, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), radio_doc_month, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), radio_doc_week, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), radio_doc_day, FALSE, FALSE, 0);

    GtkWidget *spin_grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(spin_grid), 10);
    gtk_grid_attach(GTK_GRID(spin_grid), gtk_label_new("Minggu:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(spin_grid), doc_week_spin, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(spin_grid), gtk_label_new("Hari (1=Sen):"), 2, 0,
                    1, 1);
    gtk_grid_attach(GTK_GRID(spin_grid), doc_day_spin, 3, 0, 1, 1);
    gtk_box_pack_start(GTK_BOX(box), spin_grid, FALSE, FALSE, 0);

    GtkWidget *widgets_for_doctor[] = {radio_doc_week, radio_doc_day,
                                       doc_week_spin, doc_day_spin};
    g_signal_connect(radio_doc_month, "toggled",
                     G_CALLBACK(on_doctor_schedule_option_toggled),
                     widgets_for_doctor);
    g_signal_connect(radio_doc_week, "toggled",
                     G_CALLBACK(on_doctor_schedule_option_toggled),
                     widgets_for_doctor);
    g_signal_connect(radio_doc_day, "toggled",
                     G_CALLBACK(on_doctor_schedule_option_toggled),
                     widgets_for_doctor);
    on_doctor_schedule_option_toggled(NULL, widgets_for_doctor);

    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        const char *doc_id_str =
            gtk_combo_box_get_active_id(GTK_COMBO_BOX(doctor_combo));
        if (!doc_id_str) {
            show_error_dialog("Silakan pilih dokter.");
            gtk_widget_destroy(dialog);
            return;
        }
        int doc_id = atoi(doc_id_str);

        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_doc_month))) {
            populate_doctor_schedule_table(doc_id, 0, 0, 0);
        } else if (gtk_toggle_button_get_active(
                       GTK_TOGGLE_BUTTON(radio_doc_week))) {
            int week = gtk_spin_button_get_value_as_int(
                GTK_SPIN_BUTTON(doc_week_spin));
            populate_doctor_schedule_table(doc_id, 1, week, 0);
        } else {
            int week = gtk_spin_button_get_value_as_int(
                GTK_SPIN_BUTTON(doc_week_spin));
            int day =
                gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(doc_day_spin));
            populate_doctor_schedule_table(doc_id, 2, week, day);
        }

        gtk_stack_set_visible_child_name(GTK_STACK(stack),
                                         "doctor_schedule_view");
    }
    gtk_widget_destroy(dialog);
}

// Callback saat tombol "Buat Jadwal" diklik
static void on_generate_schedule_clicked(GtkButton *button,
                                         gpointer user_data) {
    if (!head) {
        show_error_dialog("Tidak ada dokter untuk membuat jadwal.");
        return;
    }
    generate_schedule();
    populate_doctor_table();
    populate_schedule_table_range(0, NUM_DAYS);
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "schedule_view");
    show_info_dialog("Jadwal baru telah berhasil dibuat!");
}

// Callback saat tombol "Tampilkan Konflik" diklik
static void on_view_conflicts_clicked(GtkButton *button, gpointer user_data) {
    populate_conflict_table();
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "conflicts_view");
}

// Callback saat tombol "Simpan Data" diklik
static void on_save_data_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new(
        "Simpan Data ke Folder", GTK_WINDOW(main_window),
        GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, "_Batal", GTK_RESPONSE_CANCEL,
        "_Simpan", GTK_RESPONSE_ACCEPT, NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *folder_path =
            gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        char doc_path[512], sched_path[512];
        snprintf(doc_path, sizeof(doc_path), "%s/doctors.csv", folder_path);
        snprintf(sched_path, sizeof(sched_path), "%s/schedule.csv",
                 folder_path);

        if (save_doctors_to_csv(doc_path) && save_schedule_to_csv(sched_path)) {
            show_info_dialog("Data dokter dan jadwal berhasil disimpan.");
        } else {
            show_error_dialog("Gagal menyimpan data.");
        }
        g_free(folder_path);
    }

    gtk_widget_destroy(dialog);
}

// Callback saat tombol "Muat Data" diklik
static void on_load_data_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new(
        "Pilih Folder Data", GTK_WINDOW(main_window),
        GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, "_Batal", GTK_RESPONSE_CANCEL,
        "_Buka", GTK_RESPONSE_ACCEPT, NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *folder_path =
            gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        char doc_path[512], sched_path[512];
        snprintf(doc_path, sizeof(doc_path), "%s/doctors.csv", folder_path);
        snprintf(sched_path, sizeof(sched_path), "%s/schedule.csv",
                 folder_path);

        bool doc_loaded = load_doctors_from_csv(doc_path);
        bool sched_loaded = load_schedule_from_csv(sched_path);

        if (doc_loaded) {
            if (sched_loaded) {
                recalculate_all_doctor_shift_counts();
            }
            populate_doctor_table();
            populate_schedule_table_range(0, NUM_DAYS);
            gtk_stack_set_visible_child_name(GTK_STACK(stack), "doctors_view");
            show_info_dialog(
                "Data dokter dan jadwal berhasil dimuat dan disinkronkan.");
        } else {
            show_error_dialog("Gagal memuat data dokter. Pastikan "
                              "'doctors.csv' ada di folder yang dipilih.");
        }
        g_free(folder_path);
    }

    gtk_widget_destroy(dialog);
}

// ----- UTILITIES -----

void recalculate_all_doctor_shift_counts() {
    if (!head)
        return;
    for (Doctor *doc = head; doc != NULL; doc = doc->next) {
        for (int i = 0; i < NUM_WEEKS; i++) {
            doc->shifts_scheduled_per_week[i] = 0;
        }
    }
    for (int day = 0; day < NUM_DAYS; day++) {
        for (int shift = 0; shift < NUM_SHIFTS_PER_DAY; shift++) {
            int doc_id = schedule[day][shift];
            if (doc_id > 0) {
                Doctor *doc = findDoctorById(doc_id);
                if (doc) {
                    int week_index = day / NUM_DAYS_PER_WEEK;
                    doc->shifts_scheduled_per_week[week_index]++;
                }
            }
        }
    }
}

// Popup dialog informasi
void show_info_dialog(const gchar *message) {
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(main_window), GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Popup dialog error
void show_error_dialog(const gchar *message) {
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(main_window), GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}
