f (temp->id == random_doctor) {
                if (temp->max_shift_per_week < temp->shift_scheduled_per_week[i / 7]) {
                    
                    if (temp->preference[i%7][j] == 1 && preference_focus == 1) {
                        schedule[i][j] = temp->id;
                        temp->shift_scheduled_per_week[i / 7] += 1;
                        break;
                    }
                    else if (preference_focus == 0) {
                        schedule[i][j] = temp->id;
                        temp->shift_scheduled_per_week[i / 7] += 1;
                        break;
                    }
                }
                random_doctor += 1;

                if (random_doctor >= number_doctor) {
                    random_doctor = 1;
                }
            }