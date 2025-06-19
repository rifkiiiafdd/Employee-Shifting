#ifndef DOCTOR_SCHEDULE_H
#define DOCTOR_SCHEDULE_H

#define JUMLAH_HARI 30
#define JUMLAH_SHIFT 3
#define JUMLAH_MINGGU 5
#define JUMLAH_HARI_PER_MINGGU 7

// Deklarasi array jadwal (harus diinisialisasi di program utama atau file lain)
extern int jadwal[JUMLAH_HARI][JUMLAH_SHIFT];

// Fungsi-fungsi jadwal dokter
int getDoctorScheduleDay(int week, int day, int doctorId, int outShifts[]);
void getDoctorScheduleWeek(int week, int doctorId, int outShifts[JUMLAH_HARI_PER_MINGGU][JUMLAH_SHIFT], int outCount[JUMLAH_HARI_PER_MINGGU]);
void getDoctorSchedule(int doctorId, int outShifts[JUMLAH_MINGGU][JUMLAH_HARI_PER_MINGGU][JUMLAH_SHIFT], int outCount[JUMLAH_MINGGU][JUMLAH_HARI_PER_MINGGU]);

#endif