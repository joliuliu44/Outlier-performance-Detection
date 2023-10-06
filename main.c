#include <stdio.h>
#include <string.h>
#include <json-c/json.h>
#include <math.h>

float calcMean(float arr[], int size) {
    float sum = 0.0;
    for (int i=0; i<size; i++) {
        sum += arr[i];
    }
    return sum / size;
}

float calcStd(float arr[], int size, float mean) {
    float sum_squared_diff = 0.0;
    for (int i=0; i<size; i++) {
        float diff = arr[i] - mean;
        sum_squared_diff += diff * diff;
    }
    return sqrt(sum_squared_diff/size);
}

int main(int argc, char **argv) {
    char *input_string = argv[1];
    int num_games = 10;
    char *user_games[num_games];
    float kdr_list[num_games];

    for (int i = 0; i < num_games; i++) {
        int user_length = snprintf(NULL, 0, "%s_%d.json", input_string, i + 1) + 1;
        user_games[i] = (char *)malloc(user_length);
        snprintf(user_games[i], user_length, "%s_%d.json", input_string, i+1);
        //printf("Iteration %d: %s\n", i + 1, user_games[i]);

    }

    for (int i=0; i<num_games;i++) {
        FILE *fp;
        char buffer[1024];

        struct json_object *parsed_json;
        struct json_object *user_id;
        struct json_object *kills;
        struct json_object *assists;
        struct json_object *deaths;
        struct json_object *kill_death_ratio;

        fp = fopen(user_games[i], "r");
        if (fp == NULL) {
            perror("Error opening file.");
            continue;
        }
        
        fread(buffer,1024,1,fp);
        fclose(fp);

        parsed_json = json_tokener_parse(buffer);

        json_object_object_get_ex(parsed_json, "user_id", &user_id);
        json_object_object_get_ex(parsed_json, "kill_death_ratio", &kill_death_ratio);

        kdr_list[i] = json_object_get_double(kill_death_ratio);

        // printf("Iteration %d: %s\n", i + 1, user_games[i]);
        // printf("ID: %s\n", json_object_get_string(user_id));
        // printf("KDR: %.2f\n", json_object_get_double(kill_death_ratio));
        // printf("kdr_list: %f\n", kdr_list[i]);
    }

    // calculate z score
    int sampleSize = sizeof(kdr_list) / sizeof(kdr_list[0]);
    
    FILE *fp;
    char buffer[1024];

    struct json_object *parsed_json;
    struct json_object *user_id;
    struct json_object *kills;
    struct json_object *assists;
    struct json_object *deaths;
    struct json_object *kill_death_ratio;

    fp = fopen("joliu44_over.json", "r");
    if (fp == NULL) {
        perror("Error opening file.");
    }
        
    fread(buffer,1024,1,fp);
    fclose(fp);

    parsed_json = json_tokener_parse(buffer);

    json_object_object_get_ex(parsed_json, "user_id", &user_id);
    json_object_object_get_ex(parsed_json, "kill_death_ratio", &kill_death_ratio);

    float obs_value = json_object_get_double(kill_death_ratio);
    float mean = calcMean(kdr_list, sampleSize);
    float stdev = calcStd(kdr_list, sampleSize, mean);
    float zScore = (obs_value - mean) / stdev;

    if (zScore >= 3) {
        printf("%s's most recent KDR (%.2f) is %.2f standard deviations from the mean.\n", json_object_get_string(user_id),obs_value,zScore);
        printf("High likelihood of cheating.\n");
    }
    else {
        printf("%s's Most recent KDR (%.2f) is %.2f standard deviations from the mean.\n", json_object_get_string(user_id),obs_value,zScore);
        printf("Within tolerance range.\n");
    }
    
}