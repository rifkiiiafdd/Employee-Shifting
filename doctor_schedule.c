#include "doctor_schedule.h"

int getDoctorScheduleDay(int week, int day, int doctorId, int outShifts[]) {
    int hari_ke = (week - 1) * JUMLAH_HARI_PER_MINGGU + (day - 1);
    int count = 0;
    for (int shift = 0; shift < JUMLAH_SHIFT; shift++) {
        if (jadwal[hari_ke][shift] == doctorId) {
            outShifts[count++] = shift;
        }
    }
    return count;
}

void getDoctorScheduleWeek(int week, int doctorId, int outShifts[JUMLAH_HARI_PER_MINGGU][JUMLAH_SHIFT], int outCount[JUMLAH_HARI_PER_MINGGU]) {
    for (int day = 0; day < JUMLAH_HARI_PER_MINGGU; day++) {
        int hari_ke = (week - 1) * JUMLAH_HARI_PER_MINGGU + day;
        int count = 0;
        for (int shift = 0; shift < JUMLAH_SHIFT; shift++) {
            if (jadwal[hari_ke][shift] == doctorId) {
                outShifts[day][count++] = shift;
            }
        }
        outCount[day] = count;
    }
}

void getDoctorSchedule(int doctorId, int outShifts[JUMLAH_MINGGU][JUMLAH_HARI_PER_MINGGU][JUMLAH_SHIFT], int outCount[JUMLAH_MINGGU][JUMLAH_HARI_PER_MINGGU]) {
    for (int week = 0; week < JUMLAH_MINGGU; week++) {
        for (int day = 0; day < JUMLAH_HARI_PER_MINGGU; day++) {
            int hari_ke = week * JUMLAH_HARI_PER_MINGGU + day;
            int count = 0;
            for (int shift = 0; shift < JUMLAH_SHIFT; shift++) {
                if (jadwal[hari_ke][shift] == doctorId) {
                    outShifts[week][day][count++] = shift;
                }
            }
            outCount[week][day] = count;
        }
    }
}